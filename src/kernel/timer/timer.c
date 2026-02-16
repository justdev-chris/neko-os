#include "timer.h"
#include "../io.h"
#include "../terminal/terminal.h"
#include "../vga.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

static volatile uint32_t timer_ticks = 0;
static uint32_t last_poll_time = 0;

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
    
    timer_ticks = 0;
    last_poll_time = 0;
    
    vga_puts("  PIT initialized\n");
}

// This is called from IRQ0 (if interrupts work)
void timer_handler(void) {
    timer_ticks++;
    terminal_update_tick();
}

// Polling function to manually update timer
void timer_poll(void) {
    static int poll_counter = 0;
    poll_counter++;
    
    // Count to ~10000 for ~100Hz (crude approximation)
    if (poll_counter >= 10000) {
        timer_ticks++;
        terminal_update_tick();
        poll_counter = 0;
    }
}

uint32_t timer_get_seconds(void) {
    return timer_ticks / 100;
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}
