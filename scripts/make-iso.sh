#!/bin/bash
set -e

echo "=== Creating NekoOS ISO ==="

mkdir -p isodir/boot/grub

echo "Copying kernel..."
cp build/kernel.bin isodir/boot/nekoos.bin

echo "Creating GRUB config..."
cat > isodir/boot/grub/grub.cfg << 'EOF'
set timeout=3
set default=0

menuentry "NekoOS" {
    multiboot /boot/nekoos.bin
    boot
}

menuentry "NekoOS (verbose)" {
    multiboot /boot/nekoos.bin
    boot
}

menuentry "Reboot" {
    reboot
}

menuentry "Shutdown" {
    halt
}
EOF

echo "Building ISO..."
if command -v grub-mkrescue &> /dev/null; then
    grub-mkrescue -o nekoos.iso isodir
else
    xorriso -as mkisofs -R -b boot/grub/i386-pc/eltorito.img \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        -o nekoos.iso isodir
fi

echo "✅ ISO created: nekoos.iso"
