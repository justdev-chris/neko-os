section .text

global idt_load
global irq0_handler
global irq1_handler

extern irq0_handler_c
extern irq1_handler_c

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

%macro IRQ_STUB 1
irq%1_handler:
    pusha
    push ds
    push es
    push fs
    push gs
    
    call irq%1_handler_c
    
    pop gs
    pop fs
    pop es
    pop ds
    popa
    
    iret
%endmacro

IRQ_STUB 0
IRQ_STUB 1
