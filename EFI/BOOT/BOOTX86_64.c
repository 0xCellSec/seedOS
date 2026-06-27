#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  InitializeLib(ImageHandle, SystemTable);
  SystemTable->ConOut->Reset(SystemTable->ConOut, FALSE);
  
  Print(L"Seed OS by NM, well you can change the name!\n");
  Print(L"Press any key to continue...\n");

  // Wait for a key press
  EFI_INPUT_KEY Key;
  SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
  UINTN EventIndex;

  // Wait for the key event to signal
  SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &EventIndex);
  SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
  
 
  return EFI_SUCCESS;
}
