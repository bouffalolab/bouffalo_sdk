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
#include "ble_common.h"
#include "usb_composite.h"

#define DBG_TAG "USB_ETH"
#include "log.h"

#define USBD_VID       0xFFFF
#define USBD_PID       0xFFFF
#define USBD_MAX_POWER 500

#define CDC_ECM_IN_EP  0x81
#define CDC_ECM_OUT_EP 0x02
#define HID_INT_EP 0x83
#define AUDIO_MIC_IN_EP 0x84
#if defined(BL616)
#define USB_ETHERNET_ECM_NOTIFY_ENABLE 0
#define CDC_ECM_INT_EP 0x85
#else
#define USB_ETHERNET_ECM_NOTIFY_ENABLE 1
#define CDC_ECM_INT_EP 0x85
#endif
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

#define HID_INT_EP_SIZE               8
#define HID_INT_EP_INTERVAL           10
#define HID_KEYBOARD_REPORT_DESC_SIZE 63

#define BLE_VOICE_BYTES_PER_MS ((UAC_AUDIO_FREQ * UAC_AUDIO_FRAME_SIZE * UAC_AUDIO_CHANNELS) / 1000U)
#define AUDIO_PACKET_SIZE ((uint32_t)BLE_VOICE_BYTES_PER_MS)
/* Keep about 3 seconds of 16 kHz mono s16le PCM in the BLE->USB mic jitter buffer. */
#define BLE_VOICE_MIC_RB_BYTES (BLE_VOICE_BYTES_PER_MS * 3000U)
/* Prefill about 180 ms of audio before the USB mic endpoint starts draining. */
#define BLE_VOICE_MIC_PREFILL_BYTES (BLE_VOICE_BYTES_PER_MS * 180U)
#define BLE_VOICE_SEVERE_UNDERRUN_PACKETS 16U
#define BLE_VOICE_EDGE_FADE_SAMPLES 8U

#define ECM_INTERFACE_COUNT 2
#define AUDIO_AC_INTF       ECM_INTERFACE_COUNT

#define AUDIO_MIC_INTF (AUDIO_AC_INTF + 1)
#define UAC_MIC_AC_SIZE (AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC + AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(UAC_AUDIO_CHANNELS, 1) + AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)
#define UAC_INTERFACE_COUNT 2
#define UAC_AC_SIZE         (AUDIO_SIZEOF_AC_HEADER_DESC(1) + UAC_MIC_AC_SIZE)
#define UAC_CONFIG_SIZE     (AUDIO_AC_DESCRIPTOR_LEN(1) + UAC_MIC_AC_SIZE + AUDIO_AS_DESCRIPTOR_LEN(1))

#define HID_INTF        (ECM_INTERFACE_COUNT + UAC_INTERFACE_COUNT)
#define HID_CONFIG_SIZE HID_KEYBOARD_DESCRIPTOR_LEN

#define USB_INTF_COUNT  (ECM_INTERFACE_COUNT + UAC_INTERFACE_COUNT + 1)
#define ECM_CONFIG_SIZE CDC_ECM_DESCRIPTOR_LEN
#if defined(BL616)
_Static_assert((CDC_ECM_IN_EP & 0x0f) <= 4 && (CDC_ECM_OUT_EP & 0x0f) <= 4 &&
               (AUDIO_MIC_IN_EP & 0x0f) <= 4 && (HID_INT_EP & 0x0f) <= 4,
               "BL616 active USB endpoints must be EP1..EP4");
#endif
#define USB_CONFIG_SIZE (9 + ECM_CONFIG_SIZE + UAC_CONFIG_SIZE + HID_CONFIG_SIZE)

static void usb_task_kick(TaskHandle_t handle)
{
    if (handle == NULL) {
        return;
    }

    if (xPortIsInsideInterrupt()) {
        BaseType_t higher_priority_task_woken = pdFALSE;
        vTaskNotifyGiveFromISR(handle, &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    } else {
        xTaskNotifyGive(handle);
    }
}

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
};

