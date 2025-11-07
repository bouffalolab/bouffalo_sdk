#ifndef __WIFI_MGMR_EXT_H_
#define __WIFI_MGMR_EXT_H_

#include <stdint.h>
#include <stdbool.h>
#include "mac_types.h"

#if defined(BL616D)
#define MAX_FIXED_CHANNELS_LIMIT (42)
#else
#define MAX_FIXED_CHANNELS_LIMIT (14)
#endif

#define MAX_AP_SCAN     50
#define MGMR_SSID_LEN   32
#define MGMR_KEY_LEN    64
#define MGMR_BSSID_LEN  18
#define MGMR_AKM_LEN    15

/* WiFi async event */
#define  EV_WIFI                  0x0002
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
#define  CODE_WIFI_ON_LOST_IP           26
#define  CODE_WIFI_ON_LOST_IP6          27
#define  CODE_WIFI_ON_SCAN_DONE_CONNECTING  31

#define WIFI_EVENT_BEACON_IND_AUTH_OPEN            0
#define WIFI_EVENT_BEACON_IND_AUTH_WEP             1
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK         2
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK        3
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK    4
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT         5
#define WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE        6
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE 7
#define WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN      0xff

#define WIFI_EVENT_BEACON_IND_CIPHER_NONE           0
#define WIFI_EVENT_BEACON_IND_CIPHER_WEP            1
#define WIFI_EVENT_BEACON_IND_CIPHER_AES            2
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP           3
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES       4

/// Interface types
typedef enum
{
    /// STA interface
    MGMR_VIF_STA = 0,
    /// AP interface
    MGMR_VIF_AP
}wifi_mgmr_vif_type;

typedef enum
{
    ACCEPT_ACL,
    DENY_ACL,
    MAX_ACL_TYPE
} ap_acl_type;

typedef enum
{
    ADD_ACL,
    DELETE_ACL,
    SHOW_ACL,
    CLEAR_ACL,
    MAX_ACL_ACTION
} ap_action_type;

typedef enum
{
    DISABLE_ACL,
    ACCEPT_UNLESS_DENIED,
    DENY_UNLESS_ACCEPTED
} ap_acl_perm;

typedef struct wifi_mgmr_scan_item {
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
} wifi_mgmr_scan_item_t;

typedef struct wifi_mgmr_sniffer_item {
    /// interface index
    char *itf;
    /// Channel type (@ref mac_chan_bandwidth)
    uint8_t type;
    /// Frequency for Primary 20MHz channel (in MHz)
    uint16_t prim20_freq;
    /// Frequency center of the contiguous channel or center of Primary 80+80 (in MHz)
    uint16_t center1_freq;
    /// Frequency center of the non-contiguous secondary 80+80 (in MHz)
    uint16_t center2_freq;
    /// Frame received callback.
    void *cb;
    /// Parameter for the monitor callback
    void *cb_arg;
} wifi_mgmr_sniffer_item_t;

/// sta connect config params
/*
 * param:
 *  ssid     : SSID of target AP
 *  key      : Password of AP
 *  bssid    : BSSID of AP
 *  akm_str  : AKM of AP
 *  pmf_cfg  : PMF config
 *  freq1    : Frequency of AP
 *  freq2    : Frequency of AP (You can specify up to two frequencies on which AP will be scanned.)
 *  use_dhcp : Whether to use the dhcp server which provided by AP
 */
