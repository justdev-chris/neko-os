section .multiboot
align 4

multiboot_header:
    dd 0x1BADB002              ; Magic number
    
    ; FLAGS: bit 0 = align modules, bit 1 = memory map, bit 2 = framebuffer
    dd 0x00000007              ; CHANGED FROM 0x00000003
    
    dd -(0x1BADB002 + 0x00000007) ; Updated checksum
    
    ; Framebuffer request fields (must be set if bit 2 in flags is set)
    dd 0                       ; header_addr
    dd 0                       ; load_addr  
    dd 0                       ; load_end_addr
    dd 0                       ; bss_end_addr
    dd 0                       ; entry_addr
    
    ; Framebuffer mode request
    dd 0                       ; mode_type (0 = linear graphics)
    dd 1024                    ; width
    dd 768                     ; height
    dd 32                      ; depth (bpp)