/**
 * @file hb_sender_test.c
 * @brief HiBooster sender test with static color bar image
 * 
 * Shell commands:
 *   hb_sender_test_start <local_port>             - Start test sender
 *   hb_sender_test_stop                            - Stop test sender
 *   hb_sender_test_status                          - Show test status
 */

#include <stdio.h>
#include <string.h>
#include "bflb_core.h"
#include "FreeRTOS.h"
#include "task.h"

#include "hb_sender.h"

#define DBG_TAG "HB_SENDER_TEST"
#include "log.h"

#define TEST_IMG_COUNT 4

/* incbin */
#define INCBIN_STYLE   INCBIN_STYLE_SNAKE
#include "incbin.h"
/* test img */
INCBIN(uint8_t, _img_test_1, "./test_img/img_test_1.jpeg");
INCBIN(uint8_t, _img_test_2, "./test_img/img_test_2.jpeg");
INCBIN(uint8_t, _img_test_3, "./test_img/img_test_3.jpeg");
INCBIN(uint8_t, _img_test_4, "./test_img/img_test_4.jpeg");

static const uint8_t *img_test[TEST_IMG_COUNT + 1] = { g_img_test_1_data, g_img_test_2_data, g_img_test_3_data, g_img_test_4_data, NULL };
static const unsigned int *img_test_size[TEST_IMG_COUNT + 1] = { &g_img_test_1_size, &g_img_test_2_size, &g_img_test_3_size, &g_img_test_4_size, NULL };

/* Color bar image info */
#define TEST_IMG_WIDTH   640
#define TEST_IMG_HEIGHT  480
#define TEST_IMG_QUALITY 50

/* Test context */
static struct {
    hb_sender_t *sender;
    TaskHandle_t test_task;
    bool running;
    uint32_t current_img_idx;  /* Current image index (0-3) */
    uint32_t last_switch_time; /* Last image switch time (ms) */
    int32_t fixed_img_idx;     /* Fixed image index (-1 = rotate, 0-3 = fixed) */
    uint32_t max_fps;          /* Max frame rate (0 = unlimited) */
} g_test_ctx = { .fixed_img_idx = -1, .max_fps = 30 };

static const char *sender_state_str(hb_sender_state_t state)
{
    switch (state) {
        case HB_SEND_STATE_SEND:
            return "send";
        case HB_SEND_STATE_STOP:
            return "stop";
        case HB_SEND_STATE_IDLE:
        default:
            return "idle";
    }
}

static void test_frame_cb(void *arg)
{
    (void)arg;
}

/* Test task: push frames periodically */
static void test_task_func(void *arg)
{
    int ret;
    hb_sender_t *s = (hb_sender_t *)arg;

    LOG_I("Test task started, waiting for receiver...\r\n");

    while (g_test_ctx.running) {
        if (hb_send_buf_available(s) == 0) {
            vTaskDelay(pdMS_TO_TICKS(2));
            continue; // Skip if sender queue is full
        }

        /* Select image index */
        uint32_t img_idx;
        if (g_test_ctx.fixed_img_idx >= 0) {
            /* Fixed mode: use specified image */
            g_test_ctx.current_img_idx = g_test_ctx.fixed_img_idx % TEST_IMG_COUNT;
        } else {
            /* Rotate mode: switch image every 10 seconds */
            uint32_t now = bflb_mtimer_get_time_ms();
            if (now - g_test_ctx.last_switch_time >= 10000) {
                g_test_ctx.current_img_idx = (g_test_ctx.current_img_idx + 1) % TEST_IMG_COUNT;
                g_test_ctx.last_switch_time = now;
                LOG_I("Switched to image %lu\r\n", (unsigned long)(g_test_ctx.current_img_idx + 1));
            }
        }

        img_idx = g_test_ctx.current_img_idx;

        /* Create frame info - use test image data */
        hb_frame_info_t info = {
            .data = (uint8_t *)img_test[img_idx],
            .size = *img_test_size[img_idx],
            .width = TEST_IMG_WIDTH,
            .height = TEST_IMG_HEIGHT,
            .quality = TEST_IMG_QUALITY,
            .seq_total = 0, // Auto-increment
            .release = test_frame_cb,
            .release_arg = NULL,
        };

        ret = hb_sender_push(s, &info);
        if (ret < 0) {
            LOG_W("Failed to push frame: %d\r\n", ret);
            break;
        }

        /* Frame rate control */
        if (g_test_ctx.max_fps > 0) {
            uint32_t delay_ms = 1000 / g_test_ctx.max_fps - 1;
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
        }
    }

    LOG_I("Task stopped\r\n");
    g_test_ctx.test_task = NULL;
    vTaskDelete(NULL);
}

