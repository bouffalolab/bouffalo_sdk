#include "bflb_core.h"
#include <risc-v/bflb_rv_privilege.h>

#if defined(BL602) || defined(BL702) || defined(BL702L)
#include <risc-v/e24/clic.h>
#else
#if !defined(BL618DG) || defined(CPU_MODEL_A0) || defined(CPU_LP)
#include <csi_core.h>
#else
#include <nmsis_core.h>
#endif
#endif

#define BFLB_RV_STRINGIFY_VALUE(x) #x
#define BFLB_RV_STRINGIFY(x)       BFLB_RV_STRINGIFY_VALUE(x)

#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
#define BFLB_RV_XSTATUS_CSR sstatus
#define BFLB_RV_XSTATUS_IE  0x2
#else
#define BFLB_RV_XSTATUS_CSR mstatus
#define BFLB_RV_XSTATUS_IE  0x8
#endif

ATTR_TCM_SECTION uintptr_t bflb_rv_irq_save(void)
{
    uintptr_t oldstat;

    asm volatile("csrrc %0, " BFLB_RV_STRINGIFY(BFLB_RV_XSTATUS_CSR) ", %1"
                 : "=r"(oldstat)
                 : "r"(BFLB_RV_XSTATUS_IE)
                 : "memory");
    return oldstat;
}

ATTR_TCM_SECTION void bflb_rv_irq_restore(uintptr_t flags)
{
    asm volatile("csrw " BFLB_RV_STRINGIFY(BFLB_RV_XSTATUS_CSR) ", %0"
                 :
                 : "r"(flags)
                 : "memory");
}

#if defined(BL618DG) && !defined(CPU_MODEL_A0) && !defined(CPU_LP)
void bflb_rv_eclic_enable_irq(int irq)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    __ECLIC_EnableIRQ_S(irq);
#else
    __ECLIC_EnableIRQ(irq);
#endif
}

void bflb_rv_eclic_disable_irq(int irq)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    __ECLIC_DisableIRQ_S(irq);
#else
    __ECLIC_DisableIRQ(irq);
#endif
}

void bflb_rv_eclic_set_pending_irq(int irq)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    ECLIC->SCTRL[irq].INTIP |= CLIC_INTIP_IP_Msk;
#else
    __ECLIC_SetPendingIRQ(irq);
#endif
}

void bflb_rv_eclic_clear_pending_irq(int irq)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    ECLIC->SCTRL[irq].INTIP &= ~CLIC_INTIP_IP_Msk;
#else
    __ECLIC_ClearPendingIRQ(irq);
#endif
}

void bflb_rv_eclic_set_nlbits(uint8_t nlbits)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    (void)nlbits;
#else
    __ECLIC_SetCfgNlbits(nlbits);
#endif
}

void bflb_rv_eclic_set_level_irq(int irq, uint8_t level)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    __ECLIC_SetLevelIRQ_S(irq, level);
#else
    __ECLIC_SetLevelIRQ(irq, level);
#endif
}

void bflb_rv_eclic_set_priority_irq(int irq, uint8_t priority)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    __ECLIC_SetPriorityIRQ_S(irq, priority);
#else
    __ECLIC_SetPriorityIRQ(irq, priority);
#endif
}

/* Cache enable/disable touches MCACHE_CTL, so S-mode only does maintenance. */
void bflb_rv_icache_enable(void)
{
#if !defined(CONFIG_RV_PRIVILEGE_MODE_S)
    EnableICache();
#endif
}

void bflb_rv_icache_disable(void)
{
#if !defined(CONFIG_RV_PRIVILEGE_MODE_S)
    DisableICache();
#endif
}

ATTR_TCM_SECTION void bflb_rv_icache_invalid_all(void)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    SInvalICache();
#else
    MInvalICache();
#endif
}

ATTR_TCM_SECTION void bflb_rv_icache_invalid_range(unsigned long addr, unsigned long size)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    SInvalICacheRange(addr, size);
#else
    MInvalICacheRange(addr, size);
#endif
}

void bflb_rv_dcache_enable(void)
{
#if !defined(CONFIG_RV_PRIVILEGE_MODE_S)
    EnableDCache();
#endif
}

void bflb_rv_dcache_disable(void)
{
#if !defined(CONFIG_RV_PRIVILEGE_MODE_S)
    DisableDCache();
#endif
}

ATTR_TCM_SECTION void bflb_rv_dcache_clean_all(void)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    SFlushDCache();
#else
    MFlushDCache();
#endif
}

ATTR_TCM_SECTION void bflb_rv_dcache_invalidate_all(void)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    SInvalDCache();
#else
    MInvalDCache();
#endif
}

ATTR_TCM_SECTION void bflb_rv_dcache_clean_invalidate_all(void)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    SFlushInvalDCache();
#else
    MFlushInvalDCache();
#endif
}

ATTR_TCM_SECTION void bflb_rv_dcache_clean_range(unsigned long addr, unsigned long size)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    SFlushDCacheRange(addr, size);
#else
    MFlushDCacheRange(addr, size);
#endif
}

ATTR_TCM_SECTION void bflb_rv_dcache_invalidate_range(unsigned long addr, unsigned long size)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    SInvalDCacheRange(addr, size);
#else
    MInvalDCacheRange(addr, size);
#endif
}

ATTR_TCM_SECTION void bflb_rv_dcache_clean_invalidate_range(unsigned long addr, unsigned long size)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    SFlushInvalDCacheRange(addr, size);
#else
    MFlushInvalDCacheRange(addr, size);
#endif
}

int bflb_rv_systimer_irqn(void)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
#if defined(SysTimer_S_IRQn)
    return SysTimer_S_IRQn;
#elif defined(STIME_IRQn)
    return STIME_IRQn;
#else
    return 5;
#endif
#else
#if defined(SysTimer_IRQn)
    return SysTimer_IRQn;
#else
    return 7;
#endif
#endif
}

ATTR_TCM_SECTION uint64_t bflb_rv_systimer_get_time(void)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    return __get_rv_time();
#else
    return SysTimer_GetLoadValue();
#endif
}

ATTR_TCM_SECTION void bflb_rv_systimer_set_compare(uint64_t value)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    SysTimer_SetCompareValue_S(value);
#else
    SysTimer_SetCompareValue(value);
#endif
}

ATTR_TCM_SECTION uint64_t bflb_rv_systimer_get_compare(void)
{
#if defined(CONFIG_RV_PRIVILEGE_MODE_S)
    return SysTimer_GetCompareValue_S();
#else
    return SysTimer_GetCompareValue();
#endif
}
#endif
