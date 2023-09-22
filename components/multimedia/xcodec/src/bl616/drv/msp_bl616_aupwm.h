/**
  ******************************************************************************
  * @file    msp_bl616_aupwm.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __MSP_BL616_AUPWM_H__
#define __MSP_BL616_AUPWM_H__

#include "msp_bl616_platform.h"
#include "msp_bl616_aupwm_reg.h"

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  AUPWM
 *  @{
 */

/** @defgroup  AUPWM_Public_Types
 *  @{
 */

/**
 *  @brief AUPWM pwm mode
 */
typedef enum {
    AUPWM_PWM_8K,                            /*!< PWM 8KHZ */
    AUPWM_PWM_16K,                           /*!< PWM 16KHZ */
    AUPWM_PWM_32K,                           /*!< PWM 32KHZ */
    AUPWM_PWM_24K,                           /*!< PWM 24KHZ */
    AUPWM_PWM_48K,                           /*!< PWM 48KHZ */
    AUPWM_PWM_22P05K,                        /*!< PWM 22P05KHZ */
    AUPWM_PWM_44P1K,                         /*!< PWM 44P1KHZ */
    AUPWM_GPDAC_8K,                          /*!< GPDAC 8KHZ */
    AUPWM_GPDAC_16K,                         /*!< GPDAC 16KHZ */
    AUPWM_GPDAC_32K,                         /*!< GPDAC 32KHZ */
    AUPWM_GPDAC_24K,                         /*!< GPDAC 24KHZ */
    AUPWM_GPDAC_48K,                         /*!< GPDAC 48KHZ */
    AUPWM_GPDAC_22P05K,                      /*!< GPDAC 22P05KHZ */
    AUPWM_GPDAC_44P1K,                       /*!< GPDAC 44P1KHZ */
}AUPWM_Mode_Type;

/**
 *  @brief AUPWM Align
 */
typedef enum {
    AUPWM_FIFO_AILGN_MSB_AT_BIT31,           /*!< Mode 0: Valid data's MSB is at [31] of FIFO register */
    AUPWM_FIFO_AILGN_MSB_AT_BIT23,           /*!< Mode 1: Valid data's MSB is at [23] of FIFO register */
    AUPWM_FIFO_AILGN_MSB_AT_BIT19,           /*!< Mode 2: Valid data's MSB is at [19] of FIFO register */
    AUPWM_FIFO_AILGN_MSB_AT_BIT15,           /*!< Mode 3: Valid data's MSB is at [15] of FIFO register */
}AUPWM_FIFO_Ailgn_Mode;

/**
 *  @brief AUPWM Channel Sel
 */
typedef enum {
    AUPWM_MONO_LEFT=1,                        /*!< Left channel only dma data will only update mixer left channel*/
    AUPWM_MONO_RIGHT=2,                       /*!< Right channel only dma data will only update mixer left channel */
    AUPWM_STEREO=3,                           /*!< Stereo Channel , dma will update left & right channel,which data will be generate by dac is decided by AUPWM_Mixer_Type  */
}AUPWM_Channel_Mode;

/**
 *  @brief AUPWM Resolution
 */
typedef enum {
    AUPWM_DRQ_EQUEL_TO_IRQ,                  /*!< DMA request threhold is equeal to Interrupt threhold */
    AUPWM_DRQ_THR_AS_4_TX_8_RX,              /*!< DMA request threhold is 4 when config TX FIFO DRQ , 8 when config RX FIFO */
    AUPWM_DRQ_THR_AS_8_TX_16_RX,             /*!< DMA request threhold is 8 when config TX FIFO DRQ , 16 when config RX FIFO */
    AUPWM_DRQ_THR_AS_16_TX_32_RX,            /*!< DMA request threhold is 16 when config TX FIFO DRQ , 32 when config RX FIFO */
}AUPWM_FIFO_DQR_Threshold_Mode;

/**
 *  @brief AUPWM initerrupt type
 */
typedef enum {
    AUPWM_INT_TX_FIFO_THR,                   /*!< AUPWM TX FIFO THL */
    AUPWM_INT_TX_FIFO_OVERRUN,               /*!< AUPWM TX FIFO overrun */
    AUPWM_INT_TX_FIFO_UNDERRUN,              /*!< AUPWM TX FIFO underrun */
    AUPWM_INT_SET_VOLUME_DONE,               /*!< AUPWM Set volume done */
    AUPWM_INT_NUM_ALL,                       /*!< AUPWM int all */
}AUPWM_INT_Type;

