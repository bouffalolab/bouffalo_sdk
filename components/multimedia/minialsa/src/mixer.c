/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <xutils/types.h>
#include <xutils/debug.h>
#include <xutils/list.h>
// #include <xutils/xringbuffer.h>
#include <msp/kernel.h>

#include <alsa/mixer.h>
#include <alsa/snd.h>

#define TAG "mixer"

#define ELEM_LOCK(elem) msp_mutex_lock(&elem->mutex, MSP_WAIT_FOREVER)
#define ELEM_UNLOCK(elem) msp_mutex_unlock(&elem->mutex)

int msp_mixer_open(msp_mixer_t **mixer_ret, int mode)
{
    msp_mixer_t *mixer;

    mixer = msp_zalloc_check(sizeof(msp_mixer_t));

    *mixer_ret = mixer;
    return 0;
}

int msp_mixer_close(msp_mixer_t *mixer)
{
    msp_free(mixer);

    return 0;
}

int msp_mixer_attach(msp_mixer_t *mixer, const char *card_name)
{
    card_dev_t *card;

    msp_check_return_einval(mixer && card_name);

    msp_card_attach(card_name, &card);
    if (card == NULL) {
        return -1;
    }
    mixer->card = card;

    return 0;
}

static void elem_inter_init(elem_inter_t *inter, int cur, int min, int max)
{
    inter->cur = cur;
    inter->min = min;
    inter->max = max;
}

int snd_elem_new(msp_mixer_elem_t **elem_ret, const char *name, sm_elem_ops_t *ops)
{
    msp_mixer_elem_t *elem = msp_zalloc_check(sizeof(msp_mixer_elem_t));

    elem->name = name;
    elem->ops = ops;
    elem_inter_init(&elem->vol_l, 50, 0, 100);
    elem_inter_init(&elem->vol_r, 50, 0, 100);
    // elem->dB_l.cur = elem->dB_r.cur = ops->volume_to_dB(elem, elem->vol_r.cur);
    // ops->set_dB(elem, elem->dB_l.cur, elem->dB_r.cur);
    msp_mutex_new(&elem->mutex);

    *elem_ret = elem;
    return 0;
}

int msp_mixer_selem_register(msp_mixer_t *mixer, struct msp_mixer_selem_regopt *options, void **classp)
{

    return 0;
}

int msp_mixer_load(msp_mixer_t *mixer)
{
    msp_check_return_einval(mixer && mixer->card);
    snd_card_drv_t *drv = (snd_card_drv_t *)mixer->card->device.drv;

    mixer->elems_head = drv->mixer_head;

    return 0;
}

msp_mixer_elem_t *msp_mixer_first_elem(msp_mixer_t *mixer)
{
    msp_check_return_val(mixer && mixer->card, NULL);

    msp_mixer_elem_t *elem;

    elem = msp_slist_first_entry(&mixer->elems_head, msp_mixer_elem_t, next);
    mixer->cur_elem = elem;

    return elem;
}

msp_mixer_elem_t *msp_mixer_last_elem(msp_mixer_t *mixer)
{
    msp_check_return_val(mixer && mixer->card, NULL);

    msp_mixer_elem_t *elem;
    msp_slist_t *tmp;

    msp_slist_for_each_entry_safe(&mixer->elems_head, tmp, elem, msp_mixer_elem_t, next) {
        mixer->cur_elem = elem;
    }

    return mixer->cur_elem;
}

msp_mixer_elem_t *msp_mixer_elem_next(msp_mixer_elem_t *elem_p)
{
    msp_check_return_val(elem_p, NULL);

    msp_mixer_elem_t *elem = elem_p->mixer->cur_elem;

    elem = msp_slist_entry(&elem_p->next, msp_mixer_elem_t, next);

    return elem;
}

