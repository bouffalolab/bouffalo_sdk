/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFILTER_CLS_H__
#define __AVFILTER_CLS_H__

#include "avutil/av_typedef.h"
#include "avutil/avpacket.h"
#include "avutil/avframe.h"

__BEGIN_DECLS__

typedef struct avfilter       avfilter_t;
typedef struct avfilter_ops   avfilter_ops_t;

struct avfilter_ops {
    const char                *name;
    avmedia_type_t            type;

    int      (*init)          (avfilter_t *avf);
    int      (*control)       (avfilter_t *avf, int cmd, void *arg, size_t *arg_size);
    int      (*filter_frame)  (avfilter_t *avf, const avframe_t *in, avframe_t *out);
    int      (*uninit)        (avfilter_t *avf);
};

#define AVF_IS_SRC(avf)       ((avf)->prev == NULL)
#define AVF_IS_SINK(avf)      ((avf)->next == NULL)

struct avfilter {
    uint8_t                   eof;
    uint8_t                   bypass;
    msp_mutex_t               lock;
    char                      *inst_name; ///< name of the avfilter instance
    avframe_t                 *oframe;     ///< used for sync filtering

    void                      *dobj;      ///< derived obj
    const struct avfilter_ops *ops;
    struct avfilter           *prev;
    struct avfilter           *next;
};

__END_DECLS__

#endif /* __AVFILTER_CLS_H__ */