/**
 *  @brief AUPWM_PRBS_Mode_Type
 */
typedef enum {
    AUPWM_LFSR32,                            /*!< AUPWM LFSR32 */
    AUPWM_LFSR24,                            /*!< AUPWM LFSR24 */
    AUPWM_LFSR16,                            /*!< AUPWM LFSR16 */
    AUPWM_LFSR12,                            /*!< AUPWM LFSR12 */
}AUPWM_PRBS_Mode_Type;

/**
 *  @brief AUPWM dac order
 */
typedef enum {
    AUPWM_DAC_2_ORDER,                       /*!< AUPWM ORDER 2 */
    AUPWM_DAC_3_ORDER,                       /*!< AUPWM ORDER 3 */
}AUPWM_DAC_Order_Type;

/**
 *  @brief AUPWM dac order
 */
typedef enum {
    AUPWM_MIXER_ONLY_LEFT,                   /*!< mixer only left */
    AUPWM_MIXER_ONLY_RIGHT,                  /*!< mixer only right */
    AUPWM_MIXER_L_PLUS_R,                    /*!< mixer left + right */
    AUPWM_MIXER_L_PLUS_R_DIV_2,              /*!< mixer (left + right)/2 */
}AUPWM_Mixer_Type;

/**
 *  @brief AUPWM MUTE
 */
typedef enum {
    AUPWM_UNMUTE,                            /*!< unmute */
    AUPWM_MUTE,                              /*!< mute */
}AUPWM_Mute_Type;

/**
 *  @brief AUPWM MUTE
 */
typedef enum {
    AUPWM_MUTE_DIRECTLY,                            /*!< mute directly */
    AUPWM_MUTE_RAMP,                                /*!< mute ramp*/
}AUPWM_Mute_Mode_Type;

/**
 *  @brief AUPWM_Ramp_Rate_Type
 */
typedef enum {
    AUIDO_RAMP_RATE_2_FS,                    /*!< ramp rate 2 fs */
    AUIDO_RAMP_RATE_4_FS,                    /*!< ramp rate 4 fs */
    AUIDO_RAMP_RATE_8_FS,                    /*!< ramp rate 8 fs */
    AUIDO_RAMP_RATE_16_FS,                   /*!< ramp rate 16 fs */
    AUIDO_RAMP_RATE_32_FS,                   /*!< ramp rate 32 fs */
    AUIDO_RAMP_RATE_64_FS,                   /*!< ramp rate 64 fs */
    AUIDO_RAMP_RATE_128_FS,                  /*!< ramp rate 128 fs */
    AUIDO_RAMP_RATE_256_FS,                  /*!< ramp rate 256 fs */
    AUIDO_RAMP_RATE_512_FS,                  /*!< ramp rate 512 fs */
    AUIDO_RAMP_RATE_1024_FS,                 /*!< ramp rate 1024 fs */
    AUIDO_RAMP_RATE_2048_FS,                 /*!< ramp rate 2048 fs */
}AUPWM_Ramp_Rate_Type;
/**
 *  @brief AUPWM_Volume_Ctr_Mode
 */
typedef enum {
    AUIDO_VOLUME_UPDATE_FORCE,               /*!< volume update force */
    AUIDO_VOLUME_UPDATE_AT_ZERO_CROSSING,    /*!< volume update at zero crossing */
    AUIDO_VOLUME_UPDATE_AT_RAMP,             /*!< volume update at zero crossing */
}AUPWM_Volume_Ctr_Mode;

/**
 *  @brief AUPWM_Zero_Cross_Rate_Type
 */
typedef enum {
    AUIDO_ZERO_CROSS_RATE_2_FS,              /*!< ramp rate 2 fs */
    AUIDO_ZERO_CROSS_RATE_4_FS,              /*!< ramp rate 4 fs */
    AUIDO_ZERO_CROSS_RATE_8_FS,              /*!< ramp rate 8 fs */
    AUIDO_ZERO_CROSS_RATE_16_FS,             /*!< ramp rate 16 fs */
    AUIDO_ZERO_CROSS_RATE_32_FS,             /*!< ramp rate 32 fs */
    AUIDO_ZERO_CROSS_RATE_64_FS,             /*!< ramp rate 64 fs */
    AUIDO_ZERO_CROSS_RATE_128_FS,            /*!< ramp rate 128 fs */
    AUIDO_ZERO_CROSS_RATE_256_FS,            /*!< ramp rate 256 fs */
    AUIDO_ZERO_CROSS_RATE_512_FS,            /*!< ramp rate 512 fs */
    AUIDO_ZERO_CROSS_RATE_1024_FS,           /*!< ramp rate 1024 fs */
    AUIDO_ZERO_CROSS_RATE_2048_FS,           /*!< ramp rate 2048 fs */
}AUPWM_Zero_Cross_Rate_Type;

