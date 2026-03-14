/**
 * @file nethub_vchan.h
 * @brief NetHub virtual channel userspace interface
 *
 * Provides a Netlink-based logical channel between host userspace and the
 * kernel/msg_router stack. Multiple payload types are multiplexed on the same
 * host link, including SYSTEM, USER and AT.
 */

#ifndef NETHUB_VCHAN_H
#define NETHUB_VCHAN_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Message header length */
#define NETHUB_VCHAN_DATA_HDR_LEN   (4)  /* sizeof(data_type) + sizeof(reserved) + sizeof(len) */

/* Maximum data length per message */
#define NETHUB_VCHAN_MAX_DATA_LEN   (1500) /* Must eq with device config */

/* ==================== Data Type Definitions ==================== */

#define NETHUB_VCHAN_DATA_TYPE_USER   0x01  /* User payload data */
#define NETHUB_VCHAN_DATA_TYPE_AT     0x02  /* AT command / response data */
#define NETHUB_VCHAN_DATA_TYPE_SYSTEM 0x03  /* System / internal coordination data */
#define NETHUB_VCHAN_DATA_TYPE_MAX    0x04  /* Maximum supported data type (for callback array) */

/* ==================== Type Definitions ==================== */

/**
 * @brief Data receive callback function type
 * @param data Pointer to received data
 * @param len Data length
 */
typedef void (*nethub_vchan_recv_callback_t)(const void *data, size_t len);

/**
 * @brief Virtual channel host-device link state
 */
typedef uint8_t nethub_vchan_link_state_t;

#define NETHUB_VCHAN_LINK_DOWN ((nethub_vchan_link_state_t)0u)
#define NETHUB_VCHAN_LINK_UP   ((nethub_vchan_link_state_t)1u)

/**
 * @brief Virtual channel host state mirrored from kernel virtualchan state machine
 *
 * Values 0..3 are intentionally kept aligned with MR_VIRTUALCHAN_HSTA_* so the
 * userspace library can report the raw kernel state without translation loss.
 */
typedef uint8_t nethub_vchan_host_state_t;

#define NETHUB_VCHAN_HOST_STATE_ERROR      ((nethub_vchan_host_state_t)0u)
#define NETHUB_VCHAN_HOST_STATE_RESET      ((nethub_vchan_host_state_t)1u)
#define NETHUB_VCHAN_HOST_STATE_HOST_READY ((nethub_vchan_host_state_t)2u)
#define NETHUB_VCHAN_HOST_STATE_DEVICE_RUN ((nethub_vchan_host_state_t)3u)
#define NETHUB_VCHAN_HOST_STATE_UNKNOWN    ((nethub_vchan_host_state_t)0xffu)

/**
 * @brief Virtual channel runtime state snapshot
 */
typedef struct {
    nethub_vchan_link_state_t link_state;
    nethub_vchan_host_state_t host_state;
} nethub_vchan_state_snapshot_t;

/**
 * @brief Link state change callback
 * @param link_state Current link state
 * @param user_data User private pointer
 */
typedef void (*nethub_vchan_link_event_callback_t)(nethub_vchan_link_state_t link_state, void *user_data);

/* ==================== Core API ==================== */

/**
 * @brief Initialize NetHub virtual channel
 * @return 0 for success, negative for failure
 */
int nethub_vchan_init(void);

/**
 * @brief Deinitialize NetHub virtual channel
 * @return 0 for success, negative for failure
 */
int nethub_vchan_deinit(void);

/**
 * @brief Send data to virtual channel
 * @param type Data type (NETHUB_VCHAN_DATA_TYPE_*)
 * @param data Pointer to data
 * @param len Data length
 * @return 0 for success, negative for failure
 */
int nethub_vchan_send(uint8_t type, const void *data, size_t len);

/**
 * @brief Register data receive callback function
 * @param data_type Data type (NETHUB_VCHAN_DATA_TYPE_*)
 * @param callback Callback function pointer
 * @return 0 for success, negative for failure
 */
int nethub_vchan_register_callback(uint8_t data_type, nethub_vchan_recv_callback_t callback);

/**
 * @brief Unregister data receive callback function
 * @param data_type Data type (NETHUB_VCHAN_DATA_TYPE_*)
 * @return 0 for success, negative for failure
 */
int nethub_vchan_unregister_callback(uint8_t data_type);

/**
 * @brief Get current virtual channel runtime snapshot
 * @param snapshot Output snapshot
 * @return 0 for success, negative for failure
 */
int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot);

/**
 * @brief Convert link state to printable string
 * @param link_state Link state
 * @return Constant string
 */
const char *nethub_vchan_link_state_name(nethub_vchan_link_state_t link_state);

/**
 * @brief Convert host state to printable string
 * @param host_state Host state
 * @return Constant string
 */
const char *nethub_vchan_host_state_name(nethub_vchan_host_state_t host_state);

/**
 * @brief Register virtual channel link event callback
 * @param callback Callback function, pass NULL to unregister
 * @param user_data User private pointer
 * @return 0 for success, negative for failure
 */
int nethub_vchan_register_link_event_callback(nethub_vchan_link_event_callback_t callback, void *user_data);

/* ==================== USER Data Type Convenient API ==================== */

/**
 * @brief Send USER type data
 * @param data Pointer to data
 * @param len Data length
 * @return 0 for success, negative for failure
 */
int nethub_vchan_user_send(const void *data, size_t len);

/**
 * @brief Register USER type data receive callback
 * @param callback Callback function pointer
 * @return 0 for success, negative for failure
 */
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);

/* ==================== AT Data Type Convenient API ==================== */

/**
 * @brief Send AT type data
 * @param data Pointer to data
 * @param len Data length
 * @return 0 for success, negative for failure
 */
int nethub_vchan_at_send(const void *data, size_t len);

/**
 * @brief Register AT type data receive callback
 * @param callback Callback function pointer
 * @return 0 for success, negative for failure
 */
int nethub_vchan_at_register_callback(nethub_vchan_recv_callback_t callback);

/* ==================== Error Code Definitions ==================== */

#define NETHUB_VCHAN_OK              0
#define NETHUB_VCHAN_ERROR           -1
#define NETHUB_VCHAN_ERROR_INIT      -2
#define NETHUB_VCHAN_ERROR_NOT_INIT  -3
#define NETHUB_VCHAN_ERROR_PARAM     -4
#define NETHUB_VCHAN_ERROR_BUSY      -5
#define NETHUB_VCHAN_ERROR_NOMEM     -6
#define NETHUB_VCHAN_ERROR_IO        -7

#ifdef __cplusplus
}
#endif

#endif /* NETHUB_VCHAN_H */
