/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef XCODEC_IF_H
#define XCODEC_IF_H

#include "xcodec_dev_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xcodec_if_t xcodec_if_t;
struct xcodec_if_t {
    int (*open)(const xcodec_if_t *h, void *cfg, int cfg_size);
    bool (*is_open)(const xcodec_if_t *h);
    int (*enable)(const xcodec_if_t *h, bool enable);
    int (*set_fs)(const xcodec_if_t *h, xcodec_dev_sample_info_t *fs);
    int (*mute)(const xcodec_if_t *h, bool mute);
    int (*set_vol)(const xcodec_if_t *h, float db);
    int (*set_mic_gain)(const xcodec_if_t *h, float db);
    int (*set_mic_channel_gain)(const xcodec_if_t *h, uint16_t channel_mask, float db);
    int (*mute_mic)(const xcodec_if_t *h, bool mute);
    int (*set_reg)(const xcodec_if_t *h, int reg, int value);
    int (*get_reg)(const xcodec_if_t *h, int reg, int *value);
    void (*dump_reg)(const xcodec_if_t *h);
    int (*close)(const xcodec_if_t *h);
};

/* Closes and frees a heap-allocated interface; only call after all borrowing
 * device handles are deleted. A failed close leaves ownership with caller. */
int xcodec_delete_codec_if(const xcodec_if_t *interface);

#ifdef __cplusplus
}
#endif
#endif
