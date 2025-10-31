/**
  ******************************************************************************
  * @file    msp_bl616_auadc.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
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

#include "msp_bl616_platform.h"
#include "msp_bl616_auadc.h"

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  AUADC
 *  @{
 */

/** @defgroup  AUADC_Private_Macros
 *  @{
 */

/*@} end of group AUADC_Private_Macros */

/** @defgroup  AUADC_Private_Types
 *  @{
 */

/*@} end of group AUADC_Private_Types */

/** @defgroup  AUADC_Private_Variables
 *  @{
 */

/*@} end of group AUADC_Private_Variables */

/** @defgroup  AUADC_Global_Variables
 *  @{
 */
#if 0
static intCallback_Type *auadcIntCbfArra[AUADC_INT_NUM_ALL] = { NULL };
#endif

/*@} end of group AUADC_Global_Variables */

/** @defgroup  AUADC_Private_Fun_Declaration
 *  @{
 */

/*@} end of group AUADC_Private_Fun_Declaration */

/** @defgroup  AUADC_Private_Functions
 *  @{
 */

/*@} end of group AUADC_Private_Functions */

/** @defgroup  AUADC_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Init AUADC adc module
 *
 * @param  cfg: cfg
 *
 * @return None
 *
*******************************************************************************/
void AUADC_Init(AUADC_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    /* set fir mode , select one order or two order fir filter */
    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_PDM_ADC_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_FIR_MODE, cfg->firMode);
    BL_WR_REG(AUADC_BASE, AUADC_PDM_ADC_0, tmpVal);

    if (cfg->source == AUADC_SOURCE_ANALOG) {
        /*set adc source analog */
        tmpVal = BL_RD_REG(AUADC_BASE, AUADC_PDM_DAC_0);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AUADC_ADC_0_SRC);
        BL_WR_REG(AUADC_BASE, AUADC_PDM_DAC_0, tmpVal);
        /* disable pdm */
        tmpVal = BL_RD_REG(AUADC_BASE, AUADC_PDM_PDM_0);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AUADC_PDM_0_EN);
        BL_WR_REG(AUADC_BASE, AUADC_PDM_PDM_0, tmpVal);

    } else {
        /*set adc source pdm */
        tmpVal = BL_RD_REG(AUADC_BASE, AUADC_PDM_DAC_0);
        tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_ADC_0_SRC);
        BL_WR_REG(AUADC_BASE, AUADC_PDM_DAC_0, tmpVal);
        /* set pdm channel */
        tmpVal = BL_RD_REG(AUADC_BASE, AUADC_PDM_PDM_0);
        tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_PDM_0_EN);
        if (cfg->source == AUADC_SOURCE_PDM_LEFT) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_PDM_SEL, 0);
        } else {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_PDM_SEL, 1);
        }
        BL_WR_REG(AUADC_BASE, AUADC_PDM_PDM_0, tmpVal);
    }

    /* Set Clock */
    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDPDM_TOP);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_RATE, cfg->clk);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_PDM_ITF_INV_SEL, cfg->pdmItfInvEnable);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_ITF_INV_SEL, cfg->adcItfInvEnable);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDIO_CKG_EN, cfg->auadcClkEnable);
    BL_WR_REG(AUADC_BASE, AUADC_AUDPDM_TOP, tmpVal);

    /* To avoid excessive PDM frequency, the OSR was reduced from 128 to 64 */
    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_CMD);
    if (cfg->source != AUADC_SOURCE_ANALOG && cfg->clk >= AUADC_CLK_32K_HZ) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_AUDADC_AUDIO_OSR_SEL);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AUADC_AUDADC_AUDIO_OSR_SEL);
    }
    BL_WR_REG(AUADC_BASE, AUADC_AUDADC_CMD, tmpVal);

#if 0//ndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(AUPDM_IRQn, AUADC_IRQHandler);
#endif
}

