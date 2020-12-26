# LiBoot
A Powerful, Easy to Use (U)EFI Bootloader

### for-inix branch

This branch tracks a customised version of LiBoot capable of booting [Inix](https://github.com/maheshkhanwalkar/Inix). The limited Linux
boot functionality has been removed completely -- this version can *only* boot Inix.

The eventual goal is to incorporate the Inix boot process into LiBoot proper, so it can be used for the development of the Inix kernel,
as well as for booting Linux distributions (main use-case)

### Status
LiBoot is currently still in the early stages of development and therefore **not** feature-complete. However, it is currently capable
of booting Linux under certain conditions. There will be ongoing work to make the project more robust and functional.

### Building

Building LiBoot is somewhat complicated, due to its underlying dependency on Tianocore EDK II, which has a particularly
unusual build system and various dependencies.

The following instructions have been tested on Ubuntu distros, so that should work. File an issue if something is missing
or builds are breaking. 

While it is possible to build EDK II on different platforms, we do not directly support it -- you are on your own!

#### Installing dependencies
    ./uefi/install_deps.sh

The provided shell script will install the required dependencies (using apt) and initialise all the git submodules for the edk2
package, which are required for building.

#### Building the EFI image
    ./uefi/build.sh

The provided shell script performs **magic** 🪄 to get the out-of-source EDK II build to actually work. The OVMF image (firmware) and LiBoot.efi
executable are then both built. The OVMF firmware is built for testing with QEMU.

The LiBoot executable is then copied to `build/LiBoot.efi` when the build completes successfully. 
