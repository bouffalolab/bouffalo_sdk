/**
 * @file bflbwifi.h
 * @brief BFLB WiFi module external API interface
 *
 * Usage flow:
 * 1. bflbwifi_init() - Initialize the module
 * 2. Call various APIs for operations
 * 3. bflbwifi_deinit() - Clean up resources
 */

#ifndef BFLBWIFI_H
#define BFLBWIFI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== Data Structure Definitions ========== */

/**
 * @brief WiFi Station state
 */
typedef enum {
    WIFI_STATE_IDLE = 0,         /* Not connected */
    WIFI_STATE_CONNECTING,       /* Connecting */
    WIFI_STATE_CONNECTED,        /* Connected to AP (but no IP obtained) */
    WIFI_STATE_GOTIP,           /* Connected and obtained IP */
    WIFI_STATE_DISCONNECTED,    /* Disconnected */
    WIFI_STATE_ERROR,           /* Error state */
} bflbwifi_sta_state_t;

/**
 * @brief WiFi encryption mode
 */
typedef enum {
    WIFI_ENC_OPEN = 0,       /* Open (no encryption) */
    WIFI_ENC_WEP = 1,        /* WEP encryption */
    WIFI_ENC_WPA = 2,        /* WPA_PSK */
    WIFI_ENC_WPA2 = 3,       /* WPA2_PSK */
    WIFI_ENC_WPA_WPA2 = 4,   /* WPA/WPA2 mixed mode */
    WIFI_ENC_WPA2_ENTERPRISE = 5, /* WPA2 Enterprise */
    WIFI_ENC_WPA3 = 6,       /* WPA3_PSK */
    WIFI_ENC_WPA2_WPA3 = 7,  /* WPA2/WPA3 mixed mode */
} bflbwifi_enc_t;

/**
 * @brief Station connection configuration
 */
typedef struct {
    char ssid[32 + 1];       /* SSID */
    char pwd[64 + 1];        /* Password (8-64 bytes) */
} bflbwifi_sta_config_t;

/**
 * @brief SoftAP configuration
 */
typedef struct {
    char ssid[32 + 1];       /* SSID */
    char pwd[64 + 1];        /* Password (8-64 bytes) */
    uint8_t channel;         /* Channel (1-14) */
    bflbwifi_enc_t enc;      /* Encryption mode (0-OPEN, 2-WPA, 3-WPA2, 4-WPA3) */
    uint8_t max_conn;        /* Maximum connections (1-4) */
    uint8_t ssid_hidden;     /* Whether to hide SSID */
} bflbwifi_ap_config_t;

/**
 * @brief Scanned AP information
 */
typedef struct {
    char ssid[32 + 1];       /* SSID */
    char bssid[18];          /* BSSID (MAC address) */
    uint8_t channel;         /* Channel */
    int8_t rssi;             /* Signal strength (dBm) */
    bflbwifi_enc_t enc;      /* Encryption mode */
} bflbwifi_ap_info_t;

/**
 * @brief Station connection information (internal use, contains complete information)
 */
typedef struct {
    char ssid[32 + 1];       /* Current connected SSID */
    char bssid[18];          /* Current connected BSSID */
    uint8_t channel;         /* Current channel */
    int8_t rssi;             /* Current signal strength */
    char ip[16];             /* IP address */
    char gateway[16];        /* Gateway */
    char netmask[16];        /* Subnet mask */
    char dns[16];            /* DNS server */
    bflbwifi_enc_t enc;      /* Encryption mode */
} bflbwifi_sta_connection_info_t;

/**
 * @brief Connected station information in AP mode (for API)
 */
typedef struct {
    char mac[18];            /* Station MAC address */
    char ip[16];             /* Station IP address */
} bflbwifi_sta_info_t;

/* ========== Error Code Definitions ========== */

/* Error code list (macro definitions, must be before enum) */
#define BFLBWIFI_ERRORS \
    X(E_SUCCESS, 0, "Success") \
    X(E_ERR, -1, "General error") \
    X(E_ERR_TTY, -2, "TTY open/communication error") \
    X(E_ERR_TIMEOUT, -3, "Operation timeout") \
    X(E_ERR_INVALID_PARAM, -4, "Invalid parameter") \
    X(E_ERR_NO_MEMORY, -5, "Out of memory") \
    X(E_ERR_CONNECT_FAILED, -6, "Connection failed") \
    X(E_ERR_WRONG_PASSWORD, -7, "Wrong password") \
    X(E_ERR_NO_AP_FOUND, -8, "AP not found") \
    X(E_ERR_NOT_INITIALIZED, -9, "Module not initialized") \
    X(E_ERR_NOT_SUPPORTED, -10, "Operation not supported") \
    X(E_ERR_BUFFER_TOO_SMALL, -11, "Buffer too small") \
    X(E_ERR_BUSY, -12, "Device busy")

/**
 * @brief Error codes
 */
typedef enum {
#define X(name, val, desc) name = val,
    BFLBWIFI_ERRORS
#undef X
} bflbwifi_error_t;

/**
 * @brief Get error description
 * @param err Error code
 * @return Error description string
 */
const char* bflbwifi_strerror(int err);

/* ========== Initialization and Cleanup ========== */

