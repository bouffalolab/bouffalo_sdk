/**
 * @file bflb_msg_ctrl.h
 * @brief Message controller header file for SDIO message routing and management
 * @author mlwang
 * @date 2025-07-17
 */

#ifndef __BFLB_MSG_CTRL___
#define __BFLB_MSG_CTRL___

#include "bflb_core.h"
#include "bflb_frame_buff_ctrl.h"

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/** Download frame type identifier */
#define BL_MSG_CTRL_DNLD_FRAME_TYPE  (0x01)
/** Number of download frame buffers */
#define BL_MSG_CTRL_DNLD_FRAME_NUM   (24)
#define BL_MSG_CTRL_DNLD_FRAME_SIZE  FRAME_BUFF_TOTALSIZE

/** Upload queue depth */
#define BL_MSG_CTRL_UPLD_QUEUE_DEPTH (32)// todo: sync with rx count, fixme

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

/**
 * @brief Message tags for routing different message types
 */
enum {
    BL_MSG_TAG_NONE = 0, /**< No message tag */

    BL_MSG_TAG_TEST_1, /**< Test message tag 1 */
    BL_MSG_TAG_TEST_2, /**< Test message tag 2 */
    BL_MSG_TAG_TEST_3, /**< Test message tag 3 */

    BL_MSG_TAG_SYS,  /**< System message tag */
    BL_MSG_TAG_WIFI, /**< WiFi message tag */

    BL_MSG_TAG_ETH, /**< Ethernet message tag */

    BL_MSG_TAG_TTY, /**< TTY message tag */

    BL_MSG_TAG_MAX /**< Maximum message tag */
};

#pragma pack(push, 1)

/**
 * @brief Message packet header structure
 */
typedef struct bflb_msg_packt {
    uint8_t tag;     /**< Message tag for routing */
    uint8_t sub_tag; /**< Message sub-tag */
    uint16_t len;    /**< Message data length */
    uint8_t data[0]; /**< Message data payload */
} bflb_msg_packt_t;

#pragma pack(pop)

/**
 * @brief Message callback function type (used for both download receive and upload send done)
 * @param frame_elem Frame element
 * @param arg Callback argument
 * @retval 0 Success
 * @retval -1 Error
 */
typedef int (*bflb_msg_cb_t)(frame_elem_t *frame_elem, void *arg);

/**
 * @brief Message controller operations structure for SDIO interaction
 */
typedef struct
{
    struct bflb_device_s *sdio_hd;
    int (*sdio_is_ready)(void);
    int (*sdio_upld_is_busy)(void);
    int (*sdio_dnld_is_busy)(void);
    int (*sdio_upld_start)(frame_elem_t *elem, void *ctrl);
    int (*sdio_dnld_start)(frame_elem_t *elem, void *ctrl);
    int (*sdio_feature_control)(struct bflb_device_s *dev, int cmd, uintptr_t arg);
    const char *sdio_name;
} bflb_msg_ctrl_ops_t;

/**
 * @brief Message controller structure
 */
typedef struct {
    struct bflb_device_s *sdio2_hd; /**< SDIO2 device handle */
    volatile bool hw_reset_flag;    /**< Hardware reset flag */

    uint32_t msg_dnld_max_size; /**< Maximum download message size */
    uint32_t msg_upld_max_size; /**< Maximum upload message size */

    QueueHandle_t dnld_queue; /**< Download done queue for callback processing */
    QueueHandle_t upld_queue; /**< Upload queue from upper layer */

    frame_queue_ctrl_t *dnld_queue_ctrl; /**< Download queue controller */

    bflb_msg_cb_t bflb_msg_dnld_recv_cb[BL_MSG_TAG_MAX]; /**< Download receive callback functions */
    void *bflb_msg_dnld_recv_arg[BL_MSG_TAG_MAX];      /**< Download receive callback arguments */

    bflb_msg_cb_t bflb_msg_upld_send_cb[BL_MSG_TAG_MAX]; /**< Upload send done callback functions */
    void *bflb_msg_upld_send_arg[BL_MSG_TAG_MAX];      /**< Upload send done callback arguments */

    bflb_msg_cb_t bflb_msg_hw_reset_cb[BL_MSG_TAG_MAX]; /**< Hardware reset callback function */
    void *bflb_msg_hw_reset_arg[BL_MSG_TAG_MAX];      /**< Hardware reset callback argument */

    bflb_msg_ctrl_ops_t *msg_sdio_ops;
} bflb_msg_ctrl_t;

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize message controller
 * @retval Pointer to message controller on success
 * @retval NULL on failure
 */
