#ifndef _BFLB_MTIMER_H
#define _BFLB_MTIMER_H

#include "stdint.h"
#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

void bflb_mtimer_config(uint64_t ticks, void (*interruptfun)(void));
uint32_t bflb_mtimer_get_freq(void);
void bflb_mtimer_delay_ms(uint32_t time);
void bflb_mtimer_delay_us(uint32_t time);
uint64_t bflb_mtimer_get_time_us();
uint64_t bflb_mtimer_get_time_ms();

#ifdef __cplusplus
}
#endif

#endif