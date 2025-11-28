/**
 * @file hd_eth_netdev.h
 * @brief Host Driver Ethernet network device header definitions
 * @author mlwang
 * @date 2025-07-17
 * @details This file contains Ethernet network device structures, state machine
 *          definitions, and packet format specifications for SDIO-based Ethernet
 *          communication with comprehensive flow control and statistics support.
 */

#ifndef __HD_ETH_NETDEV_H__
#define __HD_ETH_NETDEV_H__

#include <linux/netdevice.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include "hd_msg_ctrl.h"

/** @defgroup HD_ETH_HOST_STATES Ethernet Host State Machine
 * @brief State definitions for Ethernet host state machine
 * @{
 */
#define HD_ETH_HSTA_ERROR        0 /**< Error state */
#define HD_ETH_HSTA_RESET        1 /**< Host reset state, needs to send HOST_READY packet */
#define HD_ETH_HSTA_HOST_READY   2 /**< Host ready state, waiting for DEVICE_READY */
#define HD_ETH_HSTA_DEVICE_READY 3 /**< Device ready state, waiting for DEVICE_START */
#define HD_ETH_HSTA_DEVICE_RUN   4 /**< Normal data transfer state */
/** @} */

/** @defgroup HD_ETH_DEVICE_STATES Ethernet Device State Machine
 * @brief State definitions for Ethernet device state machine
 * @{
 */
#define HD_ETH_DSTA_ERROR        0 /**< Error state */
#define HD_ETH_DSTA_IDLE         1 /**< Device waiting for host ready */
#define HD_ETH_DSTA_HOST_READY   2 /**< Host ready, send DEVICE_READY after initialization */
#define HD_ETH_DSTA_DEVICE_READY 3 /**< Device ready, send DEVICE_START after network link up */
#define HD_ETH_DSTA_DEVICE_RUN   4 /**< Normal data transfer state */
/** @} */

/** @defgroup HD_ETH_FLAGS Ethernet Packet Flags
 * @brief Flag definitions for Ethernet packet types
 * @{
 */
enum {
    HD_ETH_FLAG_NONE, /**< No specific flag */

    HD_ETH_FLAG_DNLD_DATA, /**< Host to device data packet */
    HD_ETH_FLAG_UPLD_DATA, /**< Device to host data packet (may carry flow control) */

    HD_ETH_FLAG_HOST_RESET, /**< Host reset (error recovery), device enters DSTA_IDLE */
    HD_ETH_FLAG_HOST_READY, /**< Host ready, device enters DSTA_HOST_READY */
    HD_ETH_FLAG_HOST_STOP,  /**< Host stop, device enters DSTA_IDLE */

    HD_ETH_FLAG_DEVICE_RESET, /**< Device reset, host enters HSTA_HOST_READY */
    HD_ETH_FLAG_DEVICE_READY, /**< Device ready (must carry MAC address), host enters HSTA_DEVICE_READY */
    HD_ETH_FLAG_DEVICE_START, /**< Device start (must carry initial flow control), host enters HSTA_DEVICE_RUN */
    HD_ETH_FLAG_DEVICE_STOP,  /**< Device stop, host enters HSTA_DEVICE_READY */

    HD_ETH_FLAG_CREDIT_UPDATE, /**< Standalone flow control update when no DNLD_DATA */

    HD_ETH_FLAG_MAX, /**< Maximum flag value */
};
/** @} */

#pragma pack(push, 1)

/**
 * @struct hd_eth_msg_packt
 * @brief Ethernet message packet structure
 * @details Custom Ethernet packet format with message header, flags, and flow control
 */
struct hd_eth_msg_packt {
    struct hd_msg_packt msg_packt; /**< Message packet header */

    uint8_t reseved[1]; /**< Alignment padding */
    uint8_t flag;       /**< Packet flag (see HD_ETH_FLAG_*) */

    uint8_t credit_update_flag; /**< Credit update flag */
    uint8_t credit_limit_cnt;   /**< Credit limit counter */

    uint8_t data[0]; /**< Variable length Ethernet packet data */
};

#pragma pack(pop)

/**
 * @struct hd_sdio_eth_priv
 * @brief Network device private data structure
 * @details Private data structure for SDIO-based Ethernet network device
 */
struct hd_sdio_eth_priv {
    struct net_device *netdev;    /**< Associated network device */
    struct hd_msg_ctrl *msg_ctrl; /**< Message control interface */
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

/** @defgroup HD_ETH_NETDEV_API Ethernet Network Device API Functions
 * @{
 */

/**
 * @brief Initialize Ethernet network device
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval Pointer to network device on success
 * @retval NULL on failure
 */
struct net_device *hd_sdio_eth_netdev_init(struct hd_msg_ctrl *msg_ctrl, uint8_t msg_tag);

/**
 * @brief Deinitialize Ethernet network device
 * @param[in] netdev Network device to cleanup
 */
void hd_sdio_eth_netdev_deinit(struct net_device *netdev);

/** @} */

#endif /* __HD_ETH_NETDEV_H__ */