/* User message callback */
// static void user_msg_callback(hb_sender_t *s, const uint8_t *data,
//                               uint16_t len, void *arg)
// {
//     LOG_I("User message received: %u bytes\r\n", len);
//     /* Could process control commands from receiver here */
// }

#if IS_ENABLED(CONFIG_SHELL)
#include "shell.h"

/**
 * @brief Shell command: Start test
 * Usage: hb_sender_test_start <local_port>
 * Example: hb_sender_test_start 8800
 */
static int cmd_hb_sender_test_start(int argc, char **argv)
{
    uint16_t local_port = 0;

    if (g_test_ctx.sender != NULL) {
        LOG_W("Already running, stop first\r\n");
        return -1;
    }

    if (argc == 1) {
        local_port = 8800;
        LOG_I("No port specified, using default local=%u\r\n", local_port);
    } else if (argc == 2) {
        local_port = atoi(argv[1]);
    }

    if (local_port == 0) {
        LOG_E("Invalid port number\r\n");
        LOG_I("Usage: hb_sender_test_start <local_port>\r\n");
        LOG_I("Example: hb_sender_test_start 8800\r\n");
        return -1;
    }

    LOG_I("Creating sender: local=%u\r\n", local_port);

    LOG_I("Test img size:\r\n");
    for (int i = 0; i < TEST_IMG_COUNT; i++) {
        LOG_I("  img_test_%d: %u bytes\r\n", i + 1, *img_test_size[i]);
    }

    /* Create sender */
    hb_sender_config_t cfg = {
        .local_port = local_port,
    };

    g_test_ctx.sender = hb_sender_create(&cfg);
    if (g_test_ctx.sender == NULL) {
        LOG_E("Failed to create sender\r\n");
        return -1;
    }

    /* Register user message callback */
    // hb_sender_on_msg(g_test_ctx.sender, user_msg_callback, NULL);

    /* Initialize context */
    g_test_ctx.current_img_idx = 0;
    g_test_ctx.last_switch_time = bflb_mtimer_get_time_ms();
    g_test_ctx.running = true;

    /* Create test task */
    BaseType_t ret = xTaskCreate(test_task_func, "hb_sender_test", 1536 / sizeof(StackType_t),
                                 g_test_ctx.sender, 20, &g_test_ctx.test_task);
    if (ret != pdPASS) {
        LOG_E("Failed to create test task\r\n");
        hb_sender_destroy(g_test_ctx.sender);
        g_test_ctx.sender = NULL;
        g_test_ctx.running = false;
        return -1;
    }

    LOG_I("Started successfully\r\n");
    LOG_I("Waiting for START from receiver...\r\n");

    return 0;
}

/**
 * @brief Shell command: Stop test
 */
