#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "bflb_core.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "ring_buffer.h"
#include "usbd_audio.h"
#include "usbd_cdc_ecm.h"
#include "usbd_core.h"
#include "usbd_hid.h"

#include "FreeRTOS.h"
#include "portmacro.h"
#include "task.h"

#include "eth_phy_port.h"
#include "eth_emac.h"
#include "usb_ecm_ble.h"
#include "usb_ecm_uac_device.h"

#define DBG_TAG "USB_ETH"
#include "log.h"

#ifndef USB_ETHERNET_UAC_ENABLE
#define USB_ETHERNET_UAC_ENABLE 1
#endif

#ifndef USB_ETHERNET_UAC_SPEAKER_ENABLE
#define USB_ETHERNET_UAC_SPEAKER_ENABLE 1
#endif

#ifndef USB_ETHERNET_UAC_MIC_ENABLE
#define USB_ETHERNET_UAC_MIC_ENABLE 1
#endif

#ifndef USB_ETHERNET_HID_ENABLE
#define USB_ETHERNET_HID_ENABLE 1
#endif

#ifndef USB_ETHERNET_BLE_ENABLE
#define USB_ETHERNET_BLE_ENABLE 0
#endif

#define USBD_VID       0xFFFF
#define USBD_PID       0xFFFF
#define USBD_MAX_POWER 500

#define CDC_ECM_IN_EP  0x81
#define CDC_ECM_OUT_EP 0x02
#define CDC_ECM_INT_EP 0x83
#define CDC_MAX_MPS    512

#define CDC_ECM_MAC_STRING_INDEX 4

#define UAC_AUDIO_FREQ           16000U
#define UAC_AUDIO_CHANNELS       1U
#define UAC_AUDIO_FRAME_SIZE     2U
#define UAC_AUDIO_RESOLUTION_BIT 16U
#define UAC_FEATURE_CTRL         0x00, 0x00
#define UAC_CHANNEL_CONFIG       0x0001

#ifdef CONFIG_USB_HS
#define UAC_EP_INTERVAL 0x04
#else
#define UAC_EP_INTERVAL 0x01
#endif

#define AUDIO_IN_FU_ID  0x02
#define AUDIO_OUT_FU_ID 0x05

#define AUDIO_SPEAKER_OUT_EP 0x04
#define AUDIO_MIC_IN_EP      0x85

#define HID_INT_EP                    0x86
#define HID_INT_EP_SIZE               8
#define HID_INT_EP_INTERVAL           10
#define HID_KEYBOARD_REPORT_DESC_SIZE 63

#define AUDIO_PACKET_SIZE ((uint32_t)((UAC_AUDIO_FREQ * UAC_AUDIO_FRAME_SIZE * UAC_AUDIO_CHANNELS) / 1000U))
#define BLE_VOICE_BYTES_PER_MS ((UAC_AUDIO_FREQ * UAC_AUDIO_FRAME_SIZE * UAC_AUDIO_CHANNELS) / 1000U)
/* Keep about 3 seconds of 16 kHz mono s16le PCM in the BLE->USB mic jitter buffer. */
#define BLE_VOICE_MIC_RB_BYTES (BLE_VOICE_BYTES_PER_MS * 3000U)
/* Prefill about 180 ms of audio before the USB mic endpoint starts draining. */
#define BLE_VOICE_MIC_PREFILL_BYTES (BLE_VOICE_BYTES_PER_MS * 180U)
#define BLE_VOICE_SEVERE_UNDERRUN_PACKETS 16U
#define BLE_VOICE_EDGE_FADE_SAMPLES 8U
#define BLE_VOICE_TX_RB_BYTES (BLE_VOICE_BYTES_PER_MS * 1000U)
#define BLE_VOICE_TX_CHUNK_BYTES (BLE_VOICE_BYTES_PER_MS * 10U)

#define UAC_STREAM_COUNT (USB_ETHERNET_UAC_SPEAKER_ENABLE + USB_ETHERNET_UAC_MIC_ENABLE)

#if USB_ETHERNET_UAC_ENABLE && (UAC_STREAM_COUNT > 0)
#define UAC_ACTIVE 1
#else
#define UAC_ACTIVE 0
#endif

#if USB_ETHERNET_HID_ENABLE
#define HID_ACTIVE 1
#else
#define HID_ACTIVE 0
#endif

#define ECM_INTERFACE_COUNT 2
#define AUDIO_AC_INTF       ECM_INTERFACE_COUNT

#if UAC_ACTIVE
#if USB_ETHERNET_UAC_SPEAKER_ENABLE
#define AUDIO_SPEAKER_INTF (AUDIO_AC_INTF + 1)
#if USB_ETHERNET_UAC_MIC_ENABLE
#define AUDIO_MIC_INTF (AUDIO_SPEAKER_INTF + 1)
#endif
#else
#define AUDIO_MIC_INTF (AUDIO_AC_INTF + 1)
#endif
#endif

#if UAC_ACTIVE
#if USB_ETHERNET_UAC_MIC_ENABLE
#define UAC_MIC_AC_SIZE (AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC + AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(UAC_AUDIO_CHANNELS, 1) + AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)
#else
#define UAC_MIC_AC_SIZE 0
#endif

#if USB_ETHERNET_UAC_SPEAKER_ENABLE
#define UAC_SPEAKER_AC_SIZE (AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC + AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(UAC_AUDIO_CHANNELS, 1) + AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)
#else
#define UAC_SPEAKER_AC_SIZE 0
#endif

#define UAC_INTERFACE_COUNT (1 + UAC_STREAM_COUNT)
#define UAC_AC_SIZE         (AUDIO_SIZEOF_AC_HEADER_DESC(UAC_STREAM_COUNT) + UAC_MIC_AC_SIZE + UAC_SPEAKER_AC_SIZE)
#define UAC_CONFIG_SIZE     (AUDIO_AC_DESCRIPTOR_LEN(UAC_STREAM_COUNT) + UAC_MIC_AC_SIZE + UAC_SPEAKER_AC_SIZE + (UAC_STREAM_COUNT * AUDIO_AS_DESCRIPTOR_LEN(1)))
#else
#define UAC_INTERFACE_COUNT 0
#define UAC_CONFIG_SIZE     0
#endif

#if HID_ACTIVE
#define HID_INTF        (ECM_INTERFACE_COUNT + UAC_INTERFACE_COUNT)
#define HID_CONFIG_SIZE HID_KEYBOARD_DESCRIPTOR_LEN
#else
#define HID_CONFIG_SIZE 0
#endif

#define USB_INTF_COUNT  (ECM_INTERFACE_COUNT + UAC_INTERFACE_COUNT + HID_ACTIVE)
#define USB_CONFIG_SIZE (9 + CDC_ECM_DESCRIPTOR_LEN + UAC_CONFIG_SIZE + HID_CONFIG_SIZE)

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
};

static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, USB_INTF_COUNT, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ECM_DESCRIPTOR_INIT(0x00, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP, CDC_MAX_MPS, CDC_ECM_MAC_STRING_INDEX),
#if UAC_ACTIVE
#if USB_ETHERNET_UAC_SPEAKER_ENABLE && USB_ETHERNET_UAC_MIC_ENABLE
    AUDIO_AC_DESCRIPTOR_INIT(AUDIO_AC_INTF, UAC_INTERFACE_COUNT, UAC_AC_SIZE, 0x00, AUDIO_SPEAKER_INTF, AUDIO_MIC_INTF),
