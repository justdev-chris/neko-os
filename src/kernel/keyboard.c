// SIMPLIFIED VERSION - Just get characters
#include <stdint.h>

char keyboard_read(void) {
    while (1) {
        // Wait for keyboard buffer to have data
        while ((inb(0x64) & 1) == 0);
        
        uint8_t scancode = inb(0x60);
        
        // Simple scancode to ASCII (just lowercase letters for now)
        if (scancode < 0x80) {  // Key press (not release)
            if (scancode == 0x1C) return '\n';  // Enter
            if (scancode == 0x0E) return '\b';  // Backspace
            if (scancode >= 0x10 && scancode <= 0x1C) {
                // qwertyuiop[]
                return "qwertyuiop[]"[scancode - 0x10];
            }
            if (scancode >= 0x1E && scancode <= 0x26) {
                // asdfghjkl;'
                return "asdfghjkl;'"[scancode - 0x1E];
            }
            if (scancode >= 0x2C && scancode <= 0x32) {
                // zxcvbnm,./
                return "zxcvbnm,./"[scancode - 0x2C];
            }
            if (scancode == 0x39) return ' ';  // Space
        }
    }
}