typedef struct wifi_mgmr_sta_connect_params {
    char ssid[MGMR_SSID_LEN];
    uint8_t ssid_tail[1];
    char key[MGMR_KEY_LEN];
    char key_tail[1];
    uint8_t ssid_len;
    uint8_t key_len;
    char bssid_str[MGMR_BSSID_LEN];
    // must be uppercase, NULL terminated string
    char akm_str[MGMR_AKM_LEN];
    uint8_t akm_len;
    uint16_t freq1;
    uint16_t freq2;
    uint8_t pmf_cfg;
    uint8_t use_dhcp;
    // listen_interval to monitor AP beacon
    // range:[1, 100]
    uint8_t listen_interval;
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

typedef struct wifi_mgmr_raw_send_params {
    // start addr of the packet to send
    void *pkt;
    // length of the packet to send
    uint32_t len;
    // channel of the packet will be sent at
    uint8_t channel;
} wifi_mgmr_raw_send_params_t;

typedef void (*cb_adhoc_tx_cfm)(void *, uint32_t);
typedef struct wifi_mgmr_adhoc_pkt_params {
    // Ethernet frame
    void *eth_frame;
    // length of the packet to send
    uint32_t len;
    // SSID
    char *ssid;
    // RA
    struct mac_addr *ra;
    // TA
    struct mac_addr *ta;
} wifi_mgmr_adhoc_pkt_params_t;

typedef struct wifi_mgmr_adhoc_start_params {
    // channel for ad-hoc mode
    uint8_t channel;
    // cb func for rx
    void *cb;
    void *cb_arg;
    // cb func for tx confirmtion
    void *cb_tx_cfm;
} wifi_mgmr_adhoc_start_params_t;

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

/**
 * Fully hosted frame information
 */
struct bl_frame_info
{
    /**
     * Interface index that received the frame. (-1 if unknown)
     */
    int fvif_idx;
    /**
     * Length (in bytes) of the frame.
     */
    uint16_t length;
    /**
     * Primary channel frequency (in MHz) on which the frame has been received.
     */
    uint16_t freq;
    /**
     * Received signal strength (in dBm)
     */
    int8_t rssi;
    /**
     * Received to ds
     */
    bool tods;
    /**
     * Received from ds
     */
    bool fromds;
    /**
     * Received rate_idx
     */
    int rate_idx;
    /**
     * Received address
     */
    struct mac_addr *ra;
    /**
     * Transmitting address
     */
    struct mac_addr *ta;
    /**
     *  Ethernet frame
     */
    uint8_t *eth_frame;
    /**
     *  Ethernet frame length
     */
    uint16_t eth_frame_length;
    /**
     * Frame payload. Can be NULL if monitor mode is started with @p uf parameter set to
     * true. In this case all other fields are still valid.
     */
    uint8_t *payload;
};

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
    /// bss mode
    uint8_t bss_mode;
} wifi_mgmr_connect_ind_stat_info_t;

typedef struct wifi_conf {
    char country_code[3];
    int channel_nums;
} wifi_conf_t;

typedef struct wifi_sta_basic_info {
    uint8_t sta_idx;
    uint8_t is_used;
    uint8_t sta_mac[6];
    uint16_t aid;
} wifi_sta_basic_info_t;

typedef struct rf_pwr_table {
    int8_t     pwr_11b[4];
    int8_t     pwr_11g[8];
    int8_t     pwr_11n_ht20[8];
    int8_t     pwr_11n_ht40[8];
    int8_t     pwr_11ac_vht20[10];
    int8_t     pwr_11ac_vht40[10];
    int8_t     reserved[10];
    int8_t     pwr_11ax_he20[12];
    int8_t     pwr_11ax_he40[12];
    int8_t     reserved2[12];
    int8_t     reserved3[12];
}rf_pwr_table_t;

typedef struct
{
    uint8_t ucSSID[32];         /**< SSID of the Wi-Fi network. */
    uint8_t ucSSIDLength;       /**< SSID length. */
    uint8_t ucBSSID[6];         /**< BSSID of the Wi-Fi network. */
    uint8_t ucSecurity;         /**< Wi-Fi Security. */
    uint8_t ucPWD[65];          /**< WPA/WPA2 passphrase. */
    uint8_t ucChannel;          /**< Channel number. */
} wifi_mgmr_ap_info_t;

/**
 * @brief Struct for TWT setup parameters
 *
 * This structure is used to store the parameters needed for setting up TWT.
 */
typedef struct
{
    uint8_t setup_type;          ///< TWT Setup command (0: Request, 1: Suggest, 2: Demand, 3: Grouping, 4: Accept, 5: Alternate, 6: Dictate, 7: Reject)
    uint8_t flow_type;           ///< Flow Type (0: Announced, 1: Unannounced)
    uint8_t wake_int_exp;        ///< Wake interval Exponent
    bool wake_dur_unit;          ///< Unit of measurement of TWT Minimum Wake Duration (false: 256us, true: tu)
    uint8_t min_twt_wake_dur;    ///< Nominal Minimum TWT Wake Duration
    uint16_t wake_int_mantissa;  ///< TWT Wake Interval Mantissa
} twt_setup_params_struct_t;

/**
 * @brief Struct for TWT teardown parameters
 *
 * This structure is used to store the parameters needed for tearing down TWT.
 */
typedef struct
{
    uint8_t neg_type;  ///< TWT negotiation type
    uint8_t all_twt;   ///< Flag indicating whether to teardown all TWT flows
    uint8_t id;        ///< TWT flow ID
} twt_teardown_params_struct_t;

