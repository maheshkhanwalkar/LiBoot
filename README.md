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
    TODO - build system has not yet been implemented!
