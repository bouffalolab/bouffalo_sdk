#ifndef _BFLB_PWM_V1_H
#define _BFLB_PWM_V2_H

#include "bflb_core.h"
#include "bflb_clock.h"

/** @defgroup PWM_CHANNEL pwm channel definition
  * @{
  */
#define PWM_CH0       0
#define PWM_CH1       1
#define PWM_CH2       2
#define PWM_CH3       3
#define PWM_CH4       4
#define PWM_V1_CH_MAX 5
/**
  * @}
  */

/** @defgroup PWM_INTSTS pwm interrupt status definition
  * @{
  */
#define PWM_INTSTS_REPT_CH0 (1 << 0)
#define PWM_INTSTS_REPT_CH1 (1 << 1)
#define PWM_INTSTS_REPT_CH2 (1 << 2)
#define PWM_INTSTS_REPT_CH3 (1 << 3)
#define PWM_INTSTS_REPT_CH4 (1 << 4)
/**
  * @}
  */

/** @defgroup PWM_INTCLR pwm interrupt clear definition
  * @{
  */
#define PWM_INTCLR_REPT_CH0 (1 << 8)
#define PWM_INTCLR_REPT_CH1 (1 << 9)
#define PWM_INTCLR_REPT_CH2 (1 << 10)
#define PWM_INTCLR_REPT_CH3 (1 << 11)
#define PWM_INTCLR_REPT_CH4 (1 << 12)
/**
  * @}
  */

/** @defgroup PWM_CMD pwm feature control cmd definition
  * @{
  */
#define PWM_CMD_SET_STOP_MODE      (0x01)
#define PWM_CMD_SET_OUT_INVERT     (0x02)
#define PWM_CMD_SET_SW_MODE        (0x03)
#define PWM_CMD_SET_SW_FORCE_VALUE (0x04)
#define PWM_CMD_SET_REPT_COUNT     (0x05)
/**
  * @}
  */

/** @defgroup PWM_STOP_MODE pwm stop mode definition
  * @{
  */
#define PWM_STOP_MODE_ABRUPT   0
#define PWM_STOP_MODE_GRACEFUL 1
/**
  * @}
  */

/**
 * @brief PWM configuration structure
 *
 * @param clk_source PWM clock source, use BFLB_SYSTEM_* definition
 * @param clk_div    PWM clock dividor, should be in 1~65535
 * @param period     PWM period count, should be in 2~65535
 */
struct bflb_pwm_v1_channel_config_s {
    uint8_t clk_source;
    uint16_t clk_div;
    uint16_t period;
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_pwm_v1_channel_init(struct bflb_device_s *dev, uint8_t ch, const struct bflb_pwm_v1_channel_config_s *config);
void bflb_pwm_v1_channel_set_threshold(struct bflb_device_s *dev, uint8_t ch, uint16_t low_threhold, uint16_t high_threhold);
void bflb_pwm_v1_deinit(struct bflb_device_s *dev, uint8_t ch);
void bflb_pwm_v1_start(struct bflb_device_s *dev, uint8_t ch);
void bflb_pwm_v1_stop(struct bflb_device_s *dev, uint8_t ch);
void bflb_pwm_v1_set_period(struct bflb_device_s *dev, uint8_t ch, uint16_t period);

void bflb_pwm_v1_int_enable(struct bflb_device_s *dev, uint8_t ch, bool enable);
uint32_t bflb_pwm_v1_get_intstatus(struct bflb_device_s *dev);
void bflb_pwm_v1_int_clear(struct bflb_device_s *dev, uint32_t int_clear);

int bflb_pwm_v1_feature_control(struct bflb_device_s *dev, uint8_t ch, int cmd, size_t arg);

#ifdef __cplusplus
}
#endif

#endif