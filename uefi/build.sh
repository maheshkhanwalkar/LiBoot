#!/bin/bash

# Fail fast on error
set -e

dir=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

function switch_to_script_dir() {
  cd $dir
}

# Switch to the directory where this script resides
switch_to_script_dir

# Setup EDK II
source ../edk2/edksetup.sh BaseTools
cp ../conf/target.txt ../edk2/Conf/
cp ../conf/MdeModulePkg.dsc ../edk2/MdeModulePkg/
cd ../edk2/MdeModulePkg/Application && ln -sfnv ../../../uefi/src LiBoot

# Build OVMF
build -p OvmfPkg/OvmfPkgX64.dsc

# Build LiBoot
build

# Copy image to sane location
switch_to_script_dir
mkdir -p ../build
cp ../edk2/Build/MdeModule/DEBUG_GCC5/X64/LiBoot.efi ../build/LiBoot.efi

# Display output message
printf "\u001b[32m[SUCCESS]\e[0m LiBoot successfully built!\n"
