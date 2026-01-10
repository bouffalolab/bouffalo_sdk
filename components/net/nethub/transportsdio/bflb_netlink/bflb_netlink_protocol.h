/**
 * @file bflb_netlink_protocol.h
 * @brief Netlink 协议定义（Device 侧）
 * @details 与 userspace 侧的 netlink_protocol.h 保持一致
 * @author Claude
 * @date 2025-01-08
 */

#ifndef __BFLB_NETLINK_PROTOCOL_H__
#define __BFLB_NETLINK_PROTOCOL_H__

#include "bflb_core.h"

/* ==================== 数据类型枚举 ==================== */

/**
 * @enum bflb_netlink_data_type
 * @brief Netlink 透传数据的应用层分类
 * @note 与 userspace 侧的 netlink_data_type 保持一致
 */
enum bflb_netlink_data_type {
    BFLB_NETLINK_DATA_TYPE_SYSTEM = 0x01,  /* 系统消息 */
    BFLB_NETLINK_DATA_TYPE_WIFI   = 0x02,  /* WiFi 相关数据 */
    BFLB_NETLINK_DATA_TYPE_BLE    = 0x03,  /* BLE 相关数据 */
    BFLB_NETLINK_DATA_TYPE_USER   = 0x04,  /* 用户数据（默认） */
    BFLB_NETLINK_DATA_TYPE_LOG    = 0x05,  /* 日志数据 */
    BFLB_NETLINK_DATA_TYPE_MAX    = 0x10,  /* 支持的最大数据类型数量（用于回调数组） */
};

/* ==================== 消息头结构 ==================== */

/**
 * @struct bflb_netlink_data_hdr
 * @brief Netlink payload 的应用层消息头
 * @details 与 userspace 侧的 netlink_data_hdr 保持一致
 *
 * 布局:
 * +----------+------------------+--------+
 * | data_type| reserved[3]      | data[] |
 * | 1 byte   | 3 bytes          | N bytes|
 * +----------+------------------+--------+
 */
#pragma pack(push, 1)
struct bflb_netlink_data_hdr {
    uint8_t  data_type;     /* 数据类型（见 bflb_netlink_data_type 枚举） */
    uint8_t  reserved[3];   /* 预留扩展字段（初始化为 0） */
    uint8_t  data[0];       /* 实际用户数据（柔性数组） */
};
#pragma pack(pop)

/* ==================== 常量定义 ==================== */

#define BFLB_NETLINK_DATA_HDR_LEN  4  /* sizeof(data_type) + sizeof(reserved) */

/* ==================== 辅助函数 ==================== */

/**
 * @brief 将数据类型转换为字符串
 * @param[in] data_type 数据类型值
 * @return 字符串描述
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

#endif /* __BFLB_NETLINK_PROTOCOL_H__ */
