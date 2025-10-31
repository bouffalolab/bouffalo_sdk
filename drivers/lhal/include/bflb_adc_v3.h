#ifndef _BFLB_ADC_V3_H
#define _BFLB_ADC_V3_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup ADC
  * @{
  */

/** @defgroup ADC_MODE adc work mode definition
  * @{
  */
#define ADC_MODE_SEPARATE   (0)
#define ADC_MODE_INTERLEAVE (1)
#define ADC_MODE_CONCURRENT (2)
/**
  * @}
  */

/** @defgroup ADC_EXTERNAL_CHANNEL adc channel definition
  * @{
  */
#define ADC_EXTERNAL_CHANNEL_0    (0)
#define ADC_EXTERNAL_CHANNEL_1    (1)
#define ADC_EXTERNAL_CHANNEL_2    (2)
#define ADC_EXTERNAL_CHANNEL_3    (3)
#define ADC_EXTERNAL_CHANNEL_4    (4)
#define ADC_EXTERNAL_CHANNEL_5    (5)
#define ADC_EXTERNAL_CHANNEL_6    (6)
#define ADC_EXTERNAL_CHANNEL_7    (7)
#define ADC_EXTERNAL_CHANNEL_8    (8)
#define ADC_EXTERNAL_CHANNEL_9    (9)
#define ADC_EXTERNAL_CHANNEL_10   (10)
#define ADC_EXTERNAL_CHANNEL_11   (11)
#define ADC_EXTERNAL_CHANNEL_NULL (15)
/**
  * @}
  */

/** @defgroup ADC0_INTERNAL_CHANNEL adc channel definition
  * @{
  */
#define ADC0_INTERNAL_CHANNEL_NULL      (4)
#define ADC0_INTERNAL_CHANNEL_TSEN_P    (10)
#define ADC0_INTERNAL_CHANNEL_TSEN_N    (11)
#define ADC0_INTERNAL_CHANNEL_VREF      (12)
#define ADC0_INTERNAL_CHANNEL_ATEST     (13)
#define ADC0_INTERNAL_CHANNEL_VBAT_HALF (14)
#define ADC0_INTERNAL_CHANNEL_AVSS      (31)
/**
  * @}
  */

/** @defgroup ADC_INJECT_CHANNEL adc channel definition
  * @{
  */
#define ADC_INJECT_CHANNEL_0 (0)
#define ADC_INJECT_CHANNEL_1 (1)
#define ADC_INJECT_CHANNEL_2 (2)
#define ADC_INJECT_CHANNEL_3 (3)
/**
  * @}
  */

/** @defgroup ADC_FIFO_THRESHOLD adc fifo threshold definition
  * @{
  */
#define ADC_FIFO_THRESHOLD_1  (0)
#define ADC_FIFO_THRESHOLD_4  (1)
#define ADC_FIFO_THRESHOLD_8  (2)
#define ADC_FIFO_THRESHOLD_16 (3)
/**
  * @}
  */

/** @defgroup ADC_CLK_DIV adc clock divison definition
  * @{
  */
#define ADC_CLK_DIV_1  (0)
#define ADC_CLK_DIV_4  (1)
#define ADC_CLK_DIV_8  (2)
#define ADC_CLK_DIV_12 (3)
#define ADC_CLK_DIV_16 (4)
#define ADC_CLK_DIV_20 (5)
#define ADC_CLK_DIV_24 (6)
#define ADC_CLK_DIV_32 (7)
/**
  * @}
  */

/** @defgroup ADC_RESOLUTION adc resolution definition
  * @{
  */
#define ADC_RESOLUTION_12B (0)
#define ADC_RESOLUTION_14B (2)
#define ADC_RESOLUTION_16B (4)
/**
  * @}
  */

/** @defgroup ADC_VREF adc reference select definition
  * @{
  */
#define ADC_VREF_INTERNAL_1P25 (0)
#define ADC_VREF_EXTERNAL_1P25 (1)
/**
  * @}
  */

/** @defgroup ADC_TSEN_MOD adc tsen mode definition
  * @{
  */
#define ADC_TSEN_MOD_INTERNAL_DIODE (0)
#define ADC_TSEN_MOD_EXTERNAL_DIODE (1)
/**
  * @}
  */

/** @defgroup ADC_INT_SATURATION adc saturation interrupt type definition
  * @{
  */
#define ADC_INT_SATURATION_NEG (1 << 0)
#define ADC_INT_SATURATION_POS (1 << 1)
#define ADC_INT_SATURATION_ALL (ADC_INT_SATURATION_NEG | ADC_INT_SATURATION_POS)
/**
  * @}
  */

/** @defgroup ADC_INT_WATCHDOG adc watch dog interrupt type definition
  * @{
  */
#define ADC_INT_WATCHDOG     (1 << 19)
#define ADC_INT_WATCHDOG_ALL (ADC_INT_WATCHDOG)
/**
  * @}
  */

/** @defgroup ADC_INT_DONE adc done interrupt type definition
  * @{
  */
#define ADC_INT_DONE_SCAN   (1 << 15)
#define ADC_INT_DONE_INJECT (1 << 16)
#define ADC_INT_DONE_ALL    (ADC_INT_DONE_SCAN | ADC_INT_DONE_INJECT)
/**
  * @}
  */

/** @defgroup ADC_INT_FIFO adc fifo interrupt type definition
  * @{
  */
#define ADC_INT_REGULAR_FIFO_RDY      (1 << 0)
#define ADC_INT_REGULAR_FIFO_OVERRUN  (1 << 1)
#define ADC_INT_REGULAR_FIFO_UNDERRUN (1 << 2)
#define ADC_INT_INJECT_FIFO0_RDY      (1 << 3)
#define ADC_INT_INJECT_FIFO0_OVERRUN  (1 << 4)
#define ADC_INT_INJECT_FIFO0_UNDERRUN (1 << 5)
#define ADC_INT_INJECT_FIFO1_RDY      (1 << 6)
#define ADC_INT_INJECT_FIFO1_OVERRUN  (1 << 7)
#define ADC_INT_INJECT_FIFO1_UNDERRUN (1 << 8)
#define ADC_INT_INJECT_FIFO2_RDY      (1 << 9)
#define ADC_INT_INJECT_FIFO2_OVERRUN  (1 << 10)
#define ADC_INT_INJECT_FIFO2_UNDERRUN (1 << 11)
#define ADC_INT_INJECT_FIFO3_RDY      (1 << 12)
#define ADC_INT_INJECT_FIFO3_OVERRUN  (1 << 13)
#define ADC_INT_INJECT_FIFO3_UNDERRUN (1 << 14)

#define ADC_INT_REGULAR_FIFO_ERROR    (ADC_INT_REGULAR_FIFO_OVERRUN | ADC_INT_REGULAR_FIFO_UNDERRUN)
#define ADC_INT_INJECT_FIFO_OVERRUN   (ADC_INT_INJECT_FIFO0_OVERRUN | \
                                       ADC_INT_INJECT_FIFO1_OVERRUN | \
                                       ADC_INT_INJECT_FIFO2_OVERRUN | \
                                       ADC_INT_INJECT_FIFO3_OVERRUN)
