ENTRY(_start)

SECTIONS
{
    . = 0x7E00;          /* Load kernel after bootloader */

    .text : {
        *(.text)
        *(.rodata*)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(COMMON)
        *(.bss)
    }

    kernel_end = .;
}