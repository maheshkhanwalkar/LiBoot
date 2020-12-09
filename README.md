# LiBoot
A Powerful, Easy to Use (U)EFI Bootloader

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

The provided shell script will perform all the configuration magic needed to setup EDK II for building, builds out the OVMF image,
and builds LiBoot. The final result will be within build/LiBoot.efi, which can be installed to the EFI System Partition (ESP) for use.
