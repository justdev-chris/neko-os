#include "multiboot.h"
#include "fb/framebuffer.h"

void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    // Parse multiboot tags to get framebuffer info
    parse_multiboot_tags(mb_info_addr);
    
    // Initialize framebuffer
    fb_init(fb_addr, fb_width, fb_height, fb_pitch);
    
    // Clear to dark blue
    fb_clear(30, 30, 50);
    
    // Draw a red rectangle
    fb_draw_rect(100, 100, 200, 100, 255, 0, 0);
    
    // Draw a green rectangle
    fb_draw_rect(400, 200, 150, 150, 0, 255, 0);
    
    // Hang forever
    while(1);
}