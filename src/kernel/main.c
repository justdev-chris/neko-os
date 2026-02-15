#include "vga.h"
#include "io.h"  
#include "keyboard/keyboard.h"
#include "terminal/terminal.h"
#include "game/game.h"
#include "game/snake.h"
#include "timer/timer.h"
#include "memory/memory.h"
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

// IDT entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// IDT and handlers
#define IDT_ENTRIES 256
struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;

// External assembly functions (defined in interrupts.s)
extern void idt_load(struct idt_ptr*);
extern void irq0_handler(void);
extern void irq1_handler(void);

// Function prototypes
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_install(void);
void irq_remap(void);
void irq_install(void);

// C handler wrappers
void irq0_handler_c(void) {
    timer_handler();
    // Send EOI to master PIC
    outb(0x20, 0x20);
}

void irq1_handler_c(void) {
    keyboard_handler();
    // Send EOI to master PIC
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
    
    // Clear IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Load IDT
    idt_load(&idtp);
}

void irq_remap(void) {
    // Remap IRQ table to 0x20-0x2F (protected mode standard)
    outb(0x20, 0x11);  // Start init of master PIC
    outb(0xA0, 0x11);  // Start init of slave PIC
    outb(0x21, 0x20);  // Master PIC vector offset -> 0x20
    outb(0xA1, 0x28);  // Slave PIC vector offset -> 0x28
    outb(0x21, 0x04);  // Tell master there's a slave at IRQ2
    outb(0xA1, 0x02);  // Tell slave its cascade identity
    outb(0x21, 0x01);  // 8086 mode for master
    outb(0xA1, 0x01);  // 8086 mode for slave
    
    // Clear masks
    outb(0x21, 0xFD);  // Enable IRQ1 (keyboard) only, mask others
    outb(0xA1, 0xFF);  // Mask all slave IRQs
}

void irq_install(void) {
    irq_remap();
    
    // Install IRQ handlers
    idt_set_gate(32, (uint32_t)irq0_handler, 0x08, 0x8E);  // Timer
    idt_set_gate(33, (uint32_t)irq1_handler, 0x08, 0x8E);  // Keyboard
    
    asm volatile("sti");  // Enable interrupts
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
    vga_set_color(0x0C);
    vga_puts("[OK] Timer driver\n");
    vga_puts("[OK] IRQ handlers\n\n");
}

void run_text_mode(void) {
    vga_set_color(0x0F);
    vga_puts("Type 'help' for commands\n\n");
    
    terminal_run_shell();
}

void kernel_main(uint32_t magic, uint32_t mb_info_addr) {
    (void)magic;  // Suppress unused parameter warning
    (void)mb_info_addr;  // Suppress unused parameter warning
    
    vga_init();
    
    // Install IDT and IRQs first
    idt_install();
    irq_install();
    
    // Initialize PIT timer with 100 Hz
    timer_init(100);
    
    // Initialize memory (using multiboot info would be better, but simple for now)
    memory_init(0, 32 * 1024 * 1024);  // Assume 32MB of memory
    
    print_banner();
    keyboard_init();
    run_text_mode();
    
    while (1) asm volatile("hlt");
}
