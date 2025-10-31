#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include "stream_buffer.h"
#include <event_groups.h>

#include <utils_crc.h>
#include <spisync.h>
#include <spisync_config.h>
#include <ramsync.h>
#include <spisync_port.h>
#include <spisync_internal.h>

#include "mem.h"
#include <shell.h>

#define BUF_SIZE                (1500)
#define UNIT_SEND_TIMEOUT       (1000)

typedef struct {
    uint8_t type;
    uint32_t duration; // ms
} loop_params_t;

extern spisync_t *g_spisync_current;
int g_sloop_busy = 0;

static void send_task(void *pvParameters)
{
    spisync_msg_t msg;
    loop_params_t *params = (loop_params_t *)pvParameters;
    uint8_t buffer[BUF_SIZE];
    uint8_t seq = 1;
    TickType_t start_time = xTaskGetTickCount();
    TickType_t end_time = start_time + pdMS_TO_TICKS(params->duration);

    while (xTaskGetTickCount() < end_time) {
        // 1500
        seq++;
        memset(buffer, seq, BUF_SIZE);

        SPISYNC_MSGINIT(&msg,
                    params->type,       /* type */
                    1,                  /* copy */
                    UNIT_SEND_TIMEOUT,  /* timeout */
                    buffer, BUF_SIZE,      /* buf buf_len */
                    NULL,               /* cb */
                    NULL);              /* cb_arg */
        printf("Sending %d bytes of value 0x%02X\n", BUF_SIZE, seq);
        int ret = spisync_write(g_spisync_current, &msg, 0);
        if (ret != BUF_SIZE) {
            printf("spisync_write ret:%d, drop it.\r\n", ret);
        }

        // vTaskDelay(pdMS_TO_TICKS(100));
    }

    printf("Send task completed.\n");
    g_sloop_busy &= 0xFE;
    vTaskDelete(NULL);
}

static void receive_task(void *pvParameters)
{
    spisync_msg_t msg;
    loop_params_t *params = (loop_params_t *)pvParameters;
    uint8_t buffer[BUF_SIZE];
    uint8_t expected_seq = 1;
    TickType_t start_time = xTaskGetTickCount();
    TickType_t end_time = start_time + pdMS_TO_TICKS(params->duration);

    while (xTaskGetTickCount() < end_time) {
        expected_seq++;

        SPISYNC_MSGINIT(&msg,
                    params->type,       /* type */
                    0,                  /* copy */
                    UNIT_SEND_TIMEOUT,  /* timeout */
                    buffer, BUF_SIZE,      /* buf buf_len */
                    NULL,               /* cb */
                    NULL);              /* cb_arg */
        int res = spisync_read(g_spisync_current, &msg, 0);
        if (res <=0) {
            printf("low read 0x%02X, res:%d\r\n", buffer[100], res);
            continue;
        }

        // compare seq
        for (int i = 0; i < BUF_SIZE; i++) {
            if (buffer[i] != expected_seq) {
                printf("Data mismatch: expected 0x%02X, got 0x%02X\n", expected_seq, buffer[i]);
                break;
            }
        }

        printf("Received %d bytes of value 0x%02X\n", BUF_SIZE, expected_seq);
    }

    printf("Receive task completed.\n");
    g_sloop_busy &= 0xEF;
    vTaskDelete(NULL);
}

int cmd_ss_loop(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: ss_loop [type 0-5] [-t time_s]\n");
        return -1;
    }

    uint8_t type = atoi(argv[1]);
    if (type < 0 || type > 5) {
        printf("Invalid type. Must be between 0 and 5.\n");
        return -1;
    }

    uint32_t time_limit = 10000; // default 10s
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            time_limit = atoi(argv[++i]) * 1000; // ms
        }
    }

    loop_params_t params;
    params.type = type;
    params.duration = time_limit;

    xTaskCreate(send_task, "send_task", 2048, &params, 5, NULL);
    g_sloop_busy |= 0x01;

    xTaskCreate(receive_task, "receive_task", 2048, &params, 5, NULL);
    g_sloop_busy |=  0x10;

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ss_loop, ss_loop, ss_loop [type 0-5] [-t time_s]);

