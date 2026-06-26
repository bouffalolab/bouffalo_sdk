#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bflb_mtimer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#include "hci_driver.h"

#if defined(BL616CL) || defined(BL616) || defined(BL618DG)
#include "btble_lib_api.h"
#include "rfparam_adapter.h"
#elif defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#endif

#include "usb_ecm_ble.h"

#ifndef USB_ETHERNET_BLE_DEVICE_NAME
#define USB_ETHERNET_BLE_DEVICE_NAME "USB-ECM-AUD"
#endif

#ifndef USB_ECM_BLE_LOCAL_DEVICE_NAME
#define USB_ECM_BLE_LOCAL_DEVICE_NAME USB_ETHERNET_BLE_DEVICE_NAME
#endif

#ifndef USB_ECM_BLE_PEER_DEVICE_NAME
#define USB_ECM_BLE_PEER_DEVICE_NAME USB_ETHERNET_BLE_DEVICE_NAME
#endif

#ifndef USB_ECM_BLE_ENABLE_ADV
#define USB_ECM_BLE_ENABLE_ADV 1
#endif

#ifndef USB_ECM_BLE_ENABLE_SCAN
#define USB_ECM_BLE_ENABLE_SCAN 1
#endif

#ifndef USB_ECM_BLE_CONNECT_BY_ADDR_ORDER
#define USB_ECM_BLE_CONNECT_BY_ADDR_ORDER 1
#endif

#define BLE_CHAN_TASK_STACK      1024
#define BLE_CHAN_CONNECT_TIMEOUT 400
/* Reconnect watchdog poll period. A disconnect is recovered within at most
 * this interval; tune against reconnect latency vs. idle radio power. */
#define BLE_RECONNECT_POLL_MS    1000
#define BLE_CHAN_AUDIO_CONN_INT_MIN 0x0006
#define BLE_CHAN_AUDIO_CONN_INT_MAX 0x000C

#ifndef USB_ETHERNET_BLE_REASM_BYTES
#define USB_ETHERNET_BLE_REASM_BYTES 1200
#endif

#define BLE_CHAN_FRAME_MAGIC0 0xb6
#define BLE_CHAN_FRAME_MAGIC1 0x16
#define BLE_CHAN_FRAME_VERSION 1
#define BLE_CHAN_FRAME_HDR_LEN 12
#define BLE_CHAN_ATT_PAYLOAD_MAX 497
#define BLE_CHAN_REASM_BUF_SIZE USB_ETHERNET_BLE_REASM_BYTES

/* Per-fragment TX flow-control budget. Voice is live audio (32 kB/s, ~one
 * 640 B frame every 20 ms) so a fragment that cannot be queued within roughly
 * a frame period is dropped rather than allowed to pile up latency. KEY/USER
 * are control text: wait hard so the LL-reliable link delivers them losslessly
 * without any app-level ACK. */
#define BLE_TX_VOICE_TIMEOUT_MS    20
#define BLE_TX_RELIABLE_TIMEOUT_MS 1000

/* BT_CONN_ROLE_MASTER/SLAVE are enum constants (MASTER=0, SLAVE=1) from
 * conn.h, invisible to the preprocessor. Do NOT redefine them here with
 * #ifndef fallbacks -- that silently overrode the enum with inverted values
 * (MASTER=1, SLAVE=0) and broke role detection, so neither side reconnected.
 * Only alias CENTRAL, which the stack does not provide. */
#ifndef BT_CONN_ROLE_CENTRAL
#define BT_CONN_ROLE_CENTRAL BT_CONN_ROLE_MASTER
#endif

static struct bt_uuid_128 g_usb_ecm_ble_svc_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x07af27a5, 0x9c22, 0x11ea, 0x9afe, 0x02fcdc4e7412));
static struct bt_uuid_128 g_usb_ecm_ble_not_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x07af27a9, 0x9c22, 0x11ea, 0x9afe, 0x02fcdc4e7412));

#define BT_UUID_USB_ECM_BLE_SVC (&g_usb_ecm_ble_svc_uuid.uuid)
#define BT_UUID_USB_ECM_BLE_NOT (&g_usb_ecm_ble_not_uuid.uuid)

