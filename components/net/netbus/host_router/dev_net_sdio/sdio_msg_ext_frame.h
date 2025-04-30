#ifndef SDIO_MSG_EXT_FRAME_H_CXADSUQJ
#define SDIO_MSG_EXT_FRAME_H_CXADSUQJ

#include <stdint.h>
#include "sdiowifi_mgmr.h"
#include "sdiowifi_mgmr_type.h"

enum {
    SDIOWIFI_EXT_FRAME_TYPE_TTY = 0x3300,
};

#pragma pack(push, 1)
typedef struct {
    uint16_t type;
} sdiowifi_ext_frame_common_hdr_t;

enum {
    SDIOWIFI_EXT_TTY_DATA,
    SDIOWIFI_EXT_TTY_BSR,
};

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

void sdiowifi_ext_frame_process(sdiowifi_mgmr_t *sdm, const uint8_t *frame, const uint16_t frame_len);

#endif /* end of include guard: SDIO_MSG_EXT_FRAME_H_CXADSUQJ */
