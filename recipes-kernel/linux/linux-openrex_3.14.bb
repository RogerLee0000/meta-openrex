# Copyright (C) 2016 FEDEVEL
# Released under the MIT license (see COPYING.MIT for the terms)
 
SUMMARY = "Linux Kernel for OpenRex board"
DESCRIPTION = "Linux Kernel for OpenRex board. More info \
at http://www.imx6rex.com/open-rex"
 
require recipes-kernel/linux/linux-imx.inc
require recipes-kernel/linux/linux-dtb.inc
 
DEPENDS += "lzop-native bc-native"
 
include linux-fslc.inc
 
#PV .= "+git${SRCPV}"
PV .= ""
 
SRCBRANCH = "jethro"
LOCALVERSION = "-yocto"
 
#Always update SRCREV based on your last commit
SRCREV = "9c8f1161f03bf41801993ebe477ea05e62d3e8e8"
 
KERNEL_SRC ?= "git://github.com/RogerLee0000/openrex-linux-3.14.git;protocol=git"
SRC_URI = "${KERNEL_SRC};branch=${SRCBRANCH} file://defconfig"
 
COMPATIBLE_MACHINE = "(mx6|mx7|imx6q-openrex)"
