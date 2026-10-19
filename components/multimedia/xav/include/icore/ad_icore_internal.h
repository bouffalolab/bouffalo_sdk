/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AD_ICORE_INTERNAL_H__
#define __AD_ICORE_INTERNAL_H__

#include "avutil/sf.h"
#include "avutil/avframe.h"
#include "avutil/avpacket.h"
#include "icore/icore_msg.h"
#include "ad_icore_typedef.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define AD_ICORE_AP_IDX          (0)
#define AD_ICORE_CP_IDX          (1)
#else
#define AD_ICORE_AP_IDX          (1)
#define AD_ICORE_CP_IDX          (2)
#endif
#define IPC_CMD_AD_ICORE  (55)
#define AD_ICORE_IPC_SERIVCE_ID 0x10

enum {
    ICORE_CMD_AD_INVALID,
    ICORE_CMD_AD_OPEN,
    ICORE_CMD_AD_DECODE,
    ICORE_CMD_AD_RESET,
    ICORE_CMD_AD_CLOSE,
};

typedef struct {
    avcodec_id_t  id;
    adi_conf_t    adi_cnf;      // req

    void          *ad;          // resp: ad_cls_t
    sf_t          sf;           // resp:
} ad_icore_open_t;

typedef struct {
    void          *ad;          // ad_cls_t
} ad_icore_reset_t;

typedef struct {
    void          *ad;          // ad_cls_t
} ad_icore_close_t;

typedef struct {
    void              *ad;      // ad_cls_t
    uint8_t           *es_data;
    int32_t           es_len;
    avframe_t         frame;
    int               got_frame;
} ad_icore_decode_t;

__END_DECLS__

#endif /* __AD_ICORE_INTERNAL_H__ */