#elif USB_ETHERNET_UAC_SPEAKER_ENABLE
    AUDIO_AC_DESCRIPTOR_INIT(AUDIO_AC_INTF, UAC_INTERFACE_COUNT, UAC_AC_SIZE, 0x00, AUDIO_SPEAKER_INTF),
#else
    AUDIO_AC_DESCRIPTOR_INIT(AUDIO_AC_INTF, UAC_INTERFACE_COUNT, UAC_AC_SIZE, 0x00, AUDIO_MIC_INTF),
#endif
#if USB_ETHERNET_UAC_MIC_ENABLE
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x01, AUDIO_INTERM_MIC, UAC_AUDIO_CHANNELS, UAC_CHANNEL_CONFIG),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x01, 0x01, UAC_FEATURE_CTRL),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x03, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID),
#endif
#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_TERMINAL_STREAMING, UAC_AUDIO_CHANNELS, UAC_CHANNEL_CONFIG),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x04, 0x01, UAC_FEATURE_CTRL),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x06, AUDIO_OUTTERM_SPEAKER, AUDIO_OUT_FU_ID),
    AUDIO_AS_DESCRIPTOR_INIT(AUDIO_SPEAKER_INTF, 0x04, UAC_AUDIO_CHANNELS, UAC_AUDIO_FRAME_SIZE, UAC_AUDIO_RESOLUTION_BIT,
                             AUDIO_SPEAKER_OUT_EP, 0x09, AUDIO_PACKET_SIZE, UAC_EP_INTERVAL, AUDIO_SAMPLE_FREQ_3B(UAC_AUDIO_FREQ)),
#endif
#if USB_ETHERNET_UAC_MIC_ENABLE
    AUDIO_AS_DESCRIPTOR_INIT(AUDIO_MIC_INTF, 0x03, UAC_AUDIO_CHANNELS, UAC_AUDIO_FRAME_SIZE, UAC_AUDIO_RESOLUTION_BIT,
                             AUDIO_MIC_IN_EP, 0x05, AUDIO_PACKET_SIZE, UAC_EP_INTERVAL, AUDIO_SAMPLE_FREQ_3B(UAC_AUDIO_FREQ)),
#endif
#endif
#if HID_ACTIVE
    HID_KEYBOARD_DESCRIPTOR_INIT(HID_INTF, 0x01, HID_KEYBOARD_REPORT_DESC_SIZE, HID_INT_EP,
                                 HID_INT_EP_SIZE, HID_INT_EP_INTERVAL),
#endif
};

#if HID_ACTIVE
static const uint8_t hid_keyboard_report_desc[HID_KEYBOARD_REPORT_DESC_SIZE] = {
    0x05, 0x01, /* USAGE_PAGE (Generic Desktop) */
    0x09, 0x06, /* USAGE (Keyboard) */
    0xA1, 0x01, /* COLLECTION (Application) */
    0x05, 0x07, /* USAGE_PAGE (Keyboard) */
    0x19, 0xE0, /* USAGE_MINIMUM (Keyboard LeftControl) */
    0x29, 0xE7, /* USAGE_MAXIMUM (Keyboard Right GUI) */
    0x15, 0x00, /* LOGICAL_MINIMUM (0) */
    0x25, 0x01, /* LOGICAL_MAXIMUM (1) */
    0x75, 0x01, /* REPORT_SIZE (1) */
    0x95, 0x08, /* REPORT_COUNT (8) */
    0x81, 0x02, /* INPUT (Data,Var,Abs) */
    0x95, 0x01, /* REPORT_COUNT (1) */
    0x75, 0x08, /* REPORT_SIZE (8) */
    0x81, 0x03, /* INPUT (Cnst,Var,Abs) */
    0x95, 0x05, /* REPORT_COUNT (5) */
    0x75, 0x01, /* REPORT_SIZE (1) */
    0x05, 0x08, /* USAGE_PAGE (LEDs) */
    0x19, 0x01, /* USAGE_MINIMUM (Num Lock) */
    0x29, 0x05, /* USAGE_MAXIMUM (Kana) */
    0x91, 0x02, /* OUTPUT (Data,Var,Abs) */
    0x95, 0x01, /* REPORT_COUNT (1) */
    0x75, 0x03, /* REPORT_SIZE (3) */
    0x91, 0x03, /* OUTPUT (Cnst,Var,Abs) */
    0x95, 0x06, /* REPORT_COUNT (6) */
    0x75, 0x08, /* REPORT_SIZE (8) */
    0x15, 0x00, /* LOGICAL_MINIMUM (0) */
    0x25, 0xFF, /* LOGICAL_MAXIMUM (255) */
    0x05, 0x07, /* USAGE_PAGE (Keyboard) */
    0x19, 0x00, /* USAGE_MINIMUM (Reserved) */
    0x29, 0x65, /* USAGE_MAXIMUM (Keyboard Application) */
    0x81, 0x00, /* INPUT (Data,Ary,Abs) */
    0xC0        /* END_COLLECTION */
};
#endif

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, 0x01),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 },
    "BouffaloLab",
    "USB ECM Audio Adapter",
    "2026051401",
    (const char[]){ MAC_ADDR_ASCII_00, MAC_ADDR_ASCII_01, MAC_ADDR_ASCII_10, MAC_ADDR_ASCII_11,
                    MAC_ADDR_ASCII_20, MAC_ADDR_ASCII_21, MAC_ADDR_ASCII_30, MAC_ADDR_ASCII_31,
                    MAC_ADDR_ASCII_40, MAC_ADDR_ASCII_41, MAC_ADDR_ASCII_50, MAC_ADDR_ASCII_51, 0 }
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return config_descriptor;
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_quality_descriptor;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;

    if (index >= (sizeof(string_descriptors) / sizeof(string_descriptors[0]))) {
        return NULL;
    }

    return string_descriptors[index];
}

static const struct usb_descriptor composite_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

static struct usbd_interface cdc_intf0;
static struct usbd_interface cdc_intf1;

#if HID_ACTIVE
static struct usbd_interface hid_intf;
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_write_buffer[USB_ALIGN_UP(HID_INT_EP_SIZE, CONFIG_USB_ALIGN_SIZE)];
static volatile bool hid_ready_flag;
static volatile bool hid_busy_flag;
static volatile bool hid_suspend_flag;
static volatile uint8_t hid_idle_rate;
static volatile uint8_t hid_protocol = 1;

static void hid_int_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    hid_busy_flag = false;
}

static struct usbd_endpoint hid_in_ep = {
    .ep_addr = HID_INT_EP,
    .ep_cb = hid_int_callback,
};

int usb_ecm_uac_hid_send_report(const uint8_t report[HID_INT_EP_SIZE])
{
    if (!hid_ready_flag || hid_busy_flag || !usb_device_is_configured(0)) {
        return -1;
    }

    if (hid_suspend_flag) {
        LOG_W("HID send blocked by suspend, remote wakeup requested\r\n");
        usbd_send_remote_wakeup(0);
        return -1;
    }

    memcpy(hid_write_buffer, report, HID_INT_EP_SIZE);
    hid_busy_flag = true;
    if (usbd_ep_start_write(0, HID_INT_EP, hid_write_buffer, HID_INT_EP_SIZE) < 0) {
        hid_busy_flag = false;
        LOG_W("HID EP start write failed ep=0x%02x\r\n", HID_INT_EP);
        return -1;
    }

    return 0;
}