/**
 * @brief Initialize BFLB WiFi module
 * @param tty_dev TTY device path (e.g. "/dev/ttyUSB0")
 * @param baudrate Baud rate (e.g. 115200)
 * @return 0 for success, negative error code for failure
 *
 * @note This function will open TTY device and start receive thread
 */
int bflbwifi_init(const char *tty_dev, int baudrate);

/**
 * @brief Clean up BFLB WiFi module
 * @note This function will close TTY and stop receive thread
 */
int bflbwifi_deinit(void);

/**
 * @brief Restart WiFi module
 * @return 0 for success, negative error code for failure
 *
 * @note This function sends AT+RST command to restart the WiFi module
 */
int bflbwifi_restart(void);

/**
 * @brief OTA firmware upgrade
 * @param filepath Path to OTA binary file
 * @return 0 for success, negative error code for failure
 *
 * @note This function blocks until OTA completes
 * @note Device will reboot after successful OTA
 * @note Other commands will be rejected during OTA
 */
int bflbwifi_ota_upgrade(const char *filepath);

/**
 * @brief Check if OTA is in progress
 * @return true if OTA in progress, false otherwise
 */
bool bflbwifi_is_ota_in_progress(void);

/**
 * @brief Get firmware version information
 * @param buf Output buffer
 * @param len Buffer size
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_get_version(char *buf, size_t len);

/* ========== Station Mode API ========== */

/**
 * @brief Connect to AP
 * @param config Station configuration parameters
 * @param timeout_ms Timeout in milliseconds, 0 means using default 30 seconds
 * @return 0 for success, negative error code for failure
 *
 * @note This function will block until connection succeeds or fails
 * @note After successful connection, status can be obtained via bflbwifi_sta_get_state()
 */
int bflbwifi_sta_connect(bflbwifi_sta_config_t *config, int timeout_ms);

/**
 * @brief Disconnect from AP
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_sta_disconnect(void);

/**
 * @brief Query current WiFi status
 * @param state Output parameter
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_sta_get_state(bflbwifi_sta_state_t *state);

/**
 * @brief Scan nearby APs
 * @param ap_list Output buffer (allocated by caller)
 * @param max_count Buffer size (maximum number of APs)
 * @param actual_count Output parameter: actual number of scanned APs
 * @param timeout_ms Timeout in milliseconds, 0 means using default 30 seconds
 * @return 0 for success, negative error code for failure
 *
 * @note If actual_count > max_count, result is truncated, returns E_ERR_BUFFER_TOO_SMALL
 */
int bflbwifi_scan(bflbwifi_ap_info_t *ap_list, int max_count, int *actual_count, int timeout_ms);

/* ========== AP Mode API ========== */

/**
 * @brief Start SoftAP
 * @param config AP configuration parameters
 * @param timeout_ms Timeout in milliseconds, 0 means using default 30 seconds
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_ap_start(bflbwifi_ap_config_t *config, int timeout_ms);

/**
 * @brief Stop SoftAP
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_ap_stop(void);

/**
 * @brief Configure AP DHCP server IP address range
 * @param start_ip Start IP (e.g. "192.168.4.10")
 * @param end_ip End IP (e.g. "192.168.4.20")
 * @param lease_min Lease time (in minutes)
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_ap_set_dhcp_range(const char *start_ip, const char *end_ip, int lease_min);

/**
 * @brief Query list of stations connected to SoftAP
 * @param sta_list Output buffer (allocated by caller)
 * @param max_count Buffer size
 * @param actual_count Output parameter: actual number of stations
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_ap_get_sta_list(bflbwifi_sta_info_t *sta_list, int max_count, int *actual_count);

/**
 * @brief Disconnect specified station
 * @param mac Station MAC address
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_ap_disconnect_sta(const char *mac);

/* ========== Internal API (for implementation, not exposed externally) ========== */

/**
 * @brief WiFi working mode (internal use)
 */
typedef enum {
    WIFI_MODE_IDLE = 0,      /* Idle mode: WiFi RF off */
    WIFI_MODE_STA,           /* Station mode: connect to AP as client */
    WIFI_MODE_AP,            /* SoftAP mode: act as hotspot */
    WIFI_MODE_STA_AP,        /* AP+STA mixed mode */
} bflbwifi_mode_t;

/**
 * @brief Set WiFi working mode (internal API)
 * @param mode Working mode
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_set_mode(bflbwifi_mode_t mode);

/**
 * @brief Query current WiFi working mode (internal API)
 * @param mode Output parameter
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_get_mode(bflbwifi_mode_t *mode);

/**
 * @brief Query Station connection information (internal API)
 * @param info Output parameter
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_get_sta_connection_info(bflbwifi_sta_connection_info_t *info);

/**
 * @brief Configure Station DHCP client (internal API)
 * @param enable true=enable DHCP, false=disable (use static IP)
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_set_station_dhcpc(bool enable);

/**
 * @brief Set static IP address (internal API)
 * @param ip IP address (e.g. "192.168.1.100")
 * @param gateway Gateway (e.g. "192.168.1.1")
 * @param netmask Subnet mask (e.g. "255.255.255.0")
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_set_static_ip(const char *ip, const char *gateway, const char *netmask);

/**
 * @brief Configure SoftAP parameters (internal API)
 * @param config AP configuration
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_ap_config(const bflbwifi_ap_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* BFLBWIFI_H */