enum {
    BLE_CHAN_ATTR_SVC = 0,
    BLE_CHAN_ATTR_NOT_CHRC,
    BLE_CHAN_ATTR_NOT_VALUE,
    BLE_CHAN_ATTR_NOT_CCC,
};

static struct bt_conn *g_conn;
static bt_addr_le_t g_local_addr;
static struct bt_gatt_exchange_params g_exchange_params;
static struct bt_gatt_discover_params g_discover_params;
static struct bt_gatt_subscribe_params g_subscribe_params;
static usb_ecm_ble_rx_cb_t g_channel_data_cb[USB_ECM_BLE_CHANNEL_MAX + 1];
static TaskHandle_t g_ble_task;
static uint16_t g_mtu = 23;
static uint16_t g_svc_start;
static uint16_t g_svc_end;
static uint16_t g_peer_not_handle;
static uint16_t g_peer_not_ccc_handle;
static bool g_scanning;
static bool g_connecting;
static bool g_service_registered;
static bool g_notify_enabled;
static bool g_peer_subscribed;
static bool g_is_central;
static uint16_t g_conn_interval;
static uint8_t g_conn_tx_phy;
static uint8_t g_conn_rx_phy;
static uint16_t g_tx_seq[USB_ECM_BLE_CHANNEL_MAX + 1];
static bt_addr_le_t g_peer_addr;
static bool g_peer_addr_valid;
static bool g_advertising;
static bool g_bt_ready;
static SemaphoreHandle_t g_tx_wake;

struct ble_att_target {
    struct bt_conn *conn;
    bool notify_enabled;
};

struct ble_reasm_state {
    uint16_t seq;
    uint16_t total_len;
    uint16_t received;
    uint8_t active;
    uint8_t data[BLE_CHAN_REASM_BUF_SIZE];
};

static struct ble_reasm_state g_reasm[USB_ECM_BLE_CHANNEL_MAX + 1];

static void ble_start_adv(void);
static void ble_start_scan(void);
static void ble_start_discovery(uint8_t type, uint16_t start_handle, uint16_t end_handle);
static struct bt_gatt_attr *ble_notify_attr(void);
static bool ble_att_target_acquire(struct ble_att_target *target);
static void ble_att_target_release(struct ble_att_target *target);
static void ble_request_audio_conn_params(struct bt_conn *conn);
static void ble_conn_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency,
                                   uint16_t timeout);
static void ble_conn_phy_updated(struct bt_conn *conn, uint8_t tx_phy, uint8_t rx_phy);
static void ble_conn_datalen_updated(struct bt_conn *conn, uint16_t tx_octets, uint16_t tx_time,
                                     uint16_t rx_octets, uint16_t rx_time);

#if USB_ECM_BLE_CONNECT_BY_ADDR_ORDER
static int addr_compare(const bt_addr_le_t *a, const bt_addr_le_t *b)
{
    int cmp = memcmp(a->a.val, b->a.val, sizeof(a->a.val));

    if (cmp != 0) {
        return cmp;
    }

    return (int)a->type - (int)b->type;
}
#endif

static void print_addr(const char *prefix, const bt_addr_le_t *addr)
{
    char str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(addr, str, sizeof(str));
    printf("[BLE] %s%s\r\n", prefix, str);
}

static size_t payload_len(size_t len)
{
    size_t max_len = (g_mtu > 3) ? (g_mtu - 3) : 20;

    if (max_len > BLE_CHAN_ATT_PAYLOAD_MAX) {
        max_len = BLE_CHAN_ATT_PAYLOAD_MAX;
    }

    return (len < max_len) ? len : max_len;
}

static size_t att_max_len(void)
{
    return payload_len((size_t)-1);
}

static void ble_request_conn_params(struct bt_conn *conn, uint16_t interval_min,
                                    uint16_t interval_max, const char *tag)
{
    struct bt_le_conn_param param = {
        .interval_min = interval_min,
        .interval_max = interval_max,
        .latency = 0,
        .timeout = BLE_CHAN_CONNECT_TIMEOUT,
    };
    int err;

    if (!conn) {
        return;
    }

    err = bt_conn_le_param_update(conn, &param);
    if (err) {
        printf("[BLE] %s conn param update failed err=%d int=[%u,%u]\r\n",
               tag, err, param.interval_min, param.interval_max);
    } else {
        printf("[BLE] %s conn param update req int=[%u,%u]\r\n",
               tag, param.interval_min, param.interval_max);
    }
}

