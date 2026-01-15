# NekoOS Makefile
# Use TAB for indentation (not spaces!)

# Compiler and flags
CC = gcc
LD = ld
ASM = nasm
CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

# Directories
SRC_DIR = src
BUILD_DIR = build
ISO_DIR = iso

# Files
KERNEL_SRCS = $(wildcard $(SRC_DIR)/kernel/*.c)
KERNEL_OBJS = $(patsubst $(SRC_DIR)/kernel/%.c,$(BUILD_DIR)/kernel/%.o,$(KERNEL_SRCS))
BOOTLOADER = $(BUILD_DIR)/boot/boot.bin

# Targets
.PHONY: all clean run

all: nekoos.iso

# Kernel
$(BUILD_DIR)/kernel/%.o: $(SRC_DIR)/kernel/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/$@ $^

# Bootloader
$(BOOTLOADER): $(SRC_DIR)/boot/boot.asm
	@mkdir -p $(@D)
	$(ASM) -f bin $< -o $@

# ISO
iso: kernel.bin $(BOOTLOADER)
	bash scripts/make-iso.sh

nekoos.iso: iso
	@echo "ISO created: nekoos.iso"

# Run
run: nekoos.iso
	qemu-system-i386 -cdrom nekoos.iso -monitor stdio

# Debug
debug: nekoos.iso
	qemu-system-i386 -cdrom nekoos.iso -s -S &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(BUILD_DIR)/kernel.bin"

# Clean
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) nekoos.iso