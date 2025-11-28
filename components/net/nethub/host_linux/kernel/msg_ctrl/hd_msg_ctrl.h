/**
 * @file hd_msg_ctrl.h
 * @brief Host Driver message control and routing header definitions
 * @details This file contains message packet structures, routing definitions,
 *          and control interfaces for inter-subsystem communication
 * @author mlwang
 * @date 2025-07-17
 * @version 1.0
 */

#ifndef __HD_MSG_CTRL___
#define __HD_MSG_CTRL___

#include "hd_sdio_manage.h"
#include "hd_debugfs.h"

/** @defgroup HD_MSG_TAGS Message Tags
 * @brief Message routing tags for different subsystems
 * @{
 */
enum {
    HD_MSG_TAG_NONE, /**< No specific tag */

    HD_MSG_TAG_TEST_1, /**< Test message tag 1 */
    HD_MSG_TAG_TEST_2, /**< Test message tag 2 */
    HD_MSG_TAG_TEST_3, /**< Test message tag 3 */

    HD_MSG_TAG_SYS,  /**< System messages */
    HD_MSG_TAG_WIFI, /**< WiFi subsystem messages */

    HD_MSG_TAG_ETH, /**< Ethernet subsystem messages */

    HD_MSG_TAG_TTY, /**< TTY/UART subsystem messages */

    HD_MSG_TAG_MAX /**< Maximum tag value (boundary marker) */
};

/** @defgroup HD_MSG_CTRL_MACROS Message Control Macros
 * @{
 */
/** Maximum pending SKBs in upload receive queue */
#define HD_MSG_CTRL_UPLD_QUEUE_DEPTH_MAX (1024)
/** @} */
/** @} */

#pragma pack(push, 1)

/**
 * @struct hd_msg_packt
 * @brief Message packet header structure
 * @details Defines the format of message packets used for inter-subsystem communication
 */
struct hd_msg_packt {
    uint8_t tag;     /**< Primary message tag (see HD_MSG_TAG_*) */
    uint8_t sub_tag; /**< Secondary message tag for subcategories */
    uint16_t len;    /**< Length of data payload in bytes */
    uint8_t data[0]; /**< Variable length data payload */
};

#pragma pack(pop)

/**
 * @struct hd_msg_stats
 * @brief Message statistics structure
 * @details Tracks message transfer statistics per tag for monitoring and debugging
 */
struct hd_msg_stats {
    unsigned long tx_packets[HD_MSG_TAG_MAX]; /**< Transmitted packets count per tag */
    unsigned long rx_packets[HD_MSG_TAG_MAX]; /**< Received packets count per tag */
    unsigned long tx_bytes[HD_MSG_TAG_MAX];   /**< Transmitted bytes count per tag */
    unsigned long rx_bytes[HD_MSG_TAG_MAX];   /**< Received bytes count per tag */
    unsigned long rx_errors;                  /**< Total receive errors */
    unsigned long tx_errors;                  /**< Total transmit errors */
    unsigned long rx_dropped;                 /**< Total received packets dropped */
};

/**
 * @typedef hd_msg_cb_t
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
typedef int (*hd_msg_cb_t)(struct sk_buff *skb, bool success, void *arg);

/**
 * @struct hd_msg_ctrl
 * @brief Message control and routing structure
 * @details Central structure for managing message routing between different subsystems
 *          through various transport layers (SDIO, USB, etc.)
 */
struct hd_msg_ctrl {
    /** @name Hardware Configuration
     * @{
     */
    uint8_t msg_hw_mode; /**< Hardware communication mode */
    /** @} */

    /** @name Transport Layer Management
     * @{
     */
    struct hd_sdio_manage *sdio_manage; /**< SDIO management interface */
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
    hd_msg_cb_t hd_msg_upld_recv_cb[HD_MSG_TAG_MAX]; /**< Upload receive callbacks per tag */
    void *hd_msg_upld_recv_arg[HD_MSG_TAG_MAX];      /**< Upload receive callback arguments per tag */
    hd_msg_cb_t hd_msg_dnld_send_cb[HD_MSG_TAG_MAX]; /**< Download send completion callbacks per tag */
    void *hd_msg_dnld_send_arg[HD_MSG_TAG_MAX];      /**< Download send completion callback arguments per tag */
    struct mutex upld_cb_mutex;                      /**< Upload callback registration/execution mutex */
    struct mutex dnld_cb_mutex;                      /**< Download callback registration/execution mutex */
    /** @} */

    /** @name Statistics and Synchronization
     * @{
     */
    struct hd_msg_stats stats; /**< Message transfer statistics */
    spinlock_t stats_lock;     /**< Statistics protection lock */
    /** @} */
};

/** @defgroup HD_MSG_CTRL_API Message Control API Functions
 * @{
 */

/**
 * @brief Initialize SDIO-based message control system
 * @param[out] hd_msg_ctrl Pointer to store created message control structure
 * @param[in] sdio_manage SDIO management structure to use for transport
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_sdio_msg_ctrl_init(struct hd_msg_ctrl **hd_msg_ctrl, struct hd_sdio_manage *sdio_manage);

/**
 * @brief Deinitialize SDIO-based message control system
 * @param[in] msg_ctrl Message control structure to cleanup
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_sdio_msg_ctrl_deinit(struct hd_msg_ctrl *msg_ctrl);

/**
 * @brief Initialize USB-based message control system
 * @param[out] hd_msg_ctrl Pointer to store created message control structure
 * @param[in] usb_manage USB management structure to use for transport
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_usb_msg_ctrl_init(struct hd_msg_ctrl **hd_msg_ctrl, void *usb_manage);

/**
 * @brief Deinitialize USB-based message control system
 * @param[in] hd_msg_ctrl Message control structure to cleanup
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_usb_msg_ctrl_deinit(struct hd_msg_ctrl *hd_msg_ctrl);

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
int hd_msg_cb_register(struct hd_msg_ctrl *msg_ctrl, uint8_t tag,
                       hd_msg_cb_t upld_recv_cb, void *upld_recv_arg,
                       hd_msg_cb_t dnld_send_cb, void *dnld_send_arg);

/**
 * @brief Unified callback unregistration for both upload receive and download send directions
 * @param[in] msg_ctrl Message control structure
 * @param[in] tag Message tag to unregister callbacks for
 * @retval 0 Success
 * @retval <0 Error code
 * @details This function unregisters both upload receive and download send callbacks
 *          for the specified message tag simultaneously.
 */
int hd_msg_cb_unregister(struct hd_msg_ctrl *msg_ctrl, uint8_t tag);

/**
 * @brief Send message through message control system
 * @param[in] msg_ctrl Message control structure
 * @param[in] skb Socket buffer containing message to send
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_msg_ctrl_send(struct hd_msg_ctrl *msg_ctrl, struct sk_buff *skb);

/**
 * @brief Dump message statistics to kernel log
 * @param[in] msg_ctrl Message control structure
 * @retval None
 */
void hd_msg_ctrl_dump_stats(struct hd_msg_ctrl *msg_ctrl);

/** @} */

#endif /* __HD_MSG_CTRL___ */
