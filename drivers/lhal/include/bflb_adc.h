#ifndef _BFLB_ADC_H
#define _BFLB_ADC_H

#include "bflb_core.h"

/** @defgroup ADC_CHANNEL adc channel definition
  * @{
  */
#define ADC_CHANNEL_0      0
#define ADC_CHANNEL_1      1
#define ADC_CHANNEL_2      2
#define ADC_CHANNEL_3      3
#define ADC_CHANNEL_4      4
#define ADC_CHANNEL_5      5
#define ADC_CHANNEL_6      6
#define ADC_CHANNEL_7      7
#define ADC_CHANNEL_8      8
#define ADC_CHANNEL_9      9
#define ADC_CHANNEL_10     10
#define ADC_CHANNEL_11     11
#define ADC_CHAN_TSEN_P    14
#define ADC_CHAN_TSEN_N    15
#define ADC_CHAN_VABT_HALF 18
#define ADC_CHANNEL_GND    23
/**
  * @}
  */

/** @defgroup ADC_CLK_DIV adc clock divison definition
  * @{
  */
#define ADC_CLK_DIV_4  1
#define ADC_CLK_DIV_8  2
#define ADC_CLK_DIV_12 3
#define ADC_CLK_DIV_16 4
#define ADC_CLK_DIV_20 5
#define ADC_CLK_DIV_24 6
#define ADC_CLK_DIV_32 7
/**
  * @}
  */

/** @defgroup ADC_RESOLUTION adc resolution definition
  * @{
  */
#define ADC_RESOLUTION_12B 0
#define ADC_RESOLUTION_14B 2
#define ADC_RESOLUTION_16B 4
/**
  * @}
  */

/** @defgroup ADC_VREF adc reference select
  * @{
  */
#define ADC_VREF_3P2V 0
#define ADC_VREF_2P0V 1
/**
  * @}
  */

/** @defgroup ADC_TSEN_MOD adc tsen mode definition
  * @{
  */
#define ADC_TSEN_MOD_INTERNAL_DIODE 0
#define ADC_TSEN_MOD_EXTERNAL_DIODE 1
/**
  * @}
  */

/** @defgroup ADC_INTSTS adc interrupt status definition
  * @{
  */
#define ADC_INTSTS_NEG_SATURATION (1 << 0)
#define ADC_INTSTS_POS_SATURATION (1 << 1)
#define ADC_INTSTS_FIFO_UNDERRUN  (1 << 2)
#define ADC_INTSTS_FIFO_OVERRUN   (1 << 3)
#define ADC_INTSTS_ADC_READY      (1 << 4)
/**
  * @}
  */

/** @defgroup ADC_INTCLR adc interrupt clear definition
  * @{
  */
#define ADC_INTCLR_NEG_SATURATION (1 << 0)
#define ADC_INTCLR_POS_SATURATION (1 << 1)
#define ADC_INTCLR_FIFO_UNDERRUN  (1 << 2)
#define ADC_INTCLR_FIFO_OVERRUN   (1 << 3)
#define ADC_INTCLR_ADC_READY      (1 << 4)
/**
  * @}
  */

/**
 * @brief ADC configuration structure
 *
 * @param clk_div                ADC clock divison, use @ref ADC_CLK_DIV
 * @param scan_conv_mode         ADC scan mode enable
 * @param continuous_conv_mode   ADC continuous conversion enable
 * @param differential_mode      ADC differential mode enable
 * @param resolution             ADC resolution, use @ref ADC_RESOLUTION
 * @param vref                   ADC reference select, use @ref ADC_VREF
 */
struct bflb_adc_config_s {
    uint8_t clk_div;
    uint8_t scan_conv_mode;
    uint8_t continuous_conv_mode;
    uint8_t differential_mode;
    uint8_t resolution;
    uint8_t vref;
};

/**
 * @brief ADC channel select
 *
 * @param pos_chan         Select ADC positive input in none-scan mode
 * @param neg_chan         Select ADC negative input in none-scan mode
 */
struct bflb_adc_channel_s {
    uint8_t pos_chan;
    uint8_t neg_chan;
};

/**
 * @brief ADC result select
 *
 * @param pos_chan         ADC positive channel
 * @param neg_chan         ADC negative channel
 * @param value            ADC value
 * @param millivolt        ADC voltage result
 */
struct bflb_adc_result_s {
    int8_t pos_chan;
    int8_t neg_chan;
    int32_t value;
    int32_t millivolt;
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_adc_init(struct bflb_device_s *dev, const struct bflb_adc_config_s *config);
void bflb_adc_deinit(struct bflb_device_s *dev);
void bflb_adc_link_rxdma(struct bflb_device_s *dev, bool enable);
int bflb_adc_channel_config(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels);
void bflb_adc_start_conversion(struct bflb_device_s *dev);
void bflb_adc_stop_conversion(struct bflb_device_s *dev);
uint8_t bflb_adc_get_count(struct bflb_device_s *dev);
uint32_t bflb_adc_read_raw(struct bflb_device_s *dev);
void bflb_adc_rxint_mask(struct bflb_device_s *dev, bool mask);
void bflb_adc_errint_mask(struct bflb_device_s *dev, bool mask);
uint32_t bflb_adc_get_intstatus(struct bflb_device_s *dev);
void bflb_adc_int_clear(struct bflb_device_s *dev, uint32_t int_clear);

void bflb_adc_parse_result(struct bflb_device_s *dev, uint32_t *buffer, struct bflb_adc_result_s *result, uint16_t count);
void bflb_adc_tsen_init(struct bflb_device_s *dev, uint8_t tsen_mod);
float bflb_adc_tsen_get_temp(struct bflb_device_s *dev, uint32_t tsen_offset);
void bflb_adc_vbat_enable(struct bflb_device_s *dev);
void bflb_adc_vbat_disable(struct bflb_device_s *dev);

#ifdef __cplusplus
}
#endif

#endif