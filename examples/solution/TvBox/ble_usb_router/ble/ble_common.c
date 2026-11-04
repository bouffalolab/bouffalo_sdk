#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#include "hci_driver.h"

#if defined(BL616CL) || defined(BL616) || defined(BL618DG)
#include "btble_lib_api.h"
#elif defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#endif

#include "ble_common.h"
#if USB_ECM_BLE_ENABLE_WRITE_RX
#include "ble_remote.h"
#endif
#if USB_ECM_BLE_ENABLE_WRITE_TX
#include "ble_notify.h"
#endif

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
#define BLE_CHAN_REASM_BUF_SIZE USB_ETHERNET_BLE_REASM_BYTES

/* Per-fragment outer -ENOMEM retry budgets; these do not bound stack-internal
 * waits. Voice uses a shorter budget than KEY/USER to limit queued latency.
 * Successful sends mean local acceptance, not peer application delivery. */
#define BLE_TX_VOICE_TIMEOUT_MS    20
#define BLE_TX_RELIABLE_TIMEOUT_MS 1000

struct bt_uuid_128 ble_common_service_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x07af27a5, 0x9c22, 0x11ea, 0x9afe, 0x02fcdc4e7412));
struct bt_uuid_128 ble_common_notify_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x07af27a9, 0x9c22, 0x11ea, 0x9afe, 0x02fcdc4e7412));


static struct bt_conn *g_conn;
static struct bt_gatt_exchange_params g_exchange_params;
/* Wire channel IDs start at 1; storage indices start at 0. */
#if USB_ECM_BLE_ENABLE_WRITE_RX
static ble_common_rx_cb_t g_channel_data_cb[BLE_COMMON_CHANNEL_MAX];
#endif
static TaskHandle_t g_ble_task;
#if defined(BL616)
static bool g_controller_init_done;
#endif
static uint16_t g_mtu = 23;
static bool g_scanning;
static bool g_connecting;
#if USB_ECM_BLE_ENABLE_WRITE_TX
static uint16_t g_tx_seq[BLE_COMMON_CHANNEL_MAX];
#endif
static bt_addr_le_t g_peer_addr;
static bool g_peer_addr_valid;
static bool g_advertising;
static bool g_bt_ready;

#if USB_ECM_BLE_ENABLE_WRITE_RX
struct ble_reasm_state {
    uint16_t seq;
    uint16_t total_len;
    uint16_t received;
    uint8_t active;
    uint8_t data[BLE_CHAN_REASM_BUF_SIZE];
};

static struct ble_reasm_state g_reasm[BLE_COMMON_CHANNEL_MAX];
#endif

static void ble_start_adv(void);
static void ble_start_scan(void);

#if USB_ECM_BLE_ENABLE_WRITE_TX
static size_t att_max_len(void)
{
    size_t max_len = (g_mtu > 3) ? (g_mtu - 3) : 20;

    if (max_len > BLE_CHAN_ATT_PAYLOAD_MAX) {
        max_len = BLE_CHAN_ATT_PAYLOAD_MAX;
    }

    return max_len;
}
#endif

static const char *ble_channel_name(ble_common_channel_t channel)
{
    switch (channel) {
        case BLE_COMMON_CHANNEL_VOICE:
            return "voice";
        case BLE_COMMON_CHANNEL_KEY:
            return "key";
        case BLE_COMMON_CHANNEL_USER:
            return "user";
        default:
            return "unknown";
    }
}

#if USB_ECM_BLE_ENABLE_WRITE_RX
static uint16_t get_le16(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}
#endif

#if USB_ECM_BLE_ENABLE_WRITE_TX
static void put_le16(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8);
}
#endif

struct bt_conn *ble_common_connection(void)
{
    return g_conn;
}

#if USB_ECM_BLE_ENABLE_WRITE_RX
static bool ble_frame_parse(const uint8_t *data, uint16_t len, ble_common_channel_t *channel,
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

    *channel = (ble_common_channel_t)data[3];
    *seq = get_le16(&data[4]);
    *total_len = get_le16(&data[6]);
    *offset = get_le16(&data[8]);
    *payload_size = get_le16(&data[10]);

    if (*channel == 0 || *channel > BLE_COMMON_CHANNEL_MAX ||
        *payload_size > len - BLE_CHAN_FRAME_HDR_LEN) {
        return false;
    }

    if ((uint32_t)*offset + *payload_size > *total_len) {
        return false;
    }

    *payload = &data[BLE_CHAN_FRAME_HDR_LEN];
    return true;
}

void ble_common_receive_frame(const uint8_t *data, uint16_t len)
{
    ble_common_channel_t channel;
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

    state = &g_reasm[channel - 1];
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
        if (g_channel_data_cb[channel - 1]) {
            g_channel_data_cb[channel - 1](channel, state->data, state->total_len);
        }
        state->active = 0;
    }
}

static struct bt_gatt_attr g_ble_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_BLE_COMMON_SVC),
    BT_GATT_CHARACTERISTIC(BT_UUID_BLE_COMMON_NOT,
                           BT_GATT_CHRC_WRITE |
                               BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL,
                           ble_remote_write_rx,
                           NULL),
};

static struct bt_gatt_service g_ble_service = BT_GATT_SERVICE(g_ble_attrs);
#endif

static void ble_exchange_mtu_cb(struct bt_conn *conn, uint8_t err,
                                struct bt_gatt_exchange_params *params)
{
    (void)params;

    if (!err && conn == g_conn) {
        g_mtu = bt_gatt_get_mtu(conn);
    } else {
        printf("[BLE] mtu exchange failed err=%u\r\n", err);
    }
}

#if USB_ECM_BLE_ENABLE_SCAN
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
    struct bt_le_conn_param param = {
        .interval_min = BT_GAP_INIT_CONN_INT_MIN,
        .interval_max = BT_GAP_INIT_CONN_INT_MAX,
        .latency = 0,
        .timeout = BLE_CHAN_CONNECT_TIMEOUT,
    };
    struct bt_conn *conn;
    int err;

    (void)rssi;
    (void)evtype;

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

    }

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
}
#endif

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
    }
#endif
}

static void ble_connected(struct bt_conn *conn, uint8_t err)
{
    struct bt_conn_info info;
    struct bt_le_conn_param param = {
        .interval_min = BLE_CHAN_AUDIO_CONN_INT_MIN,
        .interval_max = BLE_CHAN_AUDIO_CONN_INT_MAX,
        .latency = 0,
        .timeout = BLE_CHAN_CONNECT_TIMEOUT,
    };
    unsigned int key;
    int ret;

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

    bt_addr_le_copy(&g_peer_addr, bt_conn_get_dst(conn));
    g_peer_addr_valid = true;
    g_advertising = false;
    g_mtu = bt_gatt_get_mtu(conn);

    (void)bt_le_set_data_len(conn, 0x00fb, 0x0848);
    ret = bt_conn_le_param_update(conn, &param);
    if (ret) {
        printf("[BLE] audio conn param update failed err=%d int=[%u,%u]\r\n",
               ret, param.interval_min, param.interval_max);
    }

    ret = hci_le_set_phy(conn, 0U, BT_HCI_LE_PHY_PREFER_2M,
                        BT_HCI_LE_PHY_PREFER_2M, BT_HCI_LE_PHY_CODED_ANY);
    if (ret) {
        printf("[BLE] phy 2M request failed err=%d\r\n", ret);
    }
    memset(&g_exchange_params, 0, sizeof(g_exchange_params));
    g_exchange_params.func = ble_exchange_mtu_cb;
    err = bt_gatt_exchange_mtu(conn, &g_exchange_params);
    if (err) {
        printf("[BLE] mtu exchange start failed err=%u\r\n", err);
    }

#if USB_ECM_BLE_ENABLE_WRITE_TX
    ble_notify_connected();
#endif
}

static void ble_disconnected(struct bt_conn *conn, uint8_t reason)
{
    struct bt_conn_info info;
    unsigned int key;

    (void)reason;

    if (bt_conn_get_info(conn, &info) || info.type != BT_CONN_TYPE_LE) {
        return;
    }

    key = irq_lock();
    if (g_conn == conn) {
        g_conn = NULL;
    }
    irq_unlock(key);

    g_connecting = false;
    g_scanning = false;
    g_advertising = false;
    g_mtu = 23;
#if USB_ECM_BLE_ENABLE_WRITE_RX
    memset(g_reasm, 0, sizeof(g_reasm));
#endif

#if USB_ECM_BLE_ENABLE_WRITE_TX
    /* Wake any sender blocked on a full TX pool so it stops waiting on a link
     * that is gone (its next attempt sees no connection and bails). */
    ble_notify_disconnected();
#endif

    /* Do NOT restart scan/adv here: starting the controller from inside the
     * disconnect callback is fragile. The watchdog poll in ble_stack_task
     * brings the link back up, and retries if a single restart fails. The
     * RAM-pinned g_peer_addr is kept on purpose so we rejoin the same peer. */
}

static struct bt_conn_cb g_conn_callbacks = {
    .connected = ble_connected,
    .disconnected = ble_disconnected,
};

#if USB_ECM_BLE_ENABLE_WRITE_TX
int ble_common_send_channel(ble_common_channel_t channel, const uint8_t *data, size_t len)
{
    uint8_t frame[BLE_CHAN_ATT_PAYLOAD_MAX];
    size_t mtu_payload;
    size_t chunk_max;
    size_t offset = 0;
    uint16_t seq;
    int err;
    uint32_t tx_timeout;

    if (channel == 0 || channel > BLE_COMMON_CHANNEL_MAX || !data || len == 0) {
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

    seq = ++g_tx_seq[channel - 1];
    tx_timeout = (channel == BLE_COMMON_CHANNEL_VOICE) ? BLE_TX_VOICE_TIMEOUT_MS
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

        err = ble_notify_send_frame(frame, BLE_CHAN_FRAME_HDR_LEN + chunk, tx_timeout);
        if (err) {
            printf("[BLE] %s tx fragment failed err=%d seq=%u off=%u len=%u\r\n",
                   ble_channel_name(channel), err, seq, (unsigned int)offset,
                   (unsigned int)chunk);
            return err;
        }

        /* No artificial inter-fragment delay: ble_notify_send_frame() already applies
         * buffer-level backpressure, so fragments stream out as fast as the
         * link drains. */
        offset += chunk;
    }
    return 0;
}
#endif

#if USB_ECM_BLE_ENABLE_WRITE_RX
void ble_common_set_channel_data_callback(ble_common_channel_t channel, ble_common_rx_cb_t cb)
{
    if (channel == 0 || channel > BLE_COMMON_CHANNEL_MAX) {
        return;
    }

    g_channel_data_cb[channel - 1] = cb;
}
#endif

static void bt_enable_cb(int err)
{
    if (err) {
        printf("[BLE] bt_enable failed err=%d\r\n", err);
        return;
    }

#if USB_ECM_BLE_ENABLE_WRITE_RX
    err = bt_gatt_service_register(&g_ble_service);
    if (err) {
        printf("[BLE] GATT register failed err=%d\r\n", err);
        return;
    }
#endif

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
#elif !defined(BL616)
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

int ble_common_stack_init(void)
{
    if (g_ble_task) {
        return 0;
    }

#if USB_ECM_BLE_ENABLE_WRITE_TX
    if (ble_notify_init() != 0) {
        return -1;
    }
#endif

#if defined(BL616)
    /* Finish the USB PHY reset before returning; do not repeat it on task-create retries. */
    if (!g_controller_init_done) {
        btble_controller_init(configMAX_PRIORITIES - 1);
        g_controller_init_done = true;
    }
#endif
    if (xTaskCreate(ble_stack_task, (char *)"ble_start", BLE_CHAN_TASK_STACK, NULL,
                    configMAX_PRIORITIES - 2, &g_ble_task) != pdPASS) {
        printf("[BLE] create ble_start task failed\r\n");
        g_ble_task = NULL;
        return -1;
    }

    return 0;
}
