// src/kernel/multiboot.c
#include <stdint.h>

// Multiboot header must be in the first 8KB of the kernel
__attribute__((section(".multiboot")))
const uint32_t multiboot_header[] = {
    // Magic number
    0x1BADB002,
    // Flags
    0x00000003,  // ALIGN + MEMINFO
    // Checksum
    -(0x1BADB002 + 0x00000003),
    // Header address
    0x00000000,
    // Load address  
    0x00000000,
    // Load end address
    0x00000000,
    // BSS end address
    0x00000000,
    // Entry address
    0x00000000,
    // Video mode
    0x00000000,
    // Width
    0x00000000,
    // Height
    0x00000000,
    // Depth
    0x00000000
};

// Simple function to parse multiboot info (optional)
void parse_multiboot_tags(uint32_t addr) {
    // Your existing code here
    uint32_t size = *(uint32_t*)addr;
    (void)size;  // Mark as used to avoid warning
    // ... rest of your code
}
