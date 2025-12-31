/**
 * @file mr_netdev.h
 * @brief Bouffalo Lab Network Device header definitions
 * @details This file contains network device structures, state machine
 *          definitions, and packet format specifications for device-side Ethernet
 *          communication with message controller integration.
 */

#ifndef __MR_NETDEV_H_
#define __MR_NETDEV_H_

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/** @defgroup MR_NETDEV_HOST_STATES Ethernet Host State Machine
 * @brief State definitions for Ethernet host state machine
 * @{
 */
#define MR_NETDEV_HSTA_ERROR      0 /**< Error state */
#define MR_NETDEV_HSTA_RESET      1 /**< Host reset state, needs to send HOST_READY packet */
#define MR_NETDEV_HSTA_HOST_READY 2 /**< Host ready state, waiting for DEVICE_READY */
#define MR_NETDEV_HSTA_DEVICE_RUN 4 /**< Normal data transfer state */
/** @} */

/** @defgroup MR_NETDEV_DEVICE_STATES Ethernet Device State Machine
 * @brief State definitions for Ethernet device state machine
 * @{
 */
#define MR_NETDEV_DSTA_ERROR      0 /**< Error state */
#define MR_NETDEV_DSTA_IDLE       1 /**< Device waiting for host ready */
#define MR_NETDEV_DSTA_HOST_READY 2 /**< Host ready, send DEVICE_READY after initialization */
#define MR_NETDEV_DSTA_DEVICE_RUN 4 /**< Normal data transfer state */
/** @} */

/** @defgroup MR_NETDEV_FLAGS Ethernet Packet Flags
 * @brief Flag definitions for Ethernet packet types
 * @{
 */
enum {
    MR_NETDEV_FLAG_NONE, /**< No specific flag */

    MR_NETDEV_FLAG_DNLD_DATA, /**< Host to device data packet */
    MR_NETDEV_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control) */

    MR_NETDEV_FLAG_HOST_RESET, /**< Host reset (error recovery), device enters DSTA_IDLE */
    MR_NETDEV_FLAG_HOST_READY, /**< Host ready, device enters DSTA_HOST_READY */
    MR_NETDEV_FLAG_HOST_STOP,  /**< Host stop, device enters DSTA_IDLE */

    MR_NETDEV_FLAG_DEVICE_RESET, /**< Device reset, host enters HSTA_HOST_READY */
    MR_NETDEV_FLAG_DEVICE_START, /**< Device start (must carry initial flow control), host enters HSTA_DEVICE_RUN */
    MR_NETDEV_FLAG_DEVICE_STOP,  /**< Device stop, host enters HSTA_RESET */

    MR_NETDEV_FLAG_CREDIT_UPDATE, /**< Standalone flow control update when no DNLD_DATA */
    MR_NETDEV_FLAG_MAC_IP_UPDATE, /**< MAC and IP address update packet */

    MR_NETDEV_FLAG_MAX, /**< Maximum flag value */
};
/** @} */

/**
 * @brief Convert network device message packet pointer to frame element pointer
 * @param ptr Pointer to mr_netdev_msg_t
 * @return Pointer to mr_frame_elem_t
 */
#define MR_NETDEV_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr) ((mr_frame_elem_t *)((uintptr_t)(ptr) - FRAME_BUFF_HEADER_ROOM))

/**
 * @brief Convert frame element pointer to network device message packet pointer
 * @param ptr Pointer to mr_frame_elem_t
 * @return Pointer to mr_netdev_msg_t
 */
#define MR_NETDEV_FRAME_ELEM_TO_MSG_PACKET_ADDR(ptr) ((mr_netdev_msg_t *)((uintptr_t)(ptr) + FRAME_BUFF_HEADER_ROOM))

/**
 * @brief Get data size from network device message packet
 * @param ptr Pointer to mr_netdev_msg_t
 * @return Data size in bytes
 */
#define MR_NETDEV_MSG_PACKET_GET_DATA_SIZE(ptr)      (ptr->msg_packt.len - (sizeof(mr_netdev_msg_t) - sizeof(mr_msg_t)))

