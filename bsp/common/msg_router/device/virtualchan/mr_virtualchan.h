/**
 * @file mr_virtualchan.h
 * @brief Virtual channel device header definitions
 * @details This file contains virtual channel device structures, state machine
 *          definitions, and packet format specifications for device-side
 *          virtual channel communication with message controller integration.
 */

#ifndef __MR_VIRTUALCHAN_H_
#define __MR_VIRTUALCHAN_H_

#include "bflb_core.h"
#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/** @defgroup MR_VIRTUALCHAN_HOST_STATES Virtual Channel Host State Machine
 * @brief State definitions for virtual channel host state machine
 * @{
 */
#define MR_VIRTUALCHAN_HSTA_ERROR      0 /**< Error state */
#define MR_VIRTUALCHAN_HSTA_RESET      1 /**< Host reset state, needs to send HOST_READY packet */
#define MR_VIRTUALCHAN_HSTA_HOST_READY 2 /**< Host ready state, waiting for DEVICE_START */
#define MR_VIRTUALCHAN_HSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup MR_VIRTUALCHAN_DEVICE_STATES Virtual Channel Device State Machine
 * @brief State definitions for virtual channel device state machine
 * @{
 */
#define MR_VIRTUALCHAN_DSTA_ERROR      0 /**< Error state */
#define MR_VIRTUALCHAN_DSTA_IDLE       1 /**< Device waiting for host ready */
#define MR_VIRTUALCHAN_DSTA_HOST_READY 2 /**< Host ready, send DEVICE_START after initialization */
#define MR_VIRTUALCHAN_DSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup MR_VIRTUALCHAN_FLAGS Virtual Channel Packet Flags
 * @brief Unified command definitions for virtual channel packets
 * @{
 */
enum {
    MR_VIRTUALCHAN_FLAG_NONE, /**< No specific flag */

    MR_VIRTUALCHAN_FLAG_DNLD_DATA, /**< Host to device data packet */
    MR_VIRTUALCHAN_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control) */

    MR_VIRTUALCHAN_FLAG_HOST_RESET, /**< Host reset command */
    MR_VIRTUALCHAN_FLAG_HOST_READY, /**< Host ready command */
    MR_VIRTUALCHAN_FLAG_HOST_STOP,  /**< Host stop command */

    MR_VIRTUALCHAN_FLAG_DEVICE_RESET, /**< Device reset command */
    MR_VIRTUALCHAN_FLAG_DEVICE_START, /**< Device start command (with initial credit_limit) */
    MR_VIRTUALCHAN_FLAG_DEVICE_STOP,  /**< Device stop command */

    MR_VIRTUALCHAN_FLAG_CREDIT_UPDATE, /**< Flow control packet */
};
/** @} */

/**
 * @brief Convert virtual channel message packet pointer to frame element pointer
 * @param ptr Pointer to mr_virtualchan_msg_t
 * @return Pointer to mr_frame_elem_t
 */
#define MR_VIRTUALCHAN_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr) ((mr_frame_elem_t *)((uint8_t *)(ptr) - FRAME_BUFF_HEADER_ROOM))

/**
 * @brief Convert frame element pointer to virtual channel message packet pointer
 * @param ptr Pointer to mr_frame_elem_t
 * @return Pointer to mr_virtualchan_msg_t
 */
#define MR_VIRTUALCHAN_FRAME_ELEM_TO_MSG_PACKET_ADDR(ptr) ((mr_virtualchan_msg_t *)((uint8_t *)(ptr) + FRAME_BUFF_HEADER_ROOM))

/**
 * @brief Get data payload size from virtual channel message packet
 * @param ptr Pointer to mr_virtualchan_msg_t
 * @return Data payload size in bytes
 */
#define MR_VIRTUALCHAN_MSG_PACKET_GET_DATA_SIZE(ptr)      (ptr->msg_pkt.len - (sizeof(mr_virtualchan_msg_t) - sizeof(mr_msg_t)))

/**
 * @brief Set data payload size for virtual channel message packet
 * @param ptr Pointer to mr_virtualchan_msg_t
 * @param size Data payload size in bytes
 */
#define MR_VIRTUALCHAN_MSG_PACKET_SET_DATA_SIZE(ptr, size)                                                \
    do {                                                                                                    \
        ptr->msg_pkt.len = (sizeof(mr_virtualchan_msg_t) - sizeof(mr_msg_t)) + size;                       \
        MR_VIRTUALCHAN_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr)->data_size = sizeof(mr_virtualchan_msg_t) + size; \
    } while (0)

/**
 * @brief Get available buffer size for data payload
 * @param ptr Pointer to mr_virtualchan_msg_t
 * @return Available buffer size in bytes
 */
#define MR_VIRTUALCHAN_MSG_PACKET_GET_BUFF_SIZE(ptr) \
    (MR_VIRTUALCHAN_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr)->buff_size - sizeof(mr_virtualchan_msg_t))

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

#pragma pack(push, 1)
/**
 * @brief Virtual channel message packet header with flow control
 * @details Header structure for virtual channel message packets sent over transport
 *          Similar to mr_netdev_msg_t and mr_tty_msg_t for virtual channel communication
 */
