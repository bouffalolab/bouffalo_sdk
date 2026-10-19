#include <stdlib.h>
#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
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
#include <bt_errno.h>

typedef struct {
    uint32_t random;
    uint32_t crc;
#if defined(BLE_TEST_SHOW_RSSI)
    int8_t rssi;
#endif /* BLE_TEST_SHOW_RSSI */
} tp_data_t;

typedef enum {
    TP_TEST_NTF,
    TP_TEST_WRT,
    TP_TEST_NTF_WRT,
    TP_TEST_MAX,
} tp_test_t;

#define NAME_LEN 30
#define BLE_NAME __TIME__
#define BT_UUID_SVC_BLE_TEST BT_UUID_DECLARE_16(0x28a5)
#define BT_UUID_CHAR_BLE_TEST_NTF BT_UUID_DECLARE_16(0x28a9)
#define BT_CHAR_BLE_TEST_NTF_ATTR_VAL_INDEX (3)
#define MAX_RECORD_SIZE 16 // This value shall be (2^n)
#define TP_DATA_BUF_SIZE 244

/* Per-direction throughput statistics. One entry per (conn, direction). */
typedef struct {
    uint32_t cnt;                  /* live counter, bumped on each tx/rx */
    uint32_t pre;                  /* previous reading, for delta/rate */
    uint32_t idx;                  /* ring write index */
    uint32_t rate[MAX_RECORD_SIZE];
} tp_stat_t;

typedef enum {
    TP_NTF_TX,                     /* slave: notify tx */
    TP_NTF_RX,                     /* master: notify rx */
    TP_WRT_TX,                     /* master: write tx */
    TP_WRT_RX,                     /* slave: write rx */
    TP_DIR_MAX,
} tp_dir_t;

/* All per-connection state, indexed by bt_conn_index(). */
typedef struct {
    struct bt_conn *conn;
    uint8_t  role;                 /* BT_HCI_ROLE_xxx */
    volatile bool running;         /* cooperative shutdown flag for tx_task */
    TaskHandle_t tx_task;          /* slave: notify, master: write (unified) */
    uint16_t wr_handle;            /* master write handle */
    struct bt_gatt_subscribe_params sub;
    struct bt_gatt_discover_params  disc;
    struct bt_uuid_16 uuid;
    tp_stat_t stats[TP_DIR_MAX];
} tp_session_t;

static uint8_t master_cnt = 0;
static uint8_t slave_cnt = 0;
static tp_test_t test_type = TP_TEST_NTF;
static tp_session_t ses[CONFIG_BT_MAX_CONN];
static tp_session_t *slv_ses;      /* slave is single-conn; CCC cb carries no conn */
static struct bt_gatt_exchange_params exchg_mtu;
static TaskHandle_t tp_show_task_hdl;

static void ble_tp_connected(struct bt_conn *conn, u8_t err);
static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason);
static void ble_param_updated(struct bt_conn *conn, u16_t interval, u16_t latency,
                             u16_t timeout);
#if defined(CONFIG_BT_OBSERVER)
static int tp_start_scan(void);
#endif
static int ble_tp_recv_wr(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          const void *buf, u16_t len, u16_t offset, u8_t flags);
static void ble_tp_notify_ccc_changed(const struct bt_gatt_attr *attr, u16_t value);
static int tp_start_adv(void);
static void tp_tx_task(void *pvParameters);

static struct bt_gatt_attr attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_SVC_BLE_TEST),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_TEST_NTF,
                           BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL,
                           ble_tp_recv_wr,
                           NULL),
    BT_GATT_CCC(ble_tp_notify_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
};

static struct bt_gatt_service ble_tp_server = BT_GATT_SERVICE(attrs);

static struct bt_conn_cb ble_tp_conn_callbacks = {
    .connected = ble_tp_connected,
    .disconnected = ble_tp_disconnected,
    .le_param_updated = ble_param_updated,
};

/* crc is simply the byte-reversed random value (wire format unchanged). */
static void tp_set_crc(tp_data_t *val)
{
    val->crc = __builtin_bswap32(val->random);
}

