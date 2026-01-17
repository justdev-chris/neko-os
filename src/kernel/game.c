#include "graphics.h"
#include "keyboard.h"

void run_neko_game(void) {
    switch_to_graphics();
    
    int cat_x = 160, cat_y = 100;
    int running = 1;
    
    terminal_writestring("Neko Game! Use WASD, ESC to quit\n");
    terminal_writestring("Graphics mode active...\n");
    
    while (running) {
        // Draw
        // Clear screen blue
        for (int y = 0; y < 200; y++) {
            for (int x = 0; x < 320; x++) {
                set_pixel(x, y, 1);  // Blue
            }
        }
        
        draw_cat(cat_x, cat_y);
        
        // Handle input
        uint8_t scancode = keyboard_get_scancode();
        if (scancode) {
            keyboard_handle_scancode(scancode);
            char c = keyboard_getchar();
            
            if (c == 27) {  // ESC
                running = 0;
            } else if (c == 'w' || c == 'W') {
                cat_y -= 5;
            } else if (c == 's' || c == 'S') {
                cat_y += 5;
            } else if (c == 'a' || c == 'A') {
                cat_x -= 5;
            } else if (c == 'd' || c == 'D') {
                cat_x += 5;
            }
        }
        
        // Simple delay
        for (volatile int i = 0; i < 100000; i++);
    }
    
    switch_to_text();
    terminal_writestring("Returned to terminal mode.\n");
}
