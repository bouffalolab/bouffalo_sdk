/**
 * @file mr_eth_netdev.h
 * @brief Ethernet network device header definitions
 * @details This file contains Ethernet network device structures, state machine
 *          definitions, and packet format specifications for SDIO-based Ethernet
 *          communication with comprehensive flow control and statistics support.
 */

#ifndef __MR_NETDEV_H__
#define __MR_NETDEV_H__

#include <linux/netdevice.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include "mr_msg_ctrl.h"

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

#pragma pack(push, 1)

/**
 * @struct mr_eth_msg_packt
 * @brief Ethernet message packet structure
 * @details Custom Ethernet packet format with message header, flags, and flow control
 */
struct mr_eth_msg_packt {
    struct mr_msg_packt msg_packt; /**< Message packet header */

    uint8_t reseved[1]; /**< Alignment padding */
    uint8_t flag;       /**< Packet flag (see MR_NETDEV_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length Ethernet packet data */
};

#pragma pack(pop)

/**
 * @struct mr_sdio_eth_priv
 * @brief Network device private data structure
 * @details Private data structure for SDIO-based Ethernet network device
 */
struct mr_sdio_eth_priv {
    struct net_device *netdev;    /**< Associated network device */
    struct mr_msg_ctrl *msg_ctrl; /**< Message control interface */
    uint8_t msg_tag;              /**< Message tag for routing */

    int eth_status; /**< Current Ethernet status */

    struct task_struct *daemon_thread;   /**< Background daemon thread */
    struct sk_buff_head thread_skb_head; /**< Thread SKB queue */
    wait_queue_head_t waitq;             /**< Thread wait queue */
    bool thread_condition;               /**< Thread wakeup condition */

    uint8_t credit_consumed_cnt; /**< Credits consumed counter */
    uint8_t credit_limit_cnt;    /**< Credit limit counter */
    spinlock_t credit_lock;      /**< Credit management spinlock */

    struct rtnl_link_stats64 stats; /**< Network statistics (64-bit) */
    spinlock_t stats_lock;          /**< Statistics protection lock */
};

/** @defgroup MR_NETDEV_NETDEV_API Ethernet Network Device API Functions
 * @{
 */

/**
 * @brief Initialize Ethernet network device
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval Pointer to network device on success
 * @retval NULL on failure
 */
struct net_device *mr_sdio_eth_netdev_init(struct mr_msg_ctrl *msg_ctrl, uint8_t msg_tag);

/**
 * @brief Deinitialize Ethernet network device
 * @param[in] netdev Network device to cleanup
 */
void mr_sdio_eth_netdev_deinit(struct net_device *netdev);

/** @} */

#endif /* __MR_NETDEV_NETDEV_H__ */