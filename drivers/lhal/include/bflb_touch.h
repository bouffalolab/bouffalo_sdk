#ifndef _BFLB_TOUCH_H
#define _BFLB_TOUCH_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup TOUCH
  * @{
  */

/** @defgroup TOUCH_RUN_MODE TOUCH run mode select type definition
  * @{
  */
#define TOUCH_RUN_MODE_SINGLE_SHOT (0) /*!< TOUCH run in single shot mode */
#define TOUCH_RUN_MODE_CONTINUE (1)    /*!< TOUCH run in continue mode */
/**
  * @}
  */

/** @defgroup TOUCH_PCHARGE_LOW TOUCH pcharge low type definition
  * @{
  */
#define TOUCH_PCHARGE_LOW_127 (0)  /*!< for one channel, the max external cap leakage cycle is 127 */
#define TOUCH_PCHARGE_LOW_255 (1)  /*!< for one channel, the max external cap leakage cycle is 255 */
#define TOUCH_PCHARGE_LOW_511 (2)  /*!< for one channel, the max external cap leakage cycle is 511 */
#define TOUCH_PCHARGE_LOW_1023 (3) /*!< for one channel, the max external cap leakage cycle is 1023 */
#define TOUCH_PCHARGE_LOW_2047 (4) /*!< for one channel, the max external cap leakage cycle is 2047 */
#define TOUCH_PCHARGE_LOW_4095 (5) /*!< for one channel, the max external cap leakage cycle is 4095 */
/**
  * @}
  */

/** @defgroup TOUCH_PCHARGE_HIGH TOUCH pcharge high type definition
  * @{
  */
#define TOUCH_PCHARGE_HIGH_511 (0)   /*!< for one channel, the max external cap charge-transfer cycle is 511 */
#define TOUCH_PCHARGE_HIGH_1023 (1)  /*!< for one channel, the max external cap charge-transfer cycle is 1023 */
#define TOUCH_PCHARGE_HIGH_2047 (2)  /*!< for one channel, the max external cap charge-transfer cycle is 2047 */
#define TOUCH_PCHARGE_HIGH_4095 (3)  /*!< for one channel, the max external cap charge-transfer cycle is 4095 */
#define TOUCH_PCHARGE_HIGH_8191 (4)  /*!< for one channel, the max external cap charge-transfer cycle is 8191 */
#define TOUCH_PCHARGE_HIGH_16383 (5) /*!< for one channel, the max external cap charge-transfer cycle is 16383 */
#define TOUCH_PCHARGE_HIGH_32767 (6) /*!< for one channel, the max external cap charge-transfer cycle is 32767 */
#define TOUCH_PCHARGE_HIGH_65535 (7) /*!< for one channel, the max external cap charge-transfer cycle is 65535 */
/**
  * @}
  */

/** @defgroup TOUCH_CLK_DIV TOUCH clock divider type definition
  * @{
  */
#define TOUCH_CLK_DIV_1 (0)   /*!< TOUCH clock div 1 */
#define TOUCH_CLK_DIV_2 (1)   /*!< TOUCH clock div 2 */
#define TOUCH_CLK_DIV_4 (2)   /*!< TOUCH clock div 4 */
#define TOUCH_CLK_DIV_8 (3)   /*!< TOUCH clock div 8 */
#define TOUCH_CLK_DIV_16 (4)  /*!< TOUCH clock div 16 */
#define TOUCH_CLK_DIV_32 (5)  /*!< TOUCH clock div 32 */
#define TOUCH_CLK_DIV_64 (6)  /*!< TOUCH clock div 64 */
#define TOUCH_CLK_DIV_128 (7) /*!< TOUCH clock div 128 */
/**
  * @}
  */

/** @defgroup TOUCH_CLK_SEL TOUCH clock source select type definition
  * @{
  */
#define TOUCH_CLK_SEL_32K (0) /*!< select 32K as TOUCH clock */
#define TOUCH_CLK_SEL_32M (1) /*!< select 32M as TOUCH clock */
/**
  * @}
  */

/** @defgroup TOUCH_CUR_SEL TOUCH current source select type definition
  * @{
  */
#define TOUCH_CUR_SEL_AON_BG (0)    /*!< select touch current from AON bandgap */
#define TOUCH_CUR_SEL_INTERN_BG (1) /*!< select touch current from internal bandgap */
/**
  * @}
  */

/** @defgroup TOUCH_HYSTERSIS_VOLTAGE TOUCH comparator hystersis voltage type definition
  * @{
  */
#define TOUCH_HYSTERSIS_VOLTAGE_30MV (0) /*!< set 30mV comparator hystersis voltage */
#define TOUCH_HYSTERSIS_VOLTAGE_80MV (1) /*!< set 80mV comparator hystersis voltage */
/**
  * @}
  */

/** @defgroup TOUCH_LDO_VOLTAGE TOUCH LDO voltage control type definition
  * @{
  */
#define TOUCH_LDO_VOLTAGE_0P6V (0) /*!< LDO output 0.6V */
#define TOUCH_LDO_VOLTAGE_0P8V (1) /*!< LDO output 0.8V */
#define TOUCH_LDO_VOLTAGE_1P0V (2) /*!< LDO output 1.0V */
#define TOUCH_LDO_VOLTAGE_1P2V (3) /*!< LDO output 1.2V */
#define TOUCH_LDO_VOLTAGE_1P4V (4) /*!< LDO output 1.4V */
#define TOUCH_LDO_VOLTAGE_1P6V (5) /*!< LDO output 1.6V */
#define TOUCH_LDO_VOLTAGE_1P8V (6) /*!< LDO output 1.8V */
#define TOUCH_LDO_VOLTAGE_2P0V (7) /*!< LDO output 2.0V */
/**
  * @}
  */

/** @defgroup TOUCH_VREF_VOLTAGE TOUCH reference voltage control type definition
  * @{
  */
#define TOUCH_VREF_VOLTAGE_0P4V (0) /*!< LDO output 0.4V */
#define TOUCH_VREF_VOLTAGE_0P6V (1) /*!< LDO output 0.6V */
#define TOUCH_VREF_VOLTAGE_0P8V (2) /*!< LDO output 0.8V */
#define TOUCH_VREF_VOLTAGE_1P0V (3) /*!< LDO output 1.0V */
#define TOUCH_VREF_VOLTAGE_1P2V (4) /*!< LDO output 1.2V */
#define TOUCH_VREF_VOLTAGE_1P4V (5) /*!< LDO output 1.4V */
#define TOUCH_VREF_VOLTAGE_1P6V (6) /*!< LDO output 1.6V */
#define TOUCH_VREF_VOLTAGE_1P8V (7) /*!< LDO output 1.8V */
/**
  * @}
  */

/** @defgroup TOUCH_COMPENSATION_CAP TOUCH compensation cap control type definition
  * @{
  */
#define TOUCH_COMPENSATION_CAP_12PF (0) /*!< compensation cap is 12pF */
#define TOUCH_COMPENSATION_CAP_13PF (1) /*!< compensation cap is 13pF */
#define TOUCH_COMPENSATION_CAP_14PF (2) /*!< compensation cap is 14pF */
#define TOUCH_COMPENSATION_CAP_15PF (3) /*!< compensation cap is 15pF */
/**
  * @}
  */

/** @defgroup OUCH_WORK_MODE TOUCH mode select type definition
  * @{
  */
#define TOUCH_WORK_MODE_SELF_CAP (0) /*!< self cap mode,each channel detection result can judge if this channel is touched */
#define TOUCH_WORK_MODE_MUTUAL (1)   /*!< mutual mode,channel 6~0 works,higher 3 channel-->column,lower 4 channel-->row */
/**
  * @}
  */

/** @defgroup TOUCH_FILTER_ORDER_CTRL TOUCH filter long time average order control,FLT(new)=(data_raw+FLT(former)*(2^N-1))/2^N
  * @{
  */
#define TOUCH_FILTER_ORDER_CTRL_0 (0) /*!< N=0 (acted like disable actually) */
#define TOUCH_FILTER_ORDER_CTRL_1 (1) /*!< N=1 */
#define TOUCH_FILTER_ORDER_CTRL_2 (2) /*!< N=2 */
#define TOUCH_FILTER_ORDER_CTRL_3 (3) /*!< N=3 */
#define TOUCH_FILTER_ORDER_CTRL_4 (4) /*!< N=4 */
#define TOUCH_FILTER_ORDER_CTRL_5 (5) /*!< N=5 */
#define TOUCH_FILTER_ORDER_CTRL_6 (6) /*!< N=6 */
#define TOUCH_FILTER_ORDER_CTRL_7 (7) /*!< N=7 */
/**
  * @}
  */

/** @defgroup TOUCH_LTA_ORDER_CTRL TOUCH LTA long time average order control,LTA(new)=(data_raw+LTA(former)*(2^N-1))/2^N
  * @{
  */
#define TOUCH_LTA_ORDER_CTRL_0 (0) /*!< N=0 (acted like disable actually) */
#define TOUCH_LTA_ORDER_CTRL_1 (1) /*!< N=1 */
#define TOUCH_LTA_ORDER_CTRL_2 (2) /*!< N=2 */
#define TOUCH_LTA_ORDER_CTRL_3 (3) /*!< N=3 */
#define TOUCH_LTA_ORDER_CTRL_4 (4) /*!< N=4 */
#define TOUCH_LTA_ORDER_CTRL_5 (5) /*!< N=5 */
#define TOUCH_LTA_ORDER_CTRL_6 (6) /*!< N=6 */
#define TOUCH_LTA_ORDER_CTRL_7 (7) /*!< N=7 */
/**
  * @}
  */

/** @defgroup TOUCH_CHANNEL TOUCH channel type definition
  * @{
  */
#define TOUCH_CHANNEL_0 (0)     /*!< TOUCH channel 0 */
#define TOUCH_CHANNEL_1 (1)     /*!< TOUCH channel 1 */
#define TOUCH_CHANNEL_2 (2)     /*!< TOUCH channel 2 */
#define TOUCH_CHANNEL_3 (3)     /*!< TOUCH channel 3 */
#define TOUCH_CHANNEL_4 (4)     /*!< TOUCH channel 4 */
#define TOUCH_CHANNEL_5 (5)     /*!< TOUCH channel 5 */
#define TOUCH_CHANNEL_6 (6)     /*!< TOUCH channel 6 */
#define TOUCH_CHANNEL_7 (7)     /*!< TOUCH channel 7 */
#define TOUCH_CHANNEL_8 (8)     /*!< TOUCH channel 8 */
#define TOUCH_CHANNEL_9 (9)     /*!< TOUCH channel 9 */
#define TOUCH_CHANNEL_10 (10)   /*!< TOUCH channel 10 */
#define TOUCH_CHANNEL_11 (11)   /*!< TOUCH channel 11 */
#define TOUCH_CHANNEL_MAX (12)  /*!< all channel */
#define TOUCH_CHANNEL_NONE (12) /*!< TOUCH channel None */
/**
  * @}
  */

/** @defgroup TOUCH_CHANNEL_HIGHZ_CFG TOUCH channel config type definition
  * @{
  */
#define TOUCH_CHANNEL_HIGHZ_CFG_GND (0)   /*!< channel tie to ground */
#define TOUCH_CHANNEL_HIGHZ_CFG_HIGHZ (1) /*!< channel HighZ output */
/**
  * @}
  */

/** @defgroup TOUCH_INT touch interrupt type definition
  * @{
  */
#define TOUCH_INT_CH0 (1 << 0)
#define TOUCH_INT_CH1 (1 << 1)
#define TOUCH_INT_CH2 (1 << 2)
#define TOUCH_INT_CH3 (1 << 3)
#define TOUCH_INT_CH4 (1 << 4)
#define TOUCH_INT_CH5 (1 << 5)
#define TOUCH_INT_CH6 (1 << 6)
#define TOUCH_INT_CH7 (1 << 7)
#define TOUCH_INT_CH8 (1 << 8)
#define TOUCH_INT_CH9 (1 << 9)
#define TOUCH_INT_CH10 (1 << 10)
#define TOUCH_INT_CH11 (1 << 11)
#define TOUCH_INT_ALL (0xFFF) /*!< all channel interrupt */
/**
  * @}
  */

/** @defgroup TOUCH_CMD touch feature control cmd definition
  * @{
  */
#define TOUCH_CMD_START (0) /*!< start touch */
#define TOUCH_CMD_STOP (1)  /*!< stop touch */
/**
  * @}
  */

// clang-format off
#define IS_TOUCH_RUN_MODE_TYPE(type) (((type) == TOUCH_RUN_MODE_SINGLE_SHOT) || \
                                      ((type) == TOUCH_RUN_MODE_CONTINUE))

