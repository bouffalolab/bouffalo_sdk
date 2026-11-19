/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef XCODEC_AUDIO_CODEC_H
#define XCODEC_AUDIO_CODEC_H

#include "xcodec_if.h"
#if defined(CONFIG_BSP_AUDIO_CODEC)
#include "audio_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The board selects the BSP codec and retains bus/clock ownership.
 * init_cfg initializes the hardware once; NULL borrows an initialized codec.
 * Delete all borrowing interfaces before deleting the initializing interface.
 * Output interfaces mute during creation; the initializer deinitializes on close.
 * Each interface gates only its configured recording/playback path.
 * acquire/release are an optional pair protecting shared control-bus access.
 * Device volume follows the existing BSP percentage curve, not physical dB.
 * init_cfg supplies the initial hardware format. Device open supplies the
 * running sample rate through the BSP; data interfaces constrain width/slots.
 * Native Codec ownership stays with the caller, including creation failure.
 * Board-managed Codecs initialize/deinitialize in the Board and omit init_cfg.
 */
typedef struct {
    audio_codec_dev_t *codec;
    const audio_codec_cfg_t *init_cfg; /* Read only during open/new. */
    int (*acquire)(void);
    void (*release)(void);
    audio_codec_work_mode_t mode;
} xcodec_audio_codec_cfg_t;
const xcodec_if_t *xcodec_ctrl_if_new(const xcodec_audio_codec_cfg_t *cfg);

#ifdef __cplusplus
}
#endif
#endif
#endif
