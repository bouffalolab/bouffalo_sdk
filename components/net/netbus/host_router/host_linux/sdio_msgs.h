#ifndef SDIO_MSGS_H_4AGV9QEH
#define SDIO_MSGS_H_4AGV9QEH

#ifdef BUILD_USERSPACE
#include <stdbool.h>
#include <stdint.h>
#else
#include <linux/kernel.h>
#endif

#include "rnm_msg.h"

enum {
    BF1B_SDIO_MSG_TYPE_ETH_FRAME = 0xabcd,
    BF1B_SDIO_MSG_TYPE_CMD       = 0xef01,

    BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_TO_WIFI_TX = 0x2000,
    BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_FROM_WIFI_RX,
    BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_AP_TO_WIFI_TX,
    BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_AP_FROM_WIFI_RX,
    BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_SNIFFER_TO_WIFI_TX,
    BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_SNIFFER_FROM_WIFI_RX,
    BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_EXT, // UART, etc
};

enum {
    SDIOWIFI_EXT_FRAME_TYPE_TTY = 0x3300,
};

enum {
    SDIOWIFI_EXT_TTY_DATA,
    SDIOWIFI_EXT_TTY_BSR,
};

#pragma pack(push, 1)

struct sdio_top_msg {
    uint8_t type_lsb;
    uint8_t type_msb;
    uint8_t len_lsb;            // length of pld, excluding padding
    uint8_t len_msb;
    uint8_t pld_off;
    uint8_t is_amsdu;
    uint8_t has_twin;
    uint8_t subtype_lsb;
    uint8_t subtype_msb;
    uint8_t first_part_len_lsb; // length of pld, excluding padding
    uint8_t first_part_len_msb;
    uint8_t _pad0[1];
    uint8_t pld_w_pad[];
};

struct llc_snap
{
    /// DSAP + SSAP fieldsr
    uint16_t dsap_ssap;
    /// LLC control + OUI byte 0
    uint16_t control_oui0;
    /// OUI bytes 1 and 2
    uint16_t oui1_2;
    /// Protocol
    uint16_t proto_id;
};

struct amsdu_subframe {
    uint8_t da[6];
    uint8_t sa[6];
    uint16_t length;
	struct llc_snap llc;
    uint8_t payload[];
};

struct eth_hdr {
    uint8_t da[6];
    uint8_t sa[6];
    uint16_t proto;
};

typedef struct {
    uint16_t type;
    uint16_t len; // type + len + payload
    uint8_t payload[];
} bl602_sdio_msg_t;

typedef struct {
    uint16_t type;
} sdiowifi_ext_frame_common_hdr_t;

typedef struct {
    sdiowifi_ext_frame_common_hdr_t hdr;
    uint16_t tty_id;
    uint16_t type;
    uint8_t data[];
} sdiowifi_ext_frame_tty_msg_t;

#define SDIOWIFI_EXT_TTY_BSR_INITIAL (1 << 0)

typedef struct {
    sdiowifi_ext_frame_common_hdr_t hdr;
    uint16_t tty_id;
    uint16_t type;
    uint16_t flags;
    uint16_t incr;
} sdiowifi_ext_frame_tty_bsr_t;

#pragma pack(pop)

#endif /* end of include guard: SDIO_MSGS_H_4AGV9QEH */
