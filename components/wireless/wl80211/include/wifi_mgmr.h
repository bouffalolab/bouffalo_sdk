#ifndef _WIFI_MGMR_H_
#define _WIFI_MGMR_H_

#include <stdint.h>

/* WiFi async event */
#define  EV_WIFI                  ((uintptr_t)wifi_mgmr_init)

#define  CODE_WIFI_ON_INIT_DONE   1
#define  CODE_WIFI_ON_MGMR_DONE   2
#define  CODE_WIFI_CMD_RECONNECT  3
#define  CODE_WIFI_ON_CONNECTED   4
#define  CODE_WIFI_ON_DISCONNECT  5
#define  CODE_WIFI_ON_PRE_GOT_IP  6
#define  CODE_WIFI_ON_GOT_IP      7
#define  CODE_WIFI_ON_CONNECTING  8
#define  CODE_WIFI_ON_SCAN_DONE   9
#define  CODE_WIFI_ON_SCAN_DONE_ONJOIN  10
#define  CODE_WIFI_ON_AP_STARTED        11
#define  CODE_WIFI_ON_AP_STOPPED        12
#define  CODE_WIFI_ON_PROV_SSID         13
#define  CODE_WIFI_ON_PROV_BSSID        14
#define  CODE_WIFI_ON_PROV_PASSWD       15
#define  CODE_WIFI_ON_PROV_CONNECT      16
#define  CODE_WIFI_ON_PROV_DISCONNECT   17
#define  CODE_WIFI_ON_PROV_SCAN_START   18
#define  CODE_WIFI_ON_PROV_STATE_GET    19
#define  CODE_WIFI_ON_MGMR_DENOISE      20
#define  CODE_WIFI_ON_AP_STA_ADD        21
#define  CODE_WIFI_ON_AP_STA_DEL        22
#define  CODE_WIFI_ON_EMERGENCY_MAC     23
#define  CODE_WIFI_ON_EXIT_PS           24
#define  CODE_WIFI_ON_GOT_IP6           25
#define  CODE_WIFI_ON_SET_PS_DONE       26

#define MAX_FIXED_CHANNELS_LIMIT                     (42)

#define MAX_AP_SCAN                                  50
#define MGMR_SSID_LEN                                32
#define MGMR_KEY_LEN                                 64
#define MGMR_BSSID_LEN                               18
#define MGMR_AKM_LEN                                 15

#define WIFI_EVENT_BEACON_IND_AUTH_OPEN              0
#define WIFI_EVENT_BEACON_IND_AUTH_WEP               1
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK           2
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK          3
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK      4
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT           5
#define WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE          6
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE 7
#define WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN           0xff

#define WIFI_EVENT_BEACON_IND_CIPHER_NONE            0
#define WIFI_EVENT_BEACON_IND_CIPHER_WEP             1
#define WIFI_EVENT_BEACON_IND_CIPHER_AES             2
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP            3
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES        4

typedef struct mode {
    char *mode_str;
    int mode;
} wifimode_t;

struct reason_code {
    uint16_t reason_code;
    const char *action;
};

/// scan params
typedef struct wifi_mgmr_scan_params {
    uint8_t ssid_length;
    uint8_t ssid_array[MGMR_SSID_LEN];
    uint8_t bssid[6];
    uint8_t bssid_set_flag;
    uint8_t probe_cnt;
    int channels_cnt;
    uint8_t channels[MAX_FIXED_CHANNELS_LIMIT];
    uint32_t duration;
    /// 0: active scan; 1: Passive scan request
    bool passive;
} wifi_mgmr_scan_params_t;

typedef struct wifi_mgmr_scan_item {
    uint32_t mode;
    uint32_t timestamp_lastseen;
    int ssid_len;
    uint8_t channel;
    int8_t rssi;
    char ssid[32];
    char ssid_tail[1]; //always put ssid_tail after ssid
    uint8_t bssid[6];
    int8_t ppm_abs;
    int8_t ppm_rel;
    uint8_t auth;
    uint8_t cipher;
    uint8_t is_used;
    uint8_t wps;
    uint8_t best_antenna;
} wifi_mgmr_scan_item_t;

typedef struct wifi_mgmr_connect_ind_stat_info {
    uint16_t status_code;
    uint16_t reason_code;
    char ssid[33];
    char passphr[65];
    /// BSSID
    uint8_t bssid[6];
    uint8_t type_ind;
    uint8_t chan_band;
    uint8_t channel;
    uint8_t security;
    /// Association Id allocated by the AP for this connection
    uint16_t aid;
    /// Index of the VIF for which the association process is complete
    uint8_t vif_idx;
    /// Index of the STA entry allocated for the AP
    uint8_t ap_idx;
    /// Index of the LMAC channel context the connection is attached to
    uint8_t ch_idx;
    /// Flag indicating if the AP is supporting QoS
    bool qos;
} wifi_mgmr_connect_ind_stat_info_t;

typedef void (*scan_item_cb_t)(void *env, void *arg, wifi_mgmr_scan_item_t *item);

void wifi_mgmr_init(void);
void wifi_mgmr_sta_ps_enter(void);
void wifi_mgmr_sta_ps_exit(void);

int wifi_mgmr_sta_get_bssid(uint8_t bssid[6]);
int wifi_mgmr_sta_mac_get(uint8_t mac[6]);
int wifi_mgmr_sta_rssi_get(int *rssi);
int wifi_mgmr_sta_channel_get(uint8_t *channel);
int wifi_mgmr_sta_aid_get(void);
int wifi_mgmr_sta_state_get(void);
int wifi_mgmr_sta_connect_ind_stat_get(wifi_mgmr_connect_ind_stat_info_t *wifi_mgmr_ind_stat);
int wifi_mgmr_sta_scan(const wifi_mgmr_scan_params_t *config);
uint32_t wifi_mgmr_sta_scanlist_nums_get(void);
uint32_t wifi_mgmr_sta_scanlist_dump(void *results, uint32_t resultNums);
int wifi_mgmr_sta_scanlist_free(void);
int wifi_mgmr_scan_ap_all(void *env, void *arg, scan_item_cb_t cb);
uint16_t wifi_mgmr_sta_info_status_code_get(void);
uint16_t wifi_mgmr_sta_info_reason_code_get(void);
int wifi_mgmr_get_channel_nums(const char *country_code, uint8_t *c24G_cnt, uint8_t *c5G_cnt);
int wifi_mgmr_get_channel_list(const char *country_code, uint8_t **c24G_list, uint8_t **c5G_list);
int wifi_mgmr_set_country_code(char *country_code);
int wifi_mgmr_get_country_code(char *country_code);

int wifi_mgmr_ap_state_get(void);
int wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns);
int wifi_mgmr_ap_mac_get(uint8_t mac[6]);
int wifi_mgmr_sta_scanlist(void);
int wifi_mgmr_sta_autoconnect_disable(void);
char *wifi_mgmr_mode_to_str(uint32_t mode);
char *wifi_mgmr_auth_to_str(uint8_t auth);
char *wifi_mgmr_cipher_to_str(uint8_t cipher);
const char *wifi_mgmr_get_sm_status_code_str(uint16_t status_code);
#endif
