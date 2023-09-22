/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __CODEC_VOLUME_H__
#define __CODEC_VOLUME_H__

/**
  \brief       Set codec ouput channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int spk_digital_gain_set(int ch, float gaindb);

/**
  \brief       Get codec ouput channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int spk_digital_gain_get(int ch, float *gaindb);

/**
  \brief       Set codec ouput channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int spk_analog_gain_set(int ch, float gaindb);

/**
  \brief       Get codec ouput channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int spk_analog_gain_get(int ch, float *gaindb);

/**
  \brief       Set codec ouput channel mute
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   mute_en     mute enable
  \return      0-success other-error
*/
int spk_mute(int ch, int mute_en);

/**
  \brief       Set codec input channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int mic_digital_gain_set(int ch, float gaindb);

/**
  \brief       Get codec input channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int mic_digital_gain_get(int ch, float *gaindb);

/**
  \brief       Set codec input channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int mic_analog_gain_set(int ch, float gaindb);

/**
  \brief       Get codec input channel digital gain
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   gaindb    Gain val (db)
  \return      0-success other-error
*/
int mic_analog_gain_get(int ch, float *gaindb);

/**
  \brief       Set codec input channel mute
  \param[in]   ch     Codec output channel handle, -1:all ch
  \param[in]   mute_en     mute enable
  \return      0-success other-error
*/
int mic_mute(int ch, int mute_en);

#endif
