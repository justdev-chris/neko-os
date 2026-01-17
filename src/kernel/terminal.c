#include "terminal.h"
#include "vga.h"
#include "keyboard.h"
#include "io.h"

#define MAX_INPUT 256

static char input_buffer[MAX_INPUT];
static size_t input_pos = 0;

void terminal_init(void) {
    vga_clear();
    vga_set_color(0x0F);
}

void terminal_putchar(char c) {
    vga_putchar(c);
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    while (*data) {
        terminal_putchar(*data++);
    }
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
    
    // Command processing
    if (strcmp(input_buffer, "help") == 0) {
        terminal_setcolor(0x0A);
        terminal_writestring("Available commands:\n");
        terminal_writestring("  help     - Show this help\n");
        terminal_writestring("  clear    - Clear screen\n");
        terminal_writestring("  neko     - Display cat\n");
        terminal_writestring("  echo     - Echo text\n");
        terminal_writestring("  reboot   - Reboot system\n");
        terminal_writestring("  color    - Change text color\n");
    } 
    else if (strcmp(input_buffer, "clear") == 0) {
        terminal_clear();
        terminal_setcolor(0x0E);
        terminal_writestring("NekoOS Terminal v0.1\n");
        terminal_writestring("Type 'help' for commands\n");
    }
    else if (strcmp(input_buffer, "neko") == 0) {
        terminal_setcolor(0x0F);
        terminal_writestring("   /\\_/\\\n");
        terminal_writestring("  ( o.o )\n");
        terminal_writestring("   > ^ <\n");
        terminal_writestring(" Neko~ desu!\n");
    }
    else if (strncmp(input_buffer, "echo ", 5) == 0) {
        terminal_setcolor(0x0B);
        terminal_writestring(input_buffer + 5);
        terminal_writestring("\n");
    }
    else if (strcmp(input_buffer, "reboot") == 0) {
        terminal_setcolor(0x0C);
        terminal_writestring("Rebooting...\n");
        outb(0x64, 0xFE);  // CPU reset
        while(1);
    }
    else if (strncmp(input_buffer, "color ", 6) == 0) {
        char *arg = input_buffer + 6;
        if (strcmp(arg, "red") == 0) vga_set_color(0x04);
        else if (strcmp(arg, "green") == 0) vga_set_color(0x02);
        else if (strcmp(arg, "blue") == 0) vga_set_color(0x01);
        else if (strcmp(arg, "yellow") == 0) vga_set_color(0x0E);
        else if (strcmp(arg, "white") == 0) vga_set_color(0x0F);
        else terminal_writestring("Unknown color\n");
    }
    else {
        terminal_setcolor(0x0C);
        terminal_writestring("Command not found: ");
        terminal_writestring(input_buffer);
        terminal_writestring("\n");
    }
}

void terminal_run_shell(void) {
    terminal_setcolor(0x0E);
    terminal_writestring("NekoOS Terminal v0.1\n");
    terminal_writestring("Type 'help' for commands\n");
    
    terminal_print_prompt();
    
    while (1) {
        uint8_t scancode = keyboard_get_scancode();
        if (scancode) {
            keyboard_handle_scancode(scancode);
            
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
}

// ============= STRING FUNCTIONS (NO LIBC AVAILABLE) =============

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void* memset(void* ptr, int value, size_t num) {
    unsigned char* p = ptr;
    while (num--) {
        *p++ = value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}
