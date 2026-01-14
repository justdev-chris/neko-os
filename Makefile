iso: kernel.bin
    bash scripts/make-iso.sh

run-iso: iso
    qemu-system-i386 -cdrom nekoos.iso