typedef struct {
    struct mr_msg_pkt msg_pkt; /**< Base message packet header */

    uint8_t reserved[1]; /**< Alignment padding */
    uint8_t flag;        /**< Packet flag (see MR_VIRTUALCHAN_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length data */
} mr_virtualchan_msg_t;
#pragma pack(pop)

/* Forward declarations */
typedef struct mr_virtualchan_priv_s mr_virtualchan_priv_t;
typedef struct mr_virtualchan_cfg_s mr_virtualchan_cfg_t;

/**
 * @brief Virtual channel configuration structure
 * @details Configuration parameters for virtual channel device initialization
 */
struct mr_virtualchan_cfg_s {
    char *name; /**< Virtual channel device name for identification */

    mr_msg_ctrl_priv_t *msg_ctrl; /**< Message control interface */

    uint8_t msg_tag; /**< Message tag for routing */

    /* Upload (device to host) buffer configuration */
    uint16_t upld_frame_size; /**< Frame buffer size in bytes */
    uint16_t upld_frame_count; /**< Number of frame buffers */
    void *upld_frame_buff;     /**< Frame buffer address (must be non-cacheable, total size = upld_frame_size * upld_frame_count) */
    uint16_t upld_frame_type;  /**< Frame buffer type identifier (for debugging) */

    /* Flow control configuration */
    uint8_t dnld_credit_max;              /**< Download credit limit (also initial value sent to host during handshake, 0 = no flow control) */
    uint8_t upld_credit_update_threshold; /**< Threshold for proactive credit update (when current credit - last update > threshold) */

    /* Callback functions */
    int (*dnld_output_cb)(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt); /**< Download data receive callback (must call mr_virtualchan_dnld_elem_free after use) */
    int (*upld_done_cb)(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt);   /**< Upload complete callback (optional, auto-freed if not registered) */
    int (*virtualchan_event)(mr_virtualchan_priv_t *priv, uint32_t event_mask);                   /**< Event notification callback (optional) */
    int (*virtualchan_reset_cb)(mr_virtualchan_priv_t *priv);                                     /**< Reset callback (optional) */

    int32_t task_stack_size; /**< Virtual channel processing task stack size */
    int32_t task_priority;   /**< Virtual channel processing task priority */

    uint32_t task_period_max_ms;                                                     /**< Max cycle period for processing task */
    int (*virtualchan_task_cb)(mr_virtualchan_priv_t *priv, uint32_t *notified_value); /**< Virtual channel task callback */
};

/**
 * @brief Virtual channel private data structure
 * @details Private data structure for device-side virtual channel implementation
 */
struct mr_virtualchan_priv_s {
    mr_virtualchan_cfg_t virtualchan_cfg; /**< Virtual channel configuration */

    mr_frame_queue_ctrl_t *upld_frame_ctrl; /**< Upload frame buffer controller */

    int virtualchan_status; /**< Current virtual channel device status */

    uint8_t device_dnld_credit_limit; /**< Device download flow control limit */
    uint8_t credit_limit_update_last; /**< Last credit limit update value */

    TaskHandle_t virtualchan_proc_task; /**< Virtual channel processing task handle */
    QueueHandle_t msg_dnld_queue;       /**< Message download processing queue */
    bool stop_requested;                /**< Thread exit flag */

    bool virtualchan_ready; /**< Virtual channel ready status */
};

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize virtual channel device instance
 * @param[in] cfg Pointer to virtual channel configuration structure
 * @retval Pointer to virtual channel private structure on success
 * @retval NULL on failure
 */
mr_virtualchan_priv_t *mr_virtualchan_init(mr_virtualchan_cfg_t *cfg);

/**
 * @brief Allocate upload message packet buffer from pool
 * @param[in] priv Pointer to virtual channel private structure
 * @param[out] virtualchan_msg_pkt Pointer to store allocated message packet
 * @param[in] timeout Timeout in ticks to wait for buffer
 * @retval 0 Success
 * @retval <0 No buffer available
 */
int mr_virtualchan_upld_elem_alloc(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t **virtualchan_msg_pkt, uint32_t timeout);

/**
 * @brief Send allocated upload message packet
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] virtualchan_msg_pkt Message packet (must be allocated by mr_virtualchan_upld_elem_alloc)
 * @retval 0 Success
 * @retval <0 Error
 * @note Automatically attaches flow control credit if configured
 */
int mr_virtualchan_upld_elem_send(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt);

/**
 * @brief Free upload message packet buffer back to pool
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] virtualchan_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Equivalent to mr_frame_queue_free_elem, must be called in upld_done_cb
 */
int mr_virtualchan_upld_elem_free(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt);

/**
 * @brief Free download message packet and update flow control
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] virtualchan_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Must be called after processing download data, automatically updates credit counter
 */
int mr_virtualchan_dnld_elem_free(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt);

/**
 * @brief Send virtual channel upload data
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] data_buff Pointer to data buffer to send
 * @param[in] data_size Size of data to send
 * @retval 0 Success
 * @retval <0 Error
 */
int mr_virtualchan_upld_data_send(mr_virtualchan_priv_t *priv, const uint8_t *data_buff, uint16_t data_size);

#endif /* __MR_VIRTUALCHAN_H_ */
