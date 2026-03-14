#ifndef _WIFI_MGMR_H_
#define _WIFI_MGMR_H_

#include <stdint.h>
#include <stdbool.h>

/* WiFi async event */
#define EV_WIFI                                      ((uintptr_t)wifi_mgmr_init)

#define CODE_WIFI_ON_INIT_DONE                       1
#define CODE_WIFI_ON_MGMR_DONE                       2
#define CODE_WIFI_CMD_RECONNECT                      3
#define CODE_WIFI_ON_CONNECTED                       4
#define CODE_WIFI_ON_DISCONNECT                      5
#define CODE_WIFI_ON_PRE_GOT_IP                      6
#define CODE_WIFI_ON_GOT_IP                          7
#define CODE_WIFI_ON_CONNECTING                      8
#define CODE_WIFI_ON_SCAN_DONE                       9
#define CODE_WIFI_ON_SCAN_DONE_ONJOIN                10
#define CODE_WIFI_ON_AP_STARTED                      11
#define CODE_WIFI_ON_AP_STOPPED                      12
#define CODE_WIFI_ON_PROV_SSID                       13
#define CODE_WIFI_ON_PROV_BSSID                      14
#define CODE_WIFI_ON_PROV_PASSWD                     15
#define CODE_WIFI_ON_PROV_CONNECT                    16
#define CODE_WIFI_ON_PROV_DISCONNECT                 17
#define CODE_WIFI_ON_PROV_SCAN_START                 18
#define CODE_WIFI_ON_PROV_STATE_GET                  19
#define CODE_WIFI_ON_MGMR_DENOISE                    20
#define CODE_WIFI_ON_AP_STA_ADD                      21
#define CODE_WIFI_ON_AP_STA_DEL                      22
#define CODE_WIFI_ON_EMERGENCY_MAC                   23
#define CODE_WIFI_ON_EXIT_PS                         24
#define CODE_WIFI_ON_GOT_IP6                         25
#define CODE_WIFI_ON_SET_PS_DONE                     26

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
    char ssid[MGMR_SSID_LEN + 1];
    char passphr[MGMR_KEY_LEN + 1];
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

typedef struct wifi_mgmr_sta_connect_params {
    char ssid[MGMR_SSID_LEN + 1];
    // uint8_t ssid_tail[1];
    char key[MGMR_KEY_LEN + 1];
    // char key_tail[1];
    // uint8_t ssid_len;
    // uint8_t key_len;
    char bssid_str[MGMR_BSSID_LEN + 1];
    // must be uppercase, NULL terminated string
    char akm_str[MGMR_AKM_LEN + 1];
    // uint8_t akm_len;
    uint16_t freq1;
    uint16_t freq2;
    uint8_t pmf_cfg;
    uint8_t use_dhcp;
    // listen_interval to monitor AP beacon
    // range:[1, 100]
    uint16_t listen_interval;
    // default: 0, scan on all channels
    // if 1, quick scan, connect to the first ssid_matched AP
    uint8_t scan_mode;
    // default: normal connect
    // if 1, quick connect
    uint8_t quick_connect;
    int timeout_ms;
    // Extra flags passed to wpa_supplicant. See FHOST_WPA_SSID_* in fhost_wpa.h.
    int wpa_flags;
    // conn scan duration, In TUs.
    uint16_t duration;
    // conn scan probe req cnt
    uint16_t probe_cnt;
    // Auth and Assoc timeout, in sec. 0, use default value
    uint8_t auth_timeout;
    // Timeout before EAPOL 1 after associtiated, in sec. 0, use default value
    uint8_t eapol_1_timeout;
    // Remaining EAPOL session timeout, in sec. 0, use default value
    uint8_t eapol_rem_timeout;
} wifi_mgmr_sta_connect_params_t;

/// ap start params
typedef struct wifi_mgmr_ap_params {
    /// must be setted
    char *ssid;
    /// if NULL and the akm is not NULL, the default value "12345678" will be setted
    char *key;
    /// OPEN/WPA/WPA2 can be supported now, if NULL and the key is not NULL, the default value "WPA2" will be setted
    char *akm;
    /// if zero, the default value 6 will be setted
    uint8_t channel;
    /// Channel type (@ref mac_chan_bandwidth)
    uint8_t type;
    /// use_ipcfg;
    bool use_ipcfg;
    /// Whether use dhcpd
    bool use_dhcpd;
    /// dhcpd pool start
    int start;
    /// dhcpd pool limit
    int limit;
    /// ap ip addr
    uint32_t ap_ipaddr;
    /// ap subnet mask
    uint32_t ap_mask;
    /// STA MAX inactivity under connection
    uint32_t ap_max_inactivity;
    /// whether use hidden ssid
    bool hidden_ssid;
    /// whether enable isolation
    bool isolation;
    /// Beacon interval in TU
    int bcn_interval;
    /// Additional vendor specific elements for Beacon and Probe Response frames
    /// a hexdump of the raw information elements (id+len+payload for one or more elements),
    //  the maximum length supported is MAX_AP_VENDOR_ELEMENTS_LEN,
    /// ref wap_supplicant.conf
    char *ap_vendor_elements;
    /// bcn_mode:
    /// 0   Start/Stop beacon transmissions automatically
    ///         a.Beacon transmission is NOT started when SAP is started.
    ///         b.Once a Probe Request frame having the same SSID is received, replies with a Probe Response frame, then Beacon transmission is started.
    ///         c.Beacon transmission is stopped again if no STA is associated for more than bcn_timer seconds.
    /// 1   Do not transmit beacon frames
    /// 2   Transmit beacon frames (Default)
    uint8_t bcn_mode;
    /// Beacon transmission is stopped again if no STA is associated for more than bcn_timer seconds
    int bcn_timer;
    /// Disable advertising WME/WMM Information Element in Beacon/ProbeResponse frames
    bool disable_wmm;
} wifi_mgmr_ap_params_t;

