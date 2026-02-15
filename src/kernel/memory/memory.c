#include "memory.h"

static unsigned long total_memory = 0;
static unsigned long used_memory = 0;

void memory_init(unsigned long mem_lower, unsigned long mem_upper) {
    total_memory = mem_upper - mem_lower;
    used_memory = 0;
}

unsigned long memory_get_total(void) {
    return total_memory;
}

unsigned long memory_get_used(void) {
    return used_memory;
}

// Simple placeholder implementations
void* malloc(unsigned long size) {
    used_memory += size;
    return (void*)0x100000;  // Placeholder address
}

void free(void* ptr) {
    // Placeholder - would need proper memory management
}
