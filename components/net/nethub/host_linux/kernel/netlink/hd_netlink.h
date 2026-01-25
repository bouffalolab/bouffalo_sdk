/**
 * @file hd_netlink.h
 * @brief Host Driver Netlink Virtualization Interface
 * @details Netlink interface for WiFi management protocol, provides
 *          userspace-kernel communication over Netlink socket with
 *          SDIO/USB transport backend. Implements credit-based flow
 *          control to prevent overwhelming the hardware.
 */

#ifndef __HD_NETLINK_H__
#define __HD_NETLINK_H__

#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <net/sock.h>

#include "hd_msg_ctrl.h"

/** @defgroup NETLINK_MACROS Netlink Module Macros
 * @{
 */
/** Netlink protocol number */
#define HD_NETLINK_PROTO       (31)  /* Custom Netlink protocol */
/** Netlink multicast group */
#define HD_NETLINK_MCGRP       (1)
/** Maximum message size */
#define HD_NETLINK_RAWMTU      (1500)                /* MTU */
#define HD_NETLINK_MAX_MSGLEN  (HD_NETLINK_RAWMTU+4) /* MTU + type(4) */
/** Maximum pending queue depth (0 = unlimited) */
#define HD_NETLINK_PENDING_MAX (128)
/** @} */

/** @defgroup HD_NETLINK_HOST_STATES Netlink Host State Machine
 * @brief State definitions for Netlink host state machine
 * @{
 */
#define HD_NETLINK_HSTA_ERROR      0 /**< Error state */
#define HD_NETLINK_HSTA_RESET      1 /**< Host reset state, needs to send HOST_READY packet */
#define HD_NETLINK_HSTA_HOST_READY 2 /**< Host ready state, waiting for DEVICE_START */
#define HD_NETLINK_HSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup HD_NETLINK_DEVICE_STATES Netlink Device State Machine
 * @brief State definitions for Netlink device state machine
 * @{
 */
#define HD_NETLINK_DSTA_ERROR      0 /**< Error state */
#define HD_NETLINK_DSTA_IDLE       1 /**< Device waiting for host ready */
#define HD_NETLINK_DSTA_HOST_READY 2 /**< Host ready, send DEVICE_START after initialization */
#define HD_NETLINK_DSTA_DEVICE_RUN 3 /**< Normal data transfer state */
/** @} */

/** @defgroup HD_NETLINK_FLAGS Netlink Packet Flags
 * @brief Unified command definitions for Netlink packets
 * @{
 */
enum {
    HD_NETLINK_FLAG_NONE, /**< No specific flag */

    HD_NETLINK_FLAG_DNLD_DATA, /**< Host to device data packet */
    HD_NETLINK_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control) */

    HD_NETLINK_FLAG_HOST_RESET, /**< Host reset command */
    HD_NETLINK_FLAG_HOST_READY, /**< Host ready command */
    HD_NETLINK_FLAG_HOST_STOP,  /**< Host stop command */

    HD_NETLINK_FLAG_DEVICE_RESET, /**< Device reset command */
    HD_NETLINK_FLAG_DEVICE_START, /**< Device start command (with initial credit_limit) */
    HD_NETLINK_FLAG_DEVICE_STOP,  /**< Device stop command */

    HD_NETLINK_FLAG_CREDIT_UPDATE, /**< Flow control packet */
};
/** @} */

#pragma pack(push, 1)

/**
 * @struct hd_netlink_msg_packt
 * @brief Netlink message packet header with flow control
 * @details Header structure for Netlink message packets sent over transport
 *          Similar to hd_tty_msg_packt
 */
struct hd_netlink_msg_packt {
    struct hd_msg_packt msg_packt; /**< Base message packet header */

    uint8_t reseved[1]; /**< Alignment padding */
    uint8_t flag;       /**< Packet flag (see HD_NETLINK_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length data */
};

#pragma pack(pop)

/** Netlink private data structure */
struct hd_netlink_priv {
    /** @name Netlink Socket
     * @{
     */
    struct sock *nl_sock;            /**< Netlink socket */
    uint32_t nl_portid;              /**< Netlink port ID */
    /** @} */

    /** @name Userspace Downlink Send Protection
     * @{
     */
    struct mutex dnld_send_mutex;    /**< Mutex for userspace downlink send */
    struct sk_buff_head pending_queue; /**< Pending queue for packets waiting for credits */
    /** @} */

    /** @name State Machine
     * @{
     */
    int host_state;                       /**< Current host state */
    struct task_struct *daemon_thread;    /**< Background daemon thread */
    struct sk_buff_head thread_skb_head;  /**< Thread SKB queue */
    wait_queue_head_t waitq;              /**< Thread wait queue */
    bool thread_condition;                /**< Thread wakeup condition */
    /** @} */

    /** @name Flow Control (Credit-based)
     * @{
     */
    bool credit_nolimit;             /**< Credit unlimited flag */
    uint8_t credit_limit_cnt;        /**< Credit limit count */
    uint8_t credit_consumed_cnt;     /**< Credit consumed count */
    spinlock_t credit_lock;          /**< Credit spinlock */
    /** @} */

    /** @name Message Control Interface
     * @{
     */
    struct hd_msg_ctrl *msg_ctrl;    /**< Message control interface */
    uint8_t msg_tag;                 /**< Message tag for routing */
    /** @} */

    /** @name Statistics (Kernel ↔ Device via SDIO/USB)
     * @{
     */
    atomic_t tx_packets;              /**< TX to device success packet count */
    atomic_t tx_dropped;              /**< TX to device dropped (pending queue full) */
    atomic_t rx_packets;              /**< RX from device packet count */
    atomic_t rx_dropped;              /**< RX from device dropped packet count */
    /** @} */

    /** @name Statistics (Kernel ↔ Userspace via Netlink)
     * @{
     */
    atomic_t nl_rx_packets;           /**< RX from userspace packet count */
    atomic_t nl_tx_packets;           /**< TX to userspace packet count */
    atomic_t nl_tx_dropped;           /**< TX to userspace dropped packet count */
    /** @} */
};

/** @defgroup NETLINK_API Netlink Module API
 * @{
 */

/**
 * @brief Initialize Netlink module
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing (e.g., HD_MSG_TAG_NETLINK)
 * @retval Pointer to private structure on success
 * @retval NULL on failure
 */
struct hd_netlink_priv *hd_netlink_init(struct hd_msg_ctrl *msg_ctrl, uint8_t msg_tag);

/**
 * @brief Deinitialize Netlink module
 * @param[in] priv Private structure to cleanup
 */
void hd_netlink_deinit(struct hd_netlink_priv *priv);

/** @} */

#endif /* __HD_NETLINK_H__ */
