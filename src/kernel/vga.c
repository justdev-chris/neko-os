#include "vga.h"

volatile unsigned short *vga_buffer = (volatile unsigned short*)0xB8000;
const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;

void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (unsigned short)0x0F00;
    }
}

void vga_print(const char *str, int x, int y) {
    int index = y * VGA_WIDTH + x;
    for (int i = 0; str[i] != '\0'; i++) {
        vga_buffer[index + i] = (unsigned short)(0x0F00 | str[i]);
    }
}