typedef void (*scan_item_cb_t)(void *env, void *arg, wifi_mgmr_scan_item_t *item);

/**
 * wifi_sta_antenna_connect
 * Connect to the AP
 * param:
 *  ssid     : SSID of target AP
 *  key      : Password of AP
 *  bssid    : BSSID of AP
 *  akm_str  : AKM of AP, must be all in upper case
 *  pmf_cfg  : PMF config
 *  freq1    : Frequency of AP
 *  freq2    : Frequency of AP (You can specify up to two frequencies on which AP will be scanned.)
 *  use_dhcp : Whether to use the dhcp server which provided by AP
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_sta_antenna_connect(const char *ssid, const char *key, const char *bssid, const char *akm_str, uint8_t pmf_cfg, uint16_t freq1, uint16_t freq2, uint8_t use_dhcp);

/**
 * wifi_sta_connect
 * Connect to the AP
 * param:
 *  ssid     : SSID of target AP
 *  key      : Password of AP
 *  bssid    : BSSID of AP
 *  akm_str  : AKM of AP, must be all in upper case
 *  pmf_cfg  : PMF config
 *  freq1    : Frequency of AP
 *  freq2    : Frequency of AP (You can specify up to two frequencies on which AP will be scanned.)
 *  use_dhcp : Whether to use the dhcp server which provided by AP
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_sta_connect(const char *ssid, const char *key, const char *bssid, const char *akm_str, uint8_t pmf_cfg, uint16_t freq1, uint16_t freq2, uint8_t use_dhcp);
/**
 * wifi_mgmr_sta_quickconnect
 * Connect to the AP
 * param:
 *  ssid     : SSID of target AP
 *  key      : Password of AP
 *  freq1    : Frequency of AP
 *  freq2    : Frequency of AP (You can specify up to two frequencies on which AP will be scanned.)
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_quickconnect(const char *ssid, const char *key, uint16_t freq1, uint16_t freq2);


/**
 * wifi_mgmr_sta_connect
 * Connect to the AP
 * param:
 *  param1 : Configuration of connecting
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_connect(const wifi_mgmr_sta_connect_params_t *config);
/**
 * wifi_sta_disconnect
 * Disconnect from the AP
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_sta_disconnect(void);

/**
 * Set IPv4 address of STA interface
 *
 * return 0 on success or -1 on error
 */
int wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns);

/**
 * Get IPv4 address of STA interface
 *
 * return 0 on success or -1 on error
 */
int wifi_sta_ip4_addr_get(uint32_t *addr, uint32_t *mask, uint32_t *gw, uint32_t *dns);

#if MACSW_FHOST_MONITOR
/**
 * wifi_mgmr_sniffer_enable
 * start sniffer mode
 * param:
 *  param1 : Configuration of sniffer mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sniffer_enable(wifi_mgmr_sniffer_item_t sniffer_item);

/**
 * wifi_mgmr_sniffer_disable
 * stop sniffer mode
 * param:
 *  param1 : Handle of sniffer mode (frome wifi_mgmr_sniffer_enable())
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sniffer_disable(wifi_mgmr_sniffer_item_t sniffer_item);
#endif

/**
 * wifi_mgmr_state_get
 * get ap/sta state
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_state_get(void);

/**
 * wifi_mgmr_sta_rssi_get
 * get the last beacon rssi
 * param:
 *  param1 : ptr of rssi
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_rssi_get(int *rssi);

/**
 * wifi_mgmr_sta_channel_get
 * get sta mode channel
 * param:
 *  param1 : ptr of channel
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_channel_get(uint8_t *channel);

/**
 * wifi_mgmr_sta_ssid_set
 * set sta mode ssid
 * param:
 *  param1 : ptr of ssid
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_ssid_set(const char *ssid);

/**
 * wifi_mgmr_sta_ssid_get
 * get sta mode ssid
 * param:
 *  param1 : ptr of ssid
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_ssid_get(char *ssid);

/**
 * wifi_mgmr_sta_passphr_set
 * set sta mode password
 * param:
 *  param1 : ptr of password
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_passphr_set(const char *passphr);

/**
 * wifi_mgmr_sta_connect_ind_stat_get
 * get state (ssid password) of sta mode
 * param:
 *  param1 : ptr of struct wifi_mgmr_connect_ind_stat_info_t
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_connect_ind_stat_get(wifi_mgmr_connect_ind_stat_info_t *wifi_mgmr_ind_stat);

/**
 * wifi_mgmr_sta_scan
 * Scan available APs
 * param:
 *  param1 : Configuration of scanning
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_scan(const wifi_mgmr_scan_params_t *config);

/**
 * wifi_mgmr_ap_bcn_mode_set
 * Beacon Transmission Control Setting in the SAP modex
 *
 * Attention:
 *  MUST called before ap start
 * param:
 *  bcn_mode : Configuration of beacon transmissions mode
 *      0 : Start/Stop beacon transmissions automatically
 *              a.Beacon transmission is NOT started when SAP is started.
 *              b.Once a Probe Request frame having the same SSID is received, replies with a Probe Response frame, then Beacon transmission is started.
 *              c.Beacon transmission is stopped again if no STA is associated for more than bcn_timer (configurable) seconds.
 *      1 : Do not transmit beacon frames
 *              Not transmit beacon frames even while in an operational state.
 *      2 : Transmit beacon frames (Default)
 *              Transmit beacon frames while in an operational state.
 *  bcn_timer : Beacon Transmission Duration after all STAs are disconnected (unit: seconds)
 *
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_bcn_mode_set(uint8_t bcn_mode, int bcn_timer);

/**
 * wifi_mgmr_ap_bcn_mode_get
 * Get Beacon Transmission Control Setting in the SAP modex
 *
 * param:
 *  bcn_mode: refer to wifi_mgmr_ap_bcn_mode_set
 *  bcn_timer: refer to wifi_mgmr_ap_bcn_mode_set
 */
