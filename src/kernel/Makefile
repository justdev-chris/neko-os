CC = i686-elf-gcc
CFLAGS = -ffreestanding -O2 -Wall -Wextra
AS = nasm
ASFLAGS = -f elf32

SRCS = main.c vga.c terminal.c
OBJS = $(SRCS:.c=.o)

all: kernel.bin

kernel.bin: $(OBJS) boot.o
	$(CC) -T ../boot/linker.ld -o $@ -ffreestanding -nostdlib $^ -lgcc

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

boot.o: ../boot/multiboot.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f *.o *.bin

run: kernel.bin
	qemu-system-i386 -kernel kernel.bin