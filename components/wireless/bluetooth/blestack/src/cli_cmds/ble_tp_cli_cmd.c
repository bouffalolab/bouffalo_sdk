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

typedef struct {
    uint32_t random;
    uint32_t crc;
#if defined(BLE_TEST_SHOW_RSSI)
    int8_t rssi;
#endif /* BLE_TEST_SHOW_RSSI */
} tp_data_t;

typedef struct {
    struct bt_conn *conn;
    uint16_t handle;
    TaskHandle_t thandle;
} tp_wrt_t;

typedef enum {
    TEST_NTF,
    TEST_WRT,
    TEST_NTF_WRT,
    TEST_MAX,
} test_type_t;

#define NAME_LEN 30
#define BLE_NAME __TIME__
#define BT_UUID_SVC_BLE_TEST BT_UUID_DECLARE_16(0x28a5)
#define BT_UUID_CHAR_BLE_TEST_NTF BT_UUID_DECLARE_16(0x28a9)
#define BT_CHAR_BLE_TEST_NTF_ATTR_VAL_INDEX (3)
#define MAX_RECORD_SIZE 16 // This value shall be (2^n)

static uint8_t master_cnt = 0;
static uint8_t slave_cnt = 0;
static test_type_t test_type = TEST_NTF;
static struct bt_gatt_subscribe_params subscribe_params[CONFIG_BT_MAX_CONN];
static struct bt_gatt_discover_params discover_params[CONFIG_BT_MAX_CONN];
static struct bt_uuid_16 uuid[CONFIG_BT_MAX_CONN];
static uint32_t ntf_tx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t ntf_rx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t wrt_tx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t wrt_rx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t pre_ntf_tx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t pre_ntf_rx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t pre_wrt_tx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t pre_wrt_rx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t tmp_ntf_tx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t tmp_ntf_rx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t tmp_wrt_tx_cnt[CONFIG_BT_MAX_CONN];
static uint32_t tmp_wrt_rx_cnt[CONFIG_BT_MAX_CONN];
static tp_wrt_t tp_wrt[CONFIG_BT_MAX_CONN];
static struct bt_conn *ble_slv_conn;
static struct bt_gatt_exchange_params exchg_mtu;
static TaskHandle_t ntf_task_hdl;
static TaskHandle_t write_task_hdl;
static u8_t created_ntf_task = 0;
static u8_t created_wr_task = 0;

static void ble_tp_connected(struct bt_conn *conn, u8_t err);
static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason);
static void ble_param_updated(struct bt_conn *conn, u16_t interval, u16_t latency,
                             u16_t timeout);
static int ble_start_scan(void);
static int ble_tp_recv_wr(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          const void *buf, u16_t len, u16_t offset, u8_t flags);
static void ble_tp_notify_ccc_changed(const struct bt_gatt_attr *attr, u16_t value);
static int ble_start_adv(void);
static void ble_tp_timer_start(void);
static void write_task(void *pvParameters);
static TimerHandle_t BLE_xTimer;

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

static void set_crc(tp_data_t *val)
{
    val->crc = 0;
    val->crc += (val->random & 0xff) << 24;
    val->crc += ((val->random >> 8) & 0xff) << 16;
    val->crc += ((val->random >> 16) & 0xff) << 8;
    val->crc += ((val->random >> 24) & 0xff);
}

static uint8_t check_crc(tp_data_t *val)
{
    uint32_t crc = 0;

    crc += (val->random & 0xff) << 24;
    crc += ((val->random >> 8) & 0xff) << 16;
    crc += ((val->random >> 16) & 0xff) << 8;
    crc += ((val->random >> 24) & 0xff);

    return crc == val->crc ? 1 : 0;
}

static int index_get(int i)
{
    return i & (MAX_RECORD_SIZE - 1);
}

static int clear_statistical_data(uint8_t idx)
{
    ntf_tx_cnt[idx] = 0;
    ntf_rx_cnt[idx] = 0;
    wrt_tx_cnt[idx] = 0;
    wrt_rx_cnt[idx] = 0;
    tmp_ntf_tx_cnt[idx] = 0;
    tmp_ntf_rx_cnt[idx] = 0;
    tmp_wrt_tx_cnt[idx] = 0;
    tmp_wrt_rx_cnt[idx] = 0;
    pre_ntf_tx_cnt[idx] = 0;
    pre_ntf_rx_cnt[idx] = 0;
    pre_wrt_tx_cnt[idx] = 0;
    pre_wrt_rx_cnt[idx] = 0;
    memset(&tp_wrt[idx], 0, sizeof(tp_wrt[idx]));
    return 0;
}

