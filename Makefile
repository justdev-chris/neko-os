# Toolchain
AS = nasm
CC = x86_64-elf-gcc
LD = x86_64-elf-ld
GRUB = grub-mkrescue

# Flags
ASFLAGS = -f elf64
CFLAGS = -ffreestanding -mno-red-zone -mcmodel=large -I src/kernel
LDFLAGS = -nostdlib -T src/linker.ld

# Output files
KERNEL = kernel.bin
ISO = nekoos.iso

# Build rules
all: $(ISO)

$(ISO): $(KERNEL)
    mkdir -p iso/boot/grub
    cp $(KERNEL) iso/boot/
    cp grub/grub.cfg iso/boot/grub/
    $(GRUB) -o $(ISO) iso

$(KERNEL): src/boot/boot.o src/kernel/main.o src/kernel/vga.o
    $(LD) $(LDFLAGS) $^ -o $@

%.o: %.asm
    $(AS) $(ASFLAGS) $< -o $@

%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    rm -f *.o $(KERNEL) $(ISO)
    rm -rf iso

run: $(ISO)
    qemu-system-x86_64 -cdrom $(ISO)

.PHONY: all clean run