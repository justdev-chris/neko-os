#include "timer.h"
#include "../io.h"
#include "../terminal/terminal.h"  // For terminal_update_tick

// PIT I/O ports
#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

// Timer variables
static volatile uint32_t timer_ticks = 0;
static const uint32_t TICKS_PER_SECOND = 100;  // We'll set PIT to 100 Hz

// External function from terminal to update uptime
extern void terminal_update_tick(void);

// Timer interrupt handler
void timer_handler(void) {
    timer_ticks++;
    terminal_update_tick();  // Update the terminal's uptime counter
}

// Initialize PIT timer
void timer_init(uint32_t frequency) {
    // Calculate divisor
    uint32_t divisor = 1193180 / frequency;  // PIT runs at 1.19318 MHz
    
    // Send command byte
    outb(PIT_COMMAND, 0x36);  // Channel 0, lobyte/hibyte, rate generator
    
    // Send divisor
    outb(PIT_CHANNEL0, divisor & 0xFF);        // Low byte
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF); // High byte
    
    timer_ticks = 0;
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

uint32_t timer_get_seconds(void) {
    return timer_ticks / TICKS_PER_SECOND;
}

// Simple blocking sleep (not ideal but works for now)
void timer_sleep(uint32_t milliseconds) {
    uint32_t start_ticks = timer_ticks;
    uint32_t ticks_to_wait = (milliseconds * TICKS_PER_SECOND) / 1000;
    
    while ((timer_ticks - start_ticks) < ticks_to_wait) {
        asm("hlt");  // Halt CPU until next interrupt
    }
}
