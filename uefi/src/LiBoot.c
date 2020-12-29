
#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>

#include <Base.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/Shell.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/DevicePathLib.h>

void print_guid(EFI_GUID guid)
{
	Print(L"Vendor GUID = {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n", guid.Data1, guid.Data2, 
			guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], 
			  guid.Data4[3], guid.Data4[4], guid.Data4[5],
			     guid.Data4[6], guid.Data4[7]); 	
}

#define MAX_IMAGE_SIZE 1024*1024*100
#define PT_LOAD 0x00000001

typedef struct elf64_header {
	char e_ident[16];
	unsigned short e_type;
	unsigned short e_machine;
	unsigned int e_version;
	unsigned long e_entry;
	unsigned long e_phoff;
	unsigned long e_shoff;
	unsigned int e_flags;
	unsigned short e_ehsize;
	unsigned short e_phentsize;
	unsigned short e_phnum;
	unsigned short e_shentsize;
	unsigned short e_shnum;
	unsigned short e_shstrndx;
} elf64_header_t;

typedef struct elf64_pheader {
    unsigned int p_type;
    unsigned int p_flags;
    unsigned long p_offset;
    unsigned long p_vaddr;
    unsigned long p_paddr;
    unsigned long p_filesz;
    unsigned long p_memsz;
    unsigned long p_align;
} elf64_pheader_t;

