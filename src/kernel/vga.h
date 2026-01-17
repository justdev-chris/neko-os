#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_init(void);
void vga_clear(void);
void vga_putchar(char c);
void vga_puts(const char* str);
void vga_set_color(uint8_t color);
void vga_putchar_at(int x, int y, char c);
void vga_puts_at(int x, int y, const char* str);

#endif