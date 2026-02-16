#include "timer.h"
#include "../io.h"
#include "../terminal/terminal.h"
#include "../vga.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

static volatile uint32_t timer_ticks = 0;

void timer_init(uint32_t frequency) {
    // Calculate divisor (1193180 / frequency)
    uint32_t divisor = 1193180 / frequency;
    
    // Send command: channel 0, lobyte/hibyte, square wave generator
    outb(PIT_COMMAND, 0x36);
    
    // Send divisor (low byte then high byte)
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
    
    timer_ticks = 0;
    
    // Small delay to let PIT initialize
    for (int i = 0; i < 1000; i++) {
        asm volatile("pause");
    }
}

void timer_handler(void) {
    timer_ticks++;
    terminal_update_tick();
}

uint32_t timer_get_seconds(void) {
    return timer_ticks / 100;
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}