/****************************************************************************/ /**
 * @brief  Init AUADC adc fifo
 *
 * @param  cfg: cfg
 *
 * @return None
 *
*******************************************************************************/
void AUADC_FifoInit(AUADC_FifoCfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_AUADC_RESOLUTION_TYPE(cfg->resolution));
    CHECK_PARAM(IS_AUADC_FIFO_AILGN_MODE(cfg->ailgnMode));
    CHECK_PARAM(IS_AUADC_FIFO_DQR_THRESHOLD_MODE(cfg->dmaThresholdMode));

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_RX_DATA_RES, cfg->resolution);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_RX_DATA_MODE, cfg->ailgnMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_RX_DRQ_CNT, cfg->dmaThresholdMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_RX_TRG_LEVEL, cfg->FifoIntThreshold);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_RX_DRQ_EN, cfg->dmaEn);

    BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL, tmpVal);

    /* Set dma interface */
    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDPDM_ITF);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_ITF_EN, cfg->dmaEn);
    BL_WR_REG(AUADC_BASE, AUADC_AUDPDM_ITF, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Init clear AUADC adc fifo
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void AUADC_FifoClear(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_RX_FIFO_FLUSH, 1);
    BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Enable Auadc
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void AUADC_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDPDM_ITF);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_EN, 1);
    BL_WR_REG(AUADC_BASE, AUADC_AUDPDM_ITF, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Disable Auadc
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void AUADC_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDPDM_ITF);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_EN, 0);
    BL_WR_REG(AUADC_BASE, AUADC_AUDPDM_ITF, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Disable FIFO Interface
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void AUADC_Start(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_RX_CH_EN, 1);
    BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Enable FIFO Interface
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void AUADC_Stop(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_RX_CH_EN, 0);
    BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Config HPF
 *
 * @param  k1_enable: k1 ENABLE
 * @param  k1: k1 paramater
 * @param  k2_enable: k2 Enable
 * @param  k2: k2 paramater
 *
 * @return None
 *
*******************************************************************************/
void AUADC_HPFConfig(uint8_t k1_enable, uint8_t k1, uint8_t k2_enable, uint8_t k2)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_PDM_ADC_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_K1, k1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_K1_EN, k1_enable);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_K2, k2_enable);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_K2_EN, k2);

    BL_WR_REG(AUADC_BASE, AUADC_PDM_ADC_1, tmpVal);
}

/****************************************************************************/ /**
 * @brief  set adc volume
 *
 * @param  volume: volume
 *
 * @return None
 *
*******************************************************************************/
void AUADC_SetVolume(uint32_t volume)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_PDM_ADC_S0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_ADC_S0_VOLUME, volume);
    BL_WR_REG(AUADC_BASE, AUADC_PDM_ADC_S0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  set auadc int mask
 *
 * @param  intType: intType
 * @param  intMask: intMask
 *
 * @return None
 *
*******************************************************************************/
void AUADC_IntMask(AUADC_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_AUADC_INT_TYPE(intType));
    CHECK_PARAM(IS_BL_MASK_TYPE(intMask));

    switch (intType) {
        case AUADC_INT_RX_FIFO_THR:
            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL);
            if (intMask) {
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUADC_RXA_INT_EN);
            } else {
                tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXA_INT_EN);
            }
            BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL, tmpVal);
            break;
        case AUADC_INT_RX_FIFO_OVERRUN:
            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL);
            if (intMask) {
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUADC_RXO_INT_EN);
            } else {
                tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXO_INT_EN);
            }
            BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL, tmpVal);
            break;
        case AUADC_INT_RX_FIFO_UNDERRUN:
            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL);
            if (intMask) {
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUADC_RXU_INT_EN);
            } else {
                tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXU_INT_EN);
            }
            BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL, tmpVal);
            break;

        case AUADC_INT_NUM_ALL:
            if (intMask) {
                tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL);
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUADC_RXA_INT_EN);
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUADC_RXU_INT_EN);
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUADC_RXO_INT_EN);
                BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL, tmpVal);

            } else {
                tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL);
                tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXA_INT_EN);
                tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXU_INT_EN);
                tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXO_INT_EN);
                BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_CTRL, tmpVal);
            }

        default:
            break;
    }
}

