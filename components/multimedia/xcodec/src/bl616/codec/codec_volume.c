/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>

#include <msp_sys.h>
#include <msp_port.h>
#include <msp_bl616_dma.h>
#include <msp_bl616_aupwm.h>

#define user_log(M, ...)   do {  printf("[%9lld]======[%s:%d] " M "",  \
                                   msp_gettick_count(),          \
                                   __func__,                       \
                                   __LINE__,                       \
                                   ##__VA_ARGS__);                 \
                              } while(0==1)

// static float s_spk_digital_gain[2] = {0};
// static float s_spk_analog_gain[2] = {0};
// static float s_mic_analog_gain[3] = {0};
// static float s_mic_digital_gain[3] = {0};

/**
  \brief       Set codec output channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int spk_digital_gain_set(int ch, float gaindb)
{
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
    return 0;
}