static void ble_request_audio_conn_params(struct bt_conn *conn)
{
    ble_request_conn_params(conn, BLE_CHAN_AUDIO_CONN_INT_MIN,
                            BLE_CHAN_AUDIO_CONN_INT_MAX, "audio");
}

static void ble_request_2m_phy(struct bt_conn *conn)
{
    int err;

    if (!conn) {
        return;
    }

    err = hci_le_set_phy(conn, 0U, BT_HCI_LE_PHY_PREFER_2M,
                         BT_HCI_LE_PHY_PREFER_2M, BT_HCI_LE_PHY_CODED_ANY);
    if (err) {
        printf("[BLE] phy 2M request failed err=%d\r\n", err);
    } else {
        printf("[BLE] phy 2M request pending\r\n");
    }
}

static const char *ble_channel_name(usb_ecm_ble_channel_t channel)
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

static uint16_t get_le16(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static void put_le16(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8);
}

static bool ble_att_target_acquire(struct ble_att_target *target)
{
    unsigned int key;

    if (!target) {
        return false;
    }

    memset(target, 0, sizeof(*target));

    key = irq_lock();
    if (g_conn) {
        target->conn = g_conn;
        target->notify_enabled = g_notify_enabled;
    }
    irq_unlock(key);

    return target->conn != NULL;
}

static void ble_att_target_release(struct ble_att_target *target)
{
    if (!target || !target->conn) {
        return;
    }

    target->conn = NULL;
}

/* Notify completion: invoked from the host work-queue thread once a queued
 * notification buffer has been sent and freed. Wake any sender blocked on a
 * full TX pool so it can retry. */
static void ble_notify_done(struct bt_conn *conn, void *user_data)
{
    (void)conn;
    (void)user_data;

    if (g_tx_wake) {
        xSemaphoreGive(g_tx_wake);
    }
}

/* Notify-only transport: every packet is sent via the local Notify
 * characteristic once the peer has subscribed. When the TX buffer pool is
 * momentarily exhausted (-ENOMEM) we block until a notification completes (or
 * until timeout_ms elapses) instead of dropping the fragment. That gives real
 * backpressure; combined with the LL's reliable in-order delivery, a fully
 * queued message arrives loss-free without any app-level ACK. */
static int ble_att_send(const uint8_t *data, size_t len, uint32_t timeout_ms)
{
    struct ble_att_target target;
    struct bt_gatt_notify_params params;
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeout_ms);
    int err;

    if (!data || len == 0) {
        return -1;
    }

    memset(&params, 0, sizeof(params));
    params.attr = ble_notify_attr();
    params.data = data;
    params.len = len;
    params.func = ble_notify_done;

    for (;;) {
        TickType_t now;

        /* Re-read the live connection each attempt: it may drop while we wait. */
        if (!ble_att_target_acquire(&target)) {
            return -1;
        }
        if (!target.notify_enabled) {
            ble_att_target_release(&target);
            return -1;
        }

        err = bt_gatt_notify_cb(target.conn, &params);
        ble_att_target_release(&target);

        if (err == 0) {
            return 0;
        }
        if (err != -ENOMEM) {
            /* -ENOTCONN / -EINVAL ... not a transient buffer shortage. */
            printf("[BLE] server notify failed err=%d len=%u\r\n", err, (unsigned int)len);
            return err;
        }

        now = xTaskGetTickCount();
        if ((int32_t)(deadline - now) <= 0) {
            return -ETIMEDOUT;
        }

        /* Buffers full: wait for a completion (bounded by the remaining budget). */
        xSemaphoreTake(g_tx_wake, deadline - now);
    }
}

