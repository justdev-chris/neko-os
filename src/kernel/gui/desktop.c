#include "framebuffer.h"

void gui_run(void) {
    // Draw Neko-themed desktop
    fb_clear(0xFF333333);  // Dark gray background
    
    // Draw a window
    fb_draw_rect(100, 100, 400, 300, 0xFF0066CC);  // Blue border
    // Draw title bar
    fb_draw_rect(100, 100, 400, 30, 0xFF0099FF);   // Lighter blue
    
    // Draw neko cat in corner
    // (You'd add pixel art here)
    
    // TODO: Add mouse/keyboard input
}