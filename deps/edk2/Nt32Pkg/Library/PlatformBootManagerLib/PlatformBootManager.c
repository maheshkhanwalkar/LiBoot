/** @file
  This file include all platform action which can be customized
  by IBV/OEM.

Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformBootManager.h"


EFI_GUID mUefiShellFileGuid = { 0x7C04A583, 0x9E3E, 0x4f1c, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 };

/**
  Perform the memory test base on the memory test intensive level,
  and update the memory resource.

  @param  Level         The memory test intensive level.

  @retval EFI_STATUS    Success test all the system memory and update
                        the memory resource

**/
EFI_STATUS
PlatformBootManagerMemoryTest (
  IN EXTENDMEM_COVERAGE_LEVEL Level
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           RequireSoftECCInit;
  EFI_GENERIC_MEMORY_TEST_PROTOCOL  *GenMemoryTest;
  UINT64                            TestedMemorySize;
  UINT64                            TotalMemorySize;
  UINTN                             TestPercent;
  UINT64                            PreviousValue;
  BOOLEAN                           ErrorOut;
  UINT32                            TempData;

  TestedMemorySize  = 0;
  TotalMemorySize   = 0;
  PreviousValue     = 0;

  RequireSoftECCInit = FALSE;

  Status = gBS->LocateProtocol (
                  &gEfiGenericMemTestProtocolGuid,
                  NULL,
                  (VOID **) &GenMemoryTest
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Status = GenMemoryTest->MemoryTestInit (
                            GenMemoryTest,
                            Level,
                            &RequireSoftECCInit
                            );
  if (Status == EFI_NO_MEDIA) {
    //
    // The PEI codes also have the relevant memory test code to check the memory,
    // it can select to test some range of the memory or all of them. If PEI code
    // checks all the memory, this BDS memory test will has no not-test memory to
    // do the test, and then the status of EFI_NO_MEDIA will be returned by
    // "MemoryTestInit". So it does not need to test memory again, just return.
    //
    return EFI_SUCCESS;
  }

  do {
    Status = GenMemoryTest->PerformMemoryTest (
                              GenMemoryTest,
                              &TestedMemorySize,
                              &TotalMemorySize,
                              &ErrorOut,
                              FALSE
                              );
    if (ErrorOut && (Status == EFI_DEVICE_ERROR)) {
      Print (L"System encounters memory errors!");
      CpuDeadLoop ();
    }
    
    TempData = (UINT32) DivU64x32 (TotalMemorySize, 16);
    TestPercent = (UINTN) DivU64x32 (
                            DivU64x32 (MultU64x32 (TestedMemorySize, 100), 16),
                            TempData
                            );
    if (TestPercent != PreviousValue) {
      Print (L"Perform memory test: %d/100", TestPercent);
      PreviousValue = TestPercent;
    }
  } while (Status != EFI_NOT_FOUND);

  Status = GenMemoryTest->Finished (GenMemoryTest);

  Print (L"\r%dM bytes of system memory tested OK\n", (UINT32) DivU64x32 (TotalMemorySize, 1024 * 1024));
  return EFI_SUCCESS;
}


/**
  Return the index of the load option in the load option array.

  The function consider two load options are equal when the 
  OptionType, Attributes, Description, FilePath and OptionalData are equal.

  @param Key    Pointer to the load option to be found.
  @param Array  Pointer to the array of load options to be found.
  @param Count  Number of entries in the Array.

  @retval -1          Key wasn't found in the Array.
  @retval 0 ~ Count-1 The index of the Key in the Array.
**/
INTN
PlatformFindLoadOption (
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION *Key,
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION *Array,
  IN UINTN                              Count
  )
{
  UINTN                             Index;

  for (Index = 0; Index < Count; Index++) {
    if ((Key->OptionType == Array[Index].OptionType) &&
        (Key->Attributes == Array[Index].Attributes) &&
        (StrCmp (Key->Description, Array[Index].Description) == 0) &&
        (CompareMem (Key->FilePath, Array[Index].FilePath, GetDevicePathSize (Key->FilePath)) == 0) &&
        (Key->OptionalDataSize == Array[Index].OptionalDataSize) &&
        (CompareMem (Key->OptionalData, Array[Index].OptionalData, Key->OptionalDataSize) == 0)) {
      return (INTN) Index;
    }
  }

  return -1;
}

VOID
PlatformRegisterFvBootOption (
  EFI_GUID                         *FileGuid,
  CHAR16                           *Description,
  UINT32                           Attributes
  )
{
  EFI_STATUS                        Status;
  UINTN                             OptionIndex;
  EFI_BOOT_MANAGER_LOAD_OPTION      NewOption;
  EFI_BOOT_MANAGER_LOAD_OPTION      *BootOptions;
  UINTN                             BootOptionCount;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH FileNode;
  EFI_LOADED_IMAGE_PROTOCOL         *LoadedImage;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;

  Status = gBS->HandleProtocol (gImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &LoadedImage);
  ASSERT_EFI_ERROR (Status);

  EfiInitializeFwVolDevicepathNode (&FileNode, FileGuid);
  DevicePath = AppendDevicePathNode (
                 DevicePathFromHandle (LoadedImage->DeviceHandle),
                 (EFI_DEVICE_PATH_PROTOCOL *) &FileNode
                 );

  Status = EfiBootManagerInitializeLoadOption (
             &NewOption,
             LoadOptionNumberUnassigned,
             LoadOptionTypeBoot,
             Attributes,
             Description,
             DevicePath,
             NULL,
             0
             );
  if (!EFI_ERROR (Status)) {
    BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);

    OptionIndex = PlatformFindLoadOption (&NewOption, BootOptions, BootOptionCount);

    if (OptionIndex == -1) {
      Status = EfiBootManagerAddLoadOptionVariable (&NewOption, (UINTN) -1);
      ASSERT_EFI_ERROR (Status);
    }
    EfiBootManagerFreeLoadOption (&NewOption);
    EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
  }
}