static bool ble_frame_parse(const uint8_t *data, uint16_t len, usb_ecm_ble_channel_t *channel,
                            uint16_t *seq, uint16_t *total_len, uint16_t *offset,
                            const uint8_t **payload, uint16_t *payload_size)
{
    if (!data || len < BLE_CHAN_FRAME_HDR_LEN) {
        return false;
    }

    if (data[0] != BLE_CHAN_FRAME_MAGIC0 || data[1] != BLE_CHAN_FRAME_MAGIC1 ||
        data[2] != BLE_CHAN_FRAME_VERSION) {
        return false;
    }

    *channel = (usb_ecm_ble_channel_t)data[3];
    *seq = get_le16(&data[4]);
    *total_len = get_le16(&data[6]);
    *offset = get_le16(&data[8]);
    *payload_size = get_le16(&data[10]);

    if (*channel == 0 || *channel > USB_ECM_BLE_CHANNEL_MAX ||
        *payload_size > len - BLE_CHAN_FRAME_HDR_LEN) {
        return false;
    }

    if ((uint32_t)*offset + *payload_size > *total_len) {
        return false;
    }

    *payload = &data[BLE_CHAN_FRAME_HDR_LEN];
    return true;
}

static void ble_receive_frame(const uint8_t *data, uint16_t len)
{
    usb_ecm_ble_channel_t channel;
    struct ble_reasm_state *state;
    const uint8_t *payload;
    uint16_t seq;
    uint16_t total_len;
    uint16_t offset;
    uint16_t payload_size;

    if (!ble_frame_parse(data, len, &channel, &seq, &total_len, &offset, &payload, &payload_size)) {
        printf("[BLE] rx drop unframed len=%u\r\n", (unsigned int)len);
        return;
    }

    if (total_len > BLE_CHAN_REASM_BUF_SIZE) {
        printf("[BLE] %s rx drop seq=%u total=%u reasm_buf=%u\r\n",
               ble_channel_name(channel), seq, total_len, BLE_CHAN_REASM_BUF_SIZE);
        return;
    }

    state = &g_reasm[channel];
    if (!state->active || state->seq != seq || state->total_len != total_len || offset == 0) {
        if (offset != 0) {
            printf("[BLE] %s rx drop seq=%u off=%u without start\r\n",
                   ble_channel_name(channel), seq, offset);
            return;
        }
        state->active = 1;
        state->seq = seq;
        state->total_len = total_len;
        state->received = 0;
    }

    if (offset != state->received) {
        printf("[BLE] %s rx drop seq=%u expected=%u off=%u\r\n",
               ble_channel_name(channel), seq, state->received, offset);
        state->active = 0;
        return;
    }

    memcpy(&state->data[offset], payload, payload_size);
    state->received = (uint16_t)(offset + payload_size);

    if (state->received >= state->total_len) {
        if (g_channel_data_cb[channel]) {
            g_channel_data_cb[channel](channel, state->data, state->total_len);
        }
        state->active = 0;
    }
}

static void ble_notify_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    (void)attr;

    g_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    g_peer_subscribed = g_notify_enabled;
    printf("[BLE] server notify %s\r\n", g_notify_enabled ? "enabled" : "disabled");
}

static struct bt_gatt_attr g_ble_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_USB_ECM_BLE_SVC),
    BT_GATT_CHARACTERISTIC(BT_UUID_USB_ECM_BLE_NOT,
                           BT_GATT_CHRC_NOTIFY,
                           0,
                           NULL,
                           NULL,
                           NULL),
    BT_GATT_CCC(ble_notify_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
};

static struct bt_gatt_service g_ble_service = BT_GATT_SERVICE(g_ble_attrs);

static struct bt_gatt_attr *ble_notify_attr(void)
{
    return &g_ble_attrs[BLE_CHAN_ATTR_NOT_VALUE];
}

static void ble_exchange_mtu_cb(struct bt_conn *conn, uint8_t err,
                                struct bt_gatt_exchange_params *params)
{
    (void)params;

    if (!err && conn == g_conn) {
        g_mtu = bt_gatt_get_mtu(conn);
        printf("[BLE] mtu=%u\r\n", g_mtu);
    } else {
        printf("[BLE] mtu exchange failed err=%u\r\n", err);
    }
}

static uint8_t ble_notify_rx(struct bt_conn *conn, struct bt_gatt_subscribe_params *params,
                             const void *data, uint16_t length)
{
    (void)conn;

    if (!data && !params->value) {
        params->value_handle = 0;
        printf("[BLE] client unsubscribed\r\n");
        return BT_GATT_ITER_STOP;
    }

    if (!data) {
        printf("[BLE] client subscribe active\r\n");
        return BT_GATT_ITER_CONTINUE;
    }

    ble_receive_frame((const uint8_t *)data, length);

    return BT_GATT_ITER_CONTINUE;
}

