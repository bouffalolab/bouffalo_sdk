#include <stdint.h>
#include <stdio.h>

#include "mm.h"
#include "psram_heap_init.h"

extern uint32_t __psram_heap_base;
extern uint32_t __psram_limit;

void ram_psram_heap_init(void)
{
    size_t heap_len = (size_t)&__psram_limit - (size_t)&__psram_heap_base;

    mm_register_heap(MM_HEAP_PSRAM_0, "PSRAM", MM_ALLOCATOR_TLSF,
                     &__psram_heap_base, heap_len);
    printf("  psram heap size: %u Kbyte\r\n", (unsigned int)(heap_len / 1024U));
}
