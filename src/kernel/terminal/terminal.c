#include "terminal.h"
#include "../vga.h"
#include "../keyboard/keyboard.h"
#include "../game/snake.h"
#include "../game/game.h"
#include "../io.h"
#include "../timer/timer.h"
#include "../memory/memory.h"
#include <stddef.h>

#define MAX_INPUT 256
static char input_buffer[MAX_INPUT];
static size_t input_pos = 0;

static unsigned long system_seconds = 0;

extern int cursor_x;
extern int cursor_y;

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static void itoa(unsigned long num, char* str) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    while (num != 0) {
        str[i++] = '0' + (num % 10);
        num = num / 10;
    }
    str[i] = '\0';
    
    // Reverse
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

static void format_uptime(char* buffer) {
    unsigned long seconds = system_seconds;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;
    
    seconds %= 60;
    minutes %= 60;
    hours %= 24;
    
    char num[20];
    int pos = 0;
    
    if (days > 0) {
        itoa(days, num);
        for (int i = 0; num[i]; i++) buffer[pos++] = num[i];
        buffer[pos++] = 'd';
        buffer[pos++] = ' ';
    }
    
    itoa(hours, num);
    if (hours < 10) buffer[pos++] = '0';
    for (int i = 0; num[i]; i++) buffer[pos++] = num[i];
    buffer[pos++] = ':';
    
    itoa(minutes, num);
    if (minutes < 10) buffer[pos++] = '0';
    for (int i = 0; num[i]; i++) buffer[pos++] = num[i];
    buffer[pos++] = ':';
    
    itoa(seconds, num);
    if (seconds < 10) buffer[pos++] = '0';
    for (int i = 0; num[i]; i++) buffer[pos++] = num[i];
    
    buffer[pos] = '\0';
}

void terminal_writestring_color(const char* data, uint8_t color) {
    uint8_t old_color = vga_get_color();
    vga_set_color(color);
    vga_puts(data);
    vga_set_color(old_color);
}

void terminal_update_tick(void) {
    static int counter = 0;
    counter++;
    if (counter >= 100) {
        system_seconds++;
        counter = 0;
    }
}

void terminal_init(void) {
    vga_clear();
    vga_set_color(0x0F);
    system_seconds = 0;
    cursor_x = 0;
    cursor_y = 0;
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
    cursor_x = 0;
    cursor_y = 0;
}

void terminal_print_prompt(void) {
    terminal_setcolor(0x0F);
    terminal_writestring("neko@os:~$ ");
    input_pos = 0;
}

void terminal_panic(const char* message) {
    asm volatile("cli");  // Disable interrupts
    
    vga_set_color(0x1F);  // Blue background, white text
    vga_clear();
    
    vga_puts("\n\n\n");
    vga_set_color(0x1C);  // Red on blue
    vga_puts("    /\\_/\\\n");
    vga_puts("   ( x.x )  NEKOOS PANIC\n");
    vga_puts("    > ^ <\n\n");
    
    vga_set_color(0x1F);  // White on blue
    vga_puts("    ");
    vga_puts(message);
    vga_puts("\n\n");
    
    vga_set_color(0x1E);  // Yellow on blue
    vga_puts("    System halted.\n\n");
    
    // Halt forever
    while(1) {
        asm volatile("hlt");
    }
}

void terminal_screenfetch(void) {
    terminal_setcolor(0x0C);
    terminal_writestring("          /\\_/\\\n");
    terminal_setcolor(0x0E);
    terminal_writestring("         ( o.o )\n");
    terminal_setcolor(0x0A);
    terminal_writestring("          > ^ <\n\n");
    
    terminal_setcolor(0x0F);
    terminal_writestring("  OS: NekoOS\n");
    terminal_writestring("  Host: NekoPC\n");
    terminal_writestring("  Kernel: 0.1.4-neko\n");
    
    char uptime_str[50];
    format_uptime(uptime_str);
    terminal_writestring("  Uptime: ");
    terminal_writestring(uptime_str);
    terminal_writestring("\n");
}

void terminal_meminfo(void) {
    unsigned long total_mem = memory_get_total();
    unsigned long used_mem = memory_get_used();
    
    char num_str[20];
    
    terminal_writestring("Memory Information:\n");
    terminal_writestring("==================\n\n");
    
    terminal_writestring("Total Memory: ");
    itoa(total_mem, num_str);
    terminal_writestring(num_str);
    terminal_writestring(" bytes\n");
    
    terminal_writestring("Used Memory:  ");
    itoa(used_mem, num_str);
    terminal_writestring(num_str);
    terminal_writestring(" bytes\n");
}