/**
 *  @brief AUPWM_Cfg_Type
 */
typedef struct {
    AUPWM_Mode_Type pwm_mode;                /*!< pwm mode */
    uint8_t clkEn;                           /*!< clock enable */
}AUPWM_Cfg_Type;

/**
 *  @brief AUPWM FIFO Configuration
 */
typedef struct {
    uint8_t dmaEn;                           /*!< Dma interface enable>*/
    AUPWM_Mixer_Type mixType;                /*!< Mix Type */
    AUPWM_FIFO_Ailgn_Mode ailgnMode;         /*!< FIFO alignMode */
    uint8_t FifoIntThreshold;                /*!< FIFO IRQ Threshold */
    AUPWM_FIFO_DQR_Threshold_Mode dmaThresholdMode;    /*!< FIFO DRQ Threshold mode */
}AUPWM_FifoCfg_Type;

/**
 *  @brief AUPWM Volume Configuration
 */
typedef struct {
    AUPWM_Mute_Mode_Type     muteMode;            /*!< AUPWM mute mode   directly mute or use ramp down */
    AUPWM_Ramp_Rate_Type muteRmpdnRate;      /*!< mute ramp down rate when voice is in up process this parameter define slope */
    AUPWM_Ramp_Rate_Type muteRmpupRate;      /*!< mute ramp up rate when voice is in down process this parameter define slop */
    AUPWM_Volume_Ctr_Mode   volumeCtrMode;   /*!< volume will update at direct or zero crossing or ramp */
    AUPWM_Zero_Cross_Rate_Type   volumeZeroCrossRate;    /*!< when volume ctrl is zero crossing , select crossing rate */
    AUPWM_Ramp_Rate_Type    volumeRmpRate;   /*!< when volume ctrl is ramp ,select ramp rate */
    AUPWM_Zero_Cross_Rate_Type volumeZeroCrossTimeout;    /*!< if select zero cross mode ,  set volume directly when time out */
}AUPWM_Volume_Cfg_Type;

/*@} end of group AUPWM_Public_Types */

/** @defgroup  AUPWM_Public_Constants
 *  @{
 */

/** @defgroup  AUPWM_MODE_TYPE
 *  @{
 */
#define IS_AUPWM_MODE_TYPE(type)                         (((type) == AUPWM_PWM_8K) || \
                                                          ((type) == AUPWM_PWM_16K) || \
                                                          ((type) == AUPWM_PWM_32K) || \
                                                          ((type) == AUPWM_PWM_24K) || \
                                                          ((type) == AUPWM_PWM_48K) || \
                                                          ((type) == AUPWM_PWM_22P05K) || \
                                                          ((type) == AUPWM_PWM_44P1K) || \
                                                          ((type) == AUPWM_GPDAC_8K) || \
                                                          ((type) == AUPWM_GPDAC_16K) || \
                                                          ((type) == AUPWM_GPDAC_32K) || \
                                                          ((type) == AUPWM_GPDAC_24K) || \
                                                          ((type) == AUPWM_GPDAC_48K) || \
                                                          ((type) == AUPWM_GPDAC_22P05K) || \
                                                          ((type) == AUPWM_GPDAC_44P1K))

/** @defgroup  AUPWM_FIFO_AILGN_MODE
 *  @{
 */
#define IS_AUPWM_FIFO_AILGN_MODE(type)                   (((type) == AUPWM_FIFO_AILGN_MSB_AT_BIT31) || \
                                                          ((type) == AUPWM_FIFO_AILGN_MSB_AT_BIT23) || \
                                                          ((type) == AUPWM_FIFO_AILGN_MSB_AT_BIT19) || \
                                                          ((type) == AUPWM_FIFO_AILGN_MSB_AT_BIT15))

/** @defgroup  AUPWM_FIFO_DQR_THRESHOLD_MODE
 *  @{
 */
