#ifndef _BSP_COMMON_AUDIO_CODEC_INTERNAL_H_
#define _BSP_COMMON_AUDIO_CODEC_INTERNAL_H_

#include <stdint.h>

#include "bflb_i2c.h"

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
