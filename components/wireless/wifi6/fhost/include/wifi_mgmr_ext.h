#ifndef __WIFI_MGMR_EXT_H_
#define __WIFI_MGMR_EXT_H_

#include <stdint.h>
#include <stdbool.h>
#include "mac_types.h"
#include "macsw.h"

#if defined(BL618DG)
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
#define  CODE_WIFI_ON_AP_CSA_DONE       13
#define  CODE_WIFI_ON_STA_CSA_DONE      14
#define  CODE_WIFI_ON_MGMR_DENOISE      20
#define  CODE_WIFI_ON_AP_STA_ADD        21
#define  CODE_WIFI_ON_AP_STA_DEL        22
#define  CODE_WIFI_ON_EMERGENCY_MAC     23
#define  CODE_WIFI_ON_EXIT_PS           24
#define  CODE_WIFI_ON_GOT_IP6           25
#define  CODE_WIFI_ON_LOST_IP           26
#define  CODE_WIFI_ON_LOST_IP6          27
#define  CODE_WIFI_ON_GOT_IP_TIMEOUT    28
#define  CODE_WIFI_ON_GOT_IP_ABORT      29
#define  CODE_WIFI_ON_SCAN_DONE_CONNECTING  31
#define  CODE_WIFI_ON_PARAMS_ERROR      32
#define  CODE_WIFI_ON_AP_STA_AUTH_FAIL  33
#ifdef CONFIG_WIFI_P2P
#define  CODE_WIFI_ON_P2P_DEVICE_FOUND             40
#define  CODE_WIFI_ON_P2P_DEVICE_LOST              41
#define  CODE_WIFI_ON_P2P_FIND_STOPPED             42
#define  CODE_WIFI_ON_P2P_GO_NEG_REQUEST           43
#define  CODE_WIFI_ON_P2P_GO_NEG_FAILURE           44
#define  CODE_WIFI_ON_P2P_GO_NEG_SUCCESS           45
#define  CODE_WIFI_ON_P2P_GROUP_FORMATION_FAILURE  46
#define  CODE_WIFI_ON_P2P_GROUP_STARTED            47
#define  CODE_WIFI_ON_P2P_GROUP_REMOVED            48
#define  CODE_WIFI_ON_P2P_INVITATION_RECEIVED      49
#define  CODE_WIFI_ON_P2P_INVITATION_RESULT        50
#define  CODE_WIFI_ON_P2P_GROUP_FORMATION_SUCCESS  51
#define  CODE_WIFI_ON_P2P_PROV_DISC_SHOW_PIN       52
#define  CODE_WIFI_ON_P2P_PROV_DISC_ENTER_PIN      53
#define  CODE_WIFI_ON_P2P_PROV_DISC_PBC_REQ        54
#define  CODE_WIFI_ON_P2P_PROV_DISC_PBC_RESP       55
#define  CODE_WIFI_ON_P2P_PROV_DISC_FAILURE        56
#define  CODE_WIFI_ON_P2P_INVITATION_ACCEPTED      57
#define  CODE_WIFI_ON_P2P_GOT_IP                   58
#endif

#define WIFI_MGMR_STA_RECONNECT_DEFAULT_AUTH_FAIL_THRESHOLD 10
#define WIFI_MGMR_STA_RECONNECT_DEFAULT_FIXED_INTERVAL_SEC  15
#define WIFI_MGMR_STA_RECONNECT_MIN_INTERVAL_SEC            1
#define WIFI_MGMR_STA_RECONNECT_MAX_INTERVAL_SEC            300
#define WIFI_MGMR_STA_RECONNECT_MAX_AUTH_FAIL_THRESHOLD     200

typedef enum
{
    WIFI_MGMR_STA_RECONNECT_MODE_EXP_BACKOFF = 0,
    WIFI_MGMR_STA_RECONNECT_MODE_FIXED_AFTER_THRESHOLD = 1,
} wifi_mgmr_sta_reconnect_mode_t;

typedef struct wifi_mgmr_sta_reconnect_policy
{
    wifi_mgmr_sta_reconnect_mode_t mode;  /* Reconnect mode, see wifi_mgmr_sta_reconnect_mode_t. */
    uint16_t auth_fail_threshold;         /* Apply fixed interval when auth_failures > threshold. */
    uint16_t fixed_interval_sec;          /* Fixed reconnect interval in seconds. */
} wifi_mgmr_sta_reconnect_policy_t;

#ifdef CONFIG_WIFI_P2P
#define WIFI_MGMR_P2P_MAX_PEERS         8
#define WIFI_MGMR_P2P_ADDR_LEN          6
#define WIFI_MGMR_P2P_ADDR_STR_LEN      18
#define WIFI_MGMR_P2P_SSID_STR_LEN      (MGMR_SSID_LEN + 1)
#define WIFI_MGMR_P2P_MODE_STR_LEN      32
#define WIFI_MGMR_P2P_STATE_STR_LEN     32
#define WIFI_MGMR_P2P_NAME_STR_LEN      64
#define WIFI_MGMR_P2P_DEVICE_NAME_MAX_LEN 32
#define WIFI_MGMR_P2P_DEV_TYPE_STR_LEN  32
#define WIFI_MGMR_P2P_PEER_RAW_LEN      1024
#define WIFI_MGMR_P2P_PEER_INFO_LEN     WIFI_MGMR_P2P_PEER_RAW_LEN
#define WIFI_MGMR_P2P_FHOST_VIF_IDX     1
#define WIFI_MGMR_P2P_DEFAULT_IFACE     "wl2"
#define WIFI_MGMR_P2P_EVENT_RAW_LEN     256
#define WIFI_MGMR_P2P_EVENT_SSID_STR_LEN (MGMR_SSID_LEN * 4 + 1)
#define WIFI_MGMR_P2P_EVENT_IFNAME_LEN  16
#define WIFI_MGMR_P2P_EVENT_ROLE_LEN    8
#define WIFI_MGMR_P2P_EVENT_PIN_LEN     9
#define WIFI_MGMR_P2P_EVENT_REASON_LEN  32
#define WIFI_MGMR_P2P_EVENT_METHOD_LEN  16
#define WIFI_MGMR_P2P_PASSPHRASE_MIN_LEN 8
#define WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN 63
#define WIFI_MGMR_P2P_MAX_PERSISTENT_GROUPS 8 /* keep equal to P2P_GROUP_STORE_MAX_GROUPS */
#define WIFI_MGMR_P2P_PERSISTENT_SSID_STR_LEN (MGMR_SSID_LEN * 4 + 1)

typedef enum
{
    WIFI_MGMR_P2P_METHOD_PBC = 0,
    WIFI_MGMR_P2P_METHOD_PIN = 1,
    WIFI_MGMR_P2P_METHOD_PIN_DISPLAY = WIFI_MGMR_P2P_METHOD_PIN,
    WIFI_MGMR_P2P_METHOD_PIN_KEYPAD = 2,
} wifi_mgmr_p2p_method_t;

/** WPS Device Password ID values carried by a GO Negotiation Request. */
typedef enum
{
    WIFI_MGMR_P2P_DEV_PW_DEFAULT = 0x0000,
    WIFI_MGMR_P2P_DEV_PW_USER_SPECIFIED = 0x0001,
    WIFI_MGMR_P2P_DEV_PW_PUSHBUTTON = 0x0004,
    WIFI_MGMR_P2P_DEV_PW_REGISTRAR_SPECIFIED = 0x0005,
} wifi_mgmr_p2p_dev_password_id_t;

