/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MP4_TYPES_H__
#define __MP4_TYPES_H__

#include "avutil/common.h"

__BEGIN_DECLS__

#define ATOMID(a, b, c, d)     ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))
#define ATOM_FTYP_SIZE_MAX     (32)

#define MP4_ES_DESCR_TAG                    (0x03)
#define MP4_DEC_CONFIG_DESCR_TAG            (0x04)
#define MP4_DEC_SPECIFIC_DESCR_TAG          (0x05)

typedef struct {
    uint32_t type;
    int64_t  size;
} mp4_atom_t;

typedef struct {
    int32_t count;
    int32_t duration;
} mp4_stts_t;

typedef struct {
    int32_t first;
    int32_t count;
    int32_t id;
} mp4_stsc_t;

typedef struct {
    uint32_t track_id;
    uint32_t stsd_id;
    uint32_t duration;
    uint32_t size;
    uint32_t flags;
} mp4_trex_t;

typedef struct {
    int found_tfhd;
    uint32_t track_id;
    uint64_t base_data_offset;
    uint64_t moof_offset;
    uint64_t implicit_offset;
    uint32_t stsd_id;
    uint32_t duration;
    uint32_t size;
    uint32_t flags;
} mp4_frag_t;

__END_DECLS__

#endif /* __MP4_TYPES_H__ */

