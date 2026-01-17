#include "vga.h"
#include "keyboard/keyboard.h"
#include "terminal/terminal.h"
#include "game/game.h"
#include "gui/framebuffer.h"
#include "gui/desktop.h"
#include <stdint.h>

// ==================== GLOBAL STATE ====================
static int gui_mode = 0;  // 0 = text, 1 = GUI

// ==================== CHANGELOG ====================
void show_changelog(void) {
    vga_set_color(0x0E);
    vga_puts("\nNekoOS v0.1.4\n");
    vga_puts("-------------\n");
    vga_set_color(0x0F);
    vga_puts("- Added GUI framework\n");
    vga_puts("- Fixed folder structure\n");
    vga_puts("- Fixed Multiboot header\n");
    vga_puts("- Added terminal commands\n");
    vga_puts("- Added keyboard driver\n");
    vga_puts("- Added VGA text mode\n");
}

// ==================== VERSION ====================
void show_version(void) {
    vga_set_color(0x0E);
    vga_puts("\nNekoOS v0.1.4\n");
    vga_set_color(0x0F);
    vga_puts("Type 'help' for commands\n");
}

// ==================== MULTIBOOT STRUCT ====================
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t reserved;
} __attribute__((packed));

// ==================== BANNER ====================
void print_banner(void) {
    vga_set_color(0x0E);
    vga_puts("  _   _      _      ___   ____\n");
    vga_puts(" | \\ | | ___| | __ / _ \\ / ___|\n");
    vga_puts(" |  \\| |/ _ \\ |/ /| | | |\\___ \\\n");
    vga_puts(" | |\\  |  __/   < | |_| |___) |\n");
    vga_puts(" |_| \\_|\\___|_|\\_\\ \\___/|____/\n\n");
    
    vga_set_color(0x0F);
    vga_puts("NekoOS v0.1.4\n\n");
    
    vga_set_color(0x0A);
    vga_puts("[OK] VGA text mode\n");
    vga_puts("[OK] Keyboard driver\n");
    vga_puts("[OK] Terminal shell\n");
    vga_puts("[..] GUI framework\n");
    vga_puts("[OK] Neko game\n\n");
}

// ==================== GUI DETECTION ====================
int detect_gui(struct multiboot_info* mb) {
    if (mb->flags & (1 << 12)) {
        if (mb->framebuffer_type == 1) {
            return 1;
        }
    }
    return 0;
}

// ==================== TEXT MODE ====================
void run_text_mode(void) {
    gui_mode = 0;
    vga_set_color(0x0F);
    vga_puts("Text mode\n\n");
    keyboard_init();
    terminal_run_shell();
}

// ==================== GUI MODE ====================
void run_gui_mode(struct multiboot_info* mb) {
    gui_mode = 1;
    
    // Initialize framebuffer
    fb_init(mb->framebuffer_addr,
            mb->framebuffer_width,
            mb->framebuffer_height,
            mb->framebuffer_bpp,
            mb->framebuffer_pitch);
    
    // Start GUI
    gui_run();
    
    // GUI event loop
    keyboard_init();
    while (gui_mode) {
        // Check for 'g' key to switch back to text
        char c = keyboard_getchar();
        if (c == 'g' || c == 'G') {
            // Switch back to text mode
            gui_mode = 0;
            
            // Reinitialize VGA text mode
            vga_init();
            vga_set_color(0x0C);
            vga_puts("\n\nSwitching to text mode...\n");
            vga_set_color(0x0F);
            vga_puts("Press any key to continue\n");
            
            // Wait for key
            while (!keyboard_getchar());
            
            // Start terminal
            terminal_run_shell();
            return;
        }
        
        // Continue GUI
        // TODO: GUI update/rendering
        
        asm volatile ("hlt");
    }
}

// ==================== KERNEL ENTRY ====================
void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    vga_init();
    print_banner();
    
    if (magic == 0x2BADB002) {
        struct multiboot_info* mb = (struct multiboot_info*)mb_info_addr;
        
        if (detect_gui(mb)) {
            vga_set_color(0x0B);
            vga_puts("GUI mode available\n");
            vga_puts("Starting GUI... (Press 'g' to switch to text)\n\n");
            run_gui_mode(mb);
        } else {
            vga_set_color(0x0E);
            vga_puts("No framebuffer detected\n");
            vga_puts("Starting text mode...\n\n");
            run_text_mode();
        }
    } else {
        vga_set_color(0x0E);
        vga_puts("Non-Multiboot boot\n");
        vga_puts("Starting text mode...\n\n");
        run_text_mode();
    }
    
    // Should never reach here
    while (1) {
        asm volatile ("hlt");
    }
}