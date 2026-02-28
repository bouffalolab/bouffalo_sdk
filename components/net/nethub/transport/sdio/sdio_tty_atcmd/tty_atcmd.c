#include "mm.h"
#include "board.h"

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_tty.h"

#include "tty_atcmd.h"

#define DBG_TAG "TTY_ATCMD"
#include "log.h"

/** Frame buffer memory attribute (aligned and non-cacheable) */
#define FRAME_BUFFER_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION

/** Command channel frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t tty_atcmd_frame_buff[TTY_ATCMD_SLOT_CNT][TTY_ATCMD_MTU+FRAME_BUFF_HEADER_ROOM];

/** Command channel TTY instance */
mr_tty_priv_t *g_tty_atcmd_priv = NULL;

static atcmd_dnld_cb_t g_atcmd_dnld_cb = NULL;
static void *g_atcmd_dnld_arg = NULL;

static int tty_atcmd_dnld_callback(mr_tty_priv_t *priv, mr_tty_msg_t *tty_msg_pkt)
{
    uint16_t data_len;

    data_len = MR_TTY_MSG_PACKET_GET_DATA_SIZE(tty_msg_pkt);

    LOG_I("TTY %s dnld %dByte:\r\n", priv->tty_cfg.name, data_len);

    if (g_atcmd_dnld_cb) {
        g_atcmd_dnld_cb(g_atcmd_dnld_arg, tty_msg_pkt->data, MR_TTY_MSG_PACKET_GET_DATA_SIZE(tty_msg_pkt));
    } else {
        // DBG_HEXDUMP(tty_msg_pkt->data, data_len);
    }

    /* Free the buffer and update flow control */
    mr_tty_dnld_elem_free(priv, tty_msg_pkt);

    return 0;
}

int tty_atcmd_init(mr_msg_ctrl_priv_t *msg_ctrl)
{
    mr_tty_priv_t *priv;

    mr_tty_cfg_t tty_cmd_cfg = {
        .name = "tty_atcmd",
        .msg_ctrl = msg_ctrl,
        .msg_tag = MR_MSG_TAG_TTY_CMD,
        .upld_frame_size = TTY_ATCMD_MTU+FRAME_BUFF_HEADER_ROOM,
        .upld_frame_count = TTY_ATCMD_SLOT_CNT,
        .upld_frame_buff = tty_atcmd_frame_buff,
        .dnld_credit_max = TTY_ATCMD_SLOT_CNT - 1,
        .upld_credit_update_threshold = TTY_ATCMD_SLOT_CNT / 2,
        .dnld_output_cb = tty_atcmd_dnld_callback,
        .upld_done_cb = NULL,
        .task_stack_size = 512,
        .task_priority = 15,
        .task_period_max_ms = 100,
    };
    priv = mr_tty_init(&tty_cmd_cfg);
    if (!priv) {
        LOG_E("Failed to init tty cmd\r\n");
    }

    g_tty_atcmd_priv = priv;

    return 0;
}

int tty_atcmd_dnld_recv_register(atcmd_dnld_cb_t dnld_cb, void *arg)
{
    g_atcmd_dnld_cb = dnld_cb;
    g_atcmd_dnld_arg = arg;

    return 0;
}

int tty_atcmd_upld_send(uint8_t *data_buff, uint16_t data_size)
{
    int ret;
    mr_tty_priv_t *priv = g_tty_atcmd_priv;

    if (!priv || !data_buff || data_size == 0 || data_size > TTY_ATCMD_MTU) {
        LOG_E("%s: Invalid parameters\r\n", __func__);
        return -1;
    }

    /* Verify TTY channel is ready for transmission */
    if (priv->tty_status != MR_TTY_DSTA_DEVICE_RUN || !priv->tty_ready) {
        LOG_E("TTY not ready or not in RUN state (status=%d, ready=%d)\r\n", priv->tty_status, priv->tty_ready);
        return -1;
    }

    ret = mr_tty_upld_data_send(priv, data_buff, data_size);
    if (ret < 0) {
        LOG_E("%s: Failed to send upld data\r\n", __func__);
        return -1;
    }

    return ret;
}