static uint8_t tp_check_crc(tp_data_t *val)
{
    return val->crc == __builtin_bswap32(val->random) ? 1 : 0;
}

static int tp_index_get(int i)
{
    return i & (MAX_RECORD_SIZE - 1);
}

static void tp_clear_session(uint8_t idx)
{
    memset(&ses[idx], 0, sizeof(ses[idx]));
}

#if defined(CONFIG_BT_OBSERVER)
static int tp_get_mst_cnt(void)
{
    int cnt = 0;
    struct bt_conn *conn;

    for (int i = 0; i < CONFIG_BT_MAX_CONN; ++i) {
        conn = bt_conn_lookup_id(i);
        if (!conn) {
            continue;
        }
        if (conn->role == BT_HCI_ROLE_MASTER) {
            cnt++;
        }
        bt_conn_unref(conn);
    }
    return cnt;
}
#endif

static struct bt_gatt_attr *tp_get_attr(u8_t index)
{
    return &attrs[index];
}

static u8_t tp_notify_func(struct bt_conn *conn,
                        struct bt_gatt_subscribe_params *params,
                        const void *data, u16_t length)
{
    u8_t idx;
    tp_data_t tp_data;
    uint8_t *p_tp_data = (uint8_t *)&tp_data;

    if (!params->value) {
        printf("Unsubscribed\r\n");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }

    if (data == NULL || length < sizeof(tp_data)) {
        printf("tp_notify_func no data\r\n");
        return BT_GATT_ITER_CONTINUE;
    }

    memcpy(p_tp_data, data, sizeof(tp_data));

    if (!tp_check_crc(&tp_data)) {
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
    printf("recv rssi:%d send rssi:%d\r\n", rssi, tp_data.rssi);
#endif /* BLE_TEST_SHOW_RSSI */

    idx = bt_conn_index(conn);
    ses[idx].stats[TP_NTF_RX].cnt += length;

    return BT_GATT_ITER_CONTINUE;
}

static u8_t tp_discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          struct bt_gatt_discover_params *params)
{
    char str[37];
    char addr[BT_ADDR_LE_STR_LEN];
    u8_t idx;

    idx = bt_conn_index(conn);
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!attr) {
        printf("%s Discover complete\r\n", addr);
        (void)memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }

    switch (params->type) {
    case BT_GATT_DISCOVER_CHARACTERISTIC: {
        struct bt_gatt_chrc *gatt_chrc = attr->user_data;
        if (test_type == TP_TEST_NTF || test_type == TP_TEST_NTF_WRT) {
            bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));
            ses[idx].sub.ccc_handle = attr->handle + 2;
            ses[idx].sub.value_handle = attr->handle + 2;
            ses[idx].sub.value = BT_GATT_CCC_NOTIFY;
            ses[idx].sub.notify = tp_notify_func;

            int err = bt_gatt_subscribe(conn, &ses[idx].sub);
            if (err) {
                printf("Subscribe failed (err %d)\r\n", err);
            }
        }
        if ((test_type == TP_TEST_WRT || test_type == TP_TEST_NTF_WRT) && !ses[idx].tx_task) {
            // start this connection's tx task (master: write)
            ses[idx].wr_handle = attr->handle + 1;
            ses[idx].running = true;
            bt_conn_ref(conn);   /* released by tx_task when it exits */
            if (xTaskCreate(tp_tx_task, (char *)"ble_tx", 512, &ses[idx], 15,
                            &ses[idx].tx_task) != pdPASS) {
                printf("creater write task failed\r\n");
                ses[idx].running = false;
                ses[idx].tx_task = NULL;
                bt_conn_unref(conn);
            }
        }
    } break;
    default:
        bt_uuid_to_str(attr->uuid, str, sizeof(str));
        printf("%s Discover type 0x%x\r\n", addr, params->type);
        break;
    }

    return BT_GATT_ITER_CONTINUE;
}

