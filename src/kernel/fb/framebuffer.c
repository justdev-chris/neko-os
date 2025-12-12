#include "framebuffer.h"

static uint8_t *fb_ptr = 0;
static uint32_t fb_width = 0, fb_height = 0, fb_pitch = 0;

void fb_init(uint64_t addr, uint32_t width, uint32_t height, uint32_t pitch) {
    fb_ptr = (uint8_t*)addr;
    fb_width = width;
    fb_height = height;
    fb_pitch = pitch;
}

void fb_put_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (x >= fb_width || y >= fb_height || !fb_ptr) return;
    
    uint32_t offset = y * fb_pitch + x * 4;
    fb_ptr[offset] = b;     // Blue
    fb_ptr[offset + 1] = g; // Green
    fb_ptr[offset + 2] = r; // Red
    // offset + 3 is alpha (usually ignored)
}

void fb_clear(uint8_t r, uint8_t g, uint8_t b) {
    for (uint32_t y = 0; y < fb_height; y++) {
        for (uint32_t x = 0; x < fb_width; x++) {
            fb_put_pixel(x, y, r, g, b);
        }
    }
}

void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b) {
    for (uint32_t dy = 0; dy < h; dy++) {
        for (uint32_t dx = 0; dx < w; dx++) {
            fb_put_pixel(x + dx, y + dy, r, g, b);
        }
    }
}