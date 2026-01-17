# NekoOS Makefile
AS = nasm
CC = gcc
LD = ld
CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra -O2
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

SRC_DIR = src
BUILD_DIR = build

KERNEL_OBJS = \
	$(BUILD_DIR)/boot/multiboot.o \
	$(BUILD_DIR)/kernel/main.o \
	$(BUILD_DIR)/kernel/multiboot.o \
	$(BUILD_DIR)/kernel/vga.o

.PHONY: all clean run debug

all: nekoos.iso

kernel.bin: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/kernel.bin $^
	@echo "Kernel built: $(BUILD_DIR)/kernel.bin"

$(BUILD_DIR)/boot/%.o: $(SRC_DIR)/boot/%.asm
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/kernel/%.o: $(SRC_DIR)/kernel/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

iso: kernel.bin
	bash scripts/make-iso.sh

nekoos.iso: iso

run-iso: nekoos.iso
	qemu-system-i386 -cdrom nekoos.iso -monitor stdio

run: run-iso

clean:
	rm -rf $(BUILD_DIR) nekoos.iso isodir

debug: nekoos.iso
	qemu-system-i386 -cdrom nekoos.iso -s -S &
	@echo "GDB server running on port 1234"
	@echo "Run: gdb -ex 'target remote localhost:1234' -ex 'symbol-file build/kernel.bin'"
