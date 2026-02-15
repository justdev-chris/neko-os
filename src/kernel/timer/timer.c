#include "timer.h"
#include "../io.h"
#include "../terminal/terminal.h"
#include "../vga.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

static volatile uint32_t timer_ticks = 0;
static const uint32_t TICKS_PER_SECOND = 100;

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
    
    timer_ticks = 0;
    
    vga_set_color(0x0A);
    vga_puts("  Timer initialized at ");
    vga_putchar('0' + frequency / 100);
    vga_putchar('0' + (frequency / 10) % 10);
    vga_putchar('0' + frequency % 10);
    vga_puts(" Hz\n");
    vga_set_color(0x0F);
}

void timer_handler(void) {
    timer_ticks++;
    terminal_update_tick();
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

uint32_t timer_get_seconds(void) {
    return timer_ticks / TICKS_PER_SECOND;
}

void timer_sleep(uint32_t milliseconds) {
    uint32_t start_ticks = timer_ticks;
    uint32_t ticks_to_wait = (milliseconds * TICKS_PER_SECOND) / 1000;
    
    while ((timer_ticks - start_ticks) < ticks_to_wait) {
        asm volatile("hlt");
    }
}