/****************************************************************************/ /**
 * @brief  clear auadc int flag
 *
 * @param  intType: intType
 *
 * @return None
 *
*******************************************************************************/
void AUADC_IntClear(AUADC_INT_Type intType)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_AUADC_INT_TYPE(intType));

    switch (intType) {
        case AUADC_INT_RX_FIFO_THR:
            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXA_INT);
            BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS, tmpVal);
            break;

        case AUADC_INT_RX_FIFO_OVERRUN:
            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXO_INT);
            BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS, tmpVal);
            break;

        case AUADC_INT_RX_FIFO_UNDERRUN:
            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXU_INT);
            BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS, tmpVal);
            break;

        case AUADC_INT_NUM_ALL:

            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXO_INT);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXU_INT);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUADC_RXA_INT);
            BL_WR_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS, tmpVal);
            break;

        default:
            break;
    }
}

#if 0
/****************************************************************************/ /**
 * @brief  register callback function
 *
 * @param  intType: intType
 * @param  cbFun: cbFun
 *
 * @return success or not
 *
*******************************************************************************/
BL_Err_Type AUADC_Int_Callback_Install(AUADC_INT_Type intType, intCallback_Type *cbFun)
{
    CHECK_PARAM(IS_AUADC_INT_TYPE(intType));

    auadcIntCbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  get int status
 *
 * @param  intType: intType
 *
 * @return interrupt flag status
 *
*******************************************************************************/
BL_Sts_Type AUADC_GetIntStatus(AUADC_INT_Type intType)
{
    uint32_t tmpVal = 0;
    BL_Sts_Type rlt = RESET;

    CHECK_PARAM(IS_AUADC_INT_TYPE(intType));

    switch (intType) {
        case AUADC_INT_RX_FIFO_THR:
            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS);
            rlt = BL_IS_REG_BIT_SET(tmpVal, AUADC_RXA_INT);
            break;
        case AUADC_INT_RX_FIFO_OVERRUN:
            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS);
            rlt = BL_IS_REG_BIT_SET(tmpVal, AUADC_RXO_INT);
            break;
        case AUADC_INT_RX_FIFO_UNDERRUN:
            tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS);
            rlt = BL_IS_REG_BIT_SET(tmpVal, AUADC_RXU_INT);
            break;

        default:
            break;
    }

    return rlt;
}

/****************************************************************************/ /**
 * @brief  Get Rx FIFO Count
 *
 * @param  None
 *
 * @return fifi count
 *
*******************************************************************************/
uint32_t AUADC_GetFifoCount(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_STATUS);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, AUADC_RXA_CNT);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  Get Rx data
 *
 * @param  None
 *
 * @return raw data
 *
*******************************************************************************/
uint32_t AUADC_GetRawData(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_PDM_DAC_0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, AUADC_ADC_0_SRC);

    if (tmpVal) {
        /* pdm interface */
        tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_RX_FIFO_DATA);
    } else {
        /*analog interface */
        tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_DATA);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_RAW_DATA);
    }

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  Get Rx data is ready or not
 *
 * @param  None
 *
 * @return reay or not
 *
*******************************************************************************/
BL_Sts_Type AUADC_FifoDataReady(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_DATA);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_DATA_RDY);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief
 *
 * @param  None
 *
 * @return reay or not
 *
