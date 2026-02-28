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
#include "wifi_mgmr.h"
#if (CONFIG_FHOST)
#include "fhost.h"
#endif
#include "at_main.h"
#include "at_core.h"
#include "at_port.h"
#include "at_base_config.h"
#include "at_wifi_config.h"
#include "at_wifi_mgmr.h"

/* Function declarations for wifi_mgmr functions */
extern int wifi_mgmr_ap_mac_set(uint8_t mac[6]);
extern int wifi_mgmr_sta_mac_set(uint8_t mac[6]);

#define DBG_TAG "WIFI_ADAPTER"
#include "log.h"

#define AT_WIFI_ADAPTER_PRINTF AT_CMD_PRINTF

// WiFi Adapter Wrapper Functions

// WiFi MAC address management
int at_wifi_mgmr_ap_mac_get(uint8_t mac[6])
{
    return wifi_mgmr_ap_mac_get(mac);
}

int at_wifi_mgmr_ap_mac_set(uint8_t mac[6])
{
    return wifi_mgmr_ap_mac_set(mac);
}

int at_wifi_mgmr_sta_mac_get(uint8_t mac[6])
{
    return wifi_mgmr_sta_mac_get(mac);
}

int at_wifi_mgmr_sta_mac_set(uint8_t mac[6])
{
    return wifi_mgmr_sta_mac_set(mac);
}

#ifdef CONFIG_ATMODULE_WIFI_STA
int at_wifi_mgmr_sta_state_get(void)
{
    return wifi_mgmr_sta_state_get();
}

int at_wifi_mgmr_sta_connect(const char *ssid, const char *key, const char *bssid, const char *akm_str, uint8_t pmf_cfg, uint16_t freq1, uint16_t freq2, uint8_t use_dhcp)
{
    return wifi_sta_connect(ssid, key, bssid, akm_str, pmf_cfg, freq1, freq2, use_dhcp);
}

int at_wifi_mgmr_sta_connect_ind_stat_get(at_wifi_mgmr_connect_ind_stat_info_t *info)
{
    wifi_mgmr_connect_ind_stat_info_t wifi_info;
    int ret = wifi_mgmr_sta_connect_ind_stat_get(&wifi_info);

    if (ret == 0) {
        // Copy only needed members from wifi_mgmr structure to adapter structure
        strncpy(info->ssid, wifi_info.ssid, sizeof(info->ssid) - 1);
        info->ssid[sizeof(info->ssid) - 1] = '\0';
        memcpy(info->bssid, wifi_info.bssid, sizeof(info->bssid));
        info->channel = wifi_info.channel;
        info->security = wifi_info.security;
    }

    return ret;
}

int at_wifi_mgmr_sta_rssi_get(int *rssi)
{
    return wifi_mgmr_sta_rssi_get(rssi);
}

int at_wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns)
{
    return wifi_mgmr_sta_ip_set(ip, mask, gw, dns);
}

int at_wifi_mgmr_sta_scanlist(void)
{
    return wifi_mgmr_sta_scanlist();
}

int at_wifi_mgmr_sta_scan(at_wifi_mgmr_scan_params_t *scan_cfg)
{
    // Convert adapter scan params to wifi_mgmr scan params
    wifi_mgmr_scan_params_t wifi_scan_cfg;

    memset(&wifi_scan_cfg, 0, sizeof(wifi_scan_cfg));
    wifi_scan_cfg.ssid_length = scan_cfg->ssid_length;
    memcpy(wifi_scan_cfg.ssid_array, scan_cfg->ssid_array, sizeof(wifi_scan_cfg.ssid_array));
    memcpy(wifi_scan_cfg.bssid, scan_cfg->bssid, sizeof(wifi_scan_cfg.bssid));
    wifi_scan_cfg.bssid_set_flag = scan_cfg->bssid_set_flag;
    wifi_scan_cfg.probe_cnt = scan_cfg->probe_cnt;
    wifi_scan_cfg.channels_cnt = scan_cfg->channels_cnt;
    memcpy(wifi_scan_cfg.channels, scan_cfg->channels, sizeof(wifi_scan_cfg.channels));
    wifi_scan_cfg.duration = scan_cfg->duration;
    wifi_scan_cfg.passive = scan_cfg->passive;

    return wifi_mgmr_sta_scan(&wifi_scan_cfg);
}

int at_wifi_mgmr_sta_scanlist_free(void)
{
    // This function may not be available, just return success
    return 0;
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
    /* Create a wrapper callback to convert between struct types */
    return wifi_mgmr_scan_ap_all(env, arg, (void (*)(void *, void *, struct wifi_mgmr_scan_item *))cb);
}

int at_wifi_mgmr_sta_disconnect(void)
{
    return wifi_sta_disconnect();
}