void wifi_mgmr_ap_bcn_mode_get(uint8_t *bcn_mode, int *bcn_timer);

/**
 * wifi_mgmr_sta_scanlist
 * List the scan results in last scan
 * return:
 *  0 : Success
 *  Others is Failed
 */
int wifi_mgmr_sta_scanlist(void);

/**
 * wifi_mgmr_sta_scanlist_nums_get
 * Get the number of scan results
 * return:
 *  Number of scan results
 */
uint32_t wifi_mgmr_sta_scanlist_nums_get(void);

/**
 * wifi_mgmr_sta_scanlist_dump
 * Get the scan results
 * param:
 *  param1 : addr for store scan results
 *  param2 : max number of items can be used for storing the scan results
 * return:
 *  Number of scan results
 */
uint32_t wifi_mgmr_sta_scanlist_dump(void * results, uint32_t resultNums);

/**
 * wifi_mgmr_scan_filter_hidden_ssid
 * Whether filter hidden ssid from scan results
 * param:
 *  param1 : Whether enable filter
 * return:
 *  0 : Success
 *  Others is Failed
 */
int wifi_mgmr_scan_filter_hidden_ssid(int filter);

/**
 * @brief Get scan results
 *
 * @param env env for cb
 * @param arg arg for cb
 * @param cb  Callback to receive scan results
 *
 * @return 0
 */
int wifi_mgmr_scan_ap_all(void *env, void *arg, scan_item_cb_t cb);

/**
 * wifi_mgmr_ap_start
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
 * wifi_mgmr_ap_stop
 * Stop AP mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_stop(void);

/**
 * wifi_mgmr_ap_acl_enable
 * MAC Address Filtering Enable && MAC Address Filtering Default permission setting
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_acl_enable(ap_acl_perm default_perm);

/**
 * wifi_mgmr_ap_acl_set
 * MAC Address White-list/Black-list setting
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_acl_set(ap_acl_type acl, ap_action_type action, char *mac_str);

/**
 * wifi_mgmr_mode_to_str
 * Get string of eht mode
 * param:
 *  param1 : Phy mode
 * return:
 *  "Unknown" : the mode can not be convert to string
 *  Others is the string of mode
 */
char *wifi_mgmr_mode_to_str(uint32_t mode);

/**
 * wifi_mgmr_mac_set
 * Set mac
 * param:
 *  param1 : Array of mac address
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_mac_set(uint8_t mac[6]);

/**
 * wifi_mgmr_sta_set_mac
 * Set mac addr for sta mode
 * param:
 *  param1 : Array of mac address
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 *
 * Attention:
 *  This interface will override the modification of the MAC address in STA mode done by wifi_mgmr_mac_set().
 *
 * Attention:
 *  This interface should be called before the initialization of the Wi-Fi module(wifi_start_firmware_task()).
 *
 * Attention:
 *  The AP and STA MAC addresses can only differ by one bit.
 * */
