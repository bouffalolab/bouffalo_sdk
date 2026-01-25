#include "audio_codec.h"

#include <stddef.h>

#include "bflb_i2c.h"

#include "audio_codec_driver.h"
#include "audio_codec_es8311.h"
#include "audio_codec_es8389.h"
#include "audio_codec_es8388.h"
#include "audio_codec_wm8978.h"

static const audio_codec_driver_t *g_drivers[] = {
    NULL,
    &audio_codec_es8311_driver,
    &audio_codec_es8389_driver,
    &audio_codec_es8388_driver,
    &audio_codec_wm8978_driver,
};

audio_codec_type_t audio_codec_scan(struct bflb_device_s *i2c, audio_codec_dev_t *out_dev)
{
    if (out_dev == NULL || i2c == NULL) {
        return AUDIO_CODEC_TYPE_UNKNOWN;
    }

    out_dev->type = AUDIO_CODEC_TYPE_UNKNOWN;
    out_dev->name = NULL;
    out_dev->i2c = i2c;
    out_dev->i2c_addr = 0;
    out_dev->drv = NULL;

    for (size_t di = 0; di < (sizeof(g_drivers) / sizeof(g_drivers[0])); di++) {
        const audio_codec_driver_t *drv = g_drivers[di];
        if (drv == NULL) {
            continue;
        }
        for (uint8_t ai = 0; ai < drv->i2c_addrs_num; ai++) {
            uint8_t addr = drv->i2c_addrs[ai];
            if (drv->probe(i2c, addr) == 0) {
                out_dev->type = drv->type;
                out_dev->name = drv->name;
                out_dev->i2c = i2c;
                out_dev->i2c_addr = addr;
                out_dev->drv = drv;
                return out_dev->type;
            }
        }
    }

    return AUDIO_CODEC_TYPE_UNKNOWN;
}

int audio_codec_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    if (dev == NULL || cfg == NULL) {
        return -1;
    }

    const audio_codec_driver_t *drv = (const audio_codec_driver_t *)dev->drv;
    if (drv == NULL || drv->init == NULL) {
        return -2;
    }

    return drv->init(dev, cfg);
}

int audio_codec_set_mute(audio_codec_dev_t *dev, bool mute)
{
    if (dev == NULL) {
        return -1;
    }

    const audio_codec_driver_t *drv = (const audio_codec_driver_t *)dev->drv;
    if (drv == NULL || drv->set_mute == NULL) {
        return -2;
    }

    return drv->set_mute(dev, mute);
}

int audio_codec_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL) {
        return -1;
    }

    const audio_codec_driver_t *drv = (const audio_codec_driver_t *)dev->drv;
    if (drv == NULL || drv->set_volume == NULL) {
        return -2;
    }

    return drv->set_volume(dev, volume_percent);
}

int audio_codec_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL) {
        return -1;
    }

    const audio_codec_driver_t *drv = (const audio_codec_driver_t *)dev->drv;
    if (drv == NULL || drv->set_record_volume == NULL) {
        return -2;
    }

    return drv->set_record_volume(dev, volume_percent);
}

int audio_codec_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    if (dev == NULL) {
        return -1;
    }

    const audio_codec_driver_t *drv = (const audio_codec_driver_t *)dev->drv;
    if (drv == NULL || drv->set_mic_pga == NULL) {
        return -2;
    }

    return drv->set_mic_pga(dev, pga_percent);
}

int audio_codec_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    if (dev == NULL) {
        return -1;
    }

    const audio_codec_driver_t *drv = (const audio_codec_driver_t *)dev->drv;
    if (drv == NULL || drv->set_sample_rate == NULL) {
        return -2;
    }

    return drv->set_sample_rate(dev, sample_rate_hz);
}

uint32_t audio_codec_calc_mclk_hz(const audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    if (sample_rate_hz == 0) {
        return 0;
    }

    if (dev != NULL && dev->drv != NULL) {
        const audio_codec_driver_t *drv = (const audio_codec_driver_t *)dev->drv;
        if (drv->calc_mclk_hz != NULL) {
            return drv->calc_mclk_hz(sample_rate_hz);
        }
    }

    return sample_rate_hz * 256U;
}
