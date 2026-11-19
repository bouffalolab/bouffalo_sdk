#include "audio_codec.h"

#include <stddef.h>
#include <math.h>

#include "bflb_i2c.h"

#include "audio_codec_driver.h"
#include "audio_codec_ac101s.h"
#include "audio_codec_acm8852.h"
#include "audio_codec_es8311.h"
#include "audio_codec_es8389.h"
#include "audio_codec_es8388.h"
#include "audio_codec_wm8978.h"
#ifdef CONFIG_AUDIO_CODEC_JY6316_ENABLE
#include "audio_codec_jy6316.h"
#endif
#ifdef CONFIG_AUDIO_CODEC_JY6416_ENABLE
#include "audio_codec_jy6416.h"
#endif
#include "audio_codec_jy6311.h"
#include "audio_codec_jy6166.h"
#ifdef CONFIG_BSP_AUDIO_CODEC_BL_ENABLE
#include "audio_codec_bl.h"
#endif

static const audio_codec_driver_t *g_drivers[] = {
#ifdef CONFIG_AUDIO_CODEC_JY6416_ENABLE
    &audio_codec_jy6416_driver,
#endif
#ifdef CONFIG_AUDIO_CODEC_JY6316_ENABLE
    &audio_codec_jy6316_driver,
#endif
    &audio_codec_jy6311_driver,
    &audio_codec_jy6166_driver,
    &audio_codec_es8311_driver,
    &audio_codec_es8389_driver,
    &audio_codec_es8388_driver,
    &audio_codec_wm8978_driver,
    &audio_codec_ac101s_driver,
    &audio_codec_acm8852_driver,
};

static void audio_codec_reset_dev(audio_codec_dev_t *dev)
{
    if (dev == NULL) {
        return;
    }

    dev->type = AUDIO_CODEC_TYPE_UNKNOWN;
    dev->name = NULL;
    dev->i2c = NULL;
    dev->i2c_addr = 0;
    dev->drv = NULL;
    dev->priv = NULL;
}