static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, USB_INTF_COUNT, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ECM_DESCRIPTOR_INIT(0x00, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP, CDC_MAX_MPS, CDC_ECM_MAC_STRING_INDEX),
    AUDIO_AC_DESCRIPTOR_INIT(AUDIO_AC_INTF, UAC_INTERFACE_COUNT, UAC_AC_SIZE, 0x00, AUDIO_MIC_INTF),
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x01, AUDIO_INTERM_MIC, UAC_AUDIO_CHANNELS, UAC_CHANNEL_CONFIG),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x01, 0x01, UAC_FEATURE_CTRL),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x03, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID),
    AUDIO_AS_DESCRIPTOR_INIT(AUDIO_MIC_INTF, 0x03, UAC_AUDIO_CHANNELS, UAC_AUDIO_FRAME_SIZE, UAC_AUDIO_RESOLUTION_BIT,
                             AUDIO_MIC_IN_EP, 0x05, AUDIO_PACKET_SIZE, UAC_EP_INTERVAL, AUDIO_SAMPLE_FREQ_3B(UAC_AUDIO_FREQ)),
    HID_KEYBOARD_DESCRIPTOR_INIT(HID_INTF, 0x01, HID_KEYBOARD_REPORT_DESC_SIZE, HID_INT_EP,
                                 HID_INT_EP_SIZE, HID_INT_EP_INTERVAL),
};

_Static_assert(sizeof(config_descriptor) == USB_CONFIG_SIZE, "USB configuration descriptor length mismatch");

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

static struct usbd_interface hid_intf;
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_write_buffer[USB_ALIGN_UP(HID_INT_EP_SIZE, CONFIG_USB_ALIGN_SIZE)];
static volatile bool hid_busy_flag;

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

int usb_composite_hid_send_report(const uint8_t report[HID_INT_EP_SIZE])
{
    uintptr_t flags = bflb_irq_save();
    int ret;

    /* Core updates these states before event callbacks. Keep the check and
     * submission atomic against USB reset/disconnect/suspend interrupts.
     * This descriptor does not advertise remote wakeup: wait for host resume. */
    if (!report || !usb_device_is_configured(0) || usb_device_is_suspend(0) || hid_busy_flag) {
        bflb_irq_restore(flags);
        return -1;
    }

    memcpy(hid_write_buffer, report, HID_INT_EP_SIZE);
    hid_busy_flag = true;
    ret = usbd_ep_start_write(0, HID_INT_EP, hid_write_buffer, HID_INT_EP_SIZE);
    if (ret < 0) {
        hid_busy_flag = false;
    }
    bflb_irq_restore(flags);
    if (ret < 0) {
        LOG_W("HID EP start write failed ep=0x%02x\r\n", HID_INT_EP);
        return -1;
    }

    return 0;
}

static struct usbd_interface audio_ac_intf;
static struct usbd_interface audio_mic_intf;
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t audio_mic_write_buffer[USB_ALIGN_UP(AUDIO_PACKET_SIZE, CONFIG_USB_ALIGN_SIZE)];
static volatile bool audio_mic_streaming;
static volatile bool audio_mic_ep_busy;
static TaskHandle_t audio_mic_handle;

static Ring_Buffer_Type audio_ble_voice_rb;
static uint8_t audio_ble_voice_storage[BLE_VOICE_MIC_RB_BYTES];
static volatile uintptr_t audio_ble_voice_irq_flag;
static volatile bool audio_ble_voice_seen;
static volatile bool audio_ble_voice_prefilled;
static uint32_t audio_ble_voice_consecutive_underrun;
static int16_t audio_ble_voice_last_sample;
static bool audio_ble_voice_have_last_sample;
static bool audio_ble_voice_resume_fade_pending;

static struct audio_entity_info audio_entity_table[] = {
    { .bEntityId = AUDIO_IN_FU_ID, .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT, .ep = AUDIO_MIC_IN_EP },
};

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
    audio_ble_voice_consecutive_underrun = 0U;
    audio_ble_voice_last_sample = 0;
    audio_ble_voice_have_last_sample = false;
    audio_ble_voice_resume_fade_pending = false;
}

