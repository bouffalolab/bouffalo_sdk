/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __SMART_AUDIO_H__
#define __SMART_AUDIO_H__

#include <media.h>
#include <media_typedef.h>
#include <xutils/list.h>

#define VOLUME_SAVE_KV_NAME "volume"
#define INTERRUPT_REASON_BY_USER 255
#define SMART_AUDIO_DEFAULT_VOLUME 60

typedef enum {
    SMTAUDIO_ONLINE_MUSIC = MEDIA_MUSIC, /*音乐播放*/
    SMTAUDIO_LOCAL_PLAY   = MEDIA_SYSTEM, /*通知播放*/
    SMTAUDIO_BT_A2DP     = 102, /*蓝牙a2dp*/
    SMTAUDIO_BT_HFP      = 103, /*蓝牙hfp*/
    SMTAUDIO_PLAY_TYPE_NUM = 4, /*类型总数*/
    SMTAUDIO_TYPE_ALL     = 255,
} smtaudio_player_type_t;

#define DEFAULT_PLAY_TYPE SMTAUDIO_LOCAL_PLAY

typedef enum {
    SMTAUDIO_STATE_PLAYING,
    SMTAUDIO_STATE_PAUSE,
    SMTAUDIO_STATE_STOP,
    SMTAUDIO_STATE_MUTE,
    SMTAUDIO_STATE_NOINIT,
    SMTAUDIO_STATE_NUM
} smtaudio_state_t;

typedef enum {
    SMTAUDIO_SUBSTATE_ONLINE_PLAYING,
    SMTAUDIO_SUBSTATE_ONLINE_PAUSE,
    SMTAUDIO_SUBSTATE_ONLINE_STOP,

    SMTAUDIO_SUBSTATE_LOCAL_PLAYING,
    SMTAUDIO_SUBSTATE_LOCAL_PAUSE,
    SMTAUDIO_SUBSTATE_LOCAL_STOP,

    SMTAUDIO_SUBSTATE_BT_A2DP_PLAYING,
    SMTAUDIO_SUBSTATE_BT_A2DP_PAUSE,
    SMTAUDIO_SUBSTATE_BT_A2DP_STOP,

    SMTAUDIO_SUBSTATE_BT_HFP_PLAYING,
    SMTAUDIO_SUBSTATE_BT_HFP_PAUSE,
    SMTAUDIO_SUBSTATE_BT_HFP_STOP,

    SMTAUDIO_SUBSTATE_MUTE,

    SMTAUDIO_SUBSTATE_NUM
} smtaudio_sub_state_t;

typedef enum {
    SMTAUDIO_PLAYER_EVENT_ERROR = AUI_PLAYER_EVENT_ERROR,
    SMTAUDIO_PLAYER_EVENT_START = AUI_PLAYER_EVENT_START,
    SMTAUDIO_PLAYER_EVENT_STOP = AUI_PLAYER_EVENT_FINISH,
    SMTAUDIO_PLAYER_EVENT_RESUME = AUI_PLAYER_EVENT_RESUME,
    SMTAUDIO_PLAYER_EVENT_UNDER_RUN = AUI_PLAYER_EVENT_UNDER_RUN,
    SMTAUDIO_PLAYER_EVENT_OVER_RUN = AUI_PLAYER_EVENT_OVER_RUN,
    SMTAUDIO_PLAYER_EVENT_PAUSE = AUI_PLAYER_EVENT_PAUSE,
} smtaudio_player_evtid_t;

typedef struct smtaudio_ops_node {
    const char *name;
    int         prio;
    int         id;
    msp_dlist_t     node;
    int         status;
    int (*init)(void);
    int (*deinit)(void);
    int (*start)(const char *url, uint64_t seek_time, int resume);
    int (*pause)(void);
    int (*stop)(void);
    int (*resume)(void);
    int (*vol_get)(void);
    int (*vol_set)(int vol);
    int (*vol_up)(int vol);
    int (*vol_down)(int vol);
    void (*callback)(int type, smtaudio_player_evtid_t evt_id);
} smtaudio_ops_node_t;

typedef struct smtaudio_delay_list_node {
    msp_dlist_t     node;
    int         prio;
    int         id;
    char *      url;
    int         seek_time;
    int         delay_reason;
    int         valid;
} smtaudio_delay_list_node_t;

typedef struct smtaudio_broken_list_node {
    msp_dlist_t     node;
    int         prio;
    int         id;
    int         interrupt_reason;
    int         valid;
} smtaudio_resume_list_node_t;

typedef struct audio_result {
    int type;
    int evt_id;
} audio_result_t;

typedef struct _audio_vol_config {
    int32_t db_min;    /* init factor of the vol scale */
    int32_t db_max;    /* init factor of the vol scale */
    uint8_t *map;      /* size=101, 0~100 vol map to db index */
} audio_vol_config_t;