typedef enum
{
    WIFI_MGMR_P2P_ROLE_UNKNOWN = 0,
    WIFI_MGMR_P2P_ROLE_GO,
    WIFI_MGMR_P2P_ROLE_CLIENT,
} wifi_mgmr_p2p_role_t;

/** Stable return values for the fixed-interface P2P API. */
typedef enum
{
    WIFI_MGMR_P2P_OK = 0,
    WIFI_MGMR_P2P_ERR_INVALID_ARG = -1,
    WIFI_MGMR_P2P_ERR_NOT_INITIALIZED = -2,
    WIFI_MGMR_P2P_ERR_VIF_BUSY = -3,
    WIFI_MGMR_P2P_ERR_INVALID_STATE = -4,
    WIFI_MGMR_P2P_ERR_NOT_SUPPORTED = -5,
    WIFI_MGMR_P2P_ERR_NO_MEM = -6,
    WIFI_MGMR_P2P_ERR_TRANSPORT = -7,
    WIFI_MGMR_P2P_ERR_SUPPLICANT = -8,
    WIFI_MGMR_P2P_ERR_TIMEOUT = -9,
} wifi_mgmr_p2p_error_t;

/** Fixed-interface P2P lifecycle state. */
typedef enum
{
    WIFI_MGMR_P2P_STATE_STOPPED = 0,
    WIFI_MGMR_P2P_STATE_IDLE,
    WIFI_MGMR_P2P_STATE_FINDING,
    WIFI_MGMR_P2P_STATE_LISTENING,
    WIFI_MGMR_P2P_STATE_NEGOTIATING,
    WIFI_MGMR_P2P_STATE_GROUP_ACTIVE,
    WIFI_MGMR_P2P_STATE_STOPPING,
} wifi_mgmr_p2p_state_t;

typedef struct wifi_mgmr_p2p_find_params
{
    uint16_t timeout;                     /* 0 to omit, otherwise timeout in seconds. */
    uint16_t freq;                        /* 0 to omit, otherwise discovery frequency in MHz. */
    uint8_t dev_id[WIFI_MGMR_P2P_ADDR_LEN]; /* Target P2P Device Address. */
    uint8_t dev_id_valid;                 /* 1 to include dev_id, otherwise 0. */
} wifi_mgmr_p2p_find_params_t;

typedef struct wifi_mgmr_p2p_connect_params
{
    uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN]; /* Peer P2P device address. */
    wifi_mgmr_p2p_method_t method;         /* WPS method. */
    uint8_t join;                          /* Join an existing GO; mutually exclusive with auto_join and auth. */
    uint8_t auto_join;                     /* Join a GO or negotiate automatically; mutually exclusive with join and auth. */
    uint8_t auth;                          /* Authorize a later request without changing lifecycle; mutually exclusive with join and auto_join. */
    int8_t go_intent;                      /* -1 to omit, otherwise 0..15. */
    uint16_t freq;                         /* 0 to omit, otherwise operating frequency in MHz. */
    const char *pin;                       /* PBC: NULL. Display: NULL for auto PIN or 4/8 digits. Keypad: required 4/8 digits. */
} wifi_mgmr_p2p_connect_params_t;

typedef struct wifi_mgmr_p2p_connect_result
{
    uint8_t generated_pin_valid;           /* 1 when supplicant returned an auto-generated display PIN. */
    char generated_pin[WIFI_MGMR_P2P_EVENT_PIN_LEN]; /* Eight digits plus NUL. */
} wifi_mgmr_p2p_connect_result_t;

typedef struct wifi_mgmr_p2p_group_add_params
{
    uint8_t persistent;                    /* 1 to create persistent GO; mutually exclusive with persistent_id_valid. */
    uint8_t persistent_id_valid;           /* 1 to reinvoke by id; mutually exclusive with persistent. */
    int persistent_id;                     /* Persistent network id. Valid when persistent_id_valid is 1. */
    uint16_t freq;                         /* 0 to omit, otherwise operating frequency in MHz. */
    uint16_t max_oper_chwidth;              /* 0 to omit, otherwise 20, 40, or 80 MHz. */
    uint8_t ht40;                          /* 1 to request HT40. */
    uint8_t vht;                           /* 1 to request VHT. */
    uint8_t he;                            /* 1 to request HE. */
    const char *passphrase;                /* NULL for random, otherwise 8..63 bytes without control characters. */
} wifi_mgmr_p2p_group_add_params_t;

typedef struct wifi_mgmr_p2p_persistent_group
{
    int id;                                 /* Network ID used by persistent=<id>. */
    uint8_t go_dev_addr[WIFI_MGMR_P2P_ADDR_LEN]; /* Stored GO P2P Device Address. */
    char ssid[WIFI_MGMR_P2P_PERSISTENT_SSID_STR_LEN]; /* Supplicant printable SSID. */
} wifi_mgmr_p2p_persistent_group_t;

typedef struct wifi_mgmr_p2p_persistent_groups
{
    uint8_t group_count;                    /* Number of entries stored in group. */
    uint8_t group_count_total;              /* Total persistent groups reported. */
    wifi_mgmr_p2p_persistent_group_t
        group[WIFI_MGMR_P2P_MAX_PERSISTENT_GROUPS];
} wifi_mgmr_p2p_persistent_groups_t;

typedef struct wifi_mgmr_p2p_prov_disc_params
{
    uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN]; /* Peer P2P device address. */
    wifi_mgmr_p2p_method_t method;         /* Provisioning config method. */
    uint8_t join;                          /* Join an existing GO; mutually exclusive with auto_join. */
    uint8_t auto_join;                     /* Find peer first; mutually exclusive with join. */
} wifi_mgmr_p2p_prov_disc_params_t;

typedef struct wifi_mgmr_p2p_invite_params
{
    uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN]; /* Valid when peer_addr_valid is 1. */
    uint8_t go_dev_addr[WIFI_MGMR_P2P_ADDR_LEN]; /* Valid when go_dev_addr_valid is 1. */
    uint8_t peer_addr_valid;               /* Required for active invite and persistent GO reinvoke; optional for persistent client. */
    uint8_t go_dev_addr_valid;             /* Optional for active invite; invalid for persistent invite. */
    uint8_t persistent;                    /* 0 to invite to current group, 1 to reinvoke persistent group. */
    int persistent_id;                     /* Persistent network id. Valid when persistent is 1. */
    uint16_t freq;                         /* 0 to omit, otherwise force operating frequency in MHz for persistent. */
    uint16_t pref_freq;                    /* 0 to omit, otherwise preferred frequency in MHz for persistent. */
    uint16_t max_oper_chwidth;              /* 0 to omit, otherwise 20, 40, or 80 MHz for persistent. */
    uint8_t ht40;                          /* 1 to request HT40 for persistent reinvocation. */
    uint8_t vht;                           /* 1 to request VHT for persistent reinvocation. */
    uint8_t he;                            /* 1 to request HE for persistent reinvocation. */
} wifi_mgmr_p2p_invite_params_t;

typedef struct wifi_mgmr_p2p_peers
{
    uint8_t peer_count;                    /* Number of addresses stored in peer_addr. */
    uint8_t peer_count_total;              /* Total addresses reported by supplicant. */
    uint8_t peer_addr[WIFI_MGMR_P2P_MAX_PEERS][WIFI_MGMR_P2P_ADDR_LEN];
} wifi_mgmr_p2p_peers_t;