int at_wifi_mgmr_sta_autoconnect_disable(void)
{
    return wifi_mgmr_sta_autoconnect_disable();
}

int at_wifi_mgmr_sta_info_status_code_get(void)
{
    return wifi_mgmr_sta_info_status_code_get();
}

#endif

#ifdef CONFIG_ATMODULE_WIFI_AP
int at_wifi_mgmr_ap_state_get(void)
{
    return wifi_mgmr_ap_state_get();
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

int at_wifi_mgmr_ap_stop(void)
{
    return wifi_mgmr_ap_stop();
}

int at_wifi_mgmr_ap_start(at_wifi_mgmr_ap_params_t *config)
{
    // Convert adapter params to wifi_mgmr params
    wifi_mgmr_ap_params_t wifi_config;

    memset(&wifi_config, 0, sizeof(wifi_mgmr_ap_params_t));
    wifi_config.use_ipcfg = true; // hardcode

    wifi_config.ssid = config->ssid;
    wifi_config.key = config->key;
    wifi_config.hidden_ssid = config->hidden_ssid;
    wifi_config.channel = config->channel;
    wifi_config.use_dhcpd = config->use_dhcpd;
    wifi_config.start = config->start;
    wifi_config.limit = config->limit;
    wifi_config.ap_ipaddr = config->ap_ipaddr;
    wifi_config.ap_mask = config->ap_mask;
    wifi_config.akm = config->akm;

    return wifi_mgmr_ap_start(&wifi_config);
}

int at_wifi_mgmr_sta_channel_get(uint8_t *channel)
{
    return wifi_mgmr_sta_channel_get(channel);
}

int at_wifi_mgmr_conf_max_sta(uint8_t max_sta)
{
    return wifi_mgmr_conf_max_sta(max_sta);
}

#endif

#ifdef CONFIG_ATMODULE_FULL_FEAT
int at_wifi_mgmr_rf_pwr_off(void)
{
    return wifi_mgmr_rf_pwr_off();
}

int at_wifi_mgmr_rf_pwr_on(void)
{
    return wifi_mgmr_rf_pwr_on();
}

void at_wifi_mgmr_get_country_code(char *country_code)
{
    wifi_mgmr_get_country_code(country_code);
}

int at_wifi_mgmr_get_channel_nums(const char *country_code, uint8_t *c24g_cnt, uint8_t *c5g_cnt)
{
    return wifi_mgmr_get_channel_nums(country_code, c24g_cnt, c5g_cnt);
}

int at_wifi_mgmr_sta_wps_pbc(int auth)
{
    return wifi_mgmr_sta_wps_pbc(auth);
}

int at_wifi_mgmr_set_country_code(const char *code)
{
    return wifi_mgmr_set_country_code((char *)code);
}

int at_wifi_mgmr_set_mode(uint8_t ap_or_sta, at_wifi_mode_t mode)
{
    return wifi_mgmr_set_mode(ap_or_sta, mode);
}

int at_wifi_mgmr_get_mode(uint8_t ap_or_sta)
{
    return wifi_mgmr_get_mode(ap_or_sta);
}

void (*pfn_sniffer_cb)(uint8_t *pkt, int pkt_len, void *env) = NULL;
static void _sniffer_cb(struct bl_frame_info *info, void *arg)
{
    if (pfn_sniffer_cb) {
        pfn_sniffer_cb(info->payload, info->length, arg);
    }
}

int at_wifi_mgmr_sniffer_enable(at_wifi_mgmr_sniffer_item_t *sniffer_item)
{
    // These functions may not be available in current WiFi stack
    wifi_mgmr_sniffer_item_t item = {
        .itf = sniffer_item->itf,
        .type = 0,
        .prim20_freq = phy_channel_to_freq(0, sniffer_item->channel),
        .center1_freq = sniffer_item->center1_freq,
        .center2_freq = sniffer_item->center2_freq,
        .cb = _sniffer_cb,
        .cb_arg = sniffer_item->cb_arg,
    };
    pfn_sniffer_cb = sniffer_item->cb;
    wifi_mgmr_sniffer_enable(item);
}

int at_wifi_mgmr_sniffer_disable(at_wifi_mgmr_sniffer_item_t *sniffer_item)
{
    wifi_mgmr_sniffer_item_t item = {
        .itf = sniffer_item->itf,
    };

    wifi_mgmr_sniffer_disable(item);
    return -1;
}

int at_wifi_mgmr_state_get(void)
{
    int state = fhost_get_vif_state(MGMR_VIF_STA);

    if (state == FHOST_STA_CONNECTED) {
        return AT_WIFI_STATE_STA_CONNECTED;
    } else if (state == FHOST_STA_DISCONNECTED) {
        return AT_WIFI_STATE_STA_DISCONNECTED;
    }
    return -1;
}
#endif
