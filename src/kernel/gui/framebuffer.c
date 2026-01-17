// src/kernel/gui/framebuffer.c
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

// ADD THESE FUNCTIONS:
void fb_clear(uint32_t color) {
    // Fill entire framebuffer with color
    for (uint32_t y = 0; y < fb.height; y++) {
        for (uint32_t x = 0; x < fb.width; x++) {
            // Calculate pixel position (simplified - assumes 32bpp)
            uint32_t offset = y * fb.pitch + x * 4;
            uint32_t* pixel = (uint32_t*)(fb.buffer + offset);
            *pixel = color;
        }
    }
}

void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    // Draw rectangle outline
    for (uint32_t i = x; i < x + w; i++) {
        // Top line
        uint32_t offset_top = y * fb.pitch + i * 4;
        uint32_t* pixel_top = (uint32_t*)(fb.buffer + offset_top);
        *pixel_top = color;
        
        // Bottom line
        uint32_t offset_bottom = (y + h - 1) * fb.pitch + i * 4;
        uint32_t* pixel_bottom = (uint32_t*)(fb.buffer + offset_bottom);
        *pixel_bottom = color;
    }
    
    for (uint32_t i = y; i < y + h; i++) {
        // Left line
        uint32_t offset_left = i * fb.pitch + x * 4;
        uint32_t* pixel_left = (uint32_t*)(fb.buffer + offset_left);
        *pixel_left = color;
        
        // Right line
        uint32_t offset_right = i * fb.pitch + (x + w - 1) * 4;
        uint32_t* pixel_right = (uint32_t*)(fb.buffer + offset_right);
        *pixel_right = color;
    }
}