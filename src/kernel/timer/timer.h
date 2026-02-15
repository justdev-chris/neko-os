#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Initialize PIT timer with specified frequency
void timer_init(uint32_t frequency);

// Get current system ticks (increments ~100 times per second)
uint32_t timer_get_ticks(void);

// Get system uptime in seconds
uint32_t timer_get_seconds(void);

// Wait for specified number of milliseconds (blocking)
void timer_sleep(uint32_t milliseconds);

#endif
