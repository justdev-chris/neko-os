#include "keyboard.h"
#include "../io.h"

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
static volatile char last_char = 0;  // Made volatile for interrupt safety
static volatile uint8_t char_available = 0;  // Flag for new character

void keyboard_init(void) {
    // Enable keyboard interrupts
    outb(0x21, 0xFD);  // Unmask IRQ1 (keyboard)
    
    // Clear any pending scancodes
    while (inb(0x64) & 0x1) {
        inb(0x60);
    }
    
    last_char = 0;
    char_available = 0;
    shift_pressed = 0;
    
    asm volatile("sti");  // Enable interrupts
}

// Interrupt handler - called from IRQ1
void keyboard_handler(void) {
    // Read scancode (this also acknowledges the interrupt to the keyboard)
    uint8_t scancode = inb(0x60);
    
    // Handle key release events
    if (scancode & 0x80) {
        scancode &= 0x7F;  // Clear release bit
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        }
        return;
    }
    
    // Handle key press events
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    
    // Convert scancode to character
    if (scancode < 128) {
        char c = kbd_us[scancode];
        
        // Apply shift for uppercase (simple version)
        if (shift_pressed && c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        } else if (shift_pressed) {
            // Handle shifted symbols (you can expand this)
            switch(c) {
                case '1': c = '!'; break;
                case '2': c = '@'; break;
                case '3': c = '#'; break;
                case '4': c = '$'; break;
                case '5': c = '%'; break;
                case '6': c = '^'; break;
                case '7': c = '&'; break;
                case '8': c = '*'; break;
                case '9': c = '('; break;
                case '0': c = ')'; break;
                case '-': c = '_'; break;
                case '=': c = '+'; break;
                case '[': c = '{'; break;
                case ']': c = '}'; break;
                case '\\': c = '|'; break;
                case ';': c = ':'; break;
                case '\'': c = '"'; break;
                case ',': c = '<'; break;
                case '.': c = '>'; break;
                case '/': c = '?'; break;
                case '`': c = '~'; break;
            }
        }
        
        // Store the character for the terminal to read
        if (c != 0) {
            last_char = c;
            char_available = 1;
        }
    }
}

// Polling function for terminal (non-blocking)
char keyboard_getchar(void) {
    if (char_available) {
        char_available = 0;
        return last_char;
    }
    return 0;
}

// For compatibility with game.c (if needed)
uint8_t keyboard_get_scancode(void) {
    if (!(inb(0x64) & 0x1)) return 0;
    return inb(0x60);
}