#define ADC_INT_INJECT_FIFO_UNDERRUN  (ADC_INT_INJECT_FIFO0_UNDERRUN | \
                                       ADC_INT_INJECT_FIFO1_UNDERRUN | \
                                       ADC_INT_INJECT_FIFO2_UNDERRUN | \
                                       ADC_INT_INJECT_FIFO3_UNDERRUN)
#define ADC_INT_INJECT_FIFO_ERROR     (ADC_INT_INJECT_FIFO_OVERRUN | ADC_INT_INJECT_FIFO_UNDERRUN)
#define ADC_INT_INJECT_FIFO_RDY       (ADC_INT_INJECT_FIFO0_RDY | \
                                       ADC_INT_INJECT_FIFO1_RDY | \
                                       ADC_INT_INJECT_FIFO2_RDY | \
                                       ADC_INT_INJECT_FIFO3_RDY)
#define ADC_INT_FIFO_ALL              (ADC_INT_REGULAR_FIFO_RDY | ADC_INT_REGULAR_FIFO_ERROR | \
                                       ADC_INT_INJECT_FIFO_RDY | ADC_INT_INJECT_FIFO_ERROR)
/**
  * @}
  */

/** @defgroup ADC_CMD dma feature control cmd definition
  * @{
  */
#define ADC_CMD_VBAT_EN            (0x01)
#define ADC_CMD_REGULAR_DATA_SHIFT (0x02)
#define ADC_CMD_INJECT_DATA_SHIFT  (0x03)
#define ADC_CMD_CONV_DELAY         (0x04)
#define ADC_CMD_PWM_TRIG_EN        (0x05)
#define ADC_CMD_SET_FIFO_THRESHOLD (0x06)
#define ADC_CMD_SET_OFFSET_CALI    (0x07)
/**
  * @}
  */

