#include "shell.h"
#include <FreeRTOS.h>
#include "task.h"
#include "board.h"

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#if defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#elif defined(BL616)
#include "btble_lib_api.h"
#include "bl616_glb.h"
#include "rfparam_adapter.h"
#elif defined(BL808)
#include "btble_lib_api.h"
#include "bl808_glb.h"
#endif

#include "hci_driver.h"
#include "hci_core.h"
#include "gatt.h"
#include "bt_log.h"

#include "bflb_mtd.h"
#include "easyflash.h"

typedef struct{
    TaskHandle_t handle;
    uint32_t wr_hdl;
}wr_arg_t;

const char* discovery_str[] = {
    "primary",
    "secondary",
    "include",
    "characteristic",
    "descriptor",
    "attribute",
};

static u16_t gatt_mtu_size;
static wr_arg_t wr_arg;
static struct bflb_device_s *uart0;
static struct bt_conn *default_conn;
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;
static void ble_start_scan(void);
static void start_discovery(u8_t type);

extern void shell_init_with_task(struct bflb_device_s *shell);

static void ble_write_task(void *arg)
{
    wr_arg_t* args = (wr_arg_t*)arg;
    uint8_t data[244] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    for(int i = 0; i < sizeof(data); ++i){
        data[i] = i & 0xff;
    }

    while(args->handle){
        int err = bt_gatt_write_without_response(default_conn, (uint32_t)arg & 0xffff, data,
            (gatt_mtu_size - 3) > sizeof(data) ? sizeof(data) : (gatt_mtu_size - 3), false);
        if(err){
            printf("Write without response (err %d)\r\n", err);
        }
        printf("Write success\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    printf("Deleted blewrite task\r\n");
    vTaskDelete(NULL);
}

void ble_write_task_set(bool enable, uint16_t handle)
{
    wr_arg.wr_hdl = handle + 0;
    if(!enable){
        if(wr_arg.handle){
            wr_arg.handle = NULL;
        }
        return;
    }

    if(xTaskCreate(ble_write_task, (char*)"blewrite", 256, (void*)&wr_arg, configMAX_PRIORITIES - 5, &wr_arg.handle) == pdPASS){
        printf("Create ble_write_task success, start write\r\n");
    }
    else{
        printf("Create ble_write_task failed\r\n");
    }
}

static u8_t notify_func(struct bt_conn *conn,
			struct bt_gatt_subscribe_params *params,
			const void *data, u16_t length)
{
    if (!params->value) {
        printf("Unsubscribed\r\n");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }
    printf("Subscribed\r\n");
    return BT_GATT_ITER_CONTINUE;
}

static void do_subscribe(uint16_t ccc_handle, uint16_t val)
{
    subscribe_params.ccc_handle = ccc_handle;
    subscribe_params.value = val;
    subscribe_params.notify = notify_func;

    int err = bt_gatt_subscribe(default_conn, &subscribe_params);
    if (err) {
        printf("Subscribe failed (err %d)\r\n", err);
    } else {
        printf("Subscribed starting\r\n");
    }
}

static void print_chrc_props(u8_t properties)
{
    printf("Properties: ");

    if (properties & BT_GATT_CHRC_BROADCAST) {
        printf("[bcast]\r\n");
    }
    if (properties & BT_GATT_CHRC_READ) {
        printf("[read]\r\n");
    }
    if (properties & BT_GATT_CHRC_WRITE) {
        printf("[write]\r\n");
    }
    if (properties & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
        printf("[write w/w rsp]\r\n");
    }
    if (properties & BT_GATT_CHRC_NOTIFY) {
        printf("[notify]\r\n");
    }
    if (properties & BT_GATT_CHRC_INDICATE) {
        printf("[indicate]\r\n");
    }
    if (properties & BT_GATT_CHRC_AUTH) {
        printf("[auth]\r\n");
    }
    if (properties & BT_GATT_CHRC_EXT_PROP) {
        printf("[ext prop]\r\n");
    }
}

static u8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params)
{
    struct bt_gatt_service_val *gatt_service;
    struct bt_gatt_chrc *gatt_chrc;
    struct bt_gatt_include *gatt_include;
    char str[37];

    if (!attr) {
        printf( "Discover %s complete\r\n", discovery_str[discover_params.type]);
        if(discover_params.type == BT_GATT_DISCOVER_PRIMARY){
            start_discovery(BT_GATT_DISCOVER_CHARACTERISTIC);
        }
        else if(discover_params.type == BT_GATT_DISCOVER_CHARACTERISTIC){
            start_discovery(BT_GATT_DISCOVER_DESCRIPTOR);
        }
        else{
            memset(params, 0, sizeof(*params));
        }
        return BT_GATT_ITER_STOP;
    }

    switch (params->type) {
    case BT_GATT_DISCOVER_SECONDARY:
    case BT_GATT_DISCOVER_PRIMARY:
        gatt_service = attr->user_data;
        bt_uuid_to_str(gatt_service->uuid, str, sizeof(str));
        printf("Service %s found: start handle %x, end_handle %x\r\n", str, attr->handle, gatt_service->end_handle);
        break;
    case BT_GATT_DISCOVER_CHARACTERISTIC:
        gatt_chrc = attr->user_data;
        bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));
        printf("Characteristic %s found: attr->handle %x  chrc->handle %x \r\n", str, attr->handle,gatt_chrc->value_handle);
        print_chrc_props(gatt_chrc->properties);
        if(gatt_chrc->properties & BT_GATT_CHRC_NOTIFY){
            do_subscribe(attr->handle + 2, BT_GATT_CCC_NOTIFY);
        }
        if(!wr_arg.handle){
            ble_write_task_set(true, gatt_chrc->value_handle);
        }
        break;
    case BT_GATT_DISCOVER_INCLUDE:
        gatt_include = attr->user_data;
        bt_uuid_to_str(gatt_include->uuid, str, sizeof(str));
        printf("Include %s found: handle %x, start %x, end %x\r\n", str, attr->handle,
            gatt_include->start_handle, gatt_include->end_handle);
        break;
    default:
        bt_uuid_to_str(attr->uuid, str, sizeof(str));
        printf("Descriptor %s found: handle %x\r\n", str, attr->handle);
        break;
    }

    return BT_GATT_ITER_CONTINUE;
}

