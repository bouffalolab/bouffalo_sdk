#ifndef __DRV_SOUND_BL606P_H__
#define __DRV_SOUND_BL606P_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_MIC_TYPE_ANALOG           0
#define INPUT_MIC_TYPE_DIGITAL          1

typedef struct {
    int audio_in_gain_list[3];
    int audio_out_gain_list[2];
    uint32_t        mic_type;                   ///< MIC type:
    uint32_t        pdm_channel;                ///< used for digital,pdm0, pdm1
    uint32_t        pdm_in_pin;                 ///< used for digital
    uint32_t        pdm_clk_pin;                ///< used for digital
    uint32_t        pa_pin;
} snd_bl606p_config_t;

extern void snd_card_bl606p_register(void *config);

#ifdef __cplusplus
}
#endif

#endif