bflb_msg_ctrl_t *bflb_msg_ctrl_init(bflb_msg_ctrl_ops_t *sdio_ops);

/**
 * @brief Deinitialize message controller
 * @param msg_ctrl Message controller instance
 * @retval 0 Success
 * @retval -1 Error
 */
int bflb_msg_ctrl_deinit(bflb_msg_ctrl_t *msg_ctrl);

/**
 * @brief Allocate frame element for message sending
 * @param msg_ctrl Message controller instance
 * @param xTicksToWait Ticks to wait for allocation
 * @retval Pointer to frame element on success
 * @retval NULL on failure
 */
frame_elem_t *bflb_msg_ctrl_alloc(bflb_msg_ctrl_t *msg_ctrl, TickType_t xTicksToWait);

/**
 * @brief Send message through message controller
 * @param msg_ctrl Message controller instance
 * @param frame_elem Frame element to send
 * @param frame_elem Frame element to send to front
 * @retval 0 Success
 * @retval -1 Error
 */
int bflb_msg_ctrl_send(bflb_msg_ctrl_t *msg_ctrl, frame_elem_t *frame_elem, uint8_t tofront);

/**
 * @brief Register message callbacks
 * @param msg_ctrl Message controller instance
 * @param tag Message tag for routing
 * @param dnld_recv_cb Download receive callback function
 * @param dnld_recv_arg Argument for download receive callback
 * @param upld_send_cb Upload send done callback function
 * @param upld_send_arg Argument for upload send done callback
 * @param hw_reset_cb Hardware reset callback function
 * @param hw_reset_arg Argument for hardware reset callback
 * @retval 0 Success
 * @retval -1 Error
 */
int bflb_msg_cb_register(bflb_msg_ctrl_t *msg_ctrl, uint8_t tag,
                       bflb_msg_cb_t dnld_recv_cb, void *dnld_recv_arg,
                       bflb_msg_cb_t upld_send_cb, void *upld_send_arg,
                       bflb_msg_cb_t hw_reset_cb, void *hw_reset_arg);

/**
 * @brief Unregister message callbacks
 * @param msg_ctrl Message controller instance
 * @param tag Message tag
 * @retval 0 Success
 * @retval -1 Error
 */
int bflb_msg_cb_unregister(bflb_msg_ctrl_t *msg_ctrl, uint8_t tag);

/**
 * @brief Upload send done callback (called from ISR)
 * @param msg_ctrl Message controller instance
 * @param frame_elem Frame element that was sent
 * @retval 0 Success
 * @retval -1 Error
 */
int bflb_msg_upld_send_done_cb(bflb_msg_ctrl_t *msg_ctrl, frame_elem_t *frame_elem, bool success);

/**
 * @brief Download receive done callback (called from ISR)
 * @param msg_ctrl Message controller instance
 * @param frame_elem Frame element that was received
 * @retval 0 Success
 * @retval -1 Error
 */
int bflb_msg_dnld_recv_done_cb(bflb_msg_ctrl_t *msg_ctrl, frame_elem_t *frame_elem, bool success);

/**
 * @brief Message host reset callback
 *
 * This function is called when the message host is reset.
 * It wakes up the message processing task and sets the hardware reset flag.
 *
 * @retval 0 Success
 */
int bflb_msg_host_reset_cb(void);

#endif /* __BL_MSG_CTRL___ */
