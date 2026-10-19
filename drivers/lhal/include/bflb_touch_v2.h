#ifndef _BFLB_TOUCH_V2_H
#define _BFLB_TOUCH_V2_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup TOUCH_V2
  * @{
  */

/** @defgroup TOUCH_V2_CLK_SEL Touch V2 clock source select type definition
  * @{
  */
#define TOUCH_V2_CLK_SEL_32K         (0) /*!< select 32K as TOUCH clock */
#define TOUCH_V2_CLK_SEL_32M         (1) /*!< select 32M as TOUCH clock */
/**
  * @}
  */

/** @defgroup TOUCH_V2_CLK_DIV Touch V2 clock divider type definition
  * @{
  */
#define TOUCH_V2_CLK_DIV_8           (0) /*!< TOUCH clock div=8 */
#define TOUCH_V2_CLK_DIV_16          (1) /*!< TOUCH clock div=16 */
#define TOUCH_V2_CLK_DIV_32          (2) /*!< TOUCH clock div=32 */
#define TOUCH_V2_CLK_DIV_64          (3) /*!< TOUCH clock div=64 */
#define TOUCH_V2_CLK_DIV_128         (4) /*!< TOUCH clock div=128 */
#define TOUCH_V2_CLK_DIV_256         (5) /*!< TOUCH clock div=256 */
#define TOUCH_V2_CLK_DIV_512         (6) /*!< TOUCH clock div=512 */
#define TOUCH_V2_CLK_DIV_1024        (7) /*!< TOUCH clock div=1024 */
/**
  * @}
  */

/** @defgroup TOUCH_V2_VREF_VOLTAGE Touch V2 reference voltage control type definition
  * @{
  */
#define TOUCH_V2_VREF_0P4V           (0)  /*!< Vref 0.4V */
#define TOUCH_V2_VREF_0P5V           (1)  /*!< Vref 0.5V */
#define TOUCH_V2_VREF_0P6V           (2)  /*!< Vref 0.6V */
#define TOUCH_V2_VREF_0P7V           (3)  /*!< Vref 0.7V */
#define TOUCH_V2_VREF_0P8V           (4)  /*!< Vref 0.8V */
#define TOUCH_V2_VREF_0P9V           (5)  /*!< Vref 0.9V */
#define TOUCH_V2_VREF_1P0V           (6)  /*!< Vref 1.0V */
#define TOUCH_V2_VREF_1P1V           (7)  /*!< Vref 1.1V */
#define TOUCH_V2_VREF_1P2V           (8)  /*!< Vref 1.2V */
#define TOUCH_V2_VREF_1P3V           (9)  /*!< Vref 1.3V */
#define TOUCH_V2_VREF_1P4V           (10) /*!< Vref 1.4V */
#define TOUCH_V2_VREF_1P5V           (11) /*!< Vref 1.5V */
#define TOUCH_V2_VREF_1P6V           (12) /*!< Vref 1.6V */
#define TOUCH_V2_VREF_1P75V          (13) /*!< Vref 1.75V */
#define TOUCH_V2_VREF_1P8V           (14) /*!< Vref 1.8V */
#define TOUCH_V2_VREF_1P8V_DUP       (15) /*!< Vref 1.8V (duplicate) */
/**
  * @}
  */

/** @defgroup TOUCH_V2_VLDO_VOLTAGE Touch V2 LDO voltage control type definition
  * @{
  */
#define TOUCH_V2_VLDO_0P6V           (0)  /*!< LDO output 0.6V */
#define TOUCH_V2_VLDO_0P7V           (1)  /*!< LDO output 0.7V */
#define TOUCH_V2_VLDO_0P8V           (2)  /*!< LDO output 0.8V */
#define TOUCH_V2_VLDO_0P9V           (3)  /*!< LDO output 0.9V */
#define TOUCH_V2_VLDO_1P0V           (4)  /*!< LDO output 1.0V */
#define TOUCH_V2_VLDO_1P1V           (5)  /*!< LDO output 1.1V */
#define TOUCH_V2_VLDO_1P2V           (6)  /*!< LDO output 1.2V */
#define TOUCH_V2_VLDO_1P3V           (7)  /*!< LDO output 1.3V */
#define TOUCH_V2_VLDO_1P4V           (8)  /*!< LDO output 1.4V */
#define TOUCH_V2_VLDO_1P5V           (9)  /*!< LDO output 1.5V */
#define TOUCH_V2_VLDO_1P6V           (10) /*!< LDO output 1.6V */
#define TOUCH_V2_VLDO_1P7V           (11) /*!< LDO output 1.7V */
#define TOUCH_V2_VLDO_1P8V           (12) /*!< LDO output 1.8V */
#define TOUCH_V2_VLDO_1P9V           (13) /*!< LDO output 1.9V */
#define TOUCH_V2_VLDO_2P0V           (14) /*!< LDO output 2.0V */
#define TOUCH_V2_VLDO_2P1V           (15) /*!< LDO output 2.1V */
/**
  * @}
  */

/** @defgroup TOUCH_V2_HYSTERESIS Touch V2 comparator hysteresis voltage type definition
  * @{
  */
#define TOUCH_V2_HYS_22MV            (0) /*!< set 22mV comparator hysteresis voltage */
#define TOUCH_V2_HYS_65MV            (1) /*!< set 65mV comparator hysteresis voltage */
/**
  * @}
  */

/** @defgroup TOUCH_V2_PCHARGE_LOW Touch V2 pcharge low type definition
  * @{
  */
#define TOUCH_V2_PCHARGE_LOW_31      (0) /*!< pcharge low time 31 clk cycles */
#define TOUCH_V2_PCHARGE_LOW_63      (1) /*!< pcharge low time 63 clk cycles */
#define TOUCH_V2_PCHARGE_LOW_127     (2) /*!< pcharge low time 127 clk cycles */
#define TOUCH_V2_PCHARGE_LOW_255     (3) /*!< pcharge low time 255 clk cycles */
#define TOUCH_V2_PCHARGE_LOW_511     (4) /*!< pcharge low time 511 clk cycles */
#define TOUCH_V2_PCHARGE_LOW_1023    (5) /*!< pcharge low time 1023 clk cycles */
#define TOUCH_V2_PCHARGE_LOW_2047    (6) /*!< pcharge low time 2047 clk cycles */
#define TOUCH_V2_PCHARGE_LOW_4095    (7) /*!< pcharge low time 4095 clk cycles */
/**
  * @}
  */

/** @defgroup TOUCH_V2_PCHARGE_HIGH Touch V2 pcharge high type definition
  * @{
  */
#define TOUCH_V2_PCHARGE_HIGH_511    (0) /*!< pcharge high time 511 clk cycles */
#define TOUCH_V2_PCHARGE_HIGH_1023   (1) /*!< pcharge high time 1023 clk cycles */
#define TOUCH_V2_PCHARGE_HIGH_2047   (2) /*!< pcharge high time 2047 clk cycles */
#define TOUCH_V2_PCHARGE_HIGH_4095   (3) /*!< pcharge high time 4095 clk cycles */
#define TOUCH_V2_PCHARGE_HIGH_8191   (4) /*!< pcharge high time 8191 clk cycles */
#define TOUCH_V2_PCHARGE_HIGH_16383  (5) /*!< pcharge high time 16383 clk cycles */
#define TOUCH_V2_PCHARGE_HIGH_32767  (6) /*!< pcharge high time 32767 clk cycles */
#define TOUCH_V2_PCHARGE_HIGH_65535  (7) /*!< pcharge high time 65535 clk cycles */
/**
  * @}
  */

/** @defgroup TOUCH_V2_LDO_SETTLE_CYC Touch V2 LDO settle cycles type definition
  * @{
  */
#define TOUCH_V2_LDO_SETTLE_1        (0) /*!< LDO settle 1 cycle */
#define TOUCH_V2_LDO_SETTLE_80       (1) /*!< LDO settle 80 cycles */
#define TOUCH_V2_LDO_SETTLE_160      (2) /*!< LDO settle 160 cycles */
#define TOUCH_V2_LDO_SETTLE_240      (3) /*!< LDO settle 240 cycles */
#define TOUCH_V2_LDO_SETTLE_320      (4) /*!< LDO settle 320 cycles */
#define TOUCH_V2_LDO_SETTLE_400      (5) /*!< LDO settle 400 cycles */
#define TOUCH_V2_LDO_SETTLE_480      (6) /*!< LDO settle 480 cycles */
#define TOUCH_V2_LDO_SETTLE_560      (7) /*!< LDO settle 560 cycles */
/**
  * @}
  */

/** @defgroup TOUCH_V2_FILTER_ORDER Touch V2 filter order type definition
  * @{
  */
#define TOUCH_V2_FILTER_ORDER_1      (0) /*!< Filter order 1 */
#define TOUCH_V2_FILTER_ORDER_2      (1) /*!< Filter order 2 */
#define TOUCH_V2_FILTER_ORDER_4      (2) /*!< Filter order 4 */
#define TOUCH_V2_FILTER_ORDER_RES    (3) /*!< Reserved for IIR filter only */
/**
  * @}
  */

/** @defgroup TOUCH_V2_LTA_ORDER Touch V2 LTA order type definition
  * @{
  */
#define TOUCH_V2_LTA_ORDER_1         (0) /*!< LTA order 1 */
#define TOUCH_V2_LTA_ORDER_2         (1) /*!< LTA order 2 */
#define TOUCH_V2_LTA_ORDER_4         (2) /*!< LTA order 4 */
#define TOUCH_V2_LTA_ORDER_RES       (3) /*!< Reserved for IIR LTA only */
/**
  * @}
  */

/** @defgroup TOUCH_V2_FILTER_TYPE Touch V2 filter type definition
  * @{
  */
#define TOUCH_V2_FILTER_IIR          (0) /*!< IIR filter */
#define TOUCH_V2_FILTER_FIR          (1) /*!< FIR filter */
/**
  * @}
  */

/** @defgroup TOUCH_V2_CHANNEL Touch V2 channel type definition
  * @{
  */
#define TOUCH_V2_CHANNEL_0           (0)  /*!< Touch channel 0 */
#define TOUCH_V2_CHANNEL_1           (1)  /*!< Touch channel 1 */
#define TOUCH_V2_CHANNEL_2           (2)  /*!< Touch channel 2 */
#define TOUCH_V2_CHANNEL_3           (3)  /*!< Touch channel 3 */
#define TOUCH_V2_CHANNEL_4           (4)  /*!< Touch channel 4 */
#define TOUCH_V2_CHANNEL_5           (5)  /*!< Touch channel 5 */
#define TOUCH_V2_CHANNEL_6           (6)  /*!< Touch channel 6 */
#define TOUCH_V2_CHANNEL_7           (7)  /*!< Touch channel 7 */
#define TOUCH_V2_CHANNEL_8           (8)  /*!< Touch channel 8 */
#define TOUCH_V2_CHANNEL_9           (9)  /*!< Touch channel 9 */
#define TOUCH_V2_CHANNEL_10          (10) /*!< Touch channel 10 */
#define TOUCH_V2_CHANNEL_11          (11) /*!< Touch channel 11 */
#define TOUCH_V2_CHANNEL_12          (12) /*!< Touch channel 12 */
#define TOUCH_V2_CHANNEL_13          (13) /*!< Touch channel 13 */
#define TOUCH_V2_CHANNEL_14          (14) /*!< Touch channel 14 */
#define TOUCH_V2_CHANNEL_15          (15) /*!< Touch channel 15 */
#define TOUCH_V2_CHANNEL_MAX         (16) /*!< Max channel number */
/**
  * @}
  */

/** @defgroup TOUCH_V2_INT_STATUS Touch V2 interrupt status type definition
  * @{
  */
#define TOUCH_V2_INT_DET_MASK        (0xFFFF << 0) /*!< Touch detection interrupt */
#define TOUCH_V2_INT_SCAN_LAST_MASK  (1 << 16)     /*!< Last channel scan interrupt */
#define TOUCH_V2_INT_HOP_FREQ_0_MASK (1 << 17)     /*!< Frequency hopping threshold interrupt 0 */
#define TOUCH_V2_INT_HOP_FREQ_1_MASK (1 << 18)     /*!< Frequency hopping threshold interrupt 1 */
/**
  * @}
  */

/**
 * @brief Touch V2 configuration structure
 */
struct bflb_touch_v2_config_s {
    uint8_t clk_sel;         /*!< Clock source selection: TOUCH_V2_CLK_SEL_32K or TOUCH_V2_CLK_SEL_32M */
    uint8_t clk_div;         /*!< Clock divider: TOUCH_V2_CLK_DIV_8 ~ TOUCH_V2_CLK_DIV_1024 */
    uint8_t vref_sel;        /*!< Reference voltage: TOUCH_V2_VREF_0P4V ~ TOUCH_V2_VREF_1P8V */
    uint8_t vldo_sel;        /*!< LDO voltage: TOUCH_V2_VLDO_0P6V ~ TOUCH_V2_VLDO_2P1V */
    uint8_t hys_sel;         /*!< Hysteresis: TOUCH_V2_HYS_22MV or TOUCH_V2_HYS_65MV */
    uint8_t pcharge_low;     /*!< Pcharge low time: TOUCH_V2_PCHARGE_LOW_31 ~ TOUCH_V2_PCHARGE_LOW_4095 */
    uint8_t pcharge_high;    /*!< Pcharge high time: TOUCH_V2_PCHARGE_HIGH_511 ~ TOUCH_V2_PCHARGE_HIGH_65535 */
    uint8_t ldo_settle_cyc;  /*!< LDO settle cycles: TOUCH_V2_LDO_SETTLE_1 ~ TOUCH_V2_LDO_SETTLE_560 */
    uint8_t filter_order;    /*!< Filter order: TOUCH_V2_FILTER_ORDER_1 ~ TOUCH_V2_FILTER_ORDER_4 */
    uint8_t filter_sel;      /*!< Filter type: TOUCH_V2_FILTER_IIR or TOUCH_V2_FILTER_FIR */
    uint8_t lta_order;       /*!< LTA order: TOUCH_V2_LTA_ORDER_1 ~ TOUCH_V2_LTA_ORDER_4 */
    uint8_t lta_sel;         /*!< LTA type: TOUCH_V2_FILTER_IIR or TOUCH_V2_FILTER_FIR */
    uint8_t scn_lst_chl_cnt; /*!< Last channel scan end times minus 1 (0-63) */
    uint32_t sleep_cycle;    /*!< Sleep cycles in continuous mode (0-0x7FFFFF) */
    uint16_t tx_channel_en;  /*!< TX channel enable bitmap (bit0=ch0, bit1=ch1, ...) */
    uint16_t rx_channel_en;  /*!< RX channel enable bitmap (bit0=ch0, bit1=ch1, ...) */
    uint16_t as_channel_sel; /*!< Active shielding channel selection bitmap */
    uint16_t det_int_mask;   /*!< Detection interrupt mask (bit0=ch0, bit1=ch1, ...) */
    uint8_t scan_int_mask;  /*!< Scan last interrupt mask */
    uint8_t hop_int_mask;   /*!< Frequency hopping interrupt mask */
    uint16_t det_dur;        /*!< Touch detection duration (number of detection times to assert interrupt) */
    bool touch_32m_clk_sel;  /*!< true: xtal clock, false: rc clock as 32M source */
    bool cont_mode_en;       /*!< Continuous mode enable */
    bool wakeup_mode_en;     /*!< Ultra low power(wake up) mode 2 enable */
    bool as_chl_sel;         /*!< Assert channel select */
    bool det_on;             /*!< Detection mode: true=on to off, false=off to on */
    bool det_keep;           /*!< Keep touch detection status when channel scan again */
    bool data_hys_en;        /*!< Hysteresis time enable for touch detect */
    bool force_val_en;       /*!< Force enable to force force_data to LTA data */
    bool channel_cal_en;     /*!< Channel calibration enable to update LTA data */
    bool filter_en;          /*!< Filter enable */
    bool lta_en;             /*!< LTA enable */
    bool hop_freq_en;        /*!< Frequency hopping enable */
    uint8_t hop_freq_th;     /*!< Frequency hopping threshold (0-255) */
    uint8_t hop_freq_div0;   /*!< First scan clock div for frequency hopping (0-15) */
    uint8_t hop_freq_div1;   /*!< Second scan clock div for frequency hopping (0-15) */
    uint8_t hop_freq_div2;   /*!< Third scan clock div for frequency hopping (0-15) */
};

/**
 * @brief Touch V2 channel data structure
 */
struct bflb_touch_v2_channel_data_s {
    uint16_t raw_data;   /*!< Raw touch data */
    uint16_t flt_data;   /*!< Filtered touch data */
    uint16_t lta_data;   /*!< LTA touch data */
    uint16_t force_data; /*!< Force data */
    uint8_t vth_data;    /*!< Threshold data */
    uint8_t hys_data;    /*!< Hysteresis data */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize touch V2 with configuration
 *
 * @param[in] dev touch V2 device handle
 * @param[in] config touch V2 configuration
 *
 * @return 0 on success, negative error code otherwise
 */
int bflb_touch_v2_init(struct bflb_device_s *dev, const struct bflb_touch_v2_config_s *config);

/**
 * @brief Deinitialize touch V2
 *
 * @param[in] dev touch V2 device handle
 */
void bflb_touch_v2_deinit(struct bflb_device_s *dev);

/**
 * @brief Enable touch V2 module
 *
 * @param[in] dev touch V2 device handle
 */
void bflb_touch_v2_enable(struct bflb_device_s *dev);

/**
 * @brief Disable touch V2 module
 *
 * @param[in] dev touch V2 device handle
 */
void bflb_touch_v2_disable(struct bflb_device_s *dev);

/**
 * @brief Start touch V2 scanning
 *
 * @param[in] dev touch V2 device handle
 */
void bflb_touch_v2_scan_start(struct bflb_device_s *dev);

/**
 * @brief Stop touch V2 scanning
 *
 * @param[in] dev touch V2 device handle
 */
void bflb_touch_v2_scan_stop(struct bflb_device_s *dev);

/**
 * @brief Perform software reset
 *
 * @param[in] dev touch V2 device handle
 */
void bflb_touch_v2_reset(struct bflb_device_s *dev);

/**
 * @brief Set channel threshold data
 *
 * @param[in] dev touch V2 device handle
 * @param[in] channel channel number (0-15)
 * @param[in] threshold threshold value (0-255)
 */
void bflb_touch_v2_set_channel_threshold(struct bflb_device_s *dev, uint8_t channel, uint8_t threshold);

/**
 * @brief Set channel hysteresis data
 *
 * @param[in] dev touch V2 device handle
 * @param[in] channel channel number (0-15)
 * @param[in] hys hysteresis value (0-15)
 */
void bflb_touch_v2_set_channel_hysteresis(struct bflb_device_s *dev, uint8_t channel, uint8_t hys);

/**
 * @brief Set channel force data
 *
 * @param[in] dev touch V2 device handle
 * @param[in] channel channel number (0-15)
 * @param[in] force_data force data value (0-65535)
 */
void bflb_touch_v2_set_channel_force_data(struct bflb_device_s *dev, uint8_t channel, uint16_t force_data);

/**
 * @brief Latch force data to LTA data
 *
 * @param[in] dev touch V2 device handle
 */
void bflb_touch_v2_latch_force_data(struct bflb_device_s *dev);

/**
 * @brief Get channel data
 *
 * @param[in] dev touch V2 device handle
 * @param[in] channel channel number (0-15)
 * @param[out] data channel data structure
 */
void bflb_touch_v2_get_channel_data(struct bflb_device_s *dev, uint8_t channel, struct bflb_touch_v2_channel_data_s *data);

/**
 * @brief Get raw data for a specific channel
 *
 * @param[in] dev touch V2 device handle
 * @param[in] channel channel number (0-15)
 *
 * @return raw data value
 */
uint16_t bflb_touch_v2_get_raw_data(struct bflb_device_s *dev, uint8_t channel);

/**
 * @brief Get filtered data for a specific channel
 *
 * @param[in] dev touch V2 device handle
 * @param[in] channel channel number (0-15)
 *
 * @return filtered data value
 */
uint16_t bflb_touch_v2_get_flt_data(struct bflb_device_s *dev, uint8_t channel);

/**
 * @brief Get LTA data for a specific channel
 *
 * @param[in] dev touch V2 device handle
 * @param[in] channel channel number (0-15)
 *
 * @return LTA data value
 */
uint16_t bflb_touch_v2_get_lta_data(struct bflb_device_s *dev, uint8_t channel);

/**
 * @brief Set TX channel enable mask
 *
 * @param[in] dev touch V2 device handle
 * @param[in] channels channel bitmap (bit0=ch0, bit1=ch1, ...)
 */
void bflb_touch_v2_set_tx_channel(struct bflb_device_s *dev, uint16_t channels);

/**
 * @brief Set RX channel enable mask
 *
 * @param[in] dev touch V2 device handle
 * @param[in] channels channel bitmap (bit0=ch0, bit1=ch1, ...)
 */
void bflb_touch_v2_set_rx_channel(struct bflb_device_s *dev, uint16_t channels);

/**
 * @brief Get interrupt status
 *
 * @param[in] dev touch V2 device handle
 *
 * @return interrupt status bitmap
 */
uint32_t bflb_touch_v2_get_int_status(struct bflb_device_s *dev);

/**
 * @brief Clear interrupt status
 *
 * @param[in] dev touch V2 device handle
 * @param[in] int_mask interrupt mask to clear
 */
void bflb_touch_v2_clear_int(struct bflb_device_s *dev, uint32_t int_mask);

/**
 * @brief Set interrupt mask
 *
 * @param[in] dev touch V2 device handle
 * @param[in] int_mask interrupt mask to set
 */
void bflb_touch_v2_set_int_mask(struct bflb_device_s *dev, uint32_t int_mask);

/**
 * @brief Configure frequency hopping
 *
 * @param[in] dev touch V2 device handle
 * @param[in] enable enable frequency hopping
 * @param[in] threshold frequency hopping threshold (0-255)
 * @param[in] div0 first scan clock divider (0-15)
 * @param[in] div1 second scan clock divider (0-15)
 * @param[in] div2 third scan clock divider (0-15)
 */
void bflb_touch_v2_config_freq_hopping(struct bflb_device_s *dev, bool enable, uint8_t threshold,
                                       uint8_t div0, uint8_t div1, uint8_t div2);

/**
 * @brief Set touch detection duration
 *
 * @param[in] dev touch V2 device handle
 * @param[in] duration number of touch detection times to assert interrupt (0-65535)
 */
void bflb_touch_v2_set_detection_duration(struct bflb_device_s *dev, uint16_t duration);

/**
 * @brief Configure sleep cycle for non-continuous mode
 *
 * @param[in] dev touch V2 device handle
 * @param[in] sleep_cycle sleep cycles (0-0x7FFFFF)
 */
void bflb_touch_v2_set_sleep_cycle(struct bflb_device_s *dev, uint32_t sleep_cycle);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif /* _BFLB_TOUCH_V2_H */