typedef struct wifi_mgmr_p2p_peer_info
{
    uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN];
    char pri_dev_type[WIFI_MGMR_P2P_DEV_TYPE_STR_LEN];
    char device_name[WIFI_MGMR_P2P_NAME_STR_LEN];
    char manufacturer[WIFI_MGMR_P2P_NAME_STR_LEN];
    char model_name[WIFI_MGMR_P2P_NAME_STR_LEN];
    char model_number[WIFI_MGMR_P2P_NAME_STR_LEN];
    char serial_number[WIFI_MGMR_P2P_NAME_STR_LEN];
    uint16_t config_methods;
    uint8_t dev_capab;
    uint8_t group_capab;
    int8_t level;
    int oper_freq;                         /* GO operating frequency in MHz, 0 if unknown. */
} wifi_mgmr_p2p_peer_info_t;

typedef struct wifi_mgmr_p2p_status
{
    wifi_mgmr_p2p_role_t role;
    uint8_t completed;
    uint16_t freq;
    char bssid[WIFI_MGMR_P2P_ADDR_STR_LEN];
    char ssid[WIFI_MGMR_P2P_SSID_STR_LEN];
    char mode[WIFI_MGMR_P2P_MODE_STR_LEN];
    char wpa_state[WIFI_MGMR_P2P_STATE_STR_LEN];
    char p2p_device_addr[WIFI_MGMR_P2P_ADDR_STR_LEN];
    char address[WIFI_MGMR_P2P_ADDR_STR_LEN];
} wifi_mgmr_p2p_status_t;

typedef struct wifi_mgmr_async_event
{
    void *entry;
    uintptr_t type;
    void (*finish)(void *);
    uint16_t size;
    uint16_t code;
    unsigned long value;
} wifi_mgmr_async_event_t;

typedef struct wifi_mgmr_p2p_event
{
    wifi_mgmr_async_event_t event;        /* Same layout as struct async_input_event. */
    uint8_t vif_idx;                      /* fhost vif index, duplicated from event.value. */
    wifi_mgmr_p2p_role_t role;
    uint16_t freq;
    int status;
    int persistent_id;
    uint16_t config_methods;
    uint8_t dev_capab;
    uint8_t group_capab;
    int8_t level;
    uint8_t go_intent;
    uint16_t dev_passwd_id;                /* wifi_mgmr_p2p_dev_password_id_t when recognized. */
    uint8_t persistent;                    /* 1 when GROUP_STARTED represents a persistent group. */
    uint8_t has_freq;
    uint8_t has_status;
    uint8_t has_persistent_id;
    uint8_t has_persistent;
    uint8_t has_config_methods;
    uint8_t has_dev_capab;
    uint8_t has_group_capab;
    uint8_t has_level;
    uint8_t has_go_intent;
    uint8_t has_dev_passwd_id;
    uint8_t raw_truncated;                /* Reserved; structured events set this to 0. */
    char raw[WIFI_MGMR_P2P_EVENT_RAW_LEN]; /* Reserved; structured events leave this empty. */
    char peer_addr[WIFI_MGMR_P2P_ADDR_STR_LEN];
    char peer_iface_addr[WIFI_MGMR_P2P_ADDR_STR_LEN];
    char go_dev_addr[WIFI_MGMR_P2P_ADDR_STR_LEN];
    char bssid[WIFI_MGMR_P2P_ADDR_STR_LEN];
    char ifname[WIFI_MGMR_P2P_EVENT_IFNAME_LEN];
    char role_name[WIFI_MGMR_P2P_EVENT_ROLE_LEN];
    char ssid[WIFI_MGMR_P2P_EVENT_SSID_STR_LEN]; /* Supplicant printf-encoded text. */
    char device_name[WIFI_MGMR_P2P_NAME_STR_LEN];
    char pin[WIFI_MGMR_P2P_EVENT_PIN_LEN];
    char reason[WIFI_MGMR_P2P_EVENT_REASON_LEN];
    char wps_method[WIFI_MGMR_P2P_EVENT_METHOD_LEN];
} wifi_mgmr_p2p_event_t;
#endif /* CONFIG_WIFI_P2P */

#define WIFI_EVENT_BEACON_IND_AUTH_OPEN            0
#define WIFI_EVENT_BEACON_IND_AUTH_WEP             1
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK         2
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK        3
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK    4
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT         5
#define WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE        6
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE 7
#define WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN      0xff

typedef enum {
    WIFI_AP_STA_AUTH_FAIL_PSK_MISMATCH = 1,
    WIFI_AP_STA_AUTH_FAIL_SAE_CONFIRM_MISMATCH = 5,
} wifi_ap_sta_auth_fail_reason_t;

typedef struct {
    uint8_t sta_mac[6];
    uint8_t auth_mode;
    uint8_t reason;
} wifi_ap_sta_auth_fail_info_t;

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

/* AP-advertised operating bandwidth, independent of local PHY capabilities. */
typedef enum {
    WIFI_MGMR_SCAN_BW_UNKNOWN = 0,
    WIFI_MGMR_SCAN_BW_20,
    WIFI_MGMR_SCAN_BW_40,
    WIFI_MGMR_SCAN_BW_80,
    WIFI_MGMR_SCAN_BW_160,
    WIFI_MGMR_SCAN_BW_80P80,
} wifi_mgmr_scan_bw_t;

typedef struct wifi_mgmr_scan_item {
    uint32_t mode;
    uint32_t timestamp_lastseen;
    int ssid_len;
    uint8_t channel;
    int8_t rssi;
    char ssid[32];
    char ssid_tail[1];//always put ssid_tail after ssid
    uint8_t bssid[6];
    int8_t ppm_abs; /* unused on wifi6, kept for layout parity with wifi4 */
    int8_t ppm_rel; /* unused on wifi6, kept for layout parity with wifi4 */
    uint8_t auth;
    uint8_t cipher;
    uint8_t is_used;
    uint8_t wps;
    uint8_t best_antenna;
    uint8_t bandwidth; /* wifi_mgmr_scan_bw_t */
    int8_t pmf; /* 0 disabled, 1 capable, 2 required, -1 unavailable */
} wifi_mgmr_scan_item_t;

/**
 * @brief Channel utilization measured during a scan.
 *
 * One entry is generated for each scanned channel, including channels where
 * no AP was found. Results remain valid until the next scan starts.
 */
typedef struct wifi_mgmr_channel_survey {
    uint8_t channel;       /**< IEEE 802.11 channel number. */
    uint8_t utilization;   /**< Channel utilization from 0 to 100 percent. */
} wifi_mgmr_channel_survey_t;

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
 *  dhcp_timeout_event_only : 0 legacy (disconnect/reconnect), 1 timeout event only
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
    // 0: legacy disconnect/reconnect on DHCP timeout (default)
    // 1: report DHCP timeout event only
    uint8_t dhcp_timeout_event_only;
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
    /// Borrowed until the connection completes.
    const uint8_t *extra_ies;
    // Total length of extra_ies in bytes; maximum 64 bytes.
    uint16_t extra_ies_len;
} wifi_mgmr_sta_connect_params_t;

/**
 * Raw management frame received during a scan.
 *
 * The frame data is owned by fhost and is valid only for the duration of the
 * scan frame callback. A callback that needs the frame after returning must
 * make its own copy.
 */
typedef struct wifi_mgmr_scan_frame {
    const uint8_t *data;
    uint16_t length;
    uint16_t frequency;
    int16_t rssi;
} wifi_mgmr_scan_frame_t;

/**
 * Process a Beacon or Probe Response received during a scan.
 *
 * Called synchronously in the WiFi task context. The callback is responsible
 * for filtering the frame and, if accepted, copying it into @p frame_queue.
 * It must be limited to fast filtering and non-blocking queue submission. It
 * must not wait, perform time-consuming work, or retain pointers from
 * @p frame, because doing so blocks fhost processing in the WiFi task.
 */
