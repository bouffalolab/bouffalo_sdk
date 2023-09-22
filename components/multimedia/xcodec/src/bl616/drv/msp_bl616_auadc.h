/**
  ******************************************************************************
  * @file    msp_bl616_auadc.h
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
#ifndef __MSP_BL616_AUADC_H__
#define __MSP_BL616_AUADC_H__

#include "msp_bl616_platform.h"
#include "msp_bl616_auadc_reg.h"

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  AUADC
 *  @{
 */

/** @defgroup  AUADC_Public_Types
 *  @{
 */

/**
 *  @brief AUADC_Clock_Type
 */
typedef enum {
    AUADC_CLK_8K_HZ,     /*!< AUADC 8K */
    AUADC_CLK_16K_HZ,    /*!< AUADC 16K */
    AUADC_CLK_22P05K_HZ, /*!< AUADC 22.05 */
    AUADC_CLK_32K_HZ,    /*!< AUADC 32K */
    AUADC_CLK_48K_HZ,    /*!< AUADC 48K */
    AUADC_CLK_96K_HZ,    /*!< AUADC 96K */
    AUADC_CLK_RESVERSE,  /*!< AUADC RESVERSE */
    AUADC_CLK_MANUAL,    /*!< AUADC CLOCK MANUAL */
    AUADC_PDAC_128K_HZ,  /*!< AUADC 128K */
    AUADC_PDAC_256K_HZ,  /*!< AUADC 256K */
    AUADC_PDAC_512K_HZ,  /*!< AUADC 512K */
} AUADC_Clock_Type;

/**
 *  @brief AUADC_Source_Type
 */
typedef enum {
    AUADC_SOURCE_ANALOG,    /*!< AUADC Souce select analog */
    AUADC_SOURCE_PDM_LEFT,  /*!< AUADC Souce select PDM left channel */
    AUADC_SOURCE_PDM_RIGHT, /*!< AUADC Souce select PDM right channel */
} AUADC_Source_Type;

/**
 *  @brief AUADC Resolution
 */
typedef enum {
    AUADC_RES_16_BITS, /*!< select ADC Resolution as 16 bit */
    AUADC_RES_20_BITS, /*!< select ADC Resolution as 20 bit */
} AUADC_Resolution_Type;

/**
 *  @brief AUADC Resolution
 */
typedef enum {
    AUADC_FIFO_AILGN_MSB_AT_BIT31, /*!< Mode 0: Valid data's MSB is at [31] of FIFO register */
    AUADC_FIFO_AILGN_MSB_AT_BIT23, /*!< Mode 1: Valid data's MSB is at [23] of FIFO register */
    AUADC_FIFO_AILGN_MSB_AT_BIT19, /*!< Mode 2: Valid data's MSB is at [19] of FIFO register */
    AUADC_FIFO_AILGN_MSB_AT_BIT15, /*!< Mode 3: Valid data's MSB is at [15] of FIFO register */
} AUADC_FIFO_Ailgn_Mode;

/**
 *  @brief AUADC initerrupt type
 */
typedef enum {
    AUADC_INT_RX_FIFO_THR,      /*!< AUADC RX FIFO counter upper than threhold interrupt */
    AUADC_INT_RX_FIFO_OVERRUN,  /*!< AUADC RX FIFO overrun */
    AUADC_INT_RX_FIFO_UNDERRUN, /*!< AUADC RX FIFO underrun */
    AUADC_INT_NUM_ALL,          /*!< AUADC int all */
} AUADC_INT_Type;

/**
 *  @brief AUADC Resolution
 */
typedef enum {
    AUADC_DRQ_EQUEL_TO_IRQ,       /*!< DMA request threhold is equeal to Interrupt threhold */
    AUADC_DRQ_THR_AS_4_TX_8_RX,   /*!< DMA request threhold is 4 when config TX FIFO DRQ , 8 when config RX FIFO */
    AUADC_DRQ_THR_AS_8_TX_16_RX,  /*!< DMA request threhold is 8 when config TX FIFO DRQ , 16 when config RX FIFO */
    AUADC_DRQ_THR_AS_16_TX_32_RX, /*!< DMA request threhold is 16 when config TX FIFO DRQ , 32 when config RX FIFO */
} AUADC_FIFO_DQR_Threshold_Mode;

