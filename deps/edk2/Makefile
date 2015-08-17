
# This Makefile isn't a part of the TianoCore EDK II
# I made this to simplify the EDK II build process.
#
# Also, this file is licensed under the BSD 3-clause license, 
# just like the rest of LiBoot.

all: 
	$(MAKE) -C BaseTools
	source ./edksetup.sh BaseTools && sed -i 's/Nt32Pkg/MdeModulePkg/g' Conf/target.txt && sed -i 's/MYTOOLS/GCC48/g' Conf/target.txt && sed -i 's/IA32/X64/g' Conf/target.txt && sed -i 's/MAX_CONCURRENT_THREAD_NUMBER = 1/MAX_CONCURRENT_THREAD_NUMBER = 4/g' Conf/target.txt && build

clean: 
	source ./edksetup.sh BaseTools && build clean
	$(MAKE) -C BaseTools clean