typedef void (*wifi_mgmr_scan_frame_cb_t)(void *arg, void *frame_queue, const wifi_mgmr_scan_frame_t *frame);

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
    /// Borrowed until the scan completes.
    const uint8_t *extra_ies;
    // Total length of extra_ies in bytes; maximum 64 bytes.
    uint16_t extra_ies_len;
    /// Optional raw Beacon/Probe Response callback; defaults to NULL.
    wifi_mgmr_scan_frame_cb_t frame_cb;
    /// Caller context passed to frame_cb; defaults to NULL.
    void *frame_cb_arg;
    /**
     * Caller-owned queue passed unchanged to frame_cb. It must be created
     * before wifi_mgmr_sta_scan() and remain valid until scan completion.
     */
    void *frame_queue;
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

/// AP start parameters
typedef struct wifi_mgmr_ap_params {
    /// The SSID of the AP. Must be set.
    char *ssid;
    /// If NULL and akm is not NULL, the default key is "12345678".
    char *key;
    /// OPEN/WPA/WPA2; if NULL and key is not NULL, the default AKM is WPA2.
    char *akm;
    /// If zero, the default channel is 6.
    uint8_t channel;
    /// Channel type, see mac_chan_bandwidth.
    uint8_t type;
    /// Whether to configure AP IP settings.
    bool use_ipcfg;
    /// Whether to start DHCP server.
    bool use_dhcpd;
    /// DHCP server pool start.
    int start;
    /// DHCP server pool limit.
    int limit;
    /// AP IP address. The AP IPv4 subnet should not overlap the active STA
    /// IPv4 subnet. If overlap is unavoidable, bind application sockets to
    /// the intended netif to avoid ambiguous routing.
    uint32_t ap_ipaddr;
    /// AP subnet mask. See ap_ipaddr for the AP/STA overlap restriction.
    uint32_t ap_mask;
    /// STA max inactivity while connected.
    uint32_t ap_max_inactivity;
    /// Whether to use hidden SSID.
    bool hidden_ssid;
    /// Whether to enable AP isolation.
    bool isolation;
    /// Beacon interval in TU.
    int bcn_interval;
    /// Additional vendor specific elements for Beacon and Probe Response frames.
    /// The value is a continuous raw IE hex string:
    /// element id + length + payload.
    /// Multiple IEs are supported by concatenating complete raw IE hex strings
    /// in order, e.g. "dd0411223301dd05aabbcc0203".
    /// Do not include "0x", spaces, or ':' separators.
    /// Current SDK limit: strlen(ap_vendor_elements) <= MAX_AP_VENDOR_ELEMENTS_LEN.
    /// Ref: wpa_supplicant.conf.
    char *ap_vendor_elements;
    /// Beacon transmission mode:
    /// 0: Start/Stop beacon transmissions automatically. Beacon transmission is
    ///    not started when SAP is started. After receiving a Probe Request with
    ///    the same SSID, the AP replies with a Probe Response and starts Beacon
    ///    transmission. Beacon transmission is stopped again if no STA is
    ///    associated for more than bcn_timer seconds.
    /// 1: Do not transmit Beacon frames.
    /// 2: Transmit Beacon frames. This is the default mode.
    uint8_t bcn_mode;
    /// Beacon transmission is stopped again if no STA is associated for more
    /// than bcn_timer seconds when bcn_mode is 0.
    int bcn_timer;
    /// Disable advertising WME/WMM Information Element in Beacon/ProbeResponse frames.
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
 * param:
 *  ip   : IPv4 address in network byte order
 *  mask : IPv4 subnet mask in network byte order
 *  gw   : IPv4 gateway address in network byte order
 *  dns  : IPv4 DNS server address in network byte order
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

/**
 * Start dhcp client on STA interface
 * IF STA interface is not connected, just return -1
 *
 * @param to_ms Timeout in ms to obtain IP (0 uses WIFI_STA_DHCPC_TIMEOUT_MS_DEFAULT)
 *
 * return 0 on success or others on error
 */
int wifi_sta_dhcp_client_start(uint32_t to_ms);

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
 * Change the channel of an enabled sniffer interface.
 *
 * The interface must already be in monitor mode. The call completes after the
 * channel configuration has been acknowledged by the WiFi firmware; it does
 * not disable monitor mode or recreate the interface.
 *
 * @param sniffer_item Sniffer configuration containing the interface, channel
 *                     and receive callback.
 * @return 0 on success, -1 on failure.
 */
int wifi_mgmr_sniffer_set_channel(wifi_mgmr_sniffer_item_t sniffer_item);

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

#ifdef CONFIG_WIFI_P2P
/**
 * @brief Fixed-interface Wi-Fi Direct (P2P) API.
 *
 * All APIs in this section operate on @ref WIFI_MGMR_P2P_DEFAULT_IFACE
 * (fhost VIF @ref WIFI_MGMR_P2P_FHOST_VIF_IDX). The VIF is shared with SoftAP,
 * so initialization can fail with @ref WIFI_MGMR_P2P_ERR_VIF_BUSY.
 *
 * Commands that start discovery, negotiation, group formation, provision
 * discovery, or invitation are synchronous only until the supplicant accepts
 * the request. A @ref WIFI_MGMR_P2P_OK return does not mean that the radio
 * protocol has completed; applications must consume the corresponding P2P
 * asynchronous events for the final result. Query APIs return a synchronous
 * snapshot of the supplicant state.
 */

/**
 * @brief Initialize the fixed P2P interface and its supplicant configuration.
 *
 * This acquires the shared AP/P2P VIF, creates or rebinds it as needed, stops
 * stale discovery, flushes cached P2P state, enables display/keypad/PBC WPS
 * methods, and configures the operating, listen, and preferred channel.
 *
 * @param[in] channel 2.4 GHz or 5 GHz channel permitted by the active country
 *                    configuration. Disabled, NO_IR, radar, and channel 14 are
 *                    rejected because P2P must be allowed to initiate radio
 *                    transmission.
 * @param[in] p2p_no_group_iface Must be 1. Dynamic group-interface mode (0) is
 *                               not supported by the fixed-VIF API.
 *
 * @retval WIFI_MGMR_P2P_OK Initialization completed and the state is IDLE.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG A parameter or channel is invalid.
 * @retval WIFI_MGMR_P2P_ERR_NOT_SUPPORTED The channel is unavailable or
 *                                          p2p_no_group_iface is 0.
 * @retval WIFI_MGMR_P2P_ERR_VIF_BUSY The shared VIF is owned by SoftAP.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE Negotiation, a group, or stop is in
 *                                          progress.
 * @return Another wifi_mgmr_p2p_error_t value on transport or supplicant
 *         failure.
 */
int wifi_mgmr_p2p_init(uint8_t channel, uint8_t p2p_no_group_iface);

/**
 * @brief Set the device name advertised by P2P/WPS.
 *
 * @param[in] name Non-empty NUL-terminated name of at most
 *                 WIFI_MGMR_P2P_DEVICE_NAME_MAX_LEN bytes. CR and LF are not
 *                 allowed.
 *
 * @retval WIFI_MGMR_P2P_OK The supplicant accepted the new name.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED Call wifi_mgmr_p2p_init() first.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE Negotiation, an active group, or
 *                                          stop is in progress.
 * @return Another wifi_mgmr_p2p_error_t value on invalid input or command
 *         failure.
 */
int wifi_mgmr_p2p_set_device_name(const char *name);

/**
 * @brief Set the operating, listen, and preferred P2P channel.
 *
 * The channel is converted internally to frequency and operating class; a
 * separate frequency-setting API is not required.
 *
 * @param[in] channel Valid 2.4 GHz or 5 GHz channel allowed for initiating
 *                    radiation by the active country configuration.
 *
 * @retval WIFI_MGMR_P2P_OK All channel settings were accepted.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED Call wifi_mgmr_p2p_init() first.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE Negotiation, an active group, or
 *                                          stop is in progress.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG The channel number is invalid.
 * @retval WIFI_MGMR_P2P_ERR_NOT_SUPPORTED The channel is disabled, NO_IR,
 *                                          radar-restricted, or outside the
 *                                          active country configuration.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 */
int wifi_mgmr_p2p_set_channel(uint8_t channel);

/**
 * @brief Park listen and operating channels on the GO channel for join.
 *
 * Call only after the peer GO is known. Discovery should still start on
 * social channels. This is a single-radio workaround so PD/WPS stay on the
 * GO operating channel instead of returning to 1/6/11.
 */
int wifi_mgmr_p2p_set_channel_for_join(uint8_t channel);

/**
 * @brief Reconfigure fixed-interface mode and the P2P channel.
 *
 * Unlike wifi_mgmr_p2p_init(), this function does not create a VIF, clear peer
 * state, or configure WPS methods. The fixed P2P VIF must already be
 * initialized and must not be negotiating or hosting/joining an active group.
 *
 * @param[in] channel See wifi_mgmr_p2p_set_channel().
 * @param[in] p2p_no_group_iface Must be 1; dynamic group interfaces are not
 *                               supported.
 *
 * @retval WIFI_MGMR_P2P_OK Configuration was accepted.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_NOT_SUPPORTED Dynamic interface mode or the
 *                                          selected channel is unsupported.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE The current lifecycle state does not
 *                                          allow reconfiguration.
 * @return Another wifi_mgmr_p2p_error_t value on invalid input or command
 *         failure.
 */
int wifi_mgmr_p2p_config(uint8_t channel, uint8_t p2p_no_group_iface);

/**
 * @brief Start P2P device discovery.
 *
 * @param[in] config Optional discovery parameters. NULL or a zero-filled
 *                   structure starts an unbounded, unfiltered search.
 *                   timeout is in seconds, freq is in MHz, and dev_id is a
 *                   P2P Device Address rather than an interface address.
 *
 * @retval WIFI_MGMR_P2P_OK The supplicant accepted the request and the local
 *                           lifecycle state changed to FINDING.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE Negotiation, an active group, or
 *                                          stop is in progress.
 * @return Another wifi_mgmr_p2p_error_t value on invalid input or command
 *         failure.
 *
 * @note Discovery completion and discovered devices are reported by P2P
 *       asynchronous events.
 */
int wifi_mgmr_p2p_find(const wifi_mgmr_p2p_find_params_t *config);

/**
 * @brief Stop a current find or listen operation.
 *
 * The initialized VIF and peer cache are preserved. Use wifi_mgmr_p2p_flush()
 * to clear discovery/provisioning state or wifi_mgmr_p2p_stop() for complete
 * teardown.
 *
 * @retval WIFI_MGMR_P2P_OK The stop request was accepted.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE Negotiation, an active group, or
 *                                          stop is in progress.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 */
int wifi_mgmr_p2p_stop_find(void);

/**
 * @brief Enter P2P listen state for a requested duration.
 *
 * @param[in] seconds Listen duration passed to the supplicant, in seconds.
 * @param[in] channel 0 keeps the current channel configuration. A nonzero
 *                    value first persistently updates the operating, listen,
 *                    and preferred channels using the same validation as
 *                    wifi_mgmr_p2p_set_channel().
 *
 * @retval WIFI_MGMR_P2P_OK The request was accepted and local state changed to
 *                           LISTENING.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE Negotiation, an active group, or
 *                                          stop is in progress.
 * @return Another wifi_mgmr_p2p_error_t value on invalid input or command
 *         failure.
 *
 * @note Listen expiration is asynchronous.
 */
int wifi_mgmr_p2p_listen(uint16_t seconds, uint8_t channel);

/**
 * @brief Return cached P2P Device Addresses.
 *
 * @param[in] discovered_only 0 returns all cached entries; 1 excludes entries
 *                            learned only from Probe Requests.
 * @param[out] peers Receives up to WIFI_MGMR_P2P_MAX_PEERS addresses. Compare
 *                   peer_count with peer_count_total to detect truncation.
 *
 * @retval WIFI_MGMR_P2P_OK The synchronous snapshot was returned.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG discovered_only is not 0/1 or peers is
 *                                        NULL.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @return Another wifi_mgmr_p2p_error_t value on query or parse failure.
 */
int wifi_mgmr_p2p_peers(uint8_t discovered_only,
                        wifi_mgmr_p2p_peers_t *peers);

/**
 * @brief Flush P2P discovery and provision-discovery state.
 *
 * This stops current discovery and clears cached peers and provisioning state,
 * but preserves the initialized VIF and any active group. It is intentionally
 * narrower than wifi_mgmr_p2p_stop().
 *
 * @retval WIFI_MGMR_P2P_OK The supplicant accepted the flush request.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 */
int wifi_mgmr_p2p_flush(void);

/**
 * @brief Query parsed information for one cached P2P peer.
 *
 * @param[in] peer_addr Six-byte P2P Device Address.
 * @param[out] peer Receives the synchronous structured peer snapshot.
 *
 * @retval WIFI_MGMR_P2P_OK Peer data was returned and parsed.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG An output pointer or address is invalid.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @return Another wifi_mgmr_p2p_error_t value when the peer is unknown or the
 *         query/response fails.
 */
int wifi_mgmr_p2p_peer(const uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN],
                       wifi_mgmr_p2p_peer_info_t *peer);

