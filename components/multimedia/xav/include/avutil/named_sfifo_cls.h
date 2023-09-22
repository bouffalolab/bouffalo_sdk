/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __NAMED_SFIFO_CLS_H__
#define __NAMED_SFIFO_CLS_H__

#include <fcntl.h>
#include "avutil/common.h"

__BEGIN_DECLS__

typedef struct named_sfifo {
    char                         *name;               ///< origin name
    size_t                       size;
    struct nsfifo_cls            *cls;
    msp_mutex_t                  lock;
} nsfifo_t;

#define GET_DERIVED_NSFIFO(p) (((char*)(p)) + sizeof(nsfifo_t))

struct nsfifo_ops  {
    int   (*init)                (nsfifo_t *fifo, const char *name, int mode);
    int   (*get_rpos)            (nsfifo_t *fifo, char **pos, uint32_t timeout);
    int   (*set_rpos)            (nsfifo_t* fifo, size_t count);
    int   (*get_wpos)            (nsfifo_t *fifo, char **pos, uint32_t timeout);
    int   (*set_wpos)            (nsfifo_t* fifo, size_t count);
    int   (*set_eof)             (nsfifo_t* fifo, uint8_t reof, uint8_t weof);
    int   (*get_eof)             (nsfifo_t* fifo, uint8_t *reof, uint8_t *weof);
    int   (*reset)               (nsfifo_t *fifo);
    int   (*get_len)             (nsfifo_t *fifo);
    void  (*uninit)              (nsfifo_t *fifo);
};


struct nsfifo_cls  {
    const char                   *name;
    size_t                       priv_size;
    const struct nsfifo_ops      *ops;
};

__END_DECLS__

#endif /* __NAMED_SFIFO_CLS_H__ */

