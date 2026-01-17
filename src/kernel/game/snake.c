#include "snake.h"
#include "../vga.h"
#include "../keyboard/keyboard.h"

#define SNAKE_WIDTH 40
#define SNAKE_HEIGHT 20
#define MAX_SNAKE_LENGTH 100

typedef struct {
    int x, y;
} Point;

static Point snake[MAX_SNAKE_LENGTH];
static int snake_length = 3;
static int direction = 0;
static Point food;
static int score = 0;
static int game_over = 0;

// Simple random
static unsigned int seed = 12345;
static int simple_rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}

// Renamed to avoid conflict
void snake_draw_border(void) {
    vga_set_color(0x0F);
    
    for (int x = 0; x < SNAKE_WIDTH + 2; x++) {
        vga_putchar_at(x, 0, '#');
        vga_putchar_at(x, SNAKE_HEIGHT + 1, '#');
    }
    
    for (int y = 1; y < SNAKE_HEIGHT + 1; y++) {
        vga_putchar_at(0, y, '#');
        vga_putchar_at(SNAKE_WIDTH + 1, y, '#');
    }
}

void draw_snake(void) {
    vga_set_color(0x0A);
    vga_putchar_at(snake[0].x + 1, snake[0].y + 1, '@');
    
    vga_set_color(0x02);
    for (int i = 1; i < snake_length; i++) {
        vga_putchar_at(snake[i].x + 1, snake[i].y + 1, 'o');
    }
}

void draw_food(void) {
    vga_set_color(0x0C);
    vga_putchar_at(food.x + 1, food.y + 1, '*');
}

void snake_init(void) {
    for (int i = 0; i < 3; i++) {
        snake[i].x = 10 - i;
        snake[i].y = 10;
    }
    snake_length = 3;
    
    food.x = 20;
    food.y = 10;
    
    direction = 0;
    score = 0;
    game_over = 0;
    
    vga_clear();
    
    vga_set_color(0x0E);
    vga_puts_at(0, SNAKE_HEIGHT + 3, "Snake - WASD to move, Q to quit");
    vga_puts_at(0, SNAKE_HEIGHT + 4, "Score: 0");
    
    snake_draw_border();
    draw_snake();
    draw_food();
}

void spawn_food(void) {
    int valid = 0;
    while (!valid) {
        food.x = simple_rand() % SNAKE_WIDTH;
        food.y = simple_rand() % SNAKE_HEIGHT;
        
        valid = 1;
        for (int i = 0; i < snake_length; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                valid = 0;
                break;
            }
        }
    }
}

void snake_update(void) {
    if (game_over) return;
    
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    
    switch (direction) {
        case 0: snake[0].x++; break;
        case 1: snake[0].y++; break;
        case 2: snake[0].x--; break;
        case 3: snake[0].y--; break;
    }
    
    if (snake[0].x < 0 || snake[0].x >= SNAKE_WIDTH ||
        snake[0].y < 0 || snake[0].y >= SNAKE_HEIGHT) {
        game_over = 1;
        return;
    }
    
    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            game_over = 1;
            return;
        }
    }
    
    if (snake[0].x == food.x && snake[0].y == food.y) {
        score += 10;
        snake_length++;
        if (snake_length > MAX_SNAKE_LENGTH) snake_length = MAX_SNAKE_LENGTH;
        spawn_food();
        
        // Update score
        vga_set_color(0x0E);
        vga_puts_at(7, SNAKE_HEIGHT + 4, "      ");
        
        // Convert score to string
        char score_str[10];
        int n = score, i = 0;
        if (n == 0) score_str[i++] = '0';
        while (n > 0) {
            score_str[i++] = '0' + (n % 10);
            n /= 10;
        }
        score_str[i] = '\0';
        for (int j = 0; j < i/2; j++) {
            char temp = score_str[j];
            score_str[j] = score_str[i-j-1];
            score_str[i-j-1] = temp;
        }
        vga_puts_at(7, SNAKE_HEIGHT + 4, score_str);
    }
}

void snake_handle_input(char key) {
    switch (key) {
        case 'w': case 'W': if (direction != 1) direction = 3; break;
        case 's': case 'S': if (direction != 3) direction = 1; break;
        case 'a': case 'A': if (direction != 0) direction = 2; break;
        case 'd': case 'D': if (direction != 2) direction = 0; break;
        case 'q': case 'Q': game_over = 1; break;
    }
}

void snake_run(void) {
    snake_init();
    
    while (!game_over) {
        char key = keyboard_getchar();
        if (key) snake_handle_input(key);
        
        snake_update();
        snake_draw_border();
        draw_snake();
        draw_food();
        
        for (int i = 0; i < 1000000; i++) asm volatile ("nop");
    }
    
    // Game over
    vga_set_color(0x0C);
    vga_puts_at(SNAKE_WIDTH/2 - 5, SNAKE_HEIGHT/2, "GAME OVER");
    
    char score_str[10];
    int n = score, i = 0;
    if (n == 0) score_str[i++] = '0';
    while (n > 0) {
        score_str[i++] = '0' + (n % 10);
        n /= 10;
    }
    score_str[i] = '\0';
    for (int j = 0; j < i/2; j++) {
        char temp = score_str[j];
        score_str[j] = score_str[i-j-1];
        score_str[i-j-1] = temp;
    }
    
    vga_puts_at(SNAKE_WIDTH/2 - 8, SNAKE_HEIGHT/2 + 1, "Score: ");
    vga_puts_at(SNAKE_WIDTH/2 - 1, SNAKE_HEIGHT/2 + 1, score_str);
    vga_puts_at(SNAKE_WIDTH/2 - 8, SNAKE_HEIGHT/2 + 2, "Press any key...");
    
    while (!keyboard_getchar());
}