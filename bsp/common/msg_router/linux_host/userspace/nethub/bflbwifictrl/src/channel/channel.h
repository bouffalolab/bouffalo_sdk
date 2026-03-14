/**
 * @file channel.h
 * @brief Unified channel interface for bflbwifid
 * @details This file defines the abstract interface for communication channels.
 *          Two channel implementations are available:
 *          - TTY: Traditional serial device communication (/dev/ttyAT0)
 *          - Virtualchannel: Netlink-based virtual channel for AT commands
 */

#ifndef _BFLBWIFI_CHANNEL_H
#define _BFLBWIFI_CHANNEL_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== Channel Type Definitions ==================== */

/**
 * @brief Channel backend type
 */
typedef uint8_t channel_type_t;

#define CHANNEL_TYPE_NONE  ((channel_type_t)0u) /**< Uninitialized channel backend */
#define CHANNEL_TYPE_TTY   ((channel_type_t)1u) /**< TTY channel (serial device) */
#define CHANNEL_TYPE_VCHAN ((channel_type_t)2u) /**< Virtualchannel (netlink) */

/**
 * @brief Channel state
 */
typedef uint8_t channel_state_t;

#define CHANNEL_STATE_DISCONNECTED ((channel_state_t)0u) /**< Channel not connected */
#define CHANNEL_STATE_CONNECTED    ((channel_state_t)1u) /**< Channel connected and ready */

/**
 * @brief Command response status
 */
typedef uint8_t channel_cmd_resp_t;

#define CHANNEL_CMD_RESP_PENDING ((channel_cmd_resp_t)0u) /**< Command waiting for response */
#define CHANNEL_CMD_RESP_OK      ((channel_cmd_resp_t)1u) /**< Command succeeded (OK) */
#define CHANNEL_CMD_RESP_ERROR   ((channel_cmd_resp_t)2u) /**< Command failed (ERROR) */
#define CHANNEL_CMD_RESP_TIMEOUT ((channel_cmd_resp_t)3u) /**< Command timed out */

/* Backend capability flags */
#define CHANNEL_CAP_LINK_EVENTS   (1u << 0) /**< Supports disconnect/reconnect events */
#define CHANNEL_CAP_RX_THREAD_CTL (1u << 1) /**< Supports pausing/resuming RX processing */
#define CHANNEL_CAP_RAW_FD        (1u << 2) /**< Exposes a raw file descriptor */

/* ==================== Callback Definitions ==================== */

/**
 * @brief Data received callback
 * @param line Received data line (null-terminated string)
 * @param user_data User data pointer
 */
typedef void (*channel_data_callback_t)(const char *line, void *user_data);

/**
 * @brief Disconnect event callback
 */
typedef void (*channel_disconnect_callback_t)(void);

/**
 * @brief Reconnect event callback
 */
typedef void (*channel_reconnect_callback_t)(void);

/* ==================== Channel Operations Structure ==================== */

/**
 * @brief Channel operations structure
 * @details All channel implementations must provide these operations
 */
typedef struct channel_ops {
    channel_type_t type;
    const char *name;
    uint32_t caps;

    /**
     * @brief Initialize the channel
     * @param dev Device path (e.g., "/dev/ttyAT0") or NULL for Virtualchannel
     * @param baudrate Baudrate (TTY only, ignored for Virtualchannel)
     * @param callback Data receive callback
     * @param user_data User data for callback
     * @return 0 on success, negative on error
     */
    int (*init)(const char *dev, int baudrate, channel_data_callback_t callback, void *user_data);

    /**
     * @brief Deinitialize the channel
     */
    void (*deinit)(void);

    /**
     * @brief Send raw data to channel
     * @param data Data buffer
     * @param len Data length
     * @return Number of bytes sent, negative on error
     */
    int (*send)(const char *data, int len);

    /**
     * @brief Send AT command (adds \r\n automatically)
     * @param cmd Command string (without \r\n)
     * @return Number of bytes sent, negative on error
     */
    int (*send_command)(const char *cmd);

    /**
     * @brief Send AT command and wait for response
     * @param cmd Command string (without \r\n)
     * @param timeout_ms Timeout in milliseconds
     * @return CHANNEL_CMD_RESP_OK, CHANNEL_CMD_RESP_ERROR, or CHANNEL_CMD_RESP_TIMEOUT
     */
    int (*send_command_sync)(const char *cmd, int timeout_ms);

    /**
     * @brief Wait for specific string in response
     * @param expected Expected string
     * @param timeout_ms Timeout in milliseconds
     * @return 0 on success, negative on error
     */
    int (*wait_for)(const char *expected, int timeout_ms);

    /**
     * @brief Get last response line
     * @param buf Buffer to store response
     * @param len Buffer size
     * @return 0 on success, negative on error
     */
    int (*get_last_response)(char *buf, size_t len);

    /**
     * @brief Clear last response buffer
     */
    void (*clear_last_response)(void);

    /**
     * @brief Get current channel state
     * @return Channel state
     */
    channel_state_t (*get_state)(void);

    /**
     * @brief Set disconnect callback
     * @param callback Callback function
     */
    void (*set_disconnect_callback)(channel_disconnect_callback_t callback);

    /**
     * @brief Set reconnect callback
     * @param callback Callback function
     */
    void (*set_reconnect_callback)(channel_reconnect_callback_t callback);

    /**
     * @brief Suspend receive thread (for OTA operations)
     * @return 0 on success, negative on error
     * @note Not applicable for Virtualchannel
     */
    int (*suspend_recv_thread)(void);

    /**
     * @brief Resume receive thread
     * @return 0 on success, negative on error
     * @note Not applicable for Virtualchannel
     */
    int (*resume_recv_thread)(void);

    /**
     * @brief Get channel file descriptor
     * @return File descriptor, negative on error
     * @note Not available for Virtualchannel
     */
    int (*get_fd)(void);

} channel_ops_t;

