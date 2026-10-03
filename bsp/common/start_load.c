#include <stdint.h>

#ifdef CPU_LP

extern uint32_t __itcm_load_addr;
extern uint32_t __dtcm_load_addr;
extern uint32_t __ram_load_addr;
extern uint32_t __nocache_ram_load_addr;
extern uint32_t __tcm_code_start__;
extern uint32_t __tcm_code_end__;
extern uint32_t __tcm_data_start__;
extern uint32_t __tcm_data_end__;
extern uint32_t __ram_data_start__;
extern uint32_t __ram_data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __nocache_ram_data_start__;
extern uint32_t __nocache_ram_data_end__;

void start_load(void)
{
    uint32_t *src = &__itcm_load_addr;
    uint32_t *dest = &__tcm_code_start__;

    while (dest < &__tcm_code_end__) {
        *dest++ = *src++;
    }

    src = &__dtcm_load_addr;
    dest = &__tcm_data_start__;
    while (dest < &__tcm_data_end__) {
        *dest++ = *src++;
    }

    src = &__ram_load_addr;
    dest = &__ram_data_start__;
    while (dest < &__ram_data_end__) {
        *dest++ = *src++;
    }

    src = &__nocache_ram_load_addr;
    dest = &__nocache_ram_data_start__;
    while (dest < &__nocache_ram_data_end__) {
        *dest++ = *src++;
    }

    dest = &__bss_start__;
    while (dest < &__bss_end__) {
        *dest++ = 0UL;
    }
}

#else

#ifdef CONFIG_PSRAM_XIP
#include "multi_bins.h"

#define MULTI_BINS_MAX_ENTRIES 16

extern const multi_bin_desc_t __multi_bins__[] __attribute__((weak));

void start_load_multi_bins_vma(const multi_bin_desc_t *table, const uint8_t *image_base)
{
    uintptr_t below = UINTPTR_MAX;

    if (table == 0 || image_base == 0) {
        return;
    }

    for (unsigned int loaded = 0; loaded < MULTI_BINS_MAX_ENTRIES; loaded++) {
        const multi_bin_desc_t *selected = 0;

        for (unsigned int i = 0; i < MULTI_BINS_MAX_ENTRIES; i++) {
            const multi_bin_desc_t *desc = &table[i];

            if ((uint8_t)desc->name[0] == 0xff) {
                break;
            }
            /* SELF is already mapped by the boot path; its offsets exclude the firmware header. */
            if (i == 0) {
                continue;
            }
            if (desc->end_addr <= desc->start_addr ||
                desc->vma_end <= desc->vma_start ||
                desc->vma_end - desc->vma_start != desc->end_addr - desc->start_addr ||
                desc->vma_start >= below) {
                continue;
            }
            if (selected == 0 || desc->vma_start > selected->vma_start) {
                selected = desc;
            }
        }

        if (selected == 0) {
            break;
        }

        uintptr_t src = (uintptr_t)image_base + selected->start_addr;
        uintptr_t dest = selected->vma_start;
        uintptr_t length = selected->end_addr - selected->start_addr;

        if (src != dest) {
            if (dest > src && dest - src < length) {
                for (uintptr_t i = length; i > 0; i--) {
                    ((uint8_t *)dest)[i - 1] = ((const uint8_t *)src)[i - 1];
                }
            } else {
                for (uintptr_t i = 0; i < length; i++) {
                    ((uint8_t *)dest)[i] = ((const uint8_t *)src)[i];
                }
            }
        }
        below = selected->vma_start;
    }

}

#endif

struct mem_load_section {
    uint32_t *start;
    uint32_t *end;
    uint32_t *load;
};
struct mem_section {
    uint32_t *start;
    uint32_t *end;
};

extern __attribute__((weak)) struct mem_load_section __mem_copy_sections;
extern __attribute__((weak)) struct mem_section __mem_setz_sections;
#ifdef CONFIG_PSRAM_XIP
extern uint32_t __rom_bss_start__;
extern uint32_t __rom_bss_end__;
extern uint32_t __rom_noinit_start__;
extern uint32_t __rom_noinit_end__;
#endif

void start_load(void)
{
    uint32_t *src, *dest;
    struct mem_load_section *copy = &__mem_copy_sections;
    struct mem_section *setz = &__mem_setz_sections;

    for (int i = 0; copy[i].start != (void *)0xffffffff; i++) {
        if (!copy[i].start || !copy[i].end || !copy[i].load)
            continue;
        src = copy[i].load;
        dest = copy[i].start;
        while (dest < copy[i].end)
            *dest++ = *src++;
    }

    for (int i = 0; setz[i].start != (void *)0xffffffff; i++) {
        if (!setz[i].start || !setz[i].end)
            continue;
        dest = setz[i].start;
        while (dest < setz[i].end)
            *dest++ = 0UL;
    }

#ifdef CONFIG_PSRAM_XIP
    start_load_multi_bins_vma(__multi_bins__, multi_bins_get_image_base());

    dest = &__rom_bss_start__;
    while (dest < &__rom_bss_end__)
        *dest++ = 0UL;

    dest = &__rom_noinit_start__;
    while (dest < &__rom_noinit_end__)
        *dest++ = 0UL;
#endif
}

#endif
