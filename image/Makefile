all:
	mkdir hda_contents
	cp ../deps/edk2/Build/DuetPkgX64/DEBUG_GCC48/X64/LiBoot.efi hda_contents/
	cp ../deps/edk2/Build/OvmfX64/DEBUG_GCC48/FV/OVMF.fd .
	qemu-system-x86_64 -bios OVMF.fd -hda fat:hda_contents
	
