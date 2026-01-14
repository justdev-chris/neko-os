ENTRY(_start)

SECTIONS
{
    . = 1M;  /* Load at 1MB (multiboot standard) */

    .multiboot : {
        *(.multiboot_header)
    }

    .text : {
        *(.text)
    }
    
    /* ... rest same ... */
}