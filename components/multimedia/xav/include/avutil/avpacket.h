/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AV_PACKET_H__
#define __AV_PACKET_H__

#include "avutil/common.h"
#ifdef __linux__
#include "xutils/list.h"
#endif

__BEGIN_DECLS__

typedef struct avpacket {
    uint8_t                           *data;
    int32_t                           size;      ///< total capcity
    int32_t                           len;       ///< length of avilable data
    int64_t                           pts;
    msp_slist_t                           node;
} avpacket_t;

/**
 * @brief  init the packet
 * @param  [in] pkt
 * @return 0/-1
 */
int avpacket_init(avpacket_t *pkt);

/**
 * @brief  malloc resource of the packet
 * @param  [in] pkt
 * @param  [in] size
 * @return 0/-1
 */
int avpacket_new(avpacket_t *pkt, size_t size);

/**
 * @brief  grow the packet data size
 * @param  [in] pkt
 * @param  [in] size
 * @return 0/-1
 */
int avpacket_grow(avpacket_t *pkt, size_t size);

/**
 * @brief  copy the src pkt to dsp pkt
 * @param  [in] spkt : src pkt
 * @param  [in] dpkt : dst pkt
 * @return 0/-1
 */
int avpacket_copy(const avpacket_t *spkt, avpacket_t *dpkt);

/**
 * @brief  free resource of the packet
 * @param  [in] pkt
 * @return
 */
int avpacket_free(avpacket_t *pkt);

__END_DECLS__

#endif /* __AV_PACKET_H__ */

