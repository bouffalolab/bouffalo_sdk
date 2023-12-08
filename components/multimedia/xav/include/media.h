/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __YOC_MEDIA_H__
#define __YOC_MEDIA_H__

#include <uservice/uservice.h>
#include "avutil/av_config.h"
#include "avutil/eq_typedef.h"
#include "xplayer/xplayer_typedef.h"
#include "media_typedef.h"

/**
 * 获取指定播放器的状态
 *
 * @param task 指定播放器服务的utask
 *        如果==NULL，播放器自己创建utask
 * @return 0:成功
 */
int aui_player_init(utask_t *task, media_evt_t evt_cb);

/**
 * @brief  init the default config value before configure
 * @param  [in] config
 * @return 0/-1
 */
int aui_player_config_init(aui_player_config_t *config);

/**
 * 配置参数
 *
 * @return 0:成功
 */
int aui_player_config(const aui_player_config_t *config);

/**
 * 播放音乐，强制停止已经在播的音乐
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @param url 媒体资源
 *        file:///fatfs0/Music/1.mp3  SD卡中的音频文件
 *        http://.../1.mp3            http音频
 *        fifo://tts/1           播放云端反馈的tts流
 *        mem://addr=%u&size=%u       播放存放在ram中资源
 * @param seek_time   播放偏移时间
 * @param resume   自动重启音乐(0：手动 1：自动)
 * @return 0:成功
 */
int aui_player_play(int type, const char *url, int resume);
int aui_player_seek_play(int type, const char *url, uint64_t seek_time, int resume);

/**
 * 暂停播放
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @return 0:成功
 */
int aui_player_pause(int type);

/**
 * 暂停状态继续播放和静音状态恢复播放
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,
 * @return 0:成功
 */
int aui_player_resume(int type);

/**
 * 停止播放器
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @return 0:成功
 */
int aui_player_stop(int type);

/**
 * 播放器seek
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @param seek_time seek时间，单位ms
 * @return 0:成功
 */
int aui_player_seek(int type, uint64_t seek_time);

/**
 * 播放器静音,调用aui_player_resume接口恢复音频输出
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @return 0:成功
 */
int aui_player_mute(int type);

/**
 * 取消播放器静音
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @return 0:成功
 */
int aui_player_unmute(int type);

/**
 * 调整音量
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,,MEDIA_ALL
 * @param inc_volume 正数加大，负数减小
 * @return 0:成功
 */
int aui_player_vol_adjust(int type, int inc_volume);

/**
 * 调整音量到指定值
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @param volume 指定音量的百分比 0~100. 0 means mute-enable
 * @return 0:成功
 */
int aui_player_vol_set(int type, int volume);

/**
 * 渐变调整音量到指定值
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,
 * @param new_volume 目标音量百分比0~100
 * @param ms 渐变时间
 * @return 0:成功
 */
int aui_player_vol_gradual(int type, int new_volume, int ms);

/**
 * 获取当前音量值
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @return 音量值
 */
int aui_player_vol_get(int type);

/**
 * 设置播放时最小音量
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @param volume 指定音量的百分比 0~100
 * @return 0:成功
 */
int aui_player_set_minvol(int type, int volume);

/**
 * 获取指定播放器的状态
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @return aui_player_state_t
 */
aui_player_state_t aui_player_get_state(int type);

/**
 * 通知播放完成后恢复音乐播放(有音量渐变效果)
 *
 * @return 0:成功
 */
int aui_player_resume_music(void);

/**
 * 配置参数
 *
 * @param eq_segments EQ段配置数组
 * @param count 段个数
 * @return 0:成功
 */
int aui_player_eq_config(eqfp_t *eq_segments, int count);

/**
 * 配置获取密钥回调
 *
 * @param cb 密钥回调
 * @return 0:成功
 */
int aui_player_key_config(media_key_cb_t cb);

/**
 * 获取播放时间
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @param t    播放时长
 * @return 0:成功
 */
int aui_player_get_time(int type, aui_play_time_t *t);

/**
 * 获取播放速度
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @param speed
 * @return 0:成功
 */
int aui_player_get_speed(int type, float *speed);

/**
 * 设置播放速度
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @param speed
 * @return 0:成功
 */
int aui_player_set_speed(int type, float speed);

/**
 * @brief  get media info of the type
 * @param  [in] type
 * @param  [in] minfo
 * @return 0/-1
 */
int aui_player_get_media_info(int type, xmedia_info_t *minfo);

/**
 * @brief  switch audio track of the type
 * @param  [in] type
 * @param  [in] idx : audio index
 * @return 0/-1
 */
int aui_player_switch_audio_track(int type, uint8_t idx);

/**
 * @brief  switch subtitle track of the type
 * @param  [in] type
 * @param  [in] idx : subtitle index
 * @return 0/-1
 */
int aui_player_switch_subtitle_track(int type, uint8_t idx);

/**
 * @brief  set extern subtitle of the type
 * @param  [in] type
 * @param  [in] url
 * @return 0/-1
 */
int aui_player_set_subtitle_url(int type, const char *url);

/**
 * @brief  hide/show subtitle of the type
 * @param  [in] type
 * @param  [in] visible
 * @return 0/-1
 */
int aui_player_set_subtitle_visible(int type, uint8_t visible);

/**
 * @brief  hide/show video of the type
 * @param  [in] type
 * @param  [in] visible
 * @return 0/-1
 */
int aui_player_set_video_visible(int type, uint8_t visible);

/**
 * @brief  crop video of the type
 * @param  [in] type
 * @param  [in] win
 * @return 0/-1
 */
int aui_player_set_video_crop(int type, const xwindow_t *win);

/**
 * @brief  set display window of the type
 * @param  [in] type
 * @param  [in] win
 * @return 0/-1
 */
int aui_player_set_display_window(int type, const xwindow_t *win);

/**
 * @brief  on/off fullscreen of the type
 * @param  [in] type
 * @param  [in] onoff
 * @return 0/-1
 */
int aui_player_set_fullscreen(int type, uint8_t onoff);

/**
 * @brief  set display format of the type
 * @param  [in] type
 * @param  [in] format
 * @return 0/-1
 */
int aui_player_set_display_format(int type, xdisplay_format_t format);

/**
 * @brief  rotate video of the type
 * @param  [in] type
 * @param  [in] rotate_type
 * @return 0/-1
 */
int aui_player_set_video_rotate(int type, xrotate_type_t rotate_type);

#ifdef __cplusplus
}
#endif

#endif
