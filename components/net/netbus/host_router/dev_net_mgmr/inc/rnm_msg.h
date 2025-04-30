#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BF1B_CMD_REBOOT = 0,
    BF1B_CMD_RESET,
    BF1B_CMD_HELLO,
    BF1B_CMD_PING,

    BF1B_CMD_GET_MAC_ADDR,

    // Scan
    BF1B_CMD_SCAN,
    BF1B_CMD_SCAN_RESULTS,

    // STA
    BF1B_CMD_STA_CONNECT,
    BF1B_CMD_STA_DISCONNECT,
    BF1B_CMD_STA_CONNECTED_IND,
    BF1B_CMD_STA_DISCONNECTED_IND,
    BF1B_CMD_STA_IP_UPDATE_IND,
    BF1B_CMD_STA_SET_AUTO_RECONNECT,
    BF1B_CMD_STA_GET_LINK_STATUS,

    // AP
    BF1B_CMD_AP_START,
    BF1B_CMD_AP_STOP,
    BF1B_CMD_AP_STARTED_IND,
    BF1B_CMD_AP_STOPPED_IND,
    BF1B_CMD_AP_GET_STA_LIST,

    // Monitor
    BF1B_CMD_MONITOR_START,
    BF1B_CMD_MONITOR_STOP,
    BF1B_CMD_MONITOR_SET_CHANNEL,
    BF1B_CMD_MONITOR_GET_CHANNEL,

    BF1B_CMD_SET_LPM_MODE,

    // OTA
    BF1B_CMD_GET_DEV_VERSION,
    BF1B_CMD_OTA,

    BF1B_CMD_EXT,

    BF1B_CMD_USER_EXT,
    BF1B_CMD_USER_EXT_NO_RSP,
    BF1B_CMD_USER_EXT_RSP,

    BF1B_CMD_MAX,
} bf1b_cmd_t;

#pragma pack(push, 1)
//// This protocol is LE so LE/BE conversion is hardly needed.

typedef enum {
    STATUS_OK,
    STATUS_NOMEM = 128,
    STATUS_INVALID_INPUT,
    STATUS_INVALID_MODE,
    STATUS_ERR_UNSPECIFIED,
    STATUS_NOT_IMPLEMENTED,
} cmd_status_t;

//// common header
typedef struct {
    uint16_t cmd;
    // flag ACK is used by server to indicate a response to client
#define RNM_MSG_FLAG_ACK         (1 << 0)
    // flag TRANSPARENT is never transfered to peer but used locally
#define RNM_MSG_FLAG_TRANSPARENT (1 << 1)
    // flag ASYNC is used by server to notify client events such as STA_CONNECTED
#define RNM_MSG_FLAG_ASYNC       (1 << 2)
    uint16_t flags;
    uint16_t status;
    uint16_t msg_id;
    uint16_t session_id;
    uint16_t msg_id_replying;
} rnm_base_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
} rnm_ack_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t sta_mac[6];
    uint8_t ap_mac[6];
} rnm_mac_addr_ind_msg_t;

//// Scan
struct bf1b_wifi_scan_record {
    uint8_t bssid[6];
    // TODO use compressed SSID encoding to save room
    uint8_t ssid[32 + 1];
    uint16_t channel;
    int8_t rssi;
    uint8_t auth_mode;
    uint8_t cipher;
};

typedef struct {
    rnm_base_msg_t hdr;
    uint16_t num;
    struct bf1b_wifi_scan_record records[];
} rnm_scan_ind_msg_t;

//// STA
typedef struct {
    rnm_base_msg_t hdr;
    uint16_t ssid_len;
    uint8_t ssid[32];
    uint8_t password[64];
} rnm_sta_connect_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t ip4_addr[4];
    uint8_t ip4_mask[4];
    uint8_t ip4_gw[4];
    uint8_t ip4_dns1[4];
    uint8_t ip4_dns2[4];
    uint8_t gw_mac[6];
} rnm_sta_ip_update_ind_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t en;
} rnm_sta_set_auto_reconnect_msg_t;

typedef enum {
    RNM_WIFI_AUTH_UNKNOWN = 0,
    RNM_WIFI_AUTH_OPEN,
    RNM_WIFI_AUTH_WEP,
    RNM_WIFI_AUTH_WPA_PSK,
    RNM_WIFI_AUTH_WPA2_PSK,
    RNM_WIFI_AUTH_WPA_WPA2_PSK,
    RNM_WIFI_AUTH_WPA_ENTERPRISE,
    RNM_WIFI_AUTH_WPA3_SAE,
    RNM_WIFI_AUTH_WPA2_PSK_WPA3_SAE,
    RNM_WIFI_AUTH_MAX,
} rnm_wifi_auth_mode_t;

