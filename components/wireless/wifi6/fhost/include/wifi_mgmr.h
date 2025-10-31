#ifndef __WIFI_MGMR_H_
#define __WIFI_MGMR_H_

#include "macsw.h"
#include "wifi_mgmr_ext.h"

#include "rtos_al.h"

#define MAC_ADDR_LIST(m) (m)[0], (m)[1], (m)[2], (m)[3], (m)[4], (m)[5]

#define WIFI_MGMR_CONFIG_SCAN_ITEM_TIMEOUT    (15000)
#define WIFI_MGMR_SCAN_ITEMS_MAX (50)
typedef struct mode{
    char* mode_str;
    int mode;
}wifimode_t;

struct ieee80211_dot_d {
    char *code;
    uint8_t channel24G_num;
    uint8_t channel5G_num;
    uint8_t channel24G_chan[MAC_DOMAINCHANNEL_24G_MAX];
    uint8_t channel5G_chan[MAC_DOMAINCHANNEL_5G_MAX];
};

struct wlan_netif {
    int mode;//0: sta; 1: ap
    int started;
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
    rtos_mutex ap_sta_info_lock;
    wifi_mgmr_scan_item_t scan_items[WIFI_MGMR_SCAN_ITEMS_MAX];
    //the params info for sta connecting
    wifi_mgmr_sta_connect_params_t sta_connect_param;
    //router info for sta mode
    wifi_mgmr_connect_ind_stat_info_t wifi_mgmr_stat_info;
    //sta info for AP mode
    wifi_mgmr_sta_basic_info_t ap_sta_info[CFG_STA_MAX];
    //ap info for AP mode
    wifi_mgmr_ap_info_t ap_info;
    /*Feature Bits*/
    uint32_t features;
    #define WIFI_MGMR_FEATURES_SCAN_SAVE_HIDDEN_SSID    (1 << 0)

    /*Manager config*/
    int scan_item_timeout;
    // the all channel nums for some country code include 2.4G & 5G
    uint8_t channel24G_num;
    uint8_t *channel24G_list;
    uint8_t channel5G_num;
    uint8_t *channel5G_list;
    char country_code[3];
    uint8_t disable_autoreconnect;
    uint8_t ready;
    uint8_t max_sta_supported;
    uint8_t num_sta;
    uint32_t use_dhcpd;
    uint32_t flags;

    #define WIFI_MGMR_CONFIG_NON_PREF_CHAN_LIST     (4)
    /*non_pref_chan list, formatted with <oper_class>:<chan>:<preference>:<reason>*/
    char non_pref_chan_list[WIFI_MGMR_CONFIG_NON_PREF_CHAN_LIST][6*4+3+1];// support 4 non_pref_chan iem
    int non_pref_chan_list_valid;

    uint16_t sta_mode;
    uint16_t ap_mode;

    // Antenna scan
    bool is_antenna_scanning;

    /// For wps, Wi-Fi authentication mode floor. sta will not connect to the AP whose authmode is lower tha    n this floor.
    //0: OPEN (Default)
    //1: WEP
    //2: WPA_PSK
    //3: WPA2_PSK
    //4: WPA_WPA2_PSK
    uint8_t wps_auth;
} wifi_mgmr_t;
extern wifi_mgmr_t wifiMgmr;

int wifi_mgmr_init(wifi_conf_t *conf);
int wifi_mgmr_get_channel_nums(const char *country_code, uint8_t *c24G_cnt, uint8_t *c5G_cnt);
char *wifi_mgmr_auth_to_str(uint8_t auth);
char *wifi_mgmr_cipher_to_str(uint8_t cipher);
int wifi_mgmr_scan_beacon_save( wifi_mgmr_scan_item_t *scan );
void wifi_mgmr_sta_info_reset();
void wifi_mgmr_sta_info_upatestatus(uint16_t status_code, uint16_t reason_code);
void wifi_mgmr_sta_info_save(void *param);
int wifi_mgmr_sta_connect_params_get();
void wifi_mgmr_ap_sta_info_del(uint8_t sta_idx);
void wifi_mgmr_ap_sta_info_add(uint8_t sta_idx, uint16_t aid, struct mac_addr *addr);
#endif
