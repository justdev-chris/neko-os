#include <stdint.h>

uint8_t vga_make_color(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

uint16_t vga_make_entry(char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}
