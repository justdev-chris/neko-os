CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra -O2 -I./src/kernel
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

SRCS = src/kernel/main.c src/kernel/vga.c src/kernel/keyboard.c src/kernel/terminal.c
OBJS = $(SRCS:.c=.o) src/boot/multiboot.o src/kernel/io.o

KERNEL = build/kernel.bin
ISO = nekoos.iso
ISO_DIR = isodir

all: $(KERNEL) $(ISO)

$(KERNEL): $(OBJS)
	@mkdir -p build
	$(LD) $(LDFLAGS) -o $@ $^

$(ISO): $(KERNEL) grub/grub.cfg
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL) $(ISO_DIR)/boot/
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $@ $(ISO_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(OBJS) $(KERNEL) $(ISO) $(ISO_DIR)

# Test with QEMU directly (fast)
run: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL)

# Test with GRUB ISO (real boot)
iso-run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

# Automated test that types "help" automatically
test-help: $(KERNEL)
	@echo "Testing NekoOS - will auto-type 'help' and exit after 10 seconds"
	echo -e "help\n" | timeout 10s qemu-system-i386 -kernel $(KERNEL) -monitor stdio 2>&1 | grep -A 20 "NekoOS" || true

# Debug with GDB
debug: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL) -S -s &
	sleep 1
	gdb $(KERNEL) -ex "target remote localhost:1234" -ex "break kernel_main" -ex "continue"

.PHONY: all clean run iso-run test-help debug