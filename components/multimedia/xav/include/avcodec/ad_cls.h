/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AD_CLS_H__
#define __AD_CLS_H__

#include "avutil/av_typedef.h"
#include "avutil/avpacket.h"
#include "avutil/avframe.h"

__BEGIN_DECLS__

typedef struct ad_cls ad_cls_t;

struct ad_ops {
    const char                *name;
    avcodec_id_t              id;

    int      (*open)          (ad_cls_t *o);
    int      (*decode)        (ad_cls_t *o, avframe_t *frame, int *got_frame, const avpacket_t *pkt);
    int      (*control)       (ad_cls_t *o, int cmd, void *arg, size_t *arg_size);
    int      (*reset)         (ad_cls_t *o);
    int      (*close)         (ad_cls_t *o);
};

struct ad_cls {
    sh_audio_t                ash;
    msp_mutex_t               lock;

    void                      *priv;
    const struct ad_ops       *ops;
};

enum {
    CODEC_CMD_UNKNOWN,
};

__END_DECLS__

#endif /* __AD_CLS_H__ */

