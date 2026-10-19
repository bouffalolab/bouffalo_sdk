#include "stdio.h"
#include "stdint.h"
#include "wifi_mgmr_ext.h"
#include "bflb_blesync.h"
#include "lwip/ip4_addr.h"

void (*scan_complete_cb)(void *) = NULL;

static void wifiprov_connect_ap_ind(struct wifi_conn *info)
{
    printf("Recevied indication to connect to AP\r\n");
    wifi_sta_connect((char *)info->ssid, (char *)info->pask, NULL, NULL, 0, 0, 0, 1);
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

    memset(&ip, 0, sizeof(ip4_addr_t));
    memset(&gw, 0, sizeof(ip4_addr_t));
    memset(&mask, 0, sizeof(ip4_addr_t));
    memset(&state, 0, sizeof(state));
    memset(&info, 0, sizeof(info));

    wifi_sta_ip4_addr_get(&ip.addr, &mask.addr, &gw.addr, NULL);
    wifi_mgmr_sta_connect_ind_stat_get(&info);

    if (ip.addr == 0) {
        state.state = 0x03;
    } else {
        state.state = 0x04;
    }

    strlcpy(state.ip, ip4addr_ntoa(&ip), sizeof(state.ip));
    strlcpy(state.mask, ip4addr_ntoa(&mask), sizeof(state.mask));
    strlcpy(state.gw, ip4addr_ntoa(&gw), sizeof(state.gw));
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

static const struct blesync_wifi_func WifiProv_conn_callback = {
    .local_connect_remote_ap = wifiprov_connect_ap_ind,
    .local_disconnect_remote_ap = wifiprov_disc_from_ap_ind,
    .local_wifi_scan = wifiprov_wifi_scan,
    .local_wifi_state_get = wifiprov_api_state_get,
};

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

const struct blesync_wifi_func *wifi_drv_register(void)
{
    return &WifiProv_conn_callback;
}

void bflb_blesync_scan_all_ap(void)
{
    wifi_mgmr_scan_ap_all(NULL, scan_complete_cb, wifi_scan_item_cb);
}
