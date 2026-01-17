VERSION = 0.1.4
CC = gcc
AS = nasm
LD = ld

SRC_DIR = src/kernel
BUILD_DIR = build
ISO_DIR = isodir

KERNEL_SRCS = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/vga.c \
	$(SRC_DIR)/terminal/terminal.c \
	$(SRC_DIR)/keyboard/keyboard.c \
	$(SRC_DIR)/game/game.c \
	$(SRC_DIR)/game/snake.c

BOOT_SRCS = \
	src/boot/multiboot.asm \
	$(SRC_DIR)/io.asm

C_OBJS = $(KERNEL_SRCS:.c=.o)
ASM_OBJS = $(BOOT_SRCS:.asm=.o)
ALL_OBJS = $(C_OBJS) $(ASM_OBJS)

KERNEL_TARGET = $(BUILD_DIR)/kernel.bin
ISO_TARGET = nekoos.iso

CFLAGS = -m32 -ffreestanding -nostdlib -Wall -Wextra -O2 \
         -I./src/kernel \
         -I./src/kernel/terminal \
         -I./src/kernel/keyboard \
         -I./src/kernel/game

ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

all: $(KERNEL_TARGET) $(ISO_TARGET)

$(KERNEL_TARGET): $(ALL_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(ISO_TARGET): $(KERNEL_TARGET) grub/grub.cfg
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_TARGET) $(ISO_DIR)/boot/
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $@ $(ISO_DIR)

%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(ALL_OBJS) $(KERNEL_TARGET) $(ISO_TARGET) $(ISO_DIR)

run: $(KERNEL_TARGET)
	qemu-system-i386 -kernel $(KERNEL_TARGET)

iso-run: $(ISO_TARGET)
	qemu-system-i386 -cdrom $(ISO_TARGET)