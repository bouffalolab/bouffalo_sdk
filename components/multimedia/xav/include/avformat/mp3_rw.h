/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MP3_RW_H__
#define __MP3_RW_H__

#include "avutil/sf.h"
#include "avutil/av_typedef.h"

__BEGIN_DECLS__

#define MP3_HDR_LEN           (4)

struct mp3_hdr_info {
    uint8_t                   layer;
    uint8_t                   lsf;
    int                       spf; //sample per frame
    int                       bitrate;
    int                       framesize;
    sf_t                      sf;
};

/**
 * @brief  get mp3 header info
 * @param  [in] buf
 * @param  [in] hinfo
 * @return 0/-1
 */
int mp3_hdr_get(const uint8_t* buf, struct mp3_hdr_info *hinfo);

/**
 * @brief  sync the mp3
 * @param  [in] rcb        : read byte callback
 * @param  [in] opaque     : in param of the callback
 * @param  [in] sync_max   : max of the sync count
 * @param  [in/out] hdr[4] : hdr of the mp3
 * @param  [in/out] hinfo
 * @return -1 on error or read byte count for sync
 */
int mp3_sync(read_bytes_t rcb, void *opaque, size_t sync_max, uint8_t hdr[4], struct mp3_hdr_info *hinfo);

__END_DECLS__

#endif /* __MP3_RW_H__ */

