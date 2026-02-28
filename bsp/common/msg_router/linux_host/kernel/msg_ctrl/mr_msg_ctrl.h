/**
 * @file mr_msg_ctrl.h
 * @brief Message control and routing header definitions
 * @details This file contains message packet structures, routing definitions,
 *          and control interfaces for inter-subsystem communication
 */

#ifndef __MR_MSG_CTRL___
#define __MR_MSG_CTRL___

#include "mr_sdio_manage.h"
#include "mr_debugfs.h"

/** @defgroup MR_MSG_TAGS Message Tags
 * @brief Message routing tags for different subsystems
 * @{
 */
enum {
    MR_MSG_TAG_NONE, /**< No message tag */

    MR_MSG_TAG_TEST_1, /**< Test message tag 1 */
    MR_MSG_TAG_TEST_2, /**< Test message tag 2 */
    MR_MSG_TAG_TEST_3, /**< Test message tag 3 */

    MR_MSG_TAG_SYS, /**< System message tag */

    MR_MSG_TAG_NETDEV_WIFI_STA, /**< WiFi message tag */
    MR_MSG_TAG_NETDEV_WIFI_AP,  /**< WiFi AP message tag */
    MR_MSG_TAG_NETDEV_ETH,      /**< Ethernet message tag */

    MR_MSG_TAG_TTY_CMD,    /**< TTY command message tag */
    MR_MSG_TAG_TTY_USER_1, /**< TTY user message tag */
    MR_MSG_TAG_TTY_USER_2, /**< TTY user message tag */
    MR_MSG_TAG_TTY_USER_3, /**< TTY user message tag */

    MR_MSG_TAG_NETLINK,    /**< Netlink message tag */
    MR_MSG_TAG_VIRTUALCHAN, /**< Virtualchan message tag */

    MR_MSG_TAG_MAX /**< Maximum message tag */
};

/** @defgroup MR_MSG_CTRL_MACROS Message Control Macros
 * @{
 */
/** Maximum pending SKBs in upload receive queue */
#define MR_MSG_CTRL_UPLD_QUEUE_DEPTH_MAX (1024)
/** @} */
/** @} */

#pragma pack(push, 1)

/**
 * @struct mr_msg_pkt
 * @brief Message packet header structure
 * @details Defines the format of message packets used for inter-subsystem communication
 */
struct mr_msg_pkt {
    uint8_t tag;     /**< Primary message tag (see MR_MSG_TAG_*) */
    uint8_t sub_tag; /**< Secondary message tag for subcategories */
    uint16_t len;    /**< Length of data payload in bytes */
    uint8_t data[0]; /**< Variable length data payload */
};

#pragma pack(pop)

/**
 * @struct mr_msg_stats
 * @brief Message statistics structure
 * @details Tracks message transfer statistics per tag for monitoring and debugging
 */
struct mr_msg_stats {
    unsigned long tx_packets[MR_MSG_TAG_MAX]; /**< Transmitted packets count per tag */
    unsigned long rx_packets[MR_MSG_TAG_MAX]; /**< Received packets count per tag */
    unsigned long tx_bytes[MR_MSG_TAG_MAX];   /**< Transmitted bytes count per tag */
    unsigned long rx_bytes[MR_MSG_TAG_MAX];   /**< Received bytes count per tag */
    unsigned long rx_errors;                  /**< Total receive errors */
    unsigned long tx_errors;                  /**< Total transmit errors */
    unsigned long rx_dropped;                 /**< Total received packets dropped */
};

/**
 * @typedef mr_msg_cb_t
 * @brief Unified message callback function type for both receive and send completion
 * @param[in] skb Socket buffer containing message data
 * @param[in] success Operation success flag (true for receive success, or send completion status)
 * @param[in] arg User-defined argument passed to callback
 * @retval 0 Success
 * @retval <0 Error code
 * @details This unified callback type is used for both:
 *          - Message reception (success always true for received messages)
 *          - Send completion notification (success indicates transmission result)
 */
typedef int (*mr_msg_cb_t)(struct sk_buff *skb, bool success, void *arg);

/**
 * @struct mr_msg_ctrl
 * @brief Message control and routing structure
 * @details Central structure for managing message routing between different subsystems
 *          through various transport layers (SDIO, USB, etc.)
 */
struct mr_msg_ctrl {
    /** @name Hardware Configuration
     * @{
     */
    uint8_t msg_hw_mode; /**< Hardware communication mode */
    /** @} */

    /** @name Transport Layer Management
     * @{
     */
    struct mr_sdio_manage *sdio_manage; /**< SDIO management interface */
    void *usb_manage;                   /**< USB management interface (TODO) */
    /** @} */

    /** @name Buffer Size Configuration
     * @{
     */
    uint32_t msg_dnld_max_size; /**< Maximum download message size */
    uint32_t msg_upld_max_size; /**< Maximum upload message size */
    /** @} */

