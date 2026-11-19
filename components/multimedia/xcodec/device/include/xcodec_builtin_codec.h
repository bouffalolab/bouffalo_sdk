/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef XCODEC_BUILTIN_CODEC_H
#define XCODEC_BUILTIN_CODEC_H

#include "xcodec_if.h"

struct bflb_device_s;

#ifdef __cplusplus
extern "C" {
#endif

/* The data interface initializes converters and owns DMA/start/stop.
 * This interface only controls gain/mute on the supplied peripherals.
 * AUDAC gain: -95..18 dB rounded to integer dB by the LHAL control API;
 * lower requests saturate at -95 dB.
 * AUADC analog PGA: 6..42 dB in 3 dB steps; PDM gain is unsupported.
 * AUADC mute is unsupported by the LHAL control API.
 */
typedef struct {
    struct bflb_device_s *audac;
    struct bflb_device_s *auadc;
    bool analog_input;
} xcodec_builtin_codec_cfg_t;
const xcodec_if_t *xcodec_builtin_codec_new(const xcodec_builtin_codec_cfg_t *cfg);

#ifdef __cplusplus
}
#endif
#endif
