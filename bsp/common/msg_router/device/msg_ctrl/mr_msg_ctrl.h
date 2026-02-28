/**
 * @file mr_msg_ctrl.h
 * @brief Message controller header file for SDIO message routing and management
 */

#ifndef __MR_MSG_CTRL___
#define __MR_MSG_CTRL___

#include "bflb_core.h"
#include "mr_frame_buff_ctrl.h"

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

/**
 * @brief Message tags for routing different message types
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

#pragma pack(push, 1)

/**
 * @brief Message packet header structure
 */
typedef struct mr_msg_pkt {
    uint8_t tag;     /**< Message tag for routing */
    uint8_t sub_tag; /**< Message sub-tag */
    uint16_t len;    /**< Message data length */
    uint8_t data[0]; /**< Message data payload */
} mr_msg_t;

#pragma pack(pop)

/* Forward declarations */
typedef struct mr_msg_ctrl_priv_s mr_msg_ctrl_priv_t;
typedef struct mr_msg_ctrl_cfg_s mr_msg_ctrl_cfg_t;
typedef struct mr_msg_ctrl_ops_s mr_msg_ctrl_ops_t;

/**
 * @brief Message callback function type (used for both download receive and upload send done)
 * @param frame_elem Frame element containing message data
 * @param arg Callback argument (user-defined context)
 * @retval 0 Success, ownership transferred to callback (caller should not free)
 * @retval <0 Error, caller retains ownership (caller must free frame_elem)
 * @note Called from msg_proc_task context (not ISR)
 */
typedef int (*mr_msg_cb_t)(mr_frame_elem_t *frame_elem, void *arg);

/**
 * @brief Message controller hardware operations interface
 * @note Provides hardware abstraction layer for different transport layers (SDIO, etc.)
 */
struct mr_msg_ctrl_ops_s {
    const char *name; /**< Device name for logging */

    int (*dev_init)(mr_msg_ctrl_priv_t *msg_ctrl);   /**< Initialize hardware device */
    int (*dev_deinit)(mr_msg_ctrl_priv_t *msg_ctrl); /**< Deinitialize hardware device */
    int (*dev_is_ready)(void);                       /**< Check if device is ready (0=ready, <0=not ready) */
    int (*dev_upld_is_busy)(void);                   /**< Check if upload hardware is busy (0=idle, >0=busy) */
    int (*dev_dnld_is_busy)(void);                   /**< Check if download hardware is busy (0=idle, >0=busy) */
    int (*dev_upld_start)(mr_frame_elem_t *elem);    /**< Start upload transmission (0=success, <0=error) */
    int (*dev_dnld_start)(mr_frame_elem_t *elem);    /**< Start download reception (0=success, <0=error) */
};

/**
 * @brief Message controller configuration structure
 * @note All parameters must be properly configured before calling mr_msg_ctrl_init
 */
struct mr_msg_ctrl_cfg_s {
    char *name; /**< Controller instance name (for task and logging) */

    mr_msg_ctrl_ops_t *dev_ops; /**< Hardware operations interface (required) */

    /* Download buffer configuration */
    uint16_t dnld_frame_size;  /**< Frame buffer size in bytes (must be > sizeof(mr_msg_t)) */
    uint16_t dnld_frame_count; /**< Number of frame buffers (determines max concurrent downloads) */
    void *dnld_frame_buff;     /**< Frame buffer pool address (must be non-cacheable, aligned, size = dnld_frame_size * dnld_frame_count) */
    uint16_t dnld_frame_type;  /**< Frame buffer type identifier (for debugging) */

    /* Upload queue configuration */
    uint16_t upld_queue_depth; /**< Upload queue depth (max pending upload messages) */

    /* Processing task configuration */
    int32_t task_stack_size; /**< Processing task stack size in bytes */
    int32_t task_priority;   /**< Processing task priority */

    uint32_t task_period_max_ms;                                                /**< Max wait period when no events pending (milliseconds) */
    int (*msg_task_cb)(mr_msg_ctrl_priv_t *msg_ctrl, uint32_t *notified_value); /**< Optional task callback for custom processing (can be NULL) */
};

