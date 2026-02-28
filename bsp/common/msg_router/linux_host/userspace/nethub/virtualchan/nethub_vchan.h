/**
 * @file nethub_vchan.h
 * @brief BFLB Virtual Channel - Bouffalo Virtual Channel Interface
 *
 * Provides Netlink-based virtual channel communication mechanism for user space
 * and kernel space data transmission. Supports multiple types: SYSTEM, USER, AT
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

#define NETHUB_VCHAN_DATA_TYPE_USER   0x01  /* AT messages */
#define NETHUB_VCHAN_DATA_TYPE_AT     0x02  /* WiFi related data */
#define NETHUB_VCHAN_DATA_TYPE_SYSTEM 0x03  /* System related data */
#define NETHUB_VCHAN_DATA_TYPE_MAX    0x04  /* Maximum supported data type (for callback array) */

/* ==================== Type Definitions ==================== */

/**
 * @brief Data receive callback function type
 * @param data Pointer to received data
 * @param len Data length
 */
typedef void (*nethub_vchan_recv_callback_t)(const void *data, size_t len);

/* ==================== Core API ==================== */

/**
 * @brief Initialize BFLB virtual channel
 * @return 0 for success, negative for failure
 */
int nethub_vchan_init(void);

/**
 * @brief Clean up BFLB virtual channel
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
