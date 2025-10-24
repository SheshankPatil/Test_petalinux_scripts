SUMMARY = "Recipe for  build an external exslerate Linux kernel module"
SECTION = "PETALINUX/modules"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

INHIBIT_PACKAGE_STRIP = "1"

SRC_URI = "file://Makefile \
           file://exslerate_drv.c \
           file://exslerate_drv.h \
           file://exslerate_gem.c \
           file://exslerate_gem.h \
           file://exslerate_ioctl.h \
           file://conv_engine.c \
           file://conv_engine.h \
	   file://COPYING \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
