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

// Convert RGB to BGR
static uint32_t rgb_to_bgr(uint32_t rgb) {
    return ((rgb & 0xFF) << 16) | (rgb & 0xFF00) | ((rgb >> 16) & 0xFF);
}

void fb_clear(uint32_t color) {
    uint32_t bgr_color = rgb_to_bgr(color);
    uint32_t pixels_per_row = fb.pitch / 4;  // 4 bytes per pixel (32bpp)
    
    for (uint32_t y = 0; y < fb.height; y++) {
        uint32_t* row = (uint32_t*)(fb.buffer + y * fb.pitch);
        for (uint32_t x = 0; x < fb.width; x++) {
            if (x < pixels_per_row) {
                row[x] = bgr_color;
            }
        }
    }
}

void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    uint32_t bgr_color = rgb_to_bgr(color);
    uint32_t pixels_per_row = fb.pitch / 4;
    
    // Top line
    if (y < fb.height) {
        uint32_t* top_row = (uint32_t*)(fb.buffer + y * fb.pitch);
        for (uint32_t i = x; i < x + w && i < fb.width && i < pixels_per_row; i++) {
            top_row[i] = bgr_color;
        }
    }
    
    // Bottom line
    if (y + h - 1 < fb.height) {
        uint32_t* bottom_row = (uint32_t*)(fb.buffer + (y + h - 1) * fb.pitch);
        for (uint32_t i = x; i < x + w && i < fb.width && i < pixels_per_row; i++) {
            bottom_row[i] = bgr_color;
        }
    }
    
    // Left and right lines
    for (uint32_t i = y; i < y + h && i < fb.height; i++) {
        uint32_t* row = (uint32_t*)(fb.buffer + i * fb.pitch);
        if (x < fb.width && x < pixels_per_row) {
            row[x] = bgr_color;
        }
        if (x + w - 1 < fb.width && x + w - 1 < pixels_per_row) {
            row[x + w - 1] = bgr_color;
        }
    }
}

void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    uint32_t bgr_color = rgb_to_bgr(color);
    uint32_t pixels_per_row = fb.pitch / 4;
    
    for (uint32_t py = y; py < y + h && py < fb.height; py++) {
        uint32_t* row = (uint32_t*)(fb.buffer + py * fb.pitch);
        for (uint32_t px = x; px < x + w && px < fb.width && px < pixels_per_row; px++) {
            row[px] = bgr_color;
        }
    }
}
