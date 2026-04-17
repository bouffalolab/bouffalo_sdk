/**
 * @file hb_receiver_test.c
 * @brief HiBooster receiver test helper
 *
 * Shell commands:
 *   hb_recv_test_create  <local_port> <peer_ip> <peer_port>
 *   hb_recv_test_destroy
 *   hb_recv_test_start
 *   hb_recv_test_stop
 *   hb_recv_test_status
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bflb_core.h"
#include "FreeRTOS.h"
#include "task.h"
#if IS_ENABLED(CONFIG_SHELL)
#include "shell.h"
#endif

#include "hb_receiver.h"

#define DBG_TAG "HB_RECV_TEST"
#include "log.h"

#define TEST_DEFAULT_BUF_SIZE   (200U * 1024U)
#define TEST_PREALLOC_BUF_COUNT 4U

static struct {
    hb_receiver_t *receiver;
    volatile bool test_started;

    uint16_t local_port;
    ip_addr_t peer_addr;
    uint16_t peer_port;
} g_test_ctx = { 0 };

static int parse_ipv4_addr(const char *str, ip_addr_t *addr)
{
    unsigned int a, b, c, d;

    if (!str || !addr) {
        return -1;
    }

    if (sscanf(str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4) {
        return -1;
    }

    if (a > 255 || b > 255 || c > 255 || d > 255) {
        return -1;
    }

    IP_ADDR4(addr, a, b, c, d);
    return 0;
}

static const char *receiver_state_str(hb_receiver_state_t state)
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

static void on_frame_complete(hb_recv_buf_desc_t *desc);

static int push_test_buffer(uint8_t *buf)
{
    hb_recv_buf_desc_t push_desc = {
        .data = buf,
        .size = TEST_DEFAULT_BUF_SIZE,
        .on_complete = on_frame_complete,
    };

    if (!g_test_ctx.receiver || !buf) {
        return -1;
    }

    return hb_recv_buf_push(g_test_ctx.receiver, &push_desc);
}

static void on_frame_complete(hb_recv_buf_desc_t *desc)
{
    if (!desc || !desc->data) {
        return;
    }

    if (g_test_ctx.receiver) {
        if (push_test_buffer(desc->data) == 0) {
            return;
        }

        LOG_W("%s failed to re-push buffer, free it\r\n", __func__);
    }

    free(desc->data);
}

#if IS_ENABLED(CONFIG_SHELL)
#include "shell.h"

/* hb_recv_test_create <local_port> <peer_ip> <peer_port> */
static int cmd_hb_recv_test_create(int argc, char **argv)
{
    ip_addr_t peer_addr;
    uint16_t local_port;
    uint16_t peer_port;

    if (g_test_ctx.receiver) {
        LOG_W("Receiver already created\r\n");
        return -1;
    }

    if (argc < 4) {
        LOG_E("Usage: hb_recv_test_create <local_port> <peer_ip> <peer_port>\r\n");
        LOG_E("Example: hb_recv_test_create 9000 192.168.1.100 8800\r\n");
        return -1;
    }

    local_port = (uint16_t)atoi(argv[1]);
    peer_port = (uint16_t)atoi(argv[3]);

    if (local_port == 0 || peer_port == 0 || parse_ipv4_addr(argv[2], &peer_addr) != 0) {
        LOG_E("Invalid create parameters\r\n");
        return -1;
    }

    hb_recv_config_t cfg = {
        .local_port = local_port,
        .peer_port = peer_port,
        .peer_addr = peer_addr,
        .frame_depth = 4,
        .ack_interval_ms = 20,
        .frame_timeout_ms = 2000,
        .loop_cb = NULL,
        .loop_cb_arg = NULL,
    };

    g_test_ctx.receiver = hb_receiver_create(&cfg);
    if (!g_test_ctx.receiver) {
        LOG_E("hb_receiver_create failed\r\n");
        return -1;
    }

    g_test_ctx.test_started = false;

    for (uint32_t i = 0; i < TEST_PREALLOC_BUF_COUNT; i++) {
        uint8_t *buf = malloc(TEST_DEFAULT_BUF_SIZE);
        if (!buf) {
            LOG_E("%s failed to alloc prealloc buffer %lu\r\n", __func__, (unsigned long)i);
            hb_receiver_destroy(g_test_ctx.receiver);
            g_test_ctx.receiver = NULL;
            return -1;
        }

        if (push_test_buffer(buf) < 0) {
            LOG_E("%s failed to push prealloc buffer %lu\r\n", __func__, (unsigned long)i);
            free(buf);
            hb_receiver_destroy(g_test_ctx.receiver);
            g_test_ctx.receiver = NULL;
            return -1;
        }
    }

    g_test_ctx.local_port = local_port;
    g_test_ctx.peer_addr = peer_addr;
    g_test_ctx.peer_port = peer_port;

    LOG_I("Receiver created: local=%u, peer=%s:%u\r\n",
          local_port, ipaddr_ntoa(&g_test_ctx.peer_addr), peer_port);

    return 0;
}

