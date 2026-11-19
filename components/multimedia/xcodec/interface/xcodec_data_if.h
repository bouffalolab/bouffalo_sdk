/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef XCODEC_DATA_IF_H
#define XCODEC_DATA_IF_H

#include "xcodec_dev_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xcodec_data_if_t xcodec_data_if_t;
struct xcodec_data_if_t {
    int (*open)(const xcodec_data_if_t *h, void *cfg, int cfg_size);
    bool (*is_open)(const xcodec_data_if_t *h);
    /* Enable/disable only dev_type; preserve the opposite direction. */
    int (*enable)(const xcodec_data_if_t *h, xcodec_dev_type_t dev_type, bool enable);
    int (*set_fmt)(const xcodec_data_if_t *h, xcodec_dev_type_t dev_type, xcodec_dev_sample_info_t *fs);
    /* Accept byte lengths; retain partial PCM frames across calls, clear on close.
     * Return OK only after all size bytes transfer; positive byte counts are invalid. */
    int (*read)(const xcodec_data_if_t *h, uint8_t *data, int size);
    int (*write)(const xcodec_data_if_t *h, uint8_t *data, int size);
    int (*close)(const xcodec_data_if_t *h);
};

/* Closes and frees a heap-allocated interface; only call after all borrowing
 * device handles are deleted. A failed close leaves ownership with caller. */
int xcodec_delete_data_if(const xcodec_data_if_t *interface);

#ifdef __cplusplus
}
#endif
#endif
