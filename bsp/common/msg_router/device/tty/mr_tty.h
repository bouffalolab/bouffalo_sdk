/**
 * @file mr_tty.h
 * @brief Bouffalo Lab TTY device header definitions
 * @details This file contains TTY device structures, state machine
 *          definitions, and packet format specifications for device-side TTY
 *          communication with message controller integration.
 */

#ifndef __MR_TTY_H_
#define __MR_TTY_H_

#include "bflb_core.h"
#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/** @defgroup MR_TTY_HOST_STATES TTY Host State Machine
 * @brief State definitions for TTY host state machine
 * @{
 */
#define MR_TTY_HSTA_ERROR      0 /**< Error state */
#define MR_TTY_HSTA_RESET      1 /**< Host reset state, needs to send HOST_READY packet */
#define MR_TTY_HSTA_HOST_READY 2 /**< Host ready state, waiting for DEVICE_READY */
#define MR_TTY_HSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup MR_TTY_DEVICE_STATES TTY Device State Machine
 * @brief State definitions for TTY device state machine
 * @{
 */
#define MR_TTY_DSTA_ERROR      0 /**< Error state */
#define MR_TTY_DSTA_IDLE       1 /**< Device waiting for host ready */
#define MR_TTY_DSTA_HOST_READY 2 /**< Host ready, send DEVICE_READY after initialization */
#define MR_TTY_DSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup MR_TTY_FLAGS TTY Packet Flags
 * @brief Unified command definitions for TTY packets
 * @{
 */
enum {
    MR_TTY_FLAG_NONE, /**< No specific flag */

    MR_TTY_FLAG_DNLD_DATA, /**< Host to device data packet */
    MR_TTY_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control) */

    MR_TTY_FLAG_HOST_RESET, /**< Host reset command */
    MR_TTY_FLAG_HOST_READY, /**< Host ready command */
    MR_TTY_FLAG_HOST_STOP,  /**< Host stop command */

    MR_TTY_FLAG_DEVICE_RESET, /**< Device reset command */
    MR_TTY_FLAG_DEVICE_START, /**< Device start command (with initial credit_limit) */
    MR_TTY_FLAG_DEVICE_STOP,  /**< Device stop command */

    MR_TTY_FLAG_CREDIT_UPDATE, /**< Flow control packet */

    MR_TTY_FLAG_OPEN,        /**< Port open command */
    MR_TTY_FLAG_CLOSE,       /**< Port close command */
    MR_TTY_FLAG_SET_TERMIOS, /**< Set terminal attributes */
    MR_TTY_FLAG_BREAK,       /**< Send break signal */
    MR_TTY_FLAG_HANGUP,      /**< Hangup signal */
};
/** @} */

/**
 * @brief Convert TTY message packet pointer to frame element pointer
 * @param ptr Pointer to mr_tty_msg_t
 * @return Pointer to mr_frame_elem_t
 */
#define MR_TTY_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr) ((mr_frame_elem_t *)((uint8_t *)(ptr) - FRAME_BUFF_HEADER_ROOM))

/**
 * @brief Convert frame element pointer to TTY message packet pointer
 * @param ptr Pointer to mr_frame_elem_t
 * @return Pointer to mr_tty_msg_t
 */
#define MR_TTY_FRAME_ELEM_TO_MSG_PACKET_ADDR(ptr) ((mr_tty_msg_t *)((uint8_t *)(ptr) + FRAME_BUFF_HEADER_ROOM))

/**
 * @brief Get data payload size from TTY message packet
 * @param ptr Pointer to mr_tty_msg_t
 * @return Data payload size in bytes
 */
#define MR_TTY_MSG_PACKET_GET_DATA_SIZE(ptr)      (ptr->msg_packt.len - (sizeof(mr_tty_msg_t) - sizeof(mr_msg_t)))

/**
 * @brief Set data payload size for TTY message packet
 * @param ptr Pointer to mr_tty_msg_t
 * @param size Data payload size in bytes
 */
#define MR_TTY_MSG_PACKET_SET_DATA_SIZE(ptr, size)                                          \
    do {                                                                                    \
        ptr->msg_packt.len = (sizeof(mr_tty_msg_t) - sizeof(mr_msg_t)) + size;              \
        MR_TTY_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr)->data_size = sizeof(mr_tty_msg_t) + size; \
    } while (0)

/**
 * @brief Get available buffer size for data payload
 * @param ptr Pointer to mr_tty_msg_t
 * @return Available buffer size in bytes
 */
#define MR_TTY_MSG_PACKET_GET_BUFF_SIZE(ptr) \
    (MR_TTY_MSG_PACKET_TO_FRAME_ELEM_ADDR(ptr)->buff_size - sizeof(mr_tty_msg_t))

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

#pragma pack(push, 1)
/**
 * @mr_tty_msg_t
 * @brief TTY message packet header with flow control
 * @details Header structure for TTY message packets sent over transport
 *          Similar to mr_eth_msg_packt but for TTY communication
 */
typedef struct {
    struct mr_msg_packt msg_packt; /**< Base message packet header */

    uint8_t reseved[1]; /**< Alignment padding */
    uint8_t flag;       /**< Packet flag (see MR_TTY_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length data */
} mr_tty_msg_t;
#pragma pack(pop)

/* Forward declarations */
typedef struct mr_tty_priv_s mr_tty_priv_t;
typedef struct mr_tty_cfg_s mr_tty_cfg_t;

/**
 * @brief TTY configuration structure
 * @details Configuration parameters for TTY device initialization
 */
struct mr_tty_cfg_s {
    char *name; /**< TTY device name for identification */

    mr_msg_ctrl_priv_t *msg_ctrl; /**< Message control interface */

    uint8_t msg_tag; /**< Message tag for routing */

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
        mr_tty_priv_t *priv,
        mr_tty_msg_t *tty_msg_packt); /**< Download data receive callback (must call mr_tty_dnld_elem_free after use) */
    int (*upld_done_cb)(
        mr_tty_priv_t *priv,
        mr_tty_msg_t *tty_msg_packt); /**< Upload complete callback (optional, auto-freed if not registered) */
    int (*tty_event)(mr_tty_priv_t *priv,
                     uint32_t event_mask);    /**< Event notification callback (optional, e.g., OPEN/CLOSE events) */
    int (*tty_reset_cb)(mr_tty_priv_t *priv); /**< Reset callback (optional) */

    int32_t task_stack_size; /**< TTY processing task stack size */
    int32_t task_priority;   /**< TTY processing task priority */

    uint32_t task_period_max_ms;                                       /**< Max cycle period for processing task */
    int (*tty_task_cb)(mr_tty_priv_t *priv, uint32_t *notified_value); /**< TTY task callback */
};

