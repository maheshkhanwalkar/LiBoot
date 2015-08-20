all: clean
	mkdir hda_contents
	cp ../deps/edk2/Build/DuetPkgX64/DEBUG_GCC48/X64/LiBoot.efi hda_contents/
	cp ../deps/edk2/Build/OvmfX64/DEBUG_GCC48/FV/OVMF.fd .
	nohup qemu-system-x86_64 -bios OVMF.fd -hda fat:hda_contents &

clean:
	rm -rf hda_contents
	rm -rf OVMF.fd
	rm -rf nohup.out	
