// src/kernel/gui/desktop.c
#include "framebuffer.h"
#include "desktop.h"

void gui_run(void) {
    // Clear screen to dark gray
    fb_clear(0xFF333333);
    
    // Draw a window
    fb_draw_rect(100, 100, 400, 300, 0xFF0066CC);  // Blue border
    
    // Draw title bar (filled rectangle - need fb_fill_rect)
    // For now just outline
    fb_draw_rect(100, 100, 400, 30, 0xFF0099FF);   // Title bar border
    
    // Draw some text placeholder (would need font rendering)
    // TODO: Add text rendering
}