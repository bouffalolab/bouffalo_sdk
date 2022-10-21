#ifndef _BFLB_TIMER_H
#define _BFLB_TIMER_H

#include "bflb_core.h"
#include "bflb_clock.h"

/** @defgroup TIMER_COUNTER_MODE timer counter mode definition
  * @{
  */
#define TIMER_COUNTER_MODE_PROLOAD 0
#define TIMER_COUNTER_MODE_UP      1
/**
  * @}
  */

/** @defgroup TIMER_COMP_ID timer compare id definition
  * @{
  */
#define TIMER_COMP_ID_0 0
#define TIMER_COMP_ID_1 1
#define TIMER_COMP_ID_2 2
/**
  * @}
  */

/**
 * @brief TIMER configuration structure
 *
 * @param counter_mode      Timer counter mode, use @ref TIMER_COUNTER_MODE
 * @param clock_source      Timer clock source, use BFLB_SYSTEM_* definition
 * @param clock_div         Timer clock divison value, from 0 to 255
 * @param trigger_comp_id   Timer count register preload trigger source slelect, use @ref TIMER_COMP_ID
 * @param comp0_val         Timer compare 0 value
 * @param comp1_val         Timer compare 1 value
 * @param comp2_val         Timer compare 2 value
 * @param preload_val       Timer preload value
 */
struct bflb_timer_config_s {
    uint8_t counter_mode;
    uint8_t clock_source;
    uint8_t clock_div;
    uint8_t trigger_comp_id;
    uint32_t comp0_val;
    uint32_t comp1_val;
    uint32_t comp2_val;
    uint32_t preload_val;
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_timer_init(struct bflb_device_s *dev, const struct bflb_timer_config_s *config);
void bflb_timer_deinit(struct bflb_device_s *dev);
void bflb_timer_start(struct bflb_device_s *dev);
void bflb_timer_stop(struct bflb_device_s *dev);
void bflb_timer_set_preloadvalue(struct bflb_device_s *dev, uint32_t val);
void bflb_timer_set_compvalue(struct bflb_device_s *dev, uint8_t cmp_no, uint32_t val);
uint32_t bflb_timer_get_compvalue(struct bflb_device_s *dev, uint8_t cmp_no);
uint32_t bflb_timer_get_countervalue(struct bflb_device_s *dev);
void bflb_timer_compint_mask(struct bflb_device_s *dev, uint8_t cmp_no, bool mask);
bool bflb_timer_get_compint_status(struct bflb_device_s *dev, uint8_t cmp_no);
void bflb_timer_compint_clear(struct bflb_device_s *dev, uint8_t cmp_no);

#ifdef __cplusplus
}
#endif

#endif