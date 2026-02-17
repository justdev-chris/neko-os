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
    vga_puts("  IDT installed\n");
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
    vga_puts("  IRQ remapped\n");
}

void irq_install(void) {
    irq_remap();
    idt_set_gate(32, (uint32_t)irq0_handler, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1_handler, 0x08, 0x8E);
    vga_puts("  IRQ handlers installed\n");
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

void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    (void)magic;
    (void)mb_info_addr;
    
    vga_init();
    terminal_init();
    
    vga_puts("Installing IDT...\n");
    idt_install();
    
    vga_puts("Installing IRQs...\n");
    irq_install();
    
    vga_puts("Initializing timer...\n");
    timer_init(100);
    
    vga_puts("Initializing memory...\n");
    memory_init(0, 32 * 1024 * 1024);
    
    print_banner();
    
    vga_puts("Initializing keyboard...\n");
    keyboard_init();
    
    vga_puts("Enabling interrupts...\n");
    asm volatile("sti");
    vga_puts("Interrupts enabled!\n");
    
    vga_puts("Starting shell...\n\n");
    terminal_run_shell();
    
    while (1) asm volatile("hlt");
}