#define IS_TOUCH_PCHARGE_LOW_TYPE(type) (((type) == TOUCH_PCHARGE_LOW_127) ||  \
                                         ((type) == TOUCH_PCHARGE_LOW_255) ||  \
                                         ((type) == TOUCH_PCHARGE_LOW_511) ||  \
                                         ((type) == TOUCH_PCHARGE_LOW_1023) || \
                                         ((type) == TOUCH_PCHARGE_LOW_2047) || \
                                         ((type) == TOUCH_PCHARGE_LOW_4095))

#define IS_TOUCH_PCHARGE_HIGH_TYPE(type) (((type) == TOUCH_PCHARGE_HIGH_511) ||   \
                                          ((type) == TOUCH_PCHARGE_HIGH_1023) ||  \
                                          ((type) == TOUCH_PCHARGE_HIGH_2047) ||  \
                                          ((type) == TOUCH_PCHARGE_HIGH_4095) ||  \
                                          ((type) == TOUCH_PCHARGE_HIGH_8191) ||  \
                                          ((type) == TOUCH_PCHARGE_HIGH_16383) || \
                                          ((type) == TOUCH_PCHARGE_HIGH_32767) || \
                                          ((type) == TOUCH_PCHARGE_HIGH_65535))

#define IS_TOUCH_CLK_DIV_TYPE(type) (((type) == TOUCH_CLK_DIV_1) ||  \
                                     ((type) == TOUCH_CLK_DIV_2) ||  \
                                     ((type) == TOUCH_CLK_DIV_4) ||  \
                                     ((type) == TOUCH_CLK_DIV_8) ||  \
                                     ((type) == TOUCH_CLK_DIV_16) || \
                                     ((type) == TOUCH_CLK_DIV_32) || \
                                     ((type) == TOUCH_CLK_DIV_64) || \
                                     ((type) == TOUCH_CLK_DIV_128))

