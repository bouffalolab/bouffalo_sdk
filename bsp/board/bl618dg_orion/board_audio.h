/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef BOARD_BL618DG_ORION_AUDIO_H
#define BOARD_BL618DG_ORION_AUDIO_H

#include "xcodec_dev.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    const xcodec_if_t *play_codec;
    const xcodec_if_t *record_codec;
    const xcodec_data_if_t *play_data;
    const xcodec_data_if_t *record_data;
} xcodec_board_t;

typedef xcodec_board_t *xcodec_board_handle_t;

/* Board owns Codec/data interfaces; delete all borrowing devices before deinit.
 * Init creates Codec and board-selected data paths using xcodec channel defaults.
 * Serialize lifecycle/control calls; deinit before initializing again.
 */
xcodec_board_handle_t xcodec_dev_board_init(void);
int xcodec_dev_board_deinit(void);

#ifdef __cplusplus
}
#endif
#endif
