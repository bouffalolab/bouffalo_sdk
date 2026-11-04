#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ble_common.h"
#include "ble_notify.h"

static struct bt_gatt_discover_params g_discover_params;
static uint16_t g_svc_start;
static uint16_t g_svc_end;
static uint16_t g_peer_not_handle;
static SemaphoreHandle_t g_tx_wake;

static void ble_start_discovery(uint8_t type, uint16_t start_handle, uint16_t end_handle);

/* Host work-queue completion wakes a sender waiting for a free TX buffer. */
static void ble_notify_done(struct bt_conn *conn, void *user_data)
{
    (void)conn;
    (void)user_data;

    if (g_tx_wake) {
        xSemaphoreGive(g_tx_wake);
    }
}

/* Sender writes the discovered peer value.
 * The 20/1000 ms per-fragment budgets only limit retries after -ENOMEM;
 * they do not bound stack-internal K_FOREVER allocations in the GATT APIs. */
int ble_notify_send_frame(const uint8_t *data, size_t len, uint32_t timeout_ms)
{
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeout_ms);
    int err;

    if (!data || len == 0) {
        return -1;
    }

    for (;;) {
        struct bt_conn *conn;
        uint16_t peer_handle;
        TickType_t now;
        unsigned int key;

        /* Re-read the live connection and destination on every attempt. */
        key = irq_lock();
        conn = ble_common_connection();
        peer_handle = g_peer_not_handle;
        irq_unlock(key);
        if (!conn || !peer_handle) {
            return -ENOTCONN;
        }
        err = bt_gatt_write_without_response_cb(conn, peer_handle, data, len,
                            false, ble_notify_done, NULL);
        if (err == 0) {
            return 0;
        }
        if (err != -ENOMEM) {
            printf("[BLE] GATT send failed err=%d len=%u\r\n", err, (unsigned int)len);
            return err;
        }

        now = xTaskGetTickCount();
        if ((int32_t)(deadline - now) <= 0) {
            return -ETIMEDOUT;
        }

        xSemaphoreTake(g_tx_wake, deadline - now);
    }
}

static uint8_t ble_discover_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               struct bt_gatt_discover_params *params)
{
    struct bt_gatt_service_val *svc;
    struct bt_gatt_chrc *chrc;

    if (!attr) {
        if (params->type == BT_GATT_DISCOVER_PRIMARY) {
            if (g_svc_start && g_svc_end) {
                ble_start_discovery(BT_GATT_DISCOVER_CHARACTERISTIC, g_svc_start, g_svc_end);
            }
        }

        return BT_GATT_ITER_STOP;
    }

    if (params->type == BT_GATT_DISCOVER_PRIMARY) {
        svc = attr->user_data;
        g_svc_start = attr->handle;
        g_svc_end = svc->end_handle;
        return BT_GATT_ITER_CONTINUE;
    }

    if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
        chrc = attr->user_data;

        if (!bt_uuid_cmp(chrc->uuid, BT_UUID_BLE_COMMON_NOT)) {
            if (!(chrc->properties & BT_GATT_CHRC_WRITE_WITHOUT_RESP)) {
                printf("[BLE] peer characteristic does not support WNR\r\n");
                return BT_GATT_ITER_STOP;
            }
            g_peer_not_handle = chrc->value_handle;
            printf("[BLE] peer value ready handle=0x%04x\r\n", g_peer_not_handle);
        }

        return BT_GATT_ITER_CONTINUE;
    }

    (void)conn;
    return BT_GATT_ITER_CONTINUE;
}

static void ble_start_discovery(uint8_t type, uint16_t start_handle, uint16_t end_handle)
{
    int err;

    memset(&g_discover_params, 0, sizeof(g_discover_params));
    g_discover_params.func = ble_discover_cb;
    g_discover_params.start_handle = start_handle;
    g_discover_params.end_handle = end_handle;
    g_discover_params.type = type;

    if (type == BT_GATT_DISCOVER_PRIMARY) {
        g_discover_params.uuid = BT_UUID_BLE_COMMON_SVC;
    } else {
        g_discover_params.uuid = NULL;
    }

    err = bt_gatt_discover(ble_common_connection(), &g_discover_params);
    if (err) {
        printf("[BLE] discover type=%u failed err=%d\r\n", type, err);
    }
}

void ble_notify_connected(void)
{
    g_svc_start = 0;
    g_svc_end = 0;
    g_peer_not_handle = 0;
    /* Discover the writable peer value. */
    ble_start_discovery(BT_GATT_DISCOVER_PRIMARY, 0x0001, 0xffff);
}

void ble_notify_disconnected(void)
{
    unsigned int key = irq_lock();

    g_peer_not_handle = 0;
    irq_unlock(key);

    if (g_tx_wake) {
        xSemaphoreGive(g_tx_wake);
    }
}

int ble_notify_init(void)
{
    if (!g_tx_wake) {
        g_tx_wake = xSemaphoreCreateBinary();
        if (!g_tx_wake) {
            printf("[BLE] create tx_wake sem failed\r\n");
            return -1;
        }
    }
    return 0;
}