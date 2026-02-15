#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
uint8_t keyboard_get_scancode(void);
char keyboard_getchar(void);
void keyboard_handler(void);  // Add this for IRQ1 handler

#endif
