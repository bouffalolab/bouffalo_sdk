#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bflb_mtd.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "easyflash.h"
#include "incbin.h"
#include "rfparam_adapter.h"

#include "FreeRTOS.h"
#include "task.h"

#include "usb_ecm_ble.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define DBG_TAG "BLE_CLI"
#include "log.h"

#define BLE_CLI_TX_TEXT_MAX     256
#define BLE_CLI_RX_PREVIEW      48
#define BLE_VOICE_RATE_HZ       16000U
#define BLE_VOICE_SAMPLE_BYTES  2U
/* 15 ms / 480 B better matches the observed BLE pacing on this link. */
#define BLE_VOICE_FRAME_MS      15U
#define BLE_VOICE_PACKET_BYTES  ((BLE_VOICE_RATE_HZ * BLE_VOICE_SAMPLE_BYTES * BLE_VOICE_FRAME_MS) / 1000U)

INCBIN(ble_voice_bin_pcm, "audio.pcm");

static void ble_cli_dump_text(const uint8_t *data, size_t len)
{
    size_t preview = len;

    if (preview > BLE_CLI_RX_PREVIEW) {
        preview = BLE_CLI_RX_PREVIEW;
    }

    printf("\"");
    for (size_t i = 0; i < preview; i++) {
        uint8_t ch = data[i];

        if (isprint(ch)) {
            putchar((int)ch);
        } else {
            putchar('.');
        }
    }

    if (preview < len) {
        printf("...");
    }
    printf("\"");
}

static const char *ble_cli_channel_name(usb_ecm_ble_channel_t channel)
{
    switch (channel) {
        case USB_ECM_BLE_CHANNEL_VOICE:
            return "voice";
        case USB_ECM_BLE_CHANNEL_KEY:
            return "key";
        case USB_ECM_BLE_CHANNEL_USER:
            return "user";
        default:
            return "unknown";
    }
}

static int ble_cli_join_args(int argc, char **argv, int start, char *buf, size_t buf_size)
{
    size_t used = 0;

    if (!buf || buf_size == 0 || start >= argc) {
        return -1;
    }

    buf[0] = '\0';

    for (int i = start; i < argc; i++) {
        size_t part_len = strlen(argv[i]);

        if (used + part_len + (used ? 1u : 0u) + 1u > buf_size) {
            return -1;
        }

        if (used) {
            buf[used++] = ' ';
        }

        memcpy(&buf[used], argv[i], part_len);
        used += part_len;
        buf[used] = '\0';
    }

    return (int)used;
}

static int ble_cli_send_text(usb_ecm_ble_channel_t channel, int argc, char **argv, int start)
{
    char text[BLE_CLI_TX_TEXT_MAX];
    int len;
    int ret;

    len = ble_cli_join_args(argc, argv, start, text, sizeof(text));
    if (len <= 0) {
        printf("text is empty or too long (max %u)\r\n", (unsigned int)(sizeof(text) - 1u));
        return -1;
    }

    ret = usb_ecm_ble_send_channel(channel, (const uint8_t *)text, (size_t)len);
    printf("send %s %s: ", ble_cli_channel_name(channel), ret == 0 ? "ok" : "failed");
    ble_cli_dump_text((const uint8_t *)text, (size_t)len);
    printf("\r\n");
    return ret;
}

static void ble_cli_log_voice_preview(const uint8_t *buf, size_t len)
{
    const int16_t *samples = (const int16_t *)buf;

    if (!buf || len < (sizeof(int16_t) * 8U) || (len % sizeof(int16_t)) != 0U) {
        return;
    }

    LOG_I("voice tx preview samples=%d,%d,%d,%d,%d,%d,%d,%d len=%u\r\n",
          samples[0], samples[1], samples[2], samples[3],
          samples[4], samples[5], samples[6], samples[7],
          (unsigned int)len);
}