static int get_mst_cnt(void)
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

static struct bt_gatt_attr *get_attr(u8_t index)
{
    return &attrs[index];
}

static u8_t notify_func(struct bt_conn *conn,
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

    if (data == NULL || length == 0) {
        printf("notify_func no data\r\n");
        return BT_GATT_ITER_CONTINUE;
    }

    for (int i = 0; i < sizeof(tp_data); ++i) {
        p_tp_data[i] = ((uint8_t *)data)[i];
    }

    if (!check_crc(&tp_data)) {
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
    ntf_rx_cnt[idx] += length;

    return BT_GATT_ITER_CONTINUE;
}

static u8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
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
        if (test_type == TEST_NTF || test_type == TEST_NTF_WRT) {
            bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));
            subscribe_params[idx].ccc_handle = attr->handle + 2;
            subscribe_params[idx].value_handle = attr->handle + 2;
            subscribe_params[idx].value = BT_GATT_CCC_NOTIFY;
            subscribe_params[idx].notify = notify_func;

            int err = bt_gatt_subscribe(conn, &subscribe_params[idx]);
            if (err) {
                printf("Subscribe failed (err %d)\r\n", err);
            }
        }
        if (test_type == TEST_WRT || test_type == TEST_NTF_WRT) {
            // create task do write
            tp_wrt[idx].handle = attr->handle + 1;
            tp_wrt[idx].conn = conn;
            if (xTaskCreate(write_task, (char *)"write_task", 512, &tp_wrt[idx], 15,
                            &write_task_hdl) != pdPASS) {
                printf("creater write task failed\r\n");
                memset(&tp_wrt[idx], 0, sizeof(tp_wrt[idx]));
            }
            created_wr_task = 1;
        }
    } break;
    default:
        bt_uuid_to_str(attr->uuid, str, sizeof(str));
        printf("%s Discover type 0x%x\r\n", addr, params->type);
        break;
    }

    return BT_GATT_ITER_CONTINUE;
}

static void exchg_mtu_cb(struct bt_conn *conn, u8_t err,
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

    uuid[idx] = *(struct bt_uuid_16 *)BT_UUID_CHAR_BLE_TEST_NTF;
    discover_params[idx].func = discover_func;
    discover_params[idx].start_handle = 0x0001;
    discover_params[idx].end_handle = 0xffff;
    discover_params[idx].type = BT_GATT_DISCOVER_CHARACTERISTIC;
    discover_params[idx].uuid = &uuid[idx].uuid;

    err = bt_gatt_discover(conn, &discover_params[idx]);
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
            ble_start_adv();
        }
        return;
    }

    printf("Connected:%s role:%d\r\n", addr, conn->role);

    clear_statistical_data(bt_conn_index(conn));
    if (conn->role == BT_HCI_ROLE_SLAVE) {
        if (slave_cnt) {
            ble_slv_conn = conn;
        }
        return;
    }
    if (!master_cnt) {
        return;
    }
    if (get_mst_cnt() < master_cnt) {
        ble_start_scan();
    }

    exchg_mtu.func = exchg_mtu_cb;
    ret = bt_gatt_exchange_mtu(conn, &exchg_mtu);
    if (ret) {
        printf("%s exchange mtu size failure, err: %d\r\n", addr, ret);
    }
}

