all: build/kernel.bin nekoos.iso

build/kernel.bin: src/kernel/main.o src/kernel/vga.o src/kernel/keyboard.o src/kernel/terminal.o src/boot/multiboot.o src/kernel/io.o
	@mkdir -p build
	ld -m elf_i386 -T linker.ld -nostdlib -o $@ $^

nekoos.iso: build/kernel.bin
	mkdir -p isodir/boot/grub
	cp build/kernel.bin isodir/boot/
	cp grub/grub.cfg isodir/boot/grub/
	grub-mkrescue -o $@ isodir

# C files
src/kernel/%.o: src/kernel/%.c
	gcc -m32 -ffreestanding -nostdlib -Wall -Wextra -O2 -I./src/kernel -c $< -o $@

# ASM files
src/boot/multiboot.o: src/boot/multiboot.asm
	nasm -f elf32 $< -o $@

src/kernel/io.o: src/kernel/io.asm
	nasm -f elf32 $< -o $@

clean:
	rm -rf src/kernel/*.o src/boot/*.o build/kernel.bin nekoos.iso isodir

run: build/kernel.bin
	qemu-system-i386 -kernel build/kernel.bin