static void tp_exchg_mtu_cb(struct bt_conn *conn, u8_t err,
                         struct bt_gatt_exchange_params *params)
{
    char addr[BT_ADDR_LE_STR_LEN];
    u8_t idx;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        printf("%s echange mtu size failure, err: %d\r\n", addr, err);
        return;
    }

    printf("%s echange mtu size success, mtu size: %d\r\n", addr,
           bt_gatt_get_mtu(conn));
    idx = bt_conn_index(conn);

    ses[idx].uuid = *(struct bt_uuid_16 *)BT_UUID_CHAR_BLE_TEST_NTF;
    ses[idx].disc.func = tp_discover_func;
    ses[idx].disc.start_handle = 0x0001;
    ses[idx].disc.end_handle = 0xffff;
    ses[idx].disc.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    ses[idx].disc.uuid = &ses[idx].uuid.uuid;

    err = bt_gatt_discover(conn, &ses[idx].disc);
    if (err) {
        printf("%s Discover failed (err %d)\r\n", addr, err);
    }
}

static void ble_tp_connected(struct bt_conn *conn, u8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    int ret;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        printf("Connected err:0x%x %s role:%d\r\n", err, addr, conn->role);
        if (conn->role == BT_HCI_ROLE_SLAVE) {
            tp_start_adv();
        }
        return;
    }

    printf("Connected:%s role:%d\r\n", addr, conn->role);

    u8_t idx = bt_conn_index(conn);
    tp_clear_session(idx);
    ses[idx].conn = conn;
    ses[idx].role = conn->role;
    if (conn->role == BT_HCI_ROLE_SLAVE) {
        if (slave_cnt) {
            slv_ses = &ses[idx];
        }
        return;
    }
    if (!master_cnt) {
        return;
    }
#if defined(CONFIG_BT_OBSERVER)
    if (tp_get_mst_cnt() < master_cnt) {
        tp_start_scan();
    }
#endif

    exchg_mtu.func = tp_exchg_mtu_cb;
    ret = bt_gatt_exchange_mtu(conn, &exchg_mtu);
    if (ret) {
        printf("%s exchange mtu size failure, err: %d\r\n", addr, ret);
    }
}

static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    u8_t idx = bt_conn_index(conn);

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printf("Disconnected:%s role:%d reason:0x%x\r\n", addr, conn->role, reason);

    /* Ask the tx task to stop; it exits its loop and self-deletes, releasing
     * the conn ref it holds. We must not free the session here (the task is
     * still using ses[idx]); the slot is reclaimed by tp_clear_session() on the
     * next connect. Clearing stats stops tp_show_task from reporting this conn. */
    ses[idx].running = false;
    if (slv_ses == &ses[idx]) {
        slv_ses = NULL;
    }
    memset(ses[idx].stats, 0, sizeof(ses[idx].stats));
}

static void ble_param_updated(struct bt_conn *conn, u16_t interval, u16_t latency,
                              u16_t timeout)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printf("%s conn param updated: int %d lat %d to %d\r\n", addr, interval, latency,
           timeout);
}

static int ble_tp_recv_wr(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          const void *buf, u16_t len, u16_t offset, u8_t flags)
{
    u8_t idx;
    tp_data_t tp_data;
    uint8_t *p_tp_data = (uint8_t *)&tp_data;

    if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
        return 0;
    }

    if (buf == NULL || len < sizeof(tp_data)) {
        printf("write rx no data\r\n");
        return len;
    }

    memcpy(p_tp_data, buf, sizeof(tp_data));

    if (!tp_check_crc(&tp_data)) {
        printf("Recv write rx data, but crc is error\r\n");
        return len;
    }

    idx = bt_conn_index(conn);
    ses[idx].stats[TP_WRT_RX].cnt += len;
    return len;
}

/* Build a test payload into `data` (capacity `cap`) and return the send length
 * clamped to the buffer. The first sizeof(tp_data_t) bytes carry random+crc. */