/**
 * @brief Query the raw supplicant P2P_PEER response for one peer.
 *
 * @param[in] peer_addr Six-byte P2P Device Address.
 * @param[out] raw Destination for a NUL-terminated response.
 * @param[in] raw_len Size of raw, including space for the NUL terminator; must
 *                    be greater than zero.
 * @param[out] copied_len Optional number of response bytes copied, excluding
 *                        the NUL terminator. Set to 0 before a failed query.
 * @param[out] truncated Optional flag set to 1 when the response did not fit in
 *                       raw. Set to 0 before a failed query.
 *
 * @retval WIFI_MGMR_P2P_OK Raw data was returned; check truncated separately.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG An address/buffer argument is invalid.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @return Another wifi_mgmr_p2p_error_t value on query failure.
 */
int wifi_mgmr_p2p_peer_raw(
    const uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN], char *raw,
    uint16_t raw_len, uint16_t *copied_len, uint8_t *truncated);

/**
 * @brief Start P2P negotiation, authorization, or join.
 *
 * @param[in] config Peer, WPS method, role options, GO intent, and optional
 *                   frequency. join, auto_join, and auth are mutually
 *                   exclusive. auto_join selects join when the peer is a GO,
 *                   otherwise it falls back to GO Negotiation. For
 *                   automatic display PIN, set method to
 *                   WIFI_MGMR_P2P_METHOD_PIN_DISPLAY and pin to NULL.
 * @param[out] result Required for automatic display PIN and receives the
 *                    generated PIN. Optional and cleared when supplied for
 *                    other methods.
 *
 * @retval WIFI_MGMR_P2P_OK The request was accepted. A negotiation/join request
 *                           changes local state to NEGOTIATING. An auth-only
 *                           request preserves the current state until a later
 *                           protocol event. In automatic PIN mode, result
 *                           already contains the generated PIN.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG The parameter combination is invalid or
 *                                        automatic PIN has no result buffer.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE Negotiation, an active group, or
 *                                          stop is already in progress.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 *
 * @note Group negotiation and formation complete asynchronously; wait for the
 *       group-started or failure events. A GO_NEG_REQUEST event is a policy
 *       notification and does not change the local lifecycle state. An
 *       application that accepts it can copy event.peer_addr, select the WPS
 *       method from event.dev_passwd_id, set config.auth to 1, and call this
 *       function to authorize the peer for a subsequent request. Authorization
 *       itself preserves the lifecycle state; a later GO negotiation event
 *       advances it.
 */
