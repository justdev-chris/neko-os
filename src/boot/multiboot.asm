; multiboot.asm
section .multiboot
align 4

; Multiboot header
multiboot_header:
    dd 0x1BADB002              ; magic
    dd 0x00000003              ; flags
    dd -(0x1BADB002 + 0x00000003) ; checksum
    
    ; Optional fields (set to 0 since we don't need them)
    dd 0x00000000              ; header_addr
    dd 0x00000000              ; load_addr  
    dd 0x00000000              ; load_end_addr
    dd 0x00000000              ; bss_end_addr
    dd 0x00000000              ; entry_addr
    
    ; Video mode (0 = linear graphics)
    dd 0x00000000              ; mode_type
    dd 0x00000000              ; width
    dd 0x00000000              ; height
    dd 0x00000000              ; depth
