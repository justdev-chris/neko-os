#include "vga.h"

void kernel_main(unsigned long magic, unsigned long mb_info) {
    vga_clear();
    vga_print("NEKOOS", 37, 12);
    vga_print("Phase 1: Booted!", 0, 1);

    while (1) {
        asm volatile ("hlt");
    }
}