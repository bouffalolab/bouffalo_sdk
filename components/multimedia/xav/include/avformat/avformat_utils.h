/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFORMAT_UTILS_H__
#define __AVFORMAT_UTILS_H__

#include "avutil/sf.h"
#include "avutil/av_typedef.h"
#include "avutil/bio.h"
#include "avutil/byte_rw.h"

__BEGIN_DECLS__

/**
 * @brief  xxx_sync read bytes from stream
 * @param  [in] opaque
 * @param  [in] bytes
 * @param  [in] size
 * @return -1 on error
 */
int sync_read_stream(void *opaque, uint8_t *bytes, size_t size);

/**
 * @brief  xxx_sync read bytes from bio_t, may be used for avformat probe
 * @param  [in] opaque
 * @param  [in] bytes
 * @param  [in] size
 * @return -1 on error
 */
int sync_read_bio(void *opaque, uint8_t *bytes, size_t size);

/**
 * @brief  get size of the id2v2
 * @param  [in] buf
 * @param  [in] buf_size
 * @return 0 on no id3 info
 */
size_t get_id3v2_size(const uint8_t *buf, size_t buf_size);

/**
 * @brief  new a track info
 * @return NULL on error
 */
track_info_t* track_info_new();

/**
 * @brief  free the track info
 * @param  [in] track
 * @return 0/-1
 */
int track_info_free(track_info_t *track);

/**
 * @brief  free the track list info
 * @param  [in] ptracks
 * @return 0/-1
 */
int tracks_info_freep(track_info_t **ptracks);

/**
 * @brief  add track to the track list info
 * @param  [in] ptracks
 * @param  [in] track
 * @return 0/-1
 */
int tracks_info_add(track_info_t **ptracks, const track_info_t *track);

/**
 * @brief  duplicate the media-track
 * @param  [in] track
 * @return NULL on error
 */
track_info_t* track_info_dup(const track_info_t *track);

/**
 * @brief  duplicate the media-track list
 * @param  [in] tracks : media track list
 * @return NULL on error
 */
track_info_t* tracks_info_dup(const track_info_t *tracks);

/**
 * @brief  init the media-info
 * @param  [in] info
 * @return 0/-1
 */
int media_info_init(media_info_t *info);

/**
 * @brief  uninit the media-info
 * @param  [in] info
 * @return
 */
void media_info_uninit(media_info_t *info);

__END_DECLS__

#endif /* __AVFORMAT_UTILS_H__ */

