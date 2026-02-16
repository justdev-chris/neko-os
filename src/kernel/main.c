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

// Function prototypes
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_install(void);
void irq_remap(void);
void irq_install(void);
void test_components(void);

// C handler wrappers
void irq0_handler_c(void) {
    timer_handler();
    outb(0x20, 0x20);
}

void irq1_handler_c(void) {
    keyboard_handler();
    outb(0x20, 0x20);
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
    
    // Test if IDT was installed
    if (idtp.base == 0) {
        terminal_panic("IDT installation failed!");
    }
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
    
    // Test if IRQ handlers were installed
    if (idt[32].base_low == 0 || idt[33].base_low == 0) {
        terminal_panic("IRQ handler installation failed!");
    }
}

void test_components(void) {
    // Test VGA
    vga_set_color(0x0F);
    vga_putchar('X');
    vga_putchar('\b');
    vga_putchar(' ');
    vga_putchar('\b');
    
    // Test timer (check if it's counting)
    uint32_t start = timer_get_seconds();
    int timeout = 0;
    while (timer_get_seconds() == start && timeout < 1000000) {
        timeout++;
        asm volatile("pause");
    }
    if (timeout >= 1000000) {
        terminal_panic("Timer not counting - PIT failure!");
    }
    
    // Test memory detection
    if (memory_get_total() == 0) {
        terminal_panic("Memory detection failed!");
    }
    if (memory_get_total() < 1024 * 1024) {  // Less than 1MB?
        terminal_panic("Insufficient memory detected!");
    }
    
    // Test keyboard (simple presence test)
    uint8_t status = inb(0x64);
    if (status == 0xFF) {  // Usually means no device
        terminal_panic("Keyboard controller not responding!");
    }
}

void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    // First, initialize minimum required systems
    vga_init();
    terminal_init();
    
    // Show boot message
    vga_set_color(0x0F);
    vga_puts("NekoOS Booting...\n\n");
    
    // Check multiboot magic number
    if (magic != 0x2BADB002) {
        terminal_panic("Invalid multiboot magic number!");
    }
    
    // Parse multiboot info for memory
    struct multiboot_info* mb_info = (struct multiboot_info*)mb_info_addr;
    unsigned long mem_upper = 0;
    
    if (mb_info->flags & (1 << 0)) {  // Check if mem_upper is valid
        mem_upper = mb_info->mem_upper * 1024;  // Convert to bytes
        vga_puts("  Multiboot info found\n");
    } else {
        terminal_panic("No memory info from bootloader!");
    }
    
    // Initialize core systems with error checking
    vga_puts("  Installing IDT...\n");
    idt_install();
    
    vga_puts("  Installing IRQs...\n");
    irq_install();
    
    vga_puts("  Initializing timer...\n");
    timer_init(100);
    
    vga_puts("  Initializing memory...\n");
    memory_init(0, mem_upper);  // Use actual detected memory
    
    vga_puts("  Initializing keyboard...\n");
    keyboard_init();
    
    // Run comprehensive component tests
    vga_puts("  Testing components...\n");
    test_components();
    
    // Enable interrupts
    vga_puts("  Enabling interrupts...\n");
    asm volatile("sti");
    
    // Final check - make sure interrupts are enabled
    uint32_t eflags;
    asm volatile("pushf; pop %0" : "=r"(eflags));
    if (!(eflags & 0x200)) {
        terminal_panic("Failed to enable interrupts!");
    }
    
    // If we get here, all systems are go!
    vga_set_color(0x0A);
    vga_puts("\n[OK] All systems operational!\n\n");
    vga_set_color(0x0F);
    
    // Show banner
    vga_set_color(0x0E);
    vga_puts("  _   _      _      ___   ____\n");
    vga_puts(" | \\ | | ___| | __ / _ \\ / ___|\n");
    vga_puts(" |  \\| |/ _ \\ |/ /| | | |\\___ \\\n");
    vga_puts(" | |\\  |  __/   < | |_| |___) |\n");
    vga_puts(" |_| \\_|\\___|_|\\_\\ \\___/|____/\n\n");
    
    vga_set_color(0x0F);
    vga_puts("NekoOS v0.1.4\n");
    vga_puts("Type 'help' for commands\n\n");
    
    // Start the shell
    terminal_run_shell();
    
    // Should never reach here
    terminal_panic("Kernel returned from shell!");
}
