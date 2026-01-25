/**
 * @file mr_netlink.h
 * @brief Bouffalo Lab Netlink Device header definitions
 * @details This file contains Netlink device structures, state machine
 *          definitions, and packet format specifications for device-side Netlink
 *          communication with message controller integration.
 */

#ifndef __MR_NETLINK_H_
#define __MR_NETLINK_H_

#include "bflb_core.h"
#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/** @defgroup MR_NETLINK_HOST_STATES Netlink Host State Machine
 * @brief State definitions for Netlink host state machine
 * @{
 */
#define MR_NETLINK_HSTA_ERROR      0 /**< Error state */
#define MR_NETLINK_HSTA_RESET      1 /**< Host reset state, needs to send HOST_READY packet */
#define MR_NETLINK_HSTA_HOST_READY 2 /**< Host ready state, waiting for DEVICE_START */
#define MR_NETLINK_HSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup MR_NETLINK_DEVICE_STATES Netlink Device State Machine
 * @brief State definitions for Netlink device state machine
 * @{
 */
#define MR_NETLINK_DSTA_ERROR      0 /**< Error state */
#define MR_NETLINK_DSTA_IDLE       1 /**< Device waiting for host ready */
#define MR_NETLINK_DSTA_HOST_READY 2 /**< Host ready, send DEVICE_START after initialization */
#define MR_NETLINK_DSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup MR_NETLINK_FLAGS Netlink Packet Flags
 * @brief Unified command definitions for Netlink packets
 * @{
 */
enum {
    MR_NETLINK_FLAG_NONE, /**< No specific flag */

    MR_NETLINK_FLAG_DNLD_DATA, /**< Host to device data packet */
    MR_NETLINK_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control) */

    MR_NETLINK_FLAG_HOST_RESET, /**< Host reset command */
    MR_NETLINK_FLAG_HOST_READY, /**< Host ready command */
    MR_NETLINK_FLAG_HOST_STOP,  /**< Host stop command */

    MR_NETLINK_FLAG_DEVICE_RESET, /**< Device reset command */
    MR_NETLINK_FLAG_DEVICE_START, /**< Device start command (with initial credit_limit) */
    MR_NETLINK_FLAG_DEVICE_STOP,  /**< Device stop command */

    MR_NETLINK_FLAG_CREDIT_UPDATE, /**< Flow control packet */
};
/** @} */

/**
 * @brief Convert Netlink message packet pointer to frame element pointer
 * @param ptr Pointer to mr_netlink_msg_t
 * @return Pointer to mr_frame_elem_t
 */
#define MR_NETLINK_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr) ((mr_frame_elem_t *)((uint8_t *)(ptr) - FRAME_BUFF_HEADER_ROOM))

/**
 * @brief Convert frame element pointer to Netlink message packet pointer
 * @param ptr Pointer to mr_frame_elem_t
 * @return Pointer to mr_netlink_msg_t
 */
#define MR_NETLINK_FRAME_ELEM_TO_MSG_PACKET_ADDR(ptr) ((mr_netlink_msg_t *)((uint8_t *)(ptr) + FRAME_BUFF_HEADER_ROOM))

/**
 * @brief Get data payload size from Netlink message packet
 * @param ptr Pointer to mr_netlink_msg_t
 * @return Data payload size in bytes
 */
#define MR_NETLINK_MSG_PACKET_GET_DATA_SIZE(ptr)      (ptr->msg_pkt.len - (sizeof(mr_netlink_msg_t) - sizeof(mr_msg_t)))

/**
 * @brief Set data payload size for Netlink message packet
 * @param ptr Pointer to mr_netlink_msg_t
 * @param size Data payload size in bytes
 */
#define MR_NETLINK_MSG_PACKET_SET_DATA_SIZE(ptr, size)                                              \
    do {                                                                                            \
        ptr->msg_pkt.len = (sizeof(mr_netlink_msg_t) - sizeof(mr_msg_t)) + size;                    \
        MR_NETLINK_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr)->data_size = sizeof(mr_netlink_msg_t) + size; \
    } while (0)

/**
 * @brief Get available buffer size for data payload
 * @param ptr Pointer to mr_netlink_msg_t
 * @return Available buffer size in bytes
 */
#define MR_NETLINK_MSG_PACKET_GET_BUFF_SIZE(ptr) \
    (MR_NETLINK_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr)->buff_size - sizeof(mr_netlink_msg_t))

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

#pragma pack(push, 1)
/**
 * @brief Netlink message packet header with flow control
 * @details Header structure for Netlink message packets sent over transport
 *          Similar to mr_netdev_msg_t and mr_tty_msg_t but for Netlink communication
 */
typedef struct {
    struct mr_msg_pkt msg_pkt; /**< Base message packet header */

    uint8_t reserved[1]; /**< Alignment padding */
    uint8_t flag;        /**< Packet flag (see MR_NETLINK_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length data */
} mr_netlink_msg_t;
#pragma pack(pop)