static int ble_subscribe_peer(void)
{
    int err;

    if (!g_conn || !g_peer_not_handle || !g_peer_not_ccc_handle) {
        return -1;
    }

    memset(&g_subscribe_params, 0, sizeof(g_subscribe_params));
    g_subscribe_params.notify = ble_notify_rx;
    g_subscribe_params.value_handle = g_peer_not_handle;
    g_subscribe_params.ccc_handle = g_peer_not_ccc_handle;
    g_subscribe_params.value = BT_GATT_CCC_NOTIFY;

    err = bt_gatt_subscribe(g_conn, &g_subscribe_params);
    if (err) {
        printf("[BLE] subscribe failed err=%d value=0x%04x ccc=0x%04x\r\n",
               err, g_peer_not_handle, g_peer_not_ccc_handle);
    } else {
        printf("[BLE] subscribe pending value=0x%04x ccc=0x%04x\r\n",
               g_peer_not_handle, g_peer_not_ccc_handle);
    }

    return err;
}

static uint8_t ble_discover_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               struct bt_gatt_discover_params *params)
{
    struct bt_gatt_service_val *svc;
    struct bt_gatt_chrc *chrc;

    if (!attr) {
        if (params->type == BT_GATT_DISCOVER_PRIMARY) {
            printf("[BLE] service discovery complete start=0x%04x end=0x%04x\r\n",
                   g_svc_start, g_svc_end);
            if (g_svc_start && g_svc_end) {
                ble_start_discovery(BT_GATT_DISCOVER_CHARACTERISTIC, g_svc_start, g_svc_end);
            }
        } else if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
            printf("[BLE] characteristic discovery complete notify=0x%04x\r\n",
                   g_peer_not_handle);
            if (g_peer_not_handle) {
                ble_start_discovery(BT_GATT_DISCOVER_DESCRIPTOR, g_peer_not_handle + 1, g_svc_end);
            }
        } else if (params->type == BT_GATT_DISCOVER_DESCRIPTOR) {
            printf("[BLE] descriptor discovery complete ccc=0x%04x\r\n", g_peer_not_ccc_handle);
            ble_subscribe_peer();
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

        if (!bt_uuid_cmp(chrc->uuid, BT_UUID_USB_ECM_BLE_NOT)) {
            g_peer_not_handle = chrc->value_handle;
        }

        return BT_GATT_ITER_CONTINUE;
    }

    if (params->type == BT_GATT_DISCOVER_DESCRIPTOR) {
        if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CCC)) {
            g_peer_not_ccc_handle = attr->handle;
            return BT_GATT_ITER_CONTINUE;
        }
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
        g_discover_params.uuid = BT_UUID_USB_ECM_BLE_SVC;
    } else {
        g_discover_params.uuid = NULL;
    }

    err = bt_gatt_discover(g_conn, &g_discover_params);
    if (err) {
        printf("[BLE] discover type=%u failed err=%d\r\n", type, err);
    }
}

static bool scan_name_cb(struct bt_data *data, void *user_data)
{
    char *name = user_data;
    size_t len;

    if (data->type != BT_DATA_NAME_SHORTENED && data->type != BT_DATA_NAME_COMPLETE) {
        return true;
    }

    len = data->data_len;
    if (len > 30) {
        len = 30;
    }

    memcpy(name, data->data, len);
    name[len] = '\0';
    return false;
}

static void ble_device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t evtype,
                             struct net_buf_simple *buf)
{
    char name[31] = { 0 };
    char peer[BT_ADDR_LE_STR_LEN];
    struct bt_le_conn_param param = {
        .interval_min = BT_GAP_INIT_CONN_INT_MIN,
        .interval_max = BT_GAP_INIT_CONN_INT_MAX,
        .latency = 0,
        .timeout = BLE_CHAN_CONNECT_TIMEOUT,
    };
    struct bt_conn *conn;
    int err;

    if (g_conn || g_connecting) {
        return;
    }

    if (g_peer_addr_valid) {
        /* Already bound: reconnect only to the same peer, ignore the name. */
        if (bt_addr_le_cmp(addr, &g_peer_addr) != 0) {
            return;
        }
    } else {
        bt_data_parse(buf, scan_name_cb, name);
        if (strcmp(name, USB_ECM_BLE_PEER_DEVICE_NAME) != 0) {
            return;
        }

#if USB_ECM_BLE_CONNECT_BY_ADDR_ORDER
        if (addr_compare(&g_local_addr, addr) >= 0) {
            return;
        }
#endif
    }

