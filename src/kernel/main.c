void kernel_main(void);

void kernel_main(void) {
    // Clear screen
    char* video_memory = (char*) 0xB8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i+1] = 0x07; // Gray on black
    }
    
    // Print welcome message
    const char* welcome = "NekoOS v0.1";
    char* vga = (char*) 0xB8000;
    
    for (int i = 0; welcome[i] != '\0'; i++) {
        vga[i*2] = welcome[i];
        vga[i*2 + 1] = 0x0F; // White on black
    }
    
    // Print status
    vga = (char*) 0xB8000 + 160; // Second line
    const char* status = "Kernel loaded successfully!";
    for (int i = 0; status[i] != '\0'; i++) {
        vga[i*2] = status[i];
        vga[i*2 + 1] = 0x0A; // Green on black
    }
    
    // Hang
    while (1) {
        asm volatile ("hlt");
    }
}