typedef enum {
    RNM_WIFI_CIPHER_UNKNOWN = 0,
    RNM_WIFI_CIPHER_NONE,
    RNM_WIFI_CIPHER_WEP,
    RNM_WIFI_CIPHER_AES,
    RNM_WIFI_CIPHER_TKIP,
    RNM_WIFI_CIPHER_TKIP_AES,
    RNM_WIFI_CIPHER_MAX,
} rnm_wifi_cipher_t;

typedef enum {
    BF1B_WIFI_LINK_STATUS_UNKNOWN = 0,
    BF1B_WIFI_LINK_STATUS_DOWN,
    BF1B_WIFI_LINK_STATUS_UP,
} bf1b_wifi_link_status_t;

struct bf1b_wifi_ap_record {
    uint8_t link_status;
    uint8_t bssid[6];
    uint8_t ssid[32 + 1];
    uint8_t channel;
    int8_t rssi;
    uint8_t auth_mode;
    uint8_t cipher;
};

typedef struct {
    rnm_base_msg_t hdr;
    struct bf1b_wifi_ap_record record;
} rnm_sta_link_status_ind_msg_t;

//// AP
// TODO hidden SSID mode
typedef struct {
    rnm_base_msg_t hdr;
    uint8_t is_open;
    uint16_t channel;
    uint16_t ssid_len;
    uint8_t ssid[32];
    uint8_t password[64];
} rnm_ap_start_msg_t;

// get station list
struct wifi_sta_info {
    uint8_t mac[6];
};

typedef struct {
    rnm_base_msg_t hdr;
    uint16_t num;
    struct wifi_sta_info sta[];
} rnm_ap_sta_list_ind_msg_t;

//// Monitor
typedef struct {
    rnm_base_msg_t hdr;
    uint16_t channel;
} rnm_monitor_set_channel_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t channel;
} rnm_monitor_channel_ind_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t en;
} rnm_set_lpm_mode_msg_t;

//// OTA
typedef struct {
    uint32_t version_num;
} bflb_ota_dev_version_t;

typedef struct {
    rnm_base_msg_t hdr;
    bflb_ota_dev_version_t version;
} rnm_dev_version_ind_msg_t;

typedef enum {
    BFLB_OTA_NONE,
    BFLB_OTA_START_ACK,
    BFLB_OTA_ABORT_ACK,
    BFLB_OTA_PROGRAM_ACK,
    BFLB_OTA_FIN_ACK,
    BFLB_OTA_COMMIT_ACK,
    BFLB_OTA_ERROR,
} bflb_ota_status_type_t;

#define RNM_OTA_PART_SIZE 1024

typedef enum {
    BFLB_OTA_ERROR_STATUS_INCORRECT = 8888,
    BFLB_OTA_ERROR_NO_ALT_PARTITION,
    BFLB_OTA_ERROR_MALFORMED_HEADER,
    BFLB_OTA_ERROR_HEADER_ADV_LEN_TOO_BIG,
    BFLB_OTA_ERROR_CKSUM_MISMATCH,
    BFLB_OTA_ERROR_PART_MISSING,
    BFLB_OTA_ERROR_SIZE_NOT_EXPECTED,
    BFLB_OTA_ERROR_MISC,
} bflb_ota_error_code_t;

typedef enum {
    BFLB_OTA_MSG_START,
    BFLB_OTA_MSG_ABORT,
    BFLB_OTA_MSG_PROGRAM_PART,
    BFLB_OTA_MSG_COMMIT,
    BFLB_OTA_MSG_STATUS,
} bflb_ota_msg_type_t;

struct bf1b_ota_status {
    uint16_t status_type;
    union {
        struct {
            uint32_t offset;
        } program_ack;
        struct {
            uint16_t code;
        } error;
    };
};

typedef struct {
    rnm_base_msg_t hdr;
    uint16_t msg_type;
    union {
        struct {
            uint32_t offset;
            uint32_t data_len;
            uint8_t data[];
        } program_part;
        struct bf1b_ota_status status;
    };
} rnm_ota_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t payload[];
} rnm_user_ext_msg_t;

/* all members are in little-endian */
typedef struct {
    rnm_base_msg_t hdr;
    /* local version */
    uint8_t minor_version;
    uint8_t major_version;
    /* reserved for future extensions */
    uint8_t reserved[6];
} rnm_handshake_v2_msg_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