/**
 * @mr_tty_priv_t
 * @brief TTY private data structure
 * @details Private data structure for device-side TTY implementation
 */
struct mr_tty_priv_s {
    mr_tty_cfg_t tty_cfg; /**< TTY configuration */

    mr_frame_queue_ctrl_t *upld_frame_ctrl; /**< upld frame buffer controller */

    int tty_status; /**< Current TTY device status */

    uint8_t device_dnld_credit_limit; /**< Device download credit limit */
    uint8_t credit_limit_update_last; /**< Last credit limit update value */
    // uint8_t host_upld_credit_limit;      /**< Host upload credit limit */
    // uint8_t device_upld_credit_consumed; /**< Device upload credit consumed */

    TaskHandle_t tty_proc_task;   /**< TTY processing task handle */
    QueueHandle_t msg_dnld_queue; /**< Message download processing queue */
    bool stop_requested;          /**< Thread exit flag */

    bool tty_ready; /**< TTY ready status */
};

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize TTY device
 * @param[in] cfg Pointer to TTY configuration structure
 * @retval Pointer to TTY private structure on success
 * @retval NULL on failure
 */
mr_tty_priv_t *mr_tty_init(mr_tty_cfg_t *cfg);

/**
 * @brief Allocate upload message packet buffer
 * @param[in] priv Pointer to TTY private structure
 * @param[out] tty_msg_packt Pointer to store allocated message packet
 * @param[in] timeout Timeout in ticks to wait for allocation
 * @retval 0 Success
 * @retval <0 Error (no buffer available)
 */
int mr_tty_upld_elem_alloc(mr_tty_priv_t *priv, mr_tty_msg_t **tty_msg_packt, uint32_t timeout);

/**
 * @brief Send upload message packet
 * @param[in] priv Pointer to TTY private structure
 * @param[in] tty_msg_packt Pointer to message packet (must be allocated by mr_tty_upld_elem_alloc)
 * @retval 0 Success
 * @retval <0 Error
 */
int mr_tty_upld_elem_send(mr_tty_priv_t *priv, mr_tty_msg_t *tty_msg_packt);

/**
 * @brief Free upload message packet buffer
 * @param[in] priv Pointer to TTY private structure
 * @param[in] tty_msg_packt Pointer to message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Should be called in upld_done_cb or after send failure
 */
int mr_tty_upld_elem_free(mr_tty_priv_t *priv, mr_tty_msg_t *tty_msg_packt);

/**
 * @brief Send TTY upload data (convenient wrapper)
 * @param[in] priv Pointer to TTY private structure
 * @param[in] data_buff Pointer to data buffer
 * @param[in] data_size Size of data in bytes
 * @retval 0 Success
 * @retval <0 Error
 * @note This function handles alloc/send/free internally
 */
int mr_tty_upld_data_send(mr_tty_priv_t *priv, const uint8_t *data_buff, uint16_t data_size);

/**
 * @brief Free download message packet buffer and update flow control
 * @param[in] priv Pointer to TTY private structure
 * @param[in] tty_msg_packt Pointer to message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Must be called after processing download data in dnld_output_cb
 */
int mr_tty_dnld_elem_free(mr_tty_priv_t *priv, mr_tty_msg_t *tty_msg_packt);

#endif /* __MR_TTY_H_ */
