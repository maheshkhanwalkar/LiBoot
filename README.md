# LiBoot
A Powerful, Easy to Use (U)EFI Bootloader

### Dependencies
LiBoot uses the TianoCore EDK II - and a copy of it is stored under `deps/`. 
When you issue `make` TianoCore EDK II and LiBoot will both be built. 

**Note:** TianoCore EDK II is licensed under the BSD 2-clause License. 

There are some packages that you need to install to be able to build LiBoot and TianoCore EDK II
They are: 

1. GCC 4.8 or newer
2. Make
3. NASM
4. GCC-C++ (aka g++) 4.8 or newer
5. libuuid-devel
6. ACPI Source Language Compiler/Decompiler (iasl)
