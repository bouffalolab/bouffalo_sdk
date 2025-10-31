#include <stdint.h>

#define __STARTUP_CLEAR_BSS      1
#define __STARTUP_CLEAR_WIFI_BSS 1

/*----------------------------------------------------------------------------
  Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t __itcm_load_addr;
extern uint32_t __dtcm_load_addr;
extern uint32_t __ram_load_addr;
extern uint32_t __nocache_ram_load_addr;
extern uint32_t __psram_load_addr;

extern uint32_t __text_code_start__;
extern uint32_t __text_code_end__;
extern uint32_t __tcm_code_start__;
extern uint32_t __tcm_code_end__;
extern uint32_t __tcm_data_start__;
extern uint32_t __tcm_data_end__;
extern uint32_t __ram_data_start__;
extern uint32_t __ram_data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __wifi_bss_start;
extern uint32_t __wifi_bss_end;
extern uint32_t __noinit_data_start__;
extern uint32_t __noinit_data_end__;
extern uint32_t __psram_data_start__;
extern uint32_t __psram_data_end__;
#ifndef CONFIG_DISABLE_NOCACHE_RAM_LOAD
extern uint32_t __nocache_ram_data_start__;
extern uint32_t __nocache_ram_data_end__;
#endif

extern uint32_t __StackTop;
extern uint32_t __StackLimit;
extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;

void start_load(void)
{
    uint32_t *tmp_src, *tmp_dst;
    uint32_t *pTable __attribute__((unused));

    /* Copy ITCM code */
    tmp_src = &__itcm_load_addr;
    tmp_dst = &__tcm_code_start__;

    for (; tmp_dst < &__tcm_code_end__;) {
        *tmp_dst++ = *tmp_src++;
    }

    /* Copy DTCM code */
    tmp_src = &__dtcm_load_addr;
    tmp_dst = &__tcm_data_start__;

    for (; tmp_dst < &__tcm_data_end__;) {
        *tmp_dst++ = *tmp_src++;
    }

    /* Add OCARAM data copy */
    tmp_src = &__ram_load_addr;
    tmp_dst = &__ram_data_start__;

    for (; tmp_dst < &__ram_data_end__;) {
        *tmp_dst++ = *tmp_src++;
    }

#ifdef CONFIG_PSRAM
    /* check efuse */
#define PSRAM_EF_INFO_ADDR (0x20056000 + 0x18)
#define PSRAM_EF_INFO_MASK (0x0000003 << 24)
    if (*((volatile uint32_t *)(PSRAM_EF_INFO_ADDR)) & PSRAM_EF_INFO_MASK) {
        /* Add psram data copy */
        tmp_src = &__psram_load_addr;
        tmp_dst = &__psram_data_start__;

        for (; tmp_dst < &__psram_data_end__;) {
            *tmp_dst++ = *tmp_src++;
        }
    }
#endif

#ifndef CONFIG_DISABLE_NOCACHE_RAM_LOAD
    /* Add no cache ram data copy */
    tmp_src = &__nocache_ram_load_addr;
    tmp_dst = &__nocache_ram_data_start__;

    for (; tmp_dst < &__nocache_ram_data_end__;) {
        *tmp_dst++ = *tmp_src++;
    }
#endif

#ifdef __STARTUP_CLEAR_BSS
    /*  Single BSS section scheme.
	 *
	 *  The BSS section is specified by following symbols
	 *    __bss_start__: start of the BSS section.
	 *    __bss_end__: end of the BSS section.
	 *
	 *  Both addresses must be aligned to 4 bytes boundary.
	 */
    tmp_dst = &__bss_start__;

    for (; tmp_dst < &__bss_end__;) {
        *tmp_dst++ = 0ul;
    }

#endif
#ifdef __STARTUP_CLEAR_WIFI_BSS
    /*  Single BSS section scheme.
	 *
	 *  The BSS section is specified by following symbols
	 *    __wifi_bss_start: start of the BSS section.
	 *    __wifi_bss_end: end of the BSS section.
	 *
	 *  Both addresses must be aligned to 4 bytes boundary.
	 */
    tmp_dst = &__wifi_bss_start;

    for (; tmp_dst < &__wifi_bss_end;) {
        *tmp_dst++ = 0ul;
    }

#endif
}
