/**
 * @file bflbwifi.h
 * @brief BFLB WiFi module external API interface
 *
 * Usage flow:
 * 1. bflbwifi_init_ex() or legacy bflbwifi_init() - Initialize the module
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

/* Control backend type definitions */
#define BFLBWIFI_CTRL_BACKEND_NONE  ((uint8_t)0u)
#define BFLBWIFI_CTRL_BACKEND_TTY   ((uint8_t)1u)
#define BFLBWIFI_CTRL_BACKEND_VCHAN ((uint8_t)2u)

#define BFLBWIFI_CTRL_TTY_BAUDRATE_DEFAULT 115200

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
 * @brief Active STA connection information snapshot
 */
typedef struct {
    char ssid[32 + 1];
    char bssid[18];
    uint8_t channel;
    int8_t rssi;
    char ip[16];
    char gateway[16];
    char netmask[16];
    char dns[16];
    bflbwifi_enc_t enc;
} bflbwifi_sta_connection_info_t;

/**
 * @brief Control path configuration
 */
typedef struct {
    uint8_t ctrl_backend;    /* BFLBWIFI_CTRL_BACKEND_* */
    const char *dev_path;    /* TTY device path, ignored by VCHAN backend */
    int baudrate;            /* TTY baudrate, ignored by VCHAN backend */
} bflbwifi_ctrl_config_t;

/**
 * @brief Control backend link state definitions
 */
typedef uint8_t bflbwifi_ctrl_link_state_t;

#define BFLBWIFI_CTRL_LINK_DISCONNECTED ((bflbwifi_ctrl_link_state_t)0u)
#define BFLBWIFI_CTRL_LINK_CONNECTED    ((bflbwifi_ctrl_link_state_t)1u)

/**
 * @brief Control backend runtime status snapshot
 *
 * This structure describes the host-side control transport only. It does not
 * imply WiFi association state on the device.
 */
typedef struct {
    uint8_t ctrl_backend;                     /* BFLBWIFI_CTRL_BACKEND_* */
    bflbwifi_ctrl_link_state_t link_state;    /* BFLBWIFI_CTRL_LINK_* */
    bool link_events_supported;               /* Backend can report disconnect/reconnect */
    bool rx_thread_ctl_supported;             /* Backend supports RX pause/resume */
    bool raw_fd_supported;                    /* Backend exposes a raw fd for OTA */
    bool ota_in_progress;                     /* OTA session is currently active */
} bflbwifi_ctrl_status_t;

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
    X(E_ERR_BUSY, -12, "Device busy") \
    X(E_ERR_BACKEND_DOWN, -13, "Control backend unavailable")

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
 * @brief Initialize a control backend configuration with neutral defaults
 * @param cfg Configuration structure to reset
 */
void bflbwifi_ctrl_config_init(bflbwifi_ctrl_config_t *cfg);

/**
 * @brief Configure a control backend configuration for TTY transport
 * @param cfg Configuration structure
 * @param dev_path TTY device path
 * @param baudrate TTY baudrate, or 0 to use BFLBWIFI_CTRL_TTY_BAUDRATE_DEFAULT
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_ctrl_config_use_tty(bflbwifi_ctrl_config_t *cfg, const char *dev_path, int baudrate);

/**
 * @brief Configure a control backend configuration for virtual channel transport
 * @param cfg Configuration structure
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_ctrl_config_use_vchan(bflbwifi_ctrl_config_t *cfg);

/**
 * @brief Initialize BFLB WiFi module using the legacy TTY shorthand
 * @param tty_dev TTY device path (e.g. "/dev/ttyUSB0")
 * @param baudrate Baud rate, or 0 to use BFLBWIFI_CTRL_TTY_BAUDRATE_DEFAULT
 * @return 0 for success, negative error code for failure
 *
 * @note This function will open TTY device and start receive thread
 * @note Prefer bflbwifi_init_ex() for new integrations
 */
int bflbwifi_init(const char *tty_dev, int baudrate);

/**
 * @brief Initialize BFLB WiFi module with explicit control backend configuration
 * @param cfg Control backend configuration
 * @return 0 for success, negative error code for failure
 *
 * @note Prefer preparing cfg with bflbwifi_ctrl_config_init() and
 *       bflbwifi_ctrl_config_use_tty()/bflbwifi_ctrl_config_use_vchan()
 */
int bflbwifi_init_ex(const bflbwifi_ctrl_config_t *cfg);

/**
 * @brief Get a snapshot of the current control backend runtime status
 * @param status Output status structure
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_get_ctrl_status(bflbwifi_ctrl_status_t *status);

/**
 * @brief Convert control backend identifier to string
 * @param ctrl_backend BFLBWIFI_CTRL_BACKEND_*
 * @return Backend name string
 */
const char *bflbwifi_ctrl_backend_name(uint8_t ctrl_backend);

/**
 * @brief Convert control backend link state to string
 * @param link_state BFLBWIFI_CTRL_LINK_*
 * @return Link state string
 */
const char *bflbwifi_ctrl_link_state_name(bflbwifi_ctrl_link_state_t link_state);

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
 * @brief Get firmware version information
 * @param buf Output buffer
 * @param len Buffer size
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_get_version(char *buf, size_t len);

/* ========== Station Mode API ========== */

/**
 * @brief Connect to an AP using station mode
 * @param config Station configuration parameters
 * @param timeout_ms Timeout in milliseconds, 0 means using default 30 seconds
 * @return 0 for success, negative error code for failure
 *
 * @note This API ensures station mode is selected before connecting
 * @note This API attempts to enable station DHCP client before connecting
 * @note This function will block until connection succeeds or fails
 * @note After successful connection, status can be obtained via
 *       bflbwifi_sta_get_state() and bflbwifi_sta_get_connection_info()
 */
int bflbwifi_sta_connect(const bflbwifi_sta_config_t *config, int timeout_ms);

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
 * @brief Query cached STA connection information
 * @param info Output parameter
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_sta_get_connection_info(bflbwifi_sta_connection_info_t *info);

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
int bflbwifi_ap_start(const bflbwifi_ap_config_t *config, int timeout_ms);

/**
 * @brief Stop SoftAP
 * @return 0 for success, negative error code for failure
 */
int bflbwifi_ap_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* BFLBWIFI_H */
