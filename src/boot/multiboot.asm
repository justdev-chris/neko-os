section .multiboot
align 4
    dd 0x1BADB002           ; Magic number
    dd 0x00000003           ; Flags: page align + memory info (NO video mode)
    dd -(0x1BADB002 + 0x00000003) ; Checksum

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
