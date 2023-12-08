/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ADTS_RW_H__
#define __ADTS_RW_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"
#include "avformat/m4a_rw.h"

__BEGIN_DECLS__

#define ADTS_HDR_SIZE           (7)
#define ADTS_FRAME_BYTES_MAX    ((1 << 13) - 1)

struct adts_hdr {
    uint8_t                   id;         ///< mpeg version
    uint8_t                   profile;
    uint8_t                   channel;
    uint16_t                  flen;       ///< aac frame length
    uint32_t                  freq;
    uint32_t                  nb_samples; ///< number of the samples in one adts frame
    uint32_t                  bps;
};

/**
 * @brief  get the header from the buf
 * @param  [in] buf
 * @param  [in] hinfo
 * @return 0/-1
 */
int adts_hdr_get(const uint8_t* buf, struct adts_hdr *hinfo);

/**
 * @brief  encode the adts hdr from m4a config
 * @param  [in] m4ac
 * @param  [in] hdr[ADTS_HDR_SIZE]
 * @param  [in] fsize : size of aac frame
 * @return 0/-1
 */
int adts_hdr_encode(const m4a_cnf_t *m4ac, uint8_t hdr[ADTS_HDR_SIZE], size_t fsize);

/**
 * @brief  sync the adts
 * @param  [in] rcb      : read byte callback
 * @param  [in] opaque   : in param of the callback
 * @param  [in] sync_max : max of the sync count
 * @param  [in/out] hdr  : hdr of the adts
 * @param  [in/out] hinfo
 * @return -1 on error or read byte count for sync
 */
int adts_sync(read_bytes_t rcb, void *opaque, size_t sync_max, uint8_t hdr[ADTS_HDR_SIZE], struct adts_hdr *hinfo);

__END_DECLS__

#endif /* __ADTS_RW_H__ */


