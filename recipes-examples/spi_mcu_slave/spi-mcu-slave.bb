SUMMARY = "SPI device demo application"
SECTION = "examples"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${WORKDIR}/COPYRIGHT;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI  = "file://spi_mcu_slave.c"
SRC_URI += "file://readme.txt"
SRC_URI += "file://COPYRIGHT"

S = "${WORKDIR}"

inherit autotools gettext

appdir = "home/root/examples/spi_mcu_slave"

FILES_${PN} = "/${appdir}/*"
FILES_${PN}-dbg = "/${appdir}/.debug/*"

do_compile() {
	${CC} ${S}/spi_mcu_slave.c -o ${B}/spi_mcu_slave
}

do_install() {
	install -d ${D}/${appdir}
	install -m 0755 ${B}/spi_mcu_slave ${D}/${appdir}
	install -m 0666 ${S}/readme.txt ${D}/${appdir}
	install -m 0666 ${S}/spi_mcu_slave.c ${D}/${appdir}
}
