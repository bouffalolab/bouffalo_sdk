/**
 ****************************************************************************************
 *
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <atomic.h>
#include <assert.h>
#include "pcm_drv.h"
#include <msp/kernel.h>

struct sbc_i {
    uint32_t len;
    uint8_t *data;
};

struct sbc2pcm_ev {
#define PLAYER_EVENT_SBC_DATA 1
#define PLAYER_EVENT_OPEN     2
#define PLAYER_EVENT_START    3
#define PLAYER_EVENT_STOP     5
#define PLAYER_EVENT_CLOSE    4
    uint8_t event_type;
    struct sbc_i sbc;
};

struct sbc2pcm_handle {
    pcm_handle_t pcm;
    msp_task_t task;
    msp_queue_t queue;
    const struct pcm_drv *pdrv;
    uint8_t runing;
    uint8_t cache_flag;
    int mode;
    int samplerate;
    int channels;
    int format;
    float speed;
    uint64_t speed_restore_tm;
};

typedef struct sbc2pcm_handle *sbc2pcm_player_handle_t;

sbc2pcm_player_handle_t sbc2pcm_player_open(int mode, int samplerate, int channels, int format);

int sbc2pcm_player_write(sbc2pcm_player_handle_t handle, const void *buf, unsigned int size);

int sbc2pcm_player_start(sbc2pcm_player_handle_t handle);

int sbc2pcm_player_stop(sbc2pcm_player_handle_t handle);

void sbc2pcm_player_close(sbc2pcm_player_handle_t handle);

int sbc2pcm_decode_init();

