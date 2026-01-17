#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>    // ADD THIS
#include <stddef.h>    // ADD THIS

void terminal_init(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_setcolor(uint8_t color);
void terminal_clear(void);
void terminal_run_shell(void);

#endif