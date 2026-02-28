#include <stdint.h>

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
        while(dest < copy[i].end)
            *dest++ = *src++;
    }

    for (int i = 0; setz[i].start != (void *)0xffffffff; i++) {
        if (!setz[i].start || !setz[i].end)
            continue;
	dest = setz[i].start;
        while(dest < setz[i].end)
            *dest++ = 0UL;
    }
}