// clang-format off
#define IS_ADC_MODE(type)               (((type) == ADC_MODE_SEPARATE)   || \
                                         ((type) == ADC_MODE_INTERLEAVE) || \
                                         ((type) == ADC_MODE_CONCURRENT))

#define IS_ADC_EXTERNAL_CHANNEL(type)   ((type) <= ADC_EXTERNAL_CHANNEL_NULL)
#define IS_ADC0_INTERNAL_CHANNEL(type)  ((type) <= ADC0_INTERNAL_CHANNEL_AVSS)
#define IS_ADC_INJECT_CHANNEL(type)     ((type) <= ADC_INJECT_CHANNEL_3)

#define IS_ADC_CLK_DIV(type)            (((type) <= ADC_CLK_DIV_32) && ((type) >= ADC_CLK_DIV_1))

#define IS_ADC_RESOLUTION(type)         (((type) == ADC_RESOLUTION_12B) || \
                                         ((type) == ADC_RESOLUTION_14B) || \
                                         ((type) == ADC_RESOLUTION_16B))

#define IS_ADC_VREF(type)               (((type) == ADC_VREF_INTERNAL_1P25) || \
                                         ((type) == ADC_VREF_EXTERNAL_1P25))

#define IS_ADC_TSEN_MODE(type)          (((type) == ADC_TSEN_MOD_INTERNAL_DIODE) || \
                                         ((type) == ADC_TSEN_MOD_EXTERNAL_DIODE))

// clang-format on

/**
 * @brief ADC common configuration structure
 *
 * @param clk_div                ADC clock divison, use @ref ADC_CLK_DIV, adc clk must be less than 2M.
 * @param mode                   ADC work mode, use @ref ADC_MODE
 * @param fifo1_enable           ADC fifo1 enable of disable, when enabled, data of ADC1 is also placed in FIFO0
 */
struct bflb_adc_common_config_s {
    uint8_t clk_div;
    uint8_t mode;
    uint8_t fifo1_enable;
};

/**
 * @brief ADC watch dog configuration structure
 *
 * @param enable                 ADC watch dog enable of disable
 * @param adc_id                 ADC ID monitored by watch dog
 * @param channel_type           0 for regular channel, 1 for inject channel monitored by watch dog
 * @param channel_id             channel ID monitored by watch dog
 * @param upper_threshold        ADC watch dog upper threshold value
 * @param lower_threshold        ADC watch dog lower threshold value
 * @param counter_threshold      ADC watch dog counter threshold
 */
struct bflb_adc_watchdog_config_s {
    uint8_t enable;
    uint8_t adc_id;
    uint8_t channel_type;
    uint8_t channel_id;
    uint16_t upper_threshold;
    uint16_t lower_threshold;
    uint8_t counter_threshold;
};

/**
 * @brief ADC configuration structure
 *
 * @param scan_conv_mode         ADC scan mode enable
 * @param continuous_conv_mode   ADC continuous conversion enable
 * @param differential_mode      ADC differential mode enable
 * @param resolution             ADC resolution, use @ref ADC_RESOLUTION
 * @param vref                   ADC reference select, use @ref ADC_VREF
 */
