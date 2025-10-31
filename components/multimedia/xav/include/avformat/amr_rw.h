/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AMR_RW_H__
#define __AMR_RW_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"

__BEGIN_DECLS__

#define AMR_HDR_SIZE           (1)
#define AMR_FRAME_INTERVAL     (20)       ///< 20ms
#define AMRNB_RATE             (8000)
#define AMRWB_RATE             (16000)
#define AMRNB_STR              "#!AMR\n"
#define AMRWB_STR              "#!AMR-WB\n"

#define AMR_TYPE_UNKNOWN       (0)
#define AMR_TYPE_NB            (1)
#define AMR_TYPE_WB            (2)

struct amr_hdr {
    uint8_t                   scode;      ///< sync code
    uint8_t                   mode;       ///< rate mode
    uint8_t                   framesize;  ///< frame size of rate
};

/**
 * @brief  get the header from the buf
 * @param  [in] buf
 * @param  [in] amr_type : AMR_TYPE_XXX
 * @param  [in] hinfo
 * @return 0/-1
 */
int amr_hdr_get(const uint8_t* buf, uint8_t amr_type, struct amr_hdr *hinfo);

/**
 * @brief  sync the amr
 * @param  [in] rcb      : read byte callback
 * @param  [in] opaque   : in param of the callback
 * @param  [in] sync_max : max of the sync count
 * @param  [in/out] hdr  : hdr of the amr
 * @param  [in] amr_type : AMR_TYPE_XXX
 * @param  [in/out] hinfo
 * @return -1 on error or read byte count for sync
 */
int amr_sync(read_bytes_t rcb, void *opaque, size_t sync_max, uint8_t hdr[AMR_HDR_SIZE], uint8_t amr_type, struct amr_hdr *hinfo);

__END_DECLS__

#endif /* __AMR_RW_H__ */