int usb_ecm_uac_hid_send_key(uint8_t key_code)
{
    const uint8_t report[HID_INT_EP_SIZE] = { 0x00, 0x00, key_code, 0x00, 0x00, 0x00, 0x00, 0x00 };

    return usb_ecm_uac_hid_send_report(report);
}

void usb_ecm_uac_hid_get_status(uint8_t *ready, uint8_t *busy, uint8_t *suspend, uint8_t *configured)
{
    if (ready) {
        *ready = hid_ready_flag ? 1U : 0U;
    }
    if (busy) {
        *busy = hid_busy_flag ? 1U : 0U;
    }
    if (suspend) {
        *suspend = hid_suspend_flag ? 1U : 0U;
    }
    if (configured) {
        *configured = usb_device_is_configured(0) ? 1U : 0U;
    }
}

void usbd_hid_get_report(uint8_t busid, uint8_t intf, uint8_t report_id, uint8_t report_type,
                         uint8_t **data, uint32_t *len)
{
    (void)busid;
    (void)intf;
    (void)report_id;
    (void)report_type;

    memset(hid_write_buffer, 0, HID_INT_EP_SIZE);
    *data = hid_write_buffer;
    *len = HID_INT_EP_SIZE;
}

uint8_t usbd_hid_get_idle(uint8_t busid, uint8_t intf, uint8_t report_id)
{
    (void)busid;
    (void)intf;
    (void)report_id;

    return hid_idle_rate;
}

uint8_t usbd_hid_get_protocol(uint8_t busid, uint8_t intf)
{
    (void)busid;
    (void)intf;

    return hid_protocol;
}

void usbd_hid_set_report(uint8_t busid, uint8_t intf, uint8_t report_id, uint8_t report_type,
                         uint8_t *report, uint32_t report_len)
{
    (void)busid;
    (void)intf;
    (void)report_id;
    (void)report_type;
    (void)report;
    (void)report_len;
}

void usbd_hid_set_idle(uint8_t busid, uint8_t intf, uint8_t report_id, uint8_t duration)
{
    (void)busid;
    (void)intf;
    (void)report_id;

    hid_idle_rate = duration;
}

void usbd_hid_set_protocol(uint8_t busid, uint8_t intf, uint8_t protocol)
{
    (void)busid;
    (void)intf;

    hid_protocol = protocol;
}
#endif

#if UAC_ACTIVE
static struct usbd_interface audio_ac_intf;
#if USB_ETHERNET_UAC_SPEAKER_ENABLE
static struct usbd_interface audio_speaker_intf;
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t audio_speaker_read_buffer[USB_ALIGN_UP(AUDIO_PACKET_SIZE, CONFIG_USB_ALIGN_SIZE)];
static volatile bool audio_speaker_streaming;
static volatile uint32_t audio_speaker_sample_rate = UAC_AUDIO_FREQ;
static volatile bool audio_speaker_mute;
static volatile int audio_speaker_volume_db;
#if USB_ETHERNET_BLE_ENABLE
static Ring_Buffer_Type audio_ble_voice_tx_rb;
static uint8_t audio_ble_voice_tx_storage[BLE_VOICE_TX_RB_BYTES];
static volatile uintptr_t audio_ble_voice_tx_irq_flag;
static TaskHandle_t audio_ble_voice_tx_handle;
static volatile uint32_t audio_ble_voice_tx_usb_packets;
static volatile uint32_t audio_ble_voice_tx_usb_bytes;
static volatile uint32_t audio_ble_voice_tx_ble_packets;
static volatile uint32_t audio_ble_voice_tx_ble_bytes;
static volatile uint32_t audio_ble_voice_tx_drop_bytes;
static volatile uint32_t audio_ble_voice_tx_send_failures;
static TickType_t audio_ble_voice_tx_last_log_tick;
#endif
#endif
#if USB_ETHERNET_UAC_MIC_ENABLE
static struct usbd_interface audio_mic_intf;
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t audio_mic_write_buffer[USB_ALIGN_UP(AUDIO_PACKET_SIZE, CONFIG_USB_ALIGN_SIZE)];
static volatile bool audio_mic_streaming;
static volatile bool audio_mic_ep_busy;
static volatile uint32_t audio_mic_sample_rate = UAC_AUDIO_FREQ;
static volatile bool audio_mic_mute;
static volatile int audio_mic_volume_db;
static TaskHandle_t audio_mic_handle;

#if USB_ETHERNET_BLE_ENABLE
static Ring_Buffer_Type audio_ble_voice_rb;
static uint8_t audio_ble_voice_storage[BLE_VOICE_MIC_RB_BYTES];
static volatile uintptr_t audio_ble_voice_irq_flag;
static volatile bool audio_ble_voice_seen;
static volatile bool audio_ble_voice_prefilled;
static volatile uint32_t audio_ble_voice_rx_packets;
static volatile uint32_t audio_ble_voice_rx_bytes;
static volatile uint32_t audio_ble_voice_drop_bytes;
static volatile uint32_t audio_ble_voice_prefill_silence_packets;
static volatile uint32_t audio_ble_voice_underrun_packets;
static volatile uint32_t audio_ble_voice_conceal_packets;
static volatile uint32_t audio_ble_voice_no_data_packets;
static volatile uint32_t audio_ble_voice_usb_packets;
static uint32_t audio_ble_voice_consecutive_underrun;
static int16_t audio_ble_voice_last_sample;
static bool audio_ble_voice_have_last_sample;
static bool audio_ble_voice_resume_fade_pending;
static volatile bool audio_ble_voice_preview_done;
static TickType_t audio_ble_voice_last_log_tick;
#endif
#endif

static struct audio_entity_info audio_entity_table[] = {
#if USB_ETHERNET_UAC_MIC_ENABLE
    { .bEntityId = AUDIO_IN_FU_ID, .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT, .ep = AUDIO_MIC_IN_EP },
#endif
#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    { .bEntityId = AUDIO_OUT_FU_ID, .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT, .ep = AUDIO_SPEAKER_OUT_EP },
#endif
};

#if USB_ETHERNET_UAC_SPEAKER_ENABLE
#if USB_ETHERNET_BLE_ENABLE
static void audio_ble_voice_tx_rb_lock(void)
{
    audio_ble_voice_tx_irq_flag = bflb_irq_save();
}

static void audio_ble_voice_tx_rb_unlock(void)
{
    bflb_irq_restore(audio_ble_voice_tx_irq_flag);
}

static void audio_ble_voice_tx_reset(void)
{
    Ring_Buffer_Reset(&audio_ble_voice_tx_rb);
    audio_ble_voice_tx_usb_packets = 0U;
    audio_ble_voice_tx_usb_bytes = 0U;
    audio_ble_voice_tx_ble_packets = 0U;
    audio_ble_voice_tx_ble_bytes = 0U;
    audio_ble_voice_tx_drop_bytes = 0U;
    audio_ble_voice_tx_send_failures = 0U;
    audio_ble_voice_tx_last_log_tick = 0;
}