/**
 * @brief Set data size for network device message packet
 * @param ptr Pointer to mr_netdev_msg_t
 * @param size Data size to set
 */
#define MR_NETDEV_MSG_PACKET_SET_DATA_SIZE(ptr, size)                                             \
    do {                                                                                          \
        ptr->msg_packt.len = (sizeof(mr_netdev_msg_t) - sizeof(mr_msg_t)) + size;                 \
        MR_NETDEV_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr)->data_size = sizeof(mr_netdev_msg_t) + size; \
    } while (0)

/**
 * @brief Get buffer size from network device message packet
 * @param ptr Pointer to mr_netdev_msg_t
 * @return Buffer size in bytes
 */
#define MR_NETDEV_MSG_PACKET_GET_BUFF_SIZE(ptr) \
    (MR_NETDEV_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr)->buff_size - sizeof(mr_netdev_msg_t))

/**
 * @brief Get available upload buffer count
 * @param priv Pointer to mr_netdev_priv_t
 * @return Number of available buffers
 */
#define MR_NETDEV_UPLD_BUFF_GET_CNT(priv) (mr_frame_queue_messages_waiting((priv)->upld_frame_ctrl->pool_queue))

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

#pragma pack(push, 1)

/**
 * @struct mr_netdev_msg_t
 * @brief Ethernet message packet structure
 * @details Custom Ethernet packet format with message header, flags, and flow control
 */
typedef struct {
    struct mr_msg_packt msg_packt; /**< Message packet header */

    uint8_t reseved[1]; /**< Alignment padding */
    uint8_t flag;       /**< Packet flag (see MR_NETDEV_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length Ethernet packet data */
} mr_netdev_msg_t;

#pragma pack(pop)

/* Forward declarations */
typedef struct mr_netdev_priv_s mr_netdev_priv_t;
typedef struct mr_netdev_cfg_s mr_netdev_cfg_t;

/**
 * @brief NETDEV configuration structure
 * @details Configuration parameters for NETDEV device initialization
 */
struct mr_netdev_cfg_s {
    char *name;

    mr_msg_ctrl_priv_t *msg_ctrl;

    uint8_t msg_tag;

    /* Upload (device to host) buffer configuration */
    uint16_t upld_frame_size;  /**< Frame buffer size in bytes */
    uint16_t upld_frame_count; /**< Number of frame buffers */
    void *upld_frame_buff; /**< Frame buffer address (must be non-cacheable, total size = upld_frame_size * upld_frame_count) */
    uint16_t upld_frame_type; /**< Frame buffer type identifier (for debugging) */

    /* Flow control configuration */
    uint8_t
        dnld_credit_max; /**< Download credit limit (also initial value sent to host during handshake, 0 = no flow control) */
    uint8_t
        upld_credit_update_threshold; /**< Threshold for proactive credit update (when current credit - last update > threshold) */

    /* Callback functions */
    int (*dnld_output_cb)(
        mr_netdev_priv_t *priv,
        mr_netdev_msg_t
            *netdev_msg_packt); /**< Download data receive callback (must call mr_tty_dnld_elem_free after use) */
    int (*upld_done_cb)(
        mr_netdev_priv_t *priv,
        mr_netdev_msg_t *netdev_msg_packt); /**< Upload complete callback (optional, auto-freed if not registered) */
    int (*netdev_event)(mr_netdev_priv_t *priv,
                        uint32_t event_mask); /**< Event notification callback (optional, e.g., OPEN/CLOSE events) */
    int (*netdev_reset_cb)(mr_netdev_priv_t *priv); /**< Reset callback (optional) */

    int32_t task_stack_size; /**< NETDEV processing task stack size */
    int32_t task_priority;   /**< NETDEV processing task priority */

    uint32_t task_period_max_ms; /**< Max cycle period for processing task */
    int (*netdev_task_cb)(mr_netdev_priv_t *priv, uint32_t *notified_value); /**< NETDEV task callback */