/* hb_recv_test_destroy */
static int cmd_hb_recv_test_destroy(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!g_test_ctx.receiver) {
        LOG_W("Receiver test is not created\r\n");
        return -1;
    }

    g_test_ctx.test_started = false;

    if (g_test_ctx.receiver) {
        hb_receiver_stop(g_test_ctx.receiver);
        hb_receiver_destroy(g_test_ctx.receiver);
        g_test_ctx.receiver = NULL;
    }

    LOG_I("Receiver test destroyed\r\n");
    return 0;
}

/* hb_recv_test_start */
static int cmd_hb_recv_test_start(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!g_test_ctx.receiver) {
        LOG_W("Receiver not created, run hb_recv_test_create first\r\n");
        return -1;
    }

    if (g_test_ctx.test_started) {
        LOG_W("Receiver test already started\r\n");
        return 0;
    }

    if (hb_receiver_start(g_test_ctx.receiver) != 0) {
        LOG_E("hb_receiver_start failed\r\n");
        return -1;
    }

    hb_receiver_clear_statistics(g_test_ctx.receiver);

    g_test_ctx.test_started = true;

    LOG_I("Receiver test started\r\n");
    return 0;
}

/* hb_recv_test_stop */
static int cmd_hb_recv_test_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!g_test_ctx.receiver) {
        LOG_W("Receiver not created\r\n");
        return -1;
    }

    g_test_ctx.test_started = false;

    if (hb_receiver_stop(g_test_ctx.receiver) != 0) {
        LOG_E("hb_receiver_stop failed\r\n");
        return -1;
    }

    LOG_I("Receiver test stopped\r\n");
    return 0;
}

/* hb_recv_test_status */
static int cmd_hb_recv_test_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    LOG_I("=== \033[32m HiBooster Receiver Status \033[0m ===\r\n");
    LOG_I("Created:      %s\r\n", g_test_ctx.receiver ? "yes" : "no");
    LOG_I("Test started: %s\r\n", g_test_ctx.test_started ? "yes" : "no");

    if (!g_test_ctx.receiver) {
        LOG_I("=====================================\r\n");
        return 0;
    }

    hb_recv_stats_t stats;
    hb_receiver_state_t state = hb_receiver_get_state(g_test_ctx.receiver);
    hb_receiver_get_statistics(g_test_ctx.receiver, &stats);
    hb_receiver_clear_statistics(g_test_ctx.receiver);
    uint32_t fps_avg = (stats.frames_ok + stats.frames_drop) * 1000U / (stats.duration_ms + 1);
    uint32_t ack_interval = stats.duration_ms / (stats.acks_sent + 1);
    uint32_t invalid_rate = stats.frags_invalid * 100U / (stats.frags_received + 1);

    LOG_I("State:        %s\r\n", receiver_state_str(state));
    LOG_I("Config:       local=%u peer=%s:%u\r\n", g_test_ctx.local_port, ipaddr_ntoa(&g_test_ctx.peer_addr), g_test_ctx.peer_port);
    LOG_I("Duration:     %lu ms\r\n", (unsigned long)stats.duration_ms);
    LOG_I("FPS AVG:      %lu\r\n", (unsigned long)fps_avg);
    LOG_I("Frames OK:    %lu\r\n", (unsigned long)stats.frames_ok);
    LOG_I("Frames drop:  %lu\r\n", (unsigned long)stats.frames_drop);
    LOG_I("Frames pend:  %lu\r\n", (unsigned long)stats.frames_pending);
    LOG_I("Speed AVG:    %lu KB/s\r\n", (unsigned long)((stats.frags_received + stats.frags_invalid) * 1024 / (stats.duration_ms + 1)));
    LOG_I("Frags recv:   %lu\r\n", (unsigned long)stats.frags_received);
    LOG_I("Invalid pkt:  %lu\r\n", (unsigned long)stats.frags_invalid);
    LOG_I("Invalid rate: %lu%%\r\n", (unsigned long)invalid_rate);
    LOG_I("ACKs sent:    %lu\r\n", (unsigned long)stats.acks_sent);
    LOG_I("ACK interval: %lu ms\r\n", (unsigned long)ack_interval);

    LOG_I("=====================================\r\n");

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_hb_recv_test_create, hb_recv_test_create, Create receiver test : hb_recv_test_create<local_port><peer_ip><peer_port>);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_recv_test_destroy, hb_recv_test_destroy, Destroy receiver test instance);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_recv_test_start, hb_recv_test_start, Start receiver test feeding);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_recv_test_stop, hb_recv_test_stop, Stop receiver test feeding);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_recv_test_status, hb_recv_test_status, Show receiver test status and stats);
#endif
