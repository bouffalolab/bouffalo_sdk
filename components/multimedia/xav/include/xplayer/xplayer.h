/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __XPLAYER_H__
#define __XPLAYER_H__

#include "avutil/av_config.h"
#include "xplayer/xplayer_typedef.h"

__BEGIN_DECLS__

typedef struct xplayer_struct xplayer_t;
typedef struct xplayer_cls xplayer_cls_t;

/**
 * @brief  regist xplayer class
 * @param  [in] ops
 * @return 0/-1
 */
int xplayer_cls_register(const xplayer_cls_t *cls);

/**
* @brief  init config param for xplayer_module_config
* @param  [in] conf
* @return 0/-1
*/
int xplayer_module_config_init(xplayer_mdl_cnf_t *conf);

/**
 * @brief  init xplayer module default(init once)
 * @param  [in] params : module config params
 * @return 0/-1
 */
int xplayer_module_init(const xplayer_mdl_cnf_t *conf);

/**
 * @brief  new one xplayer obj by the name. create default player if name is NULL
 * @param  [in] name : player class by the name, may be NULL
 * @return NULL on error
 */
xplayer_t* xplayer_new(const char *name);

/**
 * @brief  get config param from the xplayer
 * @param  [in] xplayer
 * @param  [in/out] conf
 * @return 0/-1
 */
int xplayer_get_config(xplayer_t *xplayer, xplayer_cnf_t *conf);

/**
 * @brief  set config param for the xplayer before play
 * @param  [in] xplayer
 * @param  [in] conf
 * @return 0/-1
 */
int xplayer_set_config(xplayer_t *xplayer, const xplayer_cnf_t *conf);

/**
 * @brief  set event callback before play
 * @param  [in] xplayer
 * @param  [in] event_cb : callback of the xplayer event
 * @param  [in] user_data : user data for event_cb
 * @return 0/-1
 */
int xplayer_set_callback(xplayer_t *xplayer, xplayer_eventcb_t event_cb, const void *user_data);

/**
 * @brief  set play source url before play
 * @param  [in] xplayer
 * @param  [in] url : example: http://ip:port/xx.mp3
 * @return 0/-1
 */
int xplayer_set_url(xplayer_t *xplayer, const char *url);

/**
 * @brief  xplayer play interface
 * @param  [in] xplayer
 * @return 0/-1
 */
int xplayer_play(xplayer_t *xplayer);

/**
 * @brief  pause the xplayer
 * @param  [in] xplayer
 * @return 0/-1
 */
int xplayer_pause(xplayer_t *xplayer);

/**
 * @brief  resume the xplayer
 * @param  [in] xplayer
 * @return 0/-1
 */
int xplayer_resume(xplayer_t *xplayer);

/**
 * @brief  stop the xplayer
 * @param  [in] xplayer
 * @return 0/-1
 */
int xplayer_stop(xplayer_t *xplayer);

/**
 * @brief  free/destroy the xplayer obj
 * @param  [in] xplayer
 * @return 0/-1
 */
int xplayer_free(xplayer_t *xplayer);

/**
 * @brief  seek to the time
 * @param  [in] xplayer
 * @param  [in] timestamp : seek time
 * @return 0/-1
 */
int xplayer_seek(xplayer_t *xplayer, uint64_t timestamp);

/**
 * @brief  set start play time before play
 * @param  [in] xplayer
 * @param  [in] start_time : start play time
 * @return 0/-1
 */
int xplayer_set_start_time(xplayer_t *xplayer, uint64_t start_time);

/**
 * @brief  get play time(current time & duration, ms)
 * @param  [in] xplayer
 * @param  [in/out] time
 * @return 0/-1
 */
int xplayer_get_time(xplayer_t *xplayer, xplay_time_t *time);

/**
 * @brief  get media info
 * @param  [in] xplayer
 * @param  [in/out] minfo : need free by the caller(use media_info_uninit function)
 * @return 0/-1
 */
