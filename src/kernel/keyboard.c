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

static const char kbd_us_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
    '7', '8', '9', '+', '4', '5', '6', '1', '2', '3', '0', '.', 0, 0, 0, 0
};

static uint8_t shift_pressed = 0;
static uint8_t ctrl_pressed = 0;
static uint8_t alt_pressed = 0;
static uint8_t caps_lock = 0;

// Circular buffer for keyboard input
#define KEY_BUFFER_SIZE 256
static char key_buffer[KEY_BUFFER_SIZE];
static uint16_t buffer_start = 0;
static uint16_t buffer_end = 0;

void keyboard_init(void) {
    // Disable keyboard
    outb(0x64, 0xAD);
    
    // Wait for controller to be ready
    while (inb(0x64) & 0x2);
    
    // Set scan code set 1
    outb(0x64, 0xF0);
    outb(0x60, 0x01);
    
    // Enable keyboard
    outb(0x64, 0xAE);
}

uint8_t keyboard_get_scancode(void) {
    if (!(inb(0x64) & 0x1)) {
        return 0;  // No data available
    }
    return inb(0x60);
}

void keyboard_handle_scancode(uint8_t scancode) {
    if (scancode & 0x80) {
        // Key released
        scancode &= 0x7F;
        
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        } else if (scancode == 0x1D) {
            ctrl_pressed = 0;
        } else if (scancode == 0x38) {
            alt_pressed = 0;
        }
    } else {
        // Key pressed
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
        } else if (scancode == 0x1D) {
            ctrl_pressed = 1;
        } else if (scancode == 0x38) {
            alt_pressed = 1;
        } else if (scancode == 0x3A) {
            caps_lock = !caps_lock;
        } else {
            char c = 0;
            
            if (shift_pressed ^ caps_lock) {
                c = kbd_us_shift[scancode];
            } else {
                c = kbd_us[scancode];
            }
            
            // Handle special keys
            if (scancode == 0x0E) {  // Backspace
                c = '\b';
            } else if (scancode == 0x1C) {  // Enter
                c = '\n';
            } else if (scancode == 0x39) {  // Space
                c = ' ';
            }
            
            if (c != 0) {
                uint16_t next_end = (buffer_end + 1) % KEY_BUFFER_SIZE;
                if (next_end != buffer_start) {
                    key_buffer[buffer_end] = c;
                    buffer_end = next_end;
                }
            }
        }
    }
}

char keyboard_getchar(void) {
    if (buffer_start == buffer_end) {
        return 0;  // Buffer empty
    }
    
    char c = key_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEY_BUFFER_SIZE;
    return c;
}

void keyboard_wait_for_enter(void) {
    while (1) {
        uint8_t scancode = keyboard_get_scancode();
        if (scancode) {
            keyboard_handle_scancode(scancode);
            char c = keyboard_getchar();
            if (c == '\n') {
                return;
            }
        }
    }
}
