/* linker.ld - Linker script for NekoOS */
OUTPUT_FORMAT(elf32-i386)
ENTRY(kernel_main)

SECTIONS {
    /* Load kernel at 1MB (conventional for kernels) */
    . = 1M;
    
    /* Multiboot header must be at the beginning */
    .multiboot : {
        *(.multiboot)
    }
    
    /* Text section */
    .text : ALIGN(4K) {
        *(.text)
    }
    
    /* Read-only data */
    .rodata : ALIGN(4K) {
        *(.rodata)
    }
    
    /* Data section */
    .data : ALIGN(4K) {
        *(.data)
    }
    
    /* BSS section (uninitialized data) */
    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
    
    /* Ensure the end is aligned */
    . = ALIGN(4K);
    end = .;
}
