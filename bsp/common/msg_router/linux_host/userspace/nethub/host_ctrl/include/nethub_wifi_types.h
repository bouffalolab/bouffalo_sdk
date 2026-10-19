#ifndef NETHUB_WIFI_TYPES_H
#define NETHUB_WIFI_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NETHUB_WIFI_SSID_MAX_LEN      32
#define NETHUB_WIFI_PASSWORD_MAX_LEN  64
#define NETHUB_WIFI_BSSID_MAX_LEN     18
#define NETHUB_WIFI_IPV4_MAX_LEN      16
#define NETHUB_WIFI_VERSION_MAX_LEN   256
#define NETHUB_WIFI_SCAN_MAX_APS      64

#define NETHUB_WIFI_DEFAULT_SOCKET_PATH "/tmp/bflbwifi.sock"
#define NETHUB_WIFI_DEFAULT_PID_FILE    "/var/run/bflbwifid.pid"
#define NETHUB_WIFI_DEFAULT_LOG_FILE    "/var/log/bflbwifi.log"

typedef struct nethub_wifi_client nethub_wifi_client_t;

typedef enum {
    NETHUB_WIFI_OK = 0,
    NETHUB_WIFI_ERR = -1,
    NETHUB_WIFI_ERR_TIMEOUT = -2,
    NETHUB_WIFI_ERR_INVALID_PARAM = -3,
    NETHUB_WIFI_ERR_NO_MEMORY = -4,
    NETHUB_WIFI_ERR_CONNECT_FAILED = -5,
    NETHUB_WIFI_ERR_WRONG_PASSWORD = -6,
    NETHUB_WIFI_ERR_NO_AP_FOUND = -7,
    NETHUB_WIFI_ERR_NOT_INITIALIZED = -8,
    NETHUB_WIFI_ERR_NOT_SUPPORTED = -9,
    NETHUB_WIFI_ERR_BUFFER_TOO_SMALL = -10,
    NETHUB_WIFI_ERR_BUSY = -11,
    NETHUB_WIFI_ERR_LINK_DOWN = -12,
    NETHUB_WIFI_ERR_PROTOCOL = -13,
} nethub_wifi_error_t;

typedef enum {
    NETHUB_WIFI_STA_IDLE = 0,
    NETHUB_WIFI_STA_CONNECTING,
    NETHUB_WIFI_STA_CONNECTED,
    NETHUB_WIFI_STA_GOT_IP,
    NETHUB_WIFI_STA_DISCONNECTED,
    NETHUB_WIFI_STA_ERROR,
} nethub_wifi_sta_state_t;

typedef enum {
    NETHUB_WIFI_MODE_IDLE = 0,
    NETHUB_WIFI_MODE_STA = 1,
    NETHUB_WIFI_MODE_AP = 2,
    NETHUB_WIFI_MODE_STA_AP = 3,
} nethub_wifi_mode_t;

typedef enum {
    NETHUB_WIFI_ENC_OPEN = 0,
    NETHUB_WIFI_ENC_WEP = 1,
    NETHUB_WIFI_ENC_WPA = 2,
    NETHUB_WIFI_ENC_WPA2 = 3,
    NETHUB_WIFI_ENC_WPA_WPA2 = 4,
    NETHUB_WIFI_ENC_WPA2_ENTERPRISE = 5,
    NETHUB_WIFI_ENC_WPA3 = 6,
    NETHUB_WIFI_ENC_WPA2_WPA3 = 7,
} nethub_wifi_enc_t;

typedef enum {
    NETHUB_WIFI_LINK_DOWN = 0,
    NETHUB_WIFI_LINK_UP = 1,
} nethub_wifi_link_state_t;

typedef struct {
    char ssid[NETHUB_WIFI_SSID_MAX_LEN + 1];
    char password[NETHUB_WIFI_PASSWORD_MAX_LEN + 1];
} nethub_wifi_sta_config_t;

typedef struct {
    char ssid[NETHUB_WIFI_SSID_MAX_LEN + 1];
    char password[NETHUB_WIFI_PASSWORD_MAX_LEN + 1];
    uint8_t channel;
    nethub_wifi_enc_t enc;
    uint8_t max_conn;
    uint8_t ssid_hidden;
} nethub_wifi_ap_config_t;

typedef struct {
    char ssid[NETHUB_WIFI_SSID_MAX_LEN + 1];
    char bssid[NETHUB_WIFI_BSSID_MAX_LEN];
    uint8_t channel;
    int8_t rssi;
    nethub_wifi_enc_t enc;
} nethub_wifi_ap_info_t;

typedef struct {
    nethub_wifi_link_state_t link_state;
    nethub_wifi_sta_state_t sta_state;
    nethub_wifi_mode_t mode;
    char ssid[NETHUB_WIFI_SSID_MAX_LEN + 1];
    char bssid[NETHUB_WIFI_BSSID_MAX_LEN];
    uint8_t channel;
    int8_t rssi;
    char ip[NETHUB_WIFI_IPV4_MAX_LEN];
    char gateway[NETHUB_WIFI_IPV4_MAX_LEN];
    char netmask[NETHUB_WIFI_IPV4_MAX_LEN];
    char dns[NETHUB_WIFI_IPV4_MAX_LEN];
} nethub_wifi_status_t;

#ifdef __cplusplus
}
#endif

#endif /* NETHUB_WIFI_TYPES_H */