static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printf("Disconnected:%s role:%d reason:0x%x\r\n", addr, conn->role, reason);
    clear_statistical_data(bt_conn_index(conn));

    if (conn->role == BT_HCI_ROLE_SLAVE) {
        if (slave_cnt) {
            ble_slv_conn = NULL;
            if (created_ntf_task) {
                printf("Delete notify task\r\n");
                vTaskDelete(ntf_task_hdl);
                created_ntf_task = 0;
            }
        }
        return;
    }
    else
    {
        if(created_wr_task)
        {
            printf("Delete wr task\r\n");
            vTaskDelete(write_task_hdl);
            created_wr_task = 0;
        }
        
    }
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

    if (flags & BT_GATT_WRITE_FLAG_CMD) {
    } else {
    }

    if (buf == NULL || len == 0) {
        printf("write rx no data\r\n");
        return len;
    }

    for (int i = 0; i < sizeof(tp_data); ++i) {
        p_tp_data[i] = ((uint8_t *)buf)[i];
    }

    if (!check_crc(&tp_data)) {
        printf("Recv write rx data, but crc is error\r\n");
        return BT_GATT_ITER_CONTINUE;
    }

    idx = bt_conn_index(conn);
    wrt_rx_cnt[idx] += len;
    return len;
}

static void notify_task(void *pvParameters)
{
    int err = -1;
    u16_t slen;
    u8_t idx;
    char data[244] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    tp_data_t tp_data;
    uint8_t *p_tp_data = (uint8_t *)&tp_data;
    struct bt_conn *conn = (struct bt_conn *)pvParameters;

    while (1) {
        slen = bt_gatt_get_mtu(conn) - 3;
        tp_data.random = ((u32_t)random());
        set_crc(&tp_data);
        for (int i = 0; i < sizeof(tp_data); ++i) {
            data[i] = p_tp_data[i];
        }
#if defined(BLE_TEST_SHOW_RSSI)
        err = bt_le_read_rssi(conn->handle, &tp_data.rssi);
        if (err) {
            printf("read rssi failed (err %d)\r\n", err);
            tp_data.rssi = 0;
        }
#endif /* BLE_TEST_SHOW_RSSI */
        err = bt_gatt_notify(conn, get_attr(BT_CHAR_BLE_TEST_NTF_ATTR_VAL_INDEX),
                    data, slen);
        if(err != 0){
            printf("bt_gatt_notify error:%d\r\n", err);
            continue;
        }

        idx = bt_conn_index(conn);
        ntf_tx_cnt[idx] += slen;
    }
}

static void ble_tp_notify_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    printf("ccc:value=[%d]\r\n", value);
    if(!slave_cnt){
        return;
    }

    if (value == BT_GATT_CCC_NOTIFY) {
        if (xTaskCreate(notify_task, (char *)"ble_ntf", 512, ble_slv_conn,  15, &ntf_task_hdl) == pdPASS) {
            created_ntf_task = 1;
            printf("Create notify task success\r\n");
        } else {
            created_ntf_task = 0;
            printf("Create notify taskfail\r\n");
        }
    } else {
        if (created_ntf_task) {
            printf("Delete notify task\r\n");
            vTaskDelete(ntf_task_hdl);
            created_ntf_task = 0;
        }
    }
}

static void write_task(void *pvParameters)
{
    int err = -1;
    u16_t slen;
    u8_t idx;
    char data[244] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    tp_data_t tp_data;
    uint8_t* p_tp_data = (uint8_t*)&tp_data;
    tp_wrt_t *tp_wrt = (tp_wrt_t *)pvParameters;
    
    while(1){
        slen = bt_gatt_get_mtu(tp_wrt->conn) - 3;
        tp_data.random = ((u32_t)random());
        set_crc(&tp_data);
        for(int i = 0; i < sizeof(tp_data); ++i){
            data[i] = p_tp_data[i];
        }
        err = bt_gatt_write_without_response(tp_wrt->conn, tp_wrt->handle, data, slen, 0);
        if(err != 0){
            printf("bt_gatt_write_without_response error:%d\r\n", err);
            continue;
        }

        idx = bt_conn_index(tp_wrt->conn);
        wrt_tx_cnt[idx] += slen;
    }
}