int wifi_mgmr_p2p_connect(
    const wifi_mgmr_p2p_connect_params_t *config,
    wifi_mgmr_p2p_connect_result_t *result);

/**
 * @brief Cancel in-progress P2P formation, join, or invitation work.
 *
 * This maps directly to the standard P2P_CANCEL command. It preserves the VIF,
 * peer cache, and active group. It is not an alias for
 * wifi_mgmr_p2p_stop_find().
 *
 * @retval WIFI_MGMR_P2P_OK A cancellable operation was accepted for cancel.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_SUPPLICANT No cancellable operation exists or the
 *                                       supplicant rejected the command.
 * @return Another wifi_mgmr_p2p_error_t value on transport failure.
 */
int wifi_mgmr_p2p_cancel(void);

/**
 * @brief Create an autonomous GO or reinvoke/create a persistent group.
 *
 * @param[in] config Optional group parameters. NULL creates an autonomous GO
 *                   with supplicant defaults. persistent and
 *                   persistent_id_valid are mutually exclusive. passphrase is
 *                   an 8..63-byte printable value and cannot replace the saved
 *                   passphrase of persistent_id_valid.
 *
 * @retval WIFI_MGMR_P2P_OK The request was accepted and local state changed to
 *                           NEGOTIATING.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG A parameter combination, persistent ID,
 *                                        or passphrase is invalid.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE Negotiation, an active group, or
 *                                          stop is already in progress.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 *
 * @note Group creation completes asynchronously.
 */
int wifi_mgmr_p2p_group_add(const wifi_mgmr_p2p_group_add_params_t *config);

/**
 * @brief List saved P2P persistent-group profiles.
 *
 * This maps to the standard LIST_NETWORKS command and returns only entries
 * marked P2P-PERSISTENT. Profiles restored from the PSM partition are visible
 * after wifi_mgmr_p2p_init().
 *
 * @param[out] groups Receives up to WIFI_MGMR_P2P_MAX_PERSISTENT_GROUPS
 *                    profiles and the total count reported by supplicant.
 *
 * @retval WIFI_MGMR_P2P_OK The list was read successfully.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG groups is NULL or the response is
 *                                        malformed.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 */
int wifi_mgmr_p2p_persistent_list(
    wifi_mgmr_p2p_persistent_groups_t *groups);

/**
 * @brief Delete one saved P2P persistent-group profile.
 *
 * This maps to the standard REMOVE_NETWORK command. On success, the updated
 * persistent-group list is written to the PSM partition immediately.
 *
 * @param[in] persistent_id Non-negative network ID returned by
 *                          wifi_mgmr_p2p_persistent_list().
 *
 * @retval WIFI_MGMR_P2P_OK The profile was removed and persisted.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG persistent_id is negative.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_SUPPLICANT The profile does not exist or cannot be
 *                                       removed.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 */
int wifi_mgmr_p2p_persistent_remove(int persistent_id);

/**
 * @brief Remove the active group on WIFI_MGMR_P2P_DEFAULT_IFACE.
 *
 * The fixed P2P VIF remains initialized and returns to IDLE after the command
 * is accepted.
 *
 * @retval WIFI_MGMR_P2P_OK The remove request was accepted.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE No active group exists.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 */
int wifi_mgmr_p2p_group_remove(void);

/**
 * @brief Start WPS PBC on the active local P2P GO.
 *
 * @param[in] peer_addr Optional six-byte P2P Device Address used to restrict
 *                      enrollment to one peer. Pass NULL to accept any peer.
 *
 * @retval WIFI_MGMR_P2P_OK WPS PBC was started.
 * @retval WIFI_MGMR_P2P_ERR_VIF_BUSY VIF 1 is owned by SoftAP.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE No P2P group is active.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 */
int wifi_mgmr_p2p_wps_pbc(
    const uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN]);

/**
 * @brief Start P2P Provision Discovery with a cached peer.
 *
 * @param[in] config Peer address, PBC/display/keypad method, and optional join
 *                   or auto-join behavior. join and auto_join are mutually
 *                   exclusive.
 *
 * @retval WIFI_MGMR_P2P_OK The request was accepted.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG The configuration is NULL or invalid.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 *
 * @note The peer response or failure is delivered asynchronously.
 */
int wifi_mgmr_p2p_prov_disc(const wifi_mgmr_p2p_prov_disc_params_t *config);

/**
 * @brief Invite a peer to an active group or reinvoke a persistent group.
 *
 * @param[in] config Invitation parameters. An active-group invitation requires
 *                   peer_addr_valid and GROUP_ACTIVE state; go_dev_addr is
 *                   optional. A persistent invitation requires a non-negative
 *                   persistent_id and does not accept go_dev_addr. peer is
 *                   required when the saved local role is GO and may be
 *                   omitted when the saved peer is the GO. A specified peer
 *                   must already be present in the supplicant peer cache.
 *                   freq, pref_freq, max_oper_chwidth, ht40, vht, and he apply
 *                   only to persistent reinvocation. Unsupported channel
 *                   capabilities are rejected by supplicant.
 *
 * @retval WIFI_MGMR_P2P_OK The invitation was accepted. Persistent reinvoke
 *                           changes local state to NEGOTIATING.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG The configuration is NULL or invalid.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_STATE The active/persistent invitation is
 *                                          not allowed in the current state.
 * @return Another wifi_mgmr_p2p_error_t value on command failure.
 *
 * @note Invitation acceptance and result are asynchronous events.
 */
int wifi_mgmr_p2p_invite(const wifi_mgmr_p2p_invite_params_t *config);

/**
 * @brief Fully stop fixed-interface P2P and release its VIF.
 *
 * This attempts to stop discovery, remove all groups, flush cached P2P state,
 * remove the supplicant VIF, and release shared AP/P2P ownership. Use
 * wifi_mgmr_p2p_group_remove() or wifi_mgmr_p2p_flush() when the initialized
 * VIF must be preserved.
 *
 * @retval WIFI_MGMR_P2P_OK Teardown completed.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED The fixed VIF is not owned by P2P.
 * @return Another wifi_mgmr_p2p_error_t value when a teardown step fails.
 */
int wifi_mgmr_p2p_stop(void);

/**
 * @brief Return a synchronous status snapshot from the fixed P2P VIF.
 *
 * @param[out] status Receives role, completion flag, frequency, BSSID, SSID,
 *                    interface mode/state, P2P Device Address, and local
 *                    interface address. Fields unavailable in the supplicant
 *                    response remain zero/empty.
 *
 * @retval WIFI_MGMR_P2P_OK Status was returned and parsed.
 * @retval WIFI_MGMR_P2P_ERR_INVALID_ARG status is NULL.
 * @retval WIFI_MGMR_P2P_ERR_NOT_INITIALIZED P2P is not initialized.
 * @return Another wifi_mgmr_p2p_error_t value on query or parse failure.
 */
int wifi_mgmr_p2p_status(wifi_mgmr_p2p_status_t *status);

/**
 * @brief Read the local fixed-interface P2P lifecycle state.
 *
 * This lock-protected local state is updated by accepted commands and parsed
 * P2P events. It is useful for admission/error reporting but does not replace
 * protocol event handling or wifi_mgmr_p2p_status().
 *
 * @return Current wifi_mgmr_p2p_state_t value.
 */
