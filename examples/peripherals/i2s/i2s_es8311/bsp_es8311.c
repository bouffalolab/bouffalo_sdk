/**
 * @file bsp_es8311.c
 * @brief
 *
 * Copyright (c) 2023 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "bflb_i2c.h"
#include "bsp_es8311.h"

// ES8311 I2C addr 0x18 0x19
#define ES8311_I2C_SLAVE_ADDR 0x18

/*
 * to define the clock soure of MCLK
 */
#define FROM_MCLK_PIN       (0)
#define FROM_SCLK_PIN       (1)
/*
 * to define whether to reverse the clock
 */
#define INVERT_MCLK         0 // do not invert
#define INVERT_SCLK         0

#define IS_DMIC             0 // Is it a digital microphone

#define MCLK_DIV_FRE        256

/*
 * Clock coefficient structer
 */
struct _coeff_div {
    uint32_t mclk;        /* mclk frequency */
    uint32_t rate;        /* sample rate */
    uint8_t pre_div;      /* the pre divider with range from 1 to 8 */
    uint8_t pre_multi;    /* the pre multiplier with x1, x2, x4 and x8 selection */
    uint8_t adc_div;      /* adcclk divider */
    uint8_t dac_div;      /* dacclk divider */
    uint8_t fs_mode;      /* double speed or single speed, =0, ss, =1, ds */
    uint8_t lrck_h;       /* adclrck divider and daclrck divider */
    uint8_t lrck_l;
    uint8_t bclk_div;     /* sclk divider */
    uint8_t adc_osr;      /* adc osr */
    uint8_t dac_osr;      /* dac osr */
};

