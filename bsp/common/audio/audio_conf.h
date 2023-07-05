#ifndef __AUDIO_CONFIG_H__
#define __AUDIO_CONFIG_H__

#if __has_include("audio_conf_user.h")

#include "audio_conf_user.h"

#else

#warning "There is no user AUDIO conf file and the default conf will be used"

/* Select audio codec Type, Optional:

bouffalo internal codec:
    BL_AUDIO_CODEC

i2s ext codec:
    AUDIO_I2S_ES8311
    AUDIO_I2S_Es8388

*/

#define AUDIO_I2S_ES8388

#if defined BL_AUDIO_CODEC

//todo

#elif defined AUDIO_I2S_ES8388

//todo

#elif defined AUDIO_I2S_ES8311

//todo

#endif

#endif
#endif // __AUDIO_CONFIG_H__