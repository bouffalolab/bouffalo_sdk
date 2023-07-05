
#include "audio.h"

int audio_init(struct audio_cfg *cfg)
{
    int ret = 0;

    ret = _AUDIO_FUNC_DEFINE(init, cfg);

    return ret;
}

int audio_play(struct audio_cfg *cfg)
{
    int ret = 0;

    ret = _AUDIO_FUNC_DEFINE(play, NULL);

    return 0;
}