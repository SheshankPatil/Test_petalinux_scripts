# PetaLinux Project Automation

This repository provides a script and templates to automate the creation and configuration of a PetaLinux project. It is designed to ensure a reproducible and consistent setup for developers.

## Overview

The `setup-petalinux-project.sh` script performs the following actions:

1.  **Creates a PetaLinux project** based on a specified template.
2.  **Imports a hardware description** (XSA) file.
3.  **Configures the root filesystem** to use `EXT4`.
4.  **Installs `gdb` and `gdbserver`** for debugging.
5.  **Applies kernel configurations** for debugging (`CONFIG_PRINTK`, `CONFIG_DEBUG`).
6.  **Adds a reserved memory region** to the device tree.
7.  **Creates skeleton recipes** for a kernel module (`exslerate`) and a user-space application (`runtime-test`).
8.  **Builds the PetaLinux project**.

## Prerequisites

- Xilinx PetaLinux Tools installed.
- A valid PetaLinux environment that can be sourced.

## Quickstart

1.  **Clone the repository**:
    ```bash
    git clone <repository-url>
    cd <repository-name>
    ```

2.  **Configure the project**:
    - Open the `project.conf` file.
    - Modify the variables to match your project requirements. Pay close attention to `PETALINUX_SETTINGS` and `XSA_DIR`.

    ```ini
    # The name of the PetaLinux project directory to be created.
    PROJECT_NAME=exslerate-runtime

    # Absolute path to the PetaLinux settings script (e.g., settings.sh).
    PETALINUX_SETTINGS=/opt/Xilinx/PetaLinux/2023.2/settings.sh

    # The hardware template to use (e.g., zynqMP, zynq, microblaze).
    TEMPLATE=zynqMP

    # Absolute path to the directory containing the hardware description (XSA) file.
    XSA_DIR=/path/to/your/xsa_directory

    # Base address and size for the reserved memory region in the device tree.
    RESERVED_BASE=0x30000000
    RESERVED_SIZE=0x40000000
    ```

3.  **Run the setup script**:
    ```bash
    ./scripts/setup-petalinux-project.sh
    ```

    The script will create the PetaLinux project in a new directory named after `PROJECT_NAME`.

    To overwrite an existing project, use the `--force` flag:
    ```bash
    ./scripts/setup-petalinux-project.sh --force
    ```

## Repository Structure

```
.
├── project.conf                # Main configuration file for the project.
├── README.md                   # This file.
├── scripts/
│   └── setup-petalinux-project.sh # The main automation script.
└── templates/
    ├── bsp.cfg.patch           # Kernel configuration fragment.
    └── system-user.dtsi        # Device tree overlay for reserved memory.
```

## Verification

After the script completes, you can find the generated PetaLinux project in the directory specified by `PROJECT_NAME`.

- **Root Filesystem**: Check `build/conf/local.conf` for `IMAGE_FSTYPES` to include `ext4`.
- **Kernel Config**: Verify that `project-spec/meta-user/recipes-kernel/linux/linux-xlnx/bsp.cfg` contains the debug flags.
- **Device Tree**: Check `project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi` for the `reserved-memory` node.