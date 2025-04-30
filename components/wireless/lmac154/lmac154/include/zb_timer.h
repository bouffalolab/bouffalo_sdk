#ifndef __ZB_TIMER_H__
#define __ZB_TIMER_H__

#include <stdint.h>

#define ZB_TIMER_CH_NUM            6
#define ZB_TIMER_US_PER_TICK       16

typedef void (*zb_timer_cb_t)(void);

void zb_timer_irq(uint32_t intStatus);
void zb_timer_cfg(uint32_t init_time);
void zb_timer_cfg_us(uint64_t init_time);
uint32_t zb_timer_get_current_time(void);
uint64_t zb_timer_get_current_time_us(void);
uint32_t zb_timer_get_remaining_time(uint8_t ch);
void zb_timer_start(uint8_t ch, uint32_t target_time, zb_timer_cb_t cb);
void* zb_timer_stop(uint8_t ch);
void zb_timer_store(void);
void zb_timer_store_time(void);
void zb_timer_store_events(void);
void zb_timer_restore(uint32_t jump_time, uint8_t run_expired);
void zb_timer_restore_time(uint32_t jump_time);
void zb_timer_restore_events(uint8_t run_expired);

#endif
