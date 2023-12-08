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

typedef void * pcm_handle_t;

struct pcm_drv {
    pcm_handle_t *(*pcm_open)(int mode, int samplerate, int channels, int format);
    int (*pcm_write)(pcm_handle_t pcm, const void *buf, unsigned int size);
    int (*pcm_start)(pcm_handle_t pcm);
    int (*pcm_stop)(pcm_handle_t pcm);
    int (*pcm_ioctl)(pcm_handle_t pcm, int cmd, void *arg);
    void (*pcm_close)(pcm_handle_t pcm);
};

const struct pcm_drv *pcm_drv_register(void);

