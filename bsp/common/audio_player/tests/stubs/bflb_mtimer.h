#ifndef TEST_BFLB_MTIMER_H
#define TEST_BFLB_MTIMER_H
#include <stdint.h>
void bflb_mtimer_delay_ms(uint32_t ms);
uint64_t bflb_mtimer_get_time_us(void);
#endif