static void audio_ble_voice_conceal(uint8_t *buffer, uint32_t len)
{
    int16_t *samples = (int16_t *)buffer;
    uint32_t sample_count = len / sizeof(int16_t);
    uint32_t fade_samples;

    if (!buffer || len == 0U) {
        return;
    }

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

static void audio_ble_voice_rx(ble_common_channel_t channel, const uint8_t *data, size_t len)
{
    uint32_t empty;
    uint32_t to_write;

    (void)channel;

    if (!data || len == 0U || (len & 1U) != 0U) {
        return;
    }

    empty = Ring_Buffer_Get_Empty_Length(&audio_ble_voice_rb);
    to_write = (len > empty) ? empty : (uint32_t)len;
    if (to_write > 0U) {
        (void)Ring_Buffer_Write(&audio_ble_voice_rb, data, to_write);
        audio_ble_voice_seen = true;
    }

    if (to_write < len) {
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

static void audio_mic_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    audio_mic_ep_busy = false;
    usb_task_kick(audio_mic_handle);
}

static struct usbd_endpoint audio_mic_in_ep = {
    .ep_addr = AUDIO_MIC_IN_EP,
    .ep_cb = audio_mic_in_callback,
};

static void audio_fill_mic_packet(uint8_t *buffer, uint32_t len)
{
    if (audio_ble_voice_read(buffer, len)) {
        return;
    }
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
        }

        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(2));
    }
}

void usbd_audio_open(uint8_t busid, uint8_t intf)
{
    if (intf == AUDIO_MIC_INTF) {
        audio_mic_streaming = true;
        audio_mic_ep_busy = false;
        audio_ble_voice_reset();
        LOG_I("UAC mic open\r\n");
        usb_task_kick(audio_mic_handle);
        return;
    }

    (void)busid;
}

void usbd_audio_close(uint8_t busid, uint8_t intf)
{
    (void)busid;

    if (intf == AUDIO_MIC_INTF) {
        audio_mic_streaming = false;
        audio_mic_ep_busy = false;
        audio_ble_voice_reset();
        LOG_I("UAC mic close\r\n");
        return;
    }
}

uint32_t usbd_audio_get_sampling_freq(uint8_t busid, uint8_t ep)
{
    (void)busid;
    (void)ep;

    return UAC_AUDIO_FREQ;
}

static volatile bool usb_ecm_ready_flag;

static TaskHandle_t usbd_ecm_emac_handle;

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
            /* Suspend does not cancel an in-flight IN report. Keep its buffer
             * busy until completion after resume, or reset/disconnect. */
            if (event != USBD_EVENT_SUSPEND) {
                hid_busy_flag = false;
            }
            audio_mic_streaming = false;
            audio_mic_ep_busy = false;
            usb_task_kick(usbd_ecm_emac_handle);
            break;

        case USBD_EVENT_RESUME:
            usb_ecm_ready_flag = true;
            usb_task_kick(usbd_ecm_emac_handle);
            break;

        case USBD_EVENT_CONFIGURED:
            LOG_I("USB composite configured\r\n");
            usb_ecm_ready_flag = true;
            hid_busy_flag = false;
            audio_mic_ep_busy = false;
            usb_task_kick(usbd_ecm_emac_handle);
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

    const uint8_t audio_entity_count = sizeof(audio_entity_table) / sizeof(audio_entity_table[0]);
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &audio_ac_intf, 0x0100, audio_entity_table, audio_entity_count));
    usbd_add_interface(busid, usbd_audio_init_intf(busid, &audio_mic_intf, 0x0100, audio_entity_table, audio_entity_count));
    usbd_add_endpoint(busid, &audio_mic_in_ep);

    usbd_add_interface(busid, usbd_hid_init_intf(busid, &hid_intf, hid_keyboard_report_desc,
                                                 HID_KEYBOARD_REPORT_DESC_SIZE));
    usbd_add_endpoint(busid, &hid_in_ep);

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
    usb_task_kick(usbd_ecm_emac_handle);
}

