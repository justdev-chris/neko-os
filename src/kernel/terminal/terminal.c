#include "terminal.h"
#include "../vga.h"
#include "../keyboard/keyboard.h"
#include "../game/snake.h"
#include "../game/game.h"
#include "../io.h"
#include <stddef.h>

#define MAX_INPUT 256
static char input_buffer[MAX_INPUT];
static size_t input_pos = 0;

// strcmp implementation since we're using -nostdlib
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void terminal_init(void) {
    vga_clear();
    vga_set_color(0x0F);
}

void terminal_putchar(char c) {
    vga_putchar(c);
}

void terminal_writestring(const char* data) {
    vga_puts(data);
}

void terminal_setcolor(uint8_t color) {
    vga_set_color(color);
}

void terminal_clear(void) {
    vga_clear();
}

void terminal_print_prompt(void) {
    terminal_setcolor(0x0F);
    terminal_writestring("\nneko@os:~$ ");
    input_pos = 0;
}

void terminal_execute_command(void) {
    input_buffer[input_pos] = '\0';
    
    if (input_pos == 0) return;
    
    terminal_writestring("\n");
    
    if (strcmp(input_buffer, "help") == 0) {
        terminal_setcolor(0x0A);
        terminal_writestring("Available commands:\n");
        terminal_writestring("  help      - Show this help\n");
        terminal_writestring("  clear     - Clear screen\n");
        terminal_writestring("  snake     - Play Snake game\n");
        terminal_writestring("  neko game - Play Neko game\n");
        terminal_writestring("  neko      - Display cat\n");
        terminal_writestring("  reboot    - Reboot system\n");
    } 
    else if (strcmp(input_buffer, "clear") == 0) {
        terminal_clear();
        terminal_setcolor(0x0E);
        terminal_writestring("NekoOS Terminal\n");
    }
    else if (strcmp(input_buffer, "snake") == 0) {
        snake_run();
        terminal_clear();
    }
    else if (strcmp(input_buffer, "neko game") == 0) {
        run_neko_game();
        terminal_clear();
    }
    else if (strcmp(input_buffer, "neko") == 0) {
        terminal_setcolor(0x0F);
        terminal_writestring("   /\\_/\\\n");
        terminal_writestring("  ( o.o )\n");
        terminal_writestring("   > ^ <\n");
    }
    else if (strcmp(input_buffer, "reboot") == 0) {
        terminal_setcolor(0x0C);
        terminal_writestring("Rebooting...\n");
        outb(0x64, 0xFE);
        while(1);
    }
    else {
        terminal_setcolor(0x0C);
        terminal_writestring("Unknown command: ");
        terminal_writestring(input_buffer);
        terminal_writestring("\n");
    }
}

void terminal_run_shell(void) {
    terminal_print_prompt();
    
    // Simple keyboard test message
    terminal_setcolor(0x0E);
    terminal_writestring("(Type 'help' then press Enter)\n");
    terminal_setcolor(0x0F);
    
    while (1) {
        char c = keyboard_getchar();
        if (c) {
            if (c == '\n') {
                terminal_execute_command();
                terminal_print_prompt();
            } else if (c == '\b') {
                if (input_pos > 0) {
                    input_pos--;
                    terminal_putchar('\b');
                    terminal_putchar(' ');
                    terminal_putchar('\b');
                }
            } else if (input_pos < MAX_INPUT - 1) {
                input_buffer[input_pos++] = c;
                terminal_putchar(c);
            }
        }
    }
}
