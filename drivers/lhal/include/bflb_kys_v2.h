#ifndef _BFLB_KYS_V2_H
#define _BFLB_KYS_V2_H

#include "bflb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BFLB_KYS_V2_INT_DONE          (1U << 7)
#define BFLB_KYS_V2_INT_FIFO_FULL     (1U << 8)
#define BFLB_KYS_V2_INT_FIFO_HALF     (1U << 9)
#define BFLB_KYS_V2_INT_FIFO_QUARTER  (1U << 10)
#define BFLB_KYS_V2_INT_FIFO_NONEMPTY (1U << 11)
#define BFLB_KYS_V2_INT_GHOST         (1U << 12)
#define BFLB_KYS_V2_INT_ENABLE_ALL    (BFLB_KYS_V2_INT_DONE | BFLB_KYS_V2_INT_FIFO_FULL |     \
                                       BFLB_KYS_V2_INT_FIFO_HALF | BFLB_KYS_V2_INT_FIFO_QUARTER | \
                                       BFLB_KYS_V2_INT_FIFO_NONEMPTY | BFLB_KYS_V2_INT_GHOST)
#define BFLB_KYS_V2_INT_CLEAR_ALL     (BFLB_KYS_V2_INT_DONE | BFLB_KYS_V2_INT_FIFO_FULL | \
                                       BFLB_KYS_V2_INT_GHOST)
#define BFLB_KYS_V2_MAX_KEYS         (8U)

/**
 * @brief KYS V2 configuration
 */
struct bflb_kys_v2_config_s {
    uint8_t col;           /*!< Matrix column/input count, range 1 to 8 */
    uint8_t row;           /*!< Matrix row/drive count, range 1 to 8 */
    uint8_t deglitch_en;   /*!< Enable input deglitch */
    uint8_t deglitch_cnt;  /*!< Deglitch count, range 0 to 15 */
    uint8_t idle_duration; /*!< Idle duration between column scans, range 0 to 3 */
    uint8_t ghost_en;      /*!< Enable ghost-key detection */
    uint8_t pds_drive_value; /*!< PDS key detect drive output value */
    uint8_t pds_input_pull_en; /*!< Enable PDS key detect input pull */
};

/**
 * @brief KYS V2 scan result
 */
struct bflb_kys_v2_result_s {
    uint32_t status;                         /*!< Raw interrupt status */
    uint8_t count;                           /*!< Number of valid key codes */
    uint8_t keycode[BFLB_KYS_V2_MAX_KEYS];  /*!< Key codes read from the FIFO */
};

/**
 * @brief KYS V2 PDS wakeup result
 */
struct bflb_kys_v2_pds_wakeup_s {
    bool wakeup;      /*!< KYS is the active PDS wakeup source */
    uint8_t row;      /*!< Wakeup row/drive index */
    uint8_t col;      /*!< Wakeup column/input index */
    uint8_t row_gpio; /*!< Drive GPIO selected by the wakeup row */
    uint8_t col_gpio; /*!< Input GPIO selected by the wakeup column */
};

/** @brief Initialize the KYS V2 controller. */
void bflb_kys_v2_init(struct bflb_device_s *dev, const struct bflb_kys_v2_config_s *config);
/** @brief Start a key matrix scan. */
void bflb_kys_v2_enable(struct bflb_device_s *dev);
/** @brief Stop the current key matrix scan. */
void bflb_kys_v2_disable(struct bflb_device_s *dev);
/** @brief Enable or disable KYS interrupt sources. */
void bflb_kys_v2_int_enable(struct bflb_device_s *dev, uint32_t flag, bool enable);
/** @brief Clear clearable KYS interrupt sources. */
void bflb_kys_v2_int_clear(struct bflb_device_s *dev, uint32_t flag);
/** @brief Get enabled KYS interrupt status. */
uint32_t bflb_kys_v2_get_int_status(struct bflb_device_s *dev);
/** @brief Get raw KYS interrupt status regardless of interrupt enable state. */
uint32_t bflb_kys_v2_get_raw_int_status(struct bflb_device_s *dev);
/** @brief Get the number of valid key codes in the FIFO. */
uint32_t bflb_kys_v2_get_fifo_count(struct bflb_device_s *dev);
/** @brief Pop one key code from the FIFO. */
uint8_t bflb_kys_v2_read_keyvalue(struct bflb_device_s *dev);
/**
 * @brief Capture the current status and drain all valid key codes from the FIFO.
 *
 * Call this after a terminal scan status is observed and before disabling or
 * clearing the controller.
 */
int bflb_kys_v2_get_result(struct bflb_device_s *dev, struct bflb_kys_v2_result_s *result);
/**
 * @brief Run one blocking key scan.
 *
 * This API requires exclusive polling-mode use of the controller. It returns
 * -EBUSY if any KYS interrupt source is enabled.
 *
 * @param dev KYS V2 device handle
 * @param result Scan result
 * @param timeout_ms Maximum wait time in milliseconds, or 0 to wait forever
 * @return 0 on success, -EBUSY when interrupt mode is active, or -ETIMEDOUT
 *         if the scan does not complete in time
 */
int bflb_kys_v2_trigger_poll(struct bflb_device_s *dev,
                             struct bflb_kys_v2_result_s *result,
                             uint32_t timeout_ms);
/**
 * @brief Configure KYS matrix pins and selectors.
 *
 * Matrix rows are driven through KEY_SCAN_DRIVE. Matrix columns are sampled
 * through KEY_SCAN_IN.
 */
void bflb_kys_v2_matrix_init(uint8_t row_count, uint8_t col_count,
                             const uint8_t row_pins[], const uint8_t col_pins[]);
/**
 * @brief Configure KYS matrix GPIO pad paths for PDS mode.
 *
 * Only KEY_SCAN_IN column GPIOs need PDS pull-up and input enable. The
 * KEY_SCAN_DRIVE row GPIOs remain under the PDS key detector output path.
 */
void bflb_kys_v2_pds_gpio_configure(uint8_t col_count, const uint8_t col_pins[]);
/** @brief Release the PDS/HBN pad control enabled for KYS input columns. */
void bflb_kys_v2_pds_gpio_deconfigure(uint8_t col_count, const uint8_t col_pins[]);
/** @brief Configure the PDS key detector from the KYS matrix configuration. */
void bflb_kys_v2_pds_configure(struct bflb_device_s *dev,
                               const struct bflb_kys_v2_config_s *config);
/** @brief Enable or disable the PDS key detector. */
void bflb_kys_v2_pds_enable(struct bflb_device_s *dev, bool enable);
/** @brief Clear the PDS key detector wakeup status. */
void bflb_kys_v2_pds_clear_wakeup(struct bflb_device_s *dev);
/** @brief Read the PDS key detector wakeup source and captured key position. */
int bflb_kys_v2_pds_get_wakeup_result(struct bflb_device_s *dev,
                                      struct bflb_kys_v2_pds_wakeup_s *result);

#ifdef __cplusplus
}
#endif

#endif