static void usbd_ecm_in_emac_rx_polling(void)
{
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
            if (usbd_cdc_ecm_start_write((uint8_t *)(uintptr_t)trans_desc.buff_addr, trans_desc.data_len) < 0) {
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
    usb_task_kick(usbd_ecm_emac_handle);
}

static void usbd_ecm_out_emac_tx_polling(void)
{
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
            if (usbd_cdc_ecm_start_read((uint8_t *)(uintptr_t)trans_desc.buff_addr, EMAC_TX_BUFF_SIZE) < 0) {
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

}

/************************* ECM-EMAC state machine *******************************/
#define ECM_EMAC_STA_STOP             0
#define ECM_EMAC_STA_START            1
#define ECM_EMAC_STA_WAIT_USBD_CFG    2
#define ECM_EMAC_STA_WAIT_EPHY_LINKUP 3
#define ECM_EMAC_STA_DATA_POLLING     4
static volatile uint32_t usbd_ecm_emac_sta = ECM_EMAC_STA_STOP;

static void usbd_ecm_emac_polling(void)
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
#if USB_ETHERNET_ECM_NOTIFY_ENABLE
            usbd_cdc_ecm_set_connect(false, NULL);
#endif
            usbd_ecm_emac_sta = ECM_EMAC_STA_WAIT_EPHY_LINKUP;
            break;

        case ECM_EMAC_STA_WAIT_EPHY_LINKUP:
            if (!usb_ecm_ready_flag) {
                usbd_ecm_emac_sta = ECM_EMAC_STA_START;
                break;
            }

            if (eth_link_state_update()) {
#if USB_ETHERNET_ECM_NOTIFY_ENABLE
                uint32_t speed_table[2] = { 100U * 1000U * 1000U, 100U * 1000U * 1000U };
                usbd_cdc_ecm_set_connect(true, speed_table);
#endif
                usbd_ecm_emac_sta = ECM_EMAC_STA_DATA_POLLING;
            } else {
#if USB_ETHERNET_ECM_NOTIFY_ENABLE
                usbd_cdc_ecm_set_connect(false, NULL);
#endif
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
                eth_emac_info_dump();
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

}

static void usbd_emac_event_cb(uint32_t event)
{
    (void)event;
    usb_task_kick(usbd_ecm_emac_handle);
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

void usb_composite_start(void)
{
    usb_ecm_ready_flag = false;

    Ring_Buffer_Init(&audio_ble_voice_rb, audio_ble_voice_storage, sizeof(audio_ble_voice_storage),
                     audio_ble_voice_rb_lock, audio_ble_voice_rb_unlock);
    audio_ble_voice_reset();
    ble_common_set_channel_data_callback(BLE_COMMON_CHANNEL_VOICE, audio_ble_voice_rx);
    LOG_I("BLE VOICE -> UAC mic bridge ready\r\n");

#if defined(BOARD_USB_VIA_GPIO)
    usb_force_reconnect_pulse();
#endif

    composite_usb_init(0, 0);

    board_emac_rmii_gpio_init(0);
    board_emac_mdio_gpio_init(0);
    eth_emac_init();
    eth_emac_event_cb_register(usbd_emac_event_cb);

    LOG_I("Create USB device cdc_ecm <-> emac task\r\n");
    xTaskCreate(usbd_ecm_emac_task, (char *)"usbd_ecm_emac", 1024, NULL, 15, &usbd_ecm_emac_handle);

    if (audio_mic_handle == NULL) {
        xTaskCreate(audio_mic_task, (char *)"uac_mic", 1024, NULL, 14, &audio_mic_handle);
    }

}

void usb_composite_stop(void)
{
    if (usbd_ecm_emac_handle) {
        vTaskDelete(usbd_ecm_emac_handle);
        usbd_ecm_emac_handle = NULL;
    }

    if (audio_mic_handle) {
        vTaskDelete(audio_mic_handle);
        audio_mic_handle = NULL;
    }

    ble_common_set_channel_data_callback(BLE_COMMON_CHANNEL_VOICE, NULL);
    audio_ble_voice_reset();

    eth_emac_deinit();
    usbd_deinitialize(0);
}
