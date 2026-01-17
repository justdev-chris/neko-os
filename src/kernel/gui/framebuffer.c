#include "framebuffer.h"

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