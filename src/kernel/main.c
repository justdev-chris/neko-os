#include "vga.h"
#include "keyboard/keyboard.h"
#include "terminal/terminal.h"
#include "game/game.h"
#include "gui/framebuffer.h"
#include "gui/desktop.h"
#include <stdint.h>

int gui_mode = 0;

void show_changelog(void) {
    vga_set_color(0x0E);
    vga_puts("\nNekoOS v0.1.4\n");
    vga_puts("-------------\n");
    vga_set_color(0x0F);
    vga_puts("- GUI framework (testing)\n");
    vga_puts("- Fixed folder structure\n");
    vga_puts("- Fixed Multiboot header\n");
    vga_puts("- Terminal with commands\n");
    vga_puts("- Keyboard driver\n");
    vga_puts("- VGA text mode\n");
}

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
    vga_puts("[TEST] GUI framework\n");
    vga_puts("[OK] Neko game\n\n");
}

int detect_gui(struct multiboot_info* mb) {
    if (mb->flags & (1 << 12)) {
        vga_set_color(0x0D);
        vga_puts("DEBUG: Framebuffer flag SET\n");
        vga_puts("Type: ");
        // Print framebuffer type
        if (mb->framebuffer_type == 1) {
            vga_puts("RGB\n");
            return 1;
        } else {
            vga_puts("Unknown (");
            // Print type number
            vga_puts(")\n");
        }
    } else {
        vga_set_color(0x0C);
        vga_puts("DEBUG: No framebuffer flag\n");
    }
    return 0;
}

void run_text_mode(void) {
    gui_mode = 0;
    vga_set_color(0x0F);
    vga_puts("\n=== TEXT MODE ===\n");
    vga_puts("Type 'help' for commands\n");
    vga_puts("Type 'gui' to switch modes\n\n");
    
    keyboard_init();
    terminal_run_shell();
}

void run_gui_mode(struct multiboot_info* mb) {
    gui_mode = 1;
    
    vga_set_color(0x0B);
    vga_puts("\nInitializing GUI...\n");
    
    // Initialize framebuffer
    fb_init(mb->framebuffer_addr,
            mb->framebuffer_width,
            mb->framebuffer_height,
            mb->framebuffer_bpp,
            mb->framebuffer_pitch);
    
    vga_puts("Starting GUI desktop...\n");
    gui_run();
    
    // Should not return
    vga_set_color(0x0C);
    vga_puts("ERROR: gui_run() returned!\n");
}

void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    vga_init();
    print_banner();
    
    vga_set_color(0x0D);
    vga_puts("Magic: 0x");
    // Print magic
    vga_puts(magic == 0x2BADB002 ? "2BADB002 (Multiboot)\n" : "Unknown\n");
    
    if (magic == 0x2BADB002) {
        struct multiboot_info* mb = (struct multiboot_info*)mb_info_addr;
        
        vga_puts("Flags: 0x");
        // Print flags
        vga_puts("\n");
        
        if (detect_gui(mb)) {
            run_gui_mode(mb);
        } else {
            vga_puts("No GUI support\n");
            run_text_mode();
        }
    } else {
        vga_puts("Not Multiboot\n");
        run_text_mode();
    }
    
    while (1) asm("hlt");
}