static u16_t tp_build_payload(char *data, size_t cap, struct bt_conn *conn)
{
    tp_data_t tp_data;
    u16_t slen = bt_gatt_get_mtu(conn) - 3;

    if (slen > cap) {
        slen = cap;
    }

    tp_data.random = ((u32_t)random());
    tp_set_crc(&tp_data);
#if defined(BLE_TEST_SHOW_RSSI)
    if (bt_le_read_rssi(conn->handle, &tp_data.rssi)) {
        tp_data.rssi = 0;
    }
#endif /* BLE_TEST_SHOW_RSSI */
    memcpy(data, &tp_data, sizeof(tp_data));
    return slen;
}

/* Unified tx loop: slave notifies, master writes. Runs until s->running is
 * cleared (disconnect/stop), then releases its conn ref and self-deletes.
 * The conn ref taken by the starter keeps the conn object valid for the whole
 * task lifetime, so a send after disconnect returns an error instead of
 * dereferencing freed memory. */
static void tp_tx_task(void *pvParameters)
{
    int err;
    u16_t slen;
    char data[TP_DATA_BUF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    tp_session_t *s = (tp_session_t *)pvParameters;
    struct bt_conn *conn = s->conn;   /* reffed by starter; keep a local copy */
    bool is_slave = (s->role == BT_HCI_ROLE_SLAVE);
    tp_dir_t sid = is_slave ? TP_NTF_TX : TP_WRT_TX;

    while (s->running) {
        slen = tp_build_payload(data, sizeof(data), conn);
        if (is_slave) {
            err = bt_gatt_notify(conn, tp_get_attr(BT_CHAR_BLE_TEST_NTF_ATTR_VAL_INDEX),
                                 data, slen);
        } else {
            err = bt_gatt_write_without_response(conn, s->wr_handle, data, slen, 0);
        }
        if (err) {
            if (err != -ENOMEM) {
                printf("ble_tx error:%d\r\n", err);
            }
            /* no tx buffer (or transient error): yield and retry */
            vTaskDelay(1);
            continue;
        }

        s->stats[sid].cnt += slen;
    }

    bt_conn_unref(conn);
    s->tx_task = NULL;
    vTaskDelete(NULL);
}

static void ble_tp_notify_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    printf("ccc:value=[%d]\r\n", value);
    if (!slave_cnt || !slv_ses) {
        return;
    }

    if (value == BT_GATT_CCC_NOTIFY) {
        if (slv_ses->tx_task) {            /* already notifying */
            return;
        }
        slv_ses->running = true;
        bt_conn_ref(slv_ses->conn);        /* released by tx_task when it exits */
        if (xTaskCreate(tp_tx_task, (char *)"ble_ntf", 512, slv_ses, 15,
                        &slv_ses->tx_task) == pdPASS) {
            printf("Create notify task success\r\n");
        } else {
            slv_ses->running = false;
            slv_ses->tx_task = NULL;
            bt_conn_unref(slv_ses->conn);
            printf("Create notify task fail\r\n");
        }
    } else {
        /* tx_task exits its loop and self-deletes */
        slv_ses->running = false;
    }
}

static int tp_start_adv(void)
{
    struct bt_le_adv_param adv_param = {
        .options = BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
    };
    char *adv_name = BLE_NAME;
    struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
    };
    printf("start ble adv, name:[%s]\r\n", BLE_NAME);
    return bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
}

#if defined(CONFIG_BT_OBSERVER)
static bool tp_data_cb(struct bt_data *data, void *user_data)
{
    char *name = user_data;
    u8_t len;

    switch (data->type) {
    case BT_DATA_NAME_SHORTENED:
    case BT_DATA_NAME_COMPLETE:
        len = (data->data_len > NAME_LEN - 1) ? (NAME_LEN - 1) : (data->data_len);
        memcpy(name, data->data, len);
        return false;
    default:
        return true;
    }
}

static void tp_device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t evtype,
                         struct net_buf_simple *buf)
{
    char le_addr[BT_ADDR_LE_STR_LEN];
    char name[NAME_LEN];
    int err;

