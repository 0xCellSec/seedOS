# UEFI

## BOOTX64.EFI

### current responsibilities
* loads kernel 
* checks kernel's elf magic numbers
* loads bootable segments
* displays segment info 
* waits for user input to continue
* gets the memory map of everything that was loaded 
* exit the BootServices
* hand over control to the kernel and initiate it through the entry


