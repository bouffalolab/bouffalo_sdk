/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFRAME_H__
#define __AVFRAME_H__

#include "avutil/av_typedef.h"

__BEGIN_DECLS__

typedef struct avframe {
#define AV_DATA_POINTERS_MAX  (8)
    avmedia_type_t            type;
    uint8_t                   *data[AV_DATA_POINTERS_MAX];
    int                       linesize[AV_DATA_POINTERS_MAX];  ///< real size of the data
    int                       capsize[AV_DATA_POINTERS_MAX];   ///< linesize <= capsize, capacity size of the data
    //int                     format;

    uint8_t                   *mpool;
    size_t                    msize;                           ///< size of the memory pool
    size_t                    moffset;                         ///< offset of the memory pool

    /* for audio below */
    sf_t                      sf;
    int                       nb_samples;                      ///< per channel
} avframe_t;

/**
 * @brief  allocate one frame
 * @return NULL on err
 */
avframe_t *avframe_alloc();

/**
 * @brief  clear nb_samples, sf, linesize, etc. mpool & data point reserved otherwise.
 * @param  [in] frame
 * @return
 */
void avframe_clear(avframe_t *frame);

/**
 * @brief  free res of the frame
 * @param  [in] frame
 * @return
 */
void avframe_free(avframe_t **frame);

/**
 * @brief  get/alloc data buffer of the frame by sf and nb_samples
 * @param  [in] frame
 * @return 0/-1
 */
int avframe_get_buffer(avframe_t *frame);

/**
 * @brief  copy the frame filled from one to another
 * @param  [in] from
 * @param  [in] to
 * @return 0/-1
 */
int avframe_copy_from(const avframe_t *from, avframe_t *to);

/**
 * @brief  set memory pool of the frame, the data source is from the mem pool
 * @param  [in] frame
 * @param  [in] buf   : ptr of the memory pool
 * @param  [in] size  : size of the memory pool
 * @return 0/-1
 */
int avframe_set_mempool(avframe_t *frame, uint8_t *buf, size_t size);

__END_DECLS__

#endif /* __AVFRAME_H__ */

