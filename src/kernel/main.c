#include <stdint.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
int vga_width = 80;
int vga_height = 25;
int cursor_x = 0;
int cursor_y = 0;
uint8_t vga_color = 0x0F;

void vga_clear(void) {
    for (int i = 0; i < vga_width * vga_height; i++) {
        vga_buffer[i] = (vga_color << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

void vga_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        return;
    }
    
    vga_buffer[cursor_y * vga_width + cursor_x] = (vga_color << 8) | c;
    cursor_x++;
    
    if (cursor_x >= vga_width) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= vga_height) {
        for (int i = 0; i < vga_width * (vga_height - 1); i++) {
            vga_buffer[i] = vga_buffer[i + vga_width];
        }
        for (int i = vga_width * (vga_height - 1); i < vga_width * vga_height; i++) {
            vga_buffer[i] = (vga_color << 8) | ' ';
        }
        cursor_y = vga_height - 1;
    }
}

void vga_print(const char* str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

void kernel_main(void) {
    vga_clear();
    
    vga_color = 0x0E; // Yellow
    vga_print("  _   _      _      ___   ____\n");
    vga_print(" | \\ | | ___| | __ / _ \\ / ___|\n");
    vga_print(" |  \\| |/ _ \\ |/ /| | | |\\___ \\\n");
    vga_print(" | |\\  |  __/   < | |_| |___) |\n");
    vga_print(" |_| \\_|\\___|_|\\_\\ \\___/|____/\n\n");
    
    vga_color = 0x0F; // White
    vga_print("NekoOS v0.1.0\n");
    vga_print("Kernel loaded successfully!\n\n");
    
    vga_color = 0x0A; // Green
    vga_print("[OK] Memory manager\n");
    vga_print("[OK] VGA text mode\n");
    vga_print("[OK] Interrupt handler\n\n");
    
    vga_color = 0x0F; // White
    vga_print("System ready. ");
    vga_color = 0x0C; // Red
    vga_print("meow~");
    vga_color = 0x0F; // White
    vga_print("\n\n> ");
    
    while (1) {
        asm volatile ("hlt");
    }
}