struct bflb_adc_config_s {
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
 * @param adc_id           ADC ID
 * @param idx              ADC index of scan or inject
 * @param pos_chan         ADC positive channel
 * @param neg_chan         ADC negative channel
 * @param value            ADC value
 * @param millivolt        ADC voltage result
 */
struct bflb_adc_result_s {
    uint8_t adc_id;
    uint8_t idx;
    int8_t pos_chan;
    int8_t neg_chan;
    int32_t value;
    int32_t millivolt;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Common initialize adc.
 *
 * @param [in] config pointer to save adc common configuration
 */
void bflb_adc_common_init(const struct bflb_adc_common_config_s *config);

/**
 * @brief Initialize adc.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save adc configuration
 */
void bflb_adc_init(struct bflb_device_s *dev, const struct bflb_adc_config_s *config);

/**
 * @brief Deinitialize adc.
 *
 * @param [in] dev device handle
 */
void bflb_adc_deinit(struct bflb_device_s *dev);

/**
 * @brief Enable adc rx dma.
 *
 * @param [in] dev device handle
 * @param [in] enable true means enable, otherwise disable.
 */
void bflb_adc_link_rxdma(struct bflb_device_s *dev, bool enable);


/**
 * @brief Config adc internal regular channels to sample.
 *
 * @param [in] dev device handle
 * @param [in] chan pointer to the channel configurations.
 * @param [in] channels pair number of channels
 * @return Zero on success; a negated errno value on failure
 */
int bflb_adc_channel_config_internal(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels);

/**
 * @brief Config adc external regular channels to sample.
 *
 * @param [in] dev device handle
 * @param [in] chan pointer to the channel configurations.
 * @param [in] channels pair number of channels
 * @return Zero on success; a negated errno value on failure
 */
int bflb_adc_channel_config_external(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels);

/**
 * @brief Config adc internal inject channels to sample.
 *
 * @param [in] dev device handle
 * @param [in] chan pointer to the channel configurations.
 * @param [in] channels pair number of channels
 * @return Zero on success; a negated errno value on failure
 */
int bflb_adc_channel_config_internal_inject(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels);

/**
 * @brief Config adc external inject channels to sample.
 *
 * @param [in] dev device handle
 * @param [in] chan pointer to the channel configurations.
 * @param [in] channels pair number of channels
 * @return Zero on success; a negated errno value on failure
 */
int bflb_adc_channel_config_external_inject(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels);

/**
 * @brief Start adc regular channels conversion
 *
 * @param [in] dev device handle
 */
void bflb_adc_start_conversion(struct bflb_device_s *dev);

/**
 * @brief Start adc inject channels conversion
 *
 * @param [in] dev device handle
 */
void bflb_adc_start_conversion_inject(struct bflb_device_s *dev);

/**
 * @brief Stop adc regular channels conversion
 *
 * @param [in] dev device handle
 */
void bflb_adc_stop_conversion(struct bflb_device_s *dev);

/**
 * @brief Stop adc inject channels conversion
 *
 * @param [in] dev device handle
 */
void bflb_adc_stop_conversion_inject(struct bflb_device_s *dev);

/**
 * @brief Get adc number of regular channels completed conversions
 *
 * @param [in] dev device handle
 * @return number of completed conversions
 */
uint8_t bflb_adc_get_count(struct bflb_device_s *dev);

/**
 * @brief Get adc number of inject channels completed conversions
 *
 * @param [in] dev device handle
 * @param [in] idx inject channel index
 * @return number of completed conversions
 */
uint8_t bflb_adc_get_count_inject(struct bflb_device_s *dev, uint8_t idx);

/**
 * @brief Read adc regular channels conversion value
 *
 * @param [in] dev device handle
 * @return conversion value
 */
uint32_t bflb_adc_read_raw(struct bflb_device_s *dev);

/**
 * @brief Read adc inject channels conversion value
 *
 * @param [in] dev device handle
 * @param [in] idx inject channel index
 * @return conversion value
 */
uint32_t bflb_adc_read_raw_inject(struct bflb_device_s *dev, uint8_t idx);

/**
 * @brief Clear ADC regular channels fifo
 *
 * @param [in] dev device handle
 */
void bflb_adc_clear_fifo(struct bflb_device_s *dev);

/**
 * @brief Clear ADC inject channels fifo
 *
 * @param [in] dev device handle
 * @param [in] idx inject channel index
 */
void bflb_adc_clear_fifo_inject(struct bflb_device_s *dev, uint8_t idx);

/**
 * @brief Enable or disable adc interrupt.
 *
 * @param [in] dev device handle
 * @param [in] int_type saturation interrupt type value, use @ref ADC_INT_SATURATION
 * @param [in] mask true means disable, false means enable
 */
void bflb_adc_int_saturation_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask);

/**
 * @brief Enable or disable adc watch dog interrupt.
 *
 * @param [in] dev device handle
 * @param [in] int_type interrupt type value, use @ref ADC_INT_WATCHDOG
 * @param [in] mask true means disable, false means enable
 */
void bflb_adc_int_watchdog_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask);

