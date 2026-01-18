bits 32

section .multiboot
align 4

multiboot_header:
    dd 0x1BADB002              ; Magic
    dd 0x00000003              ; Flags: align + meminfo ONLY (no framebuffer)
    dd -(0x1BADB002 + 0x00000003) ; Checksum
    
    ; NO FRAMEBUFFER REQUEST SECTION
    ; (remove the framebuffer settings)

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