static void audio_ble_voice_tx_log_stats(bool force)
{
    TickType_t now = xTaskGetTickCount();
    uint32_t rb_level;

    if (!force && audio_ble_voice_tx_last_log_tick != 0 &&
        (now - audio_ble_voice_tx_last_log_tick) < pdMS_TO_TICKS(1000)) {
        return;
    }

    rb_level = Ring_Buffer_Get_Length(&audio_ble_voice_tx_rb);
    LOG_I("BLE voice tx usb_pkt=%u usb_bytes=%u ble_pkt=%u ble_bytes=%u rb=%u drop=%u fail=%u\r\n",
          (unsigned int)audio_ble_voice_tx_usb_packets,
          (unsigned int)audio_ble_voice_tx_usb_bytes,
          (unsigned int)audio_ble_voice_tx_ble_packets,
          (unsigned int)audio_ble_voice_tx_ble_bytes,
          (unsigned int)rb_level,
          (unsigned int)audio_ble_voice_tx_drop_bytes,
          (unsigned int)audio_ble_voice_tx_send_failures);
    audio_ble_voice_tx_last_log_tick = now;
}

static void audio_ble_voice_tx_task_kick(void)
{
    if (audio_ble_voice_tx_handle == NULL) {
        return;
    }

    if (xPortIsInsideInterrupt()) {
        BaseType_t higher_priority_task_woken = pdFALSE;
        vTaskNotifyGiveFromISR(audio_ble_voice_tx_handle, &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    } else {
        xTaskNotifyGive(audio_ble_voice_tx_handle);
    }
}

static uint32_t audio_ble_voice_tx_enqueue(const uint8_t *data, uint32_t len)
{
    uint32_t empty;
    uint32_t to_write;
    uint32_t wrote;

    if (!data || len == 0U) {
        return 0U;
    }

    empty = Ring_Buffer_Get_Empty_Length(&audio_ble_voice_tx_rb);
    to_write = (len > empty) ? empty : len;
    wrote = 0U;
    if (to_write > 0U) {
        wrote = Ring_Buffer_Write(&audio_ble_voice_tx_rb, data, to_write);
    }

    if (wrote < len) {
        audio_ble_voice_tx_drop_bytes += (len - wrote);
    }

    if (wrote > 0U) {
        audio_ble_voice_tx_usb_packets++;
        audio_ble_voice_tx_usb_bytes += wrote;
        audio_ble_voice_tx_task_kick();
    }

    return wrote;
}

static void audio_ble_voice_tx_task(void *param)
{
    uint8_t packet[BLE_VOICE_TX_CHUNK_BYTES];

    (void)param;

    while (1) {
        uint32_t available;
        uint32_t chunk;
        uint32_t read_len;

        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10));

        while (1) {
            available = Ring_Buffer_Get_Length(&audio_ble_voice_tx_rb);
            if (available < AUDIO_PACKET_SIZE) {
                break;
            }

            chunk = (available > sizeof(packet)) ? sizeof(packet) : available;
            chunk &= ~(uint32_t)(sizeof(int16_t) - 1U);
            if (chunk == 0U) {
                break;
            }

            read_len = Ring_Buffer_Read(&audio_ble_voice_tx_rb, packet, chunk);
            read_len &= ~(uint32_t)(sizeof(int16_t) - 1U);
            if (read_len == 0U) {
                break;
            }

            if (usb_ecm_ble_send_channel(USB_ECM_BLE_CHANNEL_VOICE, packet, read_len) == 0) {
                audio_ble_voice_tx_ble_packets++;
                audio_ble_voice_tx_ble_bytes += read_len;
            } else {
                audio_ble_voice_tx_send_failures++;
                LOG_W("BLE voice tx send failed len=%u\r\n", (unsigned int)read_len);
                break;
            }

            audio_ble_voice_tx_log_stats(false);
        }
    }
}
#endif

static void audio_speaker_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)ep;
#if !USB_ETHERNET_BLE_ENABLE
    (void)nbytes;
#endif

#if USB_ETHERNET_BLE_ENABLE
    if (audio_speaker_streaming && nbytes > 0U) {
        (void)audio_ble_voice_tx_enqueue(audio_speaker_read_buffer, nbytes);
    }
#endif

    if (audio_speaker_streaming) {
        usbd_ep_start_read(busid, AUDIO_SPEAKER_OUT_EP, audio_speaker_read_buffer, AUDIO_PACKET_SIZE);
    }
}

static struct usbd_endpoint audio_speaker_out_ep = {
    .ep_addr = AUDIO_SPEAKER_OUT_EP,
    .ep_cb = audio_speaker_out_callback,
};
#endif

#if USB_ETHERNET_UAC_MIC_ENABLE
#if USB_ETHERNET_BLE_ENABLE
static void audio_ble_voice_rb_lock(void)
{
    audio_ble_voice_irq_flag = bflb_irq_save();
}

static void audio_ble_voice_rb_unlock(void)
{
    bflb_irq_restore(audio_ble_voice_irq_flag);
}

static void audio_ble_voice_reset(void)
{
    Ring_Buffer_Reset(&audio_ble_voice_rb);
    audio_ble_voice_seen = false;
    audio_ble_voice_prefilled = false;
    audio_ble_voice_rx_packets = 0U;
    audio_ble_voice_rx_bytes = 0U;
    audio_ble_voice_drop_bytes = 0U;
    audio_ble_voice_prefill_silence_packets = 0U;
    audio_ble_voice_underrun_packets = 0U;
    audio_ble_voice_conceal_packets = 0U;
    audio_ble_voice_no_data_packets = 0U;
    audio_ble_voice_usb_packets = 0U;
    audio_ble_voice_consecutive_underrun = 0U;
    audio_ble_voice_last_sample = 0;
    audio_ble_voice_have_last_sample = false;
    audio_ble_voice_resume_fade_pending = false;
    audio_ble_voice_preview_done = false;
    audio_ble_voice_last_log_tick = 0;
}

static void audio_ble_voice_log_preview(const uint8_t *data, size_t len)
{
    const int16_t *samples = (const int16_t *)data;
    size_t sample_count;

    if (!data || len < (sizeof(int16_t) * 8U) || (len % sizeof(int16_t)) != 0U || audio_ble_voice_preview_done) {
        return;
    }

    sample_count = len / sizeof(int16_t);
    if (sample_count < 8U) {
        return;
    }

    LOG_I("BLE voice preview samples=%d,%d,%d,%d,%d,%d,%d,%d len=%u\r\n",
          samples[0], samples[1], samples[2], samples[3],
          samples[4], samples[5], samples[6], samples[7],
          (unsigned int)len);
    audio_ble_voice_preview_done = true;
}

static void audio_ble_voice_log_stats(bool force)
{
    TickType_t now;
    uint32_t rb_level;

    now = xTaskGetTickCount();
    if (!force && audio_ble_voice_last_log_tick != 0 &&
        (now - audio_ble_voice_last_log_tick) < pdMS_TO_TICKS(1000)) {
        return;
    }

    rb_level = Ring_Buffer_Get_Length(&audio_ble_voice_rb);
    LOG_I("BLE voice stats rx_pkt=%u rx_bytes=%u usb_pkt=%u rb=%u drop=%u prefill_sil=%u underrun=%u conceal=%u no_data=%u\r\n",
          (unsigned int)audio_ble_voice_rx_packets,
          (unsigned int)audio_ble_voice_rx_bytes,
          (unsigned int)audio_ble_voice_usb_packets,
          (unsigned int)rb_level,
          (unsigned int)audio_ble_voice_drop_bytes,
          (unsigned int)audio_ble_voice_prefill_silence_packets,
          (unsigned int)audio_ble_voice_underrun_packets,
          (unsigned int)audio_ble_voice_conceal_packets,
          (unsigned int)audio_ble_voice_no_data_packets);
    audio_ble_voice_last_log_tick = now;
}

