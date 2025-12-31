/**
 * @file mr_msg_ctrl_sdio.c
 * @brief SDIO-specific message controller initialization and configuration
 */

#include "bflb_core.h"

#include "mm.h"

#include "FreeRTOS.h"
#include "task.h"

#include "mr_frame_buff_ctrl.h"
#include "mr_msg_ctrl.h"

#include "mr_sdio_drv.h"

#define DBG_TAG "MSG"
#include "log.h"

/**
 * @brief Frame buffer memory attribute (aligned and non-cached)
 * @note Required for DMA transfers, ensures cache coherency
 */
#define FRAME_BUFFER_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION

/**
 * @brief Message buffer configuration for SDIO transport
 * @note Frame size = 512*3 bytes data + header room for mr_frame_elem_t
 */
#define MSG_FRAME_CNT     (8)                                /**< Total frame buffer count */
#define MSG_FRAME_SIZE    (512 * 3 + FRAME_BUFF_HEADER_ROOM) /**< Frame buffer size (data + header) */

/** Download frame buffer pool (statically allocated, non-cached) */
static FRAME_BUFFER_ATTR uint8_t msg_dnld_frame_buff[MSG_FRAME_CNT][MSG_FRAME_SIZE];

/**
 * @brief Initialize SDIO-based message controller
 * @param msg_ctrl Pointer to receive initialized message controller instance
 * @retval 0 Success
 * @retval -1 Error (initialization failed)
 * @note Configures message controller with SDIO
 * @note Frame buffers are statically allocated in non-cached memory
 */
int msg_ctrl_sdio_init(mr_msg_ctrl_priv_t **msg_ctrl)
{
    if (msg_ctrl == NULL) {
        LOG_E("msg_ctrl pointer is NULL\r\n");
        return -1;
    }

    /* Configure message controller for SDIO transport */
    mr_msg_ctrl_cfg_t msg_ctrl_cfg = {
        .name = "msg_ctrl_sdio",       /* Instance name for debugging */
        .dev_ops = &msg_ctrl_sdio_ops, /* SDIO hardware operations */

        /* Download (Host→Device) buffer configuration */
        .dnld_frame_size = MSG_FRAME_SIZE,      /* 1536 bytes per frame */
        .dnld_frame_count = MSG_FRAME_CNT,      /* 8 concurrent download buffers */
        .dnld_frame_buff = msg_dnld_frame_buff, /* Statically allocated non-cached buffer pool */
        .dnld_frame_type = 0x01,                /* Frame type ID for debugging */

        /* Upload (Device→Host) queue configuration */
        .upld_queue_depth = 64, /* Max 64 pending upload messages */

        /* Processing task configuration */
        .task_priority = 20,       /* FreeRTOS priority */
        .task_stack_size = 1024,   /* Stack size in bytes */
        .task_period_max_ms = 100, /* 100ms max wait when idle */
        .msg_task_cb = NULL,       /* No custom task callback */
    };

    *msg_ctrl = mr_msg_ctrl_init(&msg_ctrl_cfg);
    if (*msg_ctrl == NULL) {
        LOG_E("Failed to initialize message controller\r\n");
        return -1;
    }

    return 0;
}