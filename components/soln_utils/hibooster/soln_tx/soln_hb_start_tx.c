#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bflb_core.h"
#include "bflb_irq.h"
#include "FreeRTOS.h"
#include "task.h"

#include "hb_sender.h"

#include "soln_fbq.h"
#include "soln_hb_start_tx.h"

#ifdef CONFIG_SOLN_HB_TX_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_HB_TX_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_HB_TRANS"
#include "log.h"

#define HB_TX_DEFAULT_LOCAL_PORT  8800

#define HB_TX_DEFAULT_FRAME_DEPTH 4

static uint32_t s_hb_tx_total_frame_count = 0;

uint32_t soln_hb_sender_get_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    uint32_t frame_count = s_hb_tx_total_frame_count;
    bflb_irq_restore(irq_flags);

    return frame_count;
}

void soln_hb_sender_clear_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_hb_tx_total_frame_count = 0;
    bflb_irq_restore(irq_flags);
}

static void hb_sender_record_frame(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_hb_tx_total_frame_count++;
    bflb_irq_restore(irq_flags);
}

static uint16_t local_jpeg_output_net_id = CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NET_ID;

static fbq_elem_t *jpeg_frame_info[HB_TX_DEFAULT_FRAME_DEPTH] = { 0 };
static bool frame_valid[HB_TX_DEFAULT_FRAME_DEPTH] = { false };

static hb_sender_t *sender_hd = NULL;

/* frame done callback */
static void hb_sender_frame_cb(void *arg)
{
    int idx = (int)(uintptr_t)arg;
    if (idx >= HB_TX_DEFAULT_FRAME_DEPTH || frame_valid[idx] == false) {
        LOG_E("Invalid frame index in callback: %d\r\n", idx);
        return;
    }

    fbq_elem_t *jpeg_frame = jpeg_frame_info[idx];
    frame_valid[idx] = false;
    jpeg_frame_info[idx] = NULL;

    fbq_free(jpeg_frame);

    hb_sender_record_frame();
}

/* sender task loop callback */
static void hb_sender_loop_cb(void *arg)
{
    int ret, idx;
    fbq_elem_t *jpeg_frame = NULL;

    ret = fbq_pop(soln_fbq_vid_jpeg_local(), &jpeg_frame, local_jpeg_output_net_id, 0);
    if (ret != FBQ_OK) {
        return;
    }

    /* check state and buf */
    if (hb_sender_get_state(sender_hd) != HB_SEND_STATE_SEND) {
        goto exit;
    }
    if (hb_send_buf_available(sender_hd) <= 0) {
        goto exit;
    }

    /* get idle slot */
    for (idx = 0; idx < HB_TX_DEFAULT_FRAME_DEPTH; idx++) {
        if (!frame_valid[idx]) {
            frame_valid[idx] = true;
            jpeg_frame_info[idx] = jpeg_frame;
            break;
        }
    }
    if (idx >= HB_TX_DEFAULT_FRAME_DEPTH) {
        goto exit;
    }

    hb_frame_info_t info = {
        .data = (uint8_t *)jpeg_frame->data,
        .size = jpeg_frame->size,
        .width = CONFIG_SOLN_VID_DEFAULT_WIDTH,
        .height = CONFIG_SOLN_VID_DEFAULT_HEIGHT,
        .quality = 50,
        .release = hb_sender_frame_cb,
        .release_arg = (void *)(uintptr_t)idx,
    };
    ret = hb_sender_push(sender_hd, &info);
    if (ret < 0) {
        frame_valid[idx] = false;
        goto exit;
    }

    return;

exit:
    fbq_free(jpeg_frame);
    return;
}

/* sender init */
int soln_hb_sender_init(uint16_t local_port)
{
    LOG_I("Initializing HiBooster sender...\r\n");

    if (sender_hd) {
        LOG_W("Already running, stop first\r\n");
        return -1;
    }

    if (local_port == 0) {
        local_port = HB_TX_DEFAULT_LOCAL_PORT;
        LOG_I("No port specified, using default local=%u\r\n", local_port);
    } else {
        LOG_I("Using specified local port=%u\r\n", local_port);
    }

    if (fbq_output_open(soln_fbq_vid_jpeg_local(), &local_jpeg_output_net_id,
                        CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NET_DEPTH,
                        SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_LOCAL_ALL) != FBQ_OK) {
        local_jpeg_output_net_id = FBQ_OUTPUT_AUTO;
        LOG_E("jpeg frame wifi_rtc out queue create failed\r\n");
        return -1;
    } else {
        LOG_I("local JPEG network output ID: %d\r\n", local_jpeg_output_net_id);
    }

    /* Create sender */
    hb_sender_config_t cfg = {
        .local_port = local_port,
        .loop_cb = hb_sender_loop_cb,
        .task_priority = 20,
        .loop_cb_arg = NULL,
    };

    sender_hd = hb_sender_create(&cfg);
    if (sender_hd == NULL) {
        (void)fbq_output_close(soln_fbq_vid_jpeg_local(), local_jpeg_output_net_id);
        LOG_E("Failed to create HiBooster sender\r\n");
        return -1;
    }

    LOG_I("HiBooster sender initialized successfully\r\n");
    return 0;
}

/* sender deinit */
int soln_hb_sender_deinit(void)
{
    LOG_I("Deinitializing HiBooster sender...\r\n");
    if (sender_hd == NULL) {
        LOG_W("Not running\r\n");
        return 0;
    }

    hb_sender_destroy(sender_hd);
    sender_hd = NULL;

    (void)fbq_output_close(soln_fbq_vid_jpeg_local(), local_jpeg_output_net_id);
    local_jpeg_output_net_id = FBQ_OUTPUT_AUTO;

    LOG_I("HiBooster sender deinitialized successfully\r\n");

    return 0;
}

#if IS_ENABLED(CONFIG_SHELL)
#include "shell.h"

static int cmd_hb_sender_start(int argc, char **argv)
{
    uint16_t local_port = 0;

    if (argc >= 2) {
        local_port = atoi(argv[1]);
    }
    soln_hb_sender_init(local_port);

    return 0;
}

static int cmd_hb_sender_stop(int argc, char **argv)
{
    soln_hb_sender_deinit();
    return 0;
}

static int cmd_hb_sender_status(int argc, char **argv)
{
    if (sender_hd == NULL) {
        LOG_I("HiBooster sender is not initialized\r\n");
        return 0;
    }

    hb_sender_state_t state = hb_sender_get_state(sender_hd);
    char *state_str = NULL;
    switch (state) {
        case HB_SEND_STATE_SEND:
            state_str = "send";
            break;
        case HB_SEND_STATE_STOP:
            state_str = "stop";
            break;
        case HB_SEND_STATE_IDLE:
            state_str = "idle";
            break;
        default:
            state_str = "unkown";
            break;
    }

    hb_sender_stats_t stats;
    hb_sender_get_stats(sender_hd, &stats);
    /* Clear statistics after display */
    hb_sender_clear_stats(sender_hd);

    LOG_I("=== \033[32m HiBooster Status \033[0m ===\r\n");
    LOG_I("State:        %s\r\n", state_str);
    LOG_I("Duration:     %lu ms\r\n", (unsigned long)stats.duration_ms);
    LOG_I("FPS AVG:      %lu\r\n", (unsigned long)((stats.frames_sent + stats.frames_dropped) * 1000 / (stats.duration_ms + 1)));
    LOG_I("Sent:         %lu\r\n", (unsigned long)stats.frames_sent);
    LOG_I("Dropped:      %lu\r\n", (unsigned long)stats.frames_dropped);
    LOG_I("Pending:      %lu\r\n", (unsigned long)stats.frames_pending);
    LOG_I("Avg latency:  %lu ms\r\n", (unsigned long)stats.avg_latency_ms);
    LOG_I("Max latency:  %lu ms\r\n", (unsigned long)stats.max_latency_ms);
    LOG_I("Speed AVG:    %lu KB/s\r\n", (unsigned long)((stats.pkts_sent + stats.pkts_resent) * 1024 / (stats.duration_ms + 1)));
    LOG_I("Pkts sent:    %lu\r\n", (unsigned long)stats.pkts_sent);
    LOG_I("Pkts resent:  %lu\r\n", (unsigned long)stats.pkts_resent);
    LOG_I("Resent rate:  %lu%%\r\n", (unsigned long)(stats.pkts_resent * 100 / (stats.pkts_sent + 1)));
    LOG_I("ACKs recv:    %lu\r\n", (unsigned long)stats.acks_received);
    LOG_I("ACK interval: %lu ms\r\n", (unsigned long)(stats.duration_ms / (stats.acks_received + 1)));
    LOG_I("ACK Delay:    %lu ms\r\n", (unsigned long)stats.ack_delay_avg);
    LOG_I("Timeout:      %lu ms\r\n", (unsigned long)stats.timeout_avg);
    LOG_I("=============================\r\n");

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_start, hb_sender_start, Start HiBooster sender);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_stop, hb_sender_stop, Stop HiBooster sender);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_status, hb_sender_status, Show HiBooster status);
#endif
