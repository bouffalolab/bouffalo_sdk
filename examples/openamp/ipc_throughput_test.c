#include "ipc_throughput_test.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include "bflb_common.h"
#include "bflb_mtimer.h"
#include "rpmsg_service.h"

#if defined(CPU_AP)
#define IPC_ROLE "AP"
#else
#define IPC_ROLE "NP"
#endif

static volatile bool test_running;
static volatile bool peer_ready;
static int throughput_ep_id = -1;
static TaskHandle_t tx_task_handle;
static uint32_t test_duration_sec;
static throughput_stats_t throughput_stats;
static uint64_t rx_crc_errors;

static uint32_t packet_crc32(const throughput_packet_t *packet)
{
    uint32_t crc;

    crc = bflb_soft_crc32((void *)packet,
                          offsetof(throughput_packet_t, crc32));
    return bflb_soft_crc32_ex(crc, (void *)packet->data,
                              packet->data_size);
}

static throughput_packet_t *alloc_buffer(uint32_t data_size)
{
    throughput_packet_t *packet;
    uint32_t i;

    packet = calloc(1, sizeof(*packet) + data_size);
    if (packet == NULL) {
        return NULL;
    }

    packet->flags = PACKET_FLAG_NONE;
    packet->data_size = data_size;
    for (i = 0; i < data_size; i++) {
        packet->data[i] = (uint8_t)(i & 0xA5U);
    }

    return packet;
}

static void update_stats(uint32_t tx_bytes, uint32_t rx_bytes,
                         uint32_t crc_errors)
{
    throughput_stats_t report;
    uint64_t elapsed_ms;
    bool report_ready = false;

    taskENTER_CRITICAL();
    if (throughput_stats.last_time_ms == 0U) {
        throughput_stats.last_time_ms = bflb_mtimer_get_time_ms();
    }

    if (tx_bytes != 0U) {
        throughput_stats.total_bytes_sent += tx_bytes;
        throughput_stats.total_packets_sent++;
    }
    if (rx_bytes != 0U) {
        throughput_stats.total_bytes_recv += rx_bytes;
        throughput_stats.total_packets_recv++;
    }
    throughput_stats.crc_errors += crc_errors;

    elapsed_ms = bflb_mtimer_get_time_ms() - throughput_stats.last_time_ms;
    if (elapsed_ms >= REPORT_INTERVAL_MS) {
        report = throughput_stats;
        throughput_stats.last_time_ms += elapsed_ms;
        throughput_stats.total_bytes_sent = 0U;
        throughput_stats.total_packets_sent = 0U;
        throughput_stats.total_bytes_recv = 0U;
        throughput_stats.total_packets_recv = 0U;
        throughput_stats.crc_errors = 0U;
        report_ready = true;
    }
    taskEXIT_CRITICAL();

    if (report_ready) {
        printf("IPC_THROUGHPUT role=%s elapsed_ms=%llu tx_packets=%lu "
               "tx_kbps=%llu rx_packets=%lu rx_kbps=%llu",
               IPC_ROLE, elapsed_ms,
               (unsigned long)report.total_packets_sent,
               (uint64_t)report.total_bytes_sent * 8U / elapsed_ms,
               (unsigned long)report.total_packets_recv,
               (uint64_t)report.total_bytes_recv * 8U / elapsed_ms);
        if (ipc_throughput_crc_enabled) {
            printf(" crc_errors=%lu", (unsigned long)report.crc_errors);
        }
        printf("\r\n");
    }
}

static int send_control_packet(uint32_t flags)
{
    throughput_packet_t packet = {
        .seq_num = 0U,
        .flags = flags,
        .data_size = 0U,
        .crc32 = 0U,
    };

    if (ipc_throughput_crc_enabled) {
        packet.crc32 = packet_crc32(&packet);
    }
    return rpmsg_service_send(throughput_ep_id, &packet, sizeof(packet));
}

static int throughput_endpoint_cb(struct rpmsg_endpoint *ept, void *data,
                                  size_t len, uint32_t src, void *priv)
{
    const throughput_packet_t *packet = data;
    bool crc_failed;

    (void)ept;
    (void)src;
    (void)priv;

    if (len < sizeof(*packet)) {
        if (ipc_throughput_crc_enabled) {
            rx_crc_errors++;
        }
        update_stats(0U, (uint32_t)len,
                     ipc_throughput_crc_enabled ? 1U : 0U);
        return RPMSG_SUCCESS;
    }

    crc_failed = ipc_throughput_crc_enabled &&
                 (packet->data_size > MAX_BUFFER_SIZE ||
                  len != sizeof(*packet) + packet->data_size ||
                  packet->crc32 != packet_crc32(packet));

    if (packet->flags == PACKET_FLAG_SYNC && len == sizeof(*packet)) {
        peer_ready = true;
        return RPMSG_SUCCESS;
    }

    if (packet->flags == PACKET_FLAG_START && len == sizeof(*packet)) {
        rx_crc_errors = crc_failed ? 1U : 0U;
        return RPMSG_SUCCESS;
    }

    if (packet->flags == PACKET_FLAG_END && len == sizeof(*packet)) {
        if (crc_failed) {
            rx_crc_errors++;
        }
        if (ipc_throughput_crc_enabled) {
            printf("IPC_THROUGHPUT_CRC role=%s failed_packets=%llu\r\n",
                   IPC_ROLE, rx_crc_errors);
        }
        return RPMSG_SUCCESS;
    }

    if (crc_failed) {
        rx_crc_errors++;
        update_stats(0U, (uint32_t)len, 1U);
        return RPMSG_SUCCESS;
    }

    if (packet->flags == PACKET_FLAG_NONE) {
        update_stats(0U, (uint32_t)len, 0U);
    }
    return RPMSG_SUCCESS;
}