int wifi_mgmr_sta_set_mac(uint8_t mac[6]);

/**
 * wifi_mgmr_ap_set_mac
 * Set mac addr for ap mode
 * param:
 *  param1 : Array of mac address
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 *
 * Attention:
 *  This interface will override the modification of the MAC address in AP mode done by wifi_mgmr_mac_set().
 *
 * Attention:
 *  This interface should be called before the initialization of the Wi-Fi module(wifi_start_firmware_task()).
 *
 * Attention:
 *  The AP and STA MAC addresses can only differ by one bit.
 * */
int wifi_mgmr_ap_set_mac(uint8_t mac[6]);

/**
 * wifi_mgmr_sta_mac_get
 * Get sta mac
 * param:
 *  param1 : Array of mac address
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_mac_get(uint8_t mac[6]);

/**
 * wifi_mgmr_ap_mac_get
 * Get ap mac
 * param:
 *  param1 : Array of mac address
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_mac_get(uint8_t mac[6]);

/**
 * wifi_mgmr_set_country_code
 * Set country code
 * param:
 *  param1 : Pointer of country code, such as "CN","US","JP","EU"
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_set_country_code(char *country_code);

/**
 * wifi_mgmr_get_country_code
 * Get country code
 * param:
 *  country_code : Pointer to get country code, such as "CN","US","JP","EU"
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_get_country_code(char *country_code);

/**
 * wifi_mgmr_set_autoconnect_enable
 * Enable station autoconnect
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_autoconnect_enable(void);

/**
 * wifi_mgmr_set_autoconnect_disable
 * Disable station autoconnect
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_autoconnect_disable(void);

/**
 * wifi_mgmr_sta_wps_pbc
 * Start sta wps pbc
 * param:
 *  auth : Wi-Fi authentication mode floor. It will not connect to the AP whose authmode is lower than this auth floor,
 *  can use the following values:
 *  0: WIFI_EVENT_BEACON_IND_AUTH_OPEN
 *  1: WIFI_EVENT_BEACON_IND_AUTH_WEP
 *  2: WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK
 *  3: WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK
 *  4: WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_wps_pbc(uint8_t auth);

/**
 * wifi_mgmr_sta_non_pref_chan_set - Set the non-preferred channel list for Wi-Fi management
 * @argc: The number of arguments passed to the function
 * @argv: An array of strings representing the arguments, typically formatted as <oper_class>:<chan>:<preference>:<reason>
 *
 * This function processes the list of non-preferred channels provided as arguments, validating each entry's format.
 * If any entry is malformed, the entire list is discarded. The function is intended to help the Wi-Fi manager
 * adjust channel preferences according to user specifications, enhancing management capabilities for scenarios like MBO (Multi-Band Operation).
 *
 * Returns 0 on success or a negative error code on failure.
 */
int wifi_mgmr_sta_non_pref_chan_set(int argc, char **argv);

/**
 * wifi_mgmr_sta_non_pref_chan_notify - Notify the Wi-Fi management system of non-preferred channel changes
 *
 * This function checks if the Wi-Fi management system is ready and if the Wi-Fi Protected Access (WPA) state
 * is active. If the system is not ready or if WPA is stopped, the function exits early. Otherwise, if the
 * non-preferred channel list is valid, it sends a command to the WPA subsystem to set the non-preferred channels
 * based on the current list. The function helps synchronize the non-preferred channel configuration with the
 * Wi-Fi manager, ensuring proper operation in scenarios where channel preferences need to be adjusted, such as
 * in MBO (Multi-Band Operation).
 *
 * Returns 0 on success or a negative error code on failure.
 */
int wifi_mgmr_sta_non_pref_chan_notify(void);

/**
 * wifi_mgmr_sta_ps_enter
 * Enter powersave mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_ps_enter(void);

/**
 * wifi_mgmr_sta_ps_exit
 * Exit powersave mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_ps_exit(void);
/**
 * wifi_mgmr_sta_ps_active_time
 * set ps active time  
 * param:
 *      ms: time for active time ,unit: ms 
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */

int wifi_mgmr_sta_ps_active_time(uint32_t ms);

/**
 * @brief Set up WiFi Manager STA TWT (Target Wake Time) functionality
 *
 * This function is used to set up the WiFi Manager's STA TWT functionality
 * in order to achieve target wake time under specified conditions.
 *
 * @param twt_params_ptr Pointer to the structure containing all the necessary parameters for TWT setup
 * @return Returns the result of the function execution, 0 for success and -1 for failure
 */