/**
 *  @brief AUADC_ADC_PGA_ClockEdge_Type
 */
typedef enum {
    AUADC_ADC_PGA_CLK_FALLING_SENT, /* falling edge sent, rising edge recieve */
    AUADC_ADC_PGA_CLK_RISING_SENT,  /* rising edge sent, falling edge recieve  */
} AUADC_ADC_PGA_ClockEdge_Type;

/**
 *  @brief AUADC_ADC_PGA_BiasCurrent_Type
 */
typedef enum {
    AUADC_ADC_PGA_BIAS_4_UA, /* PGA bias current 4 uA*/
    AUADC_ADC_PGA_BIAS_5_UA, /* PGA bias current 5 uA*/
    AUADC_ADC_PGA_BIAS_6_UA, /* PGA bias current 6 uA*/
    AUADC_ADC_PGA_BIAS_7_UA, /* PGA bias current 7 uA*/
} AUADC_ADC_PGA_BiasCurrent_Type;

/**
 *  @brief }AUADC_ADC_PGA_HighFilter_Res_Type;
 */
typedef enum {
    AUADC_ADC_PGA_HF_480K,    /* PGA high pass filter R=480KOhm for AC-coupled mode */
    AUADC_ADC_PGA_HF_320K,    /* PGA high pass filter R=320KOhm for AC-coupled mode */
    AUADC_ADC_PGA_HF_160K,    /* PGA high pass filter R=160KOhm for AC-coupled mode */
    AUADC_ADC_PGA_HF_4K_FAST, /* PGA high pass filter R=4KOhm for AC-coupled mode,fast startup*/
} AUADC_ADC_PGA_HighFilter_Res_Type;

/**
 *  @brief AUADC_ADC_PGA_Chopper_Type
 */
typedef enum {
    AUADC_ADC_PGA_OPMIC_OFF_OPAAF_OFF, /* opmic off & opaaf off */
    AUADC_ADC_PGA_OPMIC_OFF_OPAAF_ON,  /* opmic off & opaaf on */
    AUADC_ADC_PGA_OPMIC_ON_OPAAF_OFF,  /* opmic on & opaaf off */
    AUADC_ADC_PGA_OPMIC_ON_OPAAF_ON,   /* opmic on & opaaf on */
} AUADC_ADC_PGA_Chopper_Type;

/**
 *  @brief AUADC_ADC_PGA_Chopper_Freq_Type
 */
typedef enum {
    AUADC_ADC_PGA_CHOPPER_FREQ_8K,
    AUADC_ADC_PGA_CHOPPER_FREQ_16K,
    AUADC_ADC_PGA_CHOPPER_FREQ_32K,
    AUADC_ADC_PGA_CHOPPER_FREQ_64K,
    AUADC_ADC_PGA_CHOPPER_FREQ_128K,
    AUADC_ADC_PGA_CHOPPER_FREQ_256K,
    AUADC_ADC_PGA_CHOPPER_FREQ_512K,
    AUADC_ADC_PGA_CHOPPER_FREQ_1024K,
} AUADC_ADC_PGA_Chopper_Freq_Type;

/**
 *  @brief AUADC_ADC_PGA_Chopper_ClockSource
 */
typedef enum {
    AUADC_ADC_PGA_CLOCK_ADC, /* PGA chopper clock source selection: adc clock */
    AUADC_ADC_PGA_CLOCK_SDM, /* PGA chopper clock source selection: synchronized clock from SDM */
} AUADC_ADC_PGA_Chopper_ClockSource;

/**
 *  @brief AUADC_ADC_SDM_BiasCurrent_Type
 */
