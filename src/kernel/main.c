#include "vga.h"
#include "terminal.h"
#include "keyboard.h"
#include "io.h"
#include <stdint.h>

// NekoOS banner
void print_banner(void) {
    vga_set_color(0x0E); // Yellow
    vga_puts("  _   _      _      ___   ____\n");
    vga_puts(" | \\ | | ___| | __ / _ \\ / ___|\n");
    vga_puts(" |  \\| |/ _ \\ |/ /| | | |\\___ \\\n");
    vga_puts(" | |\\  |  __/   < | |_| |___) |\n");
    vga_puts(" |_| \\_|\\___|_|\\_\\ \\___/|____/\n\n");
    
    vga_set_color(0x0F); // White
    vga_puts("NekoOS v0.1.3 - Kernel loaded successfully!\n\n");
    
    vga_set_color(0x0A); // Green
    vga_puts("[OK] Memory manager\n");
    vga_puts("[OK] VGA text mode (80x25)\n");
    vga_puts("[OK] Interrupt handler\n");
    vga_puts("[OK] PS/2 Keyboard driver\n");
    vga_puts("[OK] Terminal shell\n\n");
    vga_puts("Updates to v0.1.3, New Game! ''neko game'' to play! \n\n");
}

void kernel_main(void) {
    // Initialize components
    vga_init();
    keyboard_init();
    terminal_init();
    
    // Show banner
    print_banner();
    
    vga_set_color(0x0F);
    vga_puts("System ready. ");
    vga_set_color(0x0C);
    vga_puts("meow~");
    vga_set_color(0x0F);
    vga_puts("\n\n");
    
    // Start the interactive terminal
    terminal_run_shell();
    
    // Should never reach here
    while (1) {
        asm volatile ("hlt");
    }
}
