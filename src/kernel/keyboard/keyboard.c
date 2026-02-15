#include "keyboard.h"
#include "../io.h"

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

void keyboard_init(void) {
    while (inb(0x64) & 0x1) {
        inb(0x60);
    }
    last_char = 0;
    shift_pressed = 0;
}

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        }
        return;
    }
    
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    
    if (scancode < 128 && kbd_us[scancode] != 0) {
        char c = kbd_us[scancode];
        if (shift_pressed && c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        last_char = c;
    }
}

char keyboard_getchar(void) {
    char c = last_char;
    last_char = 0;
    return c;
}

uint8_t keyboard_get_scancode(void) {
    if (!(inb(0x64) & 0x1)) return 0;
    return inb(0x60);
}