    bt_addr_le_to_str(addr, peer, sizeof(peer));
    printf("[BLE] found peer %s rssi=%d evt=%u name=%s\r\n", peer, rssi, evtype, name);

    err = bt_le_scan_stop();
    if (err) {
        printf("[BLE] stop scan failed err=%d\r\n", err);
        return;
    }
    g_scanning = false;

    g_connecting = true;
    conn = bt_conn_create_le(addr, &param);
    if (!conn) {
        g_connecting = false;
        printf("[BLE] connect create failed\r\n");
        return; /* watchdog re-scans */
    }

    printf("[BLE] connect pending\r\n");
}

static void ble_start_scan(void)
{
#if !USB_ECM_BLE_ENABLE_SCAN
    return;
#else
    struct bt_le_scan_param scan_param = {
        .type = BT_LE_SCAN_TYPE_ACTIVE,
        .filter_dup = BT_LE_SCAN_FILTER_DUPLICATE,
        .interval = BT_GAP_SCAN_FAST_INTERVAL,
        .window = BT_GAP_SCAN_FAST_WINDOW,
    };
    int err;

    if (g_conn || g_scanning || g_connecting) {
        return;
    }

    err = bt_le_scan_start(&scan_param, ble_device_found);
    if (err) {
        printf("[BLE] scan start failed err=%d\r\n", err);
    } else {
        g_scanning = true;
        printf("[BLE] scan started\r\n");
    }
#endif
}

static void ble_start_adv(void)
{
#if !USB_ECM_BLE_ENABLE_ADV
    return;
#else
    static const char local_name[] = USB_ECM_BLE_LOCAL_DEVICE_NAME;
    struct bt_le_adv_param param = {
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        .options = BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME,
    };
    struct bt_data adv_data[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL),
    };
    struct bt_data adv_rsp[] = {
        BT_DATA(BT_DATA_NAME_COMPLETE, local_name, sizeof(local_name) - 1),
    };
    int err;

    if (g_conn || g_advertising) {
        return;
    }

    err = bt_le_adv_start(&param, adv_data, sizeof(adv_data) / sizeof(adv_data[0]),
                          adv_rsp, sizeof(adv_rsp) / sizeof(adv_rsp[0]));
    if (err) {
        printf("[BLE] advertising start failed err=%d\r\n", err);
    } else {
        g_advertising = true;
        printf("[BLE] advertising as %s\r\n", local_name);
    }
#endif
}

static void ble_connected(struct bt_conn *conn, uint8_t err)
{
    struct bt_conn_info info;
    unsigned int key;

    if (bt_conn_get_info(conn, &info) || info.type != BT_CONN_TYPE_LE) {
        return;
    }

    g_connecting = false;

    if (err) {
        printf("[BLE] connect failed err=%u\r\n", err);
        return; /* watchdog re-scans */
    }

    key = irq_lock();
    if (!g_conn) {
        g_conn = conn;
    }
    irq_unlock(key);

    g_is_central = (info.role == BT_CONN_ROLE_CENTRAL);
    bt_addr_le_copy(&g_peer_addr, bt_conn_get_dst(conn));
    g_peer_addr_valid = true;
    g_advertising = false;
    print_addr("connected peer ", bt_conn_get_dst(conn));
    g_mtu = bt_gatt_get_mtu(conn);

    (void)bt_le_set_data_len(conn, 0x00fb, 0x0848);
    ble_request_audio_conn_params(conn);
    ble_request_2m_phy(conn);
    memset(&g_exchange_params, 0, sizeof(g_exchange_params));
    g_exchange_params.func = ble_exchange_mtu_cb;
    err = bt_gatt_exchange_mtu(conn, &g_exchange_params);
    if (err) {
        printf("[BLE] mtu exchange start failed err=%u\r\n", err);
    }

    g_svc_start = 0;
    g_svc_end = 0;
    g_peer_not_handle = 0;
    g_peer_not_ccc_handle = 0;
    ble_start_discovery(BT_GATT_DISCOVER_PRIMARY, 0x0001, 0xffff);
}

