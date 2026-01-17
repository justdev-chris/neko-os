#include "vga.h"
#include "keyboard/keyboard.h"
#include "terminal/terminal.h"
#include "game/game.h"
#include "gui/framebuffer.h"
#include "gui/desktop.c"
#include <stdint.h>

// Changelog function
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

// Version info
void show_version(void) {
    vga_set_color(0x0E);
    vga_puts("\nNekoOS v0.1.4\n");
    vga_set_color(0x0F);
    vga_puts("Type 'help' for commands\n");
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
    vga_puts("[..] GUI framework\n");
    vga_puts("[OK] Neko game\n\n");
}

int detect_gui(struct multiboot_info* mb) {
    if (mb->flags & (1 << 12)) {
        if (mb->framebuffer_type == 1) {
            return 1;
        }
    }
    return 0;
}

void run_gui(struct multiboot_info* mb) {
    vga_set_color(0x0B);
    vga_puts("GUI mode detected\n");
    
    fb_init(mb->framebuffer_addr,
            mb->framebuffer_width,
            mb->framebuffer_height,
            mb->framebuffer_bpp,
            mb->framebuffer_pitch);
    
    vga_puts("GUI loaded\n");
    keyboard_init();
    terminal_run_shell();
}

void run_text(void) {
    vga_set_color(0x0F);
    vga_puts("Text mode\n\n");
    keyboard_init();
    terminal_run_shell();
}

void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    vga_init();
    print_banner();
    
    if (magic == 0x2BADB002) {
        struct multiboot_info* mb = (struct multiboot_info*)mb_info_addr;
        
        if (detect_gui(mb)) {
            run_gui(mb);
        } else {
            run_text();
        }
    } else {
        run_text();
    }
    
    while (1) asm("hlt");
}