static int ble_start_adv(void)
{
    struct bt_le_adv_param adv_param = {
        .options = BT_LE_ADV_OPT_CONNECTABLE  | BT_LE_ADV_OPT_ONE_TIME,
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

static bool data_cb(struct bt_data *data, void *user_data)
{
	char *name = user_data;
    u8_t len;

	switch (data->type) {
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
        len = (data->data_len > NAME_LEN - 1)?(NAME_LEN - 1):(data->data_len);
		memcpy(name, data->data, len);
		return false;		
	default:
		return true;
	}
}

static void device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t evtype,
			 struct net_buf_simple *buf)
{
	char 		le_addr[BT_ADDR_LE_STR_LEN];
	char 		name[NAME_LEN];
    struct bt_conn *conn;
    int err;

	(void)memset(name, 0, sizeof(name));
	bt_data_parse(buf, data_cb, name);
	bt_addr_le_to_str(addr, le_addr, sizeof(le_addr));
    if(0 != strcmp(name, BLE_NAME)){
        return;
    }
    printf("[DEVICE]: %s Found. RSSI %i %s \r\n",le_addr, rssi, name);

    err = bt_le_scan_stop();
    if (err) {
        printf("Stopping scanning failed (err %d)\r\n", err);
    }

    struct bt_le_conn_param param = {
        .interval_min =  BT_GAP_INIT_CONN_INT_MAX<<1,
        .interval_max =  BT_GAP_INIT_CONN_INT_MAX<<1,
        .latency = 0,
        .timeout = 500,
    };

    conn = bt_conn_create_le(addr, &param);

    if(!conn){
        printf("Connection failed\r\n");
    }else{
        if(conn->state == BT_CONN_CONNECTED)
            printf("Le link with this peer device has existed\r\n");
        else
            printf("Connection pending\r\n");
    }
}

static int ble_start_scan(void)
{
    struct bt_le_scan_param scan_param;
    int err;

    scan_param.type = BT_HCI_LE_SCAN_ACTIVE;
    scan_param.filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE;
    scan_param.interval = BT_GAP_SCAN_FAST_INTERVAL;
    scan_param.window = BT_GAP_SCAN_FAST_WINDOW;

    err = bt_le_scan_start(&scan_param, device_found);
    if(err){
        printf("Failed to start scan (err %d)\r\n", err);
    }

    return 0;
}

static void bt_gatt_mtu_changed_cb(struct bt_conn *conn, int mtu)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printf("mtu changed:%s mtu:%d\r\n", addr, mtu);
}

static char* get_conn_dst_addr(int idx)
{
    struct bt_conn *conn;
    static char addr[BT_ADDR_LE_STR_LEN];

    conn = bt_conn_lookup_id(idx);
    if(!conn){
        return NULL;
    }
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    bt_conn_unref(conn);
    return addr;
}

static uint32_t get_averge_rate(uint32_t* val, uint32_t idx)
{
    uint32_t temp = 0;
    int i;
    if(idx <= 1){
        return 0;
    } 

    i = idx > MAX_RECORD_SIZE ? 0 : 1;

    for(; i < MAX_RECORD_SIZE; ++i){
        temp += val[i];
    }

    return temp/(idx > MAX_RECORD_SIZE ? MAX_RECORD_SIZE: idx - 1);
}

static uint32_t ntf_tx_rate[CONFIG_BT_MAX_CONN][MAX_RECORD_SIZE];
static uint32_t wrt_rx_rate[CONFIG_BT_MAX_CONN][MAX_RECORD_SIZE];
static uint32_t ntf_rx_rate[CONFIG_BT_MAX_CONN][MAX_RECORD_SIZE];
static uint32_t wrt_tx_rate[CONFIG_BT_MAX_CONN][MAX_RECORD_SIZE];
static uint32_t ntf_tx_idx[CONFIG_BT_MAX_CONN];
static uint32_t wrt_rx_idx[CONFIG_BT_MAX_CONN];
static uint32_t ntf_rx_idx[CONFIG_BT_MAX_CONN];
static uint32_t wrt_tx_idx[CONFIG_BT_MAX_CONN];

