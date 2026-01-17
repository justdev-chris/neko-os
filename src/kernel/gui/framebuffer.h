#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

void fb_init(uint32_t addr, uint32_t width, uint32_t height,
             uint32_t bpp, uint32_t pitch);
void fb_clear(uint32_t color);
void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

#endif