wifi_mgmr_p2p_state_t wifi_mgmr_p2p_state_get(void);

/**
 * @brief Convert a P2P API return value to a stable diagnostic string.
 *
 * @param[in] error A wifi_mgmr_p2p_error_t value or another integer.
 *
 * @return Static NUL-terminated text. Unknown values map to "unknown error";
 *         the returned pointer must not be freed or modified.
 */
const char *wifi_mgmr_p2p_error_string(int error);
#endif /* CONFIG_WIFI_P2P */

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
 * @brief Copy AP results from the scan cache.
 *
 * Call after receiving the EV_WIFI/CODE_WIFI_ON_SCAN_DONE event. For example:
 * @code
 * uint32_t num = wifi_mgmr_sta_scanlist_nums_get();
 * if (num > 0) {
 *     wifi_mgmr_scan_item_t results[num];
 *     num = wifi_mgmr_sta_scanlist_dump(results, num);
 * }
 * @endcode
 * The caller provides the result array.
 *
 * @param results    Destination array of wifi_mgmr_scan_item_t entries.
 * @param resultNums Number of entries available in @p results.
 *
 * @return Number of entries copied.
 */
uint32_t wifi_mgmr_sta_scanlist_dump(void * results, uint32_t resultNums);

/**
 * @brief Get the number of channel survey results from the latest scan.
 *
 * Call this after receiving the EV_WIFI/CODE_WIFI_ON_SCAN_DONE event to size
 * the buffer passed to
 * wifi_mgmr_scan_channel_survey_dump().
 *
 * @return Number of valid channel survey results.
 */
uint32_t wifi_mgmr_scan_channel_survey_nums_get(void);

/**
 * @brief Copy channel survey results from the latest scan.
 *
 * Call after receiving the EV_WIFI/CODE_WIFI_ON_SCAN_DONE event. For example:
 * @code
 * uint32_t num = wifi_mgmr_scan_channel_survey_nums_get();
 * if (num > 0) {
 *     wifi_mgmr_channel_survey_t results[num];
 *     num = wifi_mgmr_scan_channel_survey_dump(results, num);
 * }
 * @endcode
 * Each returned entry contains a channel number and its utilization percentage.
 * The caller provides the result array.
 *
 * @param results     Destination array of wifi_mgmr_channel_survey_t entries.
 * @param result_nums Number of entries available in @p results.
 *
 * @return Number of entries copied. Returns 0 for an invalid or empty buffer.
 */
uint32_t wifi_mgmr_scan_channel_survey_dump(wifi_mgmr_channel_survey_t *results,
                                            uint32_t result_nums);

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
 * Get the oldest pending SoftAP station credential mismatch record.
 *
 * Drain pending records on CODE_WIFI_ON_AP_STA_AUTH_FAIL. Reports
 * PSK_MISMATCH or SAE_CONFIRM_MISMATCH. auth_mode uses
 * WIFI_EVENT_BEACON_IND_AUTH_*.
 *
 * @return 0 on success, -1 if WiFi is not ready, the queue is empty,
 *         or input is invalid.
 */
int wifi_mgmr_ap_sta_auth_fail_get(wifi_ap_sta_auth_fail_info_t *info);

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
 * Set IPv4 address of AP interface
 *
 * This API can be used to update the AP static IPv4 configuration after AP mode
 * has been started.
 *
 * The Wi-Fi manager and AP must be started, and the AP DHCP server must be
 * disabled before calling this API.
 *
 * param:
 *  ip   : IPv4 address in network byte order
 *  mask : IPv4 subnet mask in network byte order
 *  gw   : IPv4 gateway address in network byte order
 *  dns  : IPv4 DNS server address in network byte order
 *
 * return 0 on success or -1 on error
 */
int wifi_mgmr_ap_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns);

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
 * wifi_mgmr_sta_reconnect_policy_set
 * Set station reconnect policy used after authentication failure.
 * param:
 *  cfg : Pointer to reconnect policy configuration.
 *        cfg->mode:
 *          WIFI_MGMR_STA_RECONNECT_MODE_EXP_BACKOFF(0)
 *            Always use legacy exponential backoff after auth failures.
 *            cfg->auth_fail_threshold and cfg->fixed_interval_sec are accepted
 *            by range check but not used for timing in this mode.
 *
 *          WIFI_MGMR_STA_RECONNECT_MODE_FIXED_AFTER_THRESHOLD(1)
 *            Use legacy exponential backoff first, then switch to fixed interval
 *            when auth_failures > cfg->auth_fail_threshold.
 *            Fixed wait duration is cfg->fixed_interval_sec seconds.
 *
 *        cfg->auth_fail_threshold:
 *          Valid range: 0 .. WIFI_MGMR_STA_RECONNECT_MAX_AUTH_FAIL_THRESHOLD.
 *          Trigger condition is strict greater-than ('>'), not greater-or-equal.
 *          Example: threshold=2 switches to fixed interval from failure #3.
 *
 *        cfg->fixed_interval_sec:
 *          Valid range:
 *          WIFI_MGMR_STA_RECONNECT_MIN_INTERVAL_SEC ..
 *          WIFI_MGMR_STA_RECONNECT_MAX_INTERVAL_SEC.
 *
 *        Invalid cfg effects:
 *          - Return -1.
 *          - Previous reconnect policy remains unchanged.
 *
 *        Example 1:
 *          mode=0, threshold=10, interval=15
 *          -> always exponential backoff.
 *
 *        Example 2:
 *          mode=1, threshold=2, interval=5
 *          -> failures #1/#2 use exponential backoff,
 *             failure #3 and later use fixed 5-second interval.
 * return:
 *  0 : Success
 *  -1 : Failed
 */
int wifi_mgmr_sta_reconnect_policy_set(const wifi_mgmr_sta_reconnect_policy_t *cfg);

/**
 * wifi_mgmr_sta_reconnect_policy_get
 * Get station reconnect policy used after authentication failure.
 * return:
 *  0 : Success
 *  -1 : Failed
 */
int wifi_mgmr_sta_reconnect_policy_get(wifi_mgmr_sta_reconnect_policy_t *cfg);

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
 *  bssid: Target AP BSSID, or NULL for any AP advertising an active PBC session.
 *  channel: Target channel number, or 0 to scan all channels.
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_wps_pbc(uint8_t auth, const uint8_t *bssid, uint8_t channel);

/** Cancel the active STA WPS PBC session. */
int wifi_mgmr_sta_wps_pbc_cancel(void);

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
 *      ms: active time in milliseconds, valid range 10-90
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
 * @brief Get WiFi Manager STA TWT (Target Wake Time) status
 *
 * This function retrieves the current TWT status including all active TWT flows
 * and their configurations.
 *
 * @param conf Pointer to an array of twt_status_info structures to store TWT flow configurations
 * @param twt_num Pointer to store the number of active TWT flows
 * @return Returns the result of the function execution, 0 for success and -1 for failure
 */
int wifi_mgmr_sta_twt_statusget(struct twt_status_info *conf, uint8_t *twt_num);

/**
 * @brief Get active STA TWT flow count.
 *
 * @return Active TWT flow count, or 0 when TWT is not enabled.
 */
int wifi_mgmr_sta_twt_flow_get(void);

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
/**
 * wifi_mgmr_null_data_send
 * Send null packet
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_null_data_send(void);

/**
 * wifi_mgmr_psk_cal
 * Calucate psk by using passpharse
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_psk_cal(char *password, const uint8_t *ssid, int ssid_len, char *output);

/*
 * Rate index constants for wifi_mgmr_rate_config fields.
 */