#define IS_TOUCH_CLK_SEL_TYPE(type) (((type) == TOUCH_CLK_SEL_32K) || \
                                     ((type) == TOUCH_CLK_SEL_32M))

#define IS_TOUCH_CUR_SEL_TYPE(type) (((type) == TOUCH_CUR_SEL_AON_BG) || \
                                       ((type) == TOUCH_CUR_SEL_INTERN_BG))

#define IS_TOUCH_HYSTERSIS_VOLTAGE_TYPE(type) (((type) == TOUCH_HYSTERSIS_VOLTAGE_30mV) || \
                                               ((type) == TOUCH_HYSTERSIS_VOLTAGE_80mV))

#define IS_TOUCH_LDO_VOLTAGE_TYPE(type) (((type) == TOUCH_LDO_VOLTAGE_0P6V) || \
                                         ((type) == TOUCH_LDO_VOLTAGE_0P8V) || \
                                         ((type) == TOUCH_LDO_VOLTAGE_1P0V) || \
                                         ((type) == TOUCH_LDO_VOLTAGE_1P2V) || \
                                         ((type) == TOUCH_LDO_VOLTAGE_1P4V) || \
                                         ((type) == TOUCH_LDO_VOLTAGE_1P6V) || \
                                         ((type) == TOUCH_LDO_VOLTAGE_1P8V) || \
                                         ((type) == TOUCH_LDO_VOLTAGE_2P0V))

