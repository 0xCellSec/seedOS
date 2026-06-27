# Makefile for building a UEFI application and running it with QEMU

# Tools
CC = gcc
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-x86_64

# Paths
EFI_INCLUDE = /usr/include/efi
EFI_LIB = /usr/lib

# Files
SRC = EFI/BOOT/BOOTX86_64.c
OBJ = EFI/BOOT/BOOTX86_64.o
SO = EFI/BOOT/BOOTX86_64.so
EFI = EFI/BOOT/BOOTX64.EFI
NSH = startup.nsh

# Compiler flags
CFLAGS = -I$(EFI_INCLUDE) -I$(EFI_INCLUDE)/x86_64 \
         -fno-stack-protector -fpic -fshort-wchar -mno-red-zone \
         -DGNU_EFI_USE_MS_ABI -Wall -Wextra -c

# Linker flags
LDFLAGS = -nostdlib -znocombreloc -T $(EFI_LIB)/elf_x86_64_efi.lds \
          -shared -Bsymbolic $(EFI_LIB)/crt0-efi-x86_64.o \
          -L$(EFI_LIB) -lefi -lgnuefi

# Objcopy flags
OBJCOPY_FLAGS = -j .text -j .sdata -j .data -j .rodata \
                -j .dynamic -j .dynsym -j .rel \
                -j .rela -j .reloc \
                --output-target=efi-app-x86_64 --subsystem=10

# Default target 
all: $(EFI)

# Compile C source to object (ADD TAB TO SECOND LINE)
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

# Link object to shared object (ADD TAB TO SECOND LINE)
$(SO): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

# Convert shared object to EFI (ADD TAB TO SECOND LINE)
$(EFI): $(SO)
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@

# Create startup.nsh for auto-running EFI (ADD TAB TO SECOND LINE)
$(NSH): $(EFI)
	echo "$(EFI)" > $(NSH)

# Run EFI binary with QEMU automatically (ADD TAB TO SECOND LINE)
run: $(EFI) $(NSH)
	$(QEMU) -bios /usr/share/edk2/x64/OVMF.4m.fd \
      	 -drive format=raw,file=fat:rw:. \
         -net none \
         -nographic


# Clean generated files
clean:
	rm -f $(OBJ) $(SO) $(EFI) $(NSH)  # ← This line MUST start with TAB, rmv space

.PHONY: all run run-gui cleanMake sure the tabs are properly added or you may get Makefile indentation errors.
