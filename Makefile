CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra -O2 -I./src/kernel
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

C_SOURCES = src/kernel/main.c src/kernel/vga.c src/kernel/keyboard.c src/kernel/terminal.c
ASM_SOURCES = src/boot/multiboot.asm src/kernel/io.asm

C_OBJS = $(C_SOURCES:.c=.o)
ASM_OBJS = $(ASM_SOURCES:.asm=.o)
ALL_OBJS = $(C_OBJS) $(ASM_OBJS)

KERNEL = build/kernel.bin
ISO = nekoos.iso
ISO_DIR = isodir

all: $(KERNEL) $(ISO)

$(KERNEL): $(ALL_OBJS)
	@mkdir -p build
	$(LD) $(LDFLAGS) -o $@ $^

nekoos.iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp build/kernel.bin isodir/boot/
	cp grub/grub.cfg isodir/boot/grub/
	grub-mkrescue -o $@ isodir 2>/dev/null || true

%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(C_OBJS) $(ASM_OBJS) $(KERNEL) $(ISO) $(ISO_DIR)

run: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL)

iso-run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

.PHONY: all clean run iso-run