typedef struct {
    channel_type_t type;
    const channel_ops_t *ops;
} channel_backend_t;

/* ==================== Channel Management API ==================== */
static inline int parse_trim_crlf(char *input, int len)
{
    if (!input || len <= 0) {
        if (input)
            *input = '\0';
        return 0;
    }

    char *start = input;
    char *end = input + len - 1;

    while (start <= end && (*start == '\r' || *start == '\n')) {
        start++;
    }

    while (end >= start && (*end == '\r' || *end == '\n')) {
        end--;
    }

    int valid_len = (end >= start) ? (end - start + 1) : 0;

    if (start != input) {
        memmove(input, start, valid_len);
    }
    input[valid_len] = '\0';

    return valid_len;
}

/**
 * @brief Get channel operations structure (runtime selected)
 * @return Pointer to channel operations structure
 */
const channel_ops_t *channel_get_ops(channel_type_t type);

/**
 * @brief Validate channel type
 * @param type Channel type
 * @return true if valid, false otherwise
 */
bool channel_type_is_valid(channel_type_t type);

/**
 * @brief Get channel type name
 * @param type Channel type
 * @return Channel type string ("TTY" or "Virtualchannel")
 */
const char *channel_get_type_name(channel_type_t type);

static inline bool channel_has_capability(const channel_ops_t *ops, uint32_t cap)
{
    return ops != NULL && (ops->caps & cap) != 0u;
}

void channel_backend_reset(channel_backend_t *backend);
bool channel_backend_is_valid(const channel_backend_t *backend);
channel_type_t channel_backend_type(const channel_backend_t *backend);
const char *channel_backend_name(const channel_backend_t *backend);
bool channel_backend_has_capability(const channel_backend_t *backend, uint32_t cap);
bool channel_backend_supports_ota(const channel_backend_t *backend);
int channel_backend_init(channel_backend_t *backend,
                         channel_type_t type,
                         const char *dev,
                         int baudrate,
                         channel_data_callback_t callback,
                         void *user_data);
void channel_backend_deinit(channel_backend_t *backend);
channel_state_t channel_backend_get_state(const channel_backend_t *backend);
void channel_backend_set_disconnect_callback(const channel_backend_t *backend,
                                             channel_disconnect_callback_t callback);
void channel_backend_set_reconnect_callback(const channel_backend_t *backend,
                                            channel_reconnect_callback_t callback);
int channel_backend_send(const channel_backend_t *backend, const char *data, int len);
int channel_backend_send_command(const channel_backend_t *backend, const char *cmd);
int channel_backend_send_command_sync(const channel_backend_t *backend, const char *cmd, int timeout_ms);
int channel_backend_wait_for(const channel_backend_t *backend, const char *expected, int timeout_ms);
int channel_backend_get_last_response(const channel_backend_t *backend, char *buf, size_t len);
void channel_backend_clear_last_response(const channel_backend_t *backend);
int channel_backend_suspend_recv_thread(const channel_backend_t *backend);
int channel_backend_resume_recv_thread(const channel_backend_t *backend);
int channel_backend_get_fd(const channel_backend_t *backend);

#ifdef __cplusplus
}
#endif

#endif /* _BFLBWIFI_CHANNEL_H */