static void audio_ble_voice_conceal(uint8_t *buffer, uint32_t len)
{
    int16_t *samples = (int16_t *)buffer;
    uint32_t sample_count = len / sizeof(int16_t);
    uint32_t fade_samples;

    if (!buffer || len == 0U) {
        return;
    }

    audio_ble_voice_conceal_packets++;
    memset(buffer, 0, len);

    if (!audio_ble_voice_have_last_sample || sample_count == 0U || (len % sizeof(int16_t)) != 0U) {
        audio_ble_voice_last_sample = 0;
        audio_ble_voice_have_last_sample = false;
        audio_ble_voice_resume_fade_pending = true;
        return;
    }

    fade_samples = sample_count;
    if (fade_samples > BLE_VOICE_EDGE_FADE_SAMPLES) {
        fade_samples = BLE_VOICE_EDGE_FADE_SAMPLES;
    }

    for (uint32_t i = 0; i < fade_samples; i++) {
        uint32_t remain = fade_samples - i;
        samples[i] = (int16_t)((int32_t)audio_ble_voice_last_sample * (int32_t)remain / (int32_t)fade_samples);
    }

    audio_ble_voice_last_sample = 0;
    audio_ble_voice_have_last_sample = false;
    audio_ble_voice_resume_fade_pending = true;
}

static void audio_ble_voice_apply_resume_fade(uint8_t *buffer, uint32_t len)
{
    int16_t *samples = (int16_t *)buffer;
    uint32_t sample_count = len / sizeof(int16_t);
    uint32_t fade_samples;

    if (!audio_ble_voice_resume_fade_pending || !buffer || len == 0U || sample_count == 0U ||
        (len % sizeof(int16_t)) != 0U) {
        return;
    }

    fade_samples = sample_count;
    if (fade_samples > BLE_VOICE_EDGE_FADE_SAMPLES) {
        fade_samples = BLE_VOICE_EDGE_FADE_SAMPLES;
    }

    for (uint32_t i = 0; i < fade_samples; i++) {
        uint32_t gain = i + 1U;
        samples[i] = (int16_t)((int32_t)samples[i] * (int32_t)gain / (int32_t)fade_samples);
    }

    audio_ble_voice_resume_fade_pending = false;
}

static void audio_ble_voice_rx(usb_ecm_ble_channel_t channel, const uint8_t *data, size_t len)
{
    uint32_t empty;
    uint32_t to_write;

    (void)channel;

    if (!data || len == 0U) {
        return;
    }

    audio_ble_voice_rx_packets++;
    audio_ble_voice_rx_bytes += (uint32_t)len;
    audio_ble_voice_log_preview(data, len);

    empty = Ring_Buffer_Get_Empty_Length(&audio_ble_voice_rb);
    to_write = (len > empty) ? empty : (uint32_t)len;
    if (to_write > 0U) {
        (void)Ring_Buffer_Write(&audio_ble_voice_rb, data, to_write);
        audio_ble_voice_seen = true;
    }

    if (to_write < len) {
        audio_ble_voice_drop_bytes += (uint32_t)(len - to_write);
        LOG_W("BLE voice rb full, drop=%u\r\n", (unsigned int)(len - to_write));
    }
}

static bool audio_ble_voice_read(uint8_t *buffer, uint32_t len)
{
    uint32_t available;
    uint32_t read;
    uint32_t read_len;

    if (!buffer || len == 0U) {
        return false;
    }

    if (!audio_ble_voice_seen) {
        return false;
    }

    available = Ring_Buffer_Get_Length(&audio_ble_voice_rb);
    if (!audio_ble_voice_prefilled) {
        if (available < BLE_VOICE_MIC_PREFILL_BYTES) {
            audio_ble_voice_prefill_silence_packets++;
            memset(buffer, 0, len);
            audio_ble_voice_resume_fade_pending = true;
            return true;
        }
        audio_ble_voice_prefilled = true;
        audio_ble_voice_consecutive_underrun = 0U;
    }

    read_len = (available < len) ? available : len;
    read_len &= ~(uint32_t)(sizeof(int16_t) - 1U);
    read = 0U;
    if (read_len > 0U) {
        read = Ring_Buffer_Read(&audio_ble_voice_rb, buffer, read_len);
        read &= ~(uint32_t)(sizeof(int16_t) - 1U);
        if (read > 0U) {
            audio_ble_voice_apply_resume_fade(buffer, read);
        }
    }

    if (read < len) {
        audio_ble_voice_underrun_packets++;
        audio_ble_voice_consecutive_underrun++;
        audio_ble_voice_conceal(buffer + read, len - read);

        if (read == 0U && audio_ble_voice_consecutive_underrun >= BLE_VOICE_SEVERE_UNDERRUN_PACKETS) {
            audio_ble_voice_prefilled = false;
            audio_ble_voice_consecutive_underrun = 0U;
        }
    } else {
        audio_ble_voice_consecutive_underrun = 0U;
    }

    if (read > 0U) {
        const int16_t *samples = (const int16_t *)buffer;
        uint32_t sample_count = read / sizeof(int16_t);

        if (sample_count > 0U) {
            audio_ble_voice_last_sample = samples[sample_count - 1U];
            audio_ble_voice_have_last_sample = true;
        }
    }

    return true;
}
#endif

static void audio_mic_task_kick(void)
{
    if (audio_mic_handle == NULL) {
        return;
    }

    if (xPortIsInsideInterrupt()) {
        BaseType_t higher_priority_task_woken = pdFALSE;
        vTaskNotifyGiveFromISR(audio_mic_handle, &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    } else {
        xTaskNotifyGive(audio_mic_handle);
    }
}

static void audio_mic_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    audio_mic_ep_busy = false;
    audio_mic_task_kick();
}

static struct usbd_endpoint audio_mic_in_ep = {
    .ep_addr = AUDIO_MIC_IN_EP,
    .ep_cb = audio_mic_in_callback,
};

static void audio_fill_mic_packet(uint8_t *buffer, uint32_t len)
{
#if USB_ETHERNET_BLE_ENABLE
    if (!audio_mic_mute && audio_ble_voice_read(buffer, len)) {
        return;
    }
    if (!audio_mic_mute) {
        audio_ble_voice_no_data_packets++;
    }
#endif
    memset(buffer, 0, len);
}

static void audio_mic_task(void *param)
{
    (void)param;

    while (1) {
        if (audio_mic_streaming && !audio_mic_ep_busy && usb_device_is_configured(0)) {
            audio_fill_mic_packet(audio_mic_write_buffer, AUDIO_PACKET_SIZE);
            audio_mic_ep_busy = true;
            if (usbd_ep_start_write(0, AUDIO_MIC_IN_EP, audio_mic_write_buffer, AUDIO_PACKET_SIZE) < 0) {
                audio_mic_ep_busy = false;
                ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(2));
                continue;
            }
#if USB_ETHERNET_BLE_ENABLE
            audio_ble_voice_usb_packets++;
            audio_ble_voice_log_stats(false);
#endif
        }

        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(2));
    }
}
#endif

