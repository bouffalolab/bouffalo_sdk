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
#ifndef CONIFG_DISABLE_NOCACHE_RAM_LOAD
extern uint32_t __nocache_ram_data_start__;
extern uint32_t __nocache_ram_data_end__;
#endif

extern uint32_t __StackTop;
extern uint32_t __StackLimit;
extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;

//extern uint32_t __copy_table_start__;
//extern uint32_t __copy_table_end__;
//extern uint32_t __zero_table_start__;
//extern uint32_t __zero_table_end__;

void start_load(void)
{
    uint32_t *pSrc, *pDest;
    uint32_t *pTable __attribute__((unused));

    /* Copy ITCM code */
    pSrc = &__itcm_load_addr;
    pDest = &__tcm_code_start__;

    for (; pDest < &__tcm_code_end__;) {
        *pDest++ = *pSrc++;
    }

    /* Copy DTCM code */
    pSrc = &__dtcm_load_addr;
    pDest = &__tcm_data_start__;

    for (; pDest < &__tcm_data_end__;) {
        *pDest++ = *pSrc++;
    }

    /* Add OCARAM data copy */
    pSrc = &__ram_load_addr;
    pDest = &__ram_data_start__;

    for (; pDest < &__ram_data_end__;) {
        *pDest++ = *pSrc++;
    }

    /* Add psram data copy */
    pSrc = &__psram_load_addr;
    pDest = &__psram_data_start__;

    for (; pDest < &__psram_data_end__;) {
        *pDest++ = *pSrc++;
    }

#ifndef CONIFG_DISABLE_NOCACHE_RAM_LOAD
    /* Add no cache ram data copy */
    pSrc = &__nocache_ram_load_addr;
    pDest = &__nocache_ram_data_start__;

    for (; pDest < &__nocache_ram_data_end__;) {
        *pDest++ = *pSrc++;
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
    pDest = &__bss_start__;

    for (; pDest < &__bss_end__;) {
        *pDest++ = 0ul;
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
    pDest = &__wifi_bss_start;

    for (; pDest < &__wifi_bss_end;) {
        *pDest++ = 0ul;
    }

#endif
}