/**
 * @brief  audio control ops register
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_ops_register(smtaudio_ops_node_t *ops);

/**
 * @brief  audio control register for system play
 * @param  [in] min_vol        : minimum volume, if current system vol is 0, TTS will be set to min_vol
 * @param  [in] aef_conf       : audio effect config data buffer point, disable aef set NULL
 * @param  [in] aef_conf_size  : audio effect config data buffer size
 * @param  [in] speed          : audio play init speed, >=0.5 && <=2
 * @param  [in] resample       : resample rate, 16000, 44100, 48000
 * @return 0 on success, negative value on failed
 */
typedef void (*audio_evt_t)(int type, smtaudio_player_evtid_t evt_id);
int8_t smtaudio_register_local_play(uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample);

/**
 * @brief  audio control register for bt music
 * @param  [in] min_vol        : minimum volume, if current system vol is 0, TTS will be set to min_vol
 * @param  [in] aef_conf       : audio effect config data buffer point, disable aef set NULL
 * @param  [in] aef_conf_size  : audio effect config data buffer size
 * @param  [in] speed          : audio play init speed, >=0.5 && <=2
 * @param  [in] resample       : resample rate, 16000, 44100, 48000
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_register_bt_a2dp(uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample);

/**
 * @brief  audio control register for bt call
 * @param  [in] min_vol        : minimum volume, if current system vol is 0, TTS will be set to min_vol
 * @param  [in] aef_conf       : audio effect config data buffer point, disable aef set NULL
 * @param  [in] aef_conf_size  : audio effect config data buffer size
 * @param  [in] speed          : audio play init speed, >=0.5 && <=2
 * @param  [in] resample       : resample rate, 16000, 44100, 48000
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_register_bt_hfp(uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample);

/**
 * @brief  audio control register for online music
 * @param  [in] min_vol        : minimum volume, if current system vol is 0, TTS will be set to min_vol
 * @param  [in] aef_conf       : audio effect config data buffer point, disable aef set NULL
 * @param  [in] aef_conf_size  : audio effect config data buffer size
 * @param  [in] speed          : audio play init speed, >=0.5 && <=2
 * @param  [in] resample       : resample rate, 16000, 44100, 48000
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_register_online_music(uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample);

/**
 * @brief  init smart audio
 * @param  [in] audio_evt_cb   : callback function to handle audio event
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_init(audio_evt_t audio_evt_cb);

/**
 * @brief  smtaudio_vol_config
 * @param  [in]vol_config: volume scale config
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_vol_config(audio_vol_config_t *vol_config);

/**
 * @brief  increase volume
 * @param  [in] vol             : volume step
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_vol_up(int16_t vol);

/**
 * @brief  decrease volume
 * @param  [in] vol             : volume step
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_vol_down(int16_t vol);

/**
 * @brief  set volume
 * @param  [in] vol             : target volume
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_vol_set(int16_t vol);

/**
 * @brief  get current volume
 * @return current volume, should be in 0~100
 */
int8_t smtaudio_vol_get(void);

/**
 * @brief  pause music playback
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_pause(void);

/**
 * @brief  mute music
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_mute(void);

/**
 * @brief  resume music playback
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_resume(void);

/**
 * @brief  start playing new stream
 * @param  [in] type
 * @param  [in] url             : stream url
 * @param  [in] seek_time       : 0 - play from beginning
 *                                >0 - play from seek position
 * @param  [in] resume          : resume music automatically
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_start(int type, char *url, uint64_t seek_time, uint8_t resume);

/**
 * @brief  stop playing current stream
 * @param  [in] type
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_stop(int type);

#define smtaudio_set_speed(speed) aui_player_set_speed(MEDIA_MUSIC, speed)
#define smtaudio_get_speed(speed) aui_player_get_speed(MEDIA_MUSIC, speed)

/**
 * @brief  set smart audio to lpm mode
 * @param  [in] state          : 0 - exit lpm, else - enter lpm
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_lpm(uint8_t state);

/**
 * @brief  get current smart audio state
 * @return smart audio state
 */
smtaudio_state_t smtaudio_get_state(void);

/**
 * @brief  get current smart audio state
 * @return smart audio play type
 */
smtaudio_player_type_t smtaudio_get_play_type(void);

/**
 * @brief  clear resume/delay_play node
 */
void smtaudio_clear_ready_list(void);

/**
 * @brief  enable/disable current node in resume/delay_play
 */
void smtaudio_enable_ready_list(int enable);

/**
 * @brief  smart audio check state for lpm 
 * @return 0 forbid, 1 allow
 */
int smtaudio_enter_lpm_check(void);

#endif //__SMART_AUDIO_H__