int wifi_mgmr_sta_twt_setup(twt_setup_params_struct_t *twt_setup_params_ptr);

/**
 * @brief Tear down WiFi Manager STA TWT (Target Wake Time) functionality
 *
 * This function tears down the WiFi Manager's STA TWT functionality
 * based on the provided parameters.
 *
 * @param twt_params_ptr Pointer to the structure containing TWT teardown parameters
 * @return Returns the result of the function execution, 0 for success and -1 for failure
 */
int wifi_mgmr_sta_twt_teardown(twt_teardown_params_struct_t *twt_teardown_params_ptr);

/**
 * wifi_mgmr_sta_set_listen_itv
 * Set listen interval
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_set_listen_itv(uint8_t itv);

/**
 * wifi_mgmr_sta_get_listen_itv
 * Get listen interval
 * return: listen interval
 */
uint8_t wifi_mgmr_sta_get_listen_itv(void);

/**
 * wifi_mgmr_sta_aid_get
 * Get aid
 * return:aid
 */
int wifi_mgmr_sta_aid_get(void);

/**
 * wifi_mgmr_sta_get_bssid
 * Get STA bssid
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_get_bssid(uint8_t bssid[6]);

/**
 * wifi_mgmr_ap_sta_info_get
 * Get STA List in AP mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_sta_info_get(struct wifi_sta_basic_info *sta_info, uint8_t idx);

/**
 * wifi_mgmr_tpc_pwr_set
 * Set power table
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_tpc_pwr_set(rf_pwr_table_t *power_table);

/**
 * wifi_mgmr_tpc_pwr_get
 * Get power table
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_tpc_pwr_get(rf_pwr_table_t *power_table);

/**
 * wifi_mgmr_connection_info
 * Get connected ap/router info
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_connection_info(wifi_mgmr_connect_ind_stat_info_t * connection_info);

/**
 * wifi_mgmr_get_ap_info
 * Get ap info
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_get_ap_info(wifi_mgmr_ap_info_t *ap);

/**
 ****************************************************************************************
 * @brief Set keep alive time for sta mode
 *
 * @param[in] the keep alive time for sta mode(unit: seconds)
 ****************************************************************************************
 */
int wifi_mgmr_sta_keepalive_time_set(uint8_t time_seconds);

/**
 ****************************************************************************************
 * @brief get sta connected or not connected for sta mode
 * return:
 *  0 : not connected
 *  1 : connected
 ****************************************************************************************
 */
int wifi_mgmr_sta_state_get(void);

/**
 ****************************************************************************************
 * @brief get sta specific state
 * return: state
 ****************************************************************************************
 */
int wifi_mgmr_sta_state_get_ext(void);

/**
 * wifi_mgmr_sta_info_status_code_get
 * Get status code
 * return:
 *  status code
 */
uint16_t wifi_mgmr_sta_info_status_code_get();
/**
 ****************************************************************************************
 * @brief get whether ap has started for ap mode
 * return:
 *  0 : not started
 *  1 : has started
 ****************************************************************************************
 */
int wifi_mgmr_ap_state_get(void);

/**
 ****************************************************************************************
 * @brief get ap specific state
 * return: state
 ****************************************************************************************
 */
int wifi_mgmr_ap_state_get_ext(void);

/**
 * wifi_mgmr_conf_max_sta
 * Set max number of sta supported for ap
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_conf_max_sta(uint8_t max_sta_supported);

/**
 * wifi_mgmr_ap_sta_delete
 * Del sta in ap mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_sta_delete(uint8_t sta_idx);

/**
 * wifi_mgmr_raw_80211_send
 * Send raw packet
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_raw_80211_send(const wifi_mgmr_raw_send_params_t *config);

int wifi_mgmr_get_stats(struct ieee80211_stats *stats, uint8_t num_stats);

int wifi_mgmr_clear_stats(void);
#ifdef CFG_BL_WIFI_PS_ENABLE
/**
 * wifi_mgmr_null_data_send
 * Send null packet
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_null_data_send(void);
#endif

/**
 * wifi_mgmr_psk_cal
 * Calucate psk by using passpharse
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_psk_cal(char *password, const uint8_t *ssid, int ssid_len, char *output);

/**
 * wifi_mgmr_rate_config
 * param: fixed_rate_cfg
 * 11ax: MCS9-893, MCS8-881, MCS7-869, MCS6-857, MCS5-845, MCS4-833, MCS3-821, MCS2-809, MCS1-797, MCS0-785;
 * 11n: MCS7-44, MCS6-40, MCS5-36, MCS4-32, MCS3-28, MCS2-24, MCS1-20, MCS0-16;
 * 11g: 54M-15, 48M-14, 36M-13, 24M-12, 18M-11, 12M-10, 9M-9, 6M-8;
 * 11b: 11M-7, 5.5M-5, 2M-3, 1M-1
 *
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 *0xFFFF: disable fixed rate
 */
