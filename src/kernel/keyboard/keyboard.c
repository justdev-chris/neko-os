#include "keyboard.h"
#include "io.h"

// US QWERTY scancode set 1
static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
    '7', '8', '9', '+', '4', '5', '6', '1', '2', '3', '0', '.', 0, 0, 0, 0
};

static uint8_t shift_pressed = 0;

void keyboard_init(void) {
    // Enable keyboard interrupts
    outb(0x21, 0xFD);  // Unmask IRQ1 (keyboard)
    
    // Clear any pending scancodes
    while (inb(0x64) & 0x1) {
        inb(0x60);
    }
    
    asm volatile("sti");  // Enable interrupts
}

// Add this back for game.c
uint8_t keyboard_get_scancode(void) {
    if (!(inb(0x64) & 0x1)) return 0;
    return inb(0x60);
}

char keyboard_getchar(void) {
    // Poll keyboard status
    if (!(inb(0x64) & 0x1)) {
        return 0;  // No data available
    }
    
    // Read scancode
    uint8_t scancode = inb(0x60);
    
    // Handle key release events
    if (scancode & 0x80) {
        scancode &= 0x7F;  // Clear release bit
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        }
        return 0;
    }
    
    // Handle key press events
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return 0;
    }
    
    // Convert scancode to character
    if (scancode < 128) {
        char c = kbd_us[scancode];
        
        // Apply shift for uppercase (simple version)
        if (shift_pressed && c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        
        return c;
    }
    
    return 0;
}
