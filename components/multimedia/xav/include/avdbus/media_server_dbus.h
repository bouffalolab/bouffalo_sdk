/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */
#if defined(CONFIG_AV_DBUS_SUPPORT) && CONFIG_AV_DBUS_SUPPORT

#ifndef __MEDIA_SERVER_DBUS_H__
#define __MEDIA_SERVER_DBUS_H__

#include "avdbus/media_typedef_dbus.h"

__BEGIN_DECLS__

typedef struct media_server_conf {
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
} msvc_conf_t;

/**
* @brief  init the config of media server
* @param  [in] conf
* @return 0/-1
*/
int media_server_config_init(msvc_conf_t *conf);

/**
* @brief  init the media server
* @param  [in] conf
* @return 0/-1
*/
int media_server_init(const msvc_conf_t *conf);

/**
* @brief  start the media server
* @return 0/-1
*/
int media_server_start();

/**
* @brief  stop the media server
* @return 0/-1
*/
int media_server_stop();

/**
* @brief  destroy the media server
* @return 0/-1
*/
int media_server_deinit();

__END_DECLS__

#endif /* __MEDIA_SERVER_DBUS_H__ */
#endif