static int ble_cli_stream_voice_bin(void)
{
    uint8_t packet[BLE_VOICE_PACKET_BYTES];
    const uint8_t *bin_src = gble_voice_bin_pcmData;
    size_t bin_total = (size_t)gble_voice_bin_pcmSize;
    size_t remaining_bytes;
    unsigned long frames = 0;
    unsigned long sent_packets = 0;
    unsigned long failed_packets = 0;
    unsigned long sent_bytes = 0;
    size_t bin_offset = 0U;
    TickType_t last_log_tick;
    TickType_t next_frame_tick;
    uint32_t start_ms;
    uint32_t elapsed_ms;
    uint32_t pcm_duration_ms;

    if (bin_src == NULL || bin_total == 0U) {
        printf("ble_voice_bin no embedded pcm data\r\n");
        return -1;
    }

    if ((bin_total % BLE_VOICE_SAMPLE_BYTES) != 0U) {
        printf("ble_voice_bin embedded pcm size must be %u-byte aligned\r\n",
               (unsigned int)BLE_VOICE_SAMPLE_BYTES);
        return -1;
    }

    remaining_bytes = bin_total;
    pcm_duration_ms = (uint32_t)(((uint64_t)bin_total * 1000ULL) /
                                 (BLE_VOICE_RATE_HZ * BLE_VOICE_SAMPLE_BYTES));

    memset(packet, 0, sizeof(packet));
    memcpy(packet, bin_src,
           (remaining_bytes < sizeof(packet)) ? remaining_bytes : sizeof(packet));
    ble_cli_log_voice_preview(packet,
                              (remaining_bytes < sizeof(packet)) ? remaining_bytes : sizeof(packet));

    start_ms = bflb_mtimer_get_time_ms();
    last_log_tick = xTaskGetTickCount();
    next_frame_tick = last_log_tick;
    LOG_I("voice bin tx start embedded_bytes=%u duration_ms=%u bytes_per_frame=%u rate=%u ch=1\r\n",
          (unsigned int)bin_total,
          (unsigned int)pcm_duration_ms,
          (unsigned int)sizeof(packet),
          (unsigned int)BLE_VOICE_RATE_HZ);

    while (remaining_bytes > 0U) {
        size_t packet_len = (remaining_bytes < sizeof(packet)) ? remaining_bytes : sizeof(packet);
        TickType_t now_tick;
        int ret;

        if (frames > 0U) {
            vTaskDelayUntil(&next_frame_tick, pdMS_TO_TICKS(BLE_VOICE_FRAME_MS));
        }

        memset(packet, 0, sizeof(packet));
        memcpy(packet, bin_src + bin_offset, packet_len);

        /* usb_ecm_ble_send_channel() now applies buffer-level flow control with
         * a short per-fragment timeout, so a frame that cannot be queued in
         * time is dropped (live audio tolerates loss) rather than busy-retried. */
        ret = usb_ecm_ble_send_channel(USB_ECM_BLE_CHANNEL_VOICE, packet, packet_len);
        if (ret != 0) {
            failed_packets++;
        }

        frames++;
        bin_offset += (uint32_t)packet_len;
        remaining_bytes -= packet_len;
        sent_packets++;
        sent_bytes += (unsigned long)packet_len;

        now_tick = xTaskGetTickCount();
        if ((now_tick - last_log_tick) >= pdMS_TO_TICKS(1000)) {
            elapsed_ms = bflb_mtimer_get_time_ms() - start_ms;
            LOG_I("voice bin tx stats sent_pkt=%lu sent_bytes=%lu failed_pkt=%lu frame=%lu remaining=%u elapsed_ms=%u\r\n",
                  sent_packets, sent_bytes, failed_packets, frames,
                  (unsigned int)remaining_bytes, (unsigned int)elapsed_ms);
            last_log_tick = now_tick;
        }
    }

    elapsed_ms = bflb_mtimer_get_time_ms() - start_ms;
    printf("ble_voice_bin done frames=%lu sent_pkt=%lu sent_bytes=%lu failed_pkt=%lu embedded_bytes=%u duration_ms=%u elapsed_ms=%u\r\n",
           frames, sent_packets, sent_bytes, failed_packets,
           (unsigned int)bin_total,
           (unsigned int)pcm_duration_ms,
           (unsigned int)elapsed_ms);
    return (failed_packets == 0U) ? 0 : -1;
}

static void ble_cli_channel_rx(usb_ecm_ble_channel_t channel, const uint8_t *data, size_t len)
{
    printf("[BLE_CLI] rx %s len=%u text=", ble_cli_channel_name(channel), (unsigned int)len);
    ble_cli_dump_text(data, len);
    printf("\r\n");
}

#ifdef CONFIG_SHELL
static int ble_hid_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: ble_hid <text...>\r\n");
        return 0;
    }

    return ble_cli_send_text(USB_ECM_BLE_CHANNEL_KEY, argc, argv, 1);
}
SHELL_CMD_EXPORT_ALIAS(ble_hid_cmd, ble_hid, send KEY channel text for HID typing);

static int ble_voice_bin_cmd(int argc, char **argv)
{
    if (argc != 1) {
        printf("usage: ble_voice_bin\r\n");
        return 0;
    }

    return ble_cli_stream_voice_bin();
}
SHELL_CMD_EXPORT_ALIAS(ble_voice_bin_cmd, ble_voice_bin,
                       send full 16k mono embedded PCM from INCBIN image);
#endif

static void start_main(void *param)
{
    (void)param;

#ifdef CONFIG_SHELL
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);
    LOG_I("Shell ready\r\n");
#endif

    usb_ecm_ble_set_channel_data_callback(USB_ECM_BLE_CHANNEL_VOICE, ble_cli_channel_rx);
    usb_ecm_ble_set_channel_data_callback(USB_ECM_BLE_CHANNEL_KEY, ble_cli_channel_rx);
    usb_ecm_ble_set_channel_data_callback(USB_ECM_BLE_CHANNEL_USER, ble_cli_channel_rx);

    LOG_I("Start BLE CLI sender\r\n");
    LOG_I("Peer name: USB-ECM-AUD\r\n");
    usb_ecm_ble_stack_init();

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    LOG_I("BLE HID CLI Sender\r\n");

    bflb_mtd_init();
    easyflash_init();

    if (rfparam_init(0, NULL, 0) != 0) {
        LOG_E("PHY RF init failed\r\n");
        return 0;
    }

    xTaskCreate(start_main, (char *)"start_task", 2048, NULL, configMAX_PRIORITIES - 1, NULL);

    LOG_I("[OS] start scheduler\r\n");
    vTaskStartScheduler();

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
