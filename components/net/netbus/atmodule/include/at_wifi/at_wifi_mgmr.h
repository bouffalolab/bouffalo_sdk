/**
  ******************************************************************************
  * @file    at_wifi_mgmr.h
  * @version V1.0
  * @date
  * @brief   WiFi Adapter Layer - Adapter between AT module and WiFi manager
  ******************************************************************************
  */

#ifndef __AT_WIFI_MGMR_H__
#define __AT_WIFI_MGMR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at_wifi_main.h"

// WiFi Adapter Constants
#ifdef WIFI_MGMR_SCAN_ITEMS_MAX
#define AT_WIFI_MGMR_SCAN_ITEMS_MAX WIFI_MGMR_SCAN_ITEMS_MAX
#else
#define AT_WIFI_MGMR_SCAN_ITEMS_MAX 50
#endif

#ifdef CFG_STA_MAX
#define AT_WIFI_MAX_STA_NUM (CFG_STA_MAX)
#else
#define AT_WIFI_MAX_STA_NUM (4)
#endif

#define AT_WIFI_MGMR_SSID_LEN   32
#define AT_WIFI_MGMR_MAX_CHANNELS 14

// WiFi Adapter Types - only include needed members
typedef struct at_wifi_mgmr_sta_basic_info {
    uint8_t sta_idx;
    uint8_t is_used;
    uint8_t sta_mac[6];
    int rssi;
} at_wifi_mgmr_sta_basic_info_t;

typedef struct at_wifi_mgmr_scan_item {
    uint32_t mode;
    uint32_t timestamp_lastseen;
    int ssid_len;
    uint8_t channel;
    int8_t rssi;
    char ssid[32];
    char ssid_tail[1];//always put ssid_tail after ssid
    uint8_t bssid[6];
    int8_t ppm_abs;
    int8_t ppm_rel;
    uint8_t auth;
    uint8_t cipher;
    uint8_t is_used;
    uint8_t wps;
    uint8_t best_antenna;
} at_wifi_mgmr_scan_item_t;

typedef struct at_wifi_mgmr_connect_ind_stat_info {
    char ssid[33];
    uint8_t bssid[6];
    uint8_t channel;
    uint8_t security;
} at_wifi_mgmr_connect_ind_stat_info_t;

typedef struct at_wifi_mgmr_scan_params {
    uint8_t ssid_length;
    uint8_t ssid_array[AT_WIFI_MGMR_SSID_LEN];
    uint8_t bssid[6];
    uint8_t bssid_set_flag;
    uint8_t probe_cnt;
    int channels_cnt;
    uint8_t channels[AT_WIFI_MGMR_MAX_CHANNELS];
    uint32_t duration;
    /// 0: active scan; 1: Passive scan request
    bool passive;
} at_wifi_mgmr_scan_params_t;

// WiFi structures for adapter - moved before function declarations
typedef struct at_wifi_mgmr_ap_params {
    char *ssid;
    char *key;
    uint8_t hidden_ssid;
    uint8_t channel;
    uint8_t use_dhcpd;
    uint8_t start;
    uint8_t limit;
    uint32_t ap_ipaddr;
    uint32_t ap_mask;
    char *akm;
} at_wifi_mgmr_ap_params_t;

typedef void (*at_wifi_mgmr_sniffer_cb_t)(uint8_t *pkt, int pkt_len, void *env);

typedef struct at_wifi_mgmr_sniffer_item {
    /// interface index
    char *itf;
    uint16_t channel;
    /// Frequency center of the contiguous channel or center of Primary 80+80 (in MHz)
    uint16_t center1_freq;
    /// Frequency center of the non-contiguous secondary 80+80 (in MHz)
    uint16_t center2_freq;
    at_wifi_mgmr_sniffer_cb_t cb;
    void *cb_arg;
} at_wifi_mgmr_sniffer_item_t;

// WiFi Adapter Function Declarations
int at_wifi_mgmr_state_get(void);

// WiFi state management
int at_wifi_mgmr_sta_state_get(void);
int at_wifi_mgmr_ap_state_get(void);
int at_wifi_mgmr_rf_pwr_off(void);
int at_wifi_mgmr_rf_pwr_on(void);

int at_wifi_mgmr_sta_connect(const char *ssid, const char *key, const char *bssid, const char *akm_str, uint8_t pmf_cfg, uint16_t freq1, uint16_t freq2, uint8_t use_dhcp);

// WiFi connection management
int at_wifi_mgmr_sta_connect_ind_stat_get(at_wifi_mgmr_connect_ind_stat_info_t *info);
int at_wifi_mgmr_sta_rssi_get(int *rssi);
int at_wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns);
int at_wifi_mgmr_sta_disconnect(void);
int at_wifi_mgmr_sta_wps_pbc(int auth);

// WiFi scan management
int at_wifi_mgmr_sta_scanlist(void);
int at_wifi_mgmr_sta_scan(at_wifi_mgmr_scan_params_t *scan_cfg);
int at_wifi_mgmr_sta_scanlist_free(void);
int at_wifi_mgmr_sta_scan_ap_all(void *env, void *arg, void (*cb)(void *, void *, at_wifi_mgmr_scan_item_t *));

// WiFi AP management
int at_wifi_mgmr_ap_sta_info_get(at_wifi_mgmr_sta_basic_info_t *sta_info, int index);
int at_wifi_mgmr_ap_sta_delete(int sta_idx);

// WiFi MAC address management
int at_wifi_mgmr_ap_mac_get(uint8_t mac[6]);
int at_wifi_mgmr_ap_mac_set(uint8_t mac[6]);
int at_wifi_mgmr_sta_mac_get(uint8_t mac[6]);
int at_wifi_mgmr_sta_mac_set(uint8_t mac[6]);

// WiFi country and channel management
void at_wifi_mgmr_get_country_code(char *country_code);
int at_wifi_mgmr_get_channel_nums(const char *country_code, uint8_t *c24g_cnt, uint8_t *c5g_cnt);
int at_wifi_mgmr_set_country_code(const char *code);

// WiFi utility functions
int at_wifi_mgmr_scan_get_ecn(uint8_t auth);
int at_wifi_mgmr_scan_get_cipher(uint8_t cipher);

// WiFi low-level management functions
int at_wifi_mgmr_sta_disconnect_lowlevel(void);
int at_wifi_mgmr_sta_autoconnect_disable(void);
int at_wifi_mgmr_ap_stop(void);
int at_wifi_mgmr_ap_start(at_wifi_mgmr_ap_params_t *config);
int at_wifi_mgmr_conf_max_sta(uint8_t max_sta);
int at_wifi_mgmr_sta_channel_get(uint8_t *channel);
int at_wifi_mgmr_set_mode(uint8_t ap_or_sta, at_wifi_mode_t mode);
int at_wifi_mgmr_get_mode(uint8_t ap_or_sta);
int at_wifi_mgmr_sta_info_status_code_get(void);
int at_wifi_mgmr_sniffer_enable(at_wifi_mgmr_sniffer_item_t *sniffer_item);
int at_wifi_mgmr_sniffer_disable(at_wifi_mgmr_sniffer_item_t *sniffer_item);


#ifdef __cplusplus
}
#endif

#endif /* AT_WIFI_ADAPTER_H */
