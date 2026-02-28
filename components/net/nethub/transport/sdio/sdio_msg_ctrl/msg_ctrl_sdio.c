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
#include "mr_netdev.h"

#include "msg_ctrl_sdio.h"

#define DBG_TAG "MSG"
#include "log.h"

#define FRAME_BUFFER_ATTR __ALIGNED(64) ATTR_WIFI_RAM_SECTION

/** Download frame buffer pool (statically allocated, non-cached) */
static FRAME_BUFFER_ATTR msg_sdio_dnld_fmt_t msg_dnld_frame_buff[MSG_DNLD_FRAME_CNT + 1];

/** MSG_ctrl sdio instance */
mr_msg_ctrl_priv_t *g_msg_sdio_ctrl = NULL;

/**
 * @brief Initialize SDIO-based message controller
 * @retval 0 Success
 * @retval -1 Error (initialization failed)
 * @note Configures message controller with SDIO
 * @note Frame buffers are statically allocated in non-cached memory
 */
int msg_ctrl_sdio_init(void)
{
    mr_msg_ctrl_priv_t *msg_ctrl;

    if (g_msg_sdio_ctrl != NULL) {
        LOG_E("msg_ctrl already initialized\r\n");
        return -1;
    }

    /* Configure message controller for SDIO transport */
    mr_msg_ctrl_cfg_t msg_ctrl_cfg = {
        .name = "msg_ctrl_sdio",       /* Instance name for debugging */
        .dev_ops = &msg_ctrl_sdio_ops, /* SDIO hardware operations */

        /* Download (Host→Device) buffer configuration */
        .dnld_frame_size = sizeof(msg_sdio_dnld_fmt_t),
        .dnld_frame_count = MSG_DNLD_FRAME_CNT,
        .dnld_frame_buff = msg_dnld_frame_buff[0].frame_elem,
        .dnld_frame_type = 0x01,

        /* Upload (Device→Host) queue configuration */
        .upld_queue_depth = 64, /* Max 64 pending upload messages */

        /* Processing task configuration */
        .task_priority = 27,       /* FreeRTOS priority */
        .task_stack_size = 1024,   /* Stack size in bytes */
        .task_period_max_ms = 100, /* 100ms max wait when idle */
        .msg_task_cb = NULL,       /* No custom task callback */
    };

    msg_ctrl = mr_msg_ctrl_init(&msg_ctrl_cfg);
    if (msg_ctrl == NULL) {
        LOG_E("Failed to initialize message controller\r\n");
        return -1;
    }

    g_msg_sdio_ctrl = msg_ctrl;

    return 0;
}
