/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __XPLAYER_TYPEDEF_H__
#define __XPLAYER_TYPEDEF_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"
#include "avutil/eq_typedef.h"

__BEGIN_DECLS__

typedef enum {
    XPLAYER_EVENT_UNKNOWN,
    XPLAYER_EVENT_ERROR,
    XPLAYER_EVENT_START,
    XPLAYER_EVENT_FINISH,
    XPLAYER_EVENT_UNDER_RUN,    ///< for stream-cache status
    XPLAYER_EVENT_OVER_RUN,     ///< for stream-cache status
} xplayer_event_t;

typedef enum {
    XROTATE_DEGREE_0       = 0,
    XROTATE_DEGREE_90      = 90,
    XROTATE_DEGREE_180     = 180,
    XROTATE_DEGREE_270     = 270,
} xrotate_type_t;

enum xplayer_status {
    XPLAYER_STATUS_STOPED,
    XPLAYER_STATUS_PLAYING,
    XPLAYER_STATUS_PAUSED,
};

typedef enum {
    XDISPLAY_LETTER_BOX,     /* use letterbox format */
    XDISPLAY_PAN_SCAN,       /* use pan and scan format */
    XDISPLAY_AUTO_FILLED,    /* use filled format */
} xdisplay_format_t;

/**
* @brief  xplayer event callback for user
* @param  [in] user_data
* @param  [in] event : XPLAYER_EVENT_XXX
* @param  [in] data  : reserved
* @param  [in] len   : reserved
*/
typedef void (*xplayer_eventcb_t)(void *user_data, uint8_t event, const void *data, uint32_t len);

typedef media_info_t xmedia_info_t;
typedef play_time_t  xplay_time_t;

typedef struct {
    uint32_t                  x;
    uint32_t                  y;
    uint32_t                  width;
    uint32_t                  height;
} xwindow_t;

typedef struct xplayer_module_config {
    //TODO:
    xwindow_t                 viewport;
} xplayer_mdl_cnf_t ;

typedef struct {
    uint8_t                   *aef_conf;           ///< config data for aef
    size_t                    aef_conf_size;       ///< size of the config data for aef
    uint32_t                  resample_rate;
    uint32_t                  web_cache_size;      ///< size of the web cache. 0 use default
    uint32_t                  web_start_threshold; ///< (0~100)start read for player when up to cache_start_threshold. 0 use default
    uint32_t                  snd_period_ms;       ///< period cache size(ms) for audio out. 0 means use default
    uint32_t                  snd_period_num;      ///< number of period_ms. total cache size for ao is (period_num * period_ms * (rate / 1000) * 2 * (16/8)). 0 means use default
    uint8_t                   eq_segments;         ///< used for equalizer config
    eqfp_t                    *eq_params;
    int32_t                   db_min;              ///< min db for digital volume curve
    int32_t                   db_max;              ///< max db for digital volume curve

    uint8_t                   freezed;             ///< hold last pic when play stoped
} xplayer_cnf_t;

__END_DECLS__

#endif /* __XPLAYER_TYPEDEF_H__ */