/**
  Do the platform specific action before the console is connected.

  Such as:
    Update console variable;
    Register new Driver#### or Boot####;
    Signal ReadyToLock event.
**/
VOID
EFIAPI
PlatformBootManagerBeforeConsole (
  VOID
  )
{
  UINTN                        Index;
  EFI_STATUS                   Status;
  WIN_NT_SYSTEM_CONFIGURATION  *Configuration;
  EFI_INPUT_KEY                Enter;
  EFI_INPUT_KEY                F2;
  EFI_BOOT_MANAGER_LOAD_OPTION BootOption;

  GetVariable2 (L"Setup", &gEfiWinNtSystemConfigGuid, (VOID **) &Configuration, NULL);
  if (Configuration != NULL) {
    //
    // SetupVariable is corrupt
    //
    Configuration->ConOutRow = PcdGet32 (PcdConOutColumn);
    Configuration->ConOutColumn = PcdGet32 (PcdConOutRow);

    Status = gRT->SetVariable (
                    L"Setup",
                    &gEfiWinNtSystemConfigGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (WIN_NT_SYSTEM_CONFIGURATION),
                    Configuration
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Failed to save Setup Variable to non-volatile storage, Status = %r\n", Status));
    }
    FreePool (Configuration);
  }

  //
  // Update the ocnsole variables.
  //
  for (Index = 0; gPlatformConsole[Index].DevicePath != NULL; Index++) {
    if ((gPlatformConsole[Index].ConnectType & CONSOLE_IN) == CONSOLE_IN) {
      EfiBootManagerUpdateConsoleVariable (ConIn, gPlatformConsole[Index].DevicePath, NULL);
    }

    if ((gPlatformConsole[Index].ConnectType & CONSOLE_OUT) == CONSOLE_OUT) {
      EfiBootManagerUpdateConsoleVariable (ConOut, gPlatformConsole[Index].DevicePath, NULL);
    }

    if ((gPlatformConsole[Index].ConnectType & STD_ERROR) == STD_ERROR) {
      EfiBootManagerUpdateConsoleVariable (ErrOut, gPlatformConsole[Index].DevicePath, NULL);
    }
  }

  //
  // Register ENTER as CONTINUE key
  //
  Enter.ScanCode    = SCAN_NULL;
  Enter.UnicodeChar = CHAR_CARRIAGE_RETURN;
  EfiBootManagerRegisterContinueKeyOption (0, &Enter, NULL);
  //
  // Map F2 to Boot Manager Menu
  //
  F2.ScanCode    = SCAN_F2;
  F2.UnicodeChar = CHAR_NULL;
  EfiBootManagerGetBootManagerMenu (&BootOption);
  EfiBootManagerAddKeyOptionVariable (NULL, (UINT16) BootOption.OptionNumber, 0, &F2, NULL);
  //
  // Register UEFI Shell
  //
  PlatformRegisterFvBootOption (&mUefiShellFileGuid, L"UEFI Shell", LOAD_OPTION_ACTIVE);
}

/**
  Do the platform specific action after the console is connected.

  Such as:
    Dynamically switch output mode;
    Signal console ready platform customized event;
    Run diagnostics like memory testing;
    Connect certain devices;
    Dispatch aditional option roms.
**/
VOID
EFIAPI
PlatformBootManagerAfterConsole (
  VOID
  )
{
  PlatformBootManagerMemoryTest (QUICK);
  EfiBootManagerConnectAll ();
  EfiBootManagerRefreshAllBootOption ();
  Print (
    L"\n"
    L"F2    to enter Boot Manager Menu.\n"
    L"Enter to boot directly.\n"
    L"\n"
    );
}

/**
  This function is called each second during the boot manager waits the timeout.

  @param TimeoutRemain  The remaining timeout.
**/
VOID
EFIAPI
PlatformBootManagerWaitCallback (
  UINT16          TimeoutRemain
  )
{
  Print (L"\r%-2d seconds remained...", TimeoutRemain);
}
