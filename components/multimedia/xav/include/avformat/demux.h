/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __DEMUX_H__
#define __DEMUX_H__

#include "avformat/demux_cls.h"

__BEGIN_DECLS__

#define DEMUX_OPS_MAX (16)

/**
 * @brief  regist demux ops
 * @param  [in] ops
 * @return 0/-1
 */
int demux_ops_register(const struct demux_ops *ops);

/**
 * @brief  open/create one demux
 * @param  [in] s : stream
 * @return
 */
demux_cls_t* demux_open(stream_cls_t *s);

/**
 * @brief  read packet from the demux
 * @param  [in] o
 * @param  [in] pkt
 * @return -1 on err
 */
int demux_read_packet(demux_cls_t *o, avpacket_t *pkt);

/**
 * @brief  seek the demux
 * @param  [in] o
 * @param  [in] timestamp : seek time(ms)
 * @return 0/-1
 */
int demux_seek(demux_cls_t *o, uint64_t timestamp);

/**
 * @brief  close/destroy a demux
 * @param  [in] o
 * @return 0/-1
 */
int demux_close(demux_cls_t *o);

/**
 * @brief  control a demux
 * @param  [in] o
 * @param  [in] cmd : command
 * @param  [in] arg
 * @param  [in/out] arg_size
 * @return
 */
int demux_control(demux_cls_t *o, int cmd, void *arg, size_t *arg_size);

__END_DECLS__

#endif /* __DEMUX_H__ */

