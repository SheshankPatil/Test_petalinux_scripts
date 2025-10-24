# Makefile for building the exslerate kernel module

# Specify the module name and its object files
obj-m += exslerate.o
exslerate-objs := exslerate_drv.o exslerate_gem.o conv_engine.o

# Compiler flags for debugging
MY_CFLAGS += -g -DDEBUG
ccflags-y += $(MY_CFLAGS)

# Current directory
SRC := $(shell pwd)

# Kernel source/build directory (default to running kernel's build directory)
KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build

# Default target: build the kernel module
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules

# Install the kernel module
modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

# Clean build artifacts
clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean
	rm -f *~ core .depend .*.cmd *.mod *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions

# Additional development targets
load: all
	sudo insmod exslerate.ko

unload:
	sudo rmmod exslerate 2>/dev/null || true

reload: unload load

info:
	modinfo exslerate.ko 2>/dev/null || echo "Module not built yet"

status:
	lsmod | grep exslerate || echo "Module not loaded"

logs:
	dmesg | grep -i exslerate | tail -20

help:
	@echo "ExSLerate Driver Makefile"
	@echo "========================"
	@echo "Targets:"
	@echo "  all            - Build the kernel module (default)"
	@echo "  modules_install- Install the module"
	@echo "  clean          - Clean build artifacts"
	@echo "  load           - Load the module (requires root)"
	@echo "  unload         - Unload the module (requires root)"
	@echo "  reload         - Unload and load the module"
	@echo "  info           - Show module information"
	@echo "  status         - Show if module is loaded"
	@echo "  logs           - Show recent kernel logs"
	@echo "  help           - Show this help"

.PHONY: all modules_install clean load unload reload info status logs help




# obj-m += exslerate.o
# exslerate-objs := exslerate_drv.o exslerate_gem.o conv_engine.o
