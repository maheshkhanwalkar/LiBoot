#!/bin/bash

# Fail fast on error
set -e

dir=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

function switch_to_dir() {
  cd $1
}

# Switch to the directory where this script resides
switch_to_dir $dir

# Setup EDK II
make -C ../edk2/BaseTools
switch_to_dir "../edk2/"
source edksetup.sh BaseTools

switch_to_dir $dir
cp ../conf/MdeModulePkg.dsc ../edk2/MdeModulePkg/
cd ../edk2/MdeModulePkg/Application && ln -sfnv ../../../uefi/src LiBoot

# Setup toolchain and architecture
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ARCH="X64"
    TOOLCHAIN="GCC5"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    ARCH="X64"
    TOOLCHAIN="XCODE5"
else
    printf "[ERROR] Unknown platform! Quitting...\n"
    exit 1
fi

# Build OVMF
echo "${ARCH}"
echo "$TOOLCHAIN"
build -a "$ARCH" -t "$TOOLCHAIN" -p OvmfPkg/OvmfPkgX64.dsc

# Build ShellPkg
build -a "$ARCH" -t "$TOOLCHAIN" -p ShellPkg/ShellPkg.dsc

# Build LiBoot
build -a "$ARCH" -t "$TOOLCHAIN"

# Copy image to sane location
switch_to_dir $dir
mkdir -p ../build
cp "../edk2/Build/MdeModule/DEBUG_${TOOLCHAIN}/${ARCH}/LiBoot.efi" ../build/LiBoot.efi

# Display output message
printf "[SUCCESS] LiBoot successfully built!\n"
