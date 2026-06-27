#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  InitializeLib(ImageHandle, SystemTable);
  SystemTable->ConOut->Reset(SystemTable->ConOut, FALSE);
  
  Print(L"Welcome user to Seed OS\n");
  Print(L"Press any key to continue...\n");

  // Wait for a key press
  EFI_INPUT_KEY Key;
  SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
  UINTN EventIndex;

  // Wait for the key event to signal
  SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &EventIndex);
  SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
  
  // TODO
  // 1. prepare resources in memory 
  // 2. pass vital tables like the graphics buffer
  // 3. exit the uefi boot service by gettint the memory map and calling ExitBootServices
  return EFI_SUCCESS;
}
