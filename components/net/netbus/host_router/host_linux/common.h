#ifndef COMMON_H_KCJUM19Y
#define COMMON_H_KCJUM19Y

#ifdef BUILD_USERSPACE
#include <stdint.h>
#else
#include <linux/kernel.h>
#endif

#define DEVICE_NAME "/dev/blctlport"
#define CTL_PORT_MSG_LEN_MAX 4096
#define PACKED_STRUCT struct __attribute__((packed))

enum ctl_port_event {
    CTL_PORT_MSG_CONNECT_REQ,
    CTL_PORT_MSG_CONNECT_RSP,
    CTL_PORT_MSG_DISCONNECT_REQ,
    CTL_PORT_MSG_DISCONNECT_RSP,
    CTL_PORT_MSG_DISCONNECT_ALL_REQ,
    CTL_PORT_MSG_DISCONNECT_ALL_RSP,
    CTL_PORT_MSG_TRANSPARENT_HOST2DEVICE,
    CTL_PORT_MSG_TRANSPARENT_DEVICE2HOST,
    CTL_PORT_MSG_CARD_INSERTED,
    CTL_PORT_MSG_CARD_DEAD,
    CTL_PORT_MSG_IP_UPDATE,
    CTL_PORT_MSG_DISCONNECT,
};

enum {
    CTL_PORT_ERROR_CODE_LENGTH = -100,
    CTL_PORT_ERROR_CODE_UNKNOWN_CMD,
};

#define CTL_PORT_MSG_MAGIC "HOSTRMSG"

typedef PACKED_STRUCT {
    uint8_t magic[8];
    uint16_t type;
    uint16_t length; // including hdr, payload
    uint32_t msg_id;
    uint8_t payload[];
} ctl_port_msg_hdr_t;

PACKED_STRUCT ip_info {
    uint8_t ip4_addr[4];
    uint8_t ip4_mask[4];
    uint8_t ip4_gw[4];
    uint8_t ip4_dns1[4];
    uint8_t ip4_dns2[4];
    uint8_t gw_mac[6];
};

#define CTL_PORT_MSG_HDR_LEN (sizeof(ctl_port_msg_hdr_t))

typedef PACKED_STRUCT {
    ctl_port_msg_hdr_t hdr;
    struct ip_info ip_info;
} ctl_port_msg_ip_update_t;

typedef PACKED_STRUCT {
    ctl_port_msg_hdr_t hdr;
    uint8_t success;
    uint16_t session_id;
} ctl_port_msg_conn_rsp_t;

#endif /* end of include guard: COMMON_H_KCJUM19Y */
