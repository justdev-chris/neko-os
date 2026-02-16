#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

void terminal_init(void);
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_writestring_color(const char* data, uint8_t color);
void terminal_setcolor(uint8_t color);
void terminal_clear(void);
void terminal_print_prompt(void);
void terminal_execute_command(void);
void terminal_run_shell(void);
void terminal_update_tick(void);
void terminal_panic(const char* message);
void terminal_screenfetch(void);
void terminal_meminfo(void);
void terminal_uptime(void);
void terminal_cat_clock(void);

#endif