static int cmd_hb_sender_test_stop(int argc, char **argv)
{
    if (g_test_ctx.sender == NULL) {
        LOG_W("Not running\r\n");
        return -1;
    }

    LOG_I("Stopping...\r\n");

    /* Stop test task first */
    g_test_ctx.running = false;

    /* Wait for task self-exit to avoid concurrent access during sender destroy. */
    for (uint32_t i = 0; i < 100 && g_test_ctx.test_task != NULL; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    /* Timeout: do not continue if producer task is still alive. */
    if (g_test_ctx.test_task != NULL) {
        LOG_E("Failed to stop test task in time\r\n");
        return -1;
    }

    /* Destroy sender */
    hb_sender_destroy(g_test_ctx.sender);
    g_test_ctx.sender = NULL;
    LOG_I("Stopped\r\n");

    return 0;
}

/**
 * @brief Shell command: Show status
 */
static int cmd_hb_sender_test_status(int argc, char **argv)
{
    if (g_test_ctx.sender == NULL) {
        LOG_I("Not running\r\n");
        return 0;
    }

    hb_sender_stats_t stats;
    hb_sender_state_t state = hb_sender_get_state(g_test_ctx.sender);
    hb_sender_get_stats(g_test_ctx.sender, &stats);

    /* Clear statistics after display */
    hb_sender_clear_stats(g_test_ctx.sender);

    LOG_I("=== \033[32m HiBooster Test Status \033[0m ===\r\n");
    LOG_I("State:        %s\r\n", sender_state_str(state));
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

    if (g_test_ctx.fixed_img_idx >= 0) {
        LOG_I("Fixed img:    %lu\r\n", (unsigned long)(g_test_ctx.fixed_img_idx + 1));
    } else {
        LOG_I("Current img:  %lu (rotate)\r\n", (unsigned long)(g_test_ctx.current_img_idx + 1));
    }

    if (g_test_ctx.max_fps > 0) {
        LOG_I("Max FPS:      %lu (delay %lu ms)\r\n", (unsigned long)g_test_ctx.max_fps, (unsigned long)(1000 / g_test_ctx.max_fps));
    } else {
        LOG_I("Max FPS:      unlimited (no delay)\r\n");
    }
    LOG_I("=============================\r\n");

    return 0;
}

/**
 * @brief Shell command: Set test image
 * Usage: hb_sender_test_img <img_idx>
 *   0 = rotate through all 4 images (default)
 *   1-4 = use specific image
 * Example: hb_sender_test_img 1
 */
static int cmd_hb_sender_test_img(int argc, char **argv)
{
    if (argc != 2) {
        LOG_I("Usage: hb_sender_test_img <img_idx>\r\n");
        LOG_I("  0 = rotate through all 4 images (default)\r\n");
        LOG_I("  1-4 = use specific image\r\n");
        LOG_I("Current setting: ");
        if (g_test_ctx.fixed_img_idx < 0) {
            LOG_I("rotate (current img %lu)\r\n", (unsigned long)(g_test_ctx.current_img_idx + 1));
        } else {
            LOG_I("fixed img %lu\r\n", (unsigned long)(g_test_ctx.fixed_img_idx + 1));
        }
        return 0;
    }

    int img_idx = atoi(argv[1]);

    if (img_idx == 0) {
        /* Rotate mode */
        g_test_ctx.fixed_img_idx = -1;
        LOG_I("Set to rotate mode (cycle through all 4 images)\r\n");
        LOG_I("Test img size:\r\n");
        for (int i = 0; i < TEST_IMG_COUNT; i++) {
            LOG_I("  img_test_%d: %u bytes\r\n", i + 1, *img_test_size[i]);
        }
    } else if (img_idx <= TEST_IMG_COUNT) {
        /* Fixed mode */
        g_test_ctx.fixed_img_idx = img_idx - 1;
        LOG_I("Set to fixed image %d (%u bytes)\r\n", img_idx, *img_test_size[img_idx - 1]);
    } else {
        LOG_E("Invalid image index: %d (must be 0-%d)\r\n", img_idx, TEST_IMG_COUNT);
        return -1;
    }

    return 0;
}

/**
 * @brief Shell command: Set max frame rate
 * Usage: hb_sender_test_fps <fps>
 *   0 = unlimited (no delay)
 *   1-60 = max frame rate
 * Example: hb_sender_test_fps 30
 */
static int cmd_hb_sender_test_fps(int argc, char **argv)
{
    if (argc != 2) {
        LOG_I("Usage: hb_sender_test_fps <fps>\r\n");
        LOG_I("  0 = unlimited (no delay)\r\n");
        LOG_I("  1-100 = max frame rate (FPS)\r\n");
        return 0;
    }

    uint32_t fps = (uint32_t)atoi(argv[1]);

    if (fps > 100) {
        LOG_E("Invalid FPS: %lu (must be 0-60)\r\n", (unsigned long)fps);
        return -1;
    }

    g_test_ctx.max_fps = fps;

    if (fps == 0) {
        LOG_I("Set to unlimited frame rate (no delay)\r\n");
    } else {
        LOG_I("Set max frame rate to %lu FPS\r\n", (unsigned long)fps);
    }

    return 0;
}

/* Export shell commands */
SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_test_start, hb_sender_test_start, Start HiBooster test sender);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_test_stop, hb_sender_test_stop, Stop HiBooster test sender);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_test_status, hb_sender_test_status, Show HiBooster test status);
SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_test_img, hb_sender_test_img, Set test image(0 = rotate, 1 - 4 = fixed));
SHELL_CMD_EXPORT_ALIAS(cmd_hb_sender_test_fps, hb_sender_test_fps, Set max frame rate(0 = unlimited, 1 - 100 = FPS));
#endif