typedef enum {
    AUADC_ADC_SDM_BIAS_4_UA, /* SDM bias current 4 uA*/
    AUADC_ADC_SDM_BIAS_5_UA, /* SDM bias current 5 uA*/
    AUADC_ADC_SDM_BIAS_6_UA, /* SDM bias current 6 uA*/
    AUADC_ADC_SDM_BIAS_7_UA, /* SDM bias current 7 uA*/
} AUADC_ADC_SDM_BiasCurrent_Type;

/**
 *  @brief AUADC_ADC_SDM_Integrator_firstOpNum
 */
typedef enum {
    AUADC_ADC_SDM_I_FIRST_NUM_12,
    AUADC_ADC_SDM_I_FIRST_NUM_24,
    AUADC_ADC_SDM_I_FIRST_NUM_36,
    AUADC_ADC_SDM_I_FIRST_NUM_48,
    AUADC_ADC_SDM_I_FIRST_NUM_60,
} AUADC_ADC_SDM_Integrator_firstOpNum;

/**
 *  @brief AUADC_ADC_SDM_Integrator_SecOpNum
 */
typedef enum {
    AUADC_ADC_SDM_I_SEC_NUM_12,
    AUADC_ADC_SDM_I_SEC_NUM_24,
    AUADC_ADC_SDM_I_SEC_NUM_36,
} AUADC_ADC_SDM_Integrator_SecOpNum;

/**
 *  @brief AUADC_ADC_SDM_QuantizerGain_Type
 */
typedef enum {
    AUADC_ADC_SDM_QG_DIV14,
    AUADC_ADC_SDM_QG_DIV12,
    AUADC_ADC_SDM_QG_DIV10,
    AUADC_ADC_SDM_QG_DIV8,
} AUADC_ADC_SDM_QuantizerGain_Type;

/**
 *  @brief AUADC_ADC_SDM_DitherLevel_Type
 */
typedef enum {
    AUADC_ADC_SDM_DL_0,          /* SDM dither level: 0 */
    AUADC_ADC_SDM_DL_LSB_DIV15,  /* SDM dither level: LSB/15 */
    AUADC_ADC_SDM_DL_LSB_DIV7_5, /* SDM dither level: LSB*2/15 */
    AUADC_ADC_SDM_DL_LSB_DIV5,   /* SDM dither level: LSB*3/15 */
} AUADC_ADC_SDM_DitherLevel_Type;

/**
 *  @brief AUADC_ADC_SDM_Order_Type
 */
typedef enum {
    AUADC_ADC_SDM_ORDER_0, /* SDM dither order: 0 */
    AUADC_ADC_SDM_ORDER_1, /* SDM dither order: 1 */
} AUADC_ADC_SDM_Order_Type;

/**
 *  @brief AUADC_ADC_AnalogChannel_Type
 */
typedef enum {
    AUADC_ADC_AnalogCh_AIN0,
    AUADC_ADC_AnalogCh_AIN1,
    AUADC_ADC_AnalogCh_AIN2,
    AUADC_ADC_AnalogCh_AIN3,
    AUADC_ADC_AnalogCh_AIN4,
    AUADC_ADC_AnalogCh_AIN5,
    AUADC_ADC_AnalogCh_AIN6,
    AUADC_ADC_AnalogCh_AIN7,
} AUADC_ADC_AnalogCh_Type;

/**
 *  @brief AUADC_ADC_PGA_Mode_Type
 */
typedef enum {
    AUADC_ADC_PGA_MODE_AC_DIFF,   /* AC-Coupled & differential-ended, Audio application */
    AUADC_ADC_PGA_MODE_AC_SINGLE, /* AC-Coupled & single-ended, Audio application */
    AUADC_ADC_PGA_MODE_DC_DIFF,   /* DC-Coupled & differential-ended, Measuring application */
    AUADC_ADC_PGA_MODE_DC_SINGLE, /* DC-Coupled & single-ended, Measuring application(may not used) */
} AUADC_ADC_PGA_Mode_Type;

/**
 *  @brief AUADC_ADC_PGA_Gain_Type
 */
