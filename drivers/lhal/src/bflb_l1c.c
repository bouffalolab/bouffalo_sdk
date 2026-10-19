#include "bflb_l1c.h"
#include "bflb_core.h"

#if defined(BL616) ||   \
    defined(BL616CL) || \
    (defined(BL618DG) && defined(CPU_MODEL_A0) && !defined(CPU_LP))
/* Xuantie T-head  */
#include "csi_core.h"

void bflb_l1c_icache_enable(void)
{
#ifdef romapi_bflb_l1c_icache_enable
    romapi_bflb_l1c_icache_enable();
#else
    csi_icache_enable();
#endif
}

void bflb_l1c_icache_disable(void)
{
#ifdef romapi_bflb_l1c_icache_disable
    romapi_bflb_l1c_icache_disable();
#else
    csi_icache_disable();
#endif
}

ATTR_TCM_SECTION void bflb_l1c_icache_invalid_all(void)
{
#ifdef romapi_bflb_l1c_icache_invalid_all
    romapi_bflb_l1c_icache_invalid_all();
#else
    csi_icache_invalid();
#endif
}

ATTR_TCM_SECTION void bflb_l1c_icache_invalid_range(void *addr, uint32_t size)
{
#ifdef romapi_bflb_l1c_icache_invalid_range
    romapi_bflb_l1c_icache_invalid_range(addr, size);
#else
    if (bflb_check_cache_addr(addr)) {
        csi_icache_invalid_range(addr, size);
    }
#endif
}

void bflb_l1c_dcache_enable(void)
{
#ifdef romapi_bflb_l1c_dcache_enable
    romapi_bflb_l1c_dcache_enable();
#else
    csi_dcache_enable();
#endif
}