/**
 * @brief Enable or disable adc done interrupt.
 *
 * @param [in] dev device handle
 * @param [in] int_type interrupt type value, use @ref ADC_INT_DONE
 * @param [in] mask true means disable, false means enable
 */
void bflb_adc_int_done_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask);

/**
 * @brief Enable or disable adc fifo interrupt.
 *
 * @param [in] dev device handle
 * @param [in] int_type interrupt type value, use @ref ADC_INT
 * @param [in] mask true means disable, false means enable
 */
void bflb_adc_int_fifo_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask);

/**
 * @brief Get adc saturation interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type saturation interrupt type value, use @ref ADC_INT_SATURATION
 * @return saturation interrupt status value, use @ref ADC_INT_SATURATION
 */
uint32_t bflb_adc_get_int_saturation_status(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Get adc watchdog interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type watchdog interrupt type value, use @ref ADC_INT_WATCHDOG
 * @return watchdog interrupt status value, use @ref ADC_INT_WATCHDOG
 */
uint32_t bflb_adc_get_int_watchdog_status(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Get adc done interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type done interrupt type value, use @ref ADC_INT_DONE
 * @return done interrupt status value, use @ref ADC_INT_DONE
 */
uint32_t bflb_adc_get_int_done_status(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Get adc fifo interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type fifo interrupt type value, use @ref ADC_INT_FIFO
 * @return fifo interrupt status value, use @ref ADC_INT_FIFO
 */
uint32_t bflb_adc_get_int_fifo_status(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Clear adc saturation interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type saturation interrupt type value, use @ref ADC_INT_SATURATION
 */
void bflb_adc_int_saturation_clear(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Clear adc watchdog interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type watchdog interrupt type value, use @ref ADC_INT_WATCHDOG
 */
void bflb_adc_int_watchdog_clear(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Clear adc done interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type done interrupt type value, use @ref ADC_INT_DONE
 */
void bflb_adc_int_done_clear(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Clear adc fifo interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type fifo interrupt type value, use @ref ADC_INT_FIFO
 */
void bflb_adc_int_fifo_clear(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Parse adc conversion value into millivolt and actual numerical value.
 *
 * @param [in] dev device handle
 * @param [in] buffer pointer to adc original value from bflb_adc_read_raw or bflb_adc_read_raw_inject api
 * @param [out] result pointer to save parse result
 * @param [in] count count to parse
 */
void bflb_adc_parse_result(struct bflb_device_s *dev, uint32_t *buffer, struct bflb_adc_result_s *result, uint16_t count);

/**
 * @brief Initialize adc temperature sensor
 *
 * @param [in] dev device handle
 * @param [in] tsen_mod temperature sensor mode, use @ref ADC_TSEN_MOD
 */
void bflb_adc_tsen_init(struct bflb_device_s *dev, uint8_t tsen_mod);

/**
 * @brief Get adc temperature
 *
 * @param [in] dev device handle
 * @return temperature
 */
float bflb_adc_tsen_get_temp(struct bflb_device_s *dev);

/**
 * @brief Initialize adc watch dog.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save adc watchdog configuration
 */
void bflb_adc_watchdog_init(struct bflb_device_s *dev, struct bflb_adc_watchdog_config_s *config);

/**
 * @brief Control adc feature.
 *
 * @param [in] dev device handle
 * @param [in] cmd feature command. use @ref ADC_CMD
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_adc_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

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