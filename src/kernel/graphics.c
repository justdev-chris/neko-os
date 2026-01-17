#include <stdint.h>
#include "io.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define VRAM 0xA0000

static uint8_t original_mode = 0x03;  // Default: 80x25 text

void save_text_mode(void) {
    // Get current mode via BIOS (in bootloader)
    // Already saved in video_mode
}

void switch_to_graphics(void) {
    // Switch to VGA mode 13h
    asm volatile (
        "mov $0x13, %%ax\n"
        "int $0x10\n"
        : : : "eax"
    );
    
    // Clear screen to blue
    volatile uint8_t* vram = (uint8_t*)VRAM;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vram[i] = 1;  // Blue
    }
}

void switch_to_text(void) {
    // Switch back to text mode
    asm volatile (
        "mov $0x03, %%ax\n"
        "int $0x10\n"
        : : : "eax"
    );
}

void set_pixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        volatile uint8_t* vram = (uint8_t*)VRAM;
        vram[y * SCREEN_WIDTH + x] = color;
    }
}

void draw_cat(int x, int y) {
    // Body (white circle)
    for (int dy = -10; dy <= 10; dy++) {
        for (int dx = -10; dx <= 10; dx++) {
            if (dx*dx + dy*dy <= 100) {
                set_pixel(x + dx, y + dy, 15);  // White
            }
        }
    }
    
    // Eyes (black)
    set_pixel(x - 5, y - 5, 0);
    set_pixel(x + 5, y - 5, 0);
    
    // Nose (pink)
    set_pixel(x, y + 2, 13);
    set_pixel(x - 1, y + 2, 13);
    set_pixel(x + 1, y + 2, 13);
}
