# NekoOS Makefile - Root directory
# Use TAB for indentation, not spaces!

CC = gcc
AS = nasm
LD = ld
CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

# Source files
SRC_DIR = src
BUILD_DIR = build

# Object files - ADD multiboot.asm here!
KERNEL_OBJS = \
	$(BUILD_DIR)/boot/multiboot.o \
	$(BUILD_DIR)/kernel/main.o \
	$(BUILD_DIR)/kernel/multiboot.o \
	$(BUILD_DIR)/kernel/vga.o

# Targets
.PHONY: all clean run

all: nekoos.iso

# Main kernel binary
kernel.bin: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/kernel.bin $^

# Assembly files (Multiboot header)
$(BUILD_DIR)/boot/%.o: $(SRC_DIR)/boot/%.asm
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

# C files
$(BUILD_DIR)/kernel/%.o: $(SRC_DIR)/kernel/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# ISO creation
iso: kernel.bin
	bash scripts/make-iso.sh

nekoos.iso: iso
	@echo "ISO created: nekoos.iso"

# Run in QEMU
run: nekoos.iso
	qemu-system-i386 -cdrom nekoos.iso -monitor stdio

# Clean
clean:
	rm -rf $(BUILD_DIR) nekoos.iso isodir

.PHONY: all clean run iso
