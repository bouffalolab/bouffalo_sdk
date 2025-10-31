#ifndef _BFLB_TIMER_H
#define _BFLB_TIMER_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup TIMER
  * @{
  */

/** @defgroup TIMER_CLK_SOURCE timer clock source definition
  * @{
  */
#if !defined(BL702L)
#define TIMER_CLKSRC_BCLK 0
#endif
#define TIMER_CLKSRC_32K  1
#define TIMER_CLKSRC_1K   2
#define TIMER_CLKSRC_XTAL 3
#if !defined(BL702) && !defined(BL602)
#define TIMER_CLKSRC_GPIO 4
#endif
#define TIMER_CLKSRC_NO   5
/**
  * @}
  */

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
#define TIMER_COMP_NONE 3
/**
  * @}
  */

#if defined(BL616L)
/** @defgroup TIMER_DMA_COMP_ID timer dma request compare id definition
  * @{
  */
#define TIMER_DMA_REQUEST_COMP_ID_0 (1 << 0)
#define TIMER_DMA_REQUEST_COMP_ID_1 (1 << 1)
#define TIMER_DMA_REQUEST_COMP_ID_2 (1 << 2)
#define TIMER_DMA_REQUEST_COMP_NONE (0)
/**
  * @}
  */
#endif

/** @defgroup TIMER_GPIO_PULSE_POLARITY timer gpio pulse polarity definition
  * @{
  */
#define TIMER_GPIO_PULSE_POLARITY_POSITIVE 0
#define TIMER_GPIO_PULSE_POLARITY_NEGATIVE 1
/**
  * @}
  */

#if defined(BL616L)
/** @defgroup TIMER_GPIO_PULSE_INT_MODE timer gpio pulse measure interrupt trigger mode definition
  * @{
  */
#define TIMER_GPIO_PULSE_INT_EVERY_FALLING_EDGE 0
#define TIMER_GPIO_PULSE_INT_EVEN_FALLING_EDGE  1
/**
  * @}
  */
#endif

#if defined(BL616L)
/** @defgroup TIMER_GPIO_PULSE_VALUE timer gpio pulse value definition
  * @{
  */
#define TIMER_GPIO_PULSE_VALUE_0 0
#define TIMER_GPIO_PULSE_VALUE_1 1
#define TIMER_GPIO_PULSE_VALUE_2 2
#define TIMER_GPIO_PULSE_VALUE_3 3
/**
  * @}
  */
#endif

#if defined(BL616L)
/** @defgroup TIMER_CMD timer feature control cmd definition
  * @{
  */
#define TIMER_CMD_DMA_REQUEST_SET_COMPARE_ID (0x01)
#define TIMER_CMD_GPIO_PULSE_SET_ENABLE      (0x02)
#define TIMER_CMD_GPIO_PULSE_SET_INT_MODE    (0x03)
#define TIMER_CMD_GPIO_PULSE_SET_POLARITY    (0x04)
#define TIMER_CMD_GPIO_PULSE_GET_BUSY        (0x05)
#define TIMER_CMD_GPIO_PULSE_GET_VALUE       (0x06)
#define TIMER_CMD_GPIO_PULSE_INT_MASK        (0x07)
#define TIMER_CMD_GPIO_PULSE_INT_CLEAR       (0x08)
#define TIMER_CMD_READ_HW_VERSION            (0x09)
#define TIMER_CMD_READ_SW_USAGE              (0x0a)
#define TIMER_CMD_WRITE_SW_USAGE             (0x0b)
/**
  * @}
  */
#endif

// clang-format off

#define IS_TIMER_COUNTER_MODE(type)   (((type) == TIMER_COUNTER_MODE_PROLOAD) || \
                                      ((type) == TIMER_COUNTER_MODE_UP))

#define IS_TIMER_CLK_SOURCE(type)   ((type) <= 3)

#define IS_TIMER_COMP_ID(type)   (((type) == TIMER_COMP_ID_0) || \
                                  ((type) == TIMER_COMP_ID_1) || \
                                  ((type) == TIMER_COMP_ID_2) || \
                                  ((type) == TIMER_COMP_NONE))

#define IS_TIMER_COMP_VAL(value) ((value) >= 2)

#define IS_TIMER_CLOCK_DIV(type) ((type) <= 255)

// clang-format on

/**
 * @brief TIMER configuration structure
 *
 * @param counter_mode      Timer counter mode, use @ref TIMER_COUNTER_MODE
 * @param clock_source      Timer clock source, use @ref TIMER_CLK_SOURCE
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

#if defined(BL616L)
/**
 * @brief TIMER capture configuration structure
 *
 * @param pin      Timer capture pin
 * @param polarity Timer capture polarity, use @ref TIMER_GPIO_PULSE_POLARITY
 * @param int_mode Timer interupt mode(individual or continuous pulse) , use @ref TIMER_GPIO_PULSE_INT
 */
struct bflb_timer_capture_config_s {
    uint8_t pin;
    uint8_t polarity;
    uint8_t int_mode;
};

