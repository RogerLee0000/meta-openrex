# meta-openrex
FEDEVEL Yocto Project for OpenRex

# How to compile & use software for OpenRex 

Here you can find basic info about how to start. Detailed documentation about how to compile and use meta-openrex can be found at OpenRex Software website: http://www.imx6rex.com/open-rex/software/

## Install the `repo` utility
    mkdir ~/bin
    curl http://commondatastorage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
    chmod a+x ~/bin/repo
    PATH=${PATH}:~/bin

## Get the YOCTO project
    cd
    mkdir fsl-community-bsp
    cd fsl-community-bsp
    git config --global user.email "your_email@example.com"
    git config --global user.name "yourname"
    repo init -u https://github.com/Freescale/fsl-community-bsp-platform -b jethro

## Add openrex support  
    cd ~/fsl-community-bsp/
    mkdir -pv .repo/local_manifests/
    cat > .repo/local_manifests/imx6openrex.xml << EOF
    <?xml version="1.0" encoding="UTF-8"?>
    <manifest>
    
      <remote fetch="git://github.com/FEDEVEL" name="fedevel"/>
    
      <project remote="fedevel" revision="jethro" name="meta-openrex" path="sources/meta-openrex">
        <copyfile src="fedevel-setup.sh" dest="fedevel-setup.sh"/>
      </project>
    </manifest>
    EOF

## Sync repositories
    repo sync

## Add OpenRex meta layer into BSP
    source openrex-setup.sh

# Building images
    cd ~/fsl-release-bsp

## Currently Supported machines <machine name>
    imx6q-openrex
    
## Setup and Build Console image
    MACHINE=<machine name> source setup-environment build-openrex
    MACHINE=<machine name> bitbake core-image-base

Example:
    MACHINE=imx6q-openrex source setup-environment build-openrex
    MACHINE=imx6q-openrex bitbake core-image-base

## Setup and Build Toolchain    
    MACHINE=<machine name> bitbake core-image-base -c populate_sdk
    
## Setup and Build FSL GUI image
    MACHINE=<machine name> source fsl-setup-release.sh -b build-x11 -e x11
    MACHINE=<machine name> bitbake fsl-image-gui
    
        
    
