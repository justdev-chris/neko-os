#ifndef MEMORY_H
#define MEMORY_H

// Initialize memory management
void memory_init(unsigned long mem_lower, unsigned long mem_upper);

// Get total memory in bytes
unsigned long memory_get_total(void);

// Get used memory in bytes
unsigned long memory_get_used(void);

// Allocate memory
void* malloc(unsigned long size);

// Free memory
void free(void* ptr);

#endif