/**
 * @brief Structure to store the values of timer pulses.
 *
 * @param goio_lat1 The gpio pos-edge latch1 value for timer0.
 * @param goio_lat2 The gpio neg-edge latch2 value for timer0.
 * @param goio_lat3 The gpio pos-edge latch3 value for timer0.
 * @param goio_lat4 The gpio neg-edge latch4 value for timer0.
 */
struct bflb_timer_capture_value_s {
    uint32_t gpio_lat1;
    uint32_t gpio_lat2;
    uint32_t gpio_lat3;
    uint32_t gpio_lat4;
};
#elif defined(BL616) || defined(BL616D)
/**
 * @brief TIMER capture configuration structure
 *
 * @param pin      Timer capture pin
 * @param polarity Timer capture polarity, use @ref TIMER_GPIO_PULSE_POLARITY
 */
struct bflb_timer_capture_config_s {
    uint8_t pin;
    uint8_t polarity;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize timer.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save timer config
 */
void bflb_timer_init(struct bflb_device_s *dev, const struct bflb_timer_config_s *config);

/**
 * @brief Deinitialize timer.
 *
 * @param [in] dev device handle
 */
void bflb_timer_deinit(struct bflb_device_s *dev);

/**
 * @brief Start timer.
 *
 * @param [in] dev device handle
 */
void bflb_timer_start(struct bflb_device_s *dev);

/**
 * @brief Stop timer.
 *
 * @param [in] dev device handle
 */
void bflb_timer_stop(struct bflb_device_s *dev);

/**
 * @brief Set timer preload value.
 *
 * @param [in] dev device handle
 * @param [in] val preload value
 */
void bflb_timer_set_preloadvalue(struct bflb_device_s *dev, uint32_t val);

/**
 * @brief Set compare value of corresponding compare id.
 *
 * @param [in] dev device handle
 * @param [in] cmp_no compare id, use @ref TIMER_COMP_ID
 * @param [in] val compare value
 */
void bflb_timer_set_compvalue(struct bflb_device_s *dev, uint8_t cmp_no, uint32_t val);

/**
 * @brief Get compare value of corresponding compare id.
 *
 * @param [in] dev device handle
 * @param [in] cmp_no compare id, use @ref TIMER_COMP_ID
 * @return uint32_t
 */
uint32_t bflb_timer_get_compvalue(struct bflb_device_s *dev, uint8_t cmp_no);

/**
 * @brief Get timer counter value.
 *
 * @param [in] dev device handle
 * @return counter value
 */
uint32_t bflb_timer_get_countervalue(struct bflb_device_s *dev);

/**
 * @brief Enable or disable timer interrupt of corresponding compare id.
 *
 * @param [in] dev device handle
 * @param [in] cmp_no compare id, use @ref TIMER_COMP_ID
 * @param [in] mask true means disable, false means enable
 */
void bflb_timer_compint_mask(struct bflb_device_s *dev, uint8_t cmp_no, bool mask);

/**
 * @brief Get timer interrupt status of corresponding compare id.
 *
 * @param [in] dev device handle
 * @param [in] cmp_no compare id, use @ref TIMER_COMP_ID
 * @return true mean yes, otherwise no.
 */
bool bflb_timer_get_compint_status(struct bflb_device_s *dev, uint8_t cmp_no);

/**
 * @brief Clear timer interrupt status of corresponding compare id.
 *
 * @param [in] dev device handle
 * @param [in] cmp_no compare id, use @ref TIMER_COMP_ID
 */
void bflb_timer_compint_clear(struct bflb_device_s *dev, uint8_t cmp_no);

/**
 * @brief Control timer feature.
 *
 * @param [in] dev device handle
 * @param [in] cmd feature command, use @ref TIMER_CMD
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_timer_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

#if defined(BL616L)
/**
 * @brief Initialize the timer capture feature.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to the timer capture configuration structure
 */
void bflb_timer_capture_init(struct bflb_device_s *dev, const struct bflb_timer_capture_config_s *config);

/**
 * @brief Start the timer capture process.
 *
 * @param [in] dev device handle
 * @return 0 on success, or a negative error code on failure.
 */
int bflb_timer_capture_start(struct bflb_device_s *dev);

/**
 * @brief Stop the timer capture process.
 *
 * @param [in] dev device handle
 * @return 0 on success, or a negative error code on failure.
 */
int bflb_timer_capture_stop(struct bflb_device_s *dev);

/**
 * @brief Get the captured latch values.
 *
 * @param [in] dev device handle
 * @param [out] gpio_lat pointer to the structure where the latch values will be stored.
 * @return 0 on success, or a negative error code on failure.
 */
int bflb_timer_capture_get_latch_value(struct bflb_device_s *dev, struct bflb_timer_capture_value_s *gpio_lat);

/**
 * @brief Calculate the pulse width from the captured latch values.
 *
 * @param [in] gpio_lat pointer to the structure containing the latch values.
 * @return The calculated pulse width, or 0 if the pulse width cannot be determined.
 */
int bflb_timer_capture_get_pulsewidth(struct bflb_timer_capture_value_s *gpio_lat);
#elif defined(BL616) || defined(BL616D)
/**
 * @brief Initialize the timer capture feature.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to the timer capture configuration structure
 */
void bflb_timer_capture_init(struct bflb_device_s *dev, const struct bflb_timer_capture_config_s *config);
#endif

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