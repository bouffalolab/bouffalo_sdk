#ifndef __BFLB_DSI_H__
#define __BFLB_DSI_H__

#include <stdint.h>
#include "bflb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* MIPI-DSI data type codes (DI field) */
#define BFLB_DSI_DATA_YUV422               0x2c /* YUV422, 8-bit */
#define BFLB_DSI_DATA_RGB565               0x0e /* RGB565 */
#define BFLB_DSI_DATA_RGB666               0x2e /* RGB666 (loosely packed) */
#define BFLB_DSI_DATA_RGB888               0x3e /* RGB888 */

#define BFLB_DSI_LANES_1                   0
#define BFLB_DSI_LANES_2                   1
#define BFLB_DSI_LANES_4                   2

/* DSI lane order types */
#define BFLB_DSI_LANE_ORDER_3210           0 /* Lane 3, 2, 1, 0 */
#define BFLB_DSI_LANE_ORDER_2130           1 /* Lane 2, 1, 3, 0 */
#define BFLB_DSI_LANE_ORDER_1320           2 /* Lane 1, 3, 2, 0 */
#define BFLB_DSI_LANE_ORDER_3120           3 /* Lane 3, 1, 2, 0 */

/* HS sync mode: event=0, pulse=1 (matches HW) */
#define BFLB_DSI_HS_SYNC_EVENT_MODE        0 /* Alias for compatibility */
#define BFLB_DSI_HS_SYNC_PULSE_MODE        1 /* Alias for compatibility */

/* DSI lane type */
#define BFLB_DSI_LANE_DATA0                0x01
#define BFLB_DSI_LANE_DATA1                0x02
#define BFLB_DSI_LANE_DATA2                0x04
#define BFLB_DSI_LANE_DATA3                0x08
#define BFLB_DSI_LANE_CLOCK                0x10

/* DSI clock lane operation types */
#define BFLB_DSI_CLOCK_LANE_ULP_ENTER      0x01 /* Clock lane low power enter */
#define BFLB_DSI_CLOCK_LANE_ULP_EXIT       0x02 /* Clock lane low power exit */
#define BFLB_DSI_CLOCK_LANE_HS_REQ         0x04 /* Clock lane HS request */
#define BFLB_DSI_CLOCK_LANE_HS_EXIT        0x08 /* Clock lane HS exit */

/* DSI lane state types */
#define BFLB_DSI_LANE_STATE_NORMAL         0x00 /* Status normal */
#define BFLB_DSI_LANE_STATE_STOP           0x01 /* Status stop */
#define BFLB_DSI_LANE_STATE_BRIDGE         0x02 /* Status bridge changing */
#define BFLB_DSI_LANE_STATE_ULP            0x04 /* Status ultra low power */
#define BFLB_DSI_LANE_STATE_HS             0x08 /* Status high speed */

/* DSI escape command types */
#define BFLB_DSI_ESCAPE_TYPE_LPDT          0 /* Escape type: LPDT */
#define BFLB_DSI_ESCAPE_TYPE_TRIGGER       1 /* Escape type: TRIGGER */
#define BFLB_DSI_ESCAPE_TYPE_ULPS          2 /* Escape type: ULPS */

/* DSI FIFO error types */
#define BFLB_DSI_FIFO_ERR_TX_OVERFLOW      0x01 /* DSI TX FIFO overflow */
#define BFLB_DSI_FIFO_ERR_TX_UNDERFLOW     0x02 /* DSI TX FIFO underflow */
#define BFLB_DSI_FIFO_ERR_RX_OVERFLOW      0x04 /* DSI RX FIFO overflow */
#define BFLB_DSI_FIFO_ERR_RX_UNDERFLOW     0x08 /* DSI RX FIFO underflow */