    struct sk_buff_head upld_skb_head; /**< Upload skb queue for deferred processing */
    struct sk_buff_head dnld_skb_head; /**< Download skb queue for deferred processing */

    /** @name Message Processing
     * @{
     */
    struct work_struct recv_work;            /**< Receive work structure */
    struct workqueue_struct *recv_workqueue; /**< Receive work queue */
    /** @} */

    /** @name Message Routing Callbacks
     * @{
     */
    mr_msg_cb_t mr_msg_upld_recv_cb[MR_MSG_TAG_MAX]; /**< Upload receive callbacks per tag */
    void *mr_msg_upld_recv_arg[MR_MSG_TAG_MAX];      /**< Upload receive callback arguments per tag */
    mr_msg_cb_t mr_msg_dnld_send_cb[MR_MSG_TAG_MAX]; /**< Download send completion callbacks per tag */
    void *mr_msg_dnld_send_arg[MR_MSG_TAG_MAX];      /**< Download send completion callback arguments per tag */
    struct mutex upld_cb_mutex;                      /**< Upload callback registration/execution mutex */
    struct mutex dnld_cb_mutex;                      /**< Download callback registration/execution mutex */
    /** @} */

    /** @name Statistics and Synchronization
     * @{
     */
    struct mr_msg_stats stats; /**< Message transfer statistics */
    spinlock_t stats_lock;     /**< Statistics protection lock */
    /** @} */
};

/** @defgroup MR_MSG_CTRL_API Message Control API Functions
 * @{
 */

/**
 * @brief Initialize SDIO-based message control system
 * @param[out] mr_msg_ctrl Pointer to store created message control structure
 * @param[in] sdio_manage SDIO management structure to use for transport
 * @retval 0 Success
 * @retval <0 Error code
 */
int mr_sdio_msg_ctrl_init(struct mr_msg_ctrl **mr_msg_ctrl, struct mr_sdio_manage *sdio_manage);

/**
 * @brief Deinitialize SDIO-based message control system
 * @param[in] msg_ctrl Message control structure to cleanup
 * @retval 0 Success
 * @retval <0 Error code
 */
int mr_sdio_msg_ctrl_deinit(struct mr_msg_ctrl *msg_ctrl);

/**
 * @brief Initialize USB-based message control system
 * @param[out] mr_msg_ctrl Pointer to store created message control structure
 * @param[in] usb_manage USB management structure to use for transport
 * @retval 0 Success
 * @retval <0 Error code
 */
int mr_usb_msg_ctrl_init(struct mr_msg_ctrl **mr_msg_ctrl, void *usb_manage);

/**
 * @brief Deinitialize USB-based message control system
 * @param[in] mr_msg_ctrl Message control structure to cleanup
 * @retval 0 Success
 * @retval <0 Error code
 */
int mr_usb_msg_ctrl_deinit(struct mr_msg_ctrl *mr_msg_ctrl);

/**
 * @brief Unified callback registration for both upload receive and download send directions
 * @param[in] msg_ctrl Message control structure
 * @param[in] tag Message tag to register callbacks for
 * @param[in] upld_recv_cb Upload receive callback function (can be NULL to keep existing)
 * @param[in] upld_recv_arg User argument to pass to upload receive callback
 * @param[in] dnld_send_cb Download send completion callback function (can be NULL to keep existing)
 * @param[in] dnld_send_arg User argument to pass to download send callback
 * @retval 0 Success
 * @retval <0 Error code
 * @details This function provides a unified interface for registering both upload and download
 *          callbacks simultaneously for a specific message tag. Pass NULL for callback functions
 *          to leave the corresponding direction unchanged.
 */
int mr_msg_cb_register(struct mr_msg_ctrl *msg_ctrl, uint8_t tag, mr_msg_cb_t upld_recv_cb, void *upld_recv_arg,
                       mr_msg_cb_t dnld_send_cb, void *dnld_send_arg);

/**
 * @brief Unified callback unregistration for both upload receive and download send directions
 * @param[in] msg_ctrl Message control structure
 * @param[in] tag Message tag to unregister callbacks for
 * @retval 0 Success
 * @retval <0 Error code
 * @details This function unregisters both upload receive and download send callbacks
 *          for the specified message tag simultaneously.
 */
int mr_msg_cb_unregister(struct mr_msg_ctrl *msg_ctrl, uint8_t tag);

/**
 * @brief Send message through message control system
 * @param[in] msg_ctrl Message control structure
 * @param[in] skb Socket buffer containing message to send
 * @retval 0 Success
 * @retval <0 Error code
 */
int mr_msg_ctrl_send(struct mr_msg_ctrl *msg_ctrl, struct sk_buff *skb);

/**
 * @brief Dump message statistics to kernel log
 * @param[in] msg_ctrl Message control structure
 * @retval None
 */
void mr_msg_ctrl_dump_stats(struct mr_msg_ctrl *msg_ctrl);

/** @} */

#endif /* __MR_MSG_CTRL___ */
