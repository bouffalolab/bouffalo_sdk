#ifndef _MSG_CTRL_SDIO_
#define _MSG_CTRL_SDIO_

#include "mr_sdio_drv.h"

#ifdef BL618DG
#define MSG_DNLD_FRAME_CNT (26)
#else
#define MSG_DNLD_FRAME_CNT (16)
#endif

#pragma pack(push, 1)

/* SDIO message download frame format */
typedef struct {
    /* elem and msg backup buff */
    uint8_t frame_elem_copy[FRAME_BUFF_HEADER_ROOM];  /* 32 Byte */
    uint8_t netdev_msg_copy[sizeof(mr_netdev_msg_t)]; /* 8 Byte */

    /* wifi desc buff */
    uint8_t trans_desc[32];                                                  /* 32 Byte */
    uint8_t hw_desc[388 - FRAME_BUFF_HEADER_ROOM - sizeof(mr_netdev_msg_t)]; /* (388 - 32 - 8) Byte */

    /* msg sdio dnld frame frame_elem */
    uint8_t frame_elem[FRAME_BUFF_HEADER_ROOM]; /* 32 Byte */
    /* msg sdio dnld frame netdev_msg */
    uint8_t netdev_msg[sizeof(mr_netdev_msg_t)];            /* 8 Byte */
    uint8_t eth_payload[512 * 3 - sizeof(mr_netdev_msg_t)]; /* (1536-8) Byte */

} msg_sdio_dnld_fmt_t;

#pragma pack(pop)

#define MSG_SDIO_DNLD_FMT_CONTAINER_OF(ptr, member) \
    ((msg_sdio_dnld_fmt_t *)((char *)(ptr) - (uintptr_t)(&((msg_sdio_dnld_fmt_t *)0)->member)))

extern mr_msg_ctrl_priv_t *g_msg_sdio_ctrl;

int msg_ctrl_sdio_init(void);

#endif