typedef enum {
    AUADC_ADC_PGA_GAIN_6DB = 0,
    AUADC_ADC_PGA_GAIN_9DB = 3,
    AUADC_ADC_PGA_GAIN_12DB,
    AUADC_ADC_PGA_GAIN_15DB,
    AUADC_ADC_PGA_GAIN_18DB,
    AUADC_ADC_PGA_GAIN_21DB,
    AUADC_ADC_PGA_GAIN_24DB,
    AUADC_ADC_PGA_GAIN_27DB,
    AUADC_ADC_PGA_GAIN_30DB,
    AUADC_ADC_PGA_GAIN_33DB,
    AUADC_ADC_PGA_GAIN_36DB,
    AUADC_ADC_PGA_GAIN_39DB,
    AUADC_ADC_PGA_GAIN_42DB,
} AUADC_ADC_PGA_Gain_Type;

/**
 *  @brief AUADC_ADC_FilterCh_Mode_Type
 */
typedef enum {
    AUADC_ADC_FILT_MODE_AUDIO,
    AUADC_ADC_FILT_MODE_MEASURE,
} AUADC_ADC_FilterCh_Mode_Type;

/**
 *  @brief AUADC_ADC_MeasureMode_Filter_Type
 */
typedef enum {
    AUADC_ADC_MEASURE_FILTER_FILE_SINC3,
    AUADC_ADC_MEASURE_FILTER_LOW_LATENCY,
} AUADC_ADC_MeasureMode_Filter_Type;

/**
 *  @brief AUADC_ADC_MeasureMode_Rate
 */
typedef enum {
    AUADC_ADC_MEASURE_RATE_SPS_2_5,
    AUADC_ADC_MEASURE_RATE_SPS_5,
    AUADC_ADC_MEASURE_RATE_SPS_10,
    AUADC_ADC_MEASURE_RATE_SPS_20,
    AUADC_ADC_MEASURE_RATE_SPS_25,
    AUADC_ADC_MEASURE_RATE_SPS_50,
    AUADC_ADC_MEASURE_RATE_SPS_100,
    AUADC_ADC_MEASURE_RATE_SPS_200,
    AUADC_ADC_MEASURE_RATE_SPS_400,
    AUADC_ADC_MEASURE_RATE_SPS_800,
    AUADC_ADC_MEASURE_RATE_SPS_1000,
    AUADC_ADC_MEASURE_RATE_SPS_2000,
    AUADC_ADC_MEASURE_RATE_SPS_4000,
} AUADC_ADC_MeasureMode_Rate;

/**
 *  @brief AUADC_Cfg_Type
 */
typedef struct {
    AUADC_Clock_Type clk;     /*!< AUADC clock select */
    uint8_t auadcClkEnable;   /*!< AUADC clock Enable */
    uint8_t firMode;          /*!< AUADC fir mode select */
    uint8_t pdmItfInvEnable;  /*!< AUADC PDM INTERFACE inv */
    uint8_t adcItfInvEnable;  /*!< AUADC ADC INTERFACE inv */
    AUADC_Source_Type source; /*!< AUADC input Source type pdm or analog */
} AUADC_Cfg_Type;

/**
 *  @brief AUADC FIFO Configuration
 */
typedef struct {
    AUADC_Resolution_Type resolution;               /*!< Receiving AUADC Sample Resolution */
    AUADC_FIFO_Ailgn_Mode ailgnMode;                /*!< FIFO alignMode */
    uint8_t FifoIntThreshold;                       /*!< FIFO IRQ Threshold */
    AUADC_FIFO_DQR_Threshold_Mode dmaThresholdMode; /*!< FIFO DRQ Threshold mode */
    BL_Fun_Type dmaEn;                              /*!< enable DMA FIFO interface */
} AUADC_FifoCfg_Type;