int wifi_mgmr_rate_config(uint16_t fixed_rate_cfg);

/**
 * wifi_mgmr_set_ht40_enable
 * Toggle state of HT40 mode
 * Attention: configuration takes effect only
 *            before the initiation of the WiFi task.
 * param:
 *  value : 1 = enable; 0 = disable
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_set_ht40_enable(uint8_t value);

/*
 * wifi_mgmr_sta_ap_tx_power_set
 * Set value of Tx power in unit of 0.5dBm for sta/ap mode
 * ref tx_power_limit_tables[2] for setting limit
*/
int wifi_mgmr_sta_ap_tx_power_set(tx_pwr_table_t *pwr_table);

/*
 * wifi_mgmr_sta_ap_tx_power_set
 * Get value of Tx power in unit of 0.5dBm for sta/ap mode
*/
int wifi_mgmr_sta_ap_tx_power_get(void);

/**
 * wifi_mgmr_sta_ap_retry_limit_set
 * Set tx retry limit for ap/sta mode
 */
int wifi_mgmr_sta_ap_retry_limit_set(uint8_t retry_limit);

/**
 * wifi_mgmr_sta_ap_retry_limit_set
 * Get tx retry limit for ap/sta mode
 */
uint32_t wifi_mgmr_sta_ap_retry_limit_get(void);

/**
 * wifi_mgmr_set_tx_queue_params
 * Set CWmin, CWmax, and AIFS for each Tx Queue
 */
int wifi_mgmr_set_tx_queue_params(int queue, int aifs, int cw_min,
                           int cw_max, int burst_time);
/**
 * wifi_mgmr_set_tx_queue_params
 * Get CWmin, CWmax, and AIFS for each Tx Queue
 */
int wifi_mgmr_get_tx_queue_params(uint8_t queue, uint8_t *aifsn, uint16_t *cwmin, uint16_t *cwmax);

/**
 * wifi_mgmr_get_remaining_tx
 * Get the number of frames remaining in each Tx queue.
 */
int wifi_mgmr_get_remaining_tx(uint8_t *tx0_cnt, uint8_t *tx1_cnt, uint8_t *tx2_cnt, uint8_t *tx3_cnt);

/**
 * wifi_mgmr_set_mode
 * param:
 * ap_or_sta: 1 = ap, 0 = sta
 * mode: @ref WiFi_Mode_t
 */
int wifi_mgmr_set_mode(uint8_t ap_or_sta, int mode);

/**
 * wifi_mgmr_get_mode
 * param:
 * ap_or_sta: 1 = ap, 0 = sta
 * return:
 * @ref WiFi_Mode_t
 * */
int wifi_mgmr_get_mode(uint8_t ap_or_sta);

/**
 * wifi_mgmr_adhoc_set_rate
 * Set tx rate
 * 1 Mbps: 7’d0
 * 2 Mbps: 7’d1
 * 5.5 Mbps: 7’d2
 * 11 Mbps: 7’d3
 * 6 Mbps: 7’d4
 * 9 Mbps: 7’d5
 * 12 Mbps: 7’d6
 * 18 Mbps: 7’d7
 * 24 Mbps: 7’d8
 * 36 Mbps: 7’d9
 * 48 Mbps: 7’d10
 * 54 Mbps: 7’d11
 * HT rates: 7’dMCS Index
 * VHT rates: {3’dnSS, 4’dMCS index}
 * HE rates: {3’dnSS, 4’dMCS index
 * Note that nSS is the number of spatial stream minus 1
 */
int8_t wifi_mgmr_adhoc_set_rate(uint8_t rate);
/**
 * wifi_mgmr_adhoc_get_rate
 * Get tx rate
 */
