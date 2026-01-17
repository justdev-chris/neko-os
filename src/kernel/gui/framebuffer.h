#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

void fb_init(uint32_t addr, uint32_t width, uint32_t height,
             uint32_t bpp, uint32_t pitch);

#endif