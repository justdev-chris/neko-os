.section .text

# Export symbols
.global idt_load
.global irq0_handler
.global irq1_handler

# IDT load function
idt_load:
    movl 4(%esp), %eax    # Get IDT pointer
    lidt (%eax)           # Load IDT
    ret

# Common interrupt stub macro
.macro IRQ_STUB num
irq\num\()_handler:
    pushal                # Push all general purpose registers
    pushl %ds             # Push data segment
    pushl %es             # Push extra segment
    pushl %fs             # Push FS segment
    pushl %gs             # Push GS segment
    
    # Call C handler
    call irq\num\()_handler_c
    
    popl %gs              # Restore segments
    popl %fs
    popl %es
    popl %ds
    popal                 # Restore registers
    
    iret                  # Return from interrupt
.endm

# Generate IRQ stubs
IRQ_STUB 0
IRQ_STUB 1
