#include <efi.h>
#include <efilib.h>

EFI_STATUS GetSystemMemoryMap(EFI_SYSTEM_TABLE *SystemTable,  UINTN *mapSize, UINTN *mapKey, UINTN *descriptorSize, 
                              UINT32 *descriptorVersion, EFI_MEMORY_DESCRIPTOR **MemoryMap)
{

  EFI_STATUS status = SystemTable->BootServices->GetMemoryMap(mapSize, NULL, mapKey, descriptorSize, descriptorVersion);

  if (status == EFI_BUFFER_TOO_SMALL) 
  {
    Print(L"Memory Map Size: %lu\n", *mapSize);
    *mapSize += *descriptorSize * 8;
    status = SystemTable->BootServices->AllocatePool(EfiLoaderData, *mapSize, (void**)MemoryMap);
    
  }

  status = SystemTable->BootServices->GetMemoryMap(mapSize, *MemoryMap, mapKey, descriptorSize, descriptorVersion);

  if (EFI_ERROR(status))
  {
    Print(L"Something went wrong: %r\n", status);
    return status;
      
  }
  return EFI_SUCCESS;

}


EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  InitializeLib(ImageHandle, SystemTable);
  SystemTable->ConOut->Reset(SystemTable->ConOut, FALSE);
  
  UINTN mapSize = 0;
  UINTN mapKey = 0;
  UINTN descriptorSize = 0;
  UINT32 descriptorVersion = 0;
  EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;


  Print(L"Welcome user to Seed OS\n");
  
  EFI_STATUS status = GetSystemMemoryMap(SystemTable, &mapSize, &mapKey, &descriptorSize, &descriptorVersion, &MemoryMap);
   if (EFI_ERROR(status))
  {
    Print(L"Something went wrong: %r\n", status);
    return status;
  }
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
