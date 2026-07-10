#include <efi.h>
#include <efilib.h>

#define PT_LOAD 1

EFI_STATUS status;

// elf header struct
struct elf64_ehdr{
  unsigned char e_ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
};

struct elf64_phdr {
  uint32_t p_type;
  uint32_t p_flags;

  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;

  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
};




EFI_STATUS GetSystemMemoryMap(EFI_SYSTEM_TABLE *SystemTable,  UINTN *mapSize, UINTN *mapKey, UINTN *descriptorSize, 
                              UINT32 *descriptorVersion, EFI_MEMORY_DESCRIPTOR **MemoryMap)
{
  status = SystemTable->BootServices->GetMemoryMap(mapSize, NULL, mapKey, descriptorSize, descriptorVersion);

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

EFI_STATUS KernelLoader( EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, EFI_PHYSICAL_ADDRESS *buffer, UINTN kernelSize, UINTN pages) {  
   //loading kernel declerations
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
  EFI_FILE_PROTOCOL *Root;
  EFI_FILE_PROTOCOL *KernelFile;
  EFI_FILE_INFO *fileInfo;

  EFI_STATUS status;

  // loading the kernel
  Print(L"loading kernel onto memory\n");
  status = SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);
  if (EFI_ERROR(status)) {
    Print(L"something went wrong in kernel loader\n", status);
  }
  Print(L"loaded image\n");
  
  status = SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);
  if (EFI_ERROR(status)) {
    Print(L"something went wrong in kernel loader\n", status);
  }
  Print(L"passed image to file system\n");
  
  FileSystem->OpenVolume(FileSystem, &Root);
  if (EFI_ERROR(status)) {
    Print(L"something went wrong in kernel loader\n", status);
  }
  Print(L"opened the file\n");
  
  status = Root->Open(Root, &KernelFile, L"kernel\\kernel.elf", EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR(status)) {
    Print(L"something went wrong in kernel loader\n", status);
  }
  Print (L"set file to read mode\n");

  fileInfo = LibFileInfo(KernelFile);
  if (fileInfo == NULL) {
    Print(L"LibFileInfo returned NULL\n");
  }
  Print(L"got file info\n");
  kernelSize = fileInfo->FileSize; 
  Print(L"captured kernel size from info\n");
  pages = (kernelSize + 0xFFF) / 0x1000;
  SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, pages, buffer);
  KernelFile->Read(KernelFile, &kernelSize, (void *)buffer);
  Print(L"Kernel loaded\n");

  return EFI_SUCCESS;
}

EFI_STATUS ElfMagicCheck (struct elf64_ehdr *elf) {
  Print(L"checking ELF magic numbers");

  // checking for the elf magic numbers
    if (elf->e_ident[0] == 0x7F && elf->e_ident[1] == 'E' && elf->e_ident[2] == 'L' 
      && elf->e_ident[3] == 'F'){
    Print(L"File validated to be an ELF executable\n");

  } 
  else {
    Print(L"Couldn't validate the file to be an ELF executable\n");
  }
  
  return EFI_SUCCESS;

}


EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{

  InitializeLib(ImageHandle, SystemTable);
  SystemTable->ConOut->Reset(SystemTable->ConOut, FALSE);
  
  // declaring necessary variables
  UINTN mapSize = 0;
  UINTN mapKey = 0;
  UINTN descriptorSize = 0;
  UINT32 descriptorVersion = 0;
  EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;


  Print(L"Welcome user to Seed OS\n");


  EFI_PHYSICAL_ADDRESS *buffer;
  UINTN kernelSize;
  UINTN pages;


  status = KernelLoader(ImageHandle, SystemTable, buffer, kernelSize, pages);
  if (EFI_ERROR(status))
  {
    Print(L"Something went wrong: %r\n", status);
    return status;
  }
  
  struct elf64_ehdr *elf = (struct elf64_ehdr *)buffer;
  status = ElfMagicCheck(elf);
  if (EFI_ERROR(status))
  {
    Print(L"Something went wrong: %r\n", status);
    return status;
  }

  // load the progam segments
  Print(L"Parsing the program segments and outputting data\n");
  struct elf64_phdr *phdr = (struct elf64_phdr *)((char *)buffer + elf->e_phoff);
  
  Print(L"\nphoff: 0x%lx", elf->e_phoff);
  Print(L"\nphnum: %d\n", elf->e_phnum, L"\n" );
  for (UINTN i = 0; i < elf->e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      Print(L"Found a loadable ELF segment!\n" 
            L"offest: 0x%lx\n"
            L"vaddr: 0x%lx\n"
            L"filesize: 0x%lx\n"
            L"memsz: 0x%lx\n",
            phdr[i].p_offset,
            phdr[i].p_vaddr,
            phdr[i].p_filesz,
            phdr[i].p_memsz);

      EFI_PHYSICAL_ADDRESS segAddr = phdr[i].p_vaddr;

      UINTN segPages = (phdr[i].p_memsz + 0xFFF) / 0x1000;

      status = SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, segPages, &segAddr);
       if (EFI_ERROR(status))
        {
          Print(L"Something went wrong: %r\n", status);
          return status;
        }

      CopyMem((void *)phdr[i].p_vaddr, (char *)buffer + phdr[i].p_offset, phdr[i].p_filesz);
      if (phdr[i].p_memsz > phdr[i].p_filesz) {
        SetMem((void *)(phdr[i].p_vaddr + phdr[i].p_filesz), phdr[i].p_memsz - phdr[i].p_filesz, 0);
      }

      Print(L"Loaded segment %d to 0x%lx (%lu bytes)\n", i, phdr[i].p_vaddr, phdr[i].p_memsz);
    }
  }
  
  Print(L"Press any key to continue...\n");

  // Wait for a key press
  EFI_INPUT_KEY Key;
  SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
  UINTN EventIndex;

  // Wait for the key event to signal
  SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &EventIndex);
  SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);

  // getting memory map
  status = GetSystemMemoryMap(SystemTable, &mapSize, &mapKey, &descriptorSize, &descriptorVersion, &MemoryMap);
  if (EFI_ERROR(status))
  {
    Print(L"Something went wrong: %r\n", status);
    return status;
  }

  // transferring control to the kernel
  status = SystemTable->BootServices->ExitBootServices(ImageHandle, mapKey);
  if (EFI_ERROR(status))
  {
    Print(L"Something went wrong: %r\n", status);
    return status;
  }

  void (*KernelEntry)(void) = (void (*)(void))elf->e_entry;
  KernelEntry();
  
  return EFI_SUCCESS;
}
