/**
 * @file hd_tty.h
 * @brief Host Driver TTY Interface Header with Dynamic Device Management
 * @author mlwang
 * @date 2025-08-07
 * @details This module implements a virtual TTY serial device that operates
 *          over SDIO transport using the msg_ctrl framework. It provides
 *          standard Linux TTY interface functionality with dynamic device
 *          registration, state machine management, and flow control similar
 *          to the eth_netdev implementation.
 */

#ifndef __HD_TTY_H__
#define __HD_TTY_H__

#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/skbuff.h>

#include "hd_msg_ctrl.h"

/** @defgroup HD_TTY_CONFIG TTY Configuration Constants
 * @{
 */
#define HD_TTY_DRIVER_NAME     "hd_tty"               /**< TTY driver name */
#define HD_TTY_DEVICE_NAME     "ttyHD"                /**< TTY device node name */
#define HD_TTY_MAJOR           0                      /**< Major number (0 = dynamic allocation) */
#define HD_TTY_TYPE            TTY_DRIVER_TYPE_SERIAL /**< TTY driver type */
#define HD_TTY_SUBTYPE         SERIAL_TYPE_NORMAL     /**< TTY driver subtype */
#define HD_TTY_FLAGS           (TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV)

/** TTY buffer sizes */
#define HD_TTY_MAX_PACKET_SIZE (512 * 3)                     /**< Maximum packet size over transport */
#define HD_TTY_MAX_DATA_LEN    (HD_TTY_MAX_PACKET_SIZE - 16) /**< Maximum data length per packet */
/** @} */

/** @defgroup HD_TTY_HOST_STATES TTY Host State Machine
 * @brief State definitions for TTY host state machine
 * @{
 */
#define HD_TTY_HSTA_ERROR      0 /**< Error state */
#define HD_TTY_HSTA_RESET      1 /**< Host reset state, needs to send HOST_READY packet */
#define HD_TTY_HSTA_HOST_READY 2 /**< Host ready state, waiting for DEVICE_START */
#define HD_TTY_HSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup HD_TTY_DEVICE_STATES TTY Device State Machine
 * @brief State definitions for TTY device state machine
 * @{
 */
#define HD_TTY_DSTA_ERROR      0 /**< Error state */
#define HD_TTY_DSTA_IDLE       1 /**< Device waiting for host ready */
#define HD_TTY_DSTA_HOST_READY 2 /**< Host ready, send DEVICE_READY after initialization */
#define HD_TTY_DSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup HD_TTY_FLAGS TTY Packet Flags
 * @brief Unified command definitions for TTY packets
 * @{
 */
enum {
    HD_TTY_FLAG_NONE, /**< No specific flag */

    HD_TTY_FLAG_DNLD_DATA, /**< Host to device data packet */
    HD_TTY_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control) */

    HD_TTY_FLAG_HOST_RESET, /**< Host reset command */
    HD_TTY_FLAG_HOST_READY, /**< Host ready command */
    HD_TTY_FLAG_HOST_STOP,  /**< Host stop command */

    HD_TTY_FLAG_DEVICE_RESET, /**< Device reset command */
    HD_TTY_FLAG_DEVICE_START, /**< Device start command (with initial credit_limit) */
    HD_TTY_FLAG_DEVICE_STOP,  /**< Device stop command */

    HD_TTY_FLAG_CREDIT_UPDATE, /**< Flow control packet */

    HD_TTY_FLAG_OPEN,        /**< Port open command */
    HD_TTY_FLAG_CLOSE,       /**< Port close command */
    HD_TTY_FLAG_SET_TERMIOS, /**< Set terminal attributes */
    HD_TTY_FLAG_BREAK,       /**< Send break signal */
    HD_TTY_FLAG_HANGUP,      /**< Hangup signal */
};
/** @} */

#pragma pack(push, 1)

/**
 * @struct hd_tty_msg_packt
 * @brief TTY message packet header with flow control
 * @details Header structure for TTY message packets sent over transport
 *          Similar to hd_eth_msg_packt but for TTY communication
 */
struct hd_tty_msg_packt {
    struct hd_msg_packt msg_packt; /**< Base message packet header */

    uint8_t reseved[1]; /**< Alignment padding */
    uint8_t flag;       /**< Packet flag (see HD_TTY_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length data */
};

#pragma pack(pop)

/**
 * @struct hd_tty_priv
 * @brief TTY private data structure
 * @details Main structure managing TTY device with state machine and flow control
 */
struct hd_tty_priv {
    /** @name Driver Management
     * @{
     */
    struct tty_driver *tty_driver; /**< TTY driver structure */
    struct hd_msg_ctrl *msg_ctrl;  /**< Message control interface */
    uint8_t msg_tag;               /**< Message tag for routing */
    /** @} */

    /** @name TTY Port and Device
     * @{
     */
    struct tty_port port;      /**< Standard TTY port structure */
    struct device *tty_dev;    /**< TTY device (for dynamic registration) */
    bool is_device_registered; /**< Device registration status */
    bool is_open;              /**< Port open status */
    /** @} */

    /** @name State Machine
     * @{
     */
    int tty_status;                      /**< Current TTY status */
    struct task_struct *daemon_thread;   /**< Background daemon thread */
    struct sk_buff_head thread_skb_head; /**< Thread SKB queue */
    wait_queue_head_t waitq;             /**< Thread wait queue */
    bool thread_condition;               /**< Thread wakeup condition */
    /** @} */

    /** @name Flow Control
     * @{
     */
    uint8_t credit_consumed_cnt; /**< Credits consumed counter */
    uint8_t credit_limit_cnt;    /**< Credit limit counter */
    bool credit_nolimit;         /**< No credit limit flag */
    spinlock_t credit_lock;      /**< Credit management spinlock */
    /** @} */

    /** @name Statistics
     * @{
     */
    unsigned long tx_packets; /**< Transmitted packets */
    unsigned long rx_packets; /**< Received packets */
    unsigned long tx_bytes;   /**< Transmitted bytes */
    unsigned long rx_bytes;   /**< Received bytes */
    unsigned long tx_errors;  /**< Transmit errors */
    unsigned long rx_errors;  /**< Receive errors */
    spinlock_t stats_lock;    /**< Statistics lock */
    /** @} */

    /** @name Debug
     * @{
     */
    struct dentry *debugfs_root; /**< DebugFS root directory */
    /** @} */
};

/* Convenience macros for data length calculation */
/**
 * @brief Calculate data length from message packet length
 * @param[in] msg_packt_len Total length field from hd_msg_packt structure
 * @return Length of data payload (excluding TTY message header)
 */
#define HD_TTY_GET_DATA_LEN(msg_packt_len) \
    ((msg_packt_len) - (sizeof(struct hd_tty_msg_packt) - sizeof(struct hd_msg_packt)))

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize TTY driver with dynamic device management
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval Pointer to TTY private structure on success
 * @retval NULL on failure
 */
struct hd_tty_priv *hd_tty_init(struct hd_msg_ctrl *msg_ctrl, uint8_t msg_tag);

/**
 * @brief Deinitialize TTY driver
 * @param[in] tty_priv TTY private structure to cleanup
 */
void hd_tty_deinit(struct hd_tty_priv *tty_priv);

#endif /* __HD_TTY_H__ */
