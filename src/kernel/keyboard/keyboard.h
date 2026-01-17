#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
uint8_t keyboard_get_scancode(void);
void keyboard_handle_scancode(uint8_t scancode);
char keyboard_getchar(void);

#endif