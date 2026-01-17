#include "keyboard.h"
#include ".../io.h"

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
#define KEY_BUFFER_SIZE 256
static char key_buffer[KEY_BUFFER_SIZE];
static uint16_t buffer_start = 0;
static uint16_t buffer_end = 0;

void keyboard_init(void) {
    // Disable keyboard
    outb(0x64, 0xAD);
    // Enable keyboard
    outb(0x64, 0xAE);
}

uint8_t keyboard_get_scancode(void) {
    if (!(inb(0x64) & 0x1)) return 0;
    return inb(0x60);
}

void keyboard_handle_scancode(uint8_t scancode) {
    if (scancode & 0x80) {
        // Key released
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) shift_pressed = 0;
    } else {
        // Key pressed
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
        } else {
            char c = kbd_us[scancode];
            if (c) {
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
    if (buffer_start == buffer_end) return 0;
    char c = key_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEY_BUFFER_SIZE;
    return c;
}
