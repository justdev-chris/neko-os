#include "vga.h"
#include "io.h"

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
int vga_width = 80;
int vga_height = 25;
int cursor_x = 0;
int cursor_y = 0;
uint8_t vga_color = 0x0F;

void vga_init(void) {
    vga_clear();
    // Disable cursor (optional)
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void vga_set_color(uint8_t color) {
    vga_color = color;
}

void vga_clear(void) {
    for (int i = 0; i < vga_width * vga_height; i++) {
        vga_buffer[i] = (vga_color << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

void vga_scroll(void) {
    // Move all lines up
    for (int y = 0; y < vga_height - 1; y++) {
        for (int x = 0; x < vga_width; x++) {
            vga_buffer[y * vga_width + x] = vga_buffer[(y + 1) * vga_width + x];
        }
    }
    
    // Clear bottom line
    for (int x = 0; x < vga_width; x++) {
        vga_buffer[(vga_height - 1) * vga_width + x] = (vga_color << 8) | ' ';
    }
    
    cursor_y = vga_height - 1;
}

void vga_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~(4 - 1);
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = vga_width - 1;
        }
        vga_buffer[cursor_y * vga_width + cursor_x] = (vga_color << 8) | ' ';
    } else {
        vga_buffer[cursor_y * vga_width + cursor_x] = (vga_color << 8) | c;
        cursor_x++;
    }
    
    if (cursor_x >= vga_width) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= vga_height) {
        vga_scroll();
    }
    
    // Update hardware cursor (optional)
    uint16_t pos = cursor_y * vga_width + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_puts(const char* str) {
    while (*str) {
        vga_putchar(*str++);
    }
}