void usbd_audio_open(uint8_t busid, uint8_t intf)
{
#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    if (intf == AUDIO_SPEAKER_INTF) {
        audio_speaker_streaming = true;
#if USB_ETHERNET_BLE_ENABLE
        audio_ble_voice_tx_reset();
#endif
        usbd_ep_start_read(busid, AUDIO_SPEAKER_OUT_EP, audio_speaker_read_buffer, AUDIO_PACKET_SIZE);
        LOG_I("UAC speaker open\r\n");
        return;
    }
#endif

#if USB_ETHERNET_UAC_MIC_ENABLE
    if (intf == AUDIO_MIC_INTF) {
        audio_mic_streaming = true;
        audio_mic_ep_busy = false;
#if USB_ETHERNET_BLE_ENABLE
        audio_ble_voice_reset();
#endif
        LOG_I("UAC mic open\r\n");
        audio_mic_task_kick();
        return;
    }
#endif

    (void)busid;
}

void usbd_audio_close(uint8_t busid, uint8_t intf)
{
    (void)busid;

#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    if (intf == AUDIO_SPEAKER_INTF) {
        audio_speaker_streaming = false;
#if USB_ETHERNET_BLE_ENABLE
        audio_ble_voice_tx_log_stats(true);
        audio_ble_voice_tx_reset();
#endif
        LOG_I("UAC speaker close\r\n");
        return;
    }
#endif

#if USB_ETHERNET_UAC_MIC_ENABLE
    if (intf == AUDIO_MIC_INTF) {
        audio_mic_streaming = false;
        audio_mic_ep_busy = false;
#if USB_ETHERNET_BLE_ENABLE
        audio_ble_voice_log_stats(true);
        audio_ble_voice_reset();
#endif
        LOG_I("UAC mic close\r\n");
        return;
    }
#endif
}

void usbd_audio_set_sampling_freq(uint8_t busid, uint8_t ep, uint32_t sampling_freq)
{
    (void)busid;

#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    if (ep == AUDIO_SPEAKER_OUT_EP) {
        audio_speaker_sample_rate = sampling_freq;
    }
#endif

#if USB_ETHERNET_UAC_MIC_ENABLE
    if (ep == AUDIO_MIC_IN_EP) {
        audio_mic_sample_rate = sampling_freq;
    }
#endif
}

uint32_t usbd_audio_get_sampling_freq(uint8_t busid, uint8_t ep)
{
    (void)busid;

#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    if (ep == AUDIO_SPEAKER_OUT_EP) {
        return audio_speaker_sample_rate;
    }
#endif

#if USB_ETHERNET_UAC_MIC_ENABLE
    if (ep == AUDIO_MIC_IN_EP) {
        return audio_mic_sample_rate;
    }
#endif

    return UAC_AUDIO_FREQ;
}

void usbd_audio_set_mute(uint8_t busid, uint8_t ep, uint8_t ch, bool mute)
{
    (void)busid;
    (void)ch;

#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    if (ep == AUDIO_SPEAKER_OUT_EP) {
        audio_speaker_mute = mute;
    }
#endif

#if USB_ETHERNET_UAC_MIC_ENABLE
    if (ep == AUDIO_MIC_IN_EP) {
        audio_mic_mute = mute;
    }
#endif
}

bool usbd_audio_get_mute(uint8_t busid, uint8_t ep, uint8_t ch)
{
    (void)busid;
    (void)ch;

#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    if (ep == AUDIO_SPEAKER_OUT_EP) {
        return audio_speaker_mute;
    }
#endif

#if USB_ETHERNET_UAC_MIC_ENABLE
    if (ep == AUDIO_MIC_IN_EP) {
        return audio_mic_mute;
    }
#endif

    return false;
}

void usbd_audio_set_volume(uint8_t busid, uint8_t ep, uint8_t ch, int volume_db)
{
    (void)busid;
    (void)ch;

#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    if (ep == AUDIO_SPEAKER_OUT_EP) {
        audio_speaker_volume_db = volume_db;
    }
#endif

#if USB_ETHERNET_UAC_MIC_ENABLE
    if (ep == AUDIO_MIC_IN_EP) {
        audio_mic_volume_db = volume_db;
    }
#endif
}

int usbd_audio_get_volume(uint8_t busid, uint8_t ep, uint8_t ch)
{
    (void)busid;
    (void)ch;

#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    if (ep == AUDIO_SPEAKER_OUT_EP) {
        return audio_speaker_volume_db;
    }
#endif

#if USB_ETHERNET_UAC_MIC_ENABLE
    if (ep == AUDIO_MIC_IN_EP) {
        return audio_mic_volume_db;
    }
#endif

    return 0;
}
#endif

static volatile bool usb_ecm_ready_flag;

static void usbd_ecm_emac_event_trig(void);

static const char *usb_event_name(uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            return "RESET";
        case USBD_EVENT_CONNECTED:
            return "CONNECTED";
        case USBD_EVENT_DISCONNECTED:
            return "DISCONNECTED";
        case USBD_EVENT_SUSPEND:
            return "SUSPEND";
        case USBD_EVENT_RESUME:
            return "RESUME";
        case USBD_EVENT_CONFIGURED:
            return "CONFIGURED";
        default:
            return "UNKNOWN";
    }
}

static void composite_event_handler(uint8_t busid, uint8_t event)
{
    LOG_I("USB event bus=%u %s(%u)\r\n", busid, usb_event_name(event), event);

    switch (event) {
        case USBD_EVENT_RESET:
        case USBD_EVENT_CONNECTED:
        case USBD_EVENT_DISCONNECTED:
        case USBD_EVENT_SUSPEND:
            usb_ecm_ready_flag = false;
#if HID_ACTIVE
            hid_ready_flag = false;
            hid_busy_flag = false;
            hid_suspend_flag = (event == USBD_EVENT_SUSPEND);
#endif
#if UAC_ACTIVE && USB_ETHERNET_UAC_SPEAKER_ENABLE
            audio_speaker_streaming = false;
#endif
#if UAC_ACTIVE && USB_ETHERNET_UAC_MIC_ENABLE
            audio_mic_streaming = false;
            audio_mic_ep_busy = false;
#endif
            usbd_ecm_emac_event_trig();
            break;

        case USBD_EVENT_RESUME:
            usb_ecm_ready_flag = true;
#if HID_ACTIVE
            hid_suspend_flag = false;
#endif
            usbd_ecm_emac_event_trig();
            break;

        case USBD_EVENT_CONFIGURED:
            LOG_I("USB composite configured\r\n");
            usb_ecm_ready_flag = true;
#if HID_ACTIVE
            hid_ready_flag = true;
            hid_busy_flag = false;
            hid_suspend_flag = false;
#endif
#if UAC_ACTIVE && USB_ETHERNET_UAC_MIC_ENABLE
            audio_mic_ep_busy = false;
#endif
            usbd_ecm_emac_event_trig();
            break;

        default:
            break;
    }
}

