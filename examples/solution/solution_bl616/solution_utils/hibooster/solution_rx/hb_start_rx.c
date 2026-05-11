#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bflb_core.h"

#if IS_ENABLED(CONFIG_SHELL)
#include "shell.h"
#endif

#include "frame_queue.h"
#include "frame_queue_ctrl.h"
#include "hb_receiver.h"

#include "hb_start_rx.h"

#define DBG_TAG "HB_RECV"
#include "log.h"

#define HB_RX_DEFAULT_PEER_IP0    192
#define HB_RX_DEFAULT_PEER_IP1    168
#define HB_RX_DEFAULT_PEER_IP2    169
#define HB_RX_DEFAULT_PEER_IP3    1
#define HB_RX_DEFAULT_LOCAL_PORT  9000
#define HB_RX_DEFAULT_PEER_PORT   8800
#define HB_RX_DEFAULT_FRAME_DEPTH 4

typedef struct {
    hb_receiver_t *receiver;
    jpeg_frame_t slots[HB_RX_DEFAULT_FRAME_DEPTH];
    bool frame_valid[HB_RX_DEFAULT_FRAME_DEPTH];
} hb_recv_ctx_t;

static hb_recv_ctx_t s_ctx = { 0 };

volatile uint32_t g_hb_rx_total_frame_cnt = 0;

/* frame done callback */
static void hb_recv_frame_cb(hb_recv_buf_desc_t *cb_desc)
{
    int idx = (int)(uintptr_t)cb_desc->user_arg;
    if ((idx >= HB_RX_DEFAULT_FRAME_DEPTH) || (s_ctx.frame_valid[idx] == false)) {
        LOG_E("Invalid slot index in callback: %d\r\n", idx);
        return;
    }

    jpeg_frame_t *slot = &s_ctx.slots[idx];
    s_ctx.frame_valid[idx] = false;

    if ((cb_desc->status == HB_RECV_STATUS_OK) &&
        (cb_desc->received_size > 0U) &&
        (cb_desc->received_size <= slot->elem_base.frame_size)) {
        slot->data_size = cb_desc->received_size;

        for (uint16_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
            frame_queue_push(g_mjpeg_in_frame_ctrl, (frame_elem_t *)slot, i, 0);
        }
    } else {
        LOG_W("Received frame error or empty, status=%u size=%u\r\n",
              (unsigned int)cb_desc->status,
              (unsigned int)cb_desc->received_size);
    }

    frame_queue_free_unlock(g_mjpeg_in_frame_ctrl, (frame_elem_t *)slot);

    g_hb_rx_total_frame_cnt++;
}

/* Receiver loop callback */
static void hb_recv_loop_cb(void *arg)
{
    int ret, idx;
    jpeg_frame_t *slot = NULL;

    if (s_ctx.receiver == NULL) {
        return;
    }

    /* check state and buf */
    if (hb_receiver_get_state(s_ctx.receiver) != HB_RECV_STATE_RECEIVING) {
        return;
    }
    if (hb_recv_buf_available(s_ctx.receiver) <= 0U) {
        return;
    }

    /* get idle slot */
    for (idx = 0; idx < HB_RX_DEFAULT_FRAME_DEPTH; idx++) {
        if (!s_ctx.frame_valid[idx]) {
            slot = &s_ctx.slots[idx];
            s_ctx.frame_valid[idx] = true;
            break;
        }
    }
    if (idx >= HB_RX_DEFAULT_FRAME_DEPTH) {
        return;
    }

    ret = frame_queue_alloc_lock(g_mjpeg_in_frame_ctrl, (frame_elem_t *)slot, 0);
    if (ret != 0) {
        s_ctx.frame_valid[idx] = false;
        return;
    }

    hb_recv_buf_desc_t desc = {
        .data = (uint8_t *)slot->elem_base.frame_addr,
        .size = slot->elem_base.frame_size,
        .user_arg = (void *)(uintptr_t)idx,
        .on_complete = hb_recv_frame_cb,
    };

    ret = hb_recv_buf_push(s_ctx.receiver, &desc);
    if (ret != 0) {
        LOG_E("hb_recv_buf_push failed: %d\r\n", ret);
        s_ctx.frame_valid[idx] = false;
        frame_queue_free_unlock(g_mjpeg_in_frame_ctrl, (frame_elem_t *)slot);
    }

    return;
}

