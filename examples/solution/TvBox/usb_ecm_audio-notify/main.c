#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "bflb_mtd.h"
#include "board.h"
#include "easyflash.h"
#include "rfparam_adapter.h"
#include "usbd_core.h"

#include "FreeRTOS.h"
#include "task.h"

#include "ble_hid_bridge.h"
#include "usb_ecm_ble.h"
#include "usb_ecm_uac_device.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define DBG_TAG "MAIN"
#include "log.h"

#ifndef USB_ETHERNET_HID_ENABLE
#define USB_ETHERNET_HID_ENABLE 1
#endif

#ifndef USB_ETHERNET_BLE_ENABLE
#define USB_ETHERNET_BLE_ENABLE 1
#endif

static TaskHandle_t start_handle;

#if USB_ETHERNET_BLE_ENABLE
static volatile uint32_t g_ble_bw_rx_packets;
static volatile uint32_t g_ble_bw_rx_bytes;

static void ble_bw_user_rx(usb_ecm_ble_channel_t channel, const uint8_t *data, size_t len)
{
    static uint32_t last_log_ms;
    static uint32_t last_log_bytes;
    uint32_t now_ms;
    uint32_t delta_ms;
    uint32_t delta_bytes;

    (void)channel;
    (void)data;

    g_ble_bw_rx_packets++;
    g_ble_bw_rx_bytes += (uint32_t)len;

    now_ms = bflb_mtimer_get_time_ms();
    if (last_log_ms == 0U) {
        last_log_ms = now_ms;
        last_log_bytes = g_ble_bw_rx_bytes;
        return;
    }

    delta_ms = now_ms - last_log_ms;
    if (delta_ms < 1000U) {
        return;
    }

    delta_bytes = g_ble_bw_rx_bytes - last_log_bytes;
    LOG_I("BLE bw rx pkt=%u bytes=%u delta_bytes=%u delta_ms=%u kbps=%u\r\n",
          (unsigned int)g_ble_bw_rx_packets,
          (unsigned int)g_ble_bw_rx_bytes,
          (unsigned int)delta_bytes,
          (unsigned int)delta_ms,
          (unsigned int)((delta_bytes * 8UL) / delta_ms));

    last_log_ms = now_ms;
    last_log_bytes = g_ble_bw_rx_bytes;
}

#endif

#if defined(CONFIG_SHELL) && USB_ETHERNET_BLE_ENABLE
static int ble_hid_cmd(int argc, char **argv)
{
    int ret;
    size_t len;

    if (argc != 2) {
        printf("usage: ble_hid <text>\r\n");
        return 0;
    }

    len = strlen(argv[1]);
    if (len == 0U) {
        printf("ble_hid: empty text\r\n");
        return -1;
    }

    ret = usb_ecm_ble_send_channel(USB_ECM_BLE_CHANNEL_KEY, (const uint8_t *)argv[1], len);
    printf("ble_hid len=%u %s\r\n", (unsigned int)len, ret == 0 ? "ok" : "failed");
    return ret;
}
SHELL_CMD_EXPORT_ALIAS(ble_hid_cmd, ble_hid, send BLE HID text over KEY channel);

