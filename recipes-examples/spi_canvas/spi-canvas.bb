SUMMARY = "SPI device demo application"
SECTION = "examples"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${WORKDIR}/COPYRIGHT;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI  = "file://main.c"
SRC_URI += "file://canvascmd.c"
SRC_URI += "file://canvascmd.h"
SRC_URI += "file://fbscreen.c"
SRC_URI += "file://fbscreen.h"
SRC_URI += "file://spidevice.c"
SRC_URI += "file://spidevice.h"
SRC_URI += "file://config.h"
SRC_URI += "file://canvas_common.h"
SRC_URI += "file://readme.txt"
SRC_URI += "file://COPYRIGHT"

S = "${WORKDIR}"

inherit autotools gettext

appdir = "home/root/examples/drawd"

FILES_${PN} = "/${appdir}/*"
FILES_${PN}-dbg = "/${appdir}/.debug/*"

do_compile() {
	${CC} -Wall -lm \
		${S}/main.c \
		${S}/canvascmd.c \
		${S}/fbscreen.c \
		${S}/spidevice.c \
		-o ${B}/spi_canvasd
}

do_install() {
	install -d ${D}/${appdir}
	install -m 0755 ${B}/spi_canvasd ${D}/${appdir}
	install -m 0666 ${S}/main.c ${D}/${appdir}
	install -m 0666 ${S}/canvascmd.c ${D}/${appdir}
	install -m 0666 ${S}/canvascmd.h ${D}/${appdir}
	install -m 0666 ${S}/fbscreen.c ${D}/${appdir}
	install -m 0666 ${S}/fbscreen.h ${D}/${appdir}
	install -m 0666 ${S}/spidevice.c ${D}/${appdir}
	install -m 0666 ${S}/spidevice.h  ${D}/${appdir}
	install -m 0666 ${S}/config.h ${D}/${appdir}
	install -m 0666 ${S}/canvas_common.h ${D}/${appdir}
	install -m 0666 ${S}/readme.txt ${D}/${appdir}
}
