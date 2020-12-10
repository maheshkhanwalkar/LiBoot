# LiBoot
A Powerful, Easy to Use (U)EFI Bootloader

### Status
LiBoot is currently still in the early stages of development and therefore **not** feature-complete. However, it is currently capable
of booting Linux under certain conditions. There will be ongoing work to make the project more robust and functional.

### Building

Building LiBoot is somewhat complicated, due to its underlying dependency on [TianoCore EDK II](https://github.com/tianocore/edk2/), which has a particularly
unusual build system and various dependencies.

#### Supported Build Platforms

* macOS
* Ubuntu

The above list only describes the platforms for which LiBoot has been successfully built. It does not indicate the ability for LiBoot to boot those
operating systems. That functionality has not yet been implemented.

While it is possible to build EDK II on different platforms, we do not directly support it -- you are on your own!

#### Installing dependencies
    ./uefi/install_deps.sh

The provided shell script will install the required dependencies and initialise all the git submodules for the edk2
package, which are required for building.

#### Building the EFI image
    ./uefi/build.sh

The provided shell script performs **magic** 🪄 to get the out-of-source EDK II build to actually work. The OVMF image (firmware) and LiBoot.efi
executable are then both built. The OVMF firmware is built for testing with QEMU.

The LiBoot executable is then copied to `build/LiBoot.efi` when the build completes successfully. 