static void start_discovery(u8_t type)
{
    int err;

    discover_params.func = discover_func;
    discover_params.start_handle = 0x0001;
    discover_params.end_handle = 0xffff;
    discover_params.type = type;
    discover_params.uuid = NULL;

    err = bt_gatt_discover(default_conn, &discover_params);
    if (err) {
        printf("Discover %s failed (err %d)\r\n", discovery_str[discover_params.type], err);
    } 
}

static void ble_connected(struct bt_conn *conn, u8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if(conn->type != BT_CONN_TYPE_LE) {
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    if (err || conn->type != BT_CONN_TYPE_LE) {
        printf("Failed to connect to %s (%u) \r\n", addr, err);
        return;
    }

    printf("Connected: %s \r\n", addr);

    if (!default_conn) {
        default_conn = conn;
    }

    gatt_mtu_size = bt_gatt_get_mtu(default_conn);

    start_discovery(BT_GATT_DISCOVER_PRIMARY);
}

static void ble_disconnected(struct bt_conn *conn, u8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if(conn->type != BT_CONN_TYPE_LE){
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printf("Disconnected: %s (reason %u) \r\n", addr, reason);

    if (default_conn == conn) {
        #if defined(CONFIG_BT_CENTRAL)
        if(conn->role == BT_HCI_ROLE_MASTER)
            bt_conn_unref(conn);
        #endif
        default_conn = NULL;
    }

    // restart scan
    ble_start_scan();

    // stop write task
    ble_write_task_set(false, 0);
}

static struct bt_conn_cb ble_conn_callbacks = {
    .connected	=   ble_connected,
    .disconnected	=   ble_disconnected,
};

static bool data_cb(struct bt_data *data, void *user_data)
{
    char *name = user_data;
    u8_t len;

    switch (data->type) {
    case BT_DATA_NAME_SHORTENED:
    case BT_DATA_NAME_COMPLETE:
        len = (data->data_len > 30 - 1)?(30 - 1):(data->data_len);
        memcpy(name, data->data, len);
        return false;		
    default:
        return true;
    }
}
static void device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t evtype,
			 struct net_buf_simple *buf)
{
    char le_addr[BT_ADDR_LE_STR_LEN];
    char name[30];
    int ret;
    struct bt_conn* conn = NULL;
    struct bt_le_conn_param param = {
        .interval_min =  BT_GAP_INIT_CONN_INT_MIN,
        .interval_max =  BT_GAP_INIT_CONN_INT_MAX,
        .latency = 0,
        .timeout = 400,
    };

    (void)memset(name, 0, sizeof(name));
    bt_data_parse(buf, data_cb, name);

    if(!strcmp(name,  CONFIG_BT_DEVICE_NAME)){
        bt_addr_le_to_str(addr, le_addr, sizeof(le_addr));
        printf("[DEVICE FOUND]: %s, AD evt type %u, RSSI %i %s \r\n",le_addr, evtype, rssi, name);
        // Stop scan 
        ret = bt_le_scan_stop();
        if(ret){
            printf("Stop scan fail err = %d\r\n", ret);
            return;
        }

        // Do connect
        conn = bt_conn_create_le(addr, /*BT_LE_CONN_PARAM_DEFAULT*/&param);

        if(!conn){
            printf("Connection failed\r\n");
        }else{
            printf("Connection starting\r\n");
        }
    }
}

static void ble_start_scan(void)
{
    struct bt_le_scan_param scan_param;
    int err;

    memset(&scan_param, 0, sizeof(scan_param));

    scan_param.type = BT_HCI_LE_SCAN_ACTIVE;
    scan_param.filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE;
    scan_param.interval = BT_GAP_SCAN_FAST_INTERVAL;
    scan_param.window = BT_GAP_SCAN_FAST_WINDOW;

    err = bt_le_scan_start(&scan_param, device_found);

    if(err){
        printf("Failed to start scan (err %d) \r\n", err);
    }else{
        printf("Start scan successfully \r\n");
    }
}

static void bt_gatt_mtu_changed_cb(struct bt_conn *conn, int mtu)
{
    if(conn == default_conn){
        gatt_mtu_size = mtu;
    }
}

static void ble_notification_all_cb_t(struct bt_conn *conn, u16_t handle,const void *data, u16_t length)
{
    printf("handle %x, %s\r\n", handle, bt_hex(data, length));
}

void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
            bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);

        bt_conn_cb_register(&ble_conn_callbacks);
        #if defined(BFLB_BLE_MTU_CHANGE_CB)
        bt_gatt_register_mtu_callback(bt_gatt_mtu_changed_cb);
        #endif

        // start advertising
        ble_start_scan();

        // Register notification callback
        bt_gatt_register_notification_callback(ble_notification_all_cb_t);
    }
}

static TaskHandle_t app_start_handle;

static void app_start_task(void *pvParameters)
{
    // Initialize BLE controller
    #if defined(BL702) || defined(BL602)
    ble_controller_init(configMAX_PRIORITIES - 1);
    #else
    btble_controller_init(configMAX_PRIORITIES - 1);
    #endif
    // Initialize BLE Host stack
    hci_driver_init();
    bt_enable(bt_enable_cb);

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    configASSERT((configMAX_PRIORITIES > 4));

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    bflb_mtd_init();
    /* ble stack need easyflash kv */
    easyflash_init();

#if defined(BL616)
    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }
#endif

    xTaskCreate(app_start_task, (char *)"app_start", 1024, NULL, configMAX_PRIORITIES - 2, &app_start_handle);

    vTaskStartScheduler();

    while (1) {
    }
}
