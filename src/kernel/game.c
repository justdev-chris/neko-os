#include "vga.h"
#include "keyboard.h"

#define GAME_WIDTH 40
#define GAME_HEIGHT 20
#define BOARDER_CHAR '█'
#define CAT_CHAR '^'
#define FISH_CHAR '>'
#define HEART_CHAR '♥'

typedef struct {
    int x, y;
} position_t;

position_t cat = {20, 10};
position_t fish = {5, 5};
int score = 0;
int lives = 3;

void draw_game_border(void) {
    vga_set_color(0x0B); // Cyan border
    
    // Top and bottom border
    for (int x = 0; x < GAME_WIDTH + 2; x++) {
        vga_buffer[0 * 80 + x] = (0x0B << 8) | BOARDER_CHAR;
        vga_buffer[(GAME_HEIGHT + 1) * 80 + x] = (0x0B << 8) | BOARDER_CHAR;
    }
    
    // Left and right border
    for (int y = 0; y < GAME_HEIGHT + 2; y++) {
        vga_buffer[y * 80 + 0] = (0x0B << 8) | BOARDER_CHAR;
        vga_buffer[y * 80 + GAME_WIDTH + 1] = (0x0B << 8) | BOARDER_CHAR;
    }
}

void draw_cat(void) {
    vga_set_color(0x0E); // Yellow cat
    
    // Cat face
    vga_buffer[(cat.y + 1) * 80 + (cat.x + 1)] = (0x0E << 8) | '^';
    vga_buffer[(cat.y + 1) * 80 + (cat.x)] = (0x0E << 8) | '/';
    vga_buffer[(cat.y + 1) * 80 + (cat.x + 2)] = (0x0E << 8) | '\\';
    
    // Cat body
    vga_buffer[(cat.y + 2) * 80 + (cat.x + 1)] = (0x0E << 8) | '|';
    vga_buffer[(cat.y + 3) * 80 + (cat.x)] = (0x0E << 8) | '/';
    vga_buffer[(cat.y + 3) * 80 + (cat.x + 2)] = (0x0E << 8) | '\\';
}

void draw_fish(void) {
    vga_set_color(0x09); // Blue fish
    vga_buffer[(fish.y + 1) * 80 + (fish.x + 1)] = (0x09 << 8) | FISH_CHAR;
}

void draw_ui(void) {
    vga_set_color(0x0F); // White text
    
    // Score
    char score_str[20];
    char* ptr = score_str;
    int s = score;
    do {
        *ptr++ = '0' + (s % 10);
        s /= 10;
    } while (s);
    *ptr-- = '\0';
    
    // Reverse string
    char* start = score_str;
    while (start < ptr) {
        char tmp = *start;
        *start = *ptr;
        *ptr = tmp;
        start++;
        ptr--;
    }
    
    vga_puts_at(45, 2, "Score: ");
    vga_puts_at(52, 2, score_str);
    
    // Lives
    vga_puts_at(45, 4, "Lives: ");
    for (int i = 0; i < lives; i++) {
        vga_set_color(0x0C); // Red hearts
        vga_buffer[4 * 80 + (52 + i)] = (0x0C << 8) | HEART_CHAR;
    }
    
    // Controls
    vga_set_color(0x0A);
    vga_puts_at(45, 6, "Controls:");
    vga_puts_at(45, 7, "WASD - Move");
    vga_puts_at(45, 8, "ESC  - Quit");
    
    vga_puts_at(45, 10, "Goal:");
    vga_puts_at(45, 11, "Eat fish (>)");
    vga_puts_at(45, 12, "to gain points!");
}

void vga_puts_at(int x, int y, const char* str) {
    int old_x = cursor_x, old_y = cursor_y;
    cursor_x = x;
    cursor_y = y;
    vga_puts(str);
    cursor_x = old_x;
    cursor_y = old_y;
}

void place_new_fish(void) {
    fish.x = 1 + (rand() % (GAME_WIDTH - 2));
    fish.y = 1 + (rand() % (GAME_HEIGHT - 2));
    
    // Don't spawn on cat
    if (fish.x == cat.x && fish.y == cat.y) {
        place_new_fish();
    }
}

void run_neko_game(void) {
    vga_clear();
    vga_set_color(0x0F);
    vga_puts("Starting Neko Game! Collect fish (>) with your cat (^)\n");
    
    // Wait a bit
    for (volatile int i = 0; i < 10000000; i++);
    
    cat.x = 20;
    cat.y = 10;
    score = 0;
    lives = 3;
    place_new_fish();
    
    int game_running = 1;
    
    while (game_running && lives > 0) {
        // Clear game area
        for (int y = 1; y <= GAME_HEIGHT; y++) {
            for (int x = 1; x <= GAME_WIDTH; x++) {
                vga_buffer[y * 80 + x] = (0x00 << 8) | ' '; // Black background
            }
        }
        
        draw_game_border();
        draw_cat();
        draw_fish();
        draw_ui();
        
        // Check collision with fish
        if (cat.x == fish.x && cat.y == fish.y) {
            score += 10;
            place_new_fish();
        }
        
        // Handle input
        uint8_t scancode = keyboard_get_scancode();
        if (scancode) {
            keyboard_handle_scancode(scancode);
            char c = keyboard_getchar();
            
            if (c == 27) { // ESC
                game_running = 0;
            } else if (c == 'w' || c == 'W') {
                if (cat.y > 1) cat.y--;
            } else if (c == 's' || c == 'S') {
                if (cat.y < GAME_HEIGHT - 2) cat.y++;
            } else if (c == 'a' || c == 'A') {
                if (cat.x > 1) cat.x--;
            } else if (c == 'd' || c == 'D') {
                if (cat.x < GAME_WIDTH - 2) cat.x++;
            }
        }
        
        // Simple "enemy" - random movement for fish sometimes
        if ((rand() % 100) < 10) { // 10% chance fish moves
            int dx = (rand() % 3) - 1; // -1, 0, or 1
            int dy = (rand() % 3) - 1;
            int new_x = fish.x + dx;
            int new_y = fish.y + dy;
            
            if (new_x >= 1 && new_x <= GAME_WIDTH - 2 &&
                new_y >= 1 && new_y <= GAME_HEIGHT - 2) {
                fish.x = new_x;
                fish.y = new_y;
            }
        }
        
        // Simple delay
        for (volatile int i = 0; i < 500000; i++);
    }
    
    vga_clear();
    vga_set_color(0x0F);
    vga_puts("Game Over!\n");
    vga_puts("Final Score: ");
    
    // Display score
    char score_str[20];
    char* ptr = score_str;
    int s = score;
    if (s == 0) {
        *ptr++ = '0';
    } else {
        while (s > 0) {
            *ptr++ = '0' + (s % 10);
            s /= 10;
        }
    }
    *ptr-- = '\0';
    
    // Reverse string
    char* start = score_str;
    while (start < ptr) {
        char tmp = *start;
        *start = *ptr;
        *ptr = tmp;
        start++;
        ptr--;
    }
    
    vga_puts(score_str);
    vga_puts("\n\nPress any key to return to terminal...\n");
    
    // Wait for key press
    while (!keyboard_get_scancode());
}
