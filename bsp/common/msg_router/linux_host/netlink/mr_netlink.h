/**
 * @file mr_netlink.h
 * @brief Netlink Interface Header for Kernel-Userspace Communication
 * @details This module implements a virtual Netlink socket interface that operates
 *          over SDIO transport using the msg_ctrl framework. It provides standard
 *          Linux Netlink interface functionality for bidirectional communication
 *          between kernel and userspace applications, with state machine management
 *          and flow control similar to the TTY implementation.
 */

#ifndef __MR_NETLINK_H__
#define __MR_NETLINK_H__

#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/kthread.h>
#include <net/sock.h>

#include "mr_msg_ctrl.h"

/** @defgroup MR_NETLINK_CONFIG Netlink Configuration Constants
 * @{
 */
#define MR_NETLINK_PROTOCOL        31 /**< Netlink protocol number (use unused protocol) */
#define MR_NETLINK_MULTICAST_GROUP 1  /**< Multicast group ID */

/** Netlink buffer sizes */
#define MR_NETLINK_MAX_PACKET_SIZE (512 * 3)                         /**< Maximum packet size over transport */
#define MR_NETLINK_MAX_DATA_LEN    (MR_NETLINK_MAX_PACKET_SIZE - 32) /**< Maximum data length per packet */
/** @} */

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

#pragma pack(push, 1)

/**
 * @struct mr_netlink_msg_pkt
 * @brief Netlink message packet header with flow control
 * @details Header structure for Netlink message packets sent over transport
 *          Similar to mr_tty_msg_pkt for bidirectional communication
 */
struct mr_netlink_msg_pkt {
    struct mr_msg_pkt msg_pkt; /**< Base message packet header */

    uint8_t reserved[1]; /**< Alignment padding */
    uint8_t flag;        /**< Packet flag (see MR_NETLINK_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length data */
};

#pragma pack(pop)

/**
 * @struct mr_netlink_priv
 * @brief Netlink private data structure
 * @details Main structure managing Netlink socket with state machine and flow control
 */
struct mr_netlink_priv {
    /** @name Driver Management
     * @{
     */
    struct sock *nl_sock;         /**< Netlink socket structure */
    struct mr_msg_ctrl *msg_ctrl; /**< Message control interface */
    uint8_t msg_tag;              /**< Message tag for routing */
    /** @} */

    /** @name State Machine
     * @{
     */
    int netlink_status;                    /**< Current Netlink status */
    struct task_struct *daemon_thread;     /**< Background daemon thread */
    struct sk_buff_head receive_skb_head;  /**< SKB queue for packets received from device */
    struct sk_buff_head transmit_skb_head; /**< SKB queue for packets to transmit to device (preserve order) */
    wait_queue_head_t waitq;               /**< Thread wait queue */
    bool thread_condition;                 /**< Thread wakeup condition */
    /** @} */

    /** @name Client Tracking (not used in multicast mode, kept for future compatibility)
     * @{
     */
    u32 client_portid; /**< Client port ID (not used in multicast mode) */
    /** @} */

    /** @name Flow Control
     * @{
     */
    uint8_t credit_consumed_cnt; /**< Credits consumed counter */
    uint8_t credit_limit_cnt;    /**< Credit limit counter */
    spinlock_t credit_lock;      /**< Credit management spinlock */
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
 * @param[in] msg_pkt_len Total length field from mr_msg_pkt structure
 * @return Length of data payload (excluding Netlink message header)
 */
#define MR_NETLINK_GET_DATA_LEN(msg_pkt_len) \
    ((msg_pkt_len) - (sizeof(struct mr_netlink_msg_pkt) - sizeof(struct mr_msg_pkt)))

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize Netlink interface
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval Pointer to Netlink private structure on success
 * @retval NULL on failure
 */
struct mr_netlink_priv *mr_netlink_init(struct mr_msg_ctrl *msg_ctrl, uint8_t msg_tag);

/**
 * @brief Deinitialize Netlink interface
 * @param[in] netlink_priv Netlink private structure to cleanup
 */
void mr_netlink_deinit(struct mr_netlink_priv *netlink_priv);

#endif /* __MR_NETLINK_H__ */
