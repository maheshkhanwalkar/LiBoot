
#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>

#include <Library/UefiLib.h>

EFI_STATUS EFIAPI uefi_main(IN EFI_HANDLE image_handle, IN EFI_SYSTEM_TABLE *sys_table)
{
	Print(L"LiBoot (U)EFI Bootloader v0.0.1\n");
	return EFI_SUCCESS;
}
