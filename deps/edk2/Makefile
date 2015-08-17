
all: 
	$(MAKE) -C BaseTools
	source ./edksetup.sh BaseTools && sed -i 's/Nt32Pkg/MdeModulePkg/g' Conf/target.txt && sed -i 's/MYTOOLS/GCC48/g' Conf/target.txt && sed -i 's/IA32/X64/g' Conf/target.txt && sed -i 's/MAX_CONCURRENT_THREAD_NUMBER = 1/MAX_CONCURRENT_THREAD_NUMBER = 4/g' Conf/target.txt && build

clean: 
	source ./edksetup.sh BaseTools && build clean
	$(MAKE) -C BaseTools clean
