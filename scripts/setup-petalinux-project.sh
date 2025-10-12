#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

# =============================================================================
# setup-petalinux-project.sh
#
# Creates and configures a PetaLinux project from a hardware description (XSA)
# and a configuration file. This script automates the following steps:
#
#  1. Sources the PetaLinux environment.
#  2. Creates a new project from a template.
#  3. Imports the hardware description (XSA).
#  4. Configures the root filesystem to EXT4.
#  5. Adds specified user packages (e.g., gdb/gdbserver).
#  6. Applies kernel configuration changes from a template.
#  7. Creates a kernel module skeleton.
#  8. Creates a user-space application skeleton.
#  9. Applies a device tree overlay from a template.
# 10. Builds the PetaLinux project.
#
# Usage:
#   ./scripts/setup-petalinux-project.sh [--config project.conf] [--force]
#
# Options:
#   --config <file>  : Path to the configuration file (default: project.conf).
#   --force          : Remove the project directory if it already exists.
#   -h, --help       : Show this help message.
#
# The script expects a configuration file (e.g., project.conf) with KEY=VALUE
# pairs. See project.conf for details on required and optional variables.
# =============================================================================

# --- Script and Template Paths ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
TEMPLATE_DIR="${REPO_ROOT}/templates"

# --- Default Configuration (can be overridden by project.conf) ---
PROJECT_NAME="petalinux-project"
PETALINUX_SETTINGS="/opt/pkg/settings.sh"
TEMPLATE="zynqMP"
XSA_DIR=""
FORCE=false
RESERVED_BASE="0x30000000"
RESERVED_SIZE="0x40000000"

# --- Helper Functions ---
info() { echo "INFO: $*"; }
warn() { echo "WARN: $*" >&2; }
die() { echo "ERROR: $*" >&2; exit 1; }

# --- Core Functions ---

# Loads configuration from the specified file.
load_config() {
  local config_file="$1"
  if [[ ! -f "$config_file" ]]; then
    die "Configuration file not found: $config_file"
  fi
  info "Loading configuration from $config_file"
  # Simple KEY=VALUE loader, ignores comments and trims whitespace.
  while IFS='=' read -r key value;
  do
    [[ "$key" =~ ^\s*# ]] && continue
    key=$(echo "$key" | xargs)
    value=$(echo "$value" | xargs)
    [[ -z "$key" ]] && continue
    case "$key" in
      PROJECT_NAME) PROJECT_NAME="$value" ;;
      PETALINUX_SETTINGS) PETALINUX_SETTINGS="$value" ;;
      TEMPLATE) TEMPLATE="$value" ;;
      XSA_DIR) XSA_DIR="$value" ;;
      RESERVED_BASE) RESERVED_BASE="$value" ;;
      RESERVED_SIZE) RESERVED_SIZE="$value" ;;
      *) warn "Unknown configuration key: $key" ;;
    esac
  done < "$config_file"
}

# Sources the PetaLinux environment settings script.
source_petalinux_environment() {
  info "Sourcing PetaLinux environment: $PETALINUX_SETTINGS"
  if [[ ! -f "$PETALINUX_SETTINGS" ]]; then
    die "PetaLinux settings file not found at: $PETALINUX_SETTINGS"
  fi
  # shellcheck disable=SC1090
  source "$PETALINUX_SETTINGS"
}

# Creates a new PetaLinux project, removing an existing one if --force is used.
create_project() {
  if [[ -d "$PROJECT_NAME" && "$FORCE" != true ]]; then
    die "Project directory '$PROJECT_NAME' already exists. Use --force to overwrite."
  elif [[ -d "$PROJECT_NAME" && "$FORCE" == true ]]; then
    info "Removing existing project directory: $PROJECT_NAME"
    rm -rf "$PROJECT_NAME"
  fi

  info "Creating PetaLinux project '$PROJECT_NAME' with template '$TEMPLATE'"
  petalinux-create --type project --template "$TEMPLATE" --name "$PROJECT_NAME"
  cd "$PROJECT_NAME" || die "Failed to change directory to $PROJECT_NAME"
}

# Imports the hardware description from the specified XSA directory.
import_hardware() {
  info "Importing hardware description from: $XSA_DIR"
  if [[ ! -d "$XSA_DIR" ]]; then
    die "XSA directory not found at: $XSA_DIR"
  fi
  petalinux-config --get-hw-description "$XSA_DIR"
}

