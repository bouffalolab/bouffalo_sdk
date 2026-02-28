/**
 * @file mr_virtualchan.h
 * @brief Virtual channel kernel-userspace communication interface header file
 * @details This module implements SDIO transport virtual socket interface based on msg_ctrl framework.
 *          Provides standard Linux Netlink interface functionality, implementing kernel-userspace
 *          bidirectional communication with state machine management and flow control,
 *          similar to TTY implementation.
 */

#ifndef __MR_VIRTUALCHAN_H__
#define __MR_VIRTUALCHAN_H__

#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/kthread.h>
#include <net/sock.h>

#include "mr_msg_ctrl.h"

/** @defgroup MR_VIRTUALCHAN_CONFIG Virtual Channel Configuration Constants
 * @{
 */
#define MR_VIRTUALCHAN_PROTOCOL        31 /**< Netlink protocol number (using unused protocol) */
#define MR_VIRTUALCHAN_MULTICAST_GROUP 1  /**< Multicast group ID */

/** Virtual channel buffer size */
#define MR_VIRTUALCHAN_MAX_PACKET_SIZE (512 * 3)                          /**< Maximum packet size for transport layer */
#define MR_VIRTUALCHAN_MAX_DATA_LEN    (MR_VIRTUALCHAN_MAX_PACKET_SIZE - 32) /**< Maximum data length per packet */
/** @} */

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
#define MR_VIRTUALCHAN_DSTA_HOST_READY 2 /**< Host ready, send DEVICE_START for initialization */
#define MR_VIRTUALCHAN_DSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup MR_VIRTUALCHAN_FLAGS Virtual Channel Packet Flags
 * @brief Unified command definitions for virtual channel packets
 * @{
 */
enum {
    MR_VIRTUALCHAN_FLAG_NONE, /**< No specific flag */

    MR_VIRTUALCHAN_FLAG_DNLD_DATA, /**< Host to device data packet */
    MR_VIRTUALCHAN_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control info) */

    MR_VIRTUALCHAN_FLAG_HOST_RESET, /**< Host reset command */
    MR_VIRTUALCHAN_FLAG_HOST_READY, /**< Host ready command */
    MR_VIRTUALCHAN_FLAG_HOST_STOP,  /**< Host stop command */

    MR_VIRTUALCHAN_FLAG_DEVICE_RESET, /**< Device reset command */
    MR_VIRTUALCHAN_FLAG_DEVICE_START, /**< Device start command (carrying initial credit_limit) */
    MR_VIRTUALCHAN_FLAG_DEVICE_STOP,  /**< Device stop command */

    MR_VIRTUALCHAN_FLAG_CREDIT_UPDATE, /**< Flow control packet */
};
/** @} */

#pragma pack(push, 1)

/**
 * @struct mr_virtualchan_msg_pkt
 * @brief Virtual channel message packet header with flow control
 * @details Header structure for virtual channel message packets on transport layer
 *          Similar to mr_tty_msg_pkt, used for bidirectional communication
 */
struct mr_virtualchan_msg_pkt {
    struct mr_msg_pkt msg_pkt; /**< Base message packet header */

    uint8_t reserved[1]; /**< Alignment padding */
    uint8_t flag;        /**< Packet flag (see MR_VIRTUALCHAN_FLAG_*) */

    uint8_t credit_update_flag; /**< Flow control update flag */
    uint8_t credit_limit_cnt;   /**< Flow control limit counter */

    uint8_t data[0]; /**< Variable length data */
};

#pragma pack(pop)

/**
 * @struct mr_virtualchan_priv
 * @brief Virtual channel private data structure
 * @details Main structure managing virtual channel socket, state machine and flow control
 */
struct mr_virtualchan_priv {
    /** @name Driver Management
     * @{
     */
    struct sock *nl_sock;         /**< Netlink socket structure */
    struct mr_msg_ctrl *msg_ctrl; /**< Message control interface */
    uint8_t msg_tag;              /**< Message routing tag */
    /** @} */

    /** @name State Machine
     * @{
     */
    int virtualchan_status;               /**< Current virtual channel status */
    struct task_struct *daemon_thread;    /**< Background daemon thread */
    struct sk_buff_head receive_skb_head; /**< SKB queue for packets received from device */
    struct sk_buff_head transmit_skb_head; /**< SKB queue for packets to send to device (maintain order) */
    wait_queue_head_t waitq;              /**< Thread wait queue */
    bool thread_condition;                /**< Thread wakeup condition */
    /** @} */

    /** @name Client Tracking (unused in multicast mode, reserved for future compatibility)
     * @{
     */
    u32 client_portid; /**< Client port ID (unused in multicast mode) */
    /** @} */

    /** @name Flow Control
     * @{
     */
    uint8_t credit_consumed_cnt; /**< Consumed credit counter */
    uint8_t credit_limit_cnt;    /**< Credit limit counter */
    spinlock_t credit_lock;      /**< Credit management spinlock */
    /** @} */

    /** @name Debugging
     * @{
     */
    struct dentry *debugfs_root; /**< DebugFS root directory */
    /** @} */
};

/* Data length calculation convenience macros */
/**
 * @brief Calculate data length from message packet length
 * @param[in] msg_pkt_len Total length field of mr_msg_pkt structure
 * @return Data payload length (excluding virtual channel message header)
 */
#define MR_VIRTUALCHAN_GET_DATA_LEN(msg_pkt_len) \
    ((msg_pkt_len) - (sizeof(struct mr_virtualchan_msg_pkt) - sizeof(struct mr_msg_pkt)))

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize virtual channel interface
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message routing tag
 * @retval Pointer to virtual channel private structure on success
 * @retval NULL on failure
 */
struct mr_virtualchan_priv *mr_virtualchan_init(struct mr_msg_ctrl *msg_ctrl, uint8_t msg_tag);

/**
 * @brief Deinitialize virtual channel interface
 * @param[in] virtualchan_priv Virtual channel private structure to cleanup
 */
void mr_virtualchan_deinit(struct mr_virtualchan_priv *virtualchan_priv);

#endif /* __MR_VIRTUALCHAN_H__ */
