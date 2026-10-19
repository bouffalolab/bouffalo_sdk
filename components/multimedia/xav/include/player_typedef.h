/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __PLAYER_TYPEDEF_H__
#define __PLAYER_TYPEDEF_H__

#include "avutil/common.h"

__BEGIN_DECLS__

#define PLAY_SPEED_MIN         (0.5)
#define PLAY_SPEED_MAX         (4.0)

enum {
    PLAYER_EVENT_UNKNOWN,
    PLAYER_EVENT_ERROR,
    PLAYER_EVENT_START,
    PLAYER_EVENT_FINISH,
    PLAYER_EVENT_UNDER_RUN,    ///< for stream-cache status
    PLAYER_EVENT_OVER_RUN,     ///< for stream-cache status
};

/**
* @brief  player event callback for user
* @param  [in] user_data
* @param  [in] event : PLAYER_EVENT_XXX
* @param  [in] data  : reserved
* @param  [in] len   : reserved
*/
typedef void (*player_event_t)(void *user_data, uint8_t event, const void *data, uint32_t len);

typedef struct player_conf {
    char                      *ao_name;      ///< ao name
    uint8_t                   vol_en;        ///< soft vol scale enable
    uint8_t                   vol_index;     ///< soft vol scale index (0~255)
    uint8_t                   eq_segments;   ///< equalizer segments number. 0 means don't need eq. TODO: not support yet!
    uint8_t                   atempo_play_en;///< atempo play enable
    float                     speed;         ///< atempo play speed.suggest: 0.5 ~ 2.0;
    uint8_t                   *aef_conf;     ///< config data for aef
    size_t                    aef_conf_size; ///< size of the config data for aef
    uint32_t                  resample_rate; ///< none zereo means need to resample
    uint32_t                  rcv_timeout;   ///< timeout for recv stream. used inner default timeout when 0
    uint32_t                  cache_size;    ///< size of the web cache. 0 use default
    uint32_t                  cache_start_threshold; ///< (0~100)start read for player when up to cache_start_threshold. 0 use default
    uint32_t                  period_ms;     ///< period cache size(ms) for audio out. 0 means use default
    uint32_t                  period_num;    ///< number of period_ms. total cache size for ao is (period_num * period_ms * (rate / 1000) * 2 * (16/8)). 0 means use default
    get_decrypt_cb_t          get_dec_cb;    ///< used for get decrypt info
    player_event_t            event_cb;      ///< callback of the player event
    void                      *user_data;    ///< user data of the player event

    int32_t                   db_min;        ///< min db for digital volume curve
    int32_t                   db_max;        ///< max db for digital volume curve
} ply_conf_t;

__END_DECLS__

#endif /* __PLAYER_TYPEDEF_H__ */

