#include "snake.h"
#include "../vga.h"
#include "../keyboard/keyboard.h"
#include "../io.h"
#include <stddef.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_LENGTH 100

static int snake_x[MAX_LENGTH];
static int snake_y[MAX_LENGTH];
static int snake_length = 3;
static int direction = 0; // 0=right, 1=down, 2=left, 3=up
static int apple_x, apple_y;
static int game_over = 0;
static int tick_counter = 0; // For better random seeding

// Better delay function
static void delay(int ticks) {
    for (volatile int i = 0; i < ticks * 50000; i++);
}

static void place_apple(void) {
    // Better pseudo-random placement
    tick_counter++;
    apple_x = (tick_counter * 1103515245 + 12345) % WIDTH;
    apple_y = (tick_counter * 123456789 + 98765) % HEIGHT;
    
    // Make sure apple doesn't spawn on snake
    for (int i = 0; i < snake_length; i++) {
        if (snake_x[i] == apple_x && snake_y[i] == apple_y) {
            // Try a different position
            tick_counter += 17;
            place_apple();
            return;
        }
    }
    
    // Make sure apple is within borders
    if (apple_x < 0) apple_x = 1;
    if (apple_x >= WIDTH) apple_x = WIDTH - 2;
    if (apple_y < 0) apple_y = 1;
    if (apple_y >= HEIGHT) apple_y = HEIGHT - 2;
}

static void init_game(void) {
    // Initialize snake in middle
    snake_length = 3;
    for (int i = 0; i < snake_length; i++) {
        snake_x[i] = WIDTH/2 - i;
        snake_y[i] = HEIGHT/2;
    }
    
    direction = 0;
    game_over = 0;
    tick_counter = 0;
    
    // Place first apple (NOT on snake)
    apple_x = WIDTH/2 + 5;
    apple_y = HEIGHT/2;
    
    // Make sure apple isn't on snake
    int on_snake = 1;
    while (on_snake) {
        on_snake = 0;
        for (int i = 0; i < snake_length; i++) {
            if (apple_x == snake_x[i] && apple_y == snake_y[i]) {
                apple_x = (apple_x + 3) % WIDTH;
                apple_y = (apple_y + 2) % HEIGHT;
                on_snake = 1;
                break;
            }
        }
    }
}

static void draw_game(void) {
    vga_clear();
    
    // Title
    vga_set_color(0x0E);
    vga_puts("=== SNAKE ===\n\n");
    
    // Draw top border
    vga_set_color(0x0F);
    for (int i = 0; i < WIDTH + 2; i++) {
        vga_putchar('#');
    }
    vga_putchar('\n');
    
    // Draw game area
    for (int y = 0; y < HEIGHT; y++) {
        vga_putchar('#'); // Left border
        
        for (int x = 0; x < WIDTH; x++) {
            int drawn = 0;
            
            // Check if snake head here
            if (x == snake_x[0] && y == snake_y[0]) {
                vga_set_color(0x0A);
                vga_putchar('@');
                drawn = 1;
            }
            // Check if snake body here
            else {
                for (int i = 1; i < snake_length; i++) {
                    if (x == snake_x[i] && y == snake_y[i]) {
                        vga_set_color(0x02);
                        vga_putchar('O');
                        drawn = 1;
                        break;
                    }
                }
            }
            
            // Check if apple here
            if (!drawn && x == apple_x && y == apple_y) {
                vga_set_color(0x0C);
                vga_putchar('*');
                drawn = 1;
            }
            
            // Empty space
            if (!drawn) {
                vga_set_color(0x07);
                vga_putchar('.');
            }
        }
        
        vga_putchar('#'); // Right border
        vga_putchar('\n');
    }
    
    // Draw bottom border
    vga_set_color(0x0F);
    for (int i = 0; i < WIDTH + 2; i++) {
        vga_putchar('#');
    }
    vga_putchar('\n');
    
    // Debug info
    vga_set_color(0x0D);
    vga_puts("\nApple at: (");
    // Simple number display
    if (apple_x < 10) vga_putchar('0' + apple_x);
    else vga_putchar('0' + apple_x/10);
    vga_putchar(',');
    if (apple_y < 10) vga_putchar('0' + apple_y);
    else vga_putchar('0' + apple_y/10);
    vga_puts(")");
    
    // Draw score and controls
    vga_set_color(0x0E);
    vga_puts("  Score: ");
    for (int i = 0; i < (snake_length - 3); i++) {
        vga_putchar('*');
    }
    
    vga_puts("\n\nWASD to move, Q to quit\n");
}

static void move_snake(void) {
    // Move body segments (from tail to head)
    for (int i = snake_length - 1; i > 0; i--) {
        snake_x[i] = snake_x[i-1];
        snake_y[i] = snake_y[i-1];
    }
    
    // Move head based on direction
    switch (direction) {
        case 0: snake_x[0]++; break; // Right
        case 1: snake_y[0]++; break; // Down
        case 2: snake_x[0]--; break; // Left
        case 3: snake_y[0]--; break; // Up
    }
}

static void check_collisions(void) {
    // Wall collision
    if (snake_x[0] < 0 || snake_x[0] >= WIDTH ||
        snake_y[0] < 0 || snake_y[0] >= HEIGHT) {
        game_over = 1;
        return;
    }
    
    // Self collision
    for (int i = 1; i < snake_length; i++) {
        if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
            game_over = 1;
            return;
        }
    }
    
    // Apple collision
    if (snake_x[0] == apple_x && snake_y[0] == apple_y) {
        // Grow snake
        if (snake_length < MAX_LENGTH) {
            // Add new segment at the tail position
            snake_x[snake_length] = snake_x[snake_length-1];
            snake_y[snake_length] = snake_y[snake_length-1];
            snake_length++;
        }
        place_apple();
    }
}

static void handle_input(void) {
    uint8_t scancode = keyboard_get_scancode();
    
    if (!scancode) return;
    
    // Only process key press (not release)
    if (scancode & 0x80) return;
    
    switch (scancode) {
        case 0x11: // W
            if (direction != 1) direction = 3;
            break;
        case 0x1F: // S
            if (direction != 3) direction = 1;
            break;
        case 0x1E: // A
            if (direction != 0) direction = 2;
            break;
        case 0x20: // D
            if (direction != 2) direction = 0;
            break;
        case 0x10: // Q
            game_over = 1;
            break;
    }
}

void snake_run(void) {
    init_game();
    
    while (!game_over) {
        draw_game();
        
        handle_input();
        move_snake();
        check_collisions();
        
        // SLOWER DELAY - increased from 100 to 300
        delay(300);
    }
    
    // Game over screen
    vga_clear();
    vga_set_color(0x0C);
    vga_puts("\n\n    GAME OVER!\n\n");
    
    vga_set_color(0x0F);
    vga_puts("    Final Score: ");
    for (int i = 0; i < (snake_length - 3); i++) {
        vga_putchar('*');
    }
    
    vga_puts("\n\n Press any key to exit");
    
    // Wait for key press
    while (!keyboard_get_scancode()) {
        delay(10);
    }
}