/* DSI interrupt bit masks  */
#define BFLB_DSI_INT_ESCAPE_TX_END         0x0001u
#define BFLB_DSI_INT_LPDT_RX_END           0x0002u
#define BFLB_DSI_INT_ULPS_RX_END           0x0004u
#define BFLB_DSI_INT_TRIGGER0_RX_END       0x0008u
#define BFLB_DSI_INT_TRIGGER1_RX_END       0x0010u
#define BFLB_DSI_INT_TRIGGER2_RX_END       0x0020u
#define BFLB_DSI_INT_TRIGGER3_RX_END       0x0040u
#define BFLB_DSI_INT_TX_FIFO_READY         0x0080u
#define BFLB_DSI_INT_RX_FIFO_READY         0x0100u
#define BFLB_DSI_INT_DATA_OVERRUN_ERR      0x0200u
#define BFLB_DSI_INT_DATA_UNDERRUN_ERR     0x0400u
#define BFLB_DSI_INT_PIXEL_COUNT_SMALL_ERR 0x0800u
#define BFLB_DSI_INT_PIXEL_COUNT_LARGE_ERR 0x1000u
#define BFLB_DSI_INT_FIFO_ERROR            0x2000u
#define BFLB_DSI_INT_ALL                   0x3FFFu

typedef struct bflb_dsi_config_s {
    uint8_t data_type;    /**< MIPI-DSI data type (use BFLB_DSI_DATA_* macros) */
    uint8_t lane_num;     /**< DSI lane number configuration (use BFLB_DSI_LANES_* macros) */
    uint8_t lane_order;   /**< Lane multiplexing order (use BFLB_DSI_LANE_ORDER_* macros) */
    uint8_t sync_type;    /**< HS sync type: event or pulse mode (use BFLB_DSI_SYNC_* macros) */
    uint8_t virtual_chan; /**< Virtual channel ID (0-3) */
    uint8_t vfp;          /**< Vertical front porch in lines */
    uint8_t vsa;          /**< Vertical sync active in lines */
} bflb_dsi_config_t;

typedef struct bflb_dsi_dphy_config_s {
    uint8_t time_clk_zero;     /**< Clock lane HS-ZERO timing parameter */
    uint8_t time_clk_trail;    /**< Clock lane HS-TRAIL timing parameter */
    uint8_t time_clk_exit;     /**< Clock lane HS-EXIT timing parameter */
    uint8_t time_data_zero;    /**< Data lane HS-ZERO timing parameter */
    uint8_t time_data_trail;   /**< Data lane HS-TRAIL timing parameter */
    uint8_t time_data_exit;    /**< Data lane HS-EXIT timing parameter */
    uint8_t time_data_prepare; /**< Data lane HS-PREPARE timing parameter */
    uint8_t time_ta_go;        /**< Turnaround GO timing parameter */
    uint8_t time_ta_get;       /**< Turnaround GET timing parameter */
    uint8_t time_req_ready;    /**< Request to ready timing parameter */
    uint8_t time_lpx;          /**< Low-power transmission timing parameter */
    uint32_t time_wakeup;      /**< Wake-up timing parameter (in clock cycles) */
} bflb_dsi_dphy_config_t;

typedef struct bflb_dsi_lpdt_msg_s {
    uint8_t data_type;     /**< DCS command or generic packet data type */
    uint8_t virtual_chan;  /**< Virtual channel ID (0-3) */
    const uint8_t *tx_buf; /**< Pointer to transmit data buffer */
    uint16_t tx_len;       /**< Transmit data length: payload bytes for long packets, 0/1/2 for short packets */
    uint8_t *rx_buf;       /**< Pointer to receive data buffer */
    uint16_t rx_len;       /**< Receive buffer size (input) / actual received length (output) */
} bflb_dsi_lpdt_msg_t;

/** FIFO configuration structure */
typedef struct {
    uint8_t tx_fifo_dma_threshold; /**< TX FIFO DMA threshold */
    uint8_t rx_fifo_dma_threshold; /**< RX FIFO DMA threshold */
    bool tx_fifo_dma_enable;       /**< Enable TX FIFO DMA */
    bool rx_fifo_dma_enable;       /**< Enable RX FIFO DMA */
} bflb_dsi_fifo_config_t;

/**
 * @brief Initialize DSI controller with configuration
 *
 * @param[in] dev DSI device handle
 * @param[in] cfg DSI configuration structure containing lane settings, sync type, data type, etc.
 */
