#include "blsync_ble_app.h"
#include <FreeRTOS.h>
#include <task.h>
#include "blsync_ble.h"
#include "ble_cli_cmds.h"
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

#include "wifi_mgmr_ext.h"

#include "lwip/ip4.h"

static bl_ble_sync_t *gp_index = NULL;

void (*scan_complete_cb)(void *) = NULL;

static void wifiprov_connect_ap_ind(struct wifi_conn *info)
{
    printf("Recevied indication to connect to AP\r\n");
    wifi_sta_connect((char *)info->ssid, (char *)info->pask, NULL, NULL, 1, 0, 0, 1);
}

static void wifiprov_disc_from_ap_ind(void)
{
    printf("Recevied indication to disconnect to AP\r\n");
    wifi_sta_disconnect();
}

static void wifiprov_wifi_scan(void (*complete)(void *))
{
    printf("Recevied indication to wifi scan\r\n");
    wifi_mgmr_scan_params_t config;
    memset(&config, 0, sizeof(wifi_mgmr_scan_params_t));

    wifi_mgmr_sta_scan(&config);

    scan_complete_cb = complete;
}

static void wifiprov_api_state_get(void (*state_get)(void *))
{
    printf("Recevied indication to wifi state get\r\n");

    wifi_mgmr_connect_ind_stat_info_t info;
    ip4_addr_t ip, gw, mask;
    struct wifi_state state;

    memset(&state, 0, sizeof(state));
    memset(&info, 0, sizeof(info));

    wifi_sta_ip4_addr_get(&ip.addr, &mask.addr, &gw.addr, NULL);
    wifi_mgmr_sta_connect_ind_stat_get(&info);

    if (ip.addr == 0) {
        state.state = 0x03;
    } else {
        state.state = 0x04;
    }

    strcpy(state.ip, ip4addr_ntoa(&ip));
    strcpy(state.mask, ip4addr_ntoa(&mask));
    strcpy(state.gw, ip4addr_ntoa(&gw));
    memcpy(state.ssid, info.ssid, sizeof(state.ssid));
    memcpy(state.bssid, info.bssid, sizeof(state.bssid));
    state.ssid_tail[0] = 0;

    printf("IP  :%s \r\n", state.ip);
    printf("GW  :%s \r\n", state.gw);
    printf("MASK:%s \r\n", state.mask);

    if (state_get) {
        state_get(&state);
    }
}

static void blesync_complete_cb(void *p_arg)
{
    bl_ble_sync_t *p_index = (bl_ble_sync_t *)p_arg;
    bl_ble_sync_stop(p_index);
    vPortFree(p_index);
}

static struct blesync_wifi_func WifiProv_conn_callback = {
    .local_connect_remote_ap = wifiprov_connect_ap_ind,
    .local_disconnect_remote_ap = wifiprov_disc_from_ap_ind,
    .local_wifi_scan = wifiprov_wifi_scan,
    .local_wifi_state_get = wifiprov_api_state_get,
};

static void ble_start_adv(void)
{
    struct bt_le_adv_param param;
    int err = -1;
    struct bt_data adv_data[1] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL)
    };
    struct bt_data adv_rsp[1] = {
        BT_DATA_BYTES(BT_DATA_MANUFACTURER_DATA, "BL616")
    };

    memset(&param, 0, sizeof(param));
    // Set advertise interval
    param.interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
    param.interval_max = BT_GAP_ADV_FAST_INT_MAX_2;
    /*Get adv type, 0:adv_ind,  1:adv_scan_ind, 2:adv_nonconn_ind 3: adv_direct_ind*/
    param.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME);

    err = bt_le_adv_start(&param, adv_data, ARRAY_SIZE(adv_data), adv_rsp, ARRAY_SIZE(adv_rsp));
    if (err) {
        printf("Failed to start advertising (err %d) \r\n", err);
    }
    printf("Start advertising success.\r\n");
}

static void blsync_init(int err)
{
    if (err != 0) {
        return;
    }
    if (gp_index != NULL) {
        printf("blsync already started\r\n");
        return;
    }
#ifdef CONFIG_BT_STACK_CLI
    ble_cli_register();
#endif /* CONFIG_BT_STACK_CLI */
    gp_index = pvPortMalloc(sizeof(bl_ble_sync_t));
    if (gp_index == NULL) {
        return;
    }
    bl_ble_sync_start(gp_index,
                      &WifiProv_conn_callback,
                      blesync_complete_cb,
                      (void *)gp_index);
}

void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
               bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);

        blsync_init(0);
        // start advertising
        ble_start_adv();
    }
}

static void ble_stack_start(void)
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
}

void blsync_ble_start(void)
{
    static uint8_t stack_started = 0;

    if (stack_started == 1) {
        blsync_init(0);
        return;
    }
    ble_stack_start();
    stack_started = 1;
}

void blsync_ble_stop(void)
{
    bl_ble_sync_stop(gp_index);
    vPortFree(gp_index);
    gp_index = NULL;
}

static void wifi_scan_item_cb(void *env, void *arg, wifi_mgmr_scan_item_t *item)
{
    blesync_wifi_item_t wifi_item;
    void (*complete)(void *) = (void (*)(void *))arg;

    wifi_item.auth = item->auth;
    wifi_item.rssi = item->rssi;
    wifi_item.channel = item->channel;
    wifi_item.ssid_len = item->ssid_len;
    memcpy(wifi_item.ssid, item->ssid, sizeof(wifi_item.ssid));
    memcpy(wifi_item.bssid, item->bssid, sizeof(wifi_item.bssid));

    if (complete) {
        complete(&wifi_item);
    }
}

void blsync_scan_all_ap(void)
{
    wifi_mgmr_scan_ap_all(NULL, scan_complete_cb, wifi_scan_item_cb);
}