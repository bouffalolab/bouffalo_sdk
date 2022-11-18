#ifndef _BFLB_SEC_IRQ_H
#define _BFLB_SEC_IRQ_H

#include "bflb_core.h"

#define BFLB_SEC_ENG_IRQ_TYPE_AES  0
#define BFLB_SEC_ENG_IRQ_TYPE_SHA  1
#define BFLB_SEC_ENG_IRQ_TYPE_PKA  2
#define BFLB_SEC_ENG_IRQ_TYPE_TRNG 3
#define BFLB_SEC_ENG_IRQ_TYPE_GMAC 4
#define BFLB_SEC_ENG_IRQ_TYPE_CDET 5

#ifdef __cplusplus
extern "C" {
#endif

void bflb_sec_irq_attach(uint8_t sec_type, void (*callback)(void *arg), void *arg);
void bflb_sec_irq_detach(uint8_t sec_type);

#ifdef __cplusplus
}
#endif

#endif