/* codec hifi mclk clock divider coefficients */
static const struct _coeff_div coeff_div[] = {
    //mclk     rate   pre_div  mult  adc_div dac_div fs_mode lrch  lrcl  bckdiv osr
    /* 8k */
    {12288000, 8000 , 0x06, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {18432000, 8000 , 0x03, 0x02, 0x03, 0x03, 0x00, 0x05, 0xff, 0x18, 0x10, 0x20},
    {16384000, 8000 , 0x08, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {8192000 , 8000 , 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {6144000 , 8000 , 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {4096000 , 8000 , 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {3072000 , 8000 , 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {2048000 , 8000 , 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1536000 , 8000 , 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1024000 , 8000 , 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 11.025k */
    {11289600, 11025, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {5644800 , 11025, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {2822400 , 11025, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1411200 , 11025, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 12k */
    {12288000, 12000, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {6144000 , 12000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {3072000 , 12000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1536000 , 12000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 16k */
    {12288000, 16000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {18432000, 16000, 0x03, 0x02, 0x03, 0x03, 0x00, 0x02, 0xff, 0x0c, 0x10, 0x20},
    {16384000, 16000, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {8192000 , 16000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {6144000 , 16000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {4096000 , 16000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {3072000 , 16000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {2048000 , 16000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1536000 , 16000, 0x03, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1024000 , 16000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 22.05k */
    {11289600, 22050, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800 , 22050, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400 , 22050, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200 , 22050, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 24k */
    {12288000, 24000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 24000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 24000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 24000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 24000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 32k */
    {12288000, 32000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 32000, 0x03, 0x04, 0x03, 0x03, 0x00, 0x02, 0xff, 0x0c, 0x10, 0x10},
    {16384000, 32000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000 , 32000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 32000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {4096000 , 32000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 32000, 0x03, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2048000 , 32000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 32000, 0x03, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},
    {1024000 , 32000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 44.1k */
    {11289600, 44100, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800 , 44100, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400 , 44100, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200 , 44100, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 48k */
    {12288000, 48000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 48000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 48000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 48000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 48000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 64k */
    {12288000, 64000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 64000, 0x03, 0x04, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {16384000, 64000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000 , 64000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 64000, 0x01, 0x04, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {4096000 , 64000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 64000, 0x01, 0x08, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {2048000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0xbf, 0x03, 0x18, 0x18},
    {1024000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},

    /* 88.2k */
    {11289600, 88200, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800 , 88200, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400 , 88200, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200 , 88200, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},

    /* 96k */
    {12288000, 96000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 96000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 96000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 96000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 96000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},
};

static int get_es8311_mclk_src(void)
{
    return FROM_SCLK_PIN;
}

/*
* look for the coefficient in coeff_div[] table
*/
static int get_coeff(uint32_t mclk, uint32_t rate)
{
    for (int i = 0; i < (sizeof(coeff_div) / sizeof(coeff_div[0])); i++) {
        if (coeff_div[i].rate == rate && coeff_div[i].mclk == mclk)
            return i;
    }
    return -1;
}


static struct bflb_device_s *i2c0;
static struct bflb_i2c_msg_s msgs[2];


int es8311_i2c_init(void)
{
    i2c0 = bflb_device_get_by_name("i2c0");
    if (i2c0 == NULL) {
        return -1;
    }
    bflb_i2c_init(i2c0, 200000);
    return 0;
}

int es8311_write_reg(uint8_t addr, uint8_t data)
{
    msgs[0].addr = ES8311_I2C_SLAVE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &addr;
    msgs[0].length = 1;

    msgs[1].flags = 0;
    msgs[1].buffer = &data;
    msgs[1].length = 1;

    return bflb_i2c_transfer(i2c0, msgs, 2);
}

int es8311_read_reg(uint8_t addr, uint8_t *rdata)
{
    msgs[0].addr = ES8311_I2C_SLAVE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &addr;
    msgs[0].length = 1;

    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = rdata;
    msgs[1].length = 1;

    return bflb_i2c_transfer(i2c0, msgs, 2);
}

/*
* set es8311 dac mute or not
* if mute = 0, dac un-mute
* if mute = 1, dac mute
*/
static void es8311_mute(int mute)
{
    uint8_t regv;
    printf("Enter into es8311_mute(), mute = %d\r\n", mute);
    es8311_read_reg(ES8311_DAC_REG31, &regv);
    regv &= 0x9f;
    if (mute) {
        es8311_write_reg(ES8311_SYSTEM_REG12, 0x02);
        es8311_write_reg(ES8311_DAC_REG31, (regv | 0x60));
        es8311_write_reg(ES8311_DAC_REG32, 0x00);
        es8311_write_reg(ES8311_DAC_REG37, 0x08);
    } else {
        es8311_write_reg(ES8311_DAC_REG31, regv);
        es8311_write_reg(ES8311_SYSTEM_REG12, 0x00);
    }
}

/*
* set es8311 into suspend mode
*/
static void es8311_suspend(void)
{
    printf("Enter into es8311_suspend()\r\n");
    es8311_write_reg(ES8311_DAC_REG32, 0x00);
    es8311_write_reg(ES8311_ADC_REG17, 0x00);
    es8311_write_reg(ES8311_SYSTEM_REG0E, 0xFF);
    es8311_write_reg(ES8311_SYSTEM_REG12, 0x02);
    es8311_write_reg(ES8311_SYSTEM_REG14, 0x00);
    es8311_write_reg(ES8311_SYSTEM_REG0D, 0xFA);
    es8311_write_reg(ES8311_ADC_REG15, 0x00);
    es8311_write_reg(ES8311_DAC_REG37, 0x08);
    es8311_write_reg(ES8311_GP_REG45, 0x01);
}

void es8311_reg_dump(void)
{
    int i;
    uint8_t tmp;

    // es8311_read_reg(ES8311_CHIP_ID1, &tmp);
    // printf("Reg[%02d]=0x%02x \n", ES8311_CHIP_ID1, tmp);

    for (i = 0; i < 0X45; i++) {
        if (es8311_read_reg(i, &tmp) != 0) {
            printf("iic read err\r\n");
        }

        printf("Reg[%02d]=0x%02x \r\n", i, tmp);
    }
}

int es8311_config_fmt(i2s_es8311_format_t fmt)
{
    int ret = 0;
    uint8_t adc_iface = 0, dac_iface = 0;
    ret |= es8311_read_reg(ES8311_SDPIN_REG09, &dac_iface);
    ret |= es8311_read_reg(ES8311_SDPOUT_REG0A, &dac_iface);
    switch (fmt) {
        case ES8311_I2S_NORMAL:
            printf("ES8311 in I2S Format\r\n");
            dac_iface &= 0xFC;
            adc_iface &= 0xFC;
            break;
        case ES8311_I2S_LEFT:
        case ES8311_I2S_RIGHT:
            printf("ES8311 in LJ Format\r\n");
            adc_iface &= 0xFC;
            dac_iface &= 0xFC;
            adc_iface |= 0x01;
            dac_iface |= 0x01;
            break;
        case ES8311_I2S_DSP:
            printf("ES8311 in DSP-A Format\r\n");
            adc_iface &= 0xDC;
            dac_iface &= 0xDC;
            adc_iface |= 0x03;
            dac_iface |= 0x03;
            break;
        default:
            dac_iface &= 0xFC;
            adc_iface &= 0xFC;
            break;
    }
    ret |= es8311_write_reg(ES8311_SDPIN_REG09, dac_iface);
    ret |= es8311_write_reg(ES8311_SDPOUT_REG0A, adc_iface);

    return ret;
}

int es8311_set_bits_per_sample(i2s_es8311_bits_t bits)
{
    int ret = 0;
    uint8_t adc_iface = 0, dac_iface = 0;
    ret |= es8311_read_reg(ES8311_SDPIN_REG09, &dac_iface);
    ret |= es8311_read_reg(ES8311_SDPOUT_REG0A, &dac_iface);
    switch (bits) {
        case ES8311_BIT_LENGTH_16BITS:
            dac_iface |= 0x0c;
            adc_iface |= 0x0c;
            break;
        case ES8311_BIT_LENGTH_24BITS:
            break;
        case ES8311_BIT_LENGTH_32BITS:
            dac_iface |= 0x10;
            adc_iface |= 0x10;
            break;
        default:
            dac_iface |= 0x0c;
            adc_iface |= 0x0c;
            break;

    }
    ret |= es8311_write_reg(ES8311_SDPIN_REG09, dac_iface);
    ret |= es8311_write_reg(ES8311_SDPOUT_REG0A, adc_iface);

    return ret;
}

int es8311_init(i2s_es8311_t *i2s_cfg)
{
    int ret = 0;
    int coeff;
    uint8_t read_data, datmp;

    ret = es8311_i2c_init();
    if (ret < 0) {
        printf("es8311 i2c init failed!\r\n");
        return ret;
    }

    ret |= es8311_read_reg(ES8311_CHIP_ID1, &read_data);
    if (read_data != ES8311_DEFAULT_ID1) {
        printf("read chip id failed!\r\n");
        return ret;
    }

    /* reset 8311 to its default */
    ret |= es8311_write_reg(ES8311_RESET_REG00, RST_DIG | RST_CMG | RST_MST | RST_ADC_DIG | RST_DAC_DIG);

    bflb_mtimer_delay_ms(20);

    ret |= es8311_write_reg(ES8311_RESET_REG00, 0x00);
    ret |= es8311_write_reg(ES8311_RESET_REG00, CSM_ON); // power on cmd

    // /* setup clock: source BCLK, polarities defaults, ADC and DAC clocks on */
    // ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG01, MCLK_SEL | BCLK_ON | CLKADC_ON | CLKDAC_ON | ANACLKADC_ON | ANACLKDAC_ON);

	// /* Frecuency config with BCLK = 32 * LRCK (automatic from uC frame) => IMCLK = 8 * BCLK
	//  * Simple math ==> IMCLK = (32 * 8 * LRCK) , where LRCK = Sample Rate*/
    // ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG02, MULT_PRE | DIV_PRE);

	// /* register ES8311_CLK_MANAGER_REG03 as default after POR */
	// /* register ES8311_CLK_MANAGER_REG04 as default after POR */
	// /* Oversampling as POR defaults on REG03 and REG04 */

	// /* DIV_CLKADC=0 and DIV_CLKDAC=0 as defaults, but just for reasurement*/
	// ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG05, DIV_CLKADC | DIV_CLKDAC);

	// /* register ES8311_CLK_MANAGER_REG06 as default after POR */
	// /* register ES8311_CLK_MANAGER_REG07 as default after POR */
	// /* register ES8311_CLK_MANAGER_REG08 as default after POR */
	// /**
	//  * REG06, REG07 and REG08 has no use in slave operation as DIV_BCLK and DIV_LRCK are disabled
	//  */

	// /* Setup audio format (fmt): master/slave, resolution, I2S
	//  *
	//  * Beware, example code has a new reset on REG00 with 0xBF value
	//  * Not sure if needed here*/
    // // ret |= es8311_write_reg(ES8311_RESET_REG00, 0xBF);

    // /* Setup SDP In and Out resolution 16bits both */
    // ret |= es8311_write_reg(ES8311_SDPIN_REG09, SDP_IN_WL_16BIT);
    // ret |= es8311_write_reg(ES8311_SDPOUT_REG0A, SDP_IN_WL_16BIT);
	// /**
	//  * SDP_IN_FMT (REG09 [1:0]) have a default value (00) so its config as I2S serial audio data format
	//  * Check if Left Justify is needed (01). Not sure about this
	//  *
	//  * Same with SDP_OUT_FMT
	//  *
	//  */

    // ret |= es8311_write_reg(ES8311_SYSTEM_REG0D, REG_0D_DEFAULT); // Power up analog circuitry - NOT default
    // ret |= es8311_write_reg(ES8311_SYSTEM_REG0E, REG_0E_DEFAULT); // Enable analog PGA, enable ADC modulator - NOT default
    // ret |= es8311_write_reg(ES8311_SYSTEM_REG12, REG_12_DEFAULT); // power-up DAC - NOT default
    // ret |= es8311_write_reg(ES8311_SYSTEM_REG13, HPSW);           // Enable output to HP drive - NOT default
    // ret |= es8311_write_reg(ES8311_ADC_REG1C, REG_1C_DEFAULT);    // ADC Equalizer bypass, cancel DC offset in digital domain

	// /* DAC ramprate and bypass DAC equalizer - NOT default */
	// ret |= es8311_write_reg(ES8311_DAC_REG37, DAC_RAMPRATE_DEFAULT | DAC_EQBYPASS_DEFAULT);

    // /* Set DAC Volume */
    // ret |= es8311_write_reg(ES8311_DAC_REG32, DAC_VOL_PERCENT_LEVEL(80));

    // /* Set ADC Volume */
    // ret |= es8311_write_reg(ES8311_ADC_REG17, ADC_VOL_PERCENT_LEVEL(70));

    // /* Set max PGA Gain for Mic (differential input) and turn DIG_MIC off */
    // ret |= es8311_write_reg(ES8311_SYSTEM_REG14, LINSEL | PGAGAIN_15DB);

    ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG01, 0x10);
    ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG02, 0x00);
    ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG03, 0x10);
    ret |= es8311_write_reg(ES8311_ADC_REG16, 0x04);
    ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG04, 0x10);
    ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG05, 0x00);
    ret |= es8311_write_reg(ES8311_SYSTEM_REG0B, 0x00);
    ret |= es8311_write_reg(ES8311_SYSTEM_REG0C, 0x20);
    ret |= es8311_write_reg(ES8311_SYSTEM_REG10, 0x13);
    ret |= es8311_write_reg(ES8311_SYSTEM_REG11, 0x7C);
    ret |= es8311_write_reg(ES8311_RESET_REG00, 0x80);

    ret |= es8311_read_reg(ES8311_RESET_REG00, &read_data);
    switch (i2s_cfg->mode) {
        case ES8311_MODE_MASTER:    /* MASTER MODE */
            read_data |= 0x40;
            break;
        case ES8311_MODE_SLAVE:     /* SLAVE MODE */
            read_data &= 0xBF;
            break;
        default:
            read_data &= 0xBF;
            break;
    }
    ret |= es8311_write_reg(ES8311_RESET_REG00, read_data);
    ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG01, 0x1F);

    /* select clock source for internal mclk */
    switch(get_es8311_mclk_src()) {
        case FROM_MCLK_PIN:
            ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG01, &read_data);
            read_data &= 0x7F;
            ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG01, read_data);
            break;
        case FROM_SCLK_PIN:
            ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG01, &read_data);
            read_data |= 0x80;
            ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG01, read_data);
            break;
        default:
            ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG01, &read_data);
            read_data &= 0x7F;
            ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG01, read_data);
            break;
    }

    int sample_fre = 0;
    int mclk_fre = 0;
    switch (i2s_cfg->samples) {
        case ES8311_08K_SAMPLES:
            sample_fre = 8000;
            break;
        case ES8311_11K_SAMPLES:
            sample_fre = 11025;
            break;
        case ES8311_16K_SAMPLES:
            sample_fre = 16000;
            break;
        case ES8311_22K_SAMPLES:
            sample_fre = 22050;
            break;
        case ES8311_24K_SAMPLES:
            sample_fre = 24000;
            break;
        case ES8311_32K_SAMPLES:
            sample_fre = 32000;
            break;
        case ES8311_44K_SAMPLES:
            sample_fre = 44100;
            break;
        case ES8311_48K_SAMPLES:
            sample_fre = 48000;
            break;
        default:
            printf("Unable to configure sample rate %dHz\r\n", sample_fre);
            break;
    }

    mclk_fre = sample_fre * MCLK_DIV_FRE;
    coeff = get_coeff(mclk_fre, sample_fre);
    if (coeff < 0) {
        printf("Unable to configure sample rate %dHz with %dHz MCLK\r\n", sample_fre, mclk_fre);
    }

    /*
     * Set clock parammeters
     */
    if (coeff >= 0) {
        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG02, &read_data);
        read_data &= 0x07;
        read_data |= (coeff_div[coeff].pre_div - 1) << 5;
        datmp = 0;
        switch (coeff_div[coeff].pre_multi) {
            case 1:
                datmp = 0;
                break;
            case 2:
                datmp = 1;
                break;
            case 4:
                datmp = 2;
                break;
            case 8:
                datmp = 3;
                break;
            default:
                break;
        }

        if (get_es8311_mclk_src() == FROM_SCLK_PIN) {
            datmp = 3;     /* DIG_MCLK = LRCK * 256 = BCLK * 8 */
        }

        read_data |= (datmp) << 3;
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG02, read_data);

        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG05, &read_data);
        read_data &= 0x00;
        read_data |= (coeff_div[coeff].adc_div - 1) << 4;
        read_data |= (coeff_div[coeff].dac_div - 1) << 0;
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG05, read_data);

        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG03, &read_data);
        read_data &= 0x80;
        read_data |= coeff_div[coeff].fs_mode << 6;
        read_data |= coeff_div[coeff].adc_osr << 0;
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG03, read_data);

        ret = es8311_read_reg(ES8311_CLK_MANAGER_REG04, &read_data);
        read_data &= 0x80;
        read_data |= coeff_div[coeff].dac_osr << 0;
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG04, read_data);

        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG07, &read_data);
        read_data &= 0xC0;
        read_data |= coeff_div[coeff].lrck_h << 0;
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG07, read_data);

        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG08, &read_data);
        read_data &= 0x00;
        read_data |= coeff_div[coeff].lrck_l << 0;
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG08, read_data);

        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG06, &read_data);
        read_data &= 0xE0;
        if (coeff_div[coeff].bclk_div < 19) {
            read_data |= (coeff_div[coeff].bclk_div - 1) << 0;
        } else {
            read_data |= (coeff_div[coeff].bclk_div) << 0;
        }
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG06, read_data);
    }

    /*
     * mclk inverted or not
     */
    if (INVERT_MCLK) {
        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG01, &read_data);
        read_data |= 0x40;
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG01, read_data);
    } else {
        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG01, &read_data);
        read_data &= ~(0x40);
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG01, read_data);
    }

    /*
     * sclk inverted or not
     */
    if (INVERT_SCLK) {
        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG06, &read_data);
        read_data |= 0x20;
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG06, read_data);
    } else {
        ret |= es8311_read_reg(ES8311_CLK_MANAGER_REG06, &read_data);
        read_data &= ~(0x20);
        ret |= es8311_write_reg(ES8311_CLK_MANAGER_REG06, read_data);
    }

    ret |= es8311_write_reg(ES8311_SYSTEM_REG13, 0x10);
    ret |= es8311_write_reg(ES8311_ADC_REG1B, 0x0A);
    ret |= es8311_write_reg(ES8311_ADC_REG1C, 0x6A);

    es8311_config_fmt(i2s_cfg->fmt);
    es8311_set_bits_per_sample(i2s_cfg->bits);

    // es8311_reg_dump();

    return ret;
}


