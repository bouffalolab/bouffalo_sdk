/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#if defined(CONFIG_AV_DBUS_SUPPORT) && CONFIG_AV_DBUS_SUPPORT
#ifndef __MEDIA_CLIENT_DBUS_H__
#define __MEDIA_CLIENT_DBUS_H__

#include "avdbus/media_typedef_dbus.h"
#include "media_typedef.h"

__BEGIN_DECLS__

typedef struct {
    int              resume;    /* 自动重启音乐(0：手动 1：自动)  */
    uint64_t         seek_time;
} auic_ply_t;

/**
 * @brief  init the aui_player client
 * @param  [in] evt_cb
 * @return 0/-1
 */
int auic_player_init(media_evt_t evt_cb);

/**
* @brief  play one url of the media-type
 * @param  [in] type : media type
 * @param  [in] url
 * @param  [in] conf
* @return 0/-1
 */
int auic_player_play(int type, const char *url, auic_ply_t *conf);

/**
* @brief  pause play the media type
 * @param  [in] type : media type
* @return 0/-1
*/
int auic_player_pause(int type);

/**
* @brief  resume play the media type
 * @param  [in] type : media type
* @return 0/-1
*/
int auic_player_resume(int type);

/**
* @brief  stop play the media type
 * @param  [in] type : media type
* @return 0/-1
*/
int auic_player_stop(int type);

/**
* @brief  mute play the media type
 * @param  [in] type : media type
* @return 0/-1
*/
int auic_player_mute(int type);

/**
* @brief  unmute play the media type
 * @param  [in] type : media type
* @return 0/-1
*/
int auic_player_unmute(int type);

/**
* @brief  seek play the media type
 * @param  [in] type : media type
 * @param  [in] seek_time : ms
* @return 0/-1
 */
int auic_player_seek(int type, uint64_t seek_time);


__END_DECLS__

#endif /* __MEDIA_CLIENT_DBUS_H__ */

#endif
