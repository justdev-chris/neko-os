section .multiboot
align 4

multiboot_header:
    dd 0x1BADB002              ; Magic number
    dd 0x00000003              ; Flags: align modules on page boundaries + provide memory map
    dd -(0x1BADB002 + 0x00000003) ; Checksum
    
    ; Additional Multiboot tags (optional)
    dd 0x00000000              ; header_addr
    dd 0x00000000              ; load_addr
    dd 0x00000000              ; load_end_addr
    dd 0x00000000              ; bss_end_addr
    dd 0x00000000              ; entry_addr
    dd 0x00000000              ; mode_type
    dd 0x00000000              ; width
    dd 0x00000000              ; height
    dd 0x00000000              ; depth

section .text
global _start

_start:
    ; Set up stack pointer
    mov esp, stack_top
    
    ; Push Multiboot info pointer (passed in EBX by bootloader)
    push ebx
    ; Push Multiboot magic number (passed in EAX by bootloader)
    push eax
    
    ; Call kernel_main (C function)
    extern kernel_main
    call kernel_main
    
    ; If kernel_main returns (shouldn't happen), halt
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384  ; 16KB stack
stack_top:
