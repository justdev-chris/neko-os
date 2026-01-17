#!/bin/bash
echo "=== Building NekoOS ==="
make clean
make

echo -e "\n=== Testing Multiboot header ==="
objdump -s -j .multiboot build/kernel.bin

echo -e "\n=== Testing in QEMU (5 seconds) ==="
echo "Type 'help' in the terminal when it loads..."
timeout 5s make run || true

echo -e "\n=== Testing ISO boot ==="
make iso-run &
sleep 5
killall qemu-system-i386 2>/dev/null

echo -e "\n=== Done ==="