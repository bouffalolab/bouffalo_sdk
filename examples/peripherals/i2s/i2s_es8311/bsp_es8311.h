/**
 * @file bsp_es8311.h
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

#ifndef __ES8311_H__
#define __ES8311_H__

typedef enum {
    ES_MODULE_MIN = -1,
    ES_MODULE_ADC = 0x01,
    ES_MODULE_DAC = 0x02,
    ES_MODULE_ADC_DAC = 0x03,
    ES_MODULE_LINE = 0x04,
    ES_MODULE_MAX
} es_module_t;

typedef enum {
    ES8311_MODE_SLAVE = 0x00,   /*!< set slave mode */
    ES8311_MODE_MASTER = 0x01,  /*!< set master mode */
} i2s_es8311_mode_t;

typedef enum {
    ES8311_I2S_NORMAL = 0,  /*!< set normal I2S format */
    ES8311_I2S_LEFT,        /*!< set all left format */
    ES8311_I2S_RIGHT,       /*!< set all right format */
    ES8311_I2S_DSP,         /*!< set dsp/pcm format */
} i2s_es8311_format_t;

typedef enum {
    ES8311_08K_SAMPLES,   /*!< set to  8k samples per second */
    ES8311_11K_SAMPLES,   /*!< set to 11.025k samples per second */
    ES8311_16K_SAMPLES,   /*!< set to 16k samples in per second */
    ES8311_22K_SAMPLES,   /*!< set to 22.050k samples per second */
    ES8311_24K_SAMPLES,   /*!< set to 24k samples in per second */
    ES8311_32K_SAMPLES,   /*!< set to 32k samples in per second */
    ES8311_44K_SAMPLES,   /*!< set to 44.1k samples per second */
    ES8311_48K_SAMPLES,   /*!< set to 48k samples per second */
} i2s_es8311_samples_t;

typedef enum {
    ES8311_BIT_LENGTH_16BITS = 1,   /*!< set 16 bits per sample */
    ES8311_BIT_LENGTH_24BITS,       /*!< set 24 bits per sample */
    ES8311_BIT_LENGTH_32BITS,       /*!< set 32 bits per sample */
} i2s_es8311_bits_t;

typedef enum {
    ES8311_MIC_GAIN_MIN = -1,
    ES8311_MIC_GAIN_0DB,
    ES8311_MIC_GAIN_6DB,
    ES8311_MIC_GAIN_12DB,
    ES8311_MIC_GAIN_18DB,
    ES8311_MIC_GAIN_24DB,
    ES8311_MIC_GAIN_30DB,
    ES8311_MIC_GAIN_36DB,
    ES8311_MIC_GAIN_42DB,
    ES8311_MIC_GAIN_MAX
} i2s_es8311_mic_gain_t;

typedef struct
{
    i2s_es8311_mode_t mode;        /*!< audio codec chip mode */
    i2s_es8311_format_t fmt;       /*!< I2S interface format */
    i2s_es8311_samples_t samples;  /*!< I2S interface samples per second */
    i2s_es8311_bits_t bits;        /*!< i2s interface number of bits per sample */
} i2s_es8311_t;

/******************************************************************************
 * 				DEFINICIONES DE REGISTROS PARA ES8311
 *****************************************************************************/