static void composite_usb_init(uint8_t busid, uintptr_t reg_base)
{
    LOG_I("USB init bus=%u reg_base=0x%08lx\r\n", busid, (unsigned long)reg_base);
    usbd_desc_register(busid, &composite_descriptor);
    usbd_add_interface(busid, usbd_cdc_ecm_init_intf(&cdc_intf0, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP));
    usbd_add_interface(busid, usbd_cdc_ecm_init_intf(&cdc_intf1, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP));

#if UAC_ACTIVE
    const uint8_t audio_entity_count = sizeof(audio_entity_table) / sizeof(audio_entity_table[0]);
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &audio_ac_intf, 0x0100, audio_entity_table, audio_entity_count));
#if USB_ETHERNET_UAC_SPEAKER_ENABLE
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &audio_speaker_intf, 0x0100, audio_entity_table, audio_entity_count));
    usbd_add_endpoint(busid, &audio_speaker_out_ep);
#endif
#if USB_ETHERNET_UAC_MIC_ENABLE
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &audio_mic_intf, 0x0100, audio_entity_table, audio_entity_count));
    usbd_add_endpoint(busid, &audio_mic_in_ep);
#endif
#endif

#if HID_ACTIVE
    usbd_add_interface(busid, usbd_hid_init_intf(busid, &hid_intf, hid_keyboard_report_desc,
                                                 HID_KEYBOARD_REPORT_DESC_SIZE));
    usbd_add_endpoint(busid, &hid_in_ep);
#endif

    usbd_initialize(busid, reg_base, composite_event_handler);
    LOG_I("USB initialize submitted\r\n");
}

#if defined(BOARD_USB_VIA_GPIO)
static void usb_force_reconnect_pulse(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    if (!gpio) {
        LOG_W("USB reconnect pulse skipped: gpio device unavailable\r\n");
        return;
    }

    /* Force a short detach window so the host observes a fresh USB attach
     * even when firmware is reflashed/reset while the cable stays plugged in.
     */
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_33, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_reset(gpio, GPIO_PIN_32);
    bflb_gpio_reset(gpio, GPIO_PIN_33);
    bflb_mtimer_delay_ms(20);

    board_usb_gpio_init();
    bflb_mtimer_delay_ms(20);
    LOG_I("USB reconnect pulse done\r\n");
}
#endif

/************************* ECM_IN <-> EMAC_RX sub-state machine *******************************/
#define ECM_IN_STA_STOP         0
#define ECM_IN_STA_START        1
#define ECM_IN_STA_WAIT_EMAC_RX 2
#define ECM_IN_STA_WAIT_USBD_IN 3
static volatile uint8_t usbd_ecm_in_status = ECM_IN_STA_STOP;
static volatile bool usbd_ecm_in_busy_flag;

void usbd_cdc_ecm_data_send_done(uint32_t len)
{
    (void)len;

    usbd_ecm_in_busy_flag = false;
    usbd_ecm_emac_event_trig();
}

static int usbd_ecm_in_emac_rx_polling(void)
{
    int ret = 0;
    static struct bflb_emac_trans_desc_s trans_desc;

polling_continue:

    switch (usbd_ecm_in_status) {
        case ECM_IN_STA_STOP:
            break;

        case ECM_IN_STA_START:
            LOG_I("USBD ECM IN/RX start\r\n");
            usbd_ecm_in_status = ECM_IN_STA_WAIT_EMAC_RX;
            goto polling_continue;

        case ECM_IN_STA_WAIT_EMAC_RX:
            if (eth_emac_rx_data_get(&trans_desc, 0) < 0) {
                break;
            }

            usbd_ecm_in_busy_flag = true;
            ret = usbd_cdc_ecm_start_write((uint8_t *)(uintptr_t)trans_desc.buff_addr, trans_desc.data_len);
            if (ret < 0) {
                usbd_ecm_in_busy_flag = false;
                eth_emac_rx_data_free(&trans_desc);
                usbd_ecm_in_status = ECM_IN_STA_WAIT_EMAC_RX;
                break;
            }
            usbd_ecm_in_status = ECM_IN_STA_WAIT_USBD_IN;
            break;

        case ECM_IN_STA_WAIT_USBD_IN:
            if (usbd_ecm_in_busy_flag) {
                break;
            }
            eth_emac_rx_data_free(&trans_desc);
            usbd_ecm_in_status = ECM_IN_STA_WAIT_EMAC_RX;
            goto polling_continue;

        default:
            LOG_E("usbd_ecm_in_status error: %d\r\n", usbd_ecm_in_status);
            usbd_ecm_in_status = ECM_IN_STA_STOP;
            break;
    }

    return ret;
}

/************************* ECM_OUT <-> EMAC_TX sub-state machine *******************************/
#define ECM_OUT_STA_STOP          0
#define ECM_OUT_STA_START         1
#define ECM_OUT_STA_WAIT_EMAC_TX  2
#define ECM_OUT_STA_WAIT_USBD_OUT 3
static volatile uint8_t usbd_ecm_out_status = ECM_OUT_STA_STOP;
static volatile bool usbd_ecm_out_busy_flag;
static volatile uint16_t usbd_ecm_out_done_len;

void usbd_cdc_ecm_data_recv_done(uint32_t len)
{
    usbd_ecm_out_busy_flag = false;
    usbd_ecm_out_done_len = len;
    usbd_ecm_emac_event_trig();
}

static int usbd_ecm_out_emac_tx_polling(void)
{
    int ret = 0;
    static struct bflb_emac_trans_desc_s trans_desc;

polling_continue:

    switch (usbd_ecm_out_status) {
        case ECM_OUT_STA_STOP:
            break;

        case ECM_OUT_STA_START:
            LOG_I("USBD ECM OUT/TX start\r\n");
            usbd_ecm_out_status = ECM_OUT_STA_WAIT_EMAC_TX;
            goto polling_continue;

        case ECM_OUT_STA_WAIT_EMAC_TX:
            if (eth_emac_tx_buff_get(&trans_desc, 0) < 0) {
                break;
            }

            usbd_ecm_out_busy_flag = true;
            ret = usbd_cdc_ecm_start_read((uint8_t *)(uintptr_t)trans_desc.buff_addr, EMAC_TX_BUFF_SIZE);
            if (ret < 0) {
                usbd_ecm_out_busy_flag = false;
                usbd_ecm_out_status = ECM_OUT_STA_START;
                break;
            }
            usbd_ecm_out_status = ECM_OUT_STA_WAIT_USBD_OUT;
            break;

        case ECM_OUT_STA_WAIT_USBD_OUT:
            if (usbd_ecm_out_busy_flag) {
                break;
            }
            trans_desc.data_len = usbd_ecm_out_done_len;
            eth_emac_tx_buff_push(&trans_desc);
            usbd_ecm_out_status = ECM_OUT_STA_WAIT_EMAC_TX;
            goto polling_continue;

        default:
            LOG_E("usbd_ecm_out_status error: %d\r\n", usbd_ecm_out_status);
            usbd_ecm_out_status = ECM_OUT_STA_STOP;
            break;
    }

    return ret;
}

/************************* ECM-EMAC state machine *******************************/
#define ECM_EMAC_STA_STOP             0
#define ECM_EMAC_STA_START            1
#define ECM_EMAC_STA_WAIT_USBD_CFG    2
#define ECM_EMAC_STA_WAIT_EPHY_LINKUP 3
#define ECM_EMAC_STA_DATA_POLLING     4
static volatile uint32_t usbd_ecm_emac_sta = ECM_EMAC_STA_STOP;