typedef void (*scan_item_cb_t)(void *env, void *arg, wifi_mgmr_scan_item_t *item);

/* Initialize WiFi manager and register event handlers */
void wifi_mgmr_init(void);
/* Enable power saving mode for STA */
void wifi_mgmr_sta_ps_enter(void);
/* Disable power saving mode for STA */
void wifi_mgmr_sta_ps_exit(void);

/* STA Status and Information  */
/* Get connected AP's BSSID (MAC address) */
int wifi_mgmr_sta_get_bssid(uint8_t bssid[6]);
/* Get STA MAC address */
int wifi_mgmr_sta_mac_get(uint8_t mac[6]);
/* Get current RSSI signal strength in dBm */
int wifi_mgmr_sta_rssi_get(int *rssi);
/* Get current WiFi channel number */
int wifi_mgmr_sta_channel_get(uint8_t *channel);
/* Get Association ID allocated by AP */
int wifi_mgmr_sta_aid_get(void);
/* Get STA connection state (1=connected, 0=disconnected) */
int wifi_mgmr_sta_state_get(void);
/* Get detailed connection status and parameters */
int wifi_mgmr_sta_connect_ind_stat_get(wifi_mgmr_connect_ind_stat_info_t *wifi_mgmr_ind_stat);
/* Get WiFi connection status code */
uint16_t wifi_mgmr_sta_info_status_code_get(void);
/* Get WiFi disconnection reason code */
uint16_t wifi_mgmr_sta_info_reason_code_get(void);

/* STA Scan Operations */
/* Start WiFi scan with specified parameters */
int wifi_mgmr_sta_scan(const wifi_mgmr_scan_params_t *config);
/* Get number of scan results */
uint32_t wifi_mgmr_sta_scanlist_nums_get(void);
/* Copy scan results to buffer */
uint32_t wifi_mgmr_sta_scanlist_dump(void *results, uint32_t resultNums);
/* Free scan results memory */
int wifi_mgmr_sta_scanlist_free(void);
/* Iterate scan results with callback function */
int wifi_mgmr_scan_ap_all(void *env, void *arg, scan_item_cb_t cb);
/* Set the listening interval during scanning (TU)  */
int wifi_mgmr_sta_set_listen_itv(uint16_t itv);
/* Get the listening interval during the scan (TU) */
uint16_t wifi_mgmr_sta_get_listen_itv(void);

/* STA Management Operations */
/* Connect to WiFi AP with specified parameters */
int wifi_mgmr_sta_connect(const wifi_mgmr_sta_connect_params_t *conn_param);
/* Disconnect from current WiFi AP */
int wifi_mgmr_sta_disconnect(void);
/* Get channel count for country code */
int wifi_mgmr_get_channel_nums(const char *country_code, uint8_t *c24G_cnt, uint8_t *c5G_cnt);
/* Get channel list for country code */
int wifi_mgmr_get_channel_list(const char *country_code, uint8_t **c24G_list, uint8_t **c5G_list);
/* Set regulatory country code */
int wifi_mgmr_set_country_code(char *country_code);
/* Get current country code */
int wifi_mgmr_get_country_code(char *country_code);
/* Set static IP configuration (not implemented, returns -1) */
int wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns);
/* Print scan list to console (not implemented, returns -1) */
int wifi_mgmr_sta_scanlist(void);
/* Enable automatic reconnection (stub for compatibility) */
int wifi_mgmr_sta_autoconnect_enable(void);
/* Disable automatic reconnection (stub for compatibility) */
int wifi_mgmr_sta_autoconnect_disable(void);

/* AP Mode Management */
/* Get AP state (not implemented, returns -1) */
int wifi_mgmr_ap_state_get(void);
/* Get AP MAC address (not implemented, returns -1) */
int wifi_mgmr_ap_mac_get(uint8_t mac[6]);
/**
 * Start AP mode
 * param:
 *  param1 : Configuration of AP mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_start(const wifi_mgmr_ap_params_t *config);
/**
 * Stop AP mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_stop(void);

/* String Conversion Utilities */
/* Convert WiFi mode to string (e.g., "BGN", "BGNAX") */
char *wifi_mgmr_mode_to_str(uint32_t mode);
/* Convert auth type to string (e.g., "WPA2-PSK", "WPA3-SAE") */
char *wifi_mgmr_auth_to_str(uint8_t auth);
/* Convert cipher type to string (e.g., "AES", "TKIP/AES") */
char *wifi_mgmr_cipher_to_str(uint8_t cipher);
/* Convert status code to error description string */
const char *wifi_mgmr_get_sm_status_code_str(uint16_t status_code);
/* Convert or check MAC address string to byte array */
int wifi_mgmr_mac_str_to_addr(const char *str, uint8_t addr[]);

#endif
