bits 32

; ========== MULTIBOOT HEADER ==========
section .multiboot
align 4
multiboot_header:
    dd 0x1BADB002              ; Magic number
    dd 0x00000003              ; Flags: align 4K + memory map
    dd -(0x1BADB002 + 0x00000003) ; Checksum

; ========== ENTRY POINT ==========
section .text
global _start

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Push Multiboot info (ebx = info struct, eax = magic 0x2BADB002)
    push ebx
    push eax
    
    ; Clear direction flag (for string operations)
    cld
    
    ; Call kernel_main(magic, mb_info)
    extern kernel_main
    call kernel_main
    
    ; Hang if kernel returns (shouldn't happen)
    cli
.hang:
    hlt
    jmp .hang

; ========== STACK ==========
section .bss
align 16
stack_bottom:
    resb 16384  ; 16KB stack
stack_top: