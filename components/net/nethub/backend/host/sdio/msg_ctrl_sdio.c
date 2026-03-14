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

static FRAME_BUFFER_ATTR msg_sdio_dnld_fmt_t msg_dnld_frame_buff[MSG_DNLD_FRAME_CNT + 1];

mr_msg_ctrl_priv_t *g_msg_sdio_ctrl = NULL;

int msg_ctrl_sdio_init(void)
{
    mr_msg_ctrl_priv_t *msg_ctrl;

    if (g_msg_sdio_ctrl != NULL) {
        LOG_E("msg_ctrl already initialized\r\n");
        return -1;
    }

    mr_msg_ctrl_cfg_t msg_ctrl_cfg = {
        .name = "msg_ctrl_sdio",
        .dev_ops = &msg_ctrl_sdio_ops,
        .dnld_frame_size = sizeof(msg_sdio_dnld_fmt_t),
        .dnld_frame_count = MSG_DNLD_FRAME_CNT,
        .dnld_frame_buff = msg_dnld_frame_buff[0].frame_elem,
        .dnld_frame_type = 0x01,
        .upld_queue_depth = 64,
        .task_priority = 27,
        .task_stack_size = 1024,
        .task_period_max_ms = 100,
        .msg_task_cb = NULL,
    };

    msg_ctrl = mr_msg_ctrl_init(&msg_ctrl_cfg);
    if (msg_ctrl == NULL) {
        LOG_E("Failed to initialize message controller\r\n");
        return -1;
    }

    g_msg_sdio_ctrl = msg_ctrl;

    LOG_I("DNLD Frame Buffer Info:\r\n");
    LOG_I("  - Buffer address: 0x%p, size: %u bytes\r\n",
          msg_dnld_frame_buff, sizeof(msg_dnld_frame_buff));
    LOG_I("  - Max buffer count: %u\r\n", MSG_DNLD_FRAME_CNT);
    LOG_I("  - Single frame size: %u bytes\r\n", sizeof(msg_sdio_dnld_fmt_t));

    return 0;
}

void msg_ctrl_sdio_print_dnld_status(void)
{
    mr_frame_queue_ctrl_t *dnld_ctrl;
    int pool_waiting = 0;
    int output_waiting = 0;
    int input_waiting = 0;
    int used_count;

    if (g_msg_sdio_ctrl == NULL || g_msg_sdio_ctrl->dnld_queue_ctrl == NULL) {
        LOG_W("MSG controller not initialized\r\n");
        return;
    }

    dnld_ctrl = g_msg_sdio_ctrl->dnld_queue_ctrl;

    if (dnld_ctrl->pool_queue) {
        pool_waiting = mr_frame_queue_messages_waiting(dnld_ctrl->pool_queue);
    }
    if (dnld_ctrl->output_queue) {
        output_waiting = mr_frame_queue_messages_waiting(dnld_ctrl->output_queue);
    }
    if (dnld_ctrl->input_queue) {
        input_waiting = mr_frame_queue_messages_waiting(dnld_ctrl->input_queue);
    }

    used_count = MSG_DNLD_FRAME_CNT - pool_waiting;

    LOG_I("DNLD Frame Buffer Status (Host->Device):\r\n");
    LOG_I("  - Max buffer count: %u\r\n", MSG_DNLD_FRAME_CNT);
    LOG_I("  - Used buffer count: %u\r\n", used_count);
    LOG_I("  - Free buffer count: %u\r\n", pool_waiting);
    LOG_I("  - Output queue waiting: %u\r\n", output_waiting);
    LOG_I("  - Input queue waiting: %u\r\n", input_waiting);
    LOG_I("  - Usage rate: %u%%\r\n", (used_count * 100) / MSG_DNLD_FRAME_CNT);
}
