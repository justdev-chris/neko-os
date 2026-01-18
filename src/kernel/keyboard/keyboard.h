#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
uint8_t keyboard_get_scancode(void);  // Make sure this is here!
char keyboard_getchar(void);

#endif