void bflb_l1c_dcache_disable(void)
{
#ifdef romapi_bflb_l1c_dcache_disable
    romapi_bflb_l1c_dcache_disable();
#else
    csi_dcache_clean_invalid();
    csi_dcache_disable();
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_all(void)
{
#ifdef romapi_bflb_l1c_dcache_clean_all
    romapi_bflb_l1c_dcache_clean_all();
#else
    csi_dcache_clean();
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_invalidate_all(void)
{
#ifdef romapi_bflb_l1c_dcache_invalidate_all
    romapi_bflb_l1c_dcache_invalidate_all();
#else
    csi_dcache_invalid();
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_invalidate_all(void)
{
#ifdef romapi_bflb_l1c_dcache_clean_invalidate_all
    romapi_bflb_l1c_dcache_clean_invalidate_all();
#else
    csi_dcache_clean_invalid();
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_range(void *addr, uint32_t size)
{
#ifdef romapi_bflb_l1c_dcache_clean_range
    romapi_bflb_l1c_dcache_clean_range(addr, size);
#else
    if (bflb_check_cache_addr(addr)) {
        csi_dcache_clean_range(addr, size);
    }
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_invalidate_range(void *addr, uint32_t size)
{
#ifdef romapi_bflb_l1c_dcache_invalidate_range
    romapi_bflb_l1c_dcache_invalidate_range(addr, size);
#else
    if (bflb_check_cache_addr(addr)) {
        csi_dcache_invalid_range(addr, size);
    }
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_invalidate_range(void *addr, uint32_t size)
{
#ifdef romapi_bflb_l1c_dcache_clean_invalidate_range
    romapi_bflb_l1c_dcache_clean_invalidate_range(addr, size);
#else
    if (bflb_check_cache_addr(addr)) {
        csi_dcache_clean_invalid_range(addr, size);
    }
#endif
}

#elif (defined(BL618DG) && !defined(CPU_MODEL_A0) && !defined(CPU_LP))
/* Nuclei */
#include "nmsis_core.h"
#include <risc-v/bflb_rv_privilege.h>

void bflb_l1c_icache_enable(void)
{
#ifdef romapi_bflb_l1c_icache_enable
    romapi_bflb_l1c_icache_enable();
#else
    uintptr_t flag = bflb_irq_save();
    bflb_rv_icache_enable();
    bflb_irq_restore(flag);
#endif
}

void bflb_l1c_icache_disable(void)
{
#ifdef romapi_bflb_l1c_icache_disable
    romapi_bflb_l1c_icache_disable();
#else
    uintptr_t flag = bflb_irq_save();
    bflb_rv_icache_disable();
    bflb_irq_restore(flag);
#endif
}

ATTR_TCM_SECTION void bflb_l1c_icache_invalid_all(void)
{
#ifdef romapi_bflb_l1c_icache_invalid_all
    romapi_bflb_l1c_icache_invalid_all();
#else
    uintptr_t flag = bflb_irq_save();
    bflb_rv_icache_invalid_all();
    bflb_irq_restore(flag);
#endif
}

ATTR_TCM_SECTION void bflb_l1c_icache_invalid_range(void *addr, uint32_t size)
{
#ifdef romapi_bflb_l1c_icache_invalid_range
    romapi_bflb_l1c_icache_invalid_range(addr, size);
#else
    if (bflb_check_cache_addr(addr)) {
        uintptr_t flag = bflb_irq_save();
        bflb_rv_icache_invalid_range((unsigned long)addr, size);
        bflb_irq_restore(flag);
    }
#endif
}

void bflb_l1c_dcache_enable(void)
{
#ifdef romapi_bflb_l1c_dcache_enable
    romapi_bflb_l1c_dcache_enable();
#else
    uintptr_t flag = bflb_irq_save();
    bflb_rv_dcache_enable();
    bflb_irq_restore(flag);
#endif
}

void bflb_l1c_dcache_disable(void)
{
#ifdef romapi_bflb_l1c_dcache_disable
    romapi_bflb_l1c_dcache_disable();
#else
    uintptr_t flag = bflb_irq_save();
    bflb_rv_dcache_disable();
    bflb_irq_restore(flag);
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_all(void)
{
#ifdef romapi_bflb_l1c_dcache_clean_all
    romapi_bflb_l1c_dcache_clean_all();
#else
    uintptr_t flag = bflb_irq_save();
    bflb_rv_dcache_clean_all();
    bflb_irq_restore(flag);
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_invalidate_all(void)
{
#ifdef romapi_bflb_l1c_dcache_invalidate_all
    romapi_bflb_l1c_dcache_invalidate_all();
#else
    uintptr_t flag = bflb_irq_save();
    bflb_rv_dcache_invalidate_all();
    bflb_irq_restore(flag);
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_invalidate_all(void)
{
#ifdef romapi_bflb_l1c_dcache_clean_invalidate_all
    romapi_bflb_l1c_dcache_clean_invalidate_all();
#else
    uintptr_t flag = bflb_irq_save();
    bflb_rv_dcache_clean_invalidate_all();
    bflb_irq_restore(flag);
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_range(void *addr, uint32_t size)
{
#ifdef romapi_bflb_l1c_dcache_clean_range
    romapi_bflb_l1c_dcache_clean_range(addr, size);
#else
    if (bflb_check_cache_addr(addr)) {
        uintptr_t flag = bflb_irq_save();
        bflb_rv_dcache_clean_range((unsigned long)addr, size);
        bflb_irq_restore(flag);
    }
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_invalidate_range(void *addr, uint32_t size)
{
#ifdef romapi_bflb_l1c_dcache_invalidate_range
    romapi_bflb_l1c_dcache_invalidate_range(addr, size);
#else
    if (bflb_check_cache_addr(addr)) {
        uintptr_t flag = bflb_irq_save();
        bflb_rv_dcache_invalidate_range((unsigned long)addr, size);
        bflb_irq_restore(flag);
    }
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_invalidate_range(void *addr, uint32_t size)
{
#ifdef romapi_bflb_l1c_dcache_clean_invalidate_range
    romapi_bflb_l1c_dcache_clean_invalidate_range(addr, size);
#else
    if (bflb_check_cache_addr(addr)) {
        uintptr_t flag = bflb_irq_save();
        bflb_rv_dcache_clean_invalidate_range((unsigned long)addr, size);
        bflb_irq_restore(flag);
    }
#endif
}

#elif defined(BL602) || defined(BL702) || defined(BL702L)
/* Private cache implementation */

#if defined(BL702) || defined(BL702L)
extern void L1C_Cache_Enable_Set(uint8_t wayDisable);
extern void L1C_Cache_Flush(void);
#elif defined(BL602)
extern int bflb_sflash_cache_flush(void);
#endif

void bflb_l1c_icache_enable(void)
{
}

void bflb_l1c_icache_disable(void)
{
#if defined(BL702) || defined(BL702L)
    L1C_Cache_Enable_Set(0x0f);
#endif
}

void bflb_l1c_icache_invalid_all(void)
{
}

void bflb_l1c_icache_invalid_range(void *addr, uint32_t size)
{
}

void bflb_l1c_dcache_enable(void)
{
}

void bflb_l1c_dcache_disable(void)
{
}

void bflb_l1c_dcache_clean_all(void)
{
}

void bflb_l1c_dcache_invalidate_all(void)
{
#if defined(BL702) || defined(BL702L)
    L1C_Cache_Flush();
#elif defined(BL602)
    bflb_sflash_cache_flush();
#endif
}

void bflb_l1c_dcache_clean_invalidate_all(void)
{
#if defined(BL702) || defined(BL702L)
    L1C_Cache_Flush();
#elif defined(BL602)
    bflb_sflash_cache_flush();
#endif
}

void bflb_l1c_dcache_clean_range(void *addr, uint32_t size)
{
}

ATTR_TCM_SECTION void bflb_l1c_dcache_invalidate_range(void *addr, uint32_t size)
{
#if defined(BL702) || defined(BL702L)
    L1C_Cache_Flush();
#elif defined(BL602)
    bflb_sflash_cache_flush();
#endif
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_invalidate_range(void *addr, uint32_t size)
{
#if defined(BL702) || defined(BL702L)
    L1C_Cache_Flush();
#elif defined(BL602)
    bflb_sflash_cache_flush();
#endif
}

#if defined(BL702) || defined(BL702L)
/****************************************************************************/ /**
 * @brief  L1C cache write set
 *
 * @param  wt_en: L1C write through enable
 * @param  wb_en: L1C write back enable
 * @param  wa_en: L1C write allocate enable
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION bflb_l1c_cache_write_set(uint8_t wt_en, uint8_t wb_en, uint8_t wa_en)
{
    uint32_t regval = 0;

    regval = getreg32(0x40009000 + 0x0);

    if (wt_en) {
        regval |= (1 << 4);
    } else {
        regval &= ~(1 << 4);
    }

    if (wb_en) {
        regval |= (1 << 5);
    } else {
        regval &= ~(1 << 5);
    }

    if (wa_en) {
        regval |= (1 << 6);
    } else {
        regval &= ~(1 << 6);
    }

    putreg32(regval, 0x40009000 + 0x0);
}
#endif

/****************************************************************************/ /**
 * @brief  Get hit count
 *
 * @param  hit_count_low: hit count low 32 bits pointer
 * @param  hit_count_high: hit count high 32 bits pointer
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION bflb_l1c_hit_count_get(uint32_t *hit_count_low, uint32_t *hit_count_high)
{
    *hit_count_low = getreg32(0x40009000 + 0x4);
    *hit_count_high = getreg32(0x40009000 + 0x8);
}

/****************************************************************************/ /**
 * @brief  Get miss count
 *
 * @param  None
 *
 * @return Miss count
 *
*******************************************************************************/
__WEAK
uint32_t ATTR_TCM_SECTION bflb_l1c_miss_count_get(void)
{
    return getreg32(0x40009000 + 0xC);
}

#else
//#error "Unsupported chip or CPU model for L1C driver"
#endif
