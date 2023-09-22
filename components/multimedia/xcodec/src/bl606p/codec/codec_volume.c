/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>

#include <xutils/debug.h>

#include <msp_sys.h>
#include <msp_dma.h>
#include <msp_port.h>
#include <msp_bl606p_dma.h>
#include <msp_bl606p_audio.h>

static float s_spk_digital_gain[2] = {0};
static float s_spk_analog_gain[2] = {0};
static float s_mic_analog_gain[3] = {0};
static float s_mic_digital_gain[3] = {0};

/**
  \brief       Set codec output channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int spk_digital_gain_set(int ch, float gaindb)
{
    uint16_t regval;

    if (gaindb > 0) {
        return -1;
    } else {
        regval = (uint16_t)(gaindb * 2 + 512);
    }

    Audio_DAC_Set_Volume(ENABLE, AUDIO_DAC_0, regval);

    for (int m = 0; m < sizeof(s_spk_digital_gain)/sizeof(float); m++) {
        s_spk_digital_gain[m] = gaindb;
    }


    return 0;
}

/**
  \brief       Get codec output channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int spk_digital_gain_get(int ch, float *gaindb)
{
    if (gaindb) {
        if ((ch >= 0) && (ch < sizeof(s_spk_digital_gain)/sizeof(float))) {
            *gaindb = s_spk_digital_gain[ch];
        }
    }
    return 0;
}

/**
  \brief       Set codec output channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int spk_analog_gain_set(int ch, float gaindb)
{
    int i;//regval
    float buf[] = {
        0,   // 0    0dB
        -3,  // 1   -3dB
        -6,  // 2
        -9,  // 3
        -12, // 4
        -15, // 5
        -18, // 6
        -21  // 7
    };

    for (i = 0; i < (sizeof(buf)/sizeof(float)); i++) {
        if (gaindb == buf[i]) {
            Audio_SetDACAnalogGain(AUDIO_DAC_0, i);// analog -3db

            for (int m = 0; m < sizeof(s_spk_analog_gain)/sizeof(float); m++) {
                s_spk_analog_gain[m] = gaindb;
            }

            break;
        }
    }
    if (i == (sizeof(buf)/sizeof(float))) {
        MSP_LOGI("ch = %d, val = %fdB, error\r\n", ch, gaindb);
        return -1;
    }
    MSP_LOGI("ch:%d, val:%fdB, success(reg:%d (0,-3,-6,-9,-12,-15,-21)\r\n", ch, gaindb, i);
    return 0;
}

/**
  \brief       Get codec output channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int spk_analog_gain_get(int ch, float *gaindb)
{
    if (gaindb) {
        if ((ch >= 0) && (ch < sizeof(s_spk_analog_gain)/sizeof(float))) {
            *gaindb = s_spk_analog_gain[ch];
        }
    }

    return 0;
}

/**
  \brief       Set codec output channel mute
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   mute_en     mute enable
  \return      0-success other-error
*/
int spk_mute(int ch, int mute_en)
{
    if (mute_en) {
        Audio_DAC_Set_Mute(ENABLE, 0, AUDIO_MUTE);
    } else {
        Audio_DAC_Set_Mute(ENABLE, 0, AUDIO_UNMUTE);
    }
    return 0;
}

/**
  \brief       Set codec input channel digital gain
  \param[in]   ch     Codec input channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int mic_digital_gain_set(int ch, float gaindb)
{
    uint16_t regval;

    regval = (uint16_t)(512 + gaindb*2);
    Audio_ADC_Set_Volume(ENABLE, AUDIO_ADC_0, regval);

    for (int m = 0; m < sizeof(s_mic_digital_gain)/sizeof(float); m++) {
        s_mic_digital_gain[m] = gaindb;
    }

    return 0;
}

/**
  \brief       Get codec input channel digital gain
  \param[in]   ch     Codec input channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int mic_digital_gain_get(int ch, float *gaindb)
{
    if (gaindb) {
        if ((ch >= 0) && (ch < sizeof(s_mic_digital_gain)/sizeof(float))) {
            *gaindb = s_mic_digital_gain[ch];
        }
    }

    return 0;
}

/**
  \brief       Set codec input channel digital gain
  \param[in]   ch     Codec input channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int mic_analog_gain_set(int ch, float gaindb)
{
    uint16_t reg_val = 0;

    if (gaindb == 0) {
        reg_val = 0;
    } else if (((gaindb/3) >= 2) && ((gaindb/3) <= 14) && (((int)gaindb)%3 == 0)) {
        reg_val = gaindb/3;
    } else {
        MSP_LOGE("ch:%d, gaindb = %ld db, arg err,  check 0 ? or 6~42(step 3 db) ?\r\n", ch);
        return -1;
    }

    MSP_LOGI("ch:%d, gaindb = %lddb, reg_val = %d\r\n", ch, gaindb, reg_val);

    Audio_SetADCAnalogGain((Audio_ADC_ID_Type)ch, reg_val);

    for (int m = 0; m < sizeof(s_mic_analog_gain)/sizeof(float); m++) {
        s_mic_analog_gain[m] = gaindb;
    }
    return 0;
}

/**
  \brief       Get codec input channel digital gain
  \param[in]   ch     Codec input channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int mic_analog_gain_get(int ch, float *gaindb)
{
    if (gaindb) {
        if ((ch >= 0) && (ch < sizeof(s_mic_analog_gain)/sizeof(float))) {
            *gaindb = s_mic_analog_gain[ch];
        }
    }

    return 0;
}

/**
  \brief       Set codec input channel mute
  \param[in]   ch     Codec input channel handle, -1:all ch
  \param[in]   mute_en     mute enable
  \return      0-success other-error
*/
int mic_mute(int ch, int mute_en)
{
    if (mute_en) {
        Audio_DAC_Set_Mute(ENABLE, 0, AUDIO_MUTE);
    } else {
        Audio_DAC_Set_Mute(ENABLE, 0, AUDIO_UNMUTE);
    }
    return 0;
}

