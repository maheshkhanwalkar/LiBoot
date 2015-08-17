# LiBoot
A Powerful, Easy to Use (U)EFI Bootloader

### Dependencies
LiBoot uses the [TianoCore EDK II](https://github.com/tianocore/edk2) - and a copy of it is stored under `deps/`. 
When you issue `make` TianoCore EDK II and LiBoot will both be built.  

**TianoCore EDK II Dependencies:**

1. GCC 4.8 or newer (1)
2. GNU Make
3. NASM
4. GCC-C++ (aka g++) 4.8 or newer (1)
5. libuuid-devel
6. ACPI Source Language Compiler/Decompiler (iasl)

**LiBoot Dependencies**
1. GCC 4.8 or newer (1)
2. GNU Make




(1) This can be changed to older GCC versions (though I haven't tested it)
