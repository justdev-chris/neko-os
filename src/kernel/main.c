void kernel_main() {
    volatile char *vga = (volatile char*)0xB8000;
    
    // VGA Code Page 437 indices
    char word[] = {0x4E, 0x45, 0x4B, 0x4F, 0x4F, 0x53}; // N E K O O S
    
    for(int i = 0; i < 6; i++) {
        vga[(12 * 80 + 37 + i) * 2] = word[i];      // Character
        vga[(12 * 80 + 37 + i) * 2 + 1] = 0x0F;     // White on black
    }
    
    while(1) asm("hlt");
}