/*
 * Copyright (C) 2016-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include "smart_audio.h"
//#include "ulog/ulog.h"
//#include <aos/kv.h>
#include <avutil/misc.h>
#include <media.h>

#define TAG "smtaudio_ctrl_local_play"

extern void smtaudio_btvol_set_vol(int vol);

static utask_t *aui_task_media = NULL;
static aui_player_config_t media_config;
static audio_vol_config_t *g_vol_config = NULL;

static int local_play_init(void);
static int local_play_deinit(void);
static int local_play_start(const char *url, uint64_t seek_time, int resume);
static int local_play_pause(void);
static int local_play_stop(void);
static int local_play_resume(void);
static int local_play_vol_get(void);
static int local_play_vol_set(int vol);
static int local_play_vol_up(int vol);
static int local_play_vol_down(int vol);

static smtaudio_ops_node_t ctrl_local_play = {
    .name     = "local_play",
    .prio     = 1,
    .id       = SMTAUDIO_LOCAL_PLAY,
    .status   = SMTAUDIO_STATE_STOP,
    .init     = local_play_init,
    .deinit   = local_play_deinit,
    .start    = local_play_start,
    .pause    = local_play_pause,
    .stop     = local_play_stop,
    .resume   = local_play_resume,
    .vol_get  = local_play_vol_get,
    .vol_set  = local_play_vol_set,
    .vol_up   = local_play_vol_up,
    .vol_down = local_play_vol_down,
};

#if 0
static int media_dec_key_cb(const void *in, size_t ilen, void *out, size_t *olen)
{
    int rc;

    CHECK_PARAM(in && ilen >= 3 && out && olen && *olen >= 16, -1);

    LOGD(TAG, "mp4 major brand is %s", in);
    char cenc_decrypt_key[33] = {0}; // "6ABED02448B8FFD2224FF54619935526"

    if (aos_kv_getstring("mp4_key", cenc_decrypt_key, 33) > 0) {
        rc = bytes_from_hex(cenc_decrypt_key, out, *olen);
        CHECK_RET_TAG_WITH_RET(rc == 0, -1);
        *olen = 16;
        return 0;
    }

    LOGW(TAG, "major brand %s get key failed", in);
    return -1;
}
#endif

//系统音、在线音乐回调函数
static void _aui_evt_cb_(int type, const char *url, aui_player_evtid_t evt_id, const void *data, size_t size)
{
    ctrl_local_play.callback(type, evt_id);
}

static int media_init(uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample)
{
#if defined(CONFIG_AV_DBUS_SUPPORT) && CONFIG_AV_DBUS_SUPPORT
    return 0;
#endif
    int ret = -1;

    ret = aui_player_init(aui_task_media, _aui_evt_cb_);

    if (ret == 0) {
        aui_player_config_init(&media_config);

        media_config.web_cache_size      = 20 * 1024;
        media_config.web_start_threshold = 30;
        media_config.resample_rate       = resample;
        if (aef_conf && aef_conf_size) {
            media_config.aef_conf      = aef_conf;
            media_config.aef_conf_size = aef_conf_size;
        }

        if (g_vol_config) {
            media_config.db_min = g_vol_config->db_min;
            media_config.db_max = g_vol_config->db_max;
            if (g_vol_config->map) {
                media_config.vol_map = g_vol_config->map;
            }
        }
#if 0
        if (speed >= 0.5f && speed <= 2.0f) {
            media_config.speed = speed;
        } else {
            media_config.speed = 1;
        }
#endif
        aui_player_config(&media_config);
    }

    return ret;
}

static int local_play_init(void)
{
    aui_task_media = utask_new("task_media", 4 * 1024, QUEUE_MSG_COUNT, MSP_DEFAULT_APP_PRI);
    if (NULL == aui_task_media) {
        LOGE(TAG, "media task created failed!");
        return -EINVAL;
    }
    return 0;
}

static int local_play_deinit(void)
{
    if (NULL != aui_task_media) {
        utask_destroy(aui_task_media);
        aui_task_media = NULL;
    }
    return 0;
}

static int local_play_start(const char *url, uint64_t seek_time, int resume)
{
    int ret = -1;
    if (url) {
        ret = aui_player_seek_play(SMTAUDIO_LOCAL_PLAY, url, seek_time, resume);
    } else {
        LOGD(TAG, "%s url invalid", __func__);
    }
    return ret;
}

static int local_play_pause(void)
{
    ctrl_local_play.status = SMTAUDIO_STATE_PAUSE;
    int ret                = aui_player_pause(SMTAUDIO_LOCAL_PLAY);
    if (ctrl_local_play.callback) {
        ctrl_local_play.callback(SMTAUDIO_LOCAL_PLAY, SMTAUDIO_PLAYER_EVENT_PAUSE);
    }
    return ret;
}

static int local_play_stop(void)
{
    ctrl_local_play.status = SMTAUDIO_STATE_STOP;
    int ret                = aui_player_stop(SMTAUDIO_LOCAL_PLAY);
    if (ctrl_local_play.callback) {
        ctrl_local_play.callback(SMTAUDIO_LOCAL_PLAY, SMTAUDIO_PLAYER_EVENT_STOP);
    }
    return ret;
}

static int local_play_resume(void)
{
    return aui_player_resume(SMTAUDIO_LOCAL_PLAY);
}

static int local_play_vol_get(void)
{
    return aui_player_vol_get(SMTAUDIO_LOCAL_PLAY);
}

static int local_play_vol_set(int vol)
{
    int ret = 0;

    aui_player_vol_set(SMTAUDIO_TYPE_ALL, vol);

#if defined(CONFIG_BT_A2DP) && CONFIG_BT_A2DP
    smtaudio_btvol_set_vol(vol);
#endif

    // ret = aos_kv_setint(VOLUME_SAVE_KV_NAME, vol);
    // if (0 != ret) {
    //     LOGE(TAG, "set kv volume failed! ret[%d]", ret);
    // }
    return ret;
}

static int local_play_vol_up(int vol)
{
    int ret = 0;
    int cur_vol = local_play_vol_get();

    aui_player_vol_adjust(SMTAUDIO_TYPE_ALL, vol);

#if defined(CONFIG_BT_A2DP) && CONFIG_BT_A2DP
    smtaudio_btvol_set_vol(cur_vol + vol);
#endif

    // ret = aos_kv_setint(VOLUME_SAVE_KV_NAME, cur_vol + vol);
    // if (0 != ret) {
    //     LOGE(TAG, "set kv volume failed! ret[%d]", ret);
    // }
    return ret;
}

static int local_play_vol_down(int vol)
{
    int ret = 0;
    int cur_vol = local_play_vol_get();

    aui_player_vol_adjust(SMTAUDIO_TYPE_ALL, -vol);

#if defined(CONFIG_BT_A2DP) && CONFIG_BT_A2DP
    smtaudio_btvol_set_vol(cur_vol - vol);
#endif

    // ret = aos_kv_setint(VOLUME_SAVE_KV_NAME, cur_vol - vol);

    // if (0 != ret) {
    //     LOGE(TAG, "set kv volume failed! ret[%d]", ret);
    // }
    return ret;
}

int8_t smtaudio_vol_config(audio_vol_config_t *vol_config)
{
    g_vol_config = vol_config;

    return 0;
}

int8_t smtaudio_register_local_play(uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample)
{
    int      vol = SMART_AUDIO_DEFAULT_VOLUME;
    int8_t   ret;

    ret = smtaudio_ops_register(&ctrl_local_play);
    ctrl_local_play.init();

    media_init(aef_conf, aef_conf_size, speed, resample);

    // aui_player_key_config(media_dec_key_cb);
    // aui_player_set_minvol(MEDIA_ALL, (min_vol <= 0) ? 20 : min_vol);

    // aos_kv_getint(VOLUME_SAVE_KV_NAME, &vol);
    ctrl_local_play.vol_set(((vol == 0) ? 20 : vol));

    return ret;
}