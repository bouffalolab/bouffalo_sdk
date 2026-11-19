/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef XCODEC_CTRL_IF_H
#define XCODEC_CTRL_IF_H

#include "xcodec_dev_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xcodec_ctrl_if_t xcodec_ctrl_if_t;
struct xcodec_ctrl_if_t {
    int (*open)(const xcodec_ctrl_if_t *ctrl, void *cfg, int cfg_size);
    bool (*is_open)(const xcodec_ctrl_if_t *ctrl);
    int (*read_reg)(const xcodec_ctrl_if_t *ctrl, int reg, int reg_len, void *data, int data_len);
    int (*write_reg)(const xcodec_ctrl_if_t *ctrl, int reg, int reg_len, void *data, int data_len);
    int (*close)(const xcodec_ctrl_if_t *ctrl);
};

/* Closes and frees a heap-allocated interface; only call after all borrowing
 * device handles are deleted. A failed close leaves ownership with caller. */
int xcodec_delete_ctrl_if(const xcodec_ctrl_if_t *interface);

#ifdef __cplusplus
}
#endif
#endif