/* Init receiver, local_port=0 means default port, peer_ip0~3 all 0 means default peer ip */
int hb_recv_init(uint16_t local_port,
                 uint8_t peer_ip0,
                 uint8_t peer_ip1,
                 uint8_t peer_ip2,
                 uint8_t peer_ip3,
                 uint16_t peer_port)
{
    if (s_ctx.receiver) {
        LOG_W("HiBooster receiver already initialized\r\n");
        return 0;
    }

    if (local_port == 0U) {
        local_port = HB_RX_DEFAULT_LOCAL_PORT;
    }
    if (peer_port == 0U) {
        peer_port = HB_RX_DEFAULT_PEER_PORT;
    }

    /* Create receiver */
    hb_recv_config_t cfg = {
        .local_port = local_port,
        .peer_port = peer_port,
        .frame_depth = HB_RX_DEFAULT_FRAME_DEPTH,
        .ack_interval_ms = 15U,
        .frame_timeout_ms = 2000U,
        .task_priority = 20,
        .loop_cb = hb_recv_loop_cb,
        .loop_cb_arg = NULL,
    };
    if ((peer_ip0 == 0U) && (peer_ip1 == 0U) && (peer_ip2 == 0U) && (peer_ip3 == 0U)) {
        IP_ADDR4(&cfg.peer_addr,
                 HB_RX_DEFAULT_PEER_IP0,
                 HB_RX_DEFAULT_PEER_IP1,
                 HB_RX_DEFAULT_PEER_IP2,
                 HB_RX_DEFAULT_PEER_IP3);
    } else {
        IP_ADDR4(&cfg.peer_addr,
                 peer_ip0,
                 peer_ip1,
                 peer_ip2,
                 peer_ip3);
    }

    s_ctx.receiver = hb_receiver_create(&cfg);
    if (s_ctx.receiver == NULL) {
        LOG_E("hb_receiver_create failed\r\n");
        return -1;
    }

    LOG_I("HiBooster receiver initialized local=%u peer=%s:%u\r\n",
          (unsigned int)local_port,
          ipaddr_ntoa(&cfg.peer_addr),
          (unsigned int)cfg.peer_port);

    return 0;
}

/* Deinit receiver */
int hb_recv_deinit(void)
{
    if (s_ctx.receiver == NULL) {
        LOG_W("HiBooster receiver is not initialized\r\n");
        return 0;
    }

    hb_receiver_destroy(s_ctx.receiver);
    s_ctx.receiver = NULL;

    return 0;
}

/* Start receiver, must be called after hb_recv_init */
int hb_recv_start(void)
{
    if (s_ctx.receiver == NULL) {
        LOG_E("HiBooster receiver is not initialized\r\n");
        return -1;
    }

    hb_receiver_start(s_ctx.receiver);

    return 0;
}

/* Stop receiver but not deinit, can be restarted by hb_recv_start */
int hb_recv_stop(void)
{
    if (s_ctx.receiver == NULL) {
        LOG_E("HiBooster receiver is not initialized\r\n");
        return -1;
    }

    hb_receiver_stop(s_ctx.receiver);

    return 0;
}

#if IS_ENABLED(CONFIG_SHELL)

static const char *hb_receiver_state_str(hb_receiver_state_t state)
{
    switch (state) {
        case HB_RECV_STATE_START:
            return "start";
        case HB_RECV_STATE_RECEIVING:
            return "receiving";
        case HB_RECV_STATE_STOP:
            return "stop";
        case HB_RECV_STATE_IDLE:
        default:
            return "idle";
    }
}

static int cmd_hb_recv_start(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    return hb_recv_start();
}

static int cmd_hb_recv_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    return hb_recv_stop();
}

static int cmd_hb_recv_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    LOG_I("=== \033[32m HiBooster Receiver Status \033[0m ===\r\n");
    LOG_I("Created:      %s\r\n", s_ctx.receiver ? "yes" : "no");

    if (s_ctx.receiver == NULL) {
        LOG_I("=====================================\r\n");
        return 0;
    }

    hb_recv_stats_t stats;
    hb_receiver_state_t state = hb_receiver_get_state(s_ctx.receiver);
    hb_receiver_get_statistics(s_ctx.receiver, &stats);
    hb_receiver_clear_statistics(s_ctx.receiver);
    uint32_t fps_avg = (stats.frames_ok + stats.frames_drop) * 1000U / (stats.duration_ms + 1U);
    uint32_t ack_interval = stats.duration_ms / (stats.acks_sent + 1U);
    uint32_t invalid_rate = stats.frags_invalid * 100U / (stats.frags_received + 1U);
    uint32_t speed_avg = (stats.frags_received + stats.frags_invalid) * 1024U / (stats.duration_ms + 1U);

    LOG_I("State:        %s\r\n", hb_receiver_state_str(state));
    LOG_I("Duration:     %lu ms\r\n", (unsigned long)stats.duration_ms);
    LOG_I("FPS AVG:      %lu\r\n", (unsigned long)fps_avg);
    LOG_I("Frames OK:    %lu\r\n", (unsigned long)stats.frames_ok);
    LOG_I("Frames drop:  %lu\r\n", (unsigned long)stats.frames_drop);
    LOG_I("Frames pend:  %lu\r\n", (unsigned long)stats.frames_pending);
    LOG_I("Speed AVG:    %lu KB/s\r\n", (unsigned long)speed_avg);
    LOG_I("Frags recv:   %lu\r\n", (unsigned long)stats.frags_received);
    LOG_I("Invalid pkt:  %lu\r\n", (unsigned long)stats.frags_invalid);
    LOG_I("Invalid rate: %lu%%\r\n", (unsigned long)invalid_rate);
    LOG_I("ACKs sent:    %lu\r\n", (unsigned long)stats.acks_sent);
    LOG_I("ACK interval: %lu ms\r\n", (unsigned long)ack_interval);
    LOG_I("=====================================\r\n");

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_hb_recv_start, hb_recv_start, Start HiBooster receiver);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_recv_stop, hb_recv_stop, Stop HiBooster receiver);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_recv_status, hb_recv_status, Show HiBooster receiver status);
#endif
