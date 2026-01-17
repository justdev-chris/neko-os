#include <stdint.h>

void parse_multiboot_tags(uint32_t addr) {
    uint32_t *tag = (uint32_t*)addr;
    uint32_t type = tag[0];
    uint32_t size = tag[1];
    
    (void)type;
    (void)size;
    
    while (type != 0) {
        tag = (uint32_t*)((uintptr_t)tag + ((size + 7) & ~7));
        type = tag[0];
        size = tag[1];
    }
}