uint8_t wifi_mgmr_adhoc_get_rate();
/**
 * wifi_mgmr_adoc_set_rts_thrshold
 * Set/ RTS threshold
 */
int8_t wifi_mgmr_adhoc_set_rts_thrshold(uint8_t rts_thrshold);
/**
 * wifi_mgmr_adoc_get_rts_thrshold
 * Get RTS threshold
 */
uint8_t wifi_mgmr_adhoc_get_rts_thrshold();

/**
 * wifi_mgmr_adoc_set_tx_power
 * Set Tx power
 * 8’h80 : -128 dBm
 * 8’hFF : -1 dBm
 * 8’h00 : 0 dBm
 * 8’h01 : 1 dBm
 * 8’h3F : 127dBm
 */
int8_t wifi_mgmr_adhoc_set_tx_power(int8_t tx_power);

/**
 * wifi_mgmr_adoc_get_tx_power
 * Get Tx power
 */
int8_t wifi_mgmr_adhoc_get_tx_power();

/**
 * wifi_mgmr_adoc_set_retry_limit
 * Set retry limit
 */
int8_t wifi_mgmr_adhoc_set_retry_limit(uint8_t retry_limit);

/**
 * wifi_mgmr_adoc_get_retry_limit
 * Get retry limit
 */
uint8_t wifi_mgmr_adhoc_get_retry_limit();

/**
 * wifi_mgmr_adhoc_get_channel
 * Get channel of ad-hoc mode
 */
uint8_t wifi_mgmr_adhoc_get_channel();
/**
 * wifi_mgmr_adhoc_pkt_send
 * Send packets for testing
 */
int wifi_mgmr_adhoc_pkt_send(const wifi_mgmr_adhoc_pkt_params_t *config);
/**
 * wifi_mgmr_adhoc_start
 * Start adhoc mode
 */
int wifi_mgmr_adhoc_start(const wifi_mgmr_adhoc_start_params_t *config);
/**
 * wifi_mgmr_adhoc_stop
 * Stop adhoc mode
 */
int wifi_mgmr_adhoc_stop(void);

/**
 * wifi_mgmr_coex_enable
 * Enable or disable wifi coex
 */
void wifi_mgmr_coex_enable(bool en);

/**
 * wifi_mgmr_set_mode
 * Set sta/ap mode (e.g. b/g/n/ax)
 */
int wifi_mgmr_set_mode(uint8_t ap_or_sta, int mode);

/**
 * wifi_mgmr_get_mode
 * Get sta/ap mode (e.g. b/g/n/ax)
 */
int wifi_mgmr_get_mode(uint8_t ap_or_sta);

/**
*  @brief Get the number of valid 2.4GHz Wi-Fi channels
*  @details
*  This function returns the total number of available Wi-Fi channels in the 2.4GHz band,
*  which is determined by:
*  - Hardware capabilities
*  - Regional regulations (e.g., 11 in North America, 13 in China, 14 in Japan)
*
*  @return uint8_t
*    - Valid range: Typically 11-14 (11 for North America, 13 for China, 14 for Japan)
*    - Returns 0 if the operation fails (e.g., Wi-Fi not initialized or hardware unsupported)
*
*  @note
*  - Channel availability is automatically adapted to regional regulations by the RF module.
*  - Difference from `wifi_mgmr_get_current_channel()`:
*    - This returns the total count of available channels, not the currently connected channel.
*    - Example: Returns 13 in China (channels 1-13 valid) even when disconnected.
*
*  @example
*  \code{.c}
*  uint8_t channel_count = wifi_mgmr_get_channelnum_24G();
*  if (channel_count > 0) {
*      printf("Valid 2.4GHz channels: 1-%d\n", channel_count);
*  } else {
*      printf("2.4GHz band unavailable!\n");
*  }
*  \endcode
*/
uint8_t wifi_mgmr_get_channelnum_24G(void);

/*
*accordingheck if a WiFi channel is valid based on regulatory domain restrictions
*
* This function validates whether a given channel number is allowed for use
* in the current regulatory domain (country code). It checks:
* - 2.4GHz band channels (1-14 typically)
* - 5GHz band channels (conditionally for BL616D platform)
*
* @param channel The WiFi channel number to validate
* @return int
*   - 0: Channel is valid for current regulatory domain
*   - -1: Channel is invalid or not allowed
*
* @note For 5GHz channels (BL616D), country-specific validation is not yet implemented (TODO)
*/
int wifi_mgmr_channel_valid_check(uint16_t channel);
#endif
