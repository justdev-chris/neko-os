#include "framebuffer.h"
#include "desktop.h"

void gui_run(void) {
    // Clear to black first
    fb_clear(0xFF000000);
    
    // Draw four colored quadrants (visible test)
    fb_fill_rect(0, 0, 512, 384, 0xFFFF0000);      // RED - top left
    fb_fill_rect(512, 0, 512, 384, 0xFF00FF00);    // GREEN - top right
    fb_fill_rect(0, 384, 512, 384, 0xFF0000FF);    // BLUE - bottom left  
    fb_fill_rect(512, 384, 512, 384, 0xFFFFFF00);  // YELLOW - bottom right
    
    // White border
    fb_draw_rect(0, 0, 1024, 768, 0xFFFFFFFF);
    
    // White cross in center
    fb_fill_rect(462, 384, 100, 1, 0xFFFFFFFF);    // Horizontal
    fb_fill_rect(512, 334, 1, 100, 0xFFFFFFFF);    // Vertical
    
    // Hang forever
    while (1) {
        // GUI would handle events here
    }
}
