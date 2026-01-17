# Compiler/linker
CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld

# Flags
CFLAGS = -ffreestanding -O2 -Wall -Wextra -I./src -nostdlib -nostdinc -fno-builtin
ASFLAGS = -f elf32
LDFLAGS = -T src/boot/linker.ld -nostdlib

# Directories
SRC_DIR = src/kernel
BUILD_DIR = build

# Source files
C_SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/vga.c $(SRC_DIR)/terminal.c $(SRC_DIR)/keyboard.c
ASM_SOURCES = $(SRC_DIR)/io.asm
BOOT_SOURCE = src/boot/multiboot.asm

# Object files
C_OBJS = $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)
ASM_OBJS = $(ASM_SOURCES:%.asm=$(BUILD_DIR)/%.o)
BOOT_OBJ = $(BUILD_DIR)/boot.o

# Target
TARGET = $(BUILD_DIR)/kernel.bin

all: $(TARGET)

# Link everything
$(TARGET): $(C_OBJS) $(ASM_OBJS) $(BOOT_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# Compile C files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble kernel ASM files
$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Assemble bootloader
$(BOOT_OBJ): $(BOOT_SOURCE)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Run in QEMU
run: $(TARGET)
	qemu-system-i386 -kernel $(TARGET)

# Run with debug output
debug: $(TARGET)
	qemu-system-i386 -kernel $(TARGET) -d cpu_reset,int

.PHONY: all clean run debug
