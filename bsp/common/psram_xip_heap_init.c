#include <stdint.h>
#include <stdio.h>

#include "mm.h"
#include "multi_bins.h"
#include "psram_heap_init.h"

#define PSRAM_HEAP_ALIGNMENT 32U

extern uint32_t __psram_limit;

void ram_psram_heap_init(void)
{
    uintptr_t heap_base;
    uintptr_t heap_limit = (uintptr_t)&__psram_limit;
    size_t heap_len;

    multi_bin_desc_t self;
    uint32_t free_vma_start;

    if (multi_bins_get_desc("SELF", &self) < 0 ||
        self.vma_end <= self.vma_start || self.vma_end > heap_limit ||
        multi_bins_get_free_vma_start_below(&free_vma_start, heap_limit) < 0 ||
        free_vma_start < self.vma_end) {
        puts("psram xip vma invalid !!!\r\n");
        while (1) {}
    }

    heap_base = ((uintptr_t)free_vma_start + PSRAM_HEAP_ALIGNMENT - 1U) &
                ~(uintptr_t)(PSRAM_HEAP_ALIGNMENT - 1U);
    if (heap_base < free_vma_start || heap_base >= heap_limit) {
        puts("psram xip heap overflow !!!\r\n");
        while (1) {}
    }

    heap_len = heap_limit - heap_base;
    if (mm_register_heap(MM_HEAP_PSRAM_0, "PSRAM", MM_ALLOCATOR_TLSF,
                         (void *)heap_base, heap_len) == NULL) {
        puts("psram xip heap init fail !!!\r\n");
        while (1) {}
    }

    printf("  psram heap size: %u Kbyte\r\n", (unsigned int)(heap_len / 1024U));
}