#define IS_TOUCH_VREF_VOLTAGE_TYPE(type) (((type) == TOUCH_VREF_VOLTAGE_0P4V) || \
                                          ((type) == TOUCH_VREF_VOLTAGE_0P6V) || \
                                          ((type) == TOUCH_VREF_VOLTAGE_0P8V) || \
                                          ((type) == TOUCH_VREF_VOLTAGE_1P0V) || \
                                          ((type) == TOUCH_VREF_VOLTAGE_1P2V) || \
                                          ((type) == TOUCH_VREF_VOLTAGE_1P4V) || \
                                          ((type) == TOUCH_VREF_VOLTAGE_1P6V) || \
                                          ((type) == TOUCH_VREF_VOLTAGE_1P8V))

#define IS_TOUCH_COMPENSATION_CAP_TYPE(type) (((type) == TOUCH_COMPENSATION_CAP_12pF) || \
                                              ((type) == TOUCH_COMPENSATION_CAP_13pF) || \
                                              ((type) == TOUCH_COMPENSATION_CAP_14pF) || \
                                              ((type) == TOUCH_COMPENSATION_CAP_15pF))

#define IS_TOUCH_WORK_MODE_TYPE(type) (((type) == TOUCH_WORK_MODE_SELF_CAP) || \
                                       ((type) == TOUCH_WORK_MODE_MUTUAL))

