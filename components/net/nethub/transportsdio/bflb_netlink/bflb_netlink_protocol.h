/**
 * @file bflb_netlink_protocol.h
 * @brief Netlink Protocol Definitions (Device Side)
 * @details This file defines the Netlink protocol data structures and constants
 *          used for communication between device and host. Must be kept in sync
 *          with userspace side's netlink_protocol.h.
 *
 * @note Protocol Version: 1.0
 * @note Alignment: 1-byte packing (no padding)
 * @note Endianness: Little-endian (Bouffalo SDK default)
 */

#ifndef __BFLB_NETLINK_PROTOCOL_H__
#define __BFLB_NETLINK_PROTOCOL_H__

#include "bflb_core.h"

/* ==================== Data Type Enumeration ==================== */

/**
 * @enum bflb_netlink_data_type
 * @brief Netlink application layer data type classification
 * @details Defines different data categories for multiplexing/demultiplexing
 *          application layer data over the Netlink transport.
 * @note Must be kept in sync with userspace side's netlink_data_type
 *
 * | Type    | Value | Description              |
 * |---------|-------|--------------------------|
 * | SYSTEM  | 0x01  | System control messages  |
 * | WIFI    | 0x02  | WiFi protocol data       |
 * | BLE     | 0x03  | BLE protocol data        |
 * | USER    | 0x04  | User application data    |
 * | LOG     | 0x05  | Debug/log messages       |
 */
enum bflb_netlink_data_type {
    BFLB_NETLINK_DATA_TYPE_SYSTEM = 0x01,  /**< System control messages */
    BFLB_NETLINK_DATA_TYPE_WIFI   = 0x02,  /**< WiFi protocol data */
    BFLB_NETLINK_DATA_TYPE_BLE    = 0x03,  /**< BLE protocol data */
    BFLB_NETLINK_DATA_TYPE_USER   = 0x04,  /**< User application data (default) */
    BFLB_NETLINK_DATA_TYPE_LOG    = 0x05,  /**< Debug/log messages */
    BFLB_NETLINK_DATA_TYPE_MAX    = 0x10,  /**< Maximum number of data types (for callback array size) */
};

/* ==================== Message Header Structure ==================== */

/**
 * @struct bflb_netlink_data_hdr
 * @brief Netlink payload application layer message header
 * @details This header is prepended to user data in the Netlink payload.
 *          Must be kept in sync with userspace side's netlink_data_hdr.
 *
 * **Memory Layout:**
 * ```
 * +--------------+------------------+------------------+
 * | data_type    | reserved[3]      | data[]           |
 * | 1 byte       | 3 bytes          | N bytes          |
 * +--------------+------------------+------------------+
 * | Offset 0     | Offset 1-3       | Offset 4+        |
 * +--------------+------------------+------------------+
 * ```
 *
 * **Usage Example:**
 * @code
 * // Prepare data with type header
 * struct bflb_netlink_data_hdr *hdr;
 * uint8_t buffer[sizeof(*hdr) + data_len];
 *
 * hdr = (struct bflb_netlink_data_hdr *)buffer;
 * hdr->data_type = BFLB_NETLINK_DATA_TYPE_WIFI;
 * memset(hdr->reserved, 0, sizeof(hdr->reserved));
 * memcpy(hdr->data, user_data, data_len);
 * @endcode
 *
 * **Note:** The `reserved` field must be initialized to 0 for future compatibility.
 */
#pragma pack(push, 1)
struct bflb_netlink_data_hdr {
    uint8_t  data_type;     /**< Data type (see bflb_netlink_data_type enum) */
    uint8_t  reserved[3];   /**< Reserved for future use (must be initialized to 0) */
    uint8_t  data[0];       /**< Flexible array for actual user data */
};
#pragma pack(pop)

/* ==================== Constants ==================== */

/**
 * @def BFLB_NETLINK_DATA_HDR_LEN
 * @brief Size of the Netlink data header in bytes
 * @details Calculated as: sizeof(data_type) + sizeof(reserved) = 1 + 3 = 4
 */
#define BFLB_NETLINK_DATA_HDR_LEN  4

/**
 * @def BFLB_NETLINK_USER_DATA_MTU
 * @brief Maximum user data payload size per packet
 * @details This is the application layer MTU, excluding transport headers.
 *          Actual transport MTU may be smaller due to protocol overhead.
 *
 * **MTU Breakdown:**
 * - User data MTU: 1024 bytes
 * - Netlink header: 4 bytes
 * - Netlink packet overhead: ~15 bytes
 * - Message control overhead: ~10 bytes
 * - **Total transport MTU**: ~1053 bytes
 */
#define BFLB_NETLINK_USER_DATA_MTU  1024

/* ==================== Helper Functions ==================== */

/**
 * @brief Convert data type enum to human-readable string
 * @param[in] data_type Data type value from bflb_netlink_data_type enum
 * @return String representation of the data type
 * @retval "SYSTEM"  for BFLB_NETLINK_DATA_TYPE_SYSTEM
 * @retval "WiFi"    for BFLB_NETLINK_DATA_TYPE_WIFI
 * @retval "BLE"     for BFLB_NETLINK_DATA_TYPE_BLE
 * @retval "USER"    for BFLB_NETLINK_DATA_TYPE_USER
 * @retval "LOG"     for BFLB_NETLINK_DATA_TYPE_LOG
 * @retval "UNKNOWN" for any other value
 *
 * @note This function is primarily used for debugging and logging
 */
static inline const char* bflb_netlink_data_type_to_str(uint8_t data_type)
{
    switch (data_type) {
        case BFLB_NETLINK_DATA_TYPE_SYSTEM: return "SYSTEM";
        case BFLB_NETLINK_DATA_TYPE_WIFI:   return "WiFi";
        case BFLB_NETLINK_DATA_TYPE_BLE:    return "BLE";
        case BFLB_NETLINK_DATA_TYPE_USER:   return "USER";
        case BFLB_NETLINK_DATA_TYPE_LOG:    return "LOG";
        default:                            return "UNKNOWN";
    }
}

/* ==================== Validation Macros ==================== */

/**
 * @def BFLB_NETLINK_IS_VALID_DATA_TYPE
 * @brief Check if a data type value is within valid range
 * @param type Data type value to validate
 * @return true if valid, false otherwise
 */
#define BFLB_NETLINK_IS_VALID_DATA_TYPE(type) \
    ((type) >= BFLB_NETLINK_DATA_TYPE_SYSTEM && \
     (type) < BFLB_NETLINK_DATA_TYPE_MAX && \
     (type) <= BFLB_NETLINK_DATA_TYPE_LOG)

#endif /* __BFLB_NETLINK_PROTOCOL_H__ */
