bits 32

section .multiboot
align 4

multiboot_header:
    dd 0x1BADB002              ; Magic
    dd 0x00000007              ; Flags: align + meminfo + framebuffer
    dd -(0x1BADB002 + 0x00000007) ; Checksum
    
    ; Framebuffer request
    dd 0                       ; header_addr
    dd 0                       ; load_addr
    dd 0                       ; load_end_addr
    dd 0                       ; bss_end_addr
    dd 0                       ; entry_addr
    dd 0                       ; mode_type (0 = linear graphics)
    dd 1024                    ; width
    dd 768                     ; height
    dd 32                      ; depth (bpp)

section .text
global _start

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Push Multiboot info
    push ebx    ; mb_info
    push eax    ; magic
    
    ; Clear direction flag
    cld
    
    ; Call kernel
    extern kernel_main
    call kernel_main
    
    ; Hang if returns
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
