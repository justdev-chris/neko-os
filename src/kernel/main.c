void kernel_main() {
    // Direct VGA write - no functions, no headers
    volatile char *vga = (volatile char*)0xB8000;
    
    // Write "NEKOOS" in center
    vga[ (12 * 80 + 37) * 2 ] = 'N';
    vga[ (12 * 80 + 38) * 2 ] = 'E';
    vga[ (12 * 80 + 39) * 2 ] = 'K';
    vga[ (12 * 80 + 40) * 2 ] = 'O';
    vga[ (12 * 80 + 41) * 2 ] = 'O';
    vga[ (12 * 80 + 42) * 2 ] = 'S';
    
    // White text
    for(int i = 0; i < 6; i++) {
        vga[ (12 * 80 + 37 + i) * 2 + 1 ] = 0x0F;
    }
    
    // Hang forever
    while(1) asm("hlt");
}