static void tx_task(void *param)
{
    uint32_t data_size = (uint32_t)(uintptr_t)param;
    uint32_t packet_len = sizeof(throughput_packet_t) + data_size;
    uint64_t duration_ms = (uint64_t)test_duration_sec * 1000U;
    uint64_t start_ms;
    uint32_t tx_sequence = 0U;
    throughput_packet_t *packet;
    int ret;

    while (test_running && !peer_ready) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (!test_running) {
        tx_task_handle = NULL;
        vTaskDelete(NULL);
        return;
    }

    packet = alloc_buffer(data_size);
    if (packet == NULL) {
        printf("IPC_THROUGHPUT_ERROR role=%s alloc_size=%lu\r\n",
               IPC_ROLE, (unsigned long)packet_len);
        test_running = false;
        tx_task_handle = NULL;
        vTaskDelete(NULL);
        return;
    }

    while (test_running &&
           send_control_packet(PACKET_FLAG_START) < 0) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    start_ms = bflb_mtimer_get_time_ms();
    while (test_running) {
        if (duration_ms != 0U &&
            bflb_mtimer_get_time_ms() - start_ms >= duration_ms) {
            break;
        }

        packet->seq_num = ++tx_sequence;
        if (ipc_throughput_crc_enabled) {
            packet->crc32 = packet_crc32(packet);
        }
        ret = rpmsg_service_send(throughput_ep_id, packet, packet_len);
        if (ret > 0) {
            update_stats((uint32_t)ret, 0U, 0U);
        } else {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    free(packet);
    test_running = false;
    if (send_control_packet(PACKET_FLAG_END) < 0) {
        printf("IPC_THROUGHPUT_ERROR role=%s end_failed\r\n", IPC_ROLE);
    }
    tx_task_handle = NULL;
    vTaskDelete(NULL);
}

#if defined(CPU_AP)
static void sync_task(void *param)
{
    (void)param;

    while (!rpmsg_service_endpoint_is_bound(throughput_ep_id)) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    while (send_control_packet(PACKET_FLAG_SYNC) < 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    peer_ready = true;
    printf("IPC_THROUGHPUT_SYNCED role=AP\r\n");
    vTaskDelete(NULL);
}
#endif

int ipc_throughput_is_ready(void)
{
    return peer_ready;
}

int cmd_ipc_throughput_test(int argc, char **argv)
{
    uint32_t data_size = (argc >= 2) ?
                             (uint32_t)atoi(argv[1]) :
                             DEFAULT_BUFFER_SIZE;
    uint32_t duration_sec = (argc >= 3) ?
                                (uint32_t)atoi(argv[2]) :
                                DEFAULT_DURATION_SEC;

    if (argc > 3 || data_size == 0U || data_size > MAX_BUFFER_SIZE) {
        printf("Usage: ipc_throughput_test [data_size:1-1500] "
               "[duration_sec:0=until stopped]\r\n");
        return -1;
    }

    if (tx_task_handle != NULL) {
        printf("IPC_THROUGHPUT_ERROR role=%s already_running\r\n", IPC_ROLE);
        return -1;
    }

    test_duration_sec = duration_sec;
    test_running = true;
    if (xTaskCreate(tx_task, "ipc_tx", APP_TASK_STACK_SIZE,
                    (void *)(uintptr_t)data_size, APP_TASK_PRIORITY,
                    &tx_task_handle) != pdPASS) {
        test_running = false;
        tx_task_handle = NULL;
        return -1;
    }

    printf("IPC_THROUGHPUT_START role=%s data_size=%lu duration_sec=%lu\r\n",
           IPC_ROLE, (unsigned long)data_size, (unsigned long)duration_sec);
    return 0;
}

static int cmd_ipc_throughput_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    test_running = false;
    return 0;
}

int ipc_throughput_init(void)
{
    int ret;

    throughput_ep_id = rpmsg_service_register_endpoint(
        "ipc_throughput", throughput_endpoint_cb);
    if (throughput_ep_id < 0) {
        return throughput_ep_id;
    }

    ret = rpmsg_service_start();
    if (ret < 0) {
        return ret;
    }

#if defined(CPU_AP)
    if (xTaskCreate(sync_task, "ipc_sync", 512, NULL,
                    APP_TASK_PRIORITY, NULL) != pdPASS) {
        return -1;
    }
#endif

    printf("IPC_THROUGHPUT_READY role=%s endpoint=ipc_throughput\r\n",
           IPC_ROLE);
    return 0;
}

#ifdef CONFIG_SHELL
#include <shell.h>
SHELL_CMD_EXPORT_ALIAS(cmd_ipc_throughput_test, ipc_throughput_test,
                       Start IPC throughput test);
SHELL_CMD_EXPORT_ALIAS(cmd_ipc_throughput_stop, ipc_throughput_stop,
                       Stop IPC throughput test);
#endif
