section .multiboot
align 4
    dd 0x1BADB002           ; Magic number
    dd 0x00000007           ; Flags: page align + memory info + video mode
    dd -(0x1BADB002 + 0x00000007) ; Checksum
    
    ; Multiboot header extensions for framebuffer
    dd 0                    ; header_addr
    dd 0                    ; load_addr
    dd 0                    ; load_end_addr
    dd 0                    ; bss_end_addr
    dd 0                    ; entry_addr
    dd 0                    ; mode_type (0 = linear graphics)
    dd 1024                 ; width
    dd 768                  ; height
    dd 32                   ; depth (bits per pixel)

section .text
global start
extern kernel_main

start:
    push ebx                ; Push multiboot info pointer
    push eax                ; Push magic number
    call kernel_main

halt:
    cli
    hlt
    jmp halt
