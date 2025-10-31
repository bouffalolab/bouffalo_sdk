/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <xutils/driver.h>

#ifndef __MSP_MIXER__
#define __MSP_MIXER__

#include <alsa/pcm.h>
#include <alsa/snd.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Mixer simple element channel identifier */
typedef enum _msp_mixer_selem_channel_id {
    /** Unknown */
    MSP_MIXER_SCHN_UNKNOWN = -1,
    /** Front left */
    MSP_MIXER_SCHN_FRONT_LEFT = 0,
    /** Front right */
    MSP_MIXER_SCHN_FRONT_RIGHT,
    /** Rear left */
    MSP_MIXER_SCHN_REAR_LEFT,
    /** Rear right */
    MSP_MIXER_SCHN_REAR_RIGHT,
    /** Front center */
    MSP_MIXER_SCHN_FRONT_CENTER,
    /** Woofer */
    MSP_MIXER_SCHN_WOOFER,
    /** Side Left */
    MSP_MIXER_SCHN_SIDE_LEFT,
    /** Side Right */
    MSP_MIXER_SCHN_SIDE_RIGHT,
    /** Rear Center */
    MSP_MIXER_SCHN_REAR_CENTER,
    MSP_MIXER_SCHN_LAST = 31,
    /** Mono (Front left alias) */
    MSP_MIXER_SCHN_MONO = MSP_MIXER_SCHN_FRONT_LEFT
} msp_mixer_selem_channel_id_t;

enum msp_mixer_selem_regopt_abstract {
    MSP_MIXER_SABSTRACT_NONE = 0,
    MSP_MIXER_SABSTRACT_BASIC,
};

typedef struct msp_mixer_selem_regopt {
    int ver;
    enum msp_mixer_selem_regopt_abstract abstract;
    const char *device;
    msp_pcm_t *playback_pcm;
    msp_pcm_t *capture_pcm;
} msp_mixer_selem_regopt_t;

typedef struct elem_inter {
    int cur;
    int min;
    int max;
} elem_inter_t;

typedef struct msp_mixer msp_mixer_t;
typedef struct _msp_mixer_elem msp_mixer_elem_t;

struct msp_mixer {
    const char *card_name;
    card_dev_t *card;
    msp_mixer_elem_t *cur_elem;
    msp_slist_t elems_head;
};

struct _msp_mixer_elem {
    const char *name;
    elem_inter_t vol_l;
    elem_inter_t vol_r;
    elem_inter_t dB_l;
    elem_inter_t dB_r;
    msp_mixer_t *mixer;
    msp_mutex_t mutex;
    void *hdl;
    struct sm_elem_ops *ops;
    msp_slist_t next;
};

typedef struct sm_elem_ops {
    int (*set_dB)(msp_mixer_elem_t *elem, int l_dB, int r_dB);
    int (*volume_to_dB)(msp_mixer_elem_t *elem, int value);
} sm_elem_ops_t;

int msp_mixer_open(msp_mixer_t **mixer, int mode);
int msp_mixer_close(msp_mixer_t *mixer);
int msp_mixer_selem_set_playback_volume(msp_mixer_elem_t *elem, msp_mixer_selem_channel_id_t channel, int value);
int msp_mixer_selem_set_capture_volume(msp_mixer_elem_t *elem, msp_mixer_selem_channel_id_t channel, int value);
int msp_mixer_selem_set_playback_volume_all(msp_mixer_elem_t *elem, int value);
int msp_mixer_selem_set_capture_volume_all(msp_mixer_elem_t *elem, int value);
int msp_mixer_selem_set_playback_volume_range(msp_mixer_elem_t *elem, int min, int max);
int msp_mixer_selem_set_capture_volume_range(msp_mixer_elem_t *elem, int min, int max);
int msp_mixer_selem_get_playback_volume(msp_mixer_elem_t *elem, msp_mixer_selem_channel_id_t channel, int *value);
int msp_mixer_load(msp_mixer_t *mixer);
int snd_elem_new(msp_mixer_elem_t **elem_ret, const char *name, sm_elem_ops_t *ops);
void msp_mixer_free(msp_mixer_t *mixer);
msp_mixer_elem_t *msp_mixer_first_elem(msp_mixer_t *mixer);
msp_mixer_elem_t *msp_mixer_last_elem(msp_mixer_t *mixer);
msp_mixer_elem_t *msp_mixer_elem_next(msp_mixer_elem_t *elem);
msp_mixer_elem_t *msp_mixer_elem_prev(msp_mixer_elem_t *elem); // not support
int msp_mixer_attach(msp_mixer_t *mixer, const char *name);

//db gain set
int msp_mixer_selem_set_playback_db(msp_mixer_elem_t *elem, msp_mixer_selem_channel_id_t channel, int db);
int msp_mixer_selem_set_playback_db_all(msp_mixer_elem_t *elem, int db);

#ifdef __cplusplus
}
#endif

#endif