/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AV_TYPEDEF_H__
#define __AV_TYPEDEF_H__

#include "avutil/common.h"
#include "avutil/sf.h"

__BEGIN_DECLS__

#define TAG_VAL(a, b, c, d)    ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))
#define AV_FLICK_BASE          (705600000)
//#define POST_PROC_SAMPLES_MAX  (48000 / 1000 * 100)  ///< 0.1s 48k
#define POST_PROC_SAMPLES_MAX  (512)

#define AO_ONE_PERIOD_MS       (5)
#define AO_TOTAL_PERIOD_NUM    (20)

#define VOL_SCALE_DB_MAX       (14)     ///< for digital volume 
#define VOL_SCALE_DB_MIN       (-60)    ///< for digital volume 

/**
 * @brief  read bytes from opaque
 * @param  [in] opaque
 * @param  [in] bytes
 * @param  [in] size : need read count
 * @return -1 on error
 */
typedef int (*read_bytes_t)(void *opaque, uint8_t *bytes, size_t size);

typedef struct irq_av {
    int (*handler)     (void *arg);
    void               *arg;
} irq_av_t;

typedef enum {
    AVMEDIA_TYPE_UNKNOWN,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_VIDEO,
} avmedia_type_t;

enum {
    DEMUX_CMD_GET_DURATION,
    DEMUX_CMD_GET_BITRATE,
};

typedef enum avcodec_id {
    AVCODEC_ID_UNKNOWN,
    AVCODEC_ID_MP3            = (1 << 0),
    AVCODEC_ID_AAC            = (1 << 1),
    AVCODEC_ID_RAWAUDIO       = (1 << 2),
    AVCODEC_ID_ADPCM_MS       = (1 << 3),
    AVCODEC_ID_FLAC           = (1 << 4),
    AVCODEC_ID_WMAV1          = (1 << 5),
    AVCODEC_ID_WMAV2          = (1 << 6),
    AVCODEC_ID_AMRNB          = (1 << 7),
    AVCODEC_ID_AMRWB          = (1 << 8),
    AVCODEC_ID_OPUS           = (1 << 9),
    AVCODEC_ID_SPEEX          = (1 << 10),
    AVCODEC_ID_ALAW           = (1 << 11),
    AVCODEC_ID_MULAW          = (1 << 12),
    AVCODEC_ID_VORBIS         = (1 << 13),
} avcodec_id_t;

typedef struct {
    avcodec_id_t id;
    uint32_t     tag;
} avcodec_tag_t;

enum {
    AVFORMAT_TYPE_UNKNOWN,
    AVFORMAT_TYPE_RAWAUDIO,
    AVFORMAT_TYPE_WAV,
    AVFORMAT_TYPE_MP3,
    AVFORMAT_TYPE_MP4,
    AVFORMAT_TYPE_ADTS,
    AVFORMAT_TYPE_FLAC,
    AVFORMAT_TYPE_ASF,
    AVFORMAT_TYPE_AMR,
    AVFORMAT_TYPE_TS,
    AVFORMAT_TYPE_OGG,
};

typedef struct sh_audio {
    sf_t                      sf;
    avcodec_id_t              id;
    uint8_t                   *extradata;     ///< extradata or frame header, if needed
    int32_t                   extradata_size;
    uint32_t                  block_align;    ///< size of one frame, if needed
    uint32_t                  bps;            ///< bps, if needed
} sh_audio_t;

//TODO
typedef struct {
    char                      *artist;
    char                      *album;
    char                      *title;

    sf_t                      sf;
} track_audio_t;

//TODO
typedef struct {
    int16_t                   width;
    int16_t                   height;
} track_video_t;

typedef struct track_info {
    struct track_info         *next;
    avmedia_type_t            type;
    char*                     codec_name;
    avcodec_id_t              codec_id;
    uint64_t                  bps;
    uint64_t                  duration;       ///< ms, track duration

    union {
        track_audio_t  a;
        track_video_t  v;
    }                         t;
} track_info_t;

typedef struct {
    uint64_t                  duration;      ///< ms, maybe a dynamic time
    uint64_t                  curtime;       ///< ms, current time
} play_time_t;

typedef struct {
    track_info_t              *tracks;       ///< for fulture use, don't need free for user
    uint64_t                  size;          ///< media file size
    uint64_t                  bps;
    uint64_t                  duration;      ///< ms, media duration
} media_info_t;

__END_DECLS__

#endif /* __AV_TYPEDEF_H__ */