/* Forward declarations */
typedef struct mr_netlink_priv_s mr_netlink_priv_t;
typedef struct mr_netlink_cfg_s mr_netlink_cfg_t;

/**
 * @brief Netlink configuration structure
 * @details Configuration parameters for Netlink device initialization
 */
struct mr_netlink_cfg_s {
    char *name; /**< Netlink device name for identification */

    mr_msg_ctrl_priv_t *msg_ctrl; /**< Message control interface */

    uint8_t msg_tag; /**< Message tag for routing */

    /* Upload (device to host) buffer configuration */
    uint16_t upld_frame_size;  /**< Frame buffer size in bytes */
    uint16_t upld_frame_count; /**< Number of frame buffers */
    void *upld_frame_buff;     /**< Frame buffer address (must be non-cacheable, total size = upld_frame_size * upld_frame_count) */
    uint16_t upld_frame_type;  /**< Frame buffer type identifier (for debugging) */

    /* Flow control configuration */
    uint8_t dnld_credit_max;              /**< Download credit limit (also initial value sent to host during handshake, 0 = no flow control) */
    uint8_t upld_credit_update_threshold; /**< Threshold for proactive credit update (when current credit - last update > threshold) */

    /* Callback functions */
    int (*dnld_output_cb)(mr_netlink_priv_t *priv, mr_netlink_msg_t *netlink_msg_pkt); /**< Download data receive callback (must call mr_netlink_dnld_elem_free after use) */
    int (*upld_done_cb)(mr_netlink_priv_t *priv, mr_netlink_msg_t *netlink_msg_pkt);   /**< Upload complete callback (optional, auto-freed if not registered) */
    int (*netlink_event)(mr_netlink_priv_t *priv, uint32_t event_mask);                /**< Event notification callback (optional) */
    int (*netlink_reset_cb)(mr_netlink_priv_t *priv);                                  /**< Reset callback (optional) */

    int32_t task_stack_size; /**< Netlink processing task stack size */
    int32_t task_priority;   /**< Netlink processing task priority */

    uint32_t task_period_max_ms;                                               /**< Max cycle period for processing task */
    int (*netlink_task_cb)(mr_netlink_priv_t *priv, uint32_t *notified_value); /**< Netlink task callback */
};

/**
 * @brief Netlink private data structure
 * @details Private data structure for device-side Netlink implementation
 */
struct mr_netlink_priv_s {
    mr_netlink_cfg_t netlink_cfg; /**< Netlink configuration */

    mr_frame_queue_ctrl_t *upld_frame_ctrl; /**< upld frame buffer controller */

    int netlink_status; /**< Current Netlink device status */

    uint8_t device_dnld_credit_limit; /**< Device download credit limit */
    uint8_t credit_limit_update_last; /**< Last credit limit update value */

    TaskHandle_t netlink_proc_task; /**< Netlink processing task handle */
    QueueHandle_t msg_dnld_queue;   /**< Message download processing queue */
    bool stop_requested;            /**< Thread exit flag */

    bool netlink_ready; /**< Netlink ready status */
};

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize Netlink device instance
 * @param[in] cfg Pointer to Netlink configuration structure
 * @retval Pointer to Netlink private structure on success
 * @retval NULL on failure
 */
mr_netlink_priv_t *mr_netlink_init(mr_netlink_cfg_t *cfg);

/**
 * @brief Allocate upload message packet buffer from pool
 * @param[in] priv Pointer to Netlink private structure
 * @param[out] netlink_msg_pkt Pointer to store allocated message packet
 * @param[in] timeout Timeout in ticks to wait for buffer
 * @retval 0 Success
 * @retval <0 No buffer available
 */
int mr_netlink_upld_elem_alloc(mr_netlink_priv_t *priv, mr_netlink_msg_t **netlink_msg_pkt, uint32_t timeout);

/**
 * @brief Send allocated upload message packet
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] netlink_msg_pkt Message packet (must be allocated by mr_netlink_upld_elem_alloc)
 * @retval 0 Success
 * @retval <0 Error
 * @note Automatically attaches flow control credit if configured
 */
int mr_netlink_upld_elem_send(mr_netlink_priv_t *priv, mr_netlink_msg_t *netlink_msg_pkt);

/**
 * @brief Free upload message packet buffer back to pool
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] netlink_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Equivalent to mr_frame_queue_free_elem, must be called in upld_done_cb
 */
int mr_netlink_upld_elem_free(mr_netlink_priv_t *priv, mr_netlink_msg_t *netlink_msg_pkt);

/**
 * @brief Free download message packet and update flow control
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] netlink_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Must be called after processing download data, automatically updates credit counter
 */
int mr_netlink_dnld_elem_free(mr_netlink_priv_t *priv, mr_netlink_msg_t *netlink_msg_pkt);

#endif /* __MR_NETLINK_H_ */
