#include "vga.h"
#include "keyboard/keyboard.h"
#include "terminal/terminal.h"
#include "game/snake.h"
#include "game/game.h"
#include "timer/timer.h"
#include "memory/memory.h"
#include "io.h"
#include <stdint.h>

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

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#define IDT_ENTRIES 256
struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;

extern void idt_load(struct idt_ptr*);
extern void irq0_handler(void);
extern void irq1_handler(void);

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

void idt_install(void) {
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (uint32_t)&idt;
    
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    idt_load(&idtp);
}

void irq_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);
}

void irq_install(void) {
    irq_remap();
    idt_set_gate(32, (uint32_t)irq0_handler, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1_handler, 0x08, 0x8E);
}

void irq0_handler_c(void) {
    timer_handler();
    outb(0x20, 0x20);
}

void irq1_handler_c(void) {
    keyboard_handler();
    outb(0x20, 0x20);
}

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
    vga_puts("[OK] Snake game\n");
    vga_puts("[OK] Timer driver\n");
    vga_puts("[OK] IRQ handlers\n\n");
    
    vga_set_color(0x0F);
}

void draw_pixel_cat(uint32_t* fb, int width, int height) {
    uint32_t orange = 0xFFFFA500;
    uint32_t black = 0xFF000000;
    uint32_t white = 0xFFFFFFFF;
    uint32_t pink = 0xFFFFC0CB;
    uint32_t blue = 0xFF87CEEB;
    
    // Clear screen to blue
    for (int i = 0; i < width * height; i++) {
        fb[i] = blue;
    }
    
    // Center the cat
    int start_x = width/2 - 16;
    int start_y = height/2 - 16;
    
    // Draw cat pixel art (32x32)
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            int px = start_x + x;
            int py = start_y + y;
            if (px < 0 || px >= width || py < 0 || py >= height) continue;
            
            // Face (main square)
            if (x >= 8 && x <= 24 && y >= 8 && y <= 24) {
                fb[py * width + px] = orange;
            }
            
            // Left ear
            if (x >= 4 && x <= 8 && y >= 2 && y <= 8) {
                if ((x == 4 && y >= 4) || (x == 8 && y >= 4) || (y >= 4))
                fb[py * width + px] = orange;
            }
            
            // Right ear
            if (x >= 24 && x <= 28 && y >= 2 && y <= 8) {
                if ((x == 24 && y >= 4) || (x == 28 && y >= 4) || (y >= 4))
                fb[py * width + px] = orange;
            }
            
            // Left eye
            if (x == 12 && y == 14) fb[py * width + px] = black;
            if (x == 13 && y == 13) fb[py * width + px] = white;
            
            // Right eye
            if (x == 20 && y == 14) fb[py * width + px] = black;
            if (x == 21 && y == 13) fb[py * width + px] = white;
            
            // Nose
            if (x == 16 && y == 18) fb[py * width + px] = pink;
            
            // Whiskers
            if (y == 16) {
                if (x == 6 || x == 8 || x == 10) fb[py * width + px] = white;
                if (x == 22 || x == 24 || x == 26) fb[py * width + px] = white;
            }
            
            // Mouth
            if ((x == 15 && y == 19) || (x == 16 && y == 20) || (x == 17 && y == 19)) {
                fb[py * width + px] = black;
            }
        }
    }
}

void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    struct multiboot_info* mb_info = (struct multiboot_info*)mb_info_addr;
    
    vga_init();
    terminal_init();
    
    idt_install();
    irq_install();
    timer_init(100);
    memory_init(0, 32 * 1024 * 1024);
    keyboard_init();
    
    asm volatile("sti");
    
    // Check command line
    char* cmdline = (char*)mb_info->cmdline;
    
    if (cmdline && strcmp(cmdline, "graphics") == 0) {
        // Graphics mode
        if (mb_info->flags & (1 << 12)) {
            uint32_t* fb = (uint32_t*)(uintptr_t)mb_info->framebuffer_addr;
            int width = mb_info->framebuffer_width;
            int height = mb_info->framebuffer_height;
            
            draw_pixel_cat(fb, width, height);
            
            while(1) asm volatile("hlt");
        } else {
            vga_puts("No framebuffer available!\n");
            while(1) asm volatile("hlt");
        }
    } else {
        // Text mode
        print_banner();
        terminal_run_shell();
    }
    
    while(1) asm volatile("hlt");
}
