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

// Simple delay function
static void delay(int ticks) {
    for (volatile int i = 0; i < ticks * 10000; i++);
}

static void place_apple(void) {
    apple_x = (apple_x * 1103515245 + 12345) % (WIDTH - 2) + 1;
    apple_y = (apple_y * 1103515245 + 12345) % (HEIGHT - 2) + 1;
    
    // Make sure apple doesn't spawn on snake
    for (int i = 0; i < snake_length; i++) {
        if (snake_x[i] == apple_x && snake_y[i] == apple_y) {
            place_apple(); // Try again
            return;
        }
    }
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
    
    // Place first apple
    apple_x = WIDTH/2 + 5;
    apple_y = HEIGHT/2;
    place_apple();
}

static void draw_border(void) {
    vga_set_color(0x0F);
    
    // Top and bottom borders
    for (int x = 0; x < WIDTH; x++) {
        vga_set_cursor(x, 0);
        vga_putchar('#');
        vga_set_cursor(x, HEIGHT-1);
        vga_putchar('#');
    }
    
    // Left and right borders
    for (int y = 1; y < HEIGHT-1; y++) {
        vga_set_cursor(0, y);
        vga_putchar('#');
        vga_set_cursor(WIDTH-1, y);
        vga_putchar('#');
    }
}

static void draw_game(void) {
    // Clear play area
    for (int y = 1; y < HEIGHT-1; y++) {
        for (int x = 1; x < WIDTH-1; x++) {
            vga_set_cursor(x, y);
            vga_putchar(' ');
        }
    }
    
    // Draw snake
    vga_set_color(0x0A); // Green head
    vga_set_cursor(snake_x[0], snake_y[0]);
    vga_putchar('@');
    
    vga_set_color(0x02); // Green body
    for (int i = 1; i < snake_length; i++) {
        vga_set_cursor(snake_x[i], snake_y[i]);
        vga_putchar('O');
    }
    
    // Draw apple
    vga_set_color(0x0C); // Red
    vga_set_cursor(apple_x, apple_y);
    vga_putchar('*');
    
    // Draw score
    vga_set_color(0x0E);
    vga_set_cursor(0, HEIGHT);
    vga_puts("Score: ");
    // Simple score display
    for (int i = 0; i < (snake_length - 3); i++) {
        vga_putchar('*');
    }
    
    vga_set_cursor(20, HEIGHT);
    vga_puts("WASD to move, Q to quit");
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
    if (snake_x[0] <= 0 || snake_x[0] >= WIDTH-1 ||
        snake_y[0] <= 0 || snake_y[0] >= HEIGHT-1) {
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
        // Grow snake (only if we have room)
        if (snake_length < MAX_LENGTH) {
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
    vga_clear();
    init_game();
    
    vga_set_color(0x0E);
    vga_set_cursor(WIDTH/2 - 5, 0);
    vga_puts("=== SNAKE ===");
    
    while (!game_over) {
        draw_border();
        draw_game();
        
        handle_input();
        move_snake();
        check_collisions();
        
        // DELAY - this fixes the speed issue
        delay(100);
    }
    
    // Game over screen
    vga_clear();
    vga_set_color(0x0C);
    vga_set_cursor(WIDTH/2 - 5, HEIGHT/2);
    vga_puts("GAME OVER!");
    
    vga_set_color(0x0F);
    vga_set_cursor(WIDTH/2 - 8, HEIGHT/2 + 1);
    vga_puts("Score: ");
    for (int i = 0; i < (snake_length - 3); i++) {
        vga_putchar('*');
    }
    
    vga_set_cursor(WIDTH/2 - 10, HEIGHT/2 + 3);
    vga_puts("Press any key to exit");
    
    // Wait for key press
    while (!keyboard_get_scancode()) {
        delay(10);
    }
}
