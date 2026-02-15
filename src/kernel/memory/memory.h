#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

void memory_init(unsigned long mem_lower, unsigned long mem_upper);
unsigned long memory_get_total(void);
unsigned long memory_get_used(void);
void* malloc(unsigned long size);
void free(void* ptr);

#endif