/**
 * @brief Message controller private data structure
 * @note This structure is opaque to applications, managed internally by msg_ctrl
 */
typedef struct mr_msg_ctrl_priv_s {
    /* Configuration */
    mr_msg_ctrl_cfg_t cfg; /**< Controller configuration (copied at init) */

    /* Runtime state */
    volatile bool hw_reset_flag; /**< Hardware reset flag (deprecated, using event mechanism now) */

    /* Task and synchronization */
    TaskHandle_t msg_proc_task; /**< Message processing task handle */

    /* Message queues */
    QueueHandle_t dnld_done_queue; /**< Download done queue (from ISR to msg_proc_task for callback) */
    QueueHandle_t upld_wait_queue; /**< Upload wait queue (from application to msg_proc_task for transmission) */
    QueueHandle_t upld_done_queue; /**< Upload done queue (from ISR to msg_proc_task for callback) */

    /* Buffer management */
    mr_frame_queue_ctrl_t *dnld_queue_ctrl; /**< Download frame buffer pool controller */

    /* Application callbacks indexed by message tag */
    mr_msg_cb_t mr_msg_dnld_recv_cb[MR_MSG_TAG_MAX]; /**< Download receive callback functions (per tag) */
    void *mr_msg_dnld_recv_arg[MR_MSG_TAG_MAX];      /**< Download receive callback arguments (per tag) */

    mr_msg_cb_t mr_msg_upld_send_cb[MR_MSG_TAG_MAX]; /**< Upload send done callback functions (per tag) */
    void *mr_msg_upld_send_arg[MR_MSG_TAG_MAX];      /**< Upload send done callback arguments (per tag) */

    mr_msg_cb_t mr_msg_hw_reset_cb[MR_MSG_TAG_MAX]; /**< Hardware reset callback functions (per tag) */
    void *mr_msg_hw_reset_arg[MR_MSG_TAG_MAX];      /**< Hardware reset callback arguments (per tag) */

} mr_msg_ctrl_priv_t;

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/* Lifecycle management */
mr_msg_ctrl_priv_t *mr_msg_ctrl_init(mr_msg_ctrl_cfg_t *cfg); /**< Initialize message controller */
int mr_msg_ctrl_deinit(mr_msg_ctrl_priv_t *msg_ctrl);         /**< Deinitialize message controller */

/* Application APIs for data transfer */
int mr_msg_ctrl_dnld_alloc(mr_msg_ctrl_priv_t *msg_ctrl, mr_frame_elem_t **frame_elem, TickType_t xTicksToWait); /**< Allocate download buffer (use with caution) */
int mr_msg_ctrl_dnld_free(mr_msg_ctrl_priv_t *msg_ctrl, mr_frame_elem_t *frame_elem);                            /**< Free download buffer */
int mr_msg_ctrl_upld_send(mr_msg_ctrl_priv_t *msg_ctrl, mr_frame_elem_t *frame_elem);                            /**< Send upload message */

/* Callback registration */
int mr_msg_cb_register(mr_msg_ctrl_priv_t *msg_ctrl, uint8_t tag,
                       mr_msg_cb_t dnld_recv_cb, void *dnld_recv_arg,
                       mr_msg_cb_t upld_send_cb, void *upld_send_arg,
                       mr_msg_cb_t hw_reset_cb, void *hw_reset_arg); /**< Register callbacks for message tag */
int mr_msg_cb_unregister(mr_msg_ctrl_priv_t *msg_ctrl, uint8_t tag); /**< Unregister callbacks for message tag */

/* Hardware layer callbacks (called by hardware drivers from ISR context) */
int mr_msg_upld_send_done_cb(mr_msg_ctrl_priv_t *msg_ctrl, mr_frame_elem_t *frame_elem, bool success); /**< Upload complete callback */
int mr_msg_dnld_recv_done_cb(mr_msg_ctrl_priv_t *msg_ctrl, mr_frame_elem_t *frame_elem, bool success); /**< Download complete callback */
int mr_msg_host_reset_cb(mr_msg_ctrl_priv_t *msg_ctrl);                                                /**< Hardware reset callback */

#endif /* __MR_MSG_CTRL___ */