void bflb_dsi_init(struct bflb_device_s *dev, const bflb_dsi_config_t *cfg);

/**
 * @brief Deinitialize DSI controller
 *
 * @param[in] dev DSI device handle
 */
void bflb_dsi_deinit(struct bflb_device_s *dev);

/* PHY control */
/**
 * @brief Reset DSI PHY
 *
 * @param[in] dev DSI device handle
 */
void bflb_dsi_phy_reset(struct bflb_device_s *dev);

/* PHY control */
/**
 * @brief Enable DSI PHY
 *
 * @param[in] dev DSI device handle
 */
void bflb_dsi_phy_enable(struct bflb_device_s *dev);

/**
 * @brief Configure DSI PHY timing parameters
 *
 * @param[in] dev DSI device handle
 * @param[in] phy PHY timing configuration structure
 */
void bflb_dsi_phy_config(struct bflb_device_s *dev, const bflb_dsi_dphy_config_t *phy);

/**
 * @brief Enable specified DSI lanes
 *
 * @param[in] dev DSI device handle
 * @param[in] lanes_mask Bitmask of lanes to enable (use BFLB_DSI_LANE_* macros)
 */
void bflb_dsi_phy_enable_lanes(struct bflb_device_s *dev, uint32_t lanes_mask);

/**
 * @brief Disable specified DSI lanes
 *
 * @param[in] dev DSI device handle
 * @param[in] lanes_mask Bitmask of lanes to disable (use BFLB_DSI_LANE_* macros)
 */
void bflb_dsi_phy_disable_lanes(struct bflb_device_s *dev, uint32_t lanes_mask);

/**
 * @brief Control DSI clock lane operations
 *
 * @param[in] dev DSI device handle
 * @param[in] operations Clock lane operations (use BFLB_DSI_CLOCK_LANE_* macros)
 *
 * @return 0 on success, negative error code on failure
 */
int bflb_dsi_phy_set_clock_lane(struct bflb_device_s *dev, uint32_t operations);

/**
 * @brief Get DSI lane state
 *
 * @param[in] dev DSI device handle
 * @param[in] lane Lane to check (use BFLB_DSI_LANE_* macros)
 * @param[out] state Pointer to store lane state (BFLB_DSI_LANE_STATE_* values)
 */
void bflb_dsi_phy_get_lanes_state(struct bflb_device_s *dev, uint32_t lane, uint32_t *state);

/**
 * @brief Perform data lane 0 turnaround
 *
 * @param[in] dev DSI device handle
 */
void bflb_dsi_phy_data_lane0_turnaround(struct bflb_device_s *dev);

/**
 * @brief Start high speed mode transmission
 *
 * @param[in] dev DSI device handle
 */
void bflb_dsi_phy_hs_mode_start(struct bflb_device_s *dev);

/**
 * @brief Stop high speed mode transmission
 *
 * @param[in] dev DSI device handle
 *
 * @return 0 on success, negative error code on timeout
 */
int bflb_dsi_phy_hs_mode_stop(struct bflb_device_s *dev);

/* ESC/LPDT packet send/receive */
/**
 * @brief Send LPDT short packet (0-2 bytes payload)
 *
 * @param[in] dev DSI device handle
 * @param[in] msg LPDT message structure with data_type, virtual_chan, and optional payload
 *
 * @return 0 on success, negative error code on failure
 */
int bflb_dsi_lpdt_send_short_packet(struct bflb_device_s *dev, const bflb_dsi_lpdt_msg_t *msg);

/**
 * @brief Send LPDT long packet (variable length payload)
 *
 * @param[in] dev DSI device handle  
 * @param[in] msg LPDT message structure with data_type, virtual_chan, tx_buf, and tx_len
 *
 * @return 0 on success, negative error code on failure
 */
int bflb_dsi_lpdt_send_long_packet(struct bflb_device_s *dev, const bflb_dsi_lpdt_msg_t *msg);