msp_mixer_elem_t *msp_mixer_elem_prev(msp_mixer_elem_t *elem_p)
{
    msp_check_return_val(elem_p, NULL);

    msp_mixer_elem_t *elem = NULL;

    return elem;
}

static int _volum_to_dB(msp_mixer_elem_t *elem, int volum)
{
    ELEM_LOCK(elem);
    int dB = elem->ops->volume_to_dB(elem, volum);
    ELEM_UNLOCK(elem);

    return dB;
}

static int _msp_mixer_elem_volume(msp_mixer_elem_t *elem, int l_dB, int r_dB)
{
    ELEM_LOCK(elem);
    elem->ops->set_dB(elem, l_dB, r_dB);
    ELEM_UNLOCK(elem);
    return 0;
}

int msp_mixer_selem_set_playback_volume(msp_mixer_elem_t *elem, msp_mixer_selem_channel_id_t channel, int value)
{
    msp_check_return_einval(elem);

    int dB = _volum_to_dB(elem, value);
    int ret = -1;

    if(MSP_MIXER_SCHN_FRONT_LEFT == channel) {
        ret = _msp_mixer_elem_volume(elem, dB, elem->dB_r.cur);
        if(ret == 0) {
            elem->dB_l.cur = dB;
            elem->vol_l.cur = value;
        }
    } else if(MSP_MIXER_SCHN_FRONT_RIGHT == channel) {
        ret = _msp_mixer_elem_volume(elem, elem->dB_l.cur, dB);
        if(ret == 0) {
            elem->dB_r.cur = dB;
            elem->vol_r.cur = value;
        }
    }

    return ret;
}

int msp_mixer_selem_set_playback_db(msp_mixer_elem_t *elem, msp_mixer_selem_channel_id_t channel, int db)
{
    msp_check_return_einval(elem);

    int ret = -1;

    if(MSP_MIXER_SCHN_FRONT_LEFT == channel) {
        ret = _msp_mixer_elem_volume(elem, db, elem->dB_r.cur);
        if(ret == 0) {
            elem->dB_l.cur = db;
        }
    } else if(MSP_MIXER_SCHN_FRONT_RIGHT == channel) {
        ret = _msp_mixer_elem_volume(elem, elem->dB_l.cur, db);
        if(ret == 0) {
            elem->dB_r.cur = db;
        }
    }

    return ret;
}

int msp_mixer_selem_set_capture_volume(msp_mixer_elem_t *elem, msp_mixer_selem_channel_id_t channel, int value)
{
    msp_check_return_einval(elem);

    return 0;
}

int msp_mixer_selem_set_playback_volume_all(msp_mixer_elem_t *elem, int value)
{
    msp_check_return_einval(elem);

    int dB = _volum_to_dB(elem, value);

    int ret = _msp_mixer_elem_volume(elem, dB, dB);
    if(ret == 0) {
        elem->dB_l.cur = elem->dB_r.cur = dB;
        elem->vol_l.cur = elem->vol_r.cur = value;
    }

    return ret;
}

int msp_mixer_selem_set_playback_db_all(msp_mixer_elem_t *elem, int db)
{
    msp_check_return_einval(elem);

    int ret = _msp_mixer_elem_volume(elem, db, db);
    if(ret == 0) {
        elem->dB_l.cur = elem->dB_r.cur = db;
    }

    return ret;
}

int msp_mixer_selem_set_capture_volume_all(msp_mixer_elem_t *elem, int value)
{
    msp_check_return_einval(elem);

    return 0;
}

int msp_mixer_selem_get_playback_volume(msp_mixer_elem_t *elem, msp_mixer_selem_channel_id_t channel, int *value)
{
    msp_check_return_einval(elem && value);

    if(MSP_MIXER_SCHN_FRONT_LEFT == channel) {
        *value = elem->vol_l.cur;
    } else if(MSP_MIXER_SCHN_FRONT_RIGHT == channel) {
        *value = elem->vol_r.cur;
    }

    return 0;
}