int es8311_codec_deinit(void)
{
    return 0;
}

#ifndef BIT
#define BIT(nr) (1UL << (nr))
#endif

int es8311_start(es_module_t mode)
{
    int ret = 0;
    uint8_t adc_iface = 0, dac_iface = 0;

    ret |= es8311_read_reg(ES8311_SDPIN_REG09, &dac_iface);
    dac_iface &= 0xBF;
    ret |= es8311_read_reg(ES8311_SDPOUT_REG0A, &adc_iface);
    adc_iface &= 0xBF;
    adc_iface |= BIT(6);
    dac_iface |= BIT(6);

    if (mode == ES_MODULE_LINE) {
        return -1;
    }
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        adc_iface &= ~(BIT(6));
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        dac_iface &= ~(BIT(6));
    }

    ret |= es8311_write_reg(ES8311_SDPIN_REG09, dac_iface);
    ret |= es8311_write_reg(ES8311_SDPOUT_REG0A, adc_iface);

    ret |= es8311_write_reg(ES8311_ADC_REG17, 0xBF);
    ret |= es8311_write_reg(ES8311_SYSTEM_REG0E, 0x02);
    ret |= es8311_write_reg(ES8311_SYSTEM_REG12, 0x00);
    ret |= es8311_write_reg(ES8311_SYSTEM_REG14, 0x15);

    /*
     * pdm dmic enable or disable
     */
    uint8_t regv = 0;
    if (IS_DMIC) {
        ret |= es8311_read_reg(ES8311_SYSTEM_REG14, &regv);
        regv |= 0x40;
        ret |= es8311_write_reg(ES8311_SYSTEM_REG14, regv);
    } else {
        ret |= es8311_read_reg(ES8311_SYSTEM_REG14, &regv);
        regv &= ~(0x40);
        ret |= es8311_write_reg(ES8311_SYSTEM_REG14, regv);
    }

    ret |= es8311_write_reg(ES8311_SYSTEM_REG0D, REG_0D_DEFAULT);
    ret |= es8311_write_reg(ES8311_ADC_REG15, 0x40);
    ret |= es8311_write_reg(ES8311_DAC_REG37, 0x48);
    ret |= es8311_write_reg(ES8311_GP_REG45, 0x00);

    /* set internal reference signal (ADCL + DACR) */
    ret |= es8311_write_reg(ES8311_GPIO_REG44, 0x50);

    // es8311_reg_dump();

    return ret;
}

