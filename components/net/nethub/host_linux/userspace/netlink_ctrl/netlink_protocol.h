/**
 * @file netlink_protocol.h
 * @brief Netlink protocol definitions (data types and message header)
 */

#ifndef __NETLINK_PROTOCOL_H__
#define __NETLINK_PROTOCOL_H__

#include <stdint.h>

/* ==================== Data Type Enumeration ==================== */

/**
 * @enum netlink_data_type
 * @brief Application layer classification for Netlink transparent data
 */
enum netlink_data_type {
    NETLINK_DATA_TYPE_SYSTEM = 0x01,  /* System messages */
    NETLINK_DATA_TYPE_WIFI   = 0x02,  /* WiFi related data */
    NETLINK_DATA_TYPE_BLE    = 0x03,  /* BLE related data */
    NETLINK_DATA_TYPE_USER   = 0x04,  /* User data (default) */
    NETLINK_DATA_TYPE_LOG    = 0x05,  /* Log data */
    NETLINK_DATA_TYPE_MAX    = 0x10,  /* Maximum supported data types (for callback array) */
};

/* ==================== Message Header Structure ==================== */

/**
 * @struct netlink_data_hdr
 * @brief Application layer message header for Netlink payload
 *
 * Layout:
 * +----------+------------------+--------+
 * | data_type| reserved[3]      | data[] |
 * | 1 byte   | 3 bytes          | N bytes|
 * +----------+------------------+--------+
 */
#pragma pack(push, 1)
struct netlink_data_hdr {
    uint8_t  data_type;     /* Data type (see netlink_data_type enum) */
    uint8_t  reserved[3];   /* Reserved expansion field (initialized to 0) */
    uint8_t  data[];        /* Actual user data (flexible array) */
} __attribute__((packed));
#pragma pack(pop)

/* ==================== Constant Definitions ==================== */

#define NETLINK_DATA_HDR_LEN   (4)  /* sizeof(data_type) + sizeof(reserved) */

/* ==================== Helper Functions ==================== */

/**
 * @brief Convert data type to string
 * @param[in] data_type Data type value
 * @return String description
 */
static inline const char* netlink_data_type_to_str(uint8_t data_type)
{
    switch (data_type) {
        case NETLINK_DATA_TYPE_SYSTEM: return "SYSTEM";
        case NETLINK_DATA_TYPE_WIFI:   return "WiFi";
        case NETLINK_DATA_TYPE_BLE:    return "BLE";
        case NETLINK_DATA_TYPE_USER:   return "USER";
        case NETLINK_DATA_TYPE_LOG:    return "LOG";
        default:                       return "UNKNOWN";
    }
}

#endif /* __NETLINK_PROTOCOL_H__ */
