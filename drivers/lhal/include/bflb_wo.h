#ifndef _BFLB_WO_H
#define _BFLB_WO_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup WO
  * @{
  */

/** @defgroup WO_MODE wo mode definition
  * @{
  */
#define WO_MODE_WRITE   0 /*!< WO direct write I/O */
#define WO_MODE_SET_CLR 1 /*!< WO set/clr I/O */
/**
  * @}
  */

/** @defgroup WO_INT wo interrupt definition
  * @{
  */
#define WO_INT_END      (1 << 0) /*!< WO end Interrupt, WO fifo empty */
#define WO_INT_FIFO     (1 << 1) /*!< WO fifo ready (fifo_available_cnt > fifo_threshold) interrupt, auto-cleared when data is pushed */
#define WO_INT_FER      (1 << 2) /*!< WO fifo error interrupt, auto-cleared when fifo overflow/underflow error flag is cleared */

/**
 * @brief WO configuration structure
 *
 * @param code_total_cnt    WO total count of one period, should be less than 512
 * @param code0_first_cnt   WO first phase count of code0, should be less than 256
 * @param code1_first_cnt   WO first phase count of code1, should be less than 256
 * @param code0_first_level WO first phase level of code0
 * @param code1_first_level WO first phase level of code1
 * @param idle_level        WO level when idle
 * @param fifo_threshold    WO fifo threshold, should be less than 128
 * @param mode              WO mode setting, use @ref WO_MODE
 */
struct bflb_wo_cfg_s {
    uint16_t code_total_cnt;
    uint8_t code0_first_cnt;
    uint8_t code1_first_cnt;
    uint8_t code0_first_level;
    uint8_t code1_first_level;
    uint8_t idle_level;
    uint8_t fifo_threshold;
    uint8_t mode;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize wo pin.
 *
 * @param [in] dev device handle
 * @param [in] pin assert this pin to wo
 * @param [in] mode use @ref WO_MODE
 */
void bflb_wo_pin_init(struct bflb_device_s *dev, uint8_t pin, uint8_t mode);

/**
 * @brief Initialize wo.
 *
 * @param [in] dev device handle
 * @param [in] cfg pointer to save wo config
 */
void bflb_wo_init(struct bflb_device_s *dev, struct bflb_wo_cfg_s *cfg);

/**
 * @brief Enable wo.
 *
 * @param [in] dev device handle
 */
void bflb_wo_enable(struct bflb_device_s *dev);

/**
 * @brief Disable wo.
 *
 * @param [in] dev device handle
 */
void bflb_wo_disable(struct bflb_device_s *dev);

/**
 * @brief Get the count of available space in fifo.
 *
 * @param [in] dev device handle
 * @return the count of available space in fifo
 */
uint32_t bflb_wo_get_fifo_available_cnt(struct bflb_device_s *dev);

/**
 * @brief Put len count of data on wo.
 *
 * @param [in] dev device handle
 * @param [in] data pointer to save send data
 * @param [in] len length to send
 * @return count has been sent successfully.
 */
uint32_t bflb_wo_push_fifo(struct bflb_device_s *dev, uint16_t *data, uint32_t len);

/**
 * @brief Clear wo fifo.
 *
 * @param [in] dev device handle
 */
void bflb_wo_clear_fifo(struct bflb_device_s *dev);

/**
 * @brief Enable wo dma.
 *
 * @param [in] dev device handle
 */
void bflb_wo_enable_dma(struct bflb_device_s *dev);

/**
 * @brief Disable wo dma.
 *
 * @param [in] dev device handle
 */
void bflb_wo_disable_dma(struct bflb_device_s *dev);

/**
 * @brief Get wo interrupt status.
 *
 * @param [in] dev device handle
 * @return interrupt status, use @ref WO_INT
 */
uint32_t bflb_wo_get_int_status(struct bflb_device_s *dev);

/**
 * @brief Disable or mask wo interrupt.
 *
 * @param [in] dev device handle
 * @param [in] int_type mask value, use @ref WO_INT
 */
void bflb_wo_int_mask(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Enable or unmask wo interrupt.
 *
 * @param [in] dev device handle
 * @param [in] int_type unmask value, use @ref WO_INT
 */
void bflb_wo_int_unmask(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Clear wo interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type clear value, use @ref WO_INT
 */
void bflb_wo_int_clear(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Initialize wo uart.
 *
 * @param [in] dev device handle
 * @param [in] baudrate baudrate of uart
 * @param [in] pin pin assert to uart_tx
 */
void bflb_wo_uart_init(struct bflb_device_s *dev, uint32_t baudrate, uint8_t pin);

/**
 * @brief Put one char on wo uart.
 *
 * @param [in] dev device handle
 * @param [in] ch char
 */
void bflb_wo_uart_putchar(struct bflb_device_s *dev, uint8_t ch);

/**
 * @brief Put a block of data on wo uart. This is a poll api.
 *
 * @param [in] dev device handle
 * @param [in] data pointer to save send data
 * @param [in] len length to send
 */
void bflb_wo_uart_put(struct bflb_device_s *dev, uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* _BFLB_WO_H */