typedef struct entry {
	unsigned long count;
	void* entries;
} entry_t;


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

	EFI_GUID sfs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	UINTN fs_num;
	EFI_HANDLE* fs_handles;

	boot->LocateHandleBuffer(ByProtocol, &sfs_guid, NULL, &fs_num, &fs_handles);
	Print(L"Number of FS: %u\n", fs_num);

	char* buffer;
	EFI_STATUS res;

	res = boot->AllocatePool(EfiLoaderData, MAX_IMAGE_SIZE, (void**)(&buffer));

	if(EFI_ERROR(res)) {
		Print(L"Could not allocate buffer. Not enough memory!\n");
		return EFI_SUCCESS;
	}

	Print(L"Allocated the buffer.\n");

	for(int i = 0; i < fs_num; i++) {
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs = NULL;
		res = boot->HandleProtocol(fs_handles[i], &sfs_guid, (void**)&fs);

		if(EFI_ERROR(res)) {
			continue;
		}

		EFI_FILE_PROTOCOL* root = NULL;
		res = fs->OpenVolume(fs, &root);

		if(EFI_ERROR(res)) {
			continue;
		}

		EFI_FILE_PROTOCOL* token = NULL;
		res = root->Open(root, &token, L"\\vminix", EFI_FILE_MODE_READ, 0);

		if(EFI_ERROR(res)) {
			Print(L"Could not open file! \n");
			continue;
		} else {
			Print(L"Opened the file!\n");
		}

		UINTN amt = MAX_IMAGE_SIZE;
		res = token->Read(token, &amt, (void*)buffer);

		if(EFI_ERROR(res)) {
			Print(L"Could not read file! \n");
		}

		Print(L"Bytes read: %d\n", amt);

		boot->FreePool(buffer);
		res = boot->AllocatePool(EfiLoaderData, amt, (void**)(&buffer));

		if(EFI_ERROR(res)) {
			Print(L"Could not reallocate buffer! \n");
		}

		token->SetPosition(token, 0);
		res = token->Read(token, &amt, (void*)buffer);

		if(EFI_ERROR(res)) {
			Print(L"Could not read file! \n");
		}

		Print(L"MAGIC: %c%c%c\n", buffer[1], buffer[2], buffer[3]);
		elf64_header_t* elf = (elf64_header_t*)buffer;

		Print(L"PH NUM: %d\n", elf->e_phnum);

		elf64_pheader_t* loadable;
		int l_pos = 0;

		boot->AllocatePool(EfiLoaderData, sizeof(elf64_pheader_t) * elf->e_phnum, (void**)&loadable);

		for(int i = 0; i < elf->e_phnum; i++) {
			elf64_pheader_t* ph = (elf64_pheader_t*)(&buffer[elf->e_phoff + i * elf->e_phentsize]);

			if(ph->p_type != PT_LOAD)
				continue;

			Print(L"Found loadable segment, p_flags: %lu\n", ph->p_flags);
			loadable[l_pos++] = *ph;
		}

		if(l_pos == 0) {
			Print(L"No loadable segments found!\n");
			return EFI_SUCCESS;
		}

		int sz;

		if(l_pos == 1) {
			sz = loadable[0].p_memsz;
		} else {
			sz = (loadable[l_pos - 1].p_vaddr + loadable[l_pos - 1].p_memsz) - loadable[0].p_vaddr;
		}

		// Calculate number of pages -- rounding up
		int pages = (sz + 0x1000 - 1) / 0x1000;
		Print(L"Number of pages required: %d\n", pages);

		char* kernel_img;
		res = boot->AllocatePages(AllocateAnyPages, EfiRuntimeServicesCode, pages, (EFI_PHYSICAL_ADDRESS*) &kernel_img);

		if(EFI_ERROR(res)) {
			Print(L"Could not allocate pages!\n");
			return EFI_SUCCESS;
		}

		// Copy the segments into the in-memory images
		for(int i = 0; i < l_pos; i++) {
			boot->CopyMem((void*)&kernel_img[loadable[i].p_vaddr - loadable[0].p_vaddr], (void*)&buffer[loadable[i].p_offset], loadable[i].p_filesz);
			boot->SetMem((void*)&kernel_img[loadable[i].p_vaddr - loadable[0].p_vaddr + loadable[i].p_filesz], loadable[i].p_memsz - loadable[i].p_filesz, 0);
		}

		unsigned long jmp_point = (elf->e_entry - loadable[0].p_vaddr) + (unsigned long)kernel_img;

		Print(L"Initialised kernel image: entry point: %llx\n", jmp_point);
		Print(L"Booting.....\n");

		// Cleanup stuff that's no longer needed
		boot->FreePool(buffer);

		// Allocate kernel args
		entry_t* img_ent, *map_ent;

		boot->AllocatePool(EfiLoaderData, sizeof(*img_ent), (void**)(&img_ent));
		boot->AllocatePool(EfiLoaderData, sizeof(*map_ent), (void**)(&map_ent));

		img_ent->count = l_pos;
		img_ent->entries = (void*)loadable;

		// Determine how big the memory map is
		UINTN map_sz = 0, map_key = 0, desc_sz;
		EFI_MEMORY_DESCRIPTOR* map = NULL;
		boot->GetMemoryMap(&map_sz, map, NULL, &desc_sz, NULL);

		// Safety: in case the allocate pool somehow splits two memory map segments
		map_sz += desc_sz * 2;
		boot->AllocatePool(EfiLoaderData, map_sz, (void**)(&map));

		// Get the memory map
		UINT32 desc_ver;
		res = boot->GetMemoryMap(&map_sz, map, &map_key, &desc_sz, &desc_ver);

		map_ent->count = map_sz / desc_sz;
		map_ent->entries = map;

		// Disable watchdog timer
		boot->SetWatchdogTimer(0, 0, 0, NULL);

		// Exit boot services
		res = boot->ExitBootServices(image_handle, map_key);

		if(EFI_ERROR(res)) {
			Print(L"Could not exit boot services!\n");
			return EFI_SUCCESS;
		}

		// Jump into the kernel
		typedef void kernel_entry(unsigned long magic, unsigned long kern_base, void* kern_img, void* map);
		((kernel_entry*)jmp_point)(0xEF1B001, (unsigned long)kernel_img, (void*)img_ent, (void*)map_ent);
	}

	return EFI_SUCCESS;
}