typedef struct {
    AUADC_ADC_PGA_ClockEdge_Type adc_edge_mode;        /* ADC output data clock edge, 0:falling edge sent,rising edge recieve, 1:rising edge sent,falling edge recieve */
    BL_Fun_Type adc_clock_phase_invert_en;             /* adc clock phase invert */
    AUADC_ADC_PGA_BiasCurrent_Type pga_opmic_bias_cur; /* PGA_OPMIC bias current control */
    AUADC_ADC_PGA_BiasCurrent_Type pga_opaaf_bias_cur; /* PGA_OPAAF bias current control */
    AUADC_ADC_PGA_Chopper_Type pga_chopper;            /* control chopper for opmic&opaaf */
    AUADC_ADC_PGA_HighFilter_Res_Type pga_hf_res;      /* PGA high pass filter Res for AC-coupled mode */
    BL_Fun_Type pga_chopper_en;                        /* PGA chopper control, ENABLE or DISABLE */
    AUADC_ADC_PGA_Chopper_Freq_Type pga_chopper_freq;  /* PGA chopper frequency control @Fs=2048k */
    // AUADC_ADC_PGA_Chopper_ClockSource pga_chopper_clk_source;    /* PGA chopper clock source selection (not used) */

    BL_Fun_Type sdm_lowpower_en;                         /* SDM lowpower funciton en, ENABLE: 0.6 of disable */
    AUADC_ADC_SDM_BiasCurrent_Type sdm_bias_cur;         /* SDM bias current control */
    AUADC_ADC_SDM_Integrator_firstOpNum sdm_i_first_num; /* op number control for first integrator in SDM */
    AUADC_ADC_SDM_Integrator_SecOpNum sdm_i_sec_num;     /* op number control for second integrator in SDM */
    BL_Fun_Type dem_en;                                  /* dem function control, ENABLE or DISABLE */
    AUADC_ADC_SDM_QuantizerGain_Type sdm_qg;             /* quantizer gain control for SDM */
    BL_Fun_Type sdm_dither_en;                           /* dither control, ENABLE or DISABLE */
    AUADC_ADC_SDM_DitherLevel_Type sdm_dither_level;     /* dither level control for SDM */
    AUADC_ADC_SDM_Order_Type sdm_dither_order;           /* dither order control for SDM */

    BL_Fun_Type pga_circuit_en;                            /* PGA related circuit enable */
    BL_Fun_Type sdm_circuit_en;                            /* SDM related circuit enable */
    BL_Fun_Type pga_positive_en;                           /* DISABLE, look into each channel will see high impedance, ENABLE:one of eight channel will be choose */
    BL_Fun_Type pga_negative_en;                           /* DISABLE, look into each channel will see high impedance, ENABLE:one of eight channel will be choose */
    AUADC_ADC_AnalogCh_Type pga_posi_ch;                   /* Positive channel selection, connected to PGA positive terminal */
    AUADC_ADC_AnalogCh_Type pga_nega_ch;                   /* Negative channel selection, connected to PGA negative terminal */
    AUADC_ADC_PGA_Mode_Type pga_coupled_mode;              /* PGA mode configuration */
    AUADC_ADC_PGA_Gain_Type pga_gain;                      /* audio mode enable, audio filter is on when set to high */
    AUADC_ADC_FilterCh_Mode_Type adc_mode;                 /* adc work mode, audio-mode or measuring-mode */
    AUADC_ADC_MeasureMode_Filter_Type measure_filter_mode; /* digital dicimation filter selection when in measuring mode */
    AUADC_ADC_MeasureMode_Rate measure_rate;               /* audadc ouput data rate selection when configured to measuring mode */
} AUADC_ADC_AnalogCfg_Type;

/*@} end of group AUADC_Public_Types */

/** @defgroup  AUADC_Public_Constants
 *  @{
 */

/** @defgroup  AUADC_CLOCK_TYPE
 *  @{
 */
#define IS_AUADC_CLOCK_TYPE(type) (((type) == AUADC_CLK_8K_HZ) ||     \
                                   ((type) == AUADC_CLK_16K_HZ) ||    \
                                   ((type) == AUADC_CLK_22P05K_HZ) || \
                                   ((type) == AUADC_CLK_32K_HZ) ||    \
                                   ((type) == AUADC_CLK_48K_HZ) ||    \
                                   ((type) == AUADC_CLK_96K_HZ) ||    \
                                   ((type) == AUADC_CLK_RESVERSE) ||  \
                                   ((type) == AUADC_CLK_MANUAL) ||    \
                                   ((type) == AUADC_PDAC_128K_HZ) ||  \
                                   ((type) == AUADC_PDAC_256K_HZ) ||  \
                                   ((type) == AUADC_PDAC_512K_HZ))