    bool link_up_flag; /**< link up flag */
    uint8_t mac[6];    /**< MAC address */
    uint8_t ip[4];     /**< IP address */
};

/**
 * @struct mr_netdev_priv_s
 * @brief Network device private data structure
 * @details Private data structure for device-side network device implementation
 *          containing state machine, flow control counters, and frame buffers
 */
struct mr_netdev_priv_s {
    mr_netdev_cfg_t netdev_cfg; /**< NETDEV configuration */

    mr_frame_queue_ctrl_t *upld_frame_ctrl; /**< upld frame buffer controller */

    int netdev_status; /**< Current Ethernet device status */

    uint8_t device_dnld_credit_limit; /**< Device download credit limit (circular counter, wraps at 256) */
    uint8_t credit_limit_update_last; /**< Last credit limit update value */
    // uint8_t host_upld_credit_limit;      /**< Host upload credit limit */
    // uint8_t device_upld_credit_consumed; /**< Device upload credit consumed */

    TaskHandle_t netdev_proc_task; /**< NETDEV processing task handle */
    QueueHandle_t msg_dnld_queue;  /**< Message download processing queue */
    bool stop_requested;           /**< Thread exit flag */

    bool netdev_ready; /**< NETDEV ready status */
};

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize network device instance
 * @param[in] cfg Pointer to network device configuration structure
 * @return Pointer to initialized private structure on success, NULL on failure
 */
mr_netdev_priv_t *mr_netdev_init(mr_netdev_cfg_t *cfg);

/**
 * @brief Allocate an upload packet buffer
 * @param[in] priv Pointer to network device private structure
 * @param[out] netdev_msg_packt Pointer to store allocated packet address
 * @param[in] timeout Timeout in milliseconds (0 = no wait, portMAX_DELAY = infinite)
 * @retval 0 Success
 * @retval <0 Error (no available buffers)
 */
int mr_netdev_upld_elem_alloc(mr_netdev_priv_t *priv, mr_netdev_msg_t **netdev_msg_packt, uint32_t timeout);

/**
 * @brief Send upload packet to host
 * @param[in] priv Pointer to network device private structure
 * @param[in] netdev_msg_packt Pointer to packet to send
 * @retval 0 Success (ownership transferred to msg_ctrl)
 * @retval <0 Error (caller must free packet)
 */
int mr_netdev_upld_elem_send(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt);

/**
 * @brief Free upload packet buffer
 * @param[in] priv Pointer to network device private structure
 * @param[in] netdev_msg_packt Pointer to packet to free
 * @retval 0 Success
 * @retval <0 Error
 */
int mr_netdev_upld_elem_free(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt);

/**
 * @brief Free download packet buffer and update flow control
 * @param[in] priv Pointer to network device private structure
 * @param[in] netdev_msg_packt Pointer to packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note This function updates the download credit counter and may trigger
 *       proactive credit update if threshold is reached
 */
int mr_netdev_dnld_elem_free(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt);

/**
 * @brief Update MAC and/or IP address
 * @param[in] priv Pointer to network device private structure
 * @param[in] mac Pointer to new MAC address (6 bytes), NULL to skip
 * @param[in] ip Pointer to new IP address (4 bytes), NULL to skip
 * @retval 0 Success
 * @retval <0 Error
 * @note This function triggers an asynchronous MAC/IP update message to host
 */
int mr_netdev_mac_ip_update(mr_netdev_priv_t *priv, uint8_t *mac, uint8_t *ip);

/**
 * @brief Set link up/down state
 * @param[in] priv Pointer to network device private structure
 * @param[in] link_up true for link up, false for link down
 * @retval 0 Success
 * @retval <0 Error
 * @note This function triggers state machine transitions:
 *       - link_up: May trigger DEVICE_START if in HOST_READY state
 *       - link_down: Triggers DEVICE_STOP and returns to IDLE state
 */
int mr_netdev_set_link_up(mr_netdev_priv_t *priv, bool link_up);

#endif
