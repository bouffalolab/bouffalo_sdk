/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __MEDIA_TYPEDEF_H__
#define __MEDIA_TYPEDEF_H__

#include "avutil/common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************
 * Audio service
 * type:
 *  0: Media playback
 *  1: System prompt sound playback
 *      There is only one playback instance for each type. A new playback will replace the old one.
 *      The playback of prompt sounds will first pause music playback and resume music playback after completion.
 *************************************************/

typedef enum {
    MEDIA_MUSIC  = 0,  /* Media playback */
    MEDIA_AV     = MEDIA_MUSIC,  /* Media playback */
    MEDIA_SYSTEM = 1,  /* System prompt sound playback */
    MEDIA_ALL    = 255,
} aui_player_type_t;

typedef enum {
    AUI_PLAYER_UNKNOWN,
    AUI_PLAYER_STOP,
    AUI_PLAYER_PLAYING,
    AUI_PLAYER_PAUSED
} aui_player_state_t;

typedef enum {
    AUI_PLAYER_EVENT_ERROR,
    AUI_PLAYER_EVENT_START,
    AUI_PLAYER_EVENT_FINISH,
    AUI_PLAYER_EVENT_UNDER_RUN,    ///< for stream-cache status
    AUI_PLAYER_EVENT_OVER_RUN,     ///< for stream-cache status
    AUI_PLAYER_EVENT_PAUSE,
    AUI_PLAYER_EVENT_RESUME,
    AUI_PLAYER_EVENT_STOP,

    AUI_PLAYER_EVENT_MUTE,
    AUI_PLAYER_EVENT_VOL_CHANGE,
} aui_player_evtid_t;

typedef int (*media_key_cb_t)(const void *in, size_t ilen, void *out, size_t *olen);

typedef struct {
    uint8_t   *aef_conf;           ///< config data for aef. resident memory
    size_t    aef_conf_size;       ///< size of the config data for aef
    uint32_t  resample_rate;
    uint32_t  web_cache_size;      ///< size of the web cache. 0 use default
    uint32_t  web_start_threshold; ///< (0~100)start read for player when up to cache_start_threshold. 0 use default
    uint32_t  snd_period_ms;       ///< period cache size(ms) for audio out. 0 means use default
    uint32_t  snd_period_num;      ///< number of period_ms. total cache size for ao is (period_num * period_ms * (rate / 1000) * 2 * (16/8)). 0 means use default

    uint8_t   *vol_map;            ///< vol-index array for digital volume curve[0~100], nums of the elems must be 101. resident memory
    int32_t   db_min;              ///< min db for digital volume curve
    int32_t   db_max;              ///< max db for digital volume curve
} aui_player_config_t;

typedef struct {
    uint64_t  duration;            ///< ms, maybe a dynamic time
    uint64_t  curtime;             ///< ms, current time
} aui_play_time_t;

typedef struct {
    int       val_old;
    int       val_new;
} aui_val_change_t;

/**
 * Player event user handler function.
 *
 * @param evt_id
 */
typedef void (*media_evt_t)(int type, const char *url, aui_player_evtid_t evt_id, const void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __MEDIA_TYPEDEF_H__ */

