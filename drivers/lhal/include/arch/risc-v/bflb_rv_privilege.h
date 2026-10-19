#ifndef _BFLB_RV_PRIVILEGE_H
#define _BFLB_RV_PRIVILEGE_H

#include <stdint.h>

#if defined(BL618DG) && !defined(CPU_MODEL_A0) && !defined(CPU_LP)
#if defined(CONFIG_RV_PRIVILEGE_MODE_S) && (!defined(__TEE_PRESENT) || (__TEE_PRESENT != 1))
#error "CONFIG_RV_PRIVILEGE_MODE_S on BL618DG N900 requires __TEE_PRESENT=1"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

uintptr_t bflb_rv_irq_save(void);
void bflb_rv_irq_restore(uintptr_t flags);

#if defined(BL618DG) && !defined(CPU_MODEL_A0) && !defined(CPU_LP)
void bflb_rv_eclic_enable_irq(int irq);
void bflb_rv_eclic_disable_irq(int irq);
void bflb_rv_eclic_set_pending_irq(int irq);
void bflb_rv_eclic_clear_pending_irq(int irq);
void bflb_rv_eclic_set_nlbits(uint8_t nlbits);
void bflb_rv_eclic_set_level_irq(int irq, uint8_t level);
void bflb_rv_eclic_set_priority_irq(int irq, uint8_t priority);

void bflb_rv_icache_enable(void);
void bflb_rv_icache_disable(void);
void bflb_rv_icache_invalid_all(void);
void bflb_rv_icache_invalid_range(unsigned long addr, unsigned long size);
void bflb_rv_dcache_enable(void);
void bflb_rv_dcache_disable(void);
void bflb_rv_dcache_clean_all(void);
void bflb_rv_dcache_invalidate_all(void);
void bflb_rv_dcache_clean_invalidate_all(void);
void bflb_rv_dcache_clean_range(unsigned long addr, unsigned long size);
void bflb_rv_dcache_invalidate_range(unsigned long addr, unsigned long size);
void bflb_rv_dcache_clean_invalidate_range(unsigned long addr, unsigned long size);

int bflb_rv_systimer_irqn(void);
uint64_t bflb_rv_systimer_get_time(void);
void bflb_rv_systimer_set_compare(uint64_t value);
uint64_t bflb_rv_systimer_get_compare(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
