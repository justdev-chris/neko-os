; NASM syntax for interrupts.asm
section .text

; Export symbols
global idt_load
global irq0_handler
global irq1_handler

; Import C handler functions
extern irq0_handler_c
extern irq1_handler_c

; IDT load function
idt_load:
    mov eax, [esp + 4]    ; Get IDT pointer
    lidt [eax]            ; Load IDT
    ret

; Common interrupt stub macro
%macro IRQ_STUB 1
irq%1_handler:
    pusha                  ; Push all general purpose registers
    push ds                ; Push data segment
    push es                ; Push extra segment
    push fs                ; Push FS segment
    push gs                ; Push GS segment
    
    ; Call C handler
    call irq%1_handler_c
    
    pop gs                 ; Restore segments
    pop fs
    pop es
    pop ds
    popa                   ; Restore registers
    
    iret                   ; Return from interrupt
%endmacro

; Generate IRQ stubs
IRQ_STUB 0
IRQ_STUB 1
