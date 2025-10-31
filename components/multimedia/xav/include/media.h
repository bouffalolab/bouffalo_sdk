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
 * Get the state of the specified player.
 *
 * @param task The utask of the specified player service.
 *        If == NULL, the player will create its own utask.
 * @return 0: Success
 */
int aui_player_init(utask_t *task, media_evt_t evt_cb);

/**
 * @brief  init the default config value before configure
 * @param  [in] config
 * @return 0/-1
 */
int aui_player_config_init(aui_player_config_t *config);

/**
 * Configure parameters.
 *
 * @return 0: Success
 */
int aui_player_config(const aui_player_config_t *config);

/**
 * Play music, forcefully stopping any currently playing music.
 *
 * @param type Supported types: MEDIA_MUSIC, MEDIA_SYSTEM
 * @param url  Media resource:
 *             - file:///fatfs0/Music/1.mp3  (SD card audio file)
 *             - http://.../1.mp3             (HTTP audio stream)
 *             - fifo://tts/1                 (Cloud TTS stream)
 *             - mem://addr=%u&size=%u        (RAM buffer audio)
 * @param seek_time Playback offset time
 * @param resume    Auto-restart music (0: manual, 1: automatic)
 * @return 0: Success
 */
int aui_player_play(int type, const char *url, int resume);
int aui_player_seek_play(int type, const char *url, uint64_t seek_time, int resume);

/**
 * Pause playback.
 *
 * @param type Supported types: MEDIA_MUSIC, MEDIA_SYSTEM, MEDIA_ALL
 * @return 0: Success
 */
int aui_player_pause(int type);

/**
 * Resume playback from paused state or unmute from muted state.
 *
 * @param type Supported types: MEDIA_MUSIC, MEDIA_SYSTEM
 * @return 0: Success
 */
int aui_player_resume(int type);

/**
 * Stop the player.
 *
 * @param type Supported types: MEDIA_MUSIC, MEDIA_SYSTEM, MEDIA_ALL
 * @return 0: Success
 */
int aui_player_stop(int type);

/**
 * Seek the player to a specific position.
 *
 * @param type      Supported types: MEDIA_MUSIC, MEDIA_SYSTEM, MEDIA_ALL
 * @param seek_time Seek time in milliseconds
 * @return 0: Success
 */
int aui_player_seek(int type, uint64_t seek_time);

/**
 * Mute the player. Call the aui_player_resume interface to resume audio output.
 *
 * @param type Supported types: MEDIA_MUSIC, MEDIA_SYSTEM, MEDIA_ALL
 * @return 0: Success
 */
int aui_player_mute(int type);

/**
 * Unmute the player.
 *
 * @param type Supported types: MEDIA_MUSIC, MEDIA_SYSTEM, MEDIA_ALL
 * @return 0: Success
 */
int aui_player_unmute(int type);

/**
 * Adjust the volume.
 *
 * @param type       Supported types: MEDIA_MUSIC, MEDIA_SYSTEM, MEDIA_ALL
 * @param inc_volume Positive value to increase, negative value to decrease
 * @return 0: Success
 */
int aui_player_vol_adjust(int type, int inc_volume);

/**
 * Adjust the volume to a specified value.
 *
 * @param type   Supported types: MEDIA_MUSIC, MEDIA_SYSTEM, MEDIA_ALL
 * @param volume Specified volume percentage (0~100). 0 means mute enabled
 * @return 0: Success
 */
int aui_player_vol_set(int type, int volume);

/**
 * Gradually adjust the volume to a specified value.
 *
 * @param type        Supported types: MEDIA_MUSIC, MEDIA_SYSTEM
 * @param new_volume  Target volume percentage (0~100)
 * @param ms          Gradual adjustment time in milliseconds
 * @return 0: Success
 */
int aui_player_vol_gradual(int type, int new_volume, int ms);

/**
 * Get the current volume value.
 *
 * @param type Supported types: MEDIA_MUSIC, MEDIA_SYSTEM
 * @return Volume value
 */
int aui_player_vol_get(int type);

/**
 * Set the minimum volume during playback.
 *
 * @param type   Supported types: MEDIA_MUSIC, MEDIA_SYSTEM, MEDIA_ALL
 * @param volume Specified volume percentage (0~100)
 * @return 0: Success
 */
int aui_player_set_minvol(int type, int volume);

/**
 * Get the state of the specified player.
 *
 * @param type Supported types: MEDIA_MUSIC, MEDIA_SYSTEM
 * @return aui_player_state_t (Player state enumeration)
 */
aui_player_state_t aui_player_get_state(int type);

/**
 * Notify to resume music playback after playback completion (with volume fade effect)
 *
 * @return 0: Success
 */
int aui_player_resume_music(void);

/**
 * Configure parameters.
 *
 * @param eq_segments Array of EQ segment configurations
 * @param count Number of segments
 * @return 0: Success
 */
int aui_player_eq_config(eqfp_t *eq_segments, int count);

/**
 * Configure the callback for obtaining the encryption key.
 *
 * @param cb Key callback function
 * @return 0: Success
 */
int aui_player_key_config(media_key_cb_t cb);

/**
 * Get the playback time.
 *
 * @param type Supported types: MEDIA_MUSIC, MEDIA_SYSTEM
 * @param t    Playback duration
 * @return 0: Success
 */
int aui_player_get_time(int type, aui_play_time_t *t);

/**
 * Get the playback speed.
 *
 * @param type  Supported types: MEDIA_MUSIC, MEDIA_SYSTEM
 * @param speed Output parameter for playback speed
 * @return 0: Success
 */
int aui_player_get_speed(int type, float *speed);

/**
 * Set the playback speed.
 *
 * @param type  Supported types: MEDIA_MUSIC, MEDIA_SYSTEM
 * @param speed Playback speed value
 * @return 0: Success
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
