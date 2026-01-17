#include "framebuffer.h"
#include "desktop.h"

void gui_run(void) {
    // Clear to blue (test color)
    fb_clear(0xFF0000FF);  // Blue
    
    // Draw red square in center
    fb_fill_rect(400, 300, 200, 200, 0xFFFF0000);  // Red
    
    // Draw green border
    fb_draw_rect(0, 0, 1024, 768, 0xFF00FF00);  // Green
    
    // Draw yellow "Neko" text placeholder
    fb_fill_rect(500, 350, 100, 50, 0xFFFFFF00);  // Yellow
    
    // Should never return in real GUI
    // For now, just stay here
    while (1) {
        // TODO: GUI event loop
    }
}