#define IS_TOUCH_FILTER_ORDER_CTRL_TYPE(type) (((type) == TOUCH_FILTER_ORDER_CTRL_0) || \
                                               ((type) == TOUCH_FILTER_ORDER_CTRL_1) || \
                                               ((type) == TOUCH_FILTER_ORDER_CTRL_2) || \
                                               ((type) == TOUCH_FILTER_ORDER_CTRL_3) || \
                                               ((type) == TOUCH_FILTER_ORDER_CTRL_4) || \
                                               ((type) == TOUCH_FILTER_ORDER_CTRL_5) || \
                                               ((type) == TOUCH_FILTER_ORDER_CTRL_6) || \
                                               ((type) == TOUCH_FILTER_ORDER_CTRL_7))

#define IS_TOUCH_LTA_ORDER_CTRL_TYPE(type) (((type) == TOUCH_LTA_ORDER_CTRL_0) || \
                                            ((type) == TOUCH_LTA_ORDER_CTRL_1) || \
                                            ((type) == TOUCH_LTA_ORDER_CTRL_2) || \
                                            ((type) == TOUCH_LTA_ORDER_CTRL_3) || \
                                            ((type) == TOUCH_LTA_ORDER_CTRL_4) || \
                                            ((type) == TOUCH_LTA_ORDER_CTRL_5) || \
                                            ((type) == TOUCH_LTA_ORDER_CTRL_6) || \
                                            ((type) == TOUCH_LTA_ORDER_CTRL_7))

#define IS_TOUCH_CHANNEL_TYPE(type) (((type) == TOUCH_CHANNEL_0) ||  \
                                     ((type) == TOUCH_CHANNEL_1) ||  \
                                     ((type) == TOUCH_CHANNEL_2) ||  \
                                     ((type) == TOUCH_CHANNEL_3) ||  \
                                     ((type) == TOUCH_CHANNEL_4) ||  \
                                     ((type) == TOUCH_CHANNEL_5) ||  \
                                     ((type) == TOUCH_CHANNEL_6) ||  \
                                     ((type) == TOUCH_CHANNEL_7) ||  \
                                     ((type) == TOUCH_CHANNEL_8) ||  \
                                     ((type) == TOUCH_CHANNEL_9) ||  \
                                     ((type) == TOUCH_CHANNEL_10) || \
                                     ((type) == TOUCH_CHANNEL_11))

#define IS_TOUCH_CHANNEL_HIGHZ_CFG_TYPE(type) (((type) == TOUCH_CHANNEL_HIGHZ_CFG_GND) || \
                                               ((type) == TOUCH_CHANNEL_HIGHZ_CFG_HIGHZ))

#define IS_TOUCH_INT_TYPE(type) (((type) == TOUCH_INT_CH0) || \
                            ((type) == TOUCH_INT_CH1) || \
                            ((type) == TOUCH_INT_CH2) || \
                            ((type) == TOUCH_INT_CH3) || \
                            ((type) == TOUCH_INT_CH4) || \
                            ((type) == TOUCH_INT_CH5) || \
                            ((type) == TOUCH_INT_CH6) || \
                            ((type) == TOUCH_INT_CH7) || \
                            ((type) == TOUCH_INT_CH8) || \
                            ((type) == TOUCH_INT_CH9) || \
                            ((type) == TOUCH_INT_CH10)|| \
                            ((type) == TOUCH_INT_CH11))

// clang-format on

/**
 * @brief TOUCH configuration type definition
 *
 * @param clk_sel           TOUCH clock source select
 * @param clk_div           TOUCH clock divider
 * @param cur_sel           TOUCH power current source select
 * @param cal_en            TOUCH channel calibration option, when enable,the counting number will be set as LTA data
 * @param work_mode         TOUCH channel work mode select
 * @param run_mode          0-->single shot mode, 1-->continous mode
 * @param ulp_en            ultra low power mode enable
 * @param cycle_en          configure for low power mode with single shot, sleep for a time and then turn on
 * @param sleep_time_ms     Sleep time in us, can save power by reduce turn on time
 * @param pcharge_low       the max external cap leakage cycle
 * @param pcharge_high      the max external cap charge-transfer cycle
 * @param ldo_volt          TOUCH LDO voltage
 * @param vref_volt         TOUCH reference voltage control
 * @param hystersis_volt    TOUCH comparator hystersis voltage
 * @param compensation_cap  TOUCH compensation cap control
 * @param filter_en         touch_channel output data filter control, will do average(low pass filter) when enable
 * @param filter_ctrl       TOUCH filter long time average order control
 * @param lta_en            TOUCH channel long time average control, will do average(low pass filter) when enable
 * @param lta_ctrl          TOUCH LTA long time average order control
 * @param data_hystersis_en touch_channel hystersis data for touch action judgement, can reduce the false trigger
 * @param data_hys_ctrl     Hystersis data for data and dectection judgement
 */