#define IS_AUPWM_FIFO_DQR_THRESHOLD_MODE(type)           (((type) == AUPWM_DRQ_EQUEL_TO_IRQ) || \
                                                          ((type) == AUPWM_DRQ_THR_AS_4_TX_8_RX) || \
                                                          ((type) == AUPWM_DRQ_THR_AS_8_TX_16_RX) || \
                                                          ((type) == AUPWM_DRQ_THR_AS_16_TX_32_RX))

/** @defgroup  AUPWM_INT_TYPE
 *  @{
 */
#define IS_AUPWM_INT_TYPE(type)                          (((type) == AUPWM_INT_TX_FIFO_THR) || \
                                                          ((type) == AUPWM_INT_TX_FIFO_OVERRUN) || \
                                                          ((type) == AUPWM_INT_TX_FIFO_UNDERRUN) || \
                                                          ((type) == AUPWM_INT_NUM_ALL))

/** @defgroup  AUPWM_PRBS_MODE_TYPE
 *  @{
 */
#define IS_AUPWM_PRBS_MODE_TYPE(type)                    (((type) == AUPWM_LFSR32) || \
                                                          ((type) == AUPWM_LFSR24) || \
                                                          ((type) == AUPWM_LFSR16) || \
                                                          ((type) == AUPWM_LFSR12))

/** @defgroup  AUPWM_DAC_ORDER_TYPE
 *  @{
 */
#define IS_AUPWM_DAC_ORDER_TYPE(type)                    (((type) == AUPWM_DAC_2_ORDER) || \
                                                          ((type) == AUPWM_DAC_3_ORDER))

/** @defgroup  AUPWM_MIXER_TYPE
 *  @{
 */
#define IS_AUPWM_MIXER_TYPE(type)                        (((type) == AUPWM_MIXER_ONLY_LEFT) || \
                                                          ((type) == AUPWM_MIXER_ONLY_RIGHT) || \
                                                          ((type) == AUPWM_MIXER_L_PLUS_R) || \
                                                          ((type) == AUPWM_MIXER_L_PLUS_R_DIV_2))

/*@} end of group AUPWM_Public_Constants */

/** @defgroup  AUPWM_Public_Macros
 *  @{
 */

/*@} end of group AUPWM_Public_Macros */

/** @defgroup  AUPWM_Public_Functions
 *  @{
 */
void AUPWM_Init(AUPWM_Cfg_Type * cfg);
void AUPWM_FifoInit(AUPWM_FifoCfg_Type * cfg);
void AUPWM_FifoDisable(void);
void AUPWM_FifoEnable(void);
void AUPWM_FifoClear(void);
void AUPWM_SetMixerChannel(AUPWM_Mixer_Type mixerSrc);
uint32_t AUPWM_GetFifoCount(void);
void AUPWM_Start(AUPWM_Channel_Mode chan);
void AUPWM_Stop(AUPWM_Channel_Mode chan);
void AUPWM_Enable(void);
void AUPWM_Disable(void);
void AUPWM_FifoFlush(void);
void AUPWM_ZeroDetectInit(uint8_t enable,uint16_t time);
uint8_t AUPWM_GetZeroDetectStatus(void);
void AUPWM_DitherInit(uint8_t enable ,AUPWM_PRBS_Mode_Type prbsMode, uint8_t amp);
void AUPWM_ScalingInit(uint8_t enable, uint8_t scaling);
void AUPWM_OrderInit(AUPWM_DAC_Order_Type order);
void AUPWM_IntMask(AUPWM_INT_Type intType,BL_Mask_Type intMask);
void AUPWM_IntClear(AUPWM_INT_Type intType);
//BL_Err_Type AUPWM_Int_Callback_Install(AUPWM_INT_Type intType,intCallback_Type* cbFun);
BL_Sts_Type AUPWM_GetIntStatus(AUPWM_INT_Type intType);
void AUPWM_IRQHandler(void);
void AUPWM_SetVolume(uint16_t volume);
void AUPWM_VolumeConfig(AUPWM_Volume_Cfg_Type * cfg );
void AUPWM_SetMute(char mute_en);
void AUPWM_GetMute(char *mute_en);
char AUPWM_GetMuteStatus(void);
void AUPWM_SendData(uint32_t data);
void AUPWM_IRQHandler(void);
int AUPWM_DigitalGainSet(float gaindb);
int AUPWM_DigitalGainGet(float *gaindb);
/*@} end of group AUPWM_Public_Functions */

/*@} end of group AUPWM */

/*@} end of group BL616_Peripheral_Driver */

#endif /* __BL616_AUPWM_H__ */
