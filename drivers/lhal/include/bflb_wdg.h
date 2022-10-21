#ifndef _BFLB_WDG_H
#define _BFLB_WDG_H

#include "bflb_core.h"
#include "bflb_clock.h"

/**
 * @brief WDG configuration structure
 *
 * @param clock_source      Wdg clock source, use BFLB_SYSTEM_* definition
 * @param clock_div         Wdg clock divison value, from 0 to 255
 * @param comp_val         Wdg compare value
 */
struct bflb_wdg_config_s {
    uint8_t clock_source;
    uint8_t clock_div;
    uint16_t comp_val;
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_wdg_init(struct bflb_device_s *dev, const struct bflb_wdg_config_s *config);
void bflb_wdg_start(struct bflb_device_s *dev);
void bflb_wdg_stop(struct bflb_device_s *dev);
uint16_t bflb_wdg_get_countervalue(struct bflb_device_s *dev);
void bflb_wdg_reset_countervalue(struct bflb_device_s *dev);
void bflb_wdg_compint_clear(struct bflb_device_s *dev);

#ifdef __cplusplus
}
#endif

#endif