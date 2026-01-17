#include "framebuffer.h"
#include <stdint.h>

static struct {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;
    uint8_t* buffer;
} fb;

void fb_init(uint32_t addr, uint32_t width, uint32_t height,
             uint32_t bpp, uint32_t pitch) {
    fb.buffer = (uint8_t*)addr;
    fb.width = width;
    fb.height = height;
    fb.bpp = bpp;
    fb.pitch = pitch;
}

// Convert RGB to BGR (framebuffer often uses BGR)
static uint32_t rgb_to_bgr(uint32_t rgb) {
    return ((rgb & 0xFF) << 16) |        // R -> B
           (rgb & 0xFF00) |              // G stays
           ((rgb >> 16) & 0xFF);         // B -> R
}

void fb_clear(uint32_t color) {
    uint32_t bgr_color = rgb_to_bgr(color);
    
    for (uint32_t y = 0; y < fb.height; y++) {
        for (uint32_t x = 0; x < fb.width; x++) {
            uint32_t offset = y * fb.pitch + x * 4;
            uint32_t* pixel = (uint32_t*)(fb.buffer + offset);
            *pixel = bgr_color;
        }
    }
}

void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    uint32_t bgr_color = rgb_to_bgr(color);
    
    // Top and bottom lines
    for (uint32_t i = x; i < x + w; i++) {
        uint32_t top_offset = y * fb.pitch + i * 4;
        uint32_t* top_pixel = (uint32_t*)(fb.buffer + top_offset);
        *top_pixel = bgr_color;
        
        uint32_t bottom_offset = (y + h - 1) * fb.pitch + i * 4;
        uint32_t* bottom_pixel = (uint32_t*)(fb.buffer + bottom_offset);
        *bottom_pixel = bgr_color;
    }
    
    // Left and right lines
    for (uint32_t i = y; i < y + h; i++) {
        uint32_t left_offset = i * fb.pitch + x * 4;
        uint32_t* left_pixel = (uint32_t*)(fb.buffer + left_offset);
        *left_pixel = bgr_color;
        
        uint32_t right_offset = i * fb.pitch + (x + w - 1) * 4;
        uint32_t* right_pixel = (uint32_t*)(fb.buffer + right_offset);
        *right_pixel = bgr_color;
    }
}

void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    uint32_t bgr_color = rgb_to_bgr(color);
    
    for (uint32_t py = y; py < y + h; py++) {
        for (uint32_t px = x; px < x + w; px++) {
            uint32_t offset = py * fb.pitch + px * 4;
            uint32_t* pixel = (uint32_t*)(fb.buffer + offset);
            *pixel = bgr_color;
        }
    }
}