static void ble_disconnected(struct bt_conn *conn, uint8_t reason)
{
    struct bt_conn_info info;
    unsigned int key;

    if (bt_conn_get_info(conn, &info) || info.type != BT_CONN_TYPE_LE) {
        return;
    }

    printf("[BLE] disconnected reason=%u\r\n", reason);

    key = irq_lock();
    if (g_conn == conn) {
        g_conn = NULL;
    }
    g_notify_enabled = false;
    g_peer_subscribed = false;
    g_is_central = false;
    g_conn_interval = 0;
    g_conn_tx_phy = 0;
    g_conn_rx_phy = 0;
    g_peer_not_handle = 0;
    g_peer_not_ccc_handle = 0;
    irq_unlock(key);

    g_connecting = false;
    g_scanning = false;
    g_advertising = false;
    g_mtu = 23;

    /* Wake any sender blocked on a full TX pool so it stops waiting on a link
     * that is gone (its next attempt sees no connection and bails). */
    if (g_tx_wake) {
        xSemaphoreGive(g_tx_wake);
    }

    /* Do NOT restart scan/adv here: starting the controller from inside the
     * disconnect callback is fragile. The watchdog poll in ble_stack_task
     * brings the link back up, and retries if a single restart fails. The
     * RAM-pinned g_peer_addr is kept on purpose so we rejoin the same peer. */
}

static struct bt_conn_cb g_conn_callbacks = {
    .connected = ble_connected,
    .disconnected = ble_disconnected,
    .le_param_updated = ble_conn_param_updated,
    .le_phy_updated = ble_conn_phy_updated,
#if defined(CONFIG_USER_DATA_LEN_UPDATE)
    .le_datalen_updated = ble_conn_datalen_updated,
#endif
};

static void ble_conn_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency,
                                   uint16_t timeout)
{
    char addr[BT_ADDR_LE_STR_LEN];

    g_conn_interval = interval;
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printf("[BLE] conn param updated peer=%s int=%u lat=%u to=%u\r\n",
           addr, interval, latency, timeout);
}

static const char *ble_phy_name(uint8_t phy)
{
    switch (phy) {
        case BT_HCI_LE_PHY_1M:
            return "1M";
        case BT_HCI_LE_PHY_2M:
            return "2M";
        case BT_HCI_LE_PHY_CODED:
            return "CODED";
        default:
            return "UNKNOWN";
    }
}

static void ble_conn_phy_updated(struct bt_conn *conn, uint8_t tx_phy, uint8_t rx_phy)
{
    char addr[BT_ADDR_LE_STR_LEN];

    g_conn_tx_phy = tx_phy;
    g_conn_rx_phy = rx_phy;
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printf("[BLE] phy updated peer=%s tx=%s(%u) rx=%s(%u)\r\n",
           addr, ble_phy_name(tx_phy), tx_phy, ble_phy_name(rx_phy), rx_phy);
}

#if defined(CONFIG_USER_DATA_LEN_UPDATE)
static void ble_conn_datalen_updated(struct bt_conn *conn, uint16_t tx_octets, uint16_t tx_time,
                                     uint16_t rx_octets, uint16_t rx_time)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printf("[BLE] data len updated peer=%s tx_oct=%u tx_time=%u rx_oct=%u rx_time=%u\r\n",
           addr, tx_octets, tx_time, rx_octets, rx_time);
}
#endif