# Configures the root filesystem and adds user packages non-interactively.
configure_rootfs() {
  info "Configuring root filesystem..."
  local config_file="project-spec/configs/config"

  info "Setting root filesystem type to EXT4"
  # Ensure the line exists and is set to 'y' for EXT4, and remove the INITRAMFS line if present.
  if grep -q "^CONFIG_YOCTO_ROOTFS_TYPE_EXT4=y" "$config_file"; then
    info "EXT4 rootfs already configured."
  else
    # Remove existing EXT4 or INITRAMFS config if present to avoid conflicts
    sed -i '/^CONFIG_YOCTO_ROOTFS_TYPE_EXT4/d' "$config_file"
    sed -i '/^CONFIG_YOCTO_ROOTFS_TYPE_INITRAMFS/d' "$config_file"
    echo "CONFIG_YOCTO_ROOTFS_TYPE_EXT4=y" >> "$config_file"
    echo "CONFIG_YOCTO_ROOTFS_TYPE=\"ext4\"" >> "$config_file"
    info "Configured EXT4 rootfs."
  fi

  info "Enabling GDB package group"
  local rootfs_config_file="project-spec/configs/rootfs_config"
  if ! grep -q "^CONFIG_packagegroup-petalinux-gdb=y" "$rootfs_config_file" 2>/dev/null;
  then
    echo 'CONFIG_packagegroup-petalinux-gdb=y' >> "$rootfs_config_file"
    info "Added CONFIG_packagegroup-petalinux-gdb to $rootfs_config_file"
  else
    info "CONFIG_packagegroup-petalinux-gdb already present in $rootfs_config_file"
  fi
}

# Applies kernel configuration changes from the template file.
apply_kernel_config() {
  local bsp_cfg_patch="${TEMPLATE_DIR}/bsp.cfg.patch"
  local bsp_cfg_target="project-spec/meta-user/recipes-kernel/linux/linux-xlnx/bsp.cfg"

  if [[ ! -f "$bsp_cfg_patch" ]]; then
    warn "Kernel config patch not found at $bsp_cfg_patch. Skipping."
    return
  fi

  info "Applying kernel configuration from $bsp_cfg_patch"
  mkdir -p "$(dirname "$bsp_cfg_target")"
  # Append content of patch file to target, ensuring no duplicates if run multiple times
  # This simple append assumes the patch file is clean and doesn't contain duplicates itself.
  # For more robust handling, one might compare line by line or use diff/patch tools.
  cat "$bsp_cfg_patch" >> "$bsp_cfg_target"
}

# Creates a skeleton for a kernel module.
add_kernel_module() {
  info "Creating kernel module 'exslerate'"
  petalinux-create -t modules --name exslerate --enable
}

# Creates a skeleton for a user-space C application.
add_userspace_app() {
  info "Creating user-space application 'runtime-test'"
  petalinux-create -t apps --template c --name runtime-test --enable
}

# Applies the device tree overlay from the template.
apply_device_tree() {
  local dtsi_template="${TEMPLATE_DIR}/system-user.dtsi"
  local dtsi_target="project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi"

  if [[ ! -f "$dtsi_template" ]]; then
    warn "Device tree template not found at $dtsi_template. Skipping."
    return
  fi

  info "Applying device tree overlay to $dtsi_target"
  mkdir -p "$(dirname "$dtsi_target")"
  cp "$dtsi_template" "$dtsi_target"

  info "Replacing placeholders in device tree"
  sed -i "s/__RESERVED_BASE__/${RESERVED_BASE}/g" "$dtsi_target"
  sed -i "s/__RESERVED_SIZE__/${RESERVED_SIZE}/g" "$dtsi_target"
}

# Builds the entire PetaLinux project.
build_project() {
  info "Starting PetaLinux build. This may take a significant amount of time."
  petalinux-build
}

# --- Main Execution ---
main() {
  local config_file="${REPO_ROOT}/project.conf"

  # Parse command-line arguments.
  while [[ ${1:-} != "" ]]; do
    case $1 in
      --config) shift; config_file="$1"; shift ;;
      --force) FORCE=true; shift ;;
      -h|--help) 
        grep '^# ' "${BASH_SOURCE[0]}" | cut -c3-
        exit 0
        ;;
      *) die "Unknown argument: $1" ;;
    esac
  done

  load_config "$config_file"
  source_petalinux_environment
  create_project
  import_hardware
  configure_rootfs
  apply_kernel_config
  add_kernel_module
  add_userspace_app
  apply_device_tree
  build_project

  info "PetaLinux project setup and build complete in '$(pwd)'."
  info "Next steps: Package the build using 'petalinux-package' or deploy to target."
}

main "$@"