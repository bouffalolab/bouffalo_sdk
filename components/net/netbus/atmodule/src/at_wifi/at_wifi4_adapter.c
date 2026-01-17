/**
  ******************************************************************************
  * @file    at_wifi_adapter.c
  * @version V1.0
  * @date
  * @brief   WiFi Adapter Layer Implementation
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <lwip/tcpip.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <lwip/udp.h>
#include <lwip/altcp.h>
#include <lwip/altcp_tcp.h>
#include <lwip/altcp_tls.h>
#include <lwip/err.h>
#include <lwip/netif.h>
#include <lwip/netifapi.h>

#include <wifi_mgmr_ext.h>

#include "at_main.h"
#include "at_core.h"
#include "at_port.h"
#include "at_base_config.h"
#include "at_wifi_config.h"
#include "at_wifi_mgmr.h"

#define DBG_TAG "WIFI_ADAPTER"
#include "log.h"

#define AT_WIFI_ADAPTER_PRINTF AT_CMD_PRINTF

static int at_wifi_event_code_get(uint32_t code)
{
    if (code == CODE_WIFI_ON_MGMR_DONE) {
        return AT_WIFI_EVENT_INIT_DONE;
    } else if (code == CODE_WIFI_ON_CONNECTED) {
        return AT_WIFI_EVENT_CONNECTED;
    } else if (code == CODE_WIFI_ON_GOT_IP) {
        return AT_WIFI_EVENT_GOTIP;
    } else if (code == CODE_WIFI_ON_DISCONNECT) {
        return AT_WIFI_EVENT_DISCONNECTED;
    } else if (code == CODE_WIFI_ON_SCAN_DONE) {
        return AT_WIFI_EVENT_SCAN_DONE;
    } else if (code == CODE_WIFI_ON_AP_STARTED) {
        return AT_WIFI_EVENT_AP_STARTED;
    } else if (code == CODE_WIFI_ON_AP_STOPPED) {
        return AT_WIFI_EVENT_AP_STOP;
    } else if (code == CODE_WIFI_ON_AP_STA_ADD) {
        return AT_WIFI_EVENT_AP_STA_ADD;
    } else if (code == CODE_WIFI_ON_AP_STA_DEL) {
        return AT_WIFI_EVENT_AP_STA_DEL;
    }
    return 0;
}

static void _wifi_event_cb(void *private_data, uint32_t code)
{
    static wifi_conf_t conf = {
        .country_code = "00",
    };

    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            wifi_mgmr_task_start();
        }
        break;
        default: {
            at_wifi_event_notify(private_data, at_wifi_event_code_get(code));
        }
        break;
    }
}

static void wifi_event_start(uint32_t code)
{
    BaseType_t xReturn;

    xReturn = xTimerPendFunctionCall(_wifi_event_cb, NULL, code, 0);
    configASSERT(xReturn == pdPASS);
}

void mfg_config(void)
{

}

void wifi_event_handler(async_input_event_t event, void *private_data)
{
    wifi_event_start(event->code);
}

// WiFi Adapter Wrapper Functions

struct netif *at_wifi_netif_get(uint8_t vif_idx)
{
    if (AT_WIFI_VIF_STA == vif_idx) {
        return wifi_mgmr_sta_netif_get();
    } else {
        return wifi_mgmr_ap_netif_get();
    }
}

int at_wifi_mgmr_sta_state_get(void)
{
    int state;
    wifi_mgmr_state_get(&state);

    return (state == WIFI_STATE_CONNECTED_IP_GETTING) || (state == WIFI_STATE_CONNECTED_IP_GOT);
}

int at_wifi_mgmr_ap_state_get(void)
{
    int state;
    wifi_mgmr_state_get(&state);
    return WIFI_STATE_AP_IS_ENABLED(state);
}

int at_wifi_mgmr_sta_connect(const char *ssid, const char *key, const char *bssid, const char *akm_str, uint8_t pmf_cfg, uint16_t freq1, uint16_t freq2, uint8_t use_dhcp)
{
    return wifi_mgmr_sta_connect_mid(wifi_mgmr_sta_enable(), ssid, strlen(key)?key:NULL, NULL, bssid, 0, 0, use_dhcp, WIFI_CONNECT_DEFAULT);
}

int at_wifi_mgmr_rf_pwr_off(void)
{
    return wifi_mgmr_rf_pwr_off();
}

int at_wifi_mgmr_rf_pwr_on(void)
{
    return wifi_mgmr_rf_pwr_on();
}

int at_wifi_mgmr_sta_connect_ind_stat_get(at_wifi_mgmr_connect_ind_stat_info_t *info)
{
    wifi_mgmr_sta_connect_ind_stat_info_t wifi_info;
    wifi_mgmr_sta_connect_ind_stat_get(&wifi_info);

    // Copy only needed members from wifi_mgmr structure to adapter structure
    strncpy(info->ssid, wifi_info.ssid, sizeof(info->ssid) - 1);
    info->ssid[sizeof(info->ssid) - 1] = '\0';
    memcpy(info->bssid, wifi_info.bssid, sizeof(info->bssid));
    info->channel = wifi_info.chan_id;
    info->security = 0;

    return 0;
}

int at_wifi_mgmr_sta_rssi_get(int *rssi)
{
    return wifi_mgmr_rssi_get(rssi);
}

int at_wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns)
{
    return wifi_mgmr_sta_ip_set(ip, mask, gw, dns, 0);
}

int at_wifi_mgmr_sta_scanlist(void)
{
    return wifi_mgmr_sta_scanlist();
}

int at_wifi_mgmr_sta_scan(at_wifi_mgmr_scan_params_t *scan_cfg)
{
    return wifi_mgmr_scan_adv(NULL,
                              NULL,
                              scan_cfg->channels,
                              scan_cfg->channels_cnt,
                              scan_cfg->bssid,
                              scan_cfg->ssid_array,
                              !scan_cfg->passive,
                              scan_cfg->duration);
}

int at_wifi_mgmr_sta_scanlist_free(void)
{
    // This function may not be available, just return success
    return 0;
}

int at_wifi_mgmr_ap_sta_info_get(at_wifi_mgmr_sta_basic_info_t *sta_info, int index)
{
    struct wifi_sta_basic_info wifi_sta_info;
    int ret = wifi_mgmr_ap_sta_info_get(&wifi_sta_info, index);

    if (ret == 0) {
        // Copy only available members to adapter structure
        sta_info->sta_idx = wifi_sta_info.sta_idx;
        sta_info->is_used = wifi_sta_info.is_used;
        memcpy(sta_info->sta_mac, wifi_sta_info.sta_mac, sizeof(sta_info->sta_mac));
        sta_info->rssi = 0; // Set default value if not available
    }

    return ret;
}

int at_wifi_mgmr_ap_sta_delete(int sta_idx)
{
    return wifi_mgmr_ap_sta_delete(sta_idx);
}

void at_wifi_mgmr_get_country_code(char *country_code)
{
    wifi_conf_t conf;
    wifi_mgmr_get_wifi_channel_conf(&conf);

    strlcpy(country_code, conf.country_code, sizeof(conf.country_code));
}

int at_wifi_mgmr_get_channel_nums(const char *country_code, uint8_t *c24g_cnt, uint8_t *c5g_cnt)
{
    wifi_conf_t conf;
    wifi_mgmr_get_wifi_channel_conf(&conf);

    if (c24g_cnt) {
        *c24g_cnt = conf.channel_nums;
    }
    return 0;
}

int at_wifi_mgmr_sta_disconnect(void)
{
    return wifi_mgmr_sta_disconnect();
}

int at_wifi_mgmr_sta_wps_pbc(int auth)
{
    return -1;
}

int at_wifi_mgmr_set_country_code(const char *code)
{
    return wifi_mgmr_set_country_code((char *)code);
}

int at_wifi_mgmr_sta_autoconnect_disable(void)
{
    return wifi_mgmr_sta_autoconnect_disable();
}

int at_wifi_mgmr_ap_stop(void)
{
    return wifi_mgmr_ap_stop(NULL);
}

int at_wifi_mgmr_ap_start(at_wifi_mgmr_ap_params_t *config)
{
    return wifi_mgmr_ap_start(wifi_mgmr_ap_enable(), config->ssid, config->hidden_ssid, config->key, config->channel);
}

int at_wifi_mgmr_conf_max_sta(uint8_t max_sta)
{
    return wifi_mgmr_conf_max_sta(max_sta);
}

int at_wifi_mgmr_sta_channel_get(uint8_t *channel)
{
    int chan;
    int ret = wifi_mgmr_channel_get(&chan);
    *channel = chan;
    return ret;
}

int at_wifi_mgmr_set_mode(uint8_t ap_or_sta, at_wifi_mode_t mode)
{
    return -1;
}

int at_wifi_mgmr_get_mode(uint8_t ap_or_sta)
{
    return -1;
}

int at_wifi_mgmr_sta_info_status_code_get(void)
{
    int status_code;

    wifi_mgmr_status_code_get(&status_code);
    return status_code;
}

void (*pfn_sniffer_cb)(uint8_t *pkt, int pkt_len, void *env) = NULL;
static void _sniffer_cb_adv(void *env, void *pkt_wrap, bl_rx_info_t *info)
{
    //if (pfn_sniffer_cb) {
    //    pfn_sniffer_cb(pkt_wrap, info->length, env);
    //}
}

int at_wifi_mgmr_sniffer_enable(at_wifi_mgmr_sniffer_item_t *sniffer_item)
{
    pfn_sniffer_cb = sniffer_item->cb;
    wifi_mgmr_sniffer_register_adv(sniffer_item->cb_arg, _sniffer_cb_adv);
    wifi_mgmr_sniffer_enable();
    return 0;
}

int at_wifi_mgmr_sniffer_disable(at_wifi_mgmr_sniffer_item_t *sniffer_item)
{
    wifi_mgmr_sniffer_disable();
    return 0;
}

int at_wifi_mgmr_state_get(void)
{
    int state;
    wifi_mgmr_state_get(&state);
    switch (state) {
        case WIFI_STATE_DISCONNECT:
            return AT_WIFI_STATE_STA_DISCONNECTED;
        break;

        case WIFI_STATE_CONNECTED_IP_GETTING:
        case WIFI_STATE_CONNECTED_IP_GOT:
            return AT_WIFI_STATE_STA_CONNECTED;
        break;

        case WIFI_STATE_CONNECTING:
            return AT_WIFI_STATE_STA_CONNECTING;
        break;

        default:
        break;
    }
    return AT_WIFI_STATE_STA_DISCONNECTED;
}

// WiFi utility functions
int at_wifi_mgmr_scan_get_ecn(uint8_t auth)
{
    if (auth == WIFI_EVENT_BEACON_IND_AUTH_OPEN) {
        return 0;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WEP) {
        return 1;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK) {
        return 2;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK) {
        return 3;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK) {
        return 4;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT) {
        return 5;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE) {
        return 6;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE) {
        return 7;
    } else {
        return 8;
    }
}

int at_wifi_mgmr_scan_get_cipher(uint8_t cipher)
{
    if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_NONE) {
        return 0;
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_WEP) {
        return 1;
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_AES) {
        return 4;
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP) {
        return 3;
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES) {
        return 5;
    } else {
        return 7;
    }
}

int at_wifi_mgmr_scan_ap_all(void *env, void *arg, void (*cb)(void *, void *, at_wifi_mgmr_scan_item_t *))
{
    return wifi_mgmr_scan_ap_all(env, arg, cb);
}

int at_wifi_start(void)
{
    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);
    at_wifi_main_init();
    return 0;
}

