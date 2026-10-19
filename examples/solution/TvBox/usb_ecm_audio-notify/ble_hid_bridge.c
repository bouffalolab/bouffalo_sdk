#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "usb_ecm_ble.h"
#include "usb_ecm_uac_device.h"

#define DBG_TAG "BLE_HID"
#include "log.h"

#ifndef USB_ETHERNET_HID_ENABLE
#define USB_ETHERNET_HID_ENABLE 1
#endif

#ifndef USB_ETHERNET_BLE_ENABLE
#define USB_ETHERNET_BLE_ENABLE 1
#endif

#if USB_ETHERNET_BLE_ENABLE && USB_ETHERNET_HID_ENABLE
#define BLE_HID_TASK_STACK       1024
#define BLE_HID_QUEUE_LEN        32
#define BLE_HID_PRESS_DELAY_MS   20
#define HID_MODIFIER_LEFT_SHIFT  0x02

struct ble_hid_key_event {
    uint8_t modifier;
    uint8_t key_code;
};

static QueueHandle_t g_ble_hid_queue;

static bool ble_hid_map_ascii(uint8_t ch, struct ble_hid_key_event *event)
{
    if (!event) {
        return false;
    }

    event->modifier = 0;
    event->key_code = 0;

    if (ch >= 'a' && ch <= 'z') {
        event->key_code = (uint8_t)(0x04u + (ch - 'a'));
        return true;
    }

    if (ch >= 'A' && ch <= 'Z') {
        event->modifier = HID_MODIFIER_LEFT_SHIFT;
        event->key_code = (uint8_t)(0x04u + (ch - 'A'));
        return true;
    }

    if (ch >= '1' && ch <= '9') {
        event->key_code = (uint8_t)(0x1eu + (ch - '1'));
        return true;
    }

    switch (ch) {
        case '0':
            event->key_code = 0x27;
            return true;
        case ' ':
            event->key_code = 0x2c;
            return true;
        case '\r':
        case '\n':
            event->key_code = 0x28;
            return true;
        case '\b':
        case 0x7f:
            event->key_code = 0x2a;
            return true;
        case '-':
            event->key_code = 0x2d;
            return true;
        case '_':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x2d;
            return true;
        case '=':
            event->key_code = 0x2e;
            return true;
        case '+':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x2e;
            return true;
        case '[':
            event->key_code = 0x2f;
            return true;
        case '{':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x2f;
            return true;
        case ']':
            event->key_code = 0x30;
            return true;
        case '}':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x30;
            return true;
        case '\\':
            event->key_code = 0x31;
            return true;
        case '|':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x31;
            return true;
        case ';':
            event->key_code = 0x33;
            return true;
        case ':':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x33;
            return true;
        case '\'':
            event->key_code = 0x34;
            return true;
        case '"':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x34;
            return true;
        case '`':
            event->key_code = 0x35;
            return true;
        case '~':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x35;
            return true;
        case ',':
            event->key_code = 0x36;
            return true;
        case '<':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x36;
            return true;
        case '.':
            event->key_code = 0x37;
            return true;
        case '>':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x37;
            return true;
        case '/':
            event->key_code = 0x38;
            return true;
        case '?':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x38;
            return true;
        case '!':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x1e;
            return true;
        case '@':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x1f;
            return true;
        case '#':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x20;
            return true;
        case '$':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x21;
            return true;
        case '%':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x22;
            return true;
        case '^':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x23;
            return true;
        case '&':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x24;
            return true;
        case '*':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x25;
            return true;
        case '(':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x26;
            return true;
        case ')':
            event->modifier = HID_MODIFIER_LEFT_SHIFT;
            event->key_code = 0x27;
            return true;
        default:
            return false;
    }
}

static int ble_hid_send_report_retry(const uint8_t report[8])
{
    for (uint8_t retry = 0; retry < 20; retry++) {
        if (usb_ecm_uac_hid_send_report(report) == 0) {
            return 0;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return -1;
}

static void ble_hid_enqueue_bytes(const uint8_t *data, size_t len)
{
    struct ble_hid_key_event event;

    if (!data || len == 0 || !g_ble_hid_queue) {
        return;
    }

    if (len > BLE_HID_QUEUE_LEN) {
        LOG_W("HID input %u bytes > queue %u, excess bytes will be dropped\r\n",
              (unsigned int)len, BLE_HID_QUEUE_LEN);
    }

    for (size_t i = 0; i < len; i++) {
        if (!ble_hid_map_ascii(data[i], &event)) {
            continue;
        }

        (void)xQueueSend(g_ble_hid_queue, &event, 0);
    }
}

static void ble_hid_task(void *param)
{
    static const uint8_t release_report[8] = { 0 };
    struct ble_hid_key_event event;

    (void)param;

    while (1) {
        if (xQueueReceive(g_ble_hid_queue, &event, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        uint8_t press_report[8] = { event.modifier, 0x00, event.key_code, 0x00,
                                    0x00,           0x00, 0x00,           0x00 };

        if (ble_hid_send_report_retry(press_report) != 0) {
            continue;
        }

        vTaskDelay(pdMS_TO_TICKS(BLE_HID_PRESS_DELAY_MS));
        ble_hid_send_report_retry(release_report);
    }
}

static void ble_hid_channel_rx(usb_ecm_ble_channel_t channel, const uint8_t *data, size_t len)
{
    (void)channel;
    ble_hid_enqueue_bytes(data, len);
}

void ble_hid_bridge_init(void)
{
    BaseType_t ret;

    g_ble_hid_queue = xQueueCreate(BLE_HID_QUEUE_LEN, sizeof(struct ble_hid_key_event));
    if (!g_ble_hid_queue) {
        LOG_E("queue create failed\r\n");
        return;
    }

    ret = xTaskCreate(ble_hid_task, (char *)"ble_hid", BLE_HID_TASK_STACK, NULL,
                      configMAX_PRIORITIES - 4, NULL);
    if (ret != pdPASS) {
        LOG_E("task create failed\r\n");
        vQueueDelete(g_ble_hid_queue);
        g_ble_hid_queue = NULL;
        return;
    }

    usb_ecm_ble_set_channel_data_callback(USB_ECM_BLE_CHANNEL_KEY, ble_hid_channel_rx);
    LOG_I("BLE KEY -> USB HID bridge ready\r\n");
}
#else
void ble_hid_bridge_init(void)
{
}
#endif