void terminal_uptime(void) {
    char uptime_str[50];
    format_uptime(uptime_str);
    terminal_writestring("System uptime: ");
    terminal_writestring(uptime_str);
    terminal_writestring("\n");
}

void terminal_cat_clock(void) {
    char time_str[9];
    
    unsigned long seconds = system_seconds % 60;
    unsigned long minutes = (system_seconds / 60) % 60;
    unsigned long hours = (system_seconds / 3600) % 24;
    
    char num[3];
    int pos = 0;
    
    itoa(hours, num);
    if (hours < 10) time_str[pos++] = '0';
    for (int i = 0; num[i]; i++) time_str[pos++] = num[i];
    time_str[pos++] = ':';
    
    itoa(minutes, num);
    if (minutes < 10) time_str[pos++] = '0';
    for (int i = 0; num[i]; i++) time_str[pos++] = num[i];
    time_str[pos++] = ':';
    
    itoa(seconds, num);
    if (seconds < 10) time_str[pos++] = '0';
    for (int i = 0; num[i]; i++) time_str[pos++] = num[i];
    time_str[pos] = '\0';
    
    terminal_setcolor(0x0E);
    terminal_writestring(" =^.^= ");
    terminal_setcolor(0x0F);
    terminal_writestring(time_str);
    terminal_setcolor(0x0E);
    terminal_writestring(" =^.^=\n");
}

void terminal_execute_command(void) {
    input_buffer[input_pos] = '\0';
    
    if (input_pos == 0) {
        terminal_print_prompt();
        return;
    }
    
    terminal_writestring("\n");
    
    if (strcmp(input_buffer, "help") == 0) {
        terminal_writestring("Available commands:\n");
        terminal_writestring("  help      - Show this help\n");
        terminal_writestring("  clear     - Clear screen\n");
        terminal_writestring("  snake     - Play Snake game\n");
        terminal_writestring("  neko      - Display cat\n");
        terminal_writestring("  reboot    - Reboot system\n");
        terminal_writestring("  uptime    - Show system uptime\n");
        terminal_writestring("  catclock  - Display cat clock\n");
        terminal_writestring("  meminfo   - Show memory info\n");
        terminal_writestring("  screenfetch - Show system info\n");
        terminal_writestring("  panic     - Test panic screen\n");
    } 
    else if (strcmp(input_buffer, "clear") == 0) {
        terminal_clear();
    }
    else if (strcmp(input_buffer, "snake") == 0) {
        snake_run();
        terminal_clear();
    }
    else if (strcmp(input_buffer, "neko") == 0) {
        terminal_setcolor(0x0E);
        terminal_writestring("   /\\_/\\\n");
        terminal_writestring("  ( o.o )\n");
        terminal_writestring("   > ^ <\n");
    }
    else if (strcmp(input_buffer, "reboot") == 0) {
        terminal_writestring("Rebooting...\n");
        outb(0x64, 0xFE);
        while(1);
    }
    else if (strcmp(input_buffer, "uptime") == 0) {
        terminal_uptime();
    }
    else if (strcmp(input_buffer, "catclock") == 0) {
        terminal_cat_clock();
    }
    else if (strcmp(input_buffer, "meminfo") == 0) {
        terminal_meminfo();
    }
    else if (strcmp(input_buffer, "screenfetch") == 0) {
        terminal_screenfetch();
    }
    else if (strcmp(input_buffer, "panic") == 0) {
        terminal_panic("Test panic from command");
    }
    else {
        terminal_setcolor(0x0C);
        terminal_writestring("Unknown command: ");
        terminal_writestring(input_buffer);
        terminal_writestring("\n");
    }
    
    terminal_print_prompt();
}

void terminal_run_shell(void) {
    terminal_clear();
    terminal_setcolor(0x0E);
    terminal_writestring("NekoOS Terminal\n");
    terminal_setcolor(0x0F);
    terminal_writestring("Type 'help' for commands\n\n");
    terminal_print_prompt();
    
    // Declare timer_poll function
    extern void timer_poll(void);
    int poll_counter = 0;
    
    while (1) {
        // Poll timer manually every loop iteration
        poll_counter++;
        if (poll_counter >= 1000) {
            timer_poll();
            poll_counter = 0;
        }
        
        char c = keyboard_getchar();
        if (c) {
            if (c == '\n') {
                terminal_execute_command();
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
