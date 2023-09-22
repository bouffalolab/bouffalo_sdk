/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __FLAC_RW_H__
#define __FLAC_RW_H__

#include "avutil/sf.h"
#include "avutil/av_typedef.h"

__BEGIN_DECLS__

#define FLAC_HDR_SIZE_MIN                       (6)
#define FLAC_HDR_SIZE_MAX                       (16)     ///< largest possible size of flac header
#define FLAC_STREAMINFO_SIZE                    (34)
#define FLAC_FRAME_SIZE_DEFAULT                 (4*1024)

enum {
    FLAC_METADATA_TYPE_STREAMINFO,
    FLAC_METADATA_TYPE_PADDING,
    FLAC_METADATA_TYPE_APPLICATION,
    FLAC_METADATA_TYPE_SEEKTABLE,
    FLAC_METADATA_TYPE_VORBIS_COMMENT,
    FLAC_METADATA_TYPE_CUESHEET,
    FLAC_METADATA_TYPE_PICTURE,
    FLAC_METADATA_TYPE_INVALID = 127,
};

struct flac_stream_info {
    size_t                     bsize_min;       ///< block size
    size_t                     bsize_max;       ///< block size
    size_t                     fsize_min;       ///< frame size
    size_t                     fsize_max;       ///< frame size
    size_t                     rate;
    uint8_t                    bits;
    uint8_t                    channels;
    uint64_t                   nb_samples;      ///< total samples, may be 0
};

struct flac_hdr {
    uint8_t                   is_var;
    uint8_t                   channels;
    uint8_t                   bits;             ///< may be 0, get from stream info
    size_t                    block_size;
    size_t                    rate;             ///< may be 0, get from stream info
    int64_t                   nb_frame_or_sample;
};

/**
 * @brief  get flac header info
 * @param  [in] buf
 * @param  [in] hinfo
 * @return 0/-1
 */
int flac_hdr_get(const uint8_t* buf, struct flac_hdr *hinfo);

/**
 * @brief  parse the stream info of flac
 * @param  [in] buf
 * @param  [in] si
 * @return 0/-1
 */
int flac_parse_si(const uint8_t buf[FLAC_STREAMINFO_SIZE], struct flac_stream_info *si);

/**
 * @brief  sync the flac
 * @param  [in] rcb        : read byte callback
 * @param  [in] opaque     : in param of the callback
 * @param  [in] sync_max   : max of the sync count
 * @param  [in/out] hdr[4] : hdr of the flac
 * @param  [in/out] hinfo
 * @return -1 on error or read byte count for sync
 */
int flac_sync(read_bytes_t rcb, void *opaque, size_t sync_max, uint8_t hdr[FLAC_HDR_SIZE_MAX], struct flac_hdr *hinfo);

__END_DECLS__

#endif /* __FLAC_RW_H__ */