    (void)memset(name, 0, sizeof(name));
    bt_data_parse(buf, tp_data_cb, name);
    bt_addr_le_to_str(addr, le_addr, sizeof(le_addr));
    if (0 != strcmp(name, BLE_NAME)) {
        return;
    }
    printf("[DEVICE]: %s Found. RSSI %i %s \r\n", le_addr, rssi, name);

    err = bt_le_scan_stop();
    if (err) {
        printf("Stopping scanning failed (err %d)\r\n", err);
    }

#if defined(CONFIG_BT_CENTRAL)
    struct bt_conn *conn;
    struct bt_le_conn_param param = {
        .interval_min = BT_GAP_INIT_CONN_INT_MAX << 1,
        .interval_max = BT_GAP_INIT_CONN_INT_MAX << 1,
        .latency = 0,
        .timeout = 500,
    };

    conn = bt_conn_create_le(addr, &param);

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

static int tp_start_scan(void)
{
    struct bt_le_scan_param scan_param;
    int err;

    scan_param.type = BT_HCI_LE_SCAN_ACTIVE;
    scan_param.filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE;
    scan_param.interval = BT_GAP_SCAN_FAST_INTERVAL;
    scan_param.window = BT_GAP_SCAN_FAST_WINDOW;

    err = bt_le_scan_start(&scan_param, tp_device_found);
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

static char *tp_get_conn_dst_addr(int idx)
{
    struct bt_conn *conn;
    static char addr[BT_ADDR_LE_STR_LEN];

    conn = bt_conn_lookup_id(idx);
    if (!conn) {
        return NULL;
    }
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    bt_conn_unref(conn);
    return addr;
}

static uint32_t tp_get_average_rate(uint32_t *val, uint32_t idx)
{
    uint32_t temp = 0;
    int i;

    if (idx <= 1) {
        return 0;
    }

    i = idx > MAX_RECORD_SIZE ? 0 : 1;

    for (; i < MAX_RECORD_SIZE; ++i) {
        temp += val[i];
    }

    return temp / (idx > MAX_RECORD_SIZE ? MAX_RECORD_SIZE : idx - 1);
}

/* Descriptor for one printed throughput line, in display order. */
static const struct {
    tp_dir_t id;
    const char *role;
    const char *dir;
} show_tbl[] = {
    { TP_NTF_TX, "Slave",  "notify tx" },
    { TP_WRT_RX, "Slave",  "write  rx" },
    { TP_NTF_RX, "Master", "notify rx" },
    { TP_WRT_TX, "Master", "write  tx" },
};

/* Self-timed reporter: wakes every ~1s and computes rate from the *actual*
 * elapsed time, so preemption jitter does not distort the reading. Replaces
 * the old software-timer + queue + snapshot handoff. */
static void tp_show_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t prev = xTaskGetTickCount();

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        TickType_t now = xTaskGetTickCount();
        uint32_t ms = (uint32_t)(now - prev) * portTICK_PERIOD_MS;
        prev = now;
        if (ms == 0) {
            continue;
        }

        bool any = false;
        for (unsigned t = 0; t < ARRAY_SIZE(show_tbl); ++t) {
            tp_dir_t id = show_tbl[t].id;
            for (int i = 0; i < CONFIG_BT_MAX_CONN; ++i) {
                tp_stat_t *st = &ses[i].stats[id];
                const char *addr;
                uint32_t cur, rate;

                if (st->cnt == 0) {
                    continue;
                }
                addr = tp_get_conn_dst_addr(i);
                if (!addr) {                       /* conn gone mid-report */
                    continue;
                }
                cur = st->cnt;                     /* 32-bit aligned: atomic read */
                rate = (uint32_t)((uint64_t)(cur - st->pre) * 1000 / ms);  /* bytes/s */
                st->pre = cur;
                st->rate[tp_index_get(st->idx++)] = rate;
                printf("%s:%s %s, total:%10lu, rate:%luKB/s, average:%luKB/s\r\n",
                       show_tbl[t].role, addr, show_tbl[t].dir,
                       cur, rate / 1024, tp_get_average_rate(st->rate, st->idx) / 1024);
                any = true;
            }
        }
        if (any) {
            printf("\r\n");
        }
    }
}

