
#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>

#include <Base.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>

#include <Protocol/LoadedImage.h>
#include <Library/DevicePathLib.h>

void print_guid(EFI_GUID guid)
{
	Print(L"Vendor GUID = {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n", guid.Data1, guid.Data2, 
			guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], 
			  guid.Data4[3], guid.Data4[4], guid.Data4[5],
			     guid.Data4[6], guid.Data4[7]); 	
}


EFI_STATUS EFIAPI uefi_main(IN EFI_HANDLE image_handle, IN EFI_SYSTEM_TABLE *sys_table)
{
	Print(L"LiBoot (U)EFI Bootloader v0.0.1\n\n");
	Print(L"Probing EFI System Table.....\n\n");

	CHAR16* vendor = sys_table->FirmwareVendor;
	UINT32 fversion = sys_table->FirmwareRevision;

	EFI_TABLE_HEADER header = sys_table->Hdr; 
	UINT32 uefi_rev = header.Revision;

	Print(L"UEFI Revision: %d.%02d\n\n", uefi_rev >> 16, uefi_rev & 0xFFFF);

	Print(L"Firmware Vendor: %s\n", vendor);
	Print(L"Firmware Revision: %d.%02d\n\n", fversion >> 16, fversion & 0xFFFF);

	EFI_CONFIGURATION_TABLE* conf = sys_table->ConfigurationTable;
	UINTN conf_num = sys_table->NumberOfTableEntries; 

	Print(L"Number of (U)EFI Configuration Tables: %d\n", conf_num);
	Print(L"Printing out Vendor GUIDs....\n\n");

	for(int i = 0; i < conf_num; i++) {
		EFI_CONFIGURATION_TABLE entry = conf[i];
		EFI_GUID guid = entry.VendorGuid; 

		print_guid(guid);				
	}

	Print(L"\n");
	EFI_BOOT_SERVICES* boot = sys_table->BootServices;
	//EFI_RUNTIME_SERVICES* run_serv = sys_table->RuntimeServices;

	EFI_GUID blockio_guid = BLOCK_IO_PROTOCOL;

	UINTN fs_num;
	EFI_HANDLE* fs_handles;

	boot->LocateHandleBuffer(ByProtocol, &blockio_guid, NULL, &fs_num, &fs_handles);
	
	Print(L"Number of Block Devices: %u\n", fs_num);	
	Print(L"Scanning Block Devices....\n\n");

	EFI_BLOCK_IO* block_io;
	EFI_GUID vmlinuz_proto = LOADED_IMAGE_PROTOCOL;

	for(int i = 0; i < fs_num; i++) {
		EFI_STATUS res = boot->HandleProtocol(fs_handles[i], &blockio_guid, (void**)&block_io);

		if(!EFI_ERROR(res)) {
			//OK, we are good to go...
			EFI_BLOCK_IO_MEDIA* media = block_io->Media; 
			if(!media->MediaPresent) {
				Print(L"DEVICE %x: \n", fs_handles[i]);
				Print(L"DEVICE NOT PRESENT, SKIPPING \n\n");
			}
			else {
				Print(L"DEVICE %x: \n", fs_handles[i]);
				Print(L"DEVICE PRESENT \n");
				Print(L"REMOVABLE MEDIA?  %s\n", media->RemovableMedia ? L"TRUE" : L"FALSE");

				Print(L"Attempting to load 'vmlinuz.efi'.....\n");
				EFI_DEVICE_PATH_PROTOCOL* vmlinuz_path = FileDevicePath(fs_handles[i], L"\\vmlinuz.efi");
				
				EFI_HANDLE vmlinuz;
				EFI_STATUS stat = boot->LoadImage(FALSE, image_handle, vmlinuz_path, NULL, 0, &vmlinuz);

				if(!EFI_ERROR(stat)) {
					EFI_LOADED_IMAGE* vmlinuz_boot;
					stat = boot->HandleProtocol(vmlinuz, &vmlinuz_proto, (void**)&vmlinuz_boot);
					
					CHAR16* boot_ops = L"root=/dev/sda2";

					vmlinuz_boot->LoadOptionsSize = 28;
					vmlinuz_boot->LoadOptions = boot_ops;

					boot->StartImage(vmlinuz, NULL, NULL); 
				}
				else {
					Print(L"Couldn't load 'vmlinuz.efi'. Skipping\n\n");
				}
			}
		}
	}	

	//boot->LocateHandleBuffer(ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &NumberFileSystemHandles, &FileSystemHandles); 


	//Print(L"\nPreparing to load Linux (currently using hard-coded path)\n");
	//Print(L"Loading 'vmlinuz.efi'....\n\n");

	//EFI_DEVICE_PATH_PROTOCOL* path = FileDevicePath(image_handle, L"vmlinuz.efi");

	//status = ShellFileExists(L"\\vmlinux.efi");
	//Print(L"\nFile Exists? %r \n\n", status);


	//EFI_HANDLE linux_handle;
	//EFI_STATUS result = boot->LoadImage(FALSE, image_handle, path, NULL, 0, &linux_handle);
	
	/*if(result == EFI_SUCCESS)
	{
		Print(L"Successfully loaded 'vmlinux.efi' into memory!\n");
		Print(L"Starting Linux Kernel....\n\n");

		boot->StartImage(linux_handle, NULL, NULL);
	}
	else
	{
		Print(L"LiBoot encountered an error when trying to load 'vmlinuz.efi'!\n");
	}*/

	return EFI_SUCCESS;
}


