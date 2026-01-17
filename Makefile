# Compiler/linker
CC = gcc
AS = nasm
LD = ld

# Flags
CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra -O2 -I./src/kernel
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Directories
SRC_DIR = src
KERNEL_DIR = $(SRC_DIR)/kernel
BOOT_DIR = $(SRC_DIR)/boot
BUILD_DIR = build
ISO_DIR = isodir

# Source files
C_SOURCES = \
	$(KERNEL_DIR)/main.c \
	$(KERNEL_DIR)/vga.c \
	$(KERNEL_DIR)/keyboard.c \
	$(KERNEL_DIR)/terminal.c \
	$(KERNEL_DIR)/multiboot.c

ASM_SOURCES = \
	$(BOOT_DIR)/multiboot.asm \
	$(KERNEL_DIR)/io.asm

# Object files
C_OBJS = $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)
ASM_OBJS = $(ASM_SOURCES:%.asm=$(BUILD_DIR)/%.o)
ALL_OBJS = $(C_OBJS) $(ASM_OBJS)

# Targets
KERNEL_TARGET = $(BUILD_DIR)/kernel.bin
ISO_TARGET = nekoos.iso
GRUB_CFG = grub/grub.cfg  # YOUR ACTUAL PATH

# Main target builds both kernel and ISO
all: $(KERNEL_TARGET) $(ISO_TARGET)

# Build kernel binary
$(KERNEL_TARGET): $(ALL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Build bootable ISO
$(ISO_TARGET): $(KERNEL_TARGET) $(GRUB_CFG)
	@echo "Creating bootable ISO..."
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_TARGET) $(ISO_DIR)/boot/
	cp $(GRUB_CFG) $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $@ $(ISO_DIR)
	@echo "ISO created: $(ISO_TARGET)"

# Compile C files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM files
$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Clean everything
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO_TARGET)

# Run kernel directly in QEMU (fast debugging)
run: $(KERNEL_TARGET)
	qemu-system-i386 -kernel $(KERNEL_TARGET)

# Run full ISO in QEMU (tests actual boot process)
iso-run: $(ISO_TARGET)
	qemu-system-i386 -cdrom $(ISO_TARGET)

# Run ISO with debugging
debug: $(ISO_TARGET)
	qemu-system-i386 -cdrom $(ISO_TARGET) -d int,cpu_reset

.PHONY: all clean run iso-run debug