static void show_task(void *pvParameters)
{
    int i;
    QueueHandle_t hdl = pvParameters;
    void* val;

    uint32_t rate;

    memset(&ntf_tx_rate, 0, sizeof(ntf_tx_rate));
    memset(&wrt_rx_rate, 0, sizeof(wrt_rx_rate));
    memset(&ntf_rx_rate, 0, sizeof(ntf_rx_rate));
    memset(&wrt_tx_rate, 0, sizeof(wrt_tx_rate));
    memset(&ntf_tx_idx, 0, sizeof(ntf_tx_idx));
    memset(&wrt_rx_idx, 0, sizeof(wrt_rx_idx));
    memset(&ntf_rx_idx, 0, sizeof(ntf_rx_idx));
    memset(&wrt_tx_idx, 0, sizeof(wrt_tx_idx));
    printf("hdl %p\r\n", hdl);

    while(1){
        BaseType_t ret = xQueueReceive(hdl, &val, portMAX_DELAY);
        if (ret != pdPASS) {
            continue;
        }

        if(slave_cnt){
            for(i = 0; i < CONFIG_BT_MAX_CONN; ++i){
                if(tmp_ntf_tx_cnt[i] == 0){
                    continue;
                }
                rate = (tmp_ntf_tx_cnt[i] - pre_ntf_tx_cnt[i]);
                ntf_tx_rate[i][index_get(ntf_tx_idx[i]++)] = rate;
                printf("Slave:%s  notify tx, total:%10ld, rate:%3.2lfKb/s, average;%3.2lfKb/s\r\n",
                        get_conn_dst_addr(i), tmp_ntf_tx_cnt[i], rate/128.0, get_averge_rate(ntf_tx_rate[i], ntf_tx_idx[i])/128.0);
                pre_ntf_tx_cnt[i] = tmp_ntf_tx_cnt[i];
            }
            for(i = 0; i < CONFIG_BT_MAX_CONN; ++i){
                if(tmp_wrt_rx_cnt[i] == 0){
                    continue;
                }
                rate = (tmp_wrt_rx_cnt[i] - pre_wrt_rx_cnt[i]);
                wrt_rx_rate[i][index_get(wrt_rx_idx[i]++)] = rate;
                printf("Slave:%s  write  rx, total:%10ld, rate:%3.2lfKb/s, average;%3.2lfKb/s\r\n",
                        get_conn_dst_addr(i), tmp_wrt_rx_cnt[i], rate/128.0, get_averge_rate(wrt_rx_rate[i], wrt_rx_idx[i])/128.0);
                pre_wrt_rx_cnt[i] = tmp_wrt_rx_cnt[i];
            }
        }
        if(master_cnt){
            for(i = 0; i < CONFIG_BT_MAX_CONN; ++i){
                if(tmp_ntf_rx_cnt[i] == 0){
                    continue;
                }
                rate = (tmp_ntf_rx_cnt[i] - pre_ntf_rx_cnt[i]);
                ntf_rx_rate[i][index_get(ntf_rx_idx[i]++)] = rate;
                printf("Master:%s notify rx, total:%10ld, rate:%3.2lfKb/s, average;%3.2lfKb/s\r\n",
                        get_conn_dst_addr(i), tmp_ntf_rx_cnt[i], rate/128.0, get_averge_rate(ntf_rx_rate[i], ntf_rx_idx[i])/128.0);
                pre_ntf_rx_cnt[i] = tmp_ntf_rx_cnt[i];
            }
            for(i = 0; i < CONFIG_BT_MAX_CONN; ++i){
                if(tmp_wrt_tx_cnt[i] == 0){
                    continue;
                }
                rate = (tmp_wrt_tx_cnt[i] - pre_wrt_tx_cnt[i]);
                wrt_tx_rate[i][index_get(wrt_tx_idx[i]++)] = rate;
                printf("Master:%s write  tx, total:%10ld, rate:%3.2lfKb/s, average;%3.2lfKb/s\r\n",
                        get_conn_dst_addr(i), tmp_wrt_tx_cnt[i], rate/128.0, get_averge_rate(wrt_tx_rate[i], wrt_tx_idx[i])/128.0);
                pre_wrt_tx_cnt[i] = tmp_wrt_tx_cnt[i];
            }
        }
        printf("\r\n");
    }
}

