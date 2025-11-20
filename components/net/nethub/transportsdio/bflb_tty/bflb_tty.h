/**
 * @file bflb_tty.h
 * @brief Bouffalo Lab TTY device header definitions
 * @author mlwang
 * @date 2025-08-08
 * @details This file contains TTY device structures, state machine
 *          definitions, and packet format specifications for device-side TTY
 *          communication with message controller integration.
 */

#ifndef __BL_TTY_H_
#define __BL_TTY_H_

#include "bflb_core.h"
#include "bflb_msg_ctrl.h"
#include "bflb_frame_buff_ctrl.h"

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/* 私有内存池配置，内存块数影响到 tty 接收与 msg 发送 */
#define TTY_FRAME_CNT                    (4)
#define TTY_FRAME_SIZE                   (512 + FRAME_BUFF_PAYLOAD_OFFSET)// call api

/* 为 msg 命令包保留的内存块, 其他空闲内存块放入 tty rx 中 */
#define TTY_CMD_FRAME_CNT                (2)// 1

/* 下行流控限制值, 也是初始流控限制值, 会消耗 msg 的 dnld pool, 为 0 时无流控限制 */
#define TTY_DNLD_CREDIT_MAX              (6)// 1

/* 流控汇报触发阈值, 当前流控值与上次更新值的差大于此值时, 主动发送流控更新 */
#define TTY_DNLD_CREDIT_UPDATE_THRESHOLD (2)// less then TTY_DNLD_CREDIT_MAX

/** 接收来自 msg 数据的队列深度 */
#define TTY_MSG_QUEUE_DEPTH              (16)

/** @defgroup BL_TTY_HOST_STATES TTY Host State Machine
 * @brief State definitions for TTY host state machine
 * @{
 */
#define BL_TTY_HSTA_ERROR                0 /**< Error state */
#define BL_TTY_HSTA_RESET                1 /**< Host reset state, needs to send HOST_READY packet */
#define BL_TTY_HSTA_HOST_READY           2 /**< Host ready state, waiting for DEVICE_READY */
#define BL_TTY_HSTA_DEVICE_RUN           3 /**< Normal data transfer state */
/** @} */

/** @defgroup BL_TTY_DEVICE_STATES TTY Device State Machine
 * @brief State definitions for TTY device state machine
 * @{
 */
#define BL_TTY_DSTA_ERROR                0 /**< Error state */
#define BL_TTY_DSTA_IDLE                 1 /**< Device waiting for host ready */
#define BL_TTY_DSTA_HOST_READY           2 /**< Host ready, send DEVICE_READY after initialization */
#define BL_TTY_DSTA_DEVICE_RUN           3 /**< Normal data transfer state */
/** @} */

/** @defgroup BL_TTY_FLAGS TTY Packet Flags
 * @brief Unified command definitions for TTY packets
 * @{
 */
enum {
    BL_TTY_FLAG_NONE, /**< No specific flag */

    BL_TTY_FLAG_DNLD_DATA, /**< Host to device data packet */
    BL_TTY_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control) */

    BL_TTY_FLAG_HOST_RESET, /**< Host reset command */
    BL_TTY_FLAG_HOST_READY, /**< Host ready command */
    BL_TTY_FLAG_HOST_STOP,  /**< Host stop command */

    BL_TTY_FLAG_DEVICE_RESET, /**< Device reset command */
    BL_TTY_FLAG_DEVICE_START, /**< Device start command (with initial credit_limit) */
    BL_TTY_FLAG_DEVICE_STOP,  /**< Device stop command */

    BL_TTY_FLAG_CREDIT_UPDATE, /**< Flow control packet */

    BL_TTY_FLAG_OPEN,        /**< Port open command */
    BL_TTY_FLAG_CLOSE,       /**< Port close command */
    BL_TTY_FLAG_SET_TERMIOS, /**< Set terminal attributes */
    BL_TTY_FLAG_BREAK,       /**< Send break signal */
    BL_TTY_FLAG_HANGUP,      /**< Hangup signal */
};
/** @} */

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

#pragma pack(push, 1)

/**
 * @struct bflb_tty_msg_packt
 * @brief TTY message packet header with flow control
 * @details Header structure for TTY message packets sent over transport
 *          Similar to bflb_eth_msg_packt but for TTY communication
 */
struct bflb_tty_msg_packt {
    struct bflb_msg_packt msg_packt; /**< Base message packet header */

    uint8_t reseved[1]; /**< Alignment padding */
    uint8_t flag;       /**< Packet flag (see BL_TTY_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length data */
};
#pragma pack(pop)


/**
 * @struct bflb_tty_priv
 * @brief TTY private data structure
 * @details Private data structure for device-side TTY implementation
 */

typedef int (*bflb_tty_callback_t)(void *ctx, uint8_t *data, uint32_t data_size);
struct bflb_tty_priv {
    bflb_msg_ctrl_t *msg_ctrl; /**< Message control interface */
    uint8_t msg_tag;         /**< Message tag for routing */
    bool reset_flag;         /**< Reset flag for msg_ctrl */

    int tty_status; /**< Current TTY device status */

    uint8_t device_dnld_credit_limit;    /**< Device download credit limit */
    uint8_t credit_limit_update_last;    /**< Last credit limit update value */
    int credit_cnt;                      /**< Download credit counter */
    uint8_t host_upld_credit_limit;      /**< Host upload credit limit */
    uint8_t device_upld_credit_consumed; /**< Device upload credit consumed */

    TaskHandle_t tty_proc_task;        /**< TTY processing task handle */
    QueueHandle_t msg_dnld_queue;      /**< Message download processing queue */
    frame_queue_ctrl_t *rx_frame_ctrl; /**< RX frame buffer controller */
    bool stop_requested;               /**< Thread exit flag */

    bool tty_ready; /**< TTY ready status */

    bflb_tty_callback_t dnld_cb;
    void *cbpri_arg;
};

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize TTY device
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval 0 Success
 * @retval <0 Error code
 */
int bflb_tty_init(bflb_msg_ctrl_t *msg_ctrl, uint8_t msg_tag);

int bflb_tty_upld_send(uint8_t *data_buff, uint32_t data_size); // bflb send
int bflb_tty_dnld_register(bflb_tty_callback_t dnld_cb, void *cbpri_arg);        // bflb recv cb

#endif /* __BL_TTY_H_ */