static int usbd_ecm_emac_polling(void)
{
    static uint32_t status_time_ms;
    static uint32_t info_dump_time_ms;
    uint32_t time_ms;

    switch (usbd_ecm_emac_sta) {
        case ECM_EMAC_STA_STOP:
            break;

        case ECM_EMAC_STA_START:
            LOG_I("usbd ecm machine start/restart\r\n");
            eth_emac_restart();
            usbd_ecm_in_status = ECM_IN_STA_START;
            usbd_ecm_in_busy_flag = false;
            usbd_ecm_out_status = ECM_OUT_STA_START;
            usbd_ecm_out_busy_flag = false;
            usbd_ecm_emac_sta = ECM_EMAC_STA_WAIT_USBD_CFG;
            break;

        case ECM_EMAC_STA_WAIT_USBD_CFG:
            if (!usb_ecm_ready_flag) {
                ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
                break;
            }

            LOG_I("USBD ECM EMAC ready\r\n");
            vTaskDelay(pdMS_TO_TICKS(20));
            usbd_cdc_ecm_set_connect(false, NULL);
            usbd_ecm_emac_sta = ECM_EMAC_STA_WAIT_EPHY_LINKUP;
            break;

        case ECM_EMAC_STA_WAIT_EPHY_LINKUP:
            if (!usb_ecm_ready_flag) {
                usbd_ecm_emac_sta = ECM_EMAC_STA_START;
                break;
            }

            if (eth_link_state_update()) {
                uint32_t speed_table[2] = { 100U * 1000U * 1000U, 100U * 1000U * 1000U };
                usbd_cdc_ecm_set_connect(true, speed_table);
                usbd_ecm_emac_sta = ECM_EMAC_STA_DATA_POLLING;
            } else {
                usbd_cdc_ecm_set_connect(false, NULL);
                ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
            }
            break;

        case ECM_EMAC_STA_DATA_POLLING:
            if (!usb_ecm_ready_flag) {
                usbd_ecm_emac_sta = ECM_EMAC_STA_START;
                break;
            }

            usbd_ecm_in_emac_rx_polling();
            usbd_ecm_out_emac_tx_polling();

            time_ms = bflb_mtimer_get_time_ms();
            if ((time_ms - info_dump_time_ms) > 5000) {
                info_dump_time_ms = time_ms;
                eth_eamc_info_dump();
            }
            if ((time_ms - status_time_ms) > 100) {
                status_time_ms = time_ms;
                usbd_ecm_emac_sta = ECM_EMAC_STA_WAIT_EPHY_LINKUP;
                break;
            }

            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
            break;

        default:
            LOG_E("usbd_ecm_emac_sta error: %d\r\n", usbd_ecm_emac_sta);
            usbd_ecm_emac_sta = ECM_EMAC_STA_STOP;
            break;
    }

    return 0;
}

static TaskHandle_t usbd_ecm_emac_handle;

static void usbd_ecm_emac_event_trig(void)
{
    if (usbd_ecm_emac_handle == NULL) {
        return;
    }

    if (xPortIsInsideInterrupt()) {
        BaseType_t higher_priority_task_woken = pdFALSE;
        vTaskNotifyGiveFromISR(usbd_ecm_emac_handle, &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    } else {
        xTaskNotifyGive(usbd_ecm_emac_handle);
    }
}

static void usbd_emac_event_cb(uint32_t event)
{
    (void)event;
    usbd_ecm_emac_event_trig();
}

static void usbd_ecm_emac_task(void *param)
{
    (void)param;

    usbd_ecm_emac_sta = ECM_EMAC_STA_START;
    LOG_I("USB device cdc_ecm <-> emac task start\r\n");

    while (1) {
        usbd_ecm_emac_polling();
    }
}

void usb_ecm_uac_device_start(void)
{
    usb_ecm_ready_flag = false;

#if UAC_ACTIVE && USB_ETHERNET_UAC_SPEAKER_ENABLE && USB_ETHERNET_BLE_ENABLE
    Ring_Buffer_Init(&audio_ble_voice_tx_rb, audio_ble_voice_tx_storage, sizeof(audio_ble_voice_tx_storage),
                     audio_ble_voice_tx_rb_lock, audio_ble_voice_tx_rb_unlock);
    audio_ble_voice_tx_reset();
#endif

#if UAC_ACTIVE && USB_ETHERNET_UAC_MIC_ENABLE && USB_ETHERNET_BLE_ENABLE
    Ring_Buffer_Init(&audio_ble_voice_rb, audio_ble_voice_storage, sizeof(audio_ble_voice_storage),
                     audio_ble_voice_rb_lock, audio_ble_voice_rb_unlock);
    audio_ble_voice_reset();
    usb_ecm_ble_set_channel_data_callback(USB_ECM_BLE_CHANNEL_VOICE, audio_ble_voice_rx);
    LOG_I("BLE VOICE -> UAC mic bridge ready\r\n");
#endif

#if defined(BOARD_USB_VIA_GPIO)
    usb_force_reconnect_pulse();
#endif

    composite_usb_init(0, 0);

    board_emac_rmii_gpio_init(BSP_EMAC_RMII_DEFAULT_PORT);
    board_emac_mdio_gpio_init(BSP_EMAC_MDIO_DEFAULT_PORT);
    eth_emac_init();
    eth_emac_event_cb_register(usbd_emac_event_cb);

    LOG_I("Create USB device cdc_ecm <-> emac task\r\n");
    xTaskCreate(usbd_ecm_emac_task, (char *)"usbd_ecm_emac", 1024, NULL, 15, &usbd_ecm_emac_handle);

#if UAC_ACTIVE && USB_ETHERNET_UAC_MIC_ENABLE
    if (audio_mic_handle == NULL) {
        xTaskCreate(audio_mic_task, (char *)"uac_mic", 1024, NULL, 14, &audio_mic_handle);
    }
#endif

#if UAC_ACTIVE && USB_ETHERNET_UAC_SPEAKER_ENABLE && USB_ETHERNET_BLE_ENABLE
    if (audio_ble_voice_tx_handle == NULL) {
        xTaskCreate(audio_ble_voice_tx_task, (char *)"ble_voice_tx", 1024, NULL, 13, &audio_ble_voice_tx_handle);
    }
#endif
}

void usb_ecm_uac_device_stop(void)
{
    if (usbd_ecm_emac_handle) {
        vTaskDelete(usbd_ecm_emac_handle);
        usbd_ecm_emac_handle = NULL;
    }

#if UAC_ACTIVE && USB_ETHERNET_UAC_MIC_ENABLE
    if (audio_mic_handle) {
        vTaskDelete(audio_mic_handle);
        audio_mic_handle = NULL;
    }
#endif

#if UAC_ACTIVE && USB_ETHERNET_UAC_SPEAKER_ENABLE && USB_ETHERNET_BLE_ENABLE
    if (audio_ble_voice_tx_handle) {
        vTaskDelete(audio_ble_voice_tx_handle);
        audio_ble_voice_tx_handle = NULL;
    }
    audio_ble_voice_tx_reset();
#endif

#if UAC_ACTIVE && USB_ETHERNET_UAC_MIC_ENABLE && USB_ETHERNET_BLE_ENABLE
    usb_ecm_ble_set_channel_data_callback(USB_ECM_BLE_CHANNEL_VOICE, NULL);
    audio_ble_voice_reset();
#endif

    eth_emac_deinit();
    usbd_deinitialize(0);
}