/**
 * @brief Receive LPDT packet
 *
 * @param[in] dev DSI device handle
 * @param[in,out] msg LPDT message structure with rx_buf and rx_len for received data
 *
 * @return 0 on success, negative error code on failure
 */
int bflb_dsi_lpdt_recv(struct bflb_device_s *dev, bflb_dsi_lpdt_msg_t *msg);

/* Lower-level API functions (following std driver pattern) */
/**
 * @brief Configure short packet for LPDT transmission
 *
 * @param[in] dev DSI device handle
 * @param[in] msg LPDT message structure with data_type, virtual_chan, and optional payload
 *
 * @return 0 on success, negative error code on failure
 */
int bflb_dsi_lpdt_config_short_packet(struct bflb_device_s *dev, const bflb_dsi_lpdt_msg_t *msg);

/**
 * @brief Configure long packet for LPDT transmission
 *
 * @param[in] dev DSI device handle
 * @param[in] msg LPDT message structure with payload data
 *
 * @return 0 on success, negative error code on failure
 */
int bflb_dsi_lpdt_config_long_packet(struct bflb_device_s *dev, const bflb_dsi_lpdt_msg_t *msg);

/**
 * @brief Start LPDT transmission
 *
 * @param[in] dev DSI device handle
 *
 * @return 0 on success, negative error code on failure
 */
int bflb_dsi_lpdt_start_tx(struct bflb_device_s *dev);

/**
 * @brief Start LPDT reception
 *
 * @param[in] dev DSI device handle
 *
 * @return 0 on success, negative error code on failure
 */
int bflb_dsi_lpdt_start_rx(struct bflb_device_s *dev);

/**
 * @brief Wait for LPDT transmission to complete
 *
 * @param[in] dev DSI device handle
 *
 * @return 0 on success, negative error code on timeout
 */
int bflb_dsi_wait_tx_done(struct bflb_device_s *dev);

/**
 * @brief Get received LPDT packet length
 *
 * @param[in] dev DSI device handle
 *
 * @return Length of received packet in bytes
 */
uint16_t bflb_dsi_lpdt_get_rx_length(struct bflb_device_s *dev);

/**
 * @brief Read data from RX FIFO
 *
 * @param[in] dev DSI device handle
 * @param[out] data Buffer to store read data
 * @param[in] maxlen Maximum number of bytes to read
 *
 * @return Number of bytes actually read, or negative error code
 */
int bflb_dsi_read_fifo(struct bflb_device_s *dev, uint8_t *data, uint16_t maxlen);

/**
 * @brief Fill TX FIFO completely with data
 *
 * @param[in] dev DSI device handle
 * @param[in] data Data buffer to write
 * @param[in] len Number of bytes to write
 *
 * @return Number of bytes actually written
 */
uint8_t bflb_dsi_full_fill_fifo(struct bflb_device_s *dev, const uint8_t *data, uint16_t len);

/**
 * @brief Feed data to TX FIFO (partial fill allowed)
 *
 * @param[in] dev DSI device handle
 * @param[in] data Data buffer to write
 * @param[in] len Number of bytes to write
 *
 * @return Number of bytes actually written
 */
int bflb_dsi_feed_fifo(struct bflb_device_s *dev, const uint8_t *data, uint16_t len);

/* FIFO access functions */
/**
 * @brief Get current TX FIFO count
 *
 * @param[in] dev DSI device handle
 *
 * @return Current number of bytes in TX FIFO
 */
uint8_t bflb_dsi_get_tx_fifo_count(struct bflb_device_s *dev);

/**
 * @brief Get current RX FIFO count
 *
 * @param[in] dev DSI device handle
 *
 * @return Current number of bytes in RX FIFO
 */
uint8_t bflb_dsi_get_rx_fifo_count(struct bflb_device_s *dev);

/* Interrupt related (expand as needed) */
/**
 * @brief Mask or unmask DSI interrupts
 *
 * @param[in] dev DSI device handle
 * @param[in] int_mask Interrupt mask (use BFLB_DSI_INT_* macros)
 * @param[in] mask true to mask (disable), false to unmask (enable)
 */
