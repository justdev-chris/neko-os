#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

void fb_init(uint64_t addr, uint32_t width, uint32_t height, uint32_t pitch);
void fb_put_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void fb_clear(uint8_t r, uint8_t g, uint8_t b);
void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b);

#endif