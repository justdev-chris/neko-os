#include "multiboot.h"

uint64_t fb_addr = 0;
uint32_t fb_width = 0, fb_height = 0, fb_pitch = 0;

void parse_multiboot_tags(uint32_t addr) {
    struct multiboot_tag *tag;
    uint32_t size = *(uint32_t*)addr;
    
    for (tag = (struct multiboot_tag*)(addr + 8);
         tag->type != 0;
         tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
        
        if (tag->type == 8) { // Framebuffer tag
            struct multiboot_tag_framebuffer *fb_tag = (struct multiboot_tag_framebuffer*)tag;
            fb_addr = fb_tag->framebuffer_addr;
            fb_width = fb_tag->framebuffer_width;
            fb_height = fb_tag->framebuffer_height;
            fb_pitch = fb_tag->framebuffer_pitch;
            break;
        }
    }
}