int usb_ecm_ble_send_channel(usb_ecm_ble_channel_t channel, const uint8_t *data, size_t len)
{
    uint8_t frame[BLE_CHAN_ATT_PAYLOAD_MAX];
    size_t mtu_payload;
    size_t chunk_max;
    size_t offset = 0;
    uint16_t seq;
    int err;
    uint32_t tx_timeout;

    if (channel == 0 || channel > USB_ECM_BLE_CHANNEL_MAX || !data || len == 0) {
        return -1;
    }

    if (len > UINT16_MAX || len > BLE_CHAN_REASM_BUF_SIZE) {
        printf("[BLE] %s tx reject len=%u reasm_buf=%u\r\n",
               ble_channel_name(channel), (unsigned int)len, BLE_CHAN_REASM_BUF_SIZE);
        return -1;
    }

    mtu_payload = att_max_len();
    if (mtu_payload <= BLE_CHAN_FRAME_HDR_LEN) {
        return -1;
    }
    chunk_max = mtu_payload - BLE_CHAN_FRAME_HDR_LEN;
    if (chunk_max > sizeof(frame) - BLE_CHAN_FRAME_HDR_LEN) {
        chunk_max = sizeof(frame) - BLE_CHAN_FRAME_HDR_LEN;
    }

    seq = ++g_tx_seq[channel];
    tx_timeout = (channel == USB_ECM_BLE_CHANNEL_VOICE) ? BLE_TX_VOICE_TIMEOUT_MS
                                                        : BLE_TX_RELIABLE_TIMEOUT_MS;

    while (offset < len) {
        size_t chunk = len - offset;

        if (chunk > chunk_max) {
            chunk = chunk_max;
        }

        frame[0] = BLE_CHAN_FRAME_MAGIC0;
        frame[1] = BLE_CHAN_FRAME_MAGIC1;
        frame[2] = BLE_CHAN_FRAME_VERSION;
        frame[3] = (uint8_t)channel;
        put_le16(&frame[4], seq);
        put_le16(&frame[6], (uint16_t)len);
        put_le16(&frame[8], (uint16_t)offset);
        put_le16(&frame[10], (uint16_t)chunk);
        memcpy(&frame[BLE_CHAN_FRAME_HDR_LEN], &data[offset], chunk);

        err = ble_att_send(frame, BLE_CHAN_FRAME_HDR_LEN + chunk, tx_timeout);
        if (err) {
            printf("[BLE] %s tx fragment failed err=%d seq=%u off=%u len=%u\r\n",
                   ble_channel_name(channel), err, seq, (unsigned int)offset,
                   (unsigned int)chunk);
            return err;
        }

        /* No artificial inter-fragment delay: ble_att_send() already applies
         * buffer-level backpressure, so fragments stream out as fast as the
         * link drains. */
        offset += chunk;
    }
    return 0;
}

void usb_ecm_ble_set_channel_data_callback(usb_ecm_ble_channel_t channel, usb_ecm_ble_rx_cb_t cb)
{
    if (channel == 0 || channel > USB_ECM_BLE_CHANNEL_MAX) {
        return;
    }

    g_channel_data_cb[channel] = cb;
}

static void bt_enable_cb(int err)
{
    if (err) {
        printf("[BLE] bt_enable failed err=%d\r\n", err);
        return;
    }

    bt_get_local_public_address(&g_local_addr);
    print_addr("local ", &g_local_addr);

    if (!g_service_registered) {
        g_service_registered = true;
        bt_gatt_service_register(&g_ble_service);
    }

    bt_conn_cb_register(&g_conn_callbacks);
    ble_start_adv();
    ble_start_scan();
    g_bt_ready = true;
}

static void ble_stack_task(void *param)
{
    (void)param;

#if defined(BL702) || defined(BL602)
    ble_controller_init(configMAX_PRIORITIES - 1);
#else
    btble_controller_init(configMAX_PRIORITIES - 1);
#endif

    hci_driver_init();
    bt_enable(bt_enable_cb);

    /* Reconnect watchdog: whenever we are not connected, make sure the radio
     * is scanning (central) and/or advertising (peripheral). All (re)starts
     * happen here -- never inside the connection callbacks -- and a restart
     * that fails once is simply retried on the next tick. */
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(BLE_RECONNECT_POLL_MS));

        if (!g_bt_ready || g_conn || g_connecting) {
            continue;
        }

        ble_start_adv();
        ble_start_scan();
    }
}

int usb_ecm_ble_stack_init(void)
{
    if (g_ble_task) {
        return 0;
    }

    if (!g_tx_wake) {
        g_tx_wake = xSemaphoreCreateBinary();
        if (!g_tx_wake) {
            printf("[BLE] create tx_wake sem failed\r\n");
            return -1;
        }
    }

    if (xTaskCreate(ble_stack_task, (char *)"ble_start", BLE_CHAN_TASK_STACK, NULL,
                    configMAX_PRIORITIES - 2, &g_ble_task) != pdPASS) {
        printf("[BLE] create ble_start task failed\r\n");
        g_ble_task = NULL;
        return -1;
    }

    return 0;
}
