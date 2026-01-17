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

# Source files (ADD ALL YOUR FILES HERE)
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

# Target
TARGET = $(BUILD_DIR)/kernel.bin

all: $(TARGET)

# Link everything
$(TARGET): $(ALL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Compile C files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM files
$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Run in QEMU
run: $(TARGET)
	qemu-system-i386 -kernel $(TARGET)

.PHONY: all clean run