int ble_test_init(uint8_t mst, uint8_t slv, uint8_t type)
{
#if !defined(CONFIG_BT_CENTRAL)
    if (mst) {
        printf("Master throughput test requires Central and Observer support\r\n");
        return -ENOTSUP;
    }
#endif

    master_cnt = mst;
    slave_cnt = slv ? 1 : 0;
    test_type = type < TP_TEST_MAX ? type : TP_TEST_NTF;

    if (CONFIG_BT_MAX_CONN - slave_cnt < mst) {
        printf("Not support %d master\r\n", mst);
        return -1;
    }

    bt_conn_cb_register(&ble_tp_conn_callbacks);
    bt_gatt_service_register(&ble_tp_server);
    bt_gatt_register_mtu_callback(&tp_mtu_changed_cb);

    if (slave_cnt) {
        tp_start_adv();
    }
#if defined(CONFIG_BT_OBSERVER)
    if (master_cnt) {
        tp_start_scan();
    }
#endif

    xTaskCreate(tp_show_task, (char *)"ble_show", 512, NULL, 15, &tp_show_task_hdl);
    return 0;
}

static int ble_test_stop(void)
{
    struct bt_conn *conn;

    /* Stop reporter first so it no longer touches sessions. */
    if (tp_show_task_hdl) {
        vTaskDelete(tp_show_task_hdl);
        tp_show_task_hdl = NULL;
    }

    /* Ask all tx tasks to exit cooperatively, then give them time to leave
     * their loop (they sit in vTaskDelay(1) or a stack call) and self-delete,
     * releasing their conn refs. */
    for (int i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        ses[i].running = false;
    }
    slv_ses = NULL;
    vTaskDelay(pdMS_TO_TICKS(20));

    for (int i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        conn = bt_conn_lookup_id(i);
        if (conn) {
            bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
            bt_conn_unref(conn);
        }
    }

    for (int i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        tp_clear_session(i);
    }

    bt_le_adv_stop();
#if defined(CONFIG_BT_OBSERVER)
    bt_le_scan_stop();
#endif
    bt_gatt_service_unregister(&ble_tp_server);
    return 0;
}
/***************
Test RX:

Start cmd:

slave : ble_tp_test 0 1 \r\n
master : ble_tp_test 1 0 \r\n


Stop cmd:
ble_tp_test stop \r\n


Test  TX:

Start cmd:

slave : ble_tp_test 0 1 \r\n
master : ble_tp_test 1 0 1 \r\n

Stop cmd:
ble_tp_test stop \r\n

Test RX and TX:

Start cmd:

slave : ble_tp_test 0 1 \r\n
master : ble_tp_test 1 0 2 \r\n

Stop cmd:
ble_tp_test stop \r\n


******************/
#if defined(CONFIG_SHELL)
static void cmd_ble_test(int argc, char **argv)
#else
static void cmd_ble_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#endif
{
    if (argc == 2 && strcmp(argv[1], "stop") == 0) {
        ble_test_stop();
        printf("BLE test stopped\n");
        return;
    }

    uint8_t mst = 0, slv = 0, type = 0;
    if (argc > 4) {
        printf("Parameter error\r\n");
        return;
    }
    if (argc >= 2) {
        mst = atoi(argv[1]);
    }
    if (argc >= 3) {
        slv = atoi(argv[2]);
    }
    if (argc >= 4) {
        type = atoi(argv[3]);
    }

    printf("mst:%d slv:%d type:%d\r\n", mst, slv, type);
    if (0 != ble_test_init(mst, slv, type)) {
        printf("Start ble test failed\r\n");
    }
}
#if defined(CONFIG_SHELL)
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test, ble_tp_test, );
#else
const struct cli_command TpCmdSet[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"ble_tp_test", "\r\n ble_tp_test:\r\n", cmd_ble_test},
};
#endif
int ble_tp_cli_register(void)
{
    return 0;
}