#define ES8311_RESET_REG00			0x00 /* reset digital,csm,clock manager etc.*/
#define ES8311_CLK_MANAGER_REG01	0x01 /* select clk src for mclk, enable clock for codec */
#define ES8311_CLK_MANAGER_REG02	0x02 /* clk divider and clk multiplier */
#define ES8311_CLK_MANAGER_REG03	0x03 /* adc fsmode and osr  */
#define ES8311_CLK_MANAGER_REG04	0x04 /* dac osr */
#define ES8311_CLK_MANAGER_REG05	0x05 /* clk divier for adc and dac */
#define ES8311_CLK_MANAGER_REG06	0x06 /* bclk inverter and divider */
#define ES8311_CLK_MANAGER_REG07	0x07 /* tri-state, lrck divider */
#define ES8311_CLK_MANAGER_REG08	0x08 /* lrck divider */
#define ES8311_SDPIN_REG09			0x09 /* dac serial digital port */
#define ES8311_SDPOUT_REG0A			0x0A /* adc serial digital port */
#define ES8311_SYSTEM_REG0B			0x0B /* system */
#define ES8311_SYSTEM_REG0C			0x0C /* system */
#define ES8311_SYSTEM_REG0D			0x0D /* system, power up/down */
#define ES8311_SYSTEM_REG0E			0x0E /* system, power up/down */
#define ES8311_SYSTEM_REG0F			0x0F /* system, low power */
#define ES8311_SYSTEM_REG10			0x10 /* system */
#define ES8311_SYSTEM_REG11         0x11 /* system */
#define ES8311_SYSTEM_REG12			0x12 /* system, Enable DAC */
#define ES8311_SYSTEM_REG13			0x13 /* system */
#define ES8311_SYSTEM_REG14			0x14 /* system, select DMIC, select analog pga gain */
#define ES8311_ADC_REG15			0x15 /* ADC, adc ramp rate, dmic sense */
#define ES8311_ADC_REG16			0x16 /* ADC */
#define ES8311_ADC_REG17			0x17 /* ADC, volume */
#define ES8311_ADC_REG18			0x18 /* ADC, alc enable and winsize */
#define ES8311_ADC_REG19			0x19 /* ADC, alc maxlevel */
#define ES8311_ADC_REG1A			0x1A /* ADC, alc automute */
#define ES8311_ADC_REG1B			0x1B /* ADC, alc automute, adc hpf s1 */
#define ES8311_ADC_REG1C			0x1C /* ADC, equalizer, hpf s2 */
#define ES8311_DAC_REG31			0x31 /* DAC, mute */
#define ES8311_DAC_REG32			0x32 /* DAC, volume */
#define ES8311_DAC_REG33			0x33 /* DAC, offset */
#define ES8311_DAC_REG34			0x34 /* DAC, drc enable, drc winsize */
#define ES8311_DAC_REG35			0x35 /* DAC, drc maxlevel, minilevel */
#define ES8311_DAC_REG37			0x37 /* DAC, ramprate */
#define ES8311_GPIO_REG44			0x44 /* GPIO, dac2adc for test */
#define ES8311_GP_REG45				0x45 /* GP CONTROL */
#define ES8311_CHIP_ID1				0xFD /* CHIP ID1 */
#define ES8311_CHIP_ID2				0xFE /* CHIP ID2 */
#define ES8311_CHIP_VER				0xFF /* VERSION */
#define ES8311_MAX_REGISTER			0xFF

#define ES8311_DEFAULT_ID1			0x83
#define ES8311_DEFAULT_ID2			0x11
#define ES8311_DEFAULT_VER			0x00

/******************************************************************************
 * 				DEFINICIONES DE BITS PARA ES8311
 *****************************************************************************/

//------------------------- RESET REG 0x00 -----------------------------------

#define CSM_ON			0x80		//<--- Mask of CSM_ON bit
#define MSC				0x40		//<--- Mask of MSC bit
#define SEQ_DIS			0x20		//<--- Mask of SEQ_DIS bit
#define RST_DIG			0x10		//<--- Mask of RST_DIG bit
#define RST_CMG			0x08		//<--- Mask of RST_CMG bit
#define RST_MST			0x04		//<--- Mask of RST_MST bit
#define RST_ADC_DIG		0x02		//<--- Mask of RST_ADC_DIG bit
#define RST_DAC_DIG		0x01		//<--- Mask of RST_DAC_DIG bit

//------------------------- CLK MANAGER REG 0x01 -----------------------------

#define MCLK_SEL		0x80		//<--- Mask of MCLK_SEL bit
#define MCLK_INV		0x40		//<--- Mask of MCLK_INV bit
#define MCLK_ON			0x20		//<--- Mask of MCLK_ON bit
#define BCLK_ON			0x10		//<--- Mask of BCLK_ON bit
#define CLKADC_ON		0x08		//<--- Mask of CLKADC_ON bit
#define CLKDAC_ON		0x04		//<--- Mask of CLKDAC_ON bit
#define ANACLKADC_ON	0x02		//<--- Mask of ANACLKADC_ON bit
#define ANACLKDAC_ON	0x01		//<--- Mask of ANACLKDAC_ON bit

//------------------------- CLK MANAGER REG 0x06 -----------------------------

#define BCLK_CON			0x40	//<--- Mask of BCLK_CON bit
#define BCLK_INV			0x20	//<--- Mask of BCLK_INV bit

//Valid values for BCLK divider at Master mode DIV_BCLK[4:0]

#define DIV_BCLK_DEFAULT	0x03	//<--- Default value MCLK/4
#define DIV_BCLK_MCLK22		0x14	//<--- MCLK/22
#define DIV_BCLK_MCLK24		0x15	//<--- MCLK/24
#define DIV_BCLK_MCLK25		0x16	//<--- MCLK/25
#define DIV_BCLK_MCLK30		0x17	//<--- MCLK/30
#define DIV_BCLK_MCLK32		0x18	//<--- MCLK/32
#define DIV_BCLK_MCLK33		0x19	//<--- MCLK/33
#define DIV_BCLK_MCLK34		0x1A	//<--- MCLK/34
#define DIV_BCLK_MCLK36		0x1B	//<--- MCLK/36
#define DIV_BCLK_MCLK44		0x1C	//<--- MCLK/44
#define DIV_BCLK_MCLK48		0x1D	//<--- MCLK/48
#define DIV_BCLK_MCLK66		0x1E	//<--- MCLK/66
#define DIV_BCLK_MCLK72		0x1F	//<--- MCLK/72

