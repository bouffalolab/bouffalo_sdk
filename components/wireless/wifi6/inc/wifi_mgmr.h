#ifndef __WIFI_MGMR_H_
#define __WIFI_MGMR_H_

#include "wifi_mgmr_ext.h"
#include "rtos_al.h"

#define REMOTE_STA_MAX 4

#define MAC_ADDR_LIST(m) (m)[0], (m)[1], (m)[2], (m)[3], (m)[4], (m)[5]

#define WIFI_MGMR_CONFIG_SCAN_ITEM_TIMEOUT    (15000)
#define WIFI_MGMR_SCAN_ITEMS_MAX (50)

struct ieee80211_dot_d {
    char *code;
    int channel_num;
};

struct wlan_netif {
    int mode;//0: sta; 1: ap
    int set;
    uint8_t mac[6];
    uint8_t dhcp_started;
};

typedef struct wifi_mgmr_sta_basic_info {
    uint8_t  sta_idx;
    uint8_t  is_used;
    uint8_t  sta_mac[6];
    uint16_t aid;
} wifi_mgmr_sta_basic_info_t;

typedef struct wifi_mgmr {
    struct wlan_netif wlan_sta;
    struct wlan_netif wlan_ap;

    rtos_mutex scan_items_lock;
    wifi_mgmr_scan_item_t scan_items[WIFI_MGMR_SCAN_ITEMS_MAX];
    //the params info for sta connecting 
    wifi_mgmr_sta_connect_params_t sta_connect_param;
    //router info for sta mode
    wifi_mgmr_connect_ind_stat_info_t wifi_mgmr_stat_info;
    //sta info for AP mode
    wifi_mgmr_sta_basic_info_t ap_sta_info[REMOTE_STA_MAX];
    //ap info for AP mode
    wifi_mgmr_ap_info_t ap_info;
    /*Feature Bits*/
    uint32_t features;
    #define WIFI_MGMR_FEATURES_SCAN_SAVE_HIDDEN_SSID    (1 << 0)

    /*Manager config*/
    int scan_item_timeout;
    // the all channel nums for some country code
    int channel_nums;
    char country_code[3];
    uint8_t disable_autoreconnect;
    uint8_t ready;
    uint8_t max_sta_supported;
    uint8_t num_sta;
    uint32_t flags;
} wifi_mgmr_t;
extern wifi_mgmr_t wifiMgmr;

int wifi_mgmr_init(wifi_conf_t *conf);
int wifi_mgmr_get_channel_nums(const char *country_code);
char *wifi_mgmr_auth_to_str(uint8_t auth);
char *wifi_mgmr_cipher_to_str(uint8_t cipher);
int wifi_mgmr_scan_beacon_save( wifi_mgmr_scan_item_t *scan );
void wifi_mgmr_sta_info_reset();
void wifi_mgmr_sta_info_upatestatus(uint16_t status_code, uint16_t reason_code);
void wifi_mgmr_sta_info_save(void *param);
int wifi_mgmr_sta_connect_params_get();
#endif