static QueueHandle_t queue_hdl;
int ble_test_init(uint8_t mst, uint8_t slv, uint8_t type)
{
    master_cnt = mst;
    slave_cnt = slv ? 1: 0;
    test_type = type < TEST_MAX ? type : TEST_NTF;

    if(CONFIG_BT_MAX_CONN - slave_cnt < mst){
        printf("Not support %d master\r\n", mst);
        return -1;
    }

    bt_conn_cb_register(&ble_tp_conn_callbacks);
    bt_gatt_service_register(&ble_tp_server);
    bt_gatt_register_mtu_callback(&bt_gatt_mtu_changed_cb);

    if(slave_cnt){
        ble_start_adv();
    }
    if(master_cnt){
        ble_start_scan();
    }

    queue_hdl = xQueueCreate(1, sizeof(void*));
    printf("queue_hdl %p\r\n", queue_hdl);

    xTaskCreate(show_task, (char *)"ble_show", 512, queue_hdl, 15, NULL);
    ble_tp_timer_start();
    return 0;
}

static void vTimerCallback(TimerHandle_t xTimer)
{
    int i, flag = 0;

    if(slave_cnt){
        for(i = 0; i < CONFIG_BT_MAX_CONN; ++i){
            if(ntf_tx_cnt[i] == 0){
                continue;
            }
            tmp_ntf_tx_cnt[i] = ntf_tx_cnt[i];
            flag = 1;
        }
        for(i = 0; i < CONFIG_BT_MAX_CONN; ++i){
            if(wrt_rx_cnt[i] == 0){
                continue;
            }
            tmp_wrt_rx_cnt[i] = wrt_rx_cnt[i];
            flag = 1;
        }
    }
    if(master_cnt){
        for(i = 0; i < CONFIG_BT_MAX_CONN; ++i){
            if(ntf_rx_cnt[i] == 0){
                continue;
            }
            tmp_ntf_rx_cnt[i] = ntf_rx_cnt[i];
            flag = 1;
        }
        for(i = 0; i < CONFIG_BT_MAX_CONN; ++i){
            if(wrt_tx_cnt[i] == 0){
                continue;
            }
            tmp_wrt_tx_cnt[i] = wrt_tx_cnt[i];
            flag = 1;
        }
    }
    if(flag){
        flag = 0;
        //triggler
        void* data = NULL;
        
        int ret = xQueueSend(queue_hdl, &data, portMAX_DELAY);
        assert(ret == pdPASS);
    }
}

static void ble_tp_timer_start(void)
{
    
    BLE_xTimer = xTimerCreate(
        "BL TP Timer",          
        pdMS_TO_TICKS(1000),
        pdTRUE,            
        0,                  
        vTimerCallback     
    );

    if (BLE_xTimer == NULL)
    {
        printf("Failed to create timer\n");
    }
    else
    {
        if (xTimerStart(BLE_xTimer, 0) != pdPASS)
        {
            printf("Failed to start timer\n");
        }
    }
}

static int ble_test_stop(void)
{
    if (BLE_xTimer != NULL) {
        xTimerStop(BLE_xTimer, portMAX_DELAY);
        xTimerDelete(BLE_xTimer, portMAX_DELAY);
        BLE_xTimer = NULL;
    }

    if (created_ntf_task) {
        vTaskDelete(ntf_task_hdl);
        created_ntf_task = 0;
    }
    if(created_wr_task)
    {
        vTaskDelete(write_task_hdl);
        created_wr_task = 0;
    }
    struct bt_conn *conn;
    for (int i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        conn = bt_conn_lookup_id(i);
        if (conn) {
            bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
            bt_conn_unref(conn);
        }
    }

    for (int i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        clear_statistical_data(i);
    }

    bt_le_adv_stop();
    bt_le_scan_stop();
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
    if(argc > 4){
        printf("Parameter error\r\n");
        return;
    }
    if(argc >= 2){
        mst = atoi(argv[1]);
    }
    if(argc >= 3){
        slv = atoi(argv[2]);
    }
    if(argc >= 4){
        type = atoi(argv[3]);
    }

    printf("mst:%d slv:%d type:%d\r\n", mst, slv, type);
    if(0 != ble_test_init(mst, slv, type)){
        printf("Start ble test failed\r\n");
    }
}
#if defined(CONFIG_SHELL)
SHELL_CMD_EXPORT_ALIAS(cmd_ble_test, ble_tp_test, );
#else
const struct cli_command TpCmdSet[] STATIC_CLI_CMD_ATTRIBUTE = {

    {"ble_tp_test", "\r\n ble_tp_test:\r\n" , cmd_ble_test}, 
};
#endif
int ble_tp_cli_register(void)
{
    return 0;
}
