#include "vga.h"
#include "../keyboard/keyboard.h"

// Add these extern declarations for vga.c variables
extern volatile uint16_t* vga_buffer;
extern int cursor_x, cursor_y;

#define GAME_WIDTH 40
#define GAME_HEIGHT 20
#define BORDER_CHAR 0xDB // █ block character
#define CAT_CHAR '^'
#define FISH_CHAR '>'
#define HEART_CHAR 3 // ♥ heart character

typedef struct {
    int x, y;
} position_t;

position_t cat = {20, 10};
position_t fish = {5, 5};
int score = 0;
int lives = 3;
unsigned int rand_seed = 12345;

// Simple rand function
int my_rand(void) {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed / 65536) % 32768;
}

void draw_border(void) {
    // Top border
    for (int x = 0; x < GAME_WIDTH + 2; x++) {
        vga_buffer[0 * 80 + x] = (0x0B << 8) | BORDER_CHAR;
    }
    // Bottom border
    for (int x = 0; x < GAME_WIDTH + 2; x++) {
        vga_buffer[(GAME_HEIGHT + 1) * 80 + x] = (0x0B << 8) | BORDER_CHAR;
    }
    // Left border
    for (int y = 0; y < GAME_HEIGHT + 2; y++) {
        vga_buffer[y * 80 + 0] = (0x0B << 8) | BORDER_CHAR;
    }
    // Right border
    for (int y = 0; y < GAME_HEIGHT + 2; y++) {
        vga_buffer[y * 80 + GAME_WIDTH + 1] = (0x0B << 8) | BORDER_CHAR;
    }
}

void draw_cat(void) {
    vga_buffer[(cat.y + 1) * 80 + (cat.x + 1)] = (0x0E << 8) | CAT_CHAR;
}

void draw_fish(void) {
    vga_buffer[(fish.y + 1) * 80 + (fish.x + 1)] = (0x09 << 8) | FISH_CHAR;
}

void draw_ui(void) {
    // Score
    vga_set_color(0x0F);
    
    // Use existing vga_puts with cursor positioning
    int old_x = cursor_x, old_y = cursor_y;
    
    // Draw "Score: "
    cursor_x = 45;
    cursor_y = 2;
    vga_puts("Score: ");
    
    // Convert score to string
    char score_str[10];
    int temp = score;
    int i = 0;
    
    if (temp == 0) {
        score_str[i++] = '0';
    } else {
        while (temp > 0) {
            score_str[i++] = '0' + (temp % 10);
            temp /= 10;
        }
    }
    score_str[i] = '\0';
    
    // Reverse the string
    for (int j = 0; j < i/2; j++) {
        char tmp = score_str[j];
        score_str[j] = score_str[i - j - 1];
        score_str[i - j - 1] = tmp;
    }
    
    // Draw score
    cursor_x = 52;
    vga_puts(score_str);
    
    // Draw "Lives: "
    cursor_x = 45;
    cursor_y = 4;
    vga_puts("Lives: ");
    
    // Draw hearts
    for (int j = 0; j < lives; j++) {
        vga_buffer[4 * 80 + (52 + j)] = (0x0C << 8) | HEART_CHAR;
    }
    
    // Draw controls
    cursor_x = 45;
    cursor_y = 6;
    vga_puts("WASD: Move");
    
    cursor_x = 45;
    cursor_y = 7;
    vga_puts("ESC: Quit");
    
    cursor_x = 45;
    cursor_y = 9;
    vga_puts("Goal: Eat fish");
    
    cursor_x = 45;
    cursor_y = 10;
    vga_puts("to gain points!");
    
    // Restore cursor
    cursor_x = old_x;
    cursor_y = old_y;
}

void place_new_fish(void) {
    fish.x = 1 + (my_rand() % (GAME_WIDTH - 2));
    fish.y = 1 + (my_rand() % (GAME_HEIGHT - 2));
}

void run_neko_game(void) {
    vga_clear();
    vga_set_color(0x0F);
    vga_puts("Starting Neko Game!\n");
    vga_puts("Collect fish (>) with your cat (^)\n\n");
    
    // Wait a moment
    for (volatile int i = 0; i < 10000000; i++);
    
    // Initialize game state
    cat.x = GAME_WIDTH / 2;
    cat.y = GAME_HEIGHT / 2;
    score = 0;
    lives = 3;
    place_new_fish();
    
    int game_running = 1;
    
    while (game_running && lives > 0) {
        // Clear game area
        for (int y = 1; y <= GAME_HEIGHT; y++) {
            for (int x = 1; x <= GAME_WIDTH; x++) {
                vga_buffer[y * 80 + x] = (0x00 << 8) | ' ';
            }
        }
        
        draw_border();
        draw_cat();
        draw_fish();
        draw_ui();
        
        // Check collision
        if (cat.x == fish.x && cat.y == fish.y) {
            score += 10;
            place_new_fish();
        }
        
        // Handle input
        uint8_t scancode = keyboard_get_scancode();
        if (scancode) {
            // Simplified input handling
            if (scancode == 0x01) { // ESC
                game_running = 0;
            } else if (scancode == 0x11) { // W
                if (cat.y > 1) cat.y--;
            } else if (scancode == 0x1F) { // S
                if (cat.y < GAME_HEIGHT - 2) cat.y++;
            } else if (scancode == 0x1E) { // A
                if (cat.x > 1) cat.x--;
            } else if (scancode == 0x20) { // D
                if (cat.x < GAME_WIDTH - 2) cat.x++;
            }
        }
        
        // Simple delay
        for (volatile int i = 0; i < 1000000; i++);
    }
    
    // Game over screen
    vga_clear();
    vga_set_color(0x0F);
    vga_puts("Game Over!\n");
    vga_puts("Final Score: ");
    
    // Display score
    char score_str[10];
    int temp = score;
    int i = 0;
    
    if (temp == 0) {
        score_str[i++] = '0';
    } else {
        while (temp > 0) {
            score_str[i++] = '0' + (temp % 10);
            temp /= 10;
        }
    }
    score_str[i] = '\0';
    
    // Reverse the string
    for (int j = 0; j < i/2; j++) {
        char tmp = score_str[j];
        score_str[j] = score_str[i - j - 1];
        score_str[i - j - 1] = tmp;
    }
    
    vga_puts(score_str);
    vga_puts("\n\nPress any key to continue...\n");
    
    // Wait for key
    while (!keyboard_get_scancode());
    
    // Clear input buffer
    while (keyboard_get_scancode());
}