int es8311_stop(es_module_t mode)
{
    int ret = 0;
    es8311_suspend();
    return ret;
}

int es8311_codec_set_voice_volume(int volume)
{
    int res = 0;
    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }
    int vol = (volume) * 2550 / 1000;
    printf("SET: volume:%d%%\r\n", volume);
    es8311_write_reg(ES8311_DAC_REG32, vol);
    return res;
}

int es8311_codec_get_voice_volume(int *volume)
{
    int ret = 0;
    int regv = 0;
    ret = es8311_read_reg(ES8311_DAC_REG32, &regv);
    if (regv != 0) {
        *volume = 0;
        ret = -1;
    } else {
        *volume = regv * 100 / 256;
    }
    printf("GET: res:%d, volume:%d\r\n", regv, *volume);
    return ret;
}

int es8311_set_voice_mute(bool enable)
{
    int ret = 0;
    printf("Es8311SetVoiceMute volume:%d\r\n", enable);
    es8311_mute(enable);
    return ret;
}

int es8311_get_voice_mute(int *mute)
{
    int ret = 0;
    uint8_t reg = 0;
    ret = es8311_read_reg(ES8311_DAC_REG31, &reg);
    if (ret == 0) {
        reg = (ret & 0x20) >> 5;
    }
    *mute = reg;
    return ret;
}

int es8311_set_mic_gain(i2s_es8311_mic_gain_t gain_db)
{
    int res = 0;
    res = es8311_write_reg(ES8311_ADC_REG16, gain_db); // MIC gain scale
    return res;
}

