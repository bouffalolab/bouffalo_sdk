#ifndef _BFLB_IRQ_H
#define _BFLB_IRQ_H

#include "stdint.h"

typedef void (*irq_callback)(int irq, void *arg);

/**
 * @brief IRQ configuration structure
 *
 * @param handler      Address of the interrupt handler
 * @param arg          The argument provided to the interrupt handler
 */
struct bflb_irq_info_s {
    irq_callback handler;
    void *arg;
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_irq_initialize(void);
uint32_t bflb_irq_save(void);
void bflb_irq_restore(uint32_t flags);
int bflb_irq_attach(int irq, irq_callback isr, void *arg);
int bflb_irq_detach(int irq);
void bflb_irq_enable(int irq);
void bflb_irq_disable(int irq);
void bflb_irq_set_pending(int irq);
void bflb_irq_clear_pending(int irq);
void bflb_irq_set_priority(int irq, uint8_t preemptprio, uint8_t subprio);

#ifdef __cplusplus
}
#endif

#endif