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

// Helper to print at position (simulated cursor)
static void print_at(int x, int y, char c) {
    // Move cursor by printing newlines and spaces
    vga_set_color(0x0F);
    // Simple positioning - just put everything at current cursor
    // This is a basic workaround
    vga_putchar(c);
}

// Simple delay function
static void delay(int ticks) {
    for (volatile int i = 0; i < ticks * 10000; i++);
}

static void place_apple(void) {
    // Simple random-ish placement
    apple_x = 5 + (apple_y * 17) % (WIDTH - 10);
    apple_y = 5 + (apple_x * 13) % (HEIGHT - 10);
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

static void draw_game(void) {
    vga_clear();
    vga_set_color(0x0E);
    vga_puts("=== SNAKE ===\n");
    
    // Draw top border
    vga_set_color(0x0F);
    for (int i = 0; i < WIDTH + 2; i++) vga_putchar('#');
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
                vga_set_color(0x08);
                vga_putchar('.');
            }
        }
        
        vga_putchar('#'); // Right border
        vga_putchar('\n');
    }
    
    // Draw bottom border
    vga_set_color(0x0F);
    for (int i = 0; i < WIDTH + 2; i++) vga_putchar('#');
    vga_putchar('\n');
    
    // Draw score
    vga_set_color(0x0E);
    vga_puts("\nScore: ");
    for (int i = 0; i < (snake_length - 3); i++) {
        vga_putchar('*');
    }
    
    vga_puts("\nWASD to move, Q to quit\n");
}

// ... rest of the functions (move_snake, check_collisions, handle_input) stay the same ...

void snake_run(void) {
    vga_clear();
    init_game();
    
    while (!game_over) {
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
    vga_puts("\n\n    GAME OVER!\n\n");
    
    vga_set_color(0x0F);
    vga_puts("    Score: ");
    for (int i = 0; i < (snake_length - 3); i++) {
        vga_putchar('*');
    }
    
    vga_puts("\n\n Press any key to exit");
    
    // Wait for key press
    while (!keyboard_get_scancode()) {
        delay(10);
    }
}
