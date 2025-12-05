#ifndef __transportsdio_H_
#define __transportsdio_H_

#include <nethub_types.h>

#include "bflb_core.h"
#include "bflb_msg_ctrl.h"
#include "bflb_frame_buff_ctrl.h"

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/** WiFi receive buffer size (MTU + headers) */
#define WIFI_RX_BUFF_SIZE                     (FRAME_BUFF_PAYLOAD)

/* Private memory pool configuration, memory block count affects WiFi reception and msg sending */
#define WIFI_FRAME_CNT                        (10 + 2)

/* Reserved memory blocks for msg command packets */
#define WIFI_CMD_FRAME_CNT                    (2)

/* Downlink flow control limit value */
#define WIFI_DNLD_CREDIT_MAX                  (22)//34

/* Flow control report trigger threshold */
#define WIFI_DNLD_CREDIT_UPDATE_THRESHOLD     (2)//(WIFI_DNLD_CREDIT_MAX / 4)

/** Queue depth for receiving data from msg */
#define WIFI_MSG_QUEUE_DEPTH                  (32)

/** @defgroup BL_WIFI_HOST_STATES WiFi Host State Machine
 * @brief State definitions for WiFi host state machine
 * @{
 */
#define BL_WIFI_HSTA_ERROR                     0 /**< Error state */
#define BL_WIFI_HSTA_RESET                     1 /**< Host reset state, needs to send HOST_READY packet */
#define BL_WIFI_HSTA_HOST_READY                2 /**< Host ready state, waiting for DEVICE_READY */
#define BL_WIFI_HSTA_DEVICE_READY              3 /**< Device ready state, waiting for DEVICE_START */
#define BL_WIFI_HSTA_DEVICE_RUN                4 /**< Normal data transfer state */
/** @} */

/** @defgroup BL_WIFI_DEVICE_STATES WiFi Device State Machine
 * @brief State definitions for WiFi device state machine
 * @{
 */
#define BL_WIFI_DSTA_ERROR                     0 /**< Error state */
#define BL_WIFI_DSTA_IDLE                      1 /**< Device waiting for host ready */
#define BL_WIFI_DSTA_HOST_READY                2 /**< Host ready, send DEVICE_READY after initialization */
#define BL_WIFI_DSTA_DEVICE_READY              3 /**< Device ready, waiting */
#define BL_WIFI_DSTA_DEVICE_RUN                4 /**< Normal data transfer state */
/** @} */

/** @defgroup BL_WIFI_FLAGS WiFi Packet Flags
 * @brief Flag definitions for WiFi packet types
 * @{
 */
enum {
    BL_WIFI_FLAG_NONE, /**< No specific flag */

    BL_WIFI_FLAG_DNLD_DATA, /**< Host to device data packet */
    BL_WIFI_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control) */

    BL_WIFI_FLAG_HOST_RESET, /**< Host reset (error recovery), device enters DSTA_IDLE */
    BL_WIFI_FLAG_HOST_READY, /**< Host ready, device enters DSTA_HOST_READY */
    BL_WIFI_FLAG_HOST_STOP,  /**< Host stop, device enters DSTA_IDLE */

    BL_WIFI_FLAG_DEVICE_RESET, /**< Device reset, host enters HSTA_HOST_READY */
    BL_WIFI_FLAG_DEVICE_READY, /**< Device ready (must carry MAC address), host enters HSTA_DEVICE_READY */
    BL_WIFI_FLAG_DEVICE_START, /**< Device start (must carry initial flow control), host enters HSTA_DEVICE_RUN */
    BL_WIFI_FLAG_DEVICE_STOP,  /**< Device stop, host enters HSTA_DEVICE_READY */

    BL_WIFI_FLAG_CREDIT_UPDATE, /**< Standalone flow control update when no DNLD_DATA */

    BL_WIFI_FLAG_MAX, /**< Maximum flag value */
};
/** @} */

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

#pragma pack(push, 1)

/**
 * @struct bflb_wifi_msg_packt
 * @brief WiFi message packet structure
 * @details Custom WiFi packet format with message header, flags, and flow control
 */
struct bflb_wifi_msg_packt {
    struct bflb_msg_packt msg_packt; /**< Message packet header */

    uint8_t reserved[1]; /**< Alignment padding */
    uint8_t flag;       /**< Packet flag (see BL_WIFI_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length WiFi packet data */
};

#pragma pack(pop)

/**
 * @struct wifi_wifista_priv
 * @brief WiFi device private data structure
 * @details Private data structure for device-side WiFi implementation
 */