int xplayer_get_media_info(xplayer_t *xplayer, xmedia_info_t *minfo);

/**
 * @brief  get soft vol index of the xplayer
 * @param  [in] xplayer
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int xplayer_get_vol(xplayer_t *xplayer, uint8_t *vol);

/**
 * @brief  set soft vol index of the xplayer
 * @param  [in] xplayer
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int xplayer_set_vol(xplayer_t *xplayer, uint8_t vol);

/**
 * @brief  get play speed of the xplayer
 * @param  [in] xplayer
 * @param  [out] speed
 * @return 0/-1
 */
int xplayer_get_speed(xplayer_t *xplayer, float *speed);

/**
 * @brief  set play speed of the xplayer
 * @param  [in] xplayer
 * @param  [in] speed : 0.5 ~ 2.0 for audio, etc
 * @return 0/-1
 */
int xplayer_set_speed(xplayer_t *xplayer, float speed);

/**
 * @brief  mute/unmute the xplayer
 * @param  [in] xplayer
 * @param  [in] mute : mute flag
 * @return 0/-1
 */
int xplayer_set_mute(xplayer_t *xplayer, uint8_t mute);

/**
 * @brief  get play source url
 * @param  [in] xplayer
 * @return NULL on error
 */
const char* xplayer_get_url(xplayer_t *xplayer);

/**
 * @brief  switch audio track of the player
 * @param  [in] xplayer
 * @param  [in] idx : index of audios
 * @return 0/-1
 */
int xplayer_switch_audio_track(xplayer_t *xplayer, uint8_t idx);

/**
 * @brief  switch subtitle track of the player
 * @param  [in] xplayer
 * @param  [in] idx : index of subtitles
 * @return 0/-1
 */
int xplayer_switch_subtitle_track(xplayer_t *xplayer, uint8_t idx);

/**
 * @brief  set external subtitle url of the player
 * @param  [in] xplayer
 * @param  [in] url : url of the external subtitle
 * @return 0/-1
 */
int xplayer_set_subtitle_url(xplayer_t *xplayer, const char *url);

/**
 * @brief  show/hide subtitle of the xplayer
 * @param  [in] xplayer
 * @param  [in] visible : show/hide
 * @return 0/-1
 */
int xplayer_set_subtitle_visible(xplayer_t *xplayer, uint8_t visible);

/**
 * @brief  show/hide video of the xplayer
 * @param  [in] xplayer
 * @param  [in] visible : show/hide
 * @return 0/-1
 */
int xplayer_set_video_visible(xplayer_t *xplayer, uint8_t visible);

/**
 * @brief  crop video of the xplayer
 * @param  [in] xplayer
 * @param  [in] win : crop window
 * @return 0/-1
 */
int xplayer_set_video_crop(xplayer_t *xplayer, const xwindow_t *win);

/**
 * @brief  set display window of the xplayer
 * @param  [in] xplayer
 * @param  [in] win : display window
 * @return 0/-1
 */
int xplayer_set_display_window(xplayer_t *xplayer, const xwindow_t *win);

/**
 * @brief  enable/disable fullscreen for the xplayer
 * @param  [in] xplayer
 * @param  [in] onoff
 * @return 0/-1
 */
int xplayer_set_fullscreen(xplayer_t *xplayer, uint8_t onoff);

/**
 * @brief  set display format for the xplayer
 * @param  [in] xplayer
 * @param  [in] format
 * @return 0/-1
 */
int xplayer_set_display_format(xplayer_t *xplayer, xdisplay_format_t format);

/**
 * @brief  rotate video of the xplayer
 * @param  [in] xplayer
 * @param  [in] type : rotate type
 * @return 0/-1
 */
int xplayer_set_video_rotate(xplayer_t *xplayer, xrotate_type_t type);

__END_DECLS__

#endif /* __XPLAYER_H__ */