static int ble_pcm_cmd(int argc, char **argv)
{
    uint32_t duration_ms = 1000U;
    int16_t sample = 1200;
    int ret = 0;
    uint8_t packet[160];
    uint32_t packets;

    if (argc >= 2) {
        duration_ms = (uint32_t)strtoul(argv[1], NULL, 0);
        if (duration_ms == 0U) {
            duration_ms = 1000U;
        }
    }

    if (argc >= 3) {
        sample = (int16_t)strtol(argv[2], NULL, 0);
    }

    for (size_t i = 0; i < sizeof(packet); i += 2U) {
        packet[i] = (uint8_t)(sample & 0xff);
        packet[i + 1U] = (uint8_t)((uint16_t)sample >> 8);
    }

    packets = duration_ms / 10U;
    if (packets == 0U) {
        packets = 1U;
    }

    for (uint32_t i = 0; i < packets; i++) {
        ret = usb_ecm_ble_send_channel(USB_ECM_BLE_CHANNEL_VOICE, packet, sizeof(packet));
        if (ret != 0) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    printf("ble_pcm duration_ms=%u sample=%d packets=%u %s\r\n",
           (unsigned int)duration_ms, (int)sample, (unsigned int)packets,
           ret == 0 ? "ok" : "failed");
    return ret;
}
SHELL_CMD_EXPORT_ALIAS(ble_pcm_cmd, ble_pcm, send BLE PCM test tone over VOICE channel);
#endif

#if defined(CONFIG_SHELL) && USB_ETHERNET_HID_ENABLE
static int hid_status_cmd(int argc, char **argv)
{
    uint8_t ready;
    uint8_t busy;
    uint8_t suspend;
    uint8_t configured;

    (void)argc;
    (void)argv;

    usb_ecm_uac_hid_get_status(&ready, &busy, &suspend, &configured);
    printf("hid_status configured=%u ready=%u busy=%u suspend=%u\r\n",
           (unsigned int)configured, (unsigned int)ready,
           (unsigned int)busy, (unsigned int)suspend);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(hid_status_cmd, hid_status, show HID device status);

static int hid_test_cmd(int argc, char **argv)
{
    static const uint8_t release_report[8] = { 0 };
    int ret;

    if (argc != 2) {
        printf("usage: hid_test release | a\r\n");
        return 0;
    }

    if (strcmp(argv[1], "release") == 0) {
        ret = usb_ecm_uac_hid_send_report(release_report);
        printf("hid_test release %s\r\n", ret == 0 ? "ok" : "failed");
        return ret;
    }

    if (strcmp(argv[1], "a") == 0) {
        ret = usb_ecm_uac_hid_send_key(0x04);
        if (ret == 0) {
            vTaskDelay(pdMS_TO_TICKS(20));
            ret = usb_ecm_uac_hid_send_report(release_report);
        }
        printf("hid_test a %s\r\n", ret == 0 ? "ok" : "failed");
        return ret;
    }

    printf("usage: hid_test release | a\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(hid_test_cmd, hid_test, safe HID keyboard test);
#endif

#ifdef CONFIG_SHELL
static int cmd_cpu_diag(int argc, char **argv)
{
#if (configGENERATE_RUN_TIME_STATS == 1)
    uint32_t window_ms = 1000U;

    if (argc >= 2) {
        unsigned long v = strtoul(argv[1], NULL, 0);
        if (v < 50UL || v > 60000UL) {
            printf("usage: cpu_diag [window_ms:50..60000]\r\n");
            return -1;
        }
        window_ms = (uint32_t)v;
    }

    configRUN_TIME_COUNTER_TYPE total0 = (configRUN_TIME_COUNTER_TYPE)portGET_RUN_TIME_COUNTER_VALUE();
    configRUN_TIME_COUNTER_TYPE idle0 = ulTaskGetIdleRunTimeCounter();

    vTaskDelay(pdMS_TO_TICKS(window_ms));

    configRUN_TIME_COUNTER_TYPE total1 = (configRUN_TIME_COUNTER_TYPE)portGET_RUN_TIME_COUNTER_VALUE();
    configRUN_TIME_COUNTER_TYPE idle1 = ulTaskGetIdleRunTimeCounter();

    if (total1 <= total0 || idle1 < idle0) {
        printf("cpu_diag: invalid sample (counter wrap or too short window)\r\n");
        return -1;
    }

    uint64_t delta_total = (uint64_t)(total1 - total0);
    uint64_t delta_idle = (uint64_t)(idle1 - idle0);
    if (delta_idle > delta_total) {
        delta_idle = delta_total;
    }

    uint64_t cpu_pm = ((delta_total - delta_idle) * 1000ULL) / delta_total;
    uint64_t idle_pm = 1000ULL - cpu_pm;

    printf("cpu_diag: window=%lu ms cpu=%lu.%lu%% idle=%lu.%lu%% total=%llu idle_ticks=%llu\r\n",
           (unsigned long)window_ms, (unsigned long)(cpu_pm / 10ULL), (unsigned long)(cpu_pm % 10ULL),
           (unsigned long)(idle_pm / 10ULL), (unsigned long)(idle_pm % 10ULL), (unsigned long long)delta_total,
           (unsigned long long)delta_idle);
    return 0;
#else
    (void)argc;
    (void)argv;
    printf("cpu_diag: runtime stats disabled\r\n");
    return -1;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_cpu_diag, cpu_diag, "sample CPU usage: cpu_diag [window_ms]");

static int cmd_task_stats(int argc, char **argv)
{
#if (configGENERATE_RUN_TIME_STATS == 1)
    static char info_buffer[1024];

    (void)argc;
    (void)argv;

    memset(info_buffer, 0, sizeof(info_buffer));
    vTaskGetRunTimeStats(info_buffer);
    puts("\r\nTask Name\t Run Time\t CPU Load\r\n");
    puts(info_buffer);
    puts("\r\n");
    return 0;
#else
    (void)argc;
    (void)argv;
    printf("task_stats: runtime stats disabled\r\n");
    return -1;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_task_stats, task_stats, "show FreeRTOS task runtime statistics");
#endif

static void start_main(void *param)
{
    (void)param;

#ifdef CONFIG_SHELL
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);
    LOG_I("Shell ready\r\n");
#endif

#if USB_ETHERNET_BLE_ENABLE
#if USB_ETHERNET_HID_ENABLE
    ble_hid_bridge_init();
#endif

    LOG_I("Start BLE communication channel\r\n");
    usb_ecm_ble_stack_init();
    usb_ecm_ble_set_channel_data_callback(USB_ECM_BLE_CHANNEL_USER, ble_bw_user_rx);
#endif

    LOG_I("Start USB ECM + UAC composite device\r\n");
    usb_ecm_uac_device_start();

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    LOG_I("USB ECM Audio Adapter, CherryUSB version: %s (0x%X)\r\n",
          CHERRYUSB_VERSION_STR, CHERRYUSB_VERSION);

#if USB_ETHERNET_BLE_ENABLE
    bflb_mtd_init();
    easyflash_init();

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_E("PHY RF init failed\r\n");
        return 0;
    }
#endif

#if defined(BOARD_USB_VIA_GPIO)
    board_usb_gpio_init();
#endif

    xTaskCreate(start_main, (char *)"start_task", 2048, NULL, configMAX_PRIORITIES - 1, &start_handle);

    LOG_I("[OS] start scheduler\r\n");
    vTaskStartScheduler();

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
