SUMMARY = "recipe file for dac-tcp-server.c"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://dac-tcp-server"

S = "${WORKDIR}/dac-tcp-server"
FILESEXTRAPATHS_prepend = "${THISDIR}/files:"

CFLAGS_prepend = "-I ${S}/include"
inherit autotools update-rc.d

INITSCRIPT_NAME = "dac-tcp-server-init"

do_install_append() {
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${S}/dac-tcp-server-init ${D}${sysconfdir}/init.d/dac-tcp-server-init
}
