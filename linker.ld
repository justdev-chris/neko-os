OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
    /* Kernel loaded at 1MB */
    . = 0x00100000;
    
    /* MULTIBOOT HEADER MUST BE FIRST! */
    .multiboot : {
        *(.multiboot)
    }
    
    .text : ALIGN(4K) {
        *(.text)
    }
    
    .rodata : ALIGN(4K) {
        *(.rodata)
    }
    
    .data : ALIGN(4K) {
        *(.data)
    }
    
    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}