#include "snake.h"
#include "../vga.h"
#include "../keyboard/keyboard.h"
#include <stdint.h>

#define SNAKE_WIDTH 40
#define SNAKE_HEIGHT 20
#define MAX_SNAKE_LENGTH 100

typedef struct {
    int x, y;
} Point;

static Point snake[MAX_SNAKE_LENGTH];
static int snake_length = 3;
static int direction = 0; // 0=right, 1=down, 2=left, 3=up
static Point food;
static int score = 0;
static int game_over = 0;

// Draw game border
void draw_border(void) {
    vga_set_color(0x0F); // White
    
    // Top and bottom
    for (int x = 0; x < SNAKE_WIDTH + 2; x++) {
        vga_putchar_at(x, 0, '#');
        vga_putchar_at(x, SNAKE_HEIGHT + 1, '#');
    }
    
    // Sides
    for (int y = 1; y < SNAKE_HEIGHT + 1; y++) {
        vga_putchar_at(0, y, '#');
        vga_putchar_at(SNAKE_WIDTH + 1, y, '#');
    }
}

// Draw snake
void draw_snake(void) {
    vga_set_color(0x0A); // Green head
    vga_putchar_at(snake[0].x + 1, snake[0].y + 1, '@');
    
    vga_set_color(0x02); // Dark green body
    for (int i = 1; i < snake_length; i++) {
        vga_putchar_at(snake[i].x + 1, snake[i].y + 1, 'o');
    }
}

// Draw food
void draw_food(void) {
    vga_set_color(0x0C); // Red
    vga_putchar_at(food.x + 1, food.y + 1, '*');
}

// Initialize game
void snake_init(void) {
    // Initial snake position (center)
    for (int i = 0; i < 3; i++) {
        snake[i].x = 10 - i;
        snake[i].y = 10;
    }
    snake_length = 3;
    
    // Initial food
    food.x = 20;
    food.y = 10;
    
    direction = 0; // Right
    score = 0;
    game_over = 0;
    
    // Clear screen
    vga_clear();
    
    // Draw UI
    vga_set_color(0x0E);
    vga_puts_at(0, SNAKE_HEIGHT + 3, "Snake Game - Use WASD, Q to quit");
    vga_puts_at(0, SNAKE_HEIGHT + 4, "Score: 0");
    
    draw_border();
    draw_snake();
    draw_food();
}

// Generate new food
void spawn_food(void) {
    int valid = 0;
    while (!valid) {
        food.x = rand() % SNAKE_WIDTH;
        food.y = rand() % SNAKE_HEIGHT;
        
        valid = 1;
        // Check if food spawns on snake
        for (int i = 0; i < snake_length; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                valid = 0;
                break;
            }
        }
    }
}

// Update game state
void snake_update(void) {
    if (game_over) return;
    
    // Move snake body
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    
    // Move head based on direction
    switch (direction) {
        case 0: snake[0].x++; break; // Right
        case 1: snake[0].y++; break; // Down
        case 2: snake[0].x--; break; // Left
        case 3: snake[0].y--; break; // Up
    }
    
    // Check collision with walls
    if (snake[0].x < 0 || snake[0].x >= SNAKE_WIDTH ||
        snake[0].y < 0 || snake[0].y >= SNAKE_HEIGHT) {
        game_over = 1;
        return;
    }
    
    // Check collision with self
    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            game_over = 1;
            return;
        }
    }
    
    // Check if ate food
    if (snake[0].x == food.x && snake[0].y == food.y) {
        score += 10;
        snake_length++;
        if (snake_length > MAX_SNAKE_LENGTH) snake_length = MAX_SNAKE_LENGTH;
        spawn_food();
        
        // Update score display
        vga_set_color(0x0E);
        vga_puts_at(7, SNAKE_HEIGHT + 4, "      ");
        vga_puts_at(7, SNAKE_HEIGHT + 4, score);
    }
}

// Handle input
void snake_handle_input(char key) {
    switch (key) {
        case 'w': case 'W': if (direction != 1) direction = 3; break;
        case 's': case 'S': if (direction != 3) direction = 1; break;
        case 'a': case 'A': if (direction != 0) direction = 2; break;
        case 'd': case 'D': if (direction != 2) direction = 0; break;
        case 'q': case 'Q': game_over = 1; break;
    }
}

// Main game loop
void snake_run(void) {
    snake_init();
    
    while (!game_over) {
        // Handle input
        char key = keyboard_getchar();
        if (key) snake_handle_input(key);
        
        // Update game
        snake_update();
        
        // Draw
        draw_border();
        draw_snake();
        draw_food();
        
        // Simple delay
        for (int i = 0; i < 1000000; i++) asm volatile ("nop");
    }
    
    // Game over screen
    vga_set_color(0x0C);
    vga_puts_at(SNAKE_WIDTH/2 - 5, SNAKE_HEIGHT/2, "GAME OVER");
    vga_puts_at(SNAKE_WIDTH/2 - 8, SNAKE_HEIGHT/2 + 1, "Final Score: ");
    vga_puts_at(SNAKE_WIDTH/2 + 5, SNAKE_HEIGHT/2 + 1, score);
    vga_puts_at(SNAKE_WIDTH/2 - 8, SNAKE_HEIGHT/2 + 2, "Press any key...");
    
    while (!keyboard_getchar());
}