void bflb_dsi_int_mask(struct bflb_device_s *dev, uint32_t int_mask, bool mask);

/**
 * @brief Clear DSI interrupt flags
 *
 * @param[in] dev DSI device handle
 * @param[in] int_flag Interrupt flags to clear (use BFLB_DSI_INT_* macros)
 */
void bflb_dsi_int_clear(struct bflb_device_s *dev, uint32_t int_flag);

/**
 * @brief Enable DSI interrupts
 *
 * @param[in] dev DSI device handle
 * @param[in] int_type Interrupt types to enable (use BFLB_DSI_INT_* macros)
 */
void bflb_dsi_int_enable(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Get DSI interrupt status
 *
 * @param[in] dev DSI device handle
 *
 * @return Current interrupt status flags (BFLB_DSI_INT_* values)
 */
uint32_t bflb_dsi_int_get(struct bflb_device_s *dev);

/**
 * @brief Send DSI trigger command
 *
 * @param[in] dev DSI device handle
 * @param[in] cmd Trigger command value
 *
 * @return 0 on success, negative error code otherwise
 */
int bflb_dsi_send_trigger_command(struct bflb_device_s *dev, uint8_t cmd);

/**
 * @brief Send DSI ULPS command
 *
 * @param[in] dev DSI device handle
 *
 * @return 0 on success, negative error code otherwise
 */
int bflb_dsi_send_ulps_command(struct bflb_device_s *dev);

/**
 * @brief Exit DSI ULPS mode
 *
 * @param[in] dev DSI device handle
 *
 * @return 0 on success, negative error code otherwise
 */
int bflb_dsi_exit_ulps(struct bflb_device_s *dev);

/**
 * @brief Configure DSI FIFO settings
 *
 * @param[in] dev DSI device handle
 * @param[in] fifo_cfg FIFO configuration
 *
 * @return 0 on success, negative error code otherwise
 */
int bflb_dsi_fifo_config(struct bflb_device_s *dev, const bflb_dsi_fifo_config_t *fifo_cfg);

/**
 * @brief Set DSI line buffer threshold
 *
 * @param[in] dev DSI device handle
 * @param[in] frame_width Frame width in pixels (must be ≤1280 and multiple of 4)
 * @param[in] dvp_tsrc_clock DVP TSRC clock frequency in Hz
 * @param[in] dsi_clock DSI clock frequency in Hz
 * @param[in] data_type Data type (use BFLB_DSI_DATA_* macros)
 * @param[in] lane_number Number of DSI data lanes (use BFLB_DSI_LANES_* macros)
 */
void bflb_dsi_set_line_buffer_threshold(struct bflb_device_s *dev, uint32_t frame_width,
                                        uint32_t dvp_tsrc_clock, uint32_t dsi_clock,
                                        uint32_t data_type, uint32_t lane_number);

/**
 * @brief Set DSI VSA and VFP timing
 *
 * @param[in] dev DSI device handle
 * @param[in] vsa Vertical sync active period in lines
 * @param[in] vfp Vertical front porch period in lines
 */
void bflb_dsi_set_vsa_vfp(struct bflb_device_s *dev, uint8_t vsa, uint8_t vfp);

/**
 * @brief Reinitialize DSI data lanes (force RX mode)
 *
 * @param[in] dev DSI device handle
 * @param[in] lanes_mask Bitmask of data lanes to reinitialize (use BFLB_DSI_LANE_DATA* macros)
 */
void bflb_dsi_phy_reinit_data_lanes(struct bflb_device_s *dev, uint32_t lanes_mask);

/**
 * @brief Stop DSI data lanes (force TX stop mode)
 *
 * @param[in] dev DSI device handle
 * @param[in] lanes_mask Bitmask of data lanes to stop (use BFLB_DSI_LANE_DATA* macros)
 */
void bflb_dsi_phy_stop_data_lanes(struct bflb_device_s *dev, uint32_t lanes_mask);

#ifdef __cplusplus
}
#endif

#endif /* __BFLB_DSI_H__ */
