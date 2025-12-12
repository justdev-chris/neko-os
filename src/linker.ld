ENTRY(_start)

SECTIONS {
    . = 1M;

    .text : ALIGN(4K) {
        *(.multiboot_header)
        *(.text)
    }

    .rodata : ALIGN(4K) {
        *(.rodata*)
    }

    .data : ALIGN(4K) {
        *(.data)
    }

    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }

    /* Add these for better organization */
    .kernel_heap : ALIGN(4K) {
        . = . + 64K;
    }

    .stack : ALIGN(4K) {
        . = . + 16K;
    }
}