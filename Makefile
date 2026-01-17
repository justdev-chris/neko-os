# NekoOS Makefile
# Line 1 MUST start with # or be empty

CC = gcc
AS = nasm
LD = ld
CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

all: nekoos.iso

kernel.bin: build/kernel/main.o build/kernel/multiboot.o build/kernel/vga.o
	$(LD) $(LDFLAGS) -o build/kernel.bin $^

build/kernel/%.o: src/kernel/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

iso: kernel.bin
	bash scripts/make-iso.sh

nekoos.iso: iso

run: nekoos.iso
	qemu-system-i386 -cdrom nekoos.iso

clean:
	rm -rf build nekoos.iso

.PHONY: all clean run iso
