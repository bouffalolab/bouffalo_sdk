#include <stdint.h>

#define __STARTUP_CLEAR_BSS      1
#define __STARTUP_CLEAR_WIFI_BSS 1

/*----------------------------------------------------------------------------
  Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t __itcm_load__;
extern uint32_t __dtcm_load__;
extern uint32_t __ram_data_load__;
extern uint32_t __psram_data_load__;

extern uint32_t __itcm_start__;
extern uint32_t __itcm_end__;
extern uint32_t __dtcm_start__;
extern uint32_t __dtcm_end__;
extern uint32_t __ram_data_start__;
extern uint32_t __ram_data_end__;
extern uint32_t __psram_data_start__;
extern uint32_t __psram_data_end__;

#ifndef CONFIG_DISABLE_NOCACHE_RAM_LOAD
extern uint32_t __ram_nocache_data_start__;
extern uint32_t __ram_nocache_data_end__;
extern uint32_t __ram_nocache_data_load__;
#endif

extern uint32_t __ram_bss_start__;
extern uint32_t __ram_bss_end__;
extern uint32_t __wifi_bss_start__;
extern uint32_t __wifi_bss_end__;
extern uint32_t __noinit_data_start__;
extern uint32_t __noinit_data_end__;

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
    pSrc = &__itcm_load__;
    pDest = &__itcm_start__;

    for (; pDest < &__itcm_end__;) {
        *pDest++ = *pSrc++;
    }

    /* Copy DTCM code */
    pSrc = &__dtcm_load__;
    pDest = &__dtcm_start__;

    for (; pDest < &__dtcm_end__;) {
        *pDest++ = *pSrc++;
    }

    /* BF Add OCARAM data copy */
    pSrc = &__ram_data_load__;
    pDest = &__ram_data_start__;

    for (; pDest < &__ram_data_end__;) {
        *pDest++ = *pSrc++;
    }

    /* BF Add psram data copy */
    pSrc = &__psram_data_load__;
    pDest = &__psram_data_start__;

    for (; pDest < &__psram_data_end__;) {
        *pDest++ = *pSrc++;
    }

#ifndef CONFIG_DISABLE_NOCACHE_RAM_LOAD
    /* BF Add no cache ram data copy */
    pSrc = &__ram_nocache_data_load__;
    pDest = &__ram_nocache_data_start__;

    for (; pDest < &__ram_nocache_data_end__;) {
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
    pDest = &__ram_bss_start__;

    for (; pDest < &__ram_bss_end__;) {
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
    pDest = &__wifi_bss_start__;

    for (; pDest < &__wifi_bss_end__;) {
        *pDest++ = 0ul;
    }

#endif
}