typedef struct wifi_wifista_priv {
    bflb_msg_ctrl_t *msg_ctrl; /**< Message control interface */
    uint8_t msg_tag;         /**< Message tag for routing */

    int wifi_status; /**< Current WiFi device status */

    uint8_t device_dnld_credit_limit;    /**< Device download credit limit */
    uint8_t credit_limit_update_last;    /**< Last credit limit update value */
    int credit_cnt;                      /**< Download credit counter */
    uint8_t host_upld_credit_limit;      /**< Host upload credit limit */
    uint8_t device_upld_credit_consumed; /**< Device upload credit consumed */

    TaskHandle_t wifi_proc_task;       /**< WiFi processing task handle */
    TaskHandle_t frame_free_task;      /**< Frame free task handle */
    QueueHandle_t msg_dnld_queue;      /**< Message download processing queue */
    QueueHandle_t frame_free_queue;    /**< Frame free processing queue */
    frame_queue_ctrl_t *rx_frame_ctrl; /**< RX frame buffer controller */
    bool stop_requested;               /**< Thread exit flag */

    uint8_t mac_addr[6];               /**< Device MAC address */
    bool wifi_connected;               /**< WiFi connection status */

    /* WiFi interface reserved function pointers */
    int (*wifi_send_frame)(struct wifi_wifista_priv *priv,
                            void *data, uint32_t len,
                            void (*free_cb)(void *arg), void *cb_arg);
    int (*wifi_recv_frame)(struct wifi_wifista_priv *priv, void *data, uint32_t len);

    /* Flow control related */
    bool flow_control_enabled;           /**< Flow control enable flag */
    uint8_t flow_control_threshold;      /**< Flow control threshold */
} wifi_wifista_priv_t;

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize WiFi device
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_init(wifi_wifista_priv_t *ctx, bflb_msg_ctrl_t *msg_ctrl, uint8_t msg_tag);

/**
 * @brief Deinitialize WiFi device
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_deinit(void);

/**
 * @brief Start WiFi device
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_start(void);

/**
 * @brief Stop WiFi device
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_stop(void);

/**
 * @brief Get WiFi connection status
 * @retval true WiFi is connected
 * @retval false WiFi is disconnected
 */
bool transportsdio_link_status(void);

/**
 * @brief Set WiFi send frame callback function
 * @param[in] send_func WiFi send frame function pointer
 */
void transportsdio_set_wifi_send_cb(int (*send_func)(struct wifi_wifista_priv *, void *, uint32_t, void (*free_cb)(void *arg), void *cb_arg));

/**
 * @brief Set WiFi receive frame callback function
 * @param[in] recv_func WiFi receive frame function pointer
 */
void transportsdio_set_wifi_recv_cb(int (*recv_func)(struct wifi_wifista_priv *, void *, uint32_t));

/**
 * @brief WiFi connection establishment callback interface - actively called after successful WiFi connection
 * @details When WiFi connection is successful, call this interface to notify the device that WiFi is connected
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_linkup(void);

/**
 * @brief WiFi connection disconnection callback interface - actively called when WiFi disconnects
 * @details When WiFi connection is disconnected, call this interface to notify the device that WiFi is disconnected
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_linkdown(void);

/**
 * @brief Set flow control parameters
 * @param[in] enabled Whether to enable flow control
 * @param[in] threshold Flow control threshold
 */
void transportsdio_set_flow_control(bool enabled, uint8_t threshold);

/**
 * @brief Get current flow control status
 * @param[out] enabled Flow control enable status
 * @param[out] threshold Flow control threshold
 */
void transportsdio_get_flow_control(bool *enabled, uint8_t *threshold);

/**
 * @brief User call interface: send WiFi data to host (upld)
 * @param[in] data Data buffer to send
 * @param[in] len Data length
 * @retval 0 Success
 * @retval -1 Parameter error
 * @retval -2 Device not initialized
 * @retval -3 Memory allocation failed
 * @retval -4 WiFi not connected
 */
int transportsdio_send_upld(const void *data, uint32_t len);
int transportsdio_send_upld2(const uint8_t *data, uint32_t offset, uint32_t pld_len, skb_free_cb_t data_free, void *skb);
int transportsdio_send_upld_skb(nh_skb_t *skb);

/**
 * @brief User call interface: receive WiFi data (dnld)
 * @param[out] data Received data buffer
 * @param[in] max_len Maximum buffer length
 * @param[out] actual_len Actual received data length
 * @retval 0 Success
 * @retval -1 Parameter error
 * @retval -2 Device not initialized
 * @retval -3 No data available
 */
int transportsdio_recv_dnld(void *data, uint32_t max_len, uint32_t *actual_len);

/**
 * @brief Set MAC address
 * @param[in] mac_addr MAC address (6 bytes)
 * @retval 0 Success
 * @retval -1 Parameter error or device not initialized
 */
int transportsdio_set_mac_addr(const uint8_t mac_addr[6]);

/**
 * @brief Get MAC address
 * @param[out] mac_addr Output MAC address buffer (6 bytes)
 * @retval 0 Success
 * @retval -1 Parameter error or device not initialized
 */
int transportsdio_get_mac_addr(uint8_t mac_addr[6]);

/**
 * @brief Initialize payload offset whitelist
 */
void transportsdio_init_payload_offset(void);

/**
 * @brief Register payload address and corresponding offset to whitelist
 * @param[in] payload payload address
 * @param[in] offset corresponding offset value
 * @retval 0 Success
 * @retval -1 Parameter error
 * @retval -2 Whitelist full
 */
int transportsdio_register_payload_offset(uint8_t *payload, uint32_t offset);

/**
 * @brief Unregister payload address from whitelist
 * @param[in] payload payload address to remove
 * @retval 0 Success
 * @retval -1 Parameter error
 * @retval -2 Payload not found
 */
int transportsdio_unregister_payload_offset(uint8_t *payload);

/**
 * @brief Get corresponding offset by payload address
 * @param[in] payload payload address
 * @return offset value, return FRAME_BUFF_PAYLOAD_OFFSET if not found
 */
uint32_t transportsdio_get_payload_offset(uint8_t *payload);

/**
 * @brief Dump g_wifi_priv structure parameters for debugging
 * @details Print important parameters including queue status, connection state,
 *          flow control status and other key information
 */
void transportsdio_dump_priv(void);

#endif /* __transportsdio_H_ */
