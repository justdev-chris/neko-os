#include "memory.h"

static unsigned long total_memory = 0;
static unsigned long used_memory = 0;
static uint32_t placement_address = 0x100000;

void memory_init(unsigned long mem_lower, unsigned long mem_upper) {
    (void)mem_lower;
    total_memory = mem_upper;
    used_memory = 0;
    placement_address = 0x100000;
}

unsigned long memory_get_total(void) {
    return total_memory;
}

unsigned long memory_get_used(void) {
    return used_memory;
}

void* malloc(unsigned long size) {
    if (size % 4 != 0) {
        size += 4 - (size % 4);
    }
    
    void* ret = (void*)placement_address;
    placement_address += size;
    used_memory += size;
    
    return ret;
}

void free(void* ptr) {
    (void)ptr;
}