struct bflb_touch_config_s {
    uint8_t clk_sel;
    uint8_t clk_div;
    uint8_t cur_sel;
    uint8_t cal_en;
    uint8_t work_mode;
    uint8_t run_mode;
    uint8_t ulp_en;
    uint8_t cycle_en;
    uint32_t sleep_time_ms;
    uint8_t pcharge_low;
    uint8_t pcharge_high;
    uint8_t ldo_volt;
    uint8_t vref_volt;
    uint8_t hystersis_volt;
    uint8_t compensation_cap;
    uint8_t filter_en;
    uint8_t filter_ctrl;
    uint8_t lta_en;
    uint8_t lta_ctrl;
    uint8_t data_hystersis_en;
    uint16_t data_hys_ctrl;
};

/**
 * @brief TOUCH channel config type definition
 *
 * @param ch  channel x config
 */
struct bflb_touch_chan_config_s {
    uint8_t ch[TOUCH_CHANNEL_MAX];
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize touch.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to touch config
 * @return A negated errno value on failure.
 */
int bflb_touch_init(struct bflb_device_s *dev, const struct bflb_touch_config_s *config);

/**
 * @brief Initialize touch.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to touch config
 * @return A negated errno value on failure.
 */
int bflb_touch_channel_init(struct bflb_device_s *dev, const struct bflb_touch_chan_config_s *config, uint8_t lp_chan_sel);

/**
 * @brief Enable touch.
 *
 * @param [in] dev device handle
 */
void bflb_touch_enable(struct bflb_device_s *dev);

/**
 * @brief Disable touch.
 *
 * @param [in] dev device handle
 */
void bflb_touch_disable(struct bflb_device_s *dev);

/**
 * @brief Set touch channel vth data.
 *
 * @param [in] dev device handle
 * @param [in] channel touch channel
 * @param [in] vth vth data
 */
void bflb_touch_set_vth_data(struct bflb_device_s *dev, uint8_t channel, uint32_t vth);

/**
 * @brief Get touch channel vth data.
 *
 * @param [in] dev device handle
 * @param [in] channel touch channel
 * @return touch channel vth data
 */
uint32_t bflb_touch_get_vth_data(struct bflb_device_s *dev, uint8_t channel);

/***
 * @brief Get touch channel raw data.
 *
 * @param [in] dev device handle
 * @param [in] channel touch channel
 * @return touch channel raw data
 */
uint32_t bflb_touch_get_raw_data(struct bflb_device_s *dev, uint8_t channel);

/**
 * @brief Get touch channel long time average data.
 *
 * @param [in] dev device handle
 * @param [in] channel touch channel
 * @return touch channel long time average data
 */
uint32_t bflb_touch_get_lta_data(struct bflb_device_s *dev, uint8_t channel);

/**
 * @brief Get touch channel filter data.
 *
 * @param [in] dev device handle
 * @param [in] channel touch channel
 * @return touch channel filter data
 */
uint32_t bflb_touch_get_flt_data(struct bflb_device_s *dev, uint8_t channel);

/**
 * @brief Get touch detection status.
 *
 * @param [in] dev device handle
 * @return true means touched, false means not touched
 */
bool bflb_touch_get_end_status(struct bflb_device_s *dev);

/**
 * @brief Enable touch global interrupt.
 *
 * @param [in] dev device handle
 */
void bflb_touch_enable_int(struct bflb_device_s *dev);

/**
 * @brief Disable touch global interrupt.
 *
 * @param [in] dev device handle
 */
void bflb_touch_disable_int(struct bflb_device_s *dev);

/** 
 * @brief Enable or disable touch interrupt.
 *
 * @param [in] dev device handle
 * @param [in] int_type interrupt type, use @ref TOUCH_INT
 * @param [in] mask true means disable, false means enable
 */

void bflb_touch_int_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask);

/**
 * @brief Get touch interrupt status.
 *
 * @param [in] dev device handle
 * @return interrupt status, use @ref TOUCH_INT
 */
uint32_t bflb_touch_get_intstatus(struct bflb_device_s *dev);

/**
 * @brief Clear touch interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_clear clear value, use @ref TOUCH_INT
 */
void bflb_touch_int_clear(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Control touch feature.
 *
 * @param [in] dev device handle
 * @param [in] cmd feature command, use @ref TOUCH_CMD
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_touch_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif
