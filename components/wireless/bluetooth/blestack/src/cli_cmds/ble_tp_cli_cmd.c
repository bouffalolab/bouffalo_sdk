#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#include "FreeRTOS.h"
#include "task.h"
#if defined(CONFIG_SHELL)
#include "shell.h"
#else
#include "cli.h"
#endif /* CONFIG_SHELL */
#include "bl_port.h"
#if defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#else
#include "btble_lib_api.h"
#endif
#include "bluetooth.h"
#include "hci_driver.h"
#include "bt_uuid.h"
#include "bt_errno.h"

typedef struct {
    uint32_t random;
    uint32_t crc;
#if defined(BLE_TEST_SHOW_RSSI)
    int8_t rssi;
#endif /* BLE_TEST_SHOW_RSSI */
} tp_payload_header_t;

typedef enum {
    TP_TEST_NOTIFY,
    TP_TEST_WRITE,
    TP_TEST_NOTIFY_WRITE,
    TP_TEST_MODE_COUNT,
} tp_test_mode_t;

#define TP_NAME_SIZE           30
#define TP_DEVICE_NAME         __TIME__
#define TP_SERVICE_UUID        BT_UUID_DECLARE_16(0x28a5)
#define TP_CHARACTERISTIC_UUID BT_UUID_DECLARE_16(0x28a9)
#define TP_NOTIFY_ATTR_INDEX   (3)
#define TP_RATE_SAMPLE_COUNT   16 /* Must be a power of two. */
#define TP_PAYLOAD_SIZE        244
#define TP_TOTAL_STR_SIZE      21 /* 20 decimal digits for uint64_t + NUL */

/* Per-connection receive statistics, sampled after warm-up. */
typedef struct {
    uint32_t rx_bytes; /* Wraps; accumulate unsigned deltas. */
    uint32_t previous_rx_bytes;
    uint64_t total_bytes;
    uint32_t sample_count;
    uint32_t measurement_start_ms;
    uint32_t first_rx_ms;
    bool has_received; /* A first packet at tick 0 is valid. */
    bool measurement_started;
    bool summary_printed;
    uint32_t rate_samples[TP_RATE_SAMPLE_COUNT]; /* bytes/s */
} tp_rx_stats_t;

typedef enum {
    TP_RX_NOTIFY, /* Master receives notifications. */
    TP_RX_WRITE,  /* Slave receives writes. */
    TP_RX_DIRECTION_COUNT,
} tp_rx_direction_t;

/* All per-connection state, indexed by bt_conn_index(). */
typedef struct {
    struct bt_conn *conn;
    uint8_t role;
    volatile bool tx_running; /* Cleared by stop/disconnect; observed by the TX task. */
    TaskHandle_t tx_task_handle;
    uint16_t write_handle;
    struct bt_gatt_subscribe_params subscribe_params;
    struct bt_gatt_discover_params discover_params;
    struct bt_uuid_16 discovery_uuid;
    tp_rx_stats_t stats[TP_RX_DIRECTION_COUNT];
} tp_session_t;

static uint8_t tp_master_count = 0;
static uint8_t tp_slave_count = 0;
static tp_test_mode_t tp_test_mode = TP_TEST_NOTIFY;
static tp_session_t tp_sessions[CONFIG_BT_MAX_CONN];
static tp_session_t *tp_slave_session; /* CCC callback carries no connection. */
static struct bt_gatt_exchange_params tp_mtu_exchange_params;
static TaskHandle_t tp_report_task_handle;

static void tp_connected_cb(struct bt_conn *conn, uint8_t err);
static void tp_disconnected_cb(struct bt_conn *conn, uint8_t reason);
static void tp_conn_param_updated_cb(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);
#if defined(CONFIG_BT_OBSERVER)
static int tp_start_scanning(void);
#endif
static int tp_write_received_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t length,
                                uint16_t offset, uint8_t flags);
static void tp_ccc_changed_cb(const struct bt_gatt_attr *attr, uint16_t value);
static int tp_start_advertising(void);
static void tp_tx_task(void *task_arg);
static void tp_print_conn_summary(int conn_index, const char *addr);

static struct bt_gatt_attr tp_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(TP_SERVICE_UUID),
    BT_GATT_CHARACTERISTIC(TP_CHARACTERISTIC_UUID, BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE, NULL, tp_write_received_cb, NULL),
    BT_GATT_CCC(tp_ccc_changed_cb, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
};

static struct bt_gatt_service tp_service = BT_GATT_SERVICE(tp_attrs);

static struct bt_conn_cb tp_conn_callbacks = {
    .connected = tp_connected_cb,
    .disconnected = tp_disconnected_cb,
    .le_param_updated = tp_conn_param_updated_cb,
};

/* The payload CRC is the byte-reversed random value. */
static void tp_set_payload_crc(tp_payload_header_t *header)
{
    header->crc = __builtin_bswap32(header->random);
}

static bool tp_is_payload_valid(const tp_payload_header_t *header)
{
    return header->crc == __builtin_bswap32(header->random);
}

static uint32_t tp_rate_sample_index(uint32_t sample_count)
{
    return sample_count & (TP_RATE_SAMPLE_COUNT - 1);
}

static void tp_reset_session(uint8_t conn_index)
{
    memset(&tp_sessions[conn_index], 0, sizeof(tp_sessions[conn_index]));
}

#if defined(CONFIG_BT_OBSERVER)
static int tp_count_master_connections(void)
{
    int count = 0;
    struct bt_conn *conn;

    for (int conn_index = 0; conn_index < CONFIG_BT_MAX_CONN; ++conn_index) {
        conn = bt_conn_lookup_id(conn_index);
        if (!conn) {
            continue;
        }
        if (conn->role == BT_HCI_ROLE_MASTER) {
            count++;
        }
        bt_conn_unref(conn);
    }
    return count;
}
#endif

static struct bt_gatt_attr *tp_get_attr(uint8_t index)
{
    return &tp_attrs[index];
}

/* Called by both receive paths; keep first-packet timing and count coherent. */
static void tp_record_received_bytes(tp_rx_stats_t *stats, uint16_t length)
{
    taskENTER_CRITICAL();
    if (!stats->has_received) {
        stats->first_rx_ms = (uint32_t)xTaskGetTickCount() * portTICK_PERIOD_MS;
        stats->has_received = true;
    }
    stats->rx_bytes += length;
    taskEXIT_CRITICAL();
}

static uint8_t tp_notify_received_cb(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *data,
                                     uint16_t length)
{
    uint8_t conn_index;
    tp_payload_header_t header;

    if (!params->value) {
        printf("Unsubscribed\r\n");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }

    if (data == NULL || length < sizeof(header)) {
        printf("tp_notify_func no data\r\n");
        return BT_GATT_ITER_CONTINUE;
    }

    memcpy(&header, data, sizeof(header));

    if (!tp_is_payload_valid(&header)) {
        printf("Recv notify data, but crc is error\r\n");
        return BT_GATT_ITER_CONTINUE;
    }

#if defined(BLE_TEST_SHOW_RSSI)
    int err;
    int8_t rssi;
    err = bt_le_read_rssi(conn->handle, &rssi);
    if (err) {
        printf("read rssi failed (err %d)\r\n", err);
        rssi = 0;
    }
    printf("recv rssi:%d send rssi:%d\r\n", rssi, header.rssi);
#endif /* BLE_TEST_SHOW_RSSI */

    conn_index = bt_conn_index(conn);
    tp_record_received_bytes(&tp_sessions[conn_index].stats[TP_RX_NOTIFY], length);

    return BT_GATT_ITER_CONTINUE;
}

static uint8_t tp_discover_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                              struct bt_gatt_discover_params *params)
{
    char uuid_text[37];
    char addr[BT_ADDR_LE_STR_LEN];
    uint8_t conn_index;

    conn_index = bt_conn_index(conn);
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!attr) {
        printf("%s Discover complete\r\n", addr);
        (void)memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }

    switch (params->type) {
        case BT_GATT_DISCOVER_CHARACTERISTIC: {
            struct bt_gatt_chrc *gatt_chrc = attr->user_data;
            if (tp_test_mode == TP_TEST_NOTIFY || tp_test_mode == TP_TEST_NOTIFY_WRITE) {
                bt_uuid_to_str(gatt_chrc->uuid, uuid_text, sizeof(uuid_text));
                tp_sessions[conn_index].subscribe_params.ccc_handle = attr->handle + 2;
                tp_sessions[conn_index].subscribe_params.value_handle = attr->handle + 2;
                tp_sessions[conn_index].subscribe_params.value = BT_GATT_CCC_NOTIFY;
                tp_sessions[conn_index].subscribe_params.notify = tp_notify_received_cb;

                int err = bt_gatt_subscribe(conn, &tp_sessions[conn_index].subscribe_params);
                if (err) {
                    printf("Subscribe failed (err %d)\r\n", err);
                }
            }
            if ((tp_test_mode == TP_TEST_WRITE || tp_test_mode == TP_TEST_NOTIFY_WRITE) &&
                !tp_sessions[conn_index].tx_task_handle) {
                /* The master sends writes. */
                tp_sessions[conn_index].write_handle = attr->handle + 1;
                tp_sessions[conn_index].tx_running = true;
                bt_conn_ref(conn); /* released by tx_task when it exits */
                if (xTaskCreate(tp_tx_task, (char *)"ble_tx", 512, &tp_sessions[conn_index], 15,
                                &tp_sessions[conn_index].tx_task_handle) != pdPASS) {
                    printf("creater write task failed\r\n");
                    tp_sessions[conn_index].tx_running = false;
                    tp_sessions[conn_index].tx_task_handle = NULL;
                    bt_conn_unref(conn);
                }
            }
        } break;
        default:
            bt_uuid_to_str(attr->uuid, uuid_text, sizeof(uuid_text));
            printf("%s Discover type 0x%x\r\n", addr, params->type);
            break;
    }

    return BT_GATT_ITER_CONTINUE;
}

static void tp_mtu_exchanged_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
    char addr[BT_ADDR_LE_STR_LEN];
    uint8_t conn_index;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        printf("%s echange mtu size failure, err: %d\r\n", addr, err);
        return;
    }

    printf("%s echange mtu size success, mtu size: %d\r\n", addr, bt_gatt_get_mtu(conn));
    conn_index = bt_conn_index(conn);

    tp_sessions[conn_index].discovery_uuid = *(struct bt_uuid_16 *)TP_CHARACTERISTIC_UUID;
    tp_sessions[conn_index].discover_params.func = tp_discover_cb;
    tp_sessions[conn_index].discover_params.start_handle = 0x0001;
    tp_sessions[conn_index].discover_params.end_handle = 0xffff;
    tp_sessions[conn_index].discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    tp_sessions[conn_index].discover_params.uuid = &tp_sessions[conn_index].discovery_uuid.uuid;

    err = bt_gatt_discover(conn, &tp_sessions[conn_index].discover_params);
    if (err) {
        printf("%s Discover failed (err %d)\r\n", addr, err);
    }
}

static void tp_connected_cb(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    int ret;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        printf("Connected err:0x%x %s role:%d\r\n", err, addr, conn->role);
        if (conn->role == BT_HCI_ROLE_SLAVE) {
            tp_start_advertising();
        }
        return;
    }

    printf("Connected:%s role:%d\r\n", addr, conn->role);

    uint8_t conn_index = bt_conn_index(conn);
    tp_reset_session(conn_index);
    tp_sessions[conn_index].conn = conn;
    tp_sessions[conn_index].role = conn->role;
    if (conn->role == BT_HCI_ROLE_SLAVE) {
        if (tp_slave_count) {
            tp_slave_session = &tp_sessions[conn_index];
        }
        return;
    }
    if (!tp_master_count) {
        return;
    }
#if defined(CONFIG_BT_OBSERVER)
    if (tp_count_master_connections() < tp_master_count) {
        tp_start_scanning();
    }
#endif

    tp_mtu_exchange_params.func = tp_mtu_exchanged_cb;
    ret = bt_gatt_exchange_mtu(conn, &tp_mtu_exchange_params);
    if (ret) {
        printf("%s exchange mtu size failure, err: %d\r\n", addr, ret);
    }
}

static void tp_disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    uint8_t conn_index = bt_conn_index(conn);

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printf("Disconnected:%s role:%d reason:0x%x\r\n", addr, conn->role, reason);

    /* The TX task still uses this session until it releases its connection ref. */
    tp_sessions[conn_index].tx_running = false;
    if (tp_slave_session == &tp_sessions[conn_index]) {
        tp_slave_session = NULL;
    }
    /* Use the callback address: the connection may no longer be discoverable. */
    tp_print_conn_summary(conn_index, addr);
    taskENTER_CRITICAL();
    memset(tp_sessions[conn_index].stats, 0, sizeof(tp_sessions[conn_index].stats));
    taskEXIT_CRITICAL();
}

static void tp_conn_param_updated_cb(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printf("%s conn param updated: int %d lat %d to %d\r\n", addr, interval, latency, timeout);
}

static int tp_write_received_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t length,
                                uint16_t offset, uint8_t flags)
{
    uint8_t conn_index;
    tp_payload_header_t header;

    if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
        return 0;
    }

    if (buf == NULL || length < sizeof(header)) {
        printf("write rx no data\r\n");
        return length;
    }

    memcpy(&header, buf, sizeof(header));

    if (!tp_is_payload_valid(&header)) {
        printf("Recv write rx data, but crc is error\r\n");
        return length;
    }

    conn_index = bt_conn_index(conn);
    tp_record_received_bytes(&tp_sessions[conn_index].stats[TP_RX_WRITE], length);
    return length;
}

/* Clamp the payload to the negotiated MTU and buffer capacity. */
static uint16_t tp_build_payload(char *payload, size_t capacity, struct bt_conn *conn)
{
    tp_payload_header_t header;
    uint16_t payload_len = bt_gatt_get_mtu(conn) - 3;

    if (payload_len > capacity) {
        payload_len = capacity;
    }

    header.random = (uint32_t)random();
    tp_set_payload_crc(&header);
#if defined(BLE_TEST_SHOW_RSSI)
    if (bt_le_read_rssi(conn->handle, &header.rssi)) {
        header.rssi = 0;
    }
#endif /* BLE_TEST_SHOW_RSSI */
    memcpy(payload, &header, sizeof(header));
    return payload_len;
}

/* The starter takes a connection ref; this task releases it on exit. */
static void tp_tx_task(void *task_arg)
{
    int err;
    uint16_t payload_len;
    char payload[TP_PAYLOAD_SIZE] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
    tp_session_t *session = (tp_session_t *)task_arg;
    struct bt_conn *conn = session->conn;
    bool is_slave = (session->role == BT_HCI_ROLE_SLAVE);

    while (session->tx_running) {
        payload_len = tp_build_payload(payload, sizeof(payload), conn);
        if (is_slave) {
            err = bt_gatt_notify(conn, tp_get_attr(TP_NOTIFY_ATTR_INDEX), payload, payload_len);
        } else {
            err = bt_gatt_write_without_response(conn, session->write_handle, payload, payload_len, 0);
        }
        if (err) {
            if (err != -ENOMEM) {
                printf("ble_tx error:%d\r\n", err);
            }
            /* Yield while waiting for a TX buffer or retrying a transient error. */
            vTaskDelay(1);
            continue;
        }
    }

    bt_conn_unref(conn);
    session->tx_task_handle = NULL;
    vTaskDelete(NULL);
}

static void tp_ccc_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    printf("ccc:value=[%d]\r\n", value);
    if (!tp_slave_count || !tp_slave_session) {
        return;
    }

    if (value == BT_GATT_CCC_NOTIFY) {
        if (tp_slave_session->tx_task_handle) {
            return;
        }
        tp_slave_session->tx_running = true;
        bt_conn_ref(tp_slave_session->conn); /* released by tx_task when it exits */
        if (xTaskCreate(tp_tx_task, (char *)"ble_ntf", 512, tp_slave_session, 15, &tp_slave_session->tx_task_handle) ==
            pdPASS) {
            printf("Create notify task success\r\n");
        } else {
            tp_slave_session->tx_running = false;
            tp_slave_session->tx_task_handle = NULL;
            bt_conn_unref(tp_slave_session->conn);
            printf("Create notify task fail\r\n");
        }
    } else {
        tp_slave_session->tx_running = false;
    }
}

static int tp_start_advertising(void)
{
    struct bt_le_adv_param adv_param = {
        .options = BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
    };
    char *adv_name = TP_DEVICE_NAME;
    struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
    };
    printf("start ble adv, name:[%s]\r\n", TP_DEVICE_NAME);
    return bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
}

#if defined(CONFIG_BT_OBSERVER)
static bool tp_parse_adv_name_cb(struct bt_data *data, void *user_data)
{
    char *name = user_data;
    uint8_t len;

    switch (data->type) {
        case BT_DATA_NAME_SHORTENED:
        case BT_DATA_NAME_COMPLETE:
            len = (data->data_len > TP_NAME_SIZE - 1) ? (TP_NAME_SIZE - 1) : (data->data_len);
            memcpy(name, data->data, len);
            return false;
        default:
            return true;
    }
}

static void tp_scan_result_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t event_type, struct net_buf_simple *buf)
{
    char peer_addr[BT_ADDR_LE_STR_LEN];
    char name[TP_NAME_SIZE];
    int err;

    (void)memset(name, 0, sizeof(name));
    bt_data_parse(buf, tp_parse_adv_name_cb, name);
    bt_addr_le_to_str(addr, peer_addr, sizeof(peer_addr));
    if (strcmp(name, TP_DEVICE_NAME) != 0) {
        return;
    }
    printf("[DEVICE]: %s Found. RSSI %i %s \r\n", peer_addr, rssi, name);

    err = bt_le_scan_stop();
    if (err) {
        printf("Stopping scanning failed (err %d)\r\n", err);
    }

#if defined(CONFIG_BT_CENTRAL)
    struct bt_conn *conn;
    struct bt_le_conn_param conn_params = {
        .interval_min = BT_GAP_INIT_CONN_INT_MAX << 1,
        .interval_max = BT_GAP_INIT_CONN_INT_MAX << 1,
        .latency = 0,
        .timeout = 500,
    };

    conn = bt_conn_create_le(addr, &conn_params);

    if (!conn) {
        printf("Connection failed\r\n");
    } else {
        if (conn->state == BT_CONN_CONNECTED) {
            printf("Le link with this peer device has existed\r\n");
        } else {
            printf("Connection pending\r\n");
        }
    }
#endif
}

static int tp_start_scanning(void)
{
    struct bt_le_scan_param scan_param;
    int err;

    scan_param.type = BT_HCI_LE_SCAN_ACTIVE;
    scan_param.filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE;
    scan_param.interval = BT_GAP_SCAN_FAST_INTERVAL;
    scan_param.window = BT_GAP_SCAN_FAST_WINDOW;

    err = bt_le_scan_start(&scan_param, tp_scan_result_cb);
    if (err) {
        printf("Failed to start scan (err %d)\r\n", err);
    }

    return 0;
}
#endif

static void tp_mtu_changed_cb(struct bt_conn *conn, int mtu)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printf("mtu changed:%s mtu:%d\r\n", addr, mtu);
}

static char *tp_get_peer_addr(int conn_index)
{
    struct bt_conn *conn;
    static char addr[BT_ADDR_LE_STR_LEN];

    conn = bt_conn_lookup_id(conn_index);
    if (!conn) {
        return NULL;
    }
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    bt_conn_unref(conn);
    return addr;
}

/* Average the valid samples, keeping only the latest ring contents. */
static uint32_t tp_get_average_rate(const uint32_t *samples, uint32_t sample_count)
{
    uint32_t valid_samples = sample_count > TP_RATE_SAMPLE_COUNT ? TP_RATE_SAMPLE_COUNT : sample_count;
    uint32_t first_sample = sample_count > TP_RATE_SAMPLE_COUNT ? sample_count % TP_RATE_SAMPLE_COUNT : 0;
    uint32_t sum = 0;

    if (valid_samples == 0) {
        return 0;
    }

    for (uint32_t sample_index = 0; sample_index < valid_samples; ++sample_index) {
        sum += samples[(first_sample + sample_index) & (TP_RATE_SAMPLE_COUNT - 1)];
    }

    return sum / valid_samples;
}

/* Report only received bytes: writes on the slave, notifications on the master. */
typedef struct {
    tp_rx_direction_t direction;
    const char *role_name;
    const char *direction_name;
} tp_rx_report_t;

static const tp_rx_report_t tp_rx_reports[] = {
    { TP_RX_WRITE, "Slave", "write  rx" },
    { TP_RX_NOTIFY, "Master", "notify rx" },
};

/* SDK printf may omit long-long support; format the total without %llu. */
static char *tp_format_total_bytes(uint64_t total_bytes, char buffer[TP_TOTAL_STR_SIZE])
{
    char *cursor = buffer + TP_TOTAL_STR_SIZE - 1;

    *cursor = '\0';
    do {
        *--cursor = '0' + total_bytes % 10;
        total_bytes /= 10;
    } while (total_bytes);
    return cursor;
}

static bool tp_print_rx_rate(int conn_index, const tp_rx_report_t *report, uint32_t elapsed_ms)
{
    tp_rx_stats_t *stats = &tp_sessions[conn_index].stats[report->direction];
    const char *addr;
    uint32_t rx_bytes;
    uint32_t rate_bytes_per_sec;
    uint32_t average_bytes_per_sec;
    uint32_t current_ms;
    uint64_t total_bytes;
    char total_text[TP_TOTAL_STR_SIZE];

    addr = tp_get_peer_addr(conn_index);
    if (!addr) {
        return false;
    }

    taskENTER_CRITICAL();
    if (stats->summary_printed || !stats->has_received) {
        taskEXIT_CRITICAL();
        return false;
    }

    current_ms = (uint32_t)xTaskGetTickCount() * portTICK_PERIOD_MS;
    rx_bytes = stats->rx_bytes;
    if (!stats->measurement_started) {
        /* Discard at least one second after the first payload. */
        if ((uint32_t)(current_ms - stats->first_rx_ms) < 1000) {
            taskEXIT_CRITICAL();
            return false;
        }
        stats->measurement_started = true;
        stats->total_bytes = 0;
        stats->previous_rx_bytes = rx_bytes;
        stats->measurement_start_ms = current_ms;
        taskEXIT_CRITICAL();
        return false;
    }

    rate_bytes_per_sec = (uint32_t)((uint64_t)(rx_bytes - stats->previous_rx_bytes) * 1000 / elapsed_ms);
    /* Unsigned subtraction handles a wrap between samples. */
    stats->total_bytes += (uint32_t)(rx_bytes - stats->previous_rx_bytes);
    stats->previous_rx_bytes = rx_bytes;
    stats->rate_samples[tp_rate_sample_index(stats->sample_count++)] = rate_bytes_per_sec;
    total_bytes = stats->total_bytes;
    average_bytes_per_sec = tp_get_average_rate(stats->rate_samples, stats->sample_count);
    taskEXIT_CRITICAL();

    printf("%s:%s %s, total:%s, rate:%luKbps, average:%luKbps\r\n", report->role_name, addr, report->direction_name,
           tp_format_total_bytes(total_bytes, total_text), (unsigned long)(rate_bytes_per_sec * 8 / 1000),
           (unsigned long)(average_bytes_per_sec * 8 / 1000));
    return true;
}

/* Use actual elapsed time to account for scheduling delays. */
static void tp_report_task(void *task_arg)
{
    TickType_t previous_tick = xTaskGetTickCount();

    (void)task_arg;
    for (;;) {
        TickType_t current_tick;
        uint32_t elapsed_ms;
        bool has_output = false;

        vTaskDelay(pdMS_TO_TICKS(1000));
        current_tick = xTaskGetTickCount();
        elapsed_ms = (uint32_t)(current_tick - previous_tick) * portTICK_PERIOD_MS;
        previous_tick = current_tick;
        if (elapsed_ms == 0) {
            continue;
        }

        for (unsigned report_index = 0; report_index < ARRAY_SIZE(tp_rx_reports); ++report_index) {
            for (int conn_index = 0; conn_index < CONFIG_BT_MAX_CONN; ++conn_index) {
                if (tp_print_rx_rate(conn_index, &tp_rx_reports[report_index], elapsed_ms)) {
                    has_output = true;
                }
            }
        }
        if (has_output) {
            printf("\r\n");
        }
    }
}

/* Finalize once, on local stop or peer disconnect, before clearing stats. */
static void tp_print_conn_summary(int conn_index, const char *addr)
{
    uint32_t current_ms = (uint32_t)xTaskGetTickCount() * portTICK_PERIOD_MS;
    bool has_output = false;

    for (unsigned report_index = 0; report_index < ARRAY_SIZE(tp_rx_reports); ++report_index) {
        tp_rx_stats_t *stats = &tp_sessions[conn_index].stats[tp_rx_reports[report_index].direction];
        uint32_t elapsed_ms;
        uint64_t total_bytes;
        char total_text[TP_TOTAL_STR_SIZE];

        taskENTER_CRITICAL();
        if (!stats->measurement_started || stats->summary_printed) {
            taskEXIT_CRITICAL();
            continue;
        }
        elapsed_ms = current_ms - stats->measurement_start_ms;
        if (elapsed_ms == 0) {
            taskEXIT_CRITICAL();
            continue;
        }
        /* Include bytes received since the last periodic sample. */
        total_bytes = stats->total_bytes + (uint32_t)(stats->rx_bytes - stats->previous_rx_bytes);
        stats->summary_printed = true;
        taskEXIT_CRITICAL();

        /* bytes / ms * 8 = kbit/s */
        printf("%s:%s %s, total:%s, average:%luKbps\r\n", tp_rx_reports[report_index].role_name, addr,
               tp_rx_reports[report_index].direction_name, tp_format_total_bytes(total_bytes, total_text),
               (unsigned long)(total_bytes * 8 / elapsed_ms));
        has_output = true;
    }
    if (has_output) {
        printf("\r\n");
    }
}

int ble_test_init(uint8_t master_count, uint8_t slave_count, uint8_t mode)
{
    if (tp_report_task_handle) {
        printf("BLE test is already running\r\n");
        return -EALREADY;
    }

#if !defined(CONFIG_BT_CENTRAL)
    if (master_count) {
        printf("Master throughput test requires Central and Observer support\r\n");
        return -ENOTSUP;
    }
#endif

    tp_master_count = master_count;
    tp_slave_count = slave_count ? 1 : 0;
    tp_test_mode = mode < TP_TEST_MODE_COUNT ? mode : TP_TEST_NOTIFY;

    if (CONFIG_BT_MAX_CONN - tp_slave_count < master_count) {
        printf("Not support %d master\r\n", master_count);
        return -1;
    }

    /* Create the reporter before starting links so failure leaves no active test. */
    if (xTaskCreate(tp_report_task, (char *)"ble_show", 512, NULL, 15, &tp_report_task_handle) != pdPASS) {
        tp_report_task_handle = NULL;
        printf("Create throughput reporter failed\r\n");
        return -ENOMEM;
    }

    bt_conn_cb_register(&tp_conn_callbacks);
    bt_gatt_service_register(&tp_service);
    bt_gatt_register_mtu_callback(&tp_mtu_changed_cb);

    if (tp_slave_count) {
        tp_start_advertising();
    }
#if defined(CONFIG_BT_OBSERVER)
    if (tp_master_count) {
        tp_start_scanning();
    }
#endif

    return 0;
}

static int tp_stop_test(void)
{
    struct bt_conn *conn;

    /* Stop reporter first so it no longer touches sessions. */
    if (tp_report_task_handle) {
        vTaskDelete(tp_report_task_handle);
        tp_report_task_handle = NULL;
    }

    /* Let TX tasks exit and release their connection refs. */
    for (int conn_index = 0; conn_index < CONFIG_BT_MAX_CONN; ++conn_index) {
        tp_sessions[conn_index].tx_running = false;
    }
    tp_slave_session = NULL;
    vTaskDelay(pdMS_TO_TICKS(20));

    for (int conn_index = 0; conn_index < CONFIG_BT_MAX_CONN; ++conn_index) {
        conn = bt_conn_lookup_id(conn_index);
        if (conn) {
            char addr[BT_ADDR_LE_STR_LEN];

            bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
            tp_print_conn_summary(conn_index, addr);
            bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
            bt_conn_unref(conn);
        }
    }

    for (int conn_index = 0; conn_index < CONFIG_BT_MAX_CONN; ++conn_index) {
        tp_reset_session(conn_index);
    }

    bt_le_adv_stop();
#if defined(CONFIG_BT_OBSERVER)
    bt_le_scan_stop();
#endif
    bt_gatt_service_unregister(&tp_service);
    return 0;
}

/*
 * Slave:          ble_tp_test 0 1
 * Master notify:  ble_tp_test 1 0
 * Master write:   ble_tp_test 1 0 1
 * Master both:    ble_tp_test 1 0 2
 * Stop:           ble_tp_test stop
 */
#if defined(CONFIG_SHELL)
static void cmd_ble_tp_test(int argc, char **argv)
#else
static void cmd_ble_tp_test(char *write_buffer, int write_buffer_len, int argc, char **argv)
#endif
{
    uint8_t master_count = 0;
    uint8_t slave_count = 0;
    uint8_t mode = 0;

    if (argc == 2 && strcmp(argv[1], "stop") == 0) {
        tp_stop_test();
        printf("BLE test stopped\n");
        return;
    }

    if (argc > 4) {
        printf("Parameter error\r\n");
        return;
    }
    if (argc >= 2) {
        master_count = atoi(argv[1]);
    }
    if (argc >= 3) {
        slave_count = atoi(argv[2]);
    }
    if (argc >= 4) {
        mode = atoi(argv[3]);
    }

    printf("mst:%d slv:%d type:%d\r\n", master_count, slave_count, mode);
    if (ble_test_init(master_count, slave_count, mode) != 0) {
        printf("Start ble test failed\r\n");
    }
}

#if defined(CONFIG_SHELL)
SHELL_CMD_EXPORT_ALIAS(cmd_ble_tp_test, ble_tp_test, );
#else
const struct cli_command TpCmdSet[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "ble_tp_test", "\r\n ble_tp_test:\r\n", cmd_ble_tp_test },
};
#endif

int ble_tp_cli_register(void)
{
    return 0;
}