//------------------------- SDP REG 0x09 -------------------------------------
/**
 * NOTA: El software esta preparado para resolucion de 16 bits, asi que esa
 * configuracion se hace hardcoded.
 */

#define SDP_IN_MUTE			0x40		//<--- Mask of SDP_IN_MUTE bit
#define SDP_IN_WL_16BIT		0x0C		//<--- Value IN Word Length 16 bits
#define SDP_IN_FMT_LEFT		0x01		//<--- Value for left justify serial audio data format

//------------------------- SDP REG 0x0A -------------------------------------
/**
 * NOTA: El software esta preparado para resolucion de 16 bits, asi que esa
 * configuracion se hace hardcoded.
 */

#define SDP_OUT_MUTE			0x40		//<--- Mask of SDP_OUT_MUTE bit
#define SDP_OUT_WL_16BIT		0x0C		//<--- Value OUT Word Length 16 bits
#define SDP_OUT_FMT_LEFT		0x01		//<--- Value for left justify serial audio data format

//------------------------- SYSTEM REG 0x0D ----------------------------------

#define REG_0D_DEFAULT			0x01		//<--- Value Start Up normal for automatic startup

//------------------------- SYSTEM REG 0x0E ----------------------------------

#define REG_0E_DEFAULT			0x02		//<--- Value for default startup

//------------------------- SYSTEM REG 0x12 ----------------------------------

#define REG_12_DEFAULT			0x00		//<--- Value for default startup

//------------------------- SYSTEM REG 0x13 ----------------------------------

#define HPSW					0x10		//<--- Mask of HPSW bit

//------------------------- SYSTEM REG 0x14 ----------------------------------

#define REG_14_DEFAULT			0x1A		//<---- Value for max PGA gain and DMIC off

#define LINSEL					0x10		//<---- Value for Mic1p-1n select

#define PGAGAIN_0DB				0x00
#define PGAGAIN_3DB				0x01
#define PGAGAIN_6DB				0x02
#define PGAGAIN_9DB				0x03
#define PGAGAIN_12DB			0x04
#define PGAGAIN_15DB			0x05
#define PGAGAIN_18DB			0x06
#define PGAGAIN_21DB			0x07
#define PGAGAIN_24DB			0x08
#define PGAGAIN_27DB			0x09
#define PGAGAIN_30DB			0x0A

//------------------------- ADC REG 0x1C -------------------------------------

#define REG_1C_DEFAULT			0x6A		//<--- Value for default startup

//------------------------	DAC VOL REG 0x32 ---------------------------------

#define DAC_VOLUME_0DB			0xBF		//<--- 0 dB for DAC volume (not default)

//------------------------- DAC REG 0x37 -------------------------------------

#define DAC_RAMPRATE_DEFAULT	0x40		//<--- Value for 0.25dB/32LRCK
#define DAC_EQBYPASS_DEFAULT	0x08		//<--- Value for DAC_EQ bypass

/******************************************************************************
 * 		DEFINICIONES ESPECIFICAS PARA UTILIZAR BCLK EN VEZ DE MCLK
 *****************************************************************************/
/* INTERNAL MASTER CLOCK = 8 * BCLK */

#define MULT_PRE	0x18		//<--- Value mask for MULT_PRE = 8 bits [4:3]
#define DIV_PRE		0x00		//<--- Value mask for DIV_PRE = 0 bits [7:5]
#define DIV_CLKADC	0x00		//<--- Value mask for DIV_CLKADC = 0 bits [7:4]
#define DIV_CLKDAC	0x00		//<--- Value mask for DIV_CLKDAC = 0 bits [3:0]

/**
 * @brief This is a translation of volume levels from percentage to uint8_t
 *
 * The volume of DAC goes in steps of 0.5dB from -95.5dB to +32dB
 * This function translates a percentage to a decimal value of one byte
 * corresponding to a level as Datasheets shows
 */
#define DAC_VOL_PERCENT_LEVEL(x)	((255 * x) / 100)

#define ADC_VOL_PERCENT_LEVEL(x)	DAC_VOL_PERCENT_LEVEL(x)

int es8311_init(i2s_es8311_t *i2s_cfg);
int es8311_start(es_module_t mode);
int es8311_stop(es_module_t mode);
int es8311_codec_set_voice_volume(int volume);
int es8311_codec_get_voice_volume(int *volume);
int es8311_set_voice_mute(bool enable);
int es8311_get_voice_mute(int *mute);

int es8311_set_mic_gain(i2s_es8311_mic_gain_t gain_db);

#endif /* __ES8311_H__ */
