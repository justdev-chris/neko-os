#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;
    uint8_t* buffer;
} framebuffer_t;

// Initialize from Multiboot info
void fb_init(uint32_t addr, uint32_t width, uint32_t height, 
             uint32_t bpp, uint32_t pitch);

// Basic drawing
void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void fb_clear(uint32_t color);

// Get framebuffer info
framebuffer_t* fb_get_info(void);

#endif