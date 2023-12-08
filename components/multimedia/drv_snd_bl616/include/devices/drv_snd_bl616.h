/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __DRV_SOUND_BL606P_H__
#define __DRV_SOUND_BL606P_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int audio_in_gain_list[3];
    int audio_out_gain_list[2];
    //uint32_t            in_negative_pin;            // input negative pin
    //uint32_t            in_positive_pin;            // input positive pin
    //uint32_t            out_negative_pin;           // output negative pin
    //uint32_t            out_positive_pin;           // output negative pin
    // uint32_t        mic_type;                   ///< MIC type:
    // uint32_t        pdm_channel;                ///< used for digital,pdm0, pdm1
    // uint32_t        pdm_in_pin;                 ///< used for digital
    // uint32_t        pdm_clk_pin;                ///< used for digital
    //uint32_t        pa_pin;
} snd_bl616_config_t;

extern void snd_card_bl616_register(void *config);
extern void snd_card_bl616_unregister(void *config);

#ifdef __cplusplus
}
#endif

#endif