audio_codec_type_t audio_codec_scan(struct bflb_device_s *i2c, audio_codec_dev_t *out_dev)
{
    if (out_dev == NULL || i2c == NULL) {
        return AUDIO_CODEC_TYPE_UNKNOWN;
    }

    audio_codec_reset_dev(out_dev);
    out_dev->i2c = i2c;

    for (size_t di = 0; di < (sizeof(g_drivers) / sizeof(g_drivers[0])); di++) {
        const audio_codec_driver_t *drv = g_drivers[di];
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

#ifdef CONFIG_BSP_AUDIO_CODEC_BL_ENABLE
int audio_codec_open_builtin(audio_codec_dev_t *out_dev, const audio_codec_bl_hw_cfg_t *cfg)
{
    if (out_dev == NULL || cfg == NULL) {
        return -1;
    }

    audio_codec_reset_dev(out_dev);
    return audio_codec_bl_open(out_dev, cfg);
}
#endif

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

int audio_codec_get_mic_channel_caps(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps)
{
    if (dev == NULL || caps == NULL || slot >= AUDIO_CODEC_MAX_SLOTS) {
        return -1;
    }
    *caps = (audio_codec_channel_caps_t){ 0 };
    const audio_codec_driver_t *drv = dev->drv;
    return drv && drv->get_mic_channel_caps ? drv->get_mic_channel_caps(dev, slot, caps) : -2;
}

int audio_codec_get_speaker_channel_caps(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps)
{
    if (dev == NULL || caps == NULL || slot >= AUDIO_CODEC_MAX_SLOTS) {
        return -1;
    }
    *caps = (audio_codec_channel_caps_t){ 0 };
    const audio_codec_driver_t *drv = dev->drv;
    return drv && drv->get_speaker_channel_caps ? drv->get_speaker_channel_caps(dev, slot, caps) : -2;
}

int audio_codec_get_channel_masks(audio_codec_dev_t *dev, uint16_t *capture_mask, uint16_t *playback_mask)
{
    if (dev == NULL || capture_mask == NULL || playback_mask == NULL) {
        return -1;
    }
    *capture_mask = 0;
    *playback_mask = 0;
    for (uint8_t slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; slot++) {
        audio_codec_channel_caps_t caps;
        int ret = audio_codec_get_mic_channel_caps(dev, slot, &caps);
        if (ret == 0) {
            *capture_mask |= (uint16_t)(1U << slot);
        } else if (ret != -2) {
            return ret;
        }
        ret = audio_codec_get_speaker_channel_caps(dev, slot, &caps);
        if (ret == 0) {
            *playback_mask |= (uint16_t)(1U << slot);
        } else if (ret != -2) {
            return ret;
        }
    }
    return 0;
}

static int audio_codec_validate_channel_control(audio_codec_dev_t *dev, uint16_t mask, uint32_t control, bool speaker)
{
    if (dev == NULL || mask == 0) {
        return -1;
    }
    for (uint8_t slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; slot++) {
        if ((mask & (1U << slot)) != 0) {
            audio_codec_channel_caps_t caps;
            int ret = speaker ? audio_codec_get_speaker_channel_caps(dev, slot, &caps) :
                                audio_codec_get_mic_channel_caps(dev, slot, &caps);
            if (ret != 0) {
                return ret;
            }
            uint16_t group = (caps.independent_controls & control) != 0 ? (uint16_t)(1U << slot) : caps.linked_mask;
            if ((caps.controls & control) == 0 || group == 0 || (mask & group) != group) {
                return -2;
            }
        }
    }
    return 0;
}

int audio_codec_set_mic_channel_volume(audio_codec_dev_t *dev, uint16_t slot_mask, int volume_percent)
{
    if (volume_percent < 0 || volume_percent > 100) {
        return -1;
    }
    int ret = audio_codec_validate_channel_control(dev, slot_mask, AUDIO_CODEC_CONTROL_VOLUME, false);
    if (ret != 0) {
        return ret;
    }
    const audio_codec_driver_t *drv = dev->drv;
    ret = drv->set_mic_channel_volume ? drv->set_mic_channel_volume(dev, slot_mask, volume_percent) : -2;
    return ret == 0 ? 0 : (ret == -2 ? -2 : -1);
}

int audio_codec_set_mic_channel_gain_db(audio_codec_dev_t *dev, uint16_t slot_mask, float gain_db)
{
    if (!isfinite(gain_db)) {
        return -1;
    }
    int ret = audio_codec_validate_channel_control(dev, slot_mask, AUDIO_CODEC_CONTROL_GAIN_DB, false);
    if (ret != 0) {
        return ret;
    }
    const audio_codec_driver_t *drv = dev->drv;
    ret = drv->set_mic_channel_gain_db ? drv->set_mic_channel_gain_db(dev, slot_mask, gain_db) : -2;
    return ret == 0 ? 0 : (ret == -2 ? -2 : -1);
}

int audio_codec_set_mic_channel_mute(audio_codec_dev_t *dev, uint16_t slot_mask, bool mute)
{
    int ret = audio_codec_validate_channel_control(dev, slot_mask, AUDIO_CODEC_CONTROL_MUTE, false);
    if (ret != 0) {
        return ret;
    }
    const audio_codec_driver_t *drv = dev->drv;
    ret = drv->set_mic_channel_mute ? drv->set_mic_channel_mute(dev, slot_mask, mute) : -2;
    return ret == 0 ? 0 : (ret == -2 ? -2 : -1);
}

int audio_codec_set_speaker_channel_volume(audio_codec_dev_t *dev, uint16_t slot_mask, int volume_percent)
{
    if (volume_percent < 0 || volume_percent > 100) {
        return -1;
    }
    int ret = audio_codec_validate_channel_control(dev, slot_mask, AUDIO_CODEC_CONTROL_VOLUME, true);
    if (ret != 0) {
        return ret;
    }
    const audio_codec_driver_t *drv = dev->drv;
    ret = drv->set_speaker_channel_volume ? drv->set_speaker_channel_volume(dev, slot_mask, volume_percent) : -2;
    return ret == 0 ? 0 : (ret == -2 ? -2 : -1);
}

int audio_codec_set_speaker_channel_mute(audio_codec_dev_t *dev, uint16_t slot_mask, bool mute)
{
    int ret = audio_codec_validate_channel_control(dev, slot_mask, AUDIO_CODEC_CONTROL_MUTE, true);
    if (ret != 0) {
        return ret;
    }
    const audio_codec_driver_t *drv = dev->drv;
    ret = drv->set_speaker_channel_mute ? drv->set_speaker_channel_mute(dev, slot_mask, mute) : -2;
    return ret == 0 ? 0 : (ret == -2 ? -2 : -1);
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

int audio_codec_set_enable(audio_codec_dev_t *dev, audio_codec_work_mode_t mode, bool enable)
{
    if (dev == NULL || mode > AUDIO_CODEC_WORK_MODE_PLAYBACK) {
        return -1;
    }
    const audio_codec_driver_t *drv = dev->drv;
    return drv && drv->set_enable ? drv->set_enable(dev, mode, enable) : -2;
}

int audio_codec_set_reg(audio_codec_dev_t *dev, int reg, int value)
{
    if (dev == NULL || reg < 0 || value < 0) {
        return -1;
    }
    const audio_codec_driver_t *drv = dev->drv;
    return drv && drv->set_reg ? drv->set_reg(dev, reg, value) : -2;
}

int audio_codec_get_reg(audio_codec_dev_t *dev, int reg, int *value)
{
    if (dev == NULL || reg < 0 || value == NULL) {
        return -1;
    }
    const audio_codec_driver_t *drv = dev->drv;
    return drv && drv->get_reg ? drv->get_reg(dev, reg, value) : -2;
}

int audio_codec_dump_reg(audio_codec_dev_t *dev)
{
    if (dev == NULL) {
        return -1;
    }
    const audio_codec_driver_t *drv = dev->drv;
    return drv && drv->dump_reg ? drv->dump_reg(dev) : -2;
}

int audio_codec_deinit(audio_codec_dev_t *dev)
{
    if (dev == NULL) {
        return -1;
    }
    const audio_codec_driver_t *drv = dev->drv;
    return drv && drv->deinit ? drv->deinit(dev) : -2;
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