/** @defgroup  AUADC_SOURCE_TYPE
 *  @{
 */
#define IS_AUADC_SOURCE_TYPE(type) (((type) == AUADC_SOURCE_ANALOG) || \
                                    ((type) == AUADC_SOURCE_PDM))

/** @defgroup  AUADC_RESOLUTION_TYPE
 *  @{
 */
#define IS_AUADC_RESOLUTION_TYPE(type) (((type) == AUADC_RES_16_BITS) || \
                                        ((type) == AUADC_RES_20_BITS))

/** @defgroup  AUADC_FIFO_AILGN_MODE
 *  @{
 */
#define IS_AUADC_FIFO_AILGN_MODE(type) (((type) == AUADC_FIFO_AILGN_MSB_AT_BIT31) || \
                                        ((type) == AUADC_FIFO_AILGN_MSB_AT_BIT23) || \
                                        ((type) == AUADC_FIFO_AILGN_MSB_AT_BIT19) || \
                                        ((type) == AUADC_FIFO_AILGN_MSB_AT_BIT15))

/** @defgroup  AUADC_INT_TYPE
 *  @{
 */
#define IS_AUADC_INT_TYPE(type) (((type) == AUADC_INT_RX_FIFO_THR) ||      \
                                 ((type) == AUADC_INT_RX_FIFO_OVERRUN) ||  \
                                 ((type) == AUADC_INT_RX_FIFO_UNDERRUN) || \
                                 ((type) == AUADC_INT_NUM_ALL))

/** @defgroup  AUADC_FIFO_DQR_THRESHOLD_MODE
 *  @{
 */
#define IS_AUADC_FIFO_DQR_THRESHOLD_MODE(type) (((type) == AUADC_DRQ_EQUEL_TO_IRQ) ||      \
                                                ((type) == AUADC_DRQ_THR_AS_4_TX_8_RX) ||  \
                                                ((type) == AUADC_DRQ_THR_AS_8_TX_16_RX) || \
                                                ((type) == AUADC_DRQ_THR_AS_16_TX_32_RX))

/*@} end of group AUADC_Public_Constants */

/** @defgroup  AUADC_Public_Macros
 *  @{
 */

/*@} end of group AUADC_Public_Macros */

/** @defgroup  AUADC_Public_Functions
 *  @{
 */
void AUADC_Init(AUADC_Cfg_Type *cfg);
void AUADC_FifoInit(AUADC_FifoCfg_Type *cfg);
void AUADC_FifoClear(void);
void AUADC_Enable(void);
void AUADC_Disable(void);
void AUADC_HPFConfig(uint8_t k1_enable, uint8_t k1, uint8_t k2_enable, uint8_t k2);
void AUADC_SetVolume(uint32_t volume);
void AUADC_IntMask(AUADC_INT_Type intType, BL_Mask_Type intMask);
void Auadc_IntClear(AUADC_INT_Type intType);
#if 0
BL_Err_Type AUADC_Int_Callback_Install(AUADC_INT_Type intType, intCallback_Type *cbFun);
#endif
BL_Sts_Type AUADC_GetIntStatus(AUADC_INT_Type intType);
uint32_t AUADC_GetFifoCount(void);
uint32_t AUADC_GetRawData(void);
BL_Sts_Type AUADC_FifoDataReady(void);
void AUADC_IRQHandler(void);
void AUADC_Stop(void);
void AUADC_Start(void);

void AUADC_ADC_Config(AUADC_ADC_AnalogCfg_Type *adc_cfg);

/*@} end of group AUADC_Public_Functions */

/*@} end of group AUADC */

/*@} end of group BL616_Peripheral_Driver */

#endif /* __BL616_AUADC_H__ */
