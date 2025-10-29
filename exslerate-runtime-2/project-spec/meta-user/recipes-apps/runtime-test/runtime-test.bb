SUMMARY = "Simple runtime-test application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://runtime-test.c \
           file://iree-run-module \
           file://iree-run-module \
           file://conv_only.vmfb \
           file://Makefile"

S = "${WORKDIR}"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 runtime-test ${D}${bindir}/
    install -m 0755 ${WORKDIR}/iree-run-module ${D}${bindir}/
    

    install -d ${D}${datadir}/runtime-test
    install -m 0644 ${WORKDIR}/conv_only.vmfb ${D}${datadir}/runtime-test/
}

FILES_${PN} += "${datadir}/runtime-test/*"

# Development: prevent Yocto from stripping debug symbols so gdb works in QEMU
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"