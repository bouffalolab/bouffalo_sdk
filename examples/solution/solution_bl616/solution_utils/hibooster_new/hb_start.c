#include <stdio.h>
#include <string.h>
#include "bflb_core.h"
#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"

#include "hb_sender.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#define DBG_TAG "HB_TRANS"
#include "log.h"

volatile uint32_t g_hibooster_total_frame_cnt = 0;

static uint16_t jpeg_out_queue_rtc_id = MJPEG_FRAME_STREAM_WIFI_RTC_ID;

static jpeg_frame_t jpeg_frame_info[4] = { 0 };
static bool frame_valid[4] = { false };

static hb_sender_t *sender_hd = NULL;

/* frame done callback */
static void hb_sender_frame_cb(void *arg)
{
    int idx = (int)(uintptr_t)arg;
    if (idx >= 4) {
        LOG_E("Invalid frame index in callback: %d\r\n", idx);
        return;
    }

    jpeg_frame_t jpeg_frame = jpeg_frame_info[idx];
    frame_valid[idx] = false;

    frame_queue_output_free(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame);

    g_hibooster_total_frame_cnt++;
}

/* sender task loop callback */
static void hb_sender_loop_cb(void *arg)
{
    int ret, idx;
    jpeg_frame_t jpeg_frame = { 0 };

    ret = frame_queue_output_pop(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame, jpeg_out_queue_rtc_id, 0);
    if (ret < 0) {
        return;
    }

    /* check state and buf */
    if (hb_sender_get_state(sender_hd) != HB_SEND_STATE_SEND) {
        goto exit;
    }
    if (hb_send_buf_available(sender_hd) <= 0) {
        goto exit;
    }

    /* get slot */
    for (idx = 0; idx < 4; idx++) {
        if (!frame_valid[idx]) {
            frame_valid[idx] = true;
            jpeg_frame_info[idx] = jpeg_frame;
            break;
        }
    }
    if (idx >= 4) {
        goto exit;
    }

    hb_frame_info_t info = {
        .data = (uint8_t *)jpeg_frame.elem_base.frame_addr,
        .size = jpeg_frame.data_size,
        .width = CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH,
        .height = CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT,
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
    /* Mark frame as done */
    frame_queue_output_free(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame);
    return;
}

/* sender init */
int hb_sender_init(uint16_t local_port)
{
    LOG_I("Initializing HiBooster sender...\r\n");

    if (sender_hd) {
        LOG_W("Already running, stop first\r\n");
        return 0;
    }

    if (local_port == 0) {
        local_port = 8800;
        LOG_I("No port specified, using default local=%u\r\n", local_port);
    } else {
        LOG_I("Using specified local port=%u\r\n", local_port);
    }

    if (frame_queue_output_create(g_jpeg_frame_ctrl, &jpeg_out_queue_rtc_id, MJPEG_FRAME_STREAM_WIFI_RTC_DEPTH) < 0) {
        jpeg_out_queue_rtc_id = FRAME_QUEUE_DEFAULT_ID;
        LOG_E("jpeg frame wifi_rtc out queue create failed\r\n");
        return 0;
    } else {
        LOG_I("jpeg frame wifi_rtc out queue ID: %d\r\n", jpeg_out_queue_rtc_id);
    }

    /* Create sender */
    hb_sender_config_t cfg = {
        .local_port = local_port,
        .loop_cb = hb_sender_loop_cb,
        .loop_cb_arg = NULL,
    };

    sender_hd = hb_sender_create(&cfg);
    if (sender_hd == NULL) {
        frame_queue_output_remove(g_jpeg_frame_ctrl, jpeg_out_queue_rtc_id);
        LOG_E("Failed to create HiBooster sender\r\n");
        return -1;
    }

    LOG_I("HiBooster sender initialized successfully\r\n");
    return 0;
}

/* sender deinit */
int hb_sender_deinit(void)
{
    LOG_I("Deinitializing HiBooster sender...\r\n");
    if (sender_hd == NULL) {
        LOG_W("Not running\r\n");
        return 0;
    }

    hb_sender_destroy(sender_hd);
    sender_hd = NULL;

    frame_queue_output_remove(g_jpeg_frame_ctrl, jpeg_out_queue_rtc_id);

    LOG_I("HiBooster sender deinitialized successfully\r\n");

    return 0;
}

#if IS_ENABLED(CONFIG_SHELL)

static int cmd_hb_sender_start(int argc, char **argv)
{
    uint16_t local_port = 0;

    if (argc >= 2) {
        local_port = atoi(argv[1]);
    }
    hb_sender_init(local_port);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_start, hb_sender_start, Start HiBooster sender);

static int cmd_hb_sender_stop(int argc, char **argv)
{
    hb_sender_deinit();
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_stop, hb_sender_stop, Stop HiBooster sender);

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
SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_status, hb_sender_status, Show HiBooster status);

#endif