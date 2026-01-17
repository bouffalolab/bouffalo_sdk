/**
 * @file bflbvchan.h
 * @brief BFLB Virtual Channel - Bouffalo Virtual Channel Interface
 *
 * Provides Netlink-based virtual channel communication mechanism for user space
 * and kernel space data transmission. Supports multiple data types: SYSTEM, WIFI,
 * BLE, USER, AT
 */

#ifndef BFLBVCHAN_H
#define BFLBVCHAN_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Message header length */
#define BFLBVCHAN_DATA_HDR_LEN   (4)  /* sizeof(data_type) + sizeof(reserved[3]) */

/* Maximum data length per message */
#define BFLBVCHAN_MAX_DATA_LEN   (1024)

/* ==================== Data Type Definitions ==================== */

#define BFLBVCHAN_DATA_TYPE_SYSTEM  0x01  /* System messages */
#define BFLBVCHAN_DATA_TYPE_WIFI    0x02  /* WiFi related data */
#define BFLBVCHAN_DATA_TYPE_BLE     0x03  /* BLE related data */
#define BFLBVCHAN_DATA_TYPE_USER    0x04  /* User data (default) */
#define BFLBVCHAN_DATA_TYPE_AT      0x05  /* AT data (default) */
#define BFLBVCHAN_DATA_TYPE_MAX     0x06  /* Maximum supported data type (for callback array) */

/* ==================== Type Definitions ==================== */

/**
 * @brief Data receive callback function type
 * @param data Pointer to received data
 * @param len Data length
 */
typedef void (*bflbvchan_recv_callback_t)(const void *data, size_t len);

/* ==================== Core API ==================== */

/**
 * @brief Initialize BFLB virtual channel
 * @return 0 for success, negative for failure
 */
int bflbvchan_init(void);

/**
 * @brief Clean up BFLB virtual channel
 * @return 0 for success, negative for failure
 */
int bflbvchan_deinit(void);

/**
 * @brief Send data to virtual channel
 * @param type Data type (BFLBVCHAN_DATA_TYPE_*)
 * @param data Pointer to data
 * @param len Data length
 * @return 0 for success, negative for failure
 */
int bflbvchan_send(uint8_t type, const void *data, size_t len);

/**
 * @brief Register data receive callback function
 * @param data_type Data type (BFLBVCHAN_DATA_TYPE_*)
 * @param callback Callback function pointer
 * @return 0 for success, negative for failure
 */
int bflbvchan_register_callback(uint8_t data_type, bflbvchan_recv_callback_t callback);

/**
 * @brief Unregister data receive callback function
 * @param data_type Data type (BFLBVCHAN_DATA_TYPE_*)
 * @return 0 for success, negative for failure
 */
int bflbvchan_unregister_callback(uint8_t data_type);

/* ==================== USER Data Type Convenient API ==================== */

/**
 * @brief Send USER type data
 * @param data Pointer to data
 * @param len Data length
 * @return 0 for success, negative for failure
 */
int bflbvchan_user_send(const void *data, size_t len);

/**
 * @brief Register USER type data receive callback
 * @param callback Callback function pointer
 * @return 0 for success, negative for failure
 */
int bflbvchan_user_register_callback(bflbvchan_recv_callback_t callback);

/* ==================== AT Data Type Convenient API ==================== */

/**
 * @brief Send AT type data
 * @param data Pointer to data
 * @param len Data length
 * @return 0 for success, negative for failure
 */
int bflbvchan_at_send(const void *data, size_t len);

/**
 * @brief Register AT type data receive callback
 * @param callback Callback function pointer
 * @return 0 for success, negative for failure
 */
int bflbvchan_at_register_callback(bflbvchan_recv_callback_t callback);

/* ==================== Error Code Definitions ==================== */

#define BFLBVCHAN_OK              0
#define BFLBVCHAN_ERROR           -1
#define BFLBVCHAN_ERROR_INIT      -2
#define BFLBVCHAN_ERROR_NOT_INIT  -3
#define BFLBVCHAN_ERROR_PARAM     -4
#define BFLBVCHAN_ERROR_BUSY      -5
#define BFLBVCHAN_ERROR_NOMEM     -6
#define BFLBVCHAN_ERROR_IO        -7

#ifdef __cplusplus
}
#endif

#endif /* BFLBVCHAN_H */
