#ifndef NETHUB_MSG_CTRL_SDIO_H
#define NETHUB_MSG_CTRL_SDIO_H

#include "mr_sdio_drv.h"

#ifdef BL618DG
#define MSG_DNLD_FRAME_CNT (26)
#else
#define MSG_DNLD_FRAME_CNT (16)
#endif

#pragma pack(push, 1)
typedef struct {
    uint8_t frame_elem_copy[FRAME_BUFF_HEADER_ROOM];
    uint8_t netdev_msg_copy[sizeof(mr_netdev_msg_t)];

    uint8_t trans_desc[32];
    uint8_t hw_desc[388 - FRAME_BUFF_HEADER_ROOM - sizeof(mr_netdev_msg_t)];

    uint8_t frame_elem[FRAME_BUFF_HEADER_ROOM];
    uint8_t netdev_msg[sizeof(mr_netdev_msg_t)];
    uint8_t eth_payload[512 * 3 - sizeof(mr_netdev_msg_t)];
} msg_sdio_dnld_fmt_t;
#pragma pack(pop)

#define MSG_SDIO_DNLD_FMT_CONTAINER_OF(ptr, member) \
    ((msg_sdio_dnld_fmt_t *)((char *)(ptr) - (uintptr_t)(&((msg_sdio_dnld_fmt_t *)0)->member)))

extern mr_msg_ctrl_priv_t *g_msg_sdio_ctrl;

int msg_ctrl_sdio_init(void);
void msg_ctrl_sdio_print_dnld_status(void);

#endif
