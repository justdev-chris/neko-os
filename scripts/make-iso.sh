#!/bin/bash
set -e

mkdir -p isodir/boot/grub

# Use build/kernel.bin instead of kernel/kernel.bin
cp build/kernel.bin isodir/boot/nekoos.bin

cat > isodir/boot/grub/grub.cfg << EOF
set timeout=5
set default=0

menuentry "NekoOS" {
    multiboot /boot/nekoos.bin
    boot
}
EOF

grub-mkrescue -o nekoos.iso isodir
echo "ISO created: nekoos.iso"
