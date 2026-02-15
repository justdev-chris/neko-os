#include "keyboard.h"
#include "../io.h"
#include "../vga.h"

static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
    '7', '8', '9', '+', '4', '5', '6', '1', '2', '3', '0', '.', 0, 0, 0, 0
};

static uint8_t shift_pressed = 0;
static volatile char last_char = 0;
static int use_polling = 0;  // Start with interrupts

void keyboard_init(void) {
    vga_puts("  Keyboard init: Starting...\n");
    
    // Try to enable keyboard interrupts
    outb(0x21, 0xFD);  // Unmask IRQ1
    
    // Clear any pending data
    int cleared = 0;
    while (inb(0x64) & 0x1) {
        inb(0x60);
        cleared++;
    }
    
    vga_puts("  Keyboard init: Cleared ");
    vga_putchar('0' + cleared);
    vga_puts(" bytes\n");
    
    last_char = 0;
    shift_pressed = 0;
    
    vga_puts("  Keyboard init: Done!\n");
}

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    
    // Handle key release
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        }
        return;
    }
    
    // Handle shift press
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    
    // Convert to char
    if (scancode < 128 && kbd_us[scancode] != 0) {
        char c = kbd_us[scancode];
        if (shift_pressed && c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        last_char = c;
    }
}

// Polling fallback for when interrupts aren't working
static char keyboard_poll(void) {
    if (!(inb(0x64) & 0x1)) return 0;
    
    uint8_t scancode = inb(0x60);
    
    // Handle key release
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        }
        return 0;
    }
    
    // Handle shift press
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return 0;
    }
    
    // Convert to char
    if (scancode < 128 && kbd_us[scancode] != 0) {
        char c = kbd_us[scancode];
        if (shift_pressed && c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        return c;
    }
    
    return 0;
}

char keyboard_getchar(void) {
    // First try interrupt mode
    char c = last_char;
    if (c != 0) {
        last_char = 0;
        return c;
    }
    
    // If no interrupt chars, try polling as fallback
    return keyboard_poll();
}

uint8_t keyboard_get_scancode(void) {
    if (!(inb(0x64) & 0x1)) return 0;
    return inb(0x60);
}
