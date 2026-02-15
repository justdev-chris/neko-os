#include "memory.h"

static unsigned long total_memory = 0;
static unsigned long used_memory = 0;
static unsigned long free_memory = 0;

// Simple memory placement address (start of heap)
static uint32_t placement_address = 0x100000;  // 1MB mark

void memory_init(unsigned long mem_lower, unsigned long mem_upper) {
    (void)mem_lower;  // Suppress unused parameter warning
    total_memory = mem_upper;
    used_memory = 0;
    free_memory = total_memory;
    placement_address = 0x100000;
}

unsigned long memory_get_total(void) {
    return total_memory;
}

unsigned long memory_get_used(void) {
    return used_memory;
}

// Very simple malloc implementation
void* malloc(unsigned long size) {
    // Align size to 4 bytes
    if (size % 4 != 0) {
        size += 4 - (size % 4);
    }
    
    void* ret = (void*)placement_address;
    placement_address += size;
    used_memory += size;
    free_memory -= size;
    
    return ret;
}

void free(void* ptr) {
    // Simple implementation doesn't support freeing
    // In a real OS, you'd have a proper memory manager
    (void)ptr;  // Avoid unused parameter warning
}
