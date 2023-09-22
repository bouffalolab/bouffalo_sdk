/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdint.h>
#include <output/ao.h>

#include <msp/kernel.h>
// #include <ulog/ulog.h>

#define TAG "VOLEXT"

static ao_cls_t   *g_ao_ext    = NULL;
static uint8_t    *g_vol_map   = NULL;
static msp_mutex_t g_vol_mutex = NULL;

void smtaudio_btvol_set_attach_ao(ao_cls_t *ao, uint8_t *vol_map)
{
    if (!msp_mutex_is_valid(&g_vol_mutex)) {
        msp_mutex_new(&g_vol_mutex);
    }

    msp_mutex_lock(&g_vol_mutex, MSP_WAIT_FOREVER);
    g_ao_ext  = ao;
    g_vol_map = vol_map;
    msp_mutex_unlock(&g_vol_mutex);
}

void smtaudio_btvol_set_vol(int vol)
{
    size_t     size;
    ovol_set_t para;

    if (!msp_mutex_is_valid(&g_vol_mutex)) {
        return;
    }

    if (g_ao_ext == NULL) {
        return;
    }

    if (g_vol_map == NULL) {
        LOGE(TAG, "vol map error");
        return;
    }

    msp_mutex_lock(&g_vol_mutex, MSP_WAIT_FOREVER);
    if (vol < 0) {
        vol = 0;
    }

    if (vol > 100) {
        vol = 100;
    }

    size           = sizeof(para);
    para.vol_index = g_vol_map[vol];
    ao_control(g_ao_ext, AO_CMD_VOL_SET, (void *)&para, &size);

    msp_mutex_unlock(&g_vol_mutex);
}
