#ifndef _BSP_COMMON_AUDIO_CODEC_INTERNAL_H_
#define _BSP_COMMON_AUDIO_CODEC_INTERNAL_H_

#include <stdint.h>
#include <math.h>
#include <stddef.h>

#include "audio_codec.h"
#include "bflb_i2c.h"

static inline int audio_codec_native_caps(uint16_t slots, uint8_t slot, const audio_codec_channel_caps_t *native,
                                          audio_codec_channel_caps_t *caps)
{
    if (caps == NULL || slot >= AUDIO_CODEC_MAX_SLOTS) {
        return -1;
    }
    if ((slots & (1U << slot)) == 0) {
        return -2;
    }
    *caps = *native;
    return 0;
}

static inline int audio_codec_gain_step_index(const float *steps, uint16_t count, float gain_db)
{
    if (steps == NULL || count == 0) {
        return -2;
    }
    if (!isfinite(gain_db) || gain_db < steps[0] || gain_db > steps[count - 1]) {
        return -1;
    }
    uint16_t best = 0;
    float difference = gain_db - steps[0];
    for (uint16_t i = 1; i < count; i++) {
        float next = gain_db - steps[i];
        if (next < 0) {
            next = -next;
        }
        if (next < difference) {
            difference = next;
            best = i;
        }
    }
    return best;
}

static inline int audio_codec_i2c_write_reg8(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t val)
{
    struct bflb_i2c_msg_s msgs[2];

    msgs[0].addr = addr;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &reg;
    msgs[0].length = 1;

    msgs[1].addr = addr;
    msgs[1].flags = 0;
    msgs[1].buffer = &val;
    msgs[1].length = 1;

    return bflb_i2c_transfer(i2c, msgs, 2);
}

static inline int audio_codec_i2c_read_reg8(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t *out_val)
{
    struct bflb_i2c_msg_s msgs[2];

    msgs[0].addr = addr;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &reg;
    msgs[0].length = 1;

    msgs[1].addr = addr;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = out_val;
    msgs[1].length = 1;

    return bflb_i2c_transfer(i2c, msgs, 2);
}

#endif