*******************************************************************************/
void AUADC_ADC_Config(AUADC_ADC_AnalogCfg_Type *adc_cfg)
{
    uint32_t tmpVal = 0;
    uint8_t ch_en = 0;

    if (adc_cfg == NULL) {
        return;
    }

    /* audadc_ana_cfg1 */
    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_ANA_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_SEL_EDGE, adc_cfg->adc_edge_mode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_CKB_EN, adc_cfg->adc_clock_phase_invert_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_PGA_LP_EN, adc_cfg->adc_clock_phase_invert_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_ICTRL_PGA_MIC, adc_cfg->pga_opmic_bias_cur);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_ICTRL_PGA_AAF, adc_cfg->pga_opaaf_bias_cur);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_PGA_RHPAS_SEL, adc_cfg->pga_hf_res);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_PGA_CHOP_CFG, adc_cfg->pga_chopper);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_PGA_CHOP_EN, adc_cfg->pga_chopper_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_PGA_CHOP_FREQ, adc_cfg->pga_chopper_freq);
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_PGA_CHOP_CKSEL, adc_cfg->pga_chopper_clk_source);
    BL_WR_REG(AUADC_BASE, AUADC_AUDADC_ANA_CFG1, tmpVal);

    /* audadc_ana_cfg2 */
    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_ANA_CFG2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_SDM_LP_EN, adc_cfg->sdm_lowpower_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_ICTRL_ADC, adc_cfg->sdm_bias_cur);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_NCTRL_ADC1, adc_cfg->sdm_i_first_num);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_NCTRL_ADC2, adc_cfg->sdm_i_sec_num);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_DEM_EN, adc_cfg->dem_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_QUAN_GAIN, adc_cfg->sdm_qg);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_DITHER_ENA, adc_cfg->sdm_dither_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_DITHER_SEL, adc_cfg->sdm_dither_level);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_DITHER_ORDER, adc_cfg->sdm_dither_order);
    BL_WR_REG(AUADC_BASE, AUADC_AUDADC_ANA_CFG2, tmpVal);

    /* audadc_cmd */
    tmpVal = BL_RD_REG(AUADC_BASE, AUADC_AUDADC_CMD);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_PGA_PU, adc_cfg->pga_circuit_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_SDM_PU, adc_cfg->sdm_circuit_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_CONV, 0);
#if 1// fixme add new function for it
    if (adc_cfg->pga_positive_en) {
        ch_en |= (0x01 << 1);
    }
    if (adc_cfg->pga_negative_en) {
        ch_en |= (0x01 << 0);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_CHANNEL_EN, ch_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_CHANNEL_SELP, adc_cfg->pga_posi_ch);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_CHANNEL_SELN, adc_cfg->pga_nega_ch);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_PGA_MODE, adc_cfg->pga_coupled_mode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_PGA_GAIN, adc_cfg->pga_gain);
#endif
    if (adc_cfg->adc_mode == AUADC_ADC_FILT_MODE_AUDIO) {
        // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_AUDIO_FILTER_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_MEAS_FILTER_EN, 0);
    } else if (adc_cfg->adc_mode == AUADC_ADC_FILT_MODE_MEASURE) {
        // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_AUDIO_FILTER_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_MEAS_FILTER_EN, 1);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_MEAS_FILTER_TYPE, adc_cfg->measure_filter_mode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_MEAS_ODR_SEL, adc_cfg->measure_rate);
    BL_WR_REG(AUADC_BASE, AUADC_AUDADC_CMD, tmpVal);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUADC_AUDADC_CONV, 1);
    BL_WR_REG(AUADC_BASE, AUADC_AUDADC_CMD, tmpVal);
}

/****************************************************************************/ /**
 * @brief  auadc interrupt function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#if 0//ndef BFLB_USE_HAL_DRIVER
void AUADC_IRQHandler(void)
{
    uint8_t intIndex = 0;

    for (intIndex = 0; intIndex < AUADC_INT_NUM_ALL; intIndex++) {
        if (AUADC_GetIntStatus(intIndex) == SET) {
            if (auadcIntCbfArra[intIndex] != NULL) {
                auadcIntCbfArra[intIndex]();
            }
            AUADC_IntClear(intIndex);
        }
    }
}
#endif

/*@} end of group AUADC_Public_Functions */

/*@} end of group AUADC */

/*@} end of group BL616_Peripheral_Driver */
