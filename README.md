# LiBoot
A Powerful, Easy to Use (U)EFI Bootloader

### Dependencies
LiBoot uses the [TianoCore EDK II](https://github.com/tianocore/edk2) - and a copy of it is stored under `deps/`. 
When you issue `make` TianoCore EDK II and LiBoot will both be built.  

**TianoCore EDK II Dependencies:**

1. GCC 4.8 or newer
2. GNU Make
3. NASM
4. GCC-C++ (aka g++) 4.8 or newer
5. libuuid-devel
6. ACPI Source Language Compiler/Decompiler (iasl)
7. Python 2 (python -> python2)

**LiBoot Dependencies**

1. GCC 4.8 or newer
2. GNU Make
3. QEMU

WARNING: The EDK2 Build System will break for systems that have python mapped to python3 
and not python2 (Arch Linux for example). Please adjust symlinks to allow the build to
complete successfully. 

Also, it is recommended to have `/bin/sh` mapped to `/bin/bash` instead of some other
shell.  