#define WIFI_RC_IDX_NO_UPDATE   (0)
#define WIFI_RC_IDX_AUTO        (-1)

/* 11b (CCK) */
#define WIFI_RC_IDX_1M          (1)
#define WIFI_RC_IDX_2M          (3)
#define WIFI_RC_IDX_5M5         (5)
#define WIFI_RC_IDX_11M         (7)

/* 11g (OFDM) */
#define WIFI_RC_IDX_6M          (8)
#define WIFI_RC_IDX_9M          (9)
#define WIFI_RC_IDX_12M         (10)
#define WIFI_RC_IDX_18M         (11)
#define WIFI_RC_IDX_24M         (12)
#define WIFI_RC_IDX_36M         (13)
#define WIFI_RC_IDX_48M         (14)
#define WIFI_RC_IDX_54M         (15)

/* 11n HT20 1SS LGI */
#define WIFI_RC_IDX_HT20_MCS0   (16)
#define WIFI_RC_IDX_HT20_MCS1   (20)
#define WIFI_RC_IDX_HT20_MCS2   (24)
#define WIFI_RC_IDX_HT20_MCS3   (28)
#define WIFI_RC_IDX_HT20_MCS4   (32)
#define WIFI_RC_IDX_HT20_MCS5   (36)
#define WIFI_RC_IDX_HT20_MCS6   (40)
#define WIFI_RC_IDX_HT20_MCS7   (44)
/**
 * wifi_mgmr_rate_config
 * param:
 *  cfg: pointer to rate config parameters
 *
 * Rate index values (used by fixed_rate_idx and retry_min_rate_idx):
 * 11ax: MCS9-893, MCS8-881, MCS7-869, MCS6-857, MCS5-845, MCS4-833, MCS3-821, MCS2-809, MCS1-797, MCS0-785;
 * 11n: MCS7-44, MCS6-40, MCS5-36, MCS4-32, MCS3-28, MCS2-24, MCS1-20, MCS0-16;
 * 11g: 54M-15, 48M-14, 36M-13, 24M-12, 18M-11, 12M-10, 9M-9, 6M-8;
 * 11b: 11M-7, 5.5M-5, 2M-3, 1M-1
 *
 * retry_min_rate_idx only supports 11b/11g legacy rate (see WIFI_RC_IDX_1M ~ WIFI_RC_IDX_54M).
 * Applies to all current peers:
 * - STA mode: the connected AP peer
 * - AP mode: all associated stations; if new STA connects, set again after association
 *
 * Example:
 *  // set min rate = 6Mbps, keep fixed rate unchanged
 *  wifi_mgmr_rate_config(&(struct wifi_mgmr_rate_config){
 *      .fixed_rate_idx = WIFI_RC_IDX_NO_UPDATE,
 *      .retry_min_rate_idx = WIFI_RC_IDX_6M,
 *  });
 *
 *  // clear fixed rate, set min rate = 6Mbps
 *  wifi_mgmr_rate_config(&(struct wifi_mgmr_rate_config){
 *      .fixed_rate_idx = WIFI_RC_IDX_AUTO,
 *      .retry_min_rate_idx = WIFI_RC_IDX_6M,
 *  });
 *
 *  // set fixed rate = MCS4, min rate = 6Mbps
 *  wifi_mgmr_rate_config(&(struct wifi_mgmr_rate_config){
 *      .fixed_rate_idx = WIFI_RC_IDX_HT20_MCS4,
 *      .retry_min_rate_idx = WIFI_RC_IDX_6M,
 *  });
 *
 * return:
 *  0 : Success
 *  -1 : Failed
 */
struct wifi_mgmr_rate_config {
    int fixed_rate_idx;         ///< > 0: set, WIFI_RC_IDX_NO_UPDATE(0): skip, WIFI_RC_IDX_AUTO(-1): clear
    int retry_min_rate_idx;     ///< > 0: set, WIFI_RC_IDX_NO_UPDATE(0): skip, WIFI_RC_IDX_AUTO(-1): clear
};

int wifi_mgmr_rate_config(const struct wifi_mgmr_rate_config *cfg);


int wifi_mgmr_rate_config_sta(uint16_t fixed_rate_cfg);

/// Rate control parameters for RC_SET_RATE command
struct fhost_rc_params {
    uint8_t update_flags;       ///< ME_RC_SET_RATE_*_BIT bitmask
    int fixed_rate_idx;         ///< >= 0: set rate, -1: auto/clear
    uint8_t gi;                 ///< 0xFF: no change, else GI override for fixed rate
    int retry_min_rate_idx;     ///< >= 0: set rate, -1: auto/clear
    int retry_max_rate_idx;     ///< >= 0: set rate, -1: auto/clear
};

int fhost_rc_set_rate_sta(void *sta, const struct fhost_rc_params *params);

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

/**
 * wifi_mgmr_set_ldpc_rx_enable
 * Toggle state of LDPC RX mode
 * Attention: configuration takes effect only
 *            before the initiation of the WiFi task.
 * param:
 *  value : 1 = enable; 0 = disable
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_set_ldpc_rx_enable(uint8_t value);

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
 * wifi_sta_ipv6_enable
 * Enable or disable ipv6
 *
 * @return 0 on success, otherwise a negative lwIP err_t value.
 */
#ifdef CFG_IPV6
int wifi_sta_ipv6_enable(int enable);
#endif

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

/**
 * @brief Check whether a Wi-Fi channel is allowed by the active country code.
 *
 * The configured 2.4 GHz channel range is checked on every target. BL618DG
 * also checks the configured 5 GHz channel list.
 *
 * @param channel Wi-Fi channel number to validate.
 * @return 0 if the channel is allowed, otherwise -1.
 */
int wifi_mgmr_channel_valid_check(uint16_t channel);

/**
 * @brief Check if a WiFi mode is valid
 *
 * @param mode The WiFi mode to validate
 * @return int
 *   - 0: Mode is valid
 *   - -1: Mode is invalid
 */
int wifi_mgmr_wifimode_valid_check(uint16_t mode);

/**
 * @brief Trigger an AP Channel Switch Announcement (CSA) to migrate the running
 *        SoftAP to a new channel without tearing down associated stations.
 *
 * Hostapd advertises the upcoming switch by inserting a CSA IE in the next
 * @p cs_count beacons. The firmware decrements the counter on every beacon TX
 * and performs the actual channel switch when the counter reaches zero. When
 * the switch completes, @ref CODE_WIFI_ON_AP_CSA_DONE is posted to the user
 * event handler with the new channel number carried in @c ev->value.
 *
 * @param channel  Target primary channel (2.4 GHz: 1..14, 5 GHz: 36..165).
 *                 The band is auto-detected from the channel number. The
 *                 channel is validated against the current regulatory domain
 *                 via @ref wifi_mgmr_channel_valid_check before being applied.
 * @param cs_count Number of beacons to send before switching. Unit is one
 *                 beacon interval (TBTT), i.e. the actual delay is roughly
 *                 cs_count * beacon_interval (default 100ms per beacon).
 *                 A value of 0 is replaced by a default of 10 (~1 second).
 *                 Larger values give associated stations more time to follow
 *                 the switch.
 *
 * @return int
 *   - 0: Request accepted and forwarded to hostapd
 *   - -1: AP is not started, channel is invalid, or hostapd rejected the request
 *
 * @note The AP must have been started via @ref wifi_mgmr_ap_start before
 *       calling this function.
 */
int wifi_mgmr_ap_chan_switch(int channel, uint8_t cs_count);

#endif
