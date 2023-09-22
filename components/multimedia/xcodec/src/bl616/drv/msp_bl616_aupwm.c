/**
  ******************************************************************************
  * @file    msp_bl616_aupwm.c
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
#include "msp_bl616_aupwm.h"

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  AUPWM
 *  @{
 */

/** @defgroup  AUPWM_Private_Macros
 *  @{
 */

/*@} end of group AUPWM_Private_Macros */

/** @defgroup  AUPWM_Private_Types
 *  @{
 */

/*@} end of group AUPWM_Private_Types */

/** @defgroup  AUPWM_Private_Variables
 *  @{
 */

/*@} end of group AUPWM_Private_Variables */

/** @defgroup  AUPWM_Global_Variables
 *  @{
 */
//static intCallback_Type *AUPWMIntCbfArra[AUPWM_INT_NUM_ALL] = { NULL };

/*@} end of group AUPWM_Global_Variables */

/** @defgroup  AUPWM_Private_Fun_Declaration
 *  @{
 */

/*@} end of group AUPWM_Private_Fun_Declaration */

/** @defgroup  AUPWM_Private_Functions
 *  @{
 */

/*@} end of group AUPWM_Private_Functions */

/** @defgroup  AUPWM_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  aupwm init
 *
 * @param  cfg: cfg
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_Init(AUPWM_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    if(cfg->clkEn){

        tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_CKG_ENA, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_AU_PWM_MODE, cfg->pwm_mode);
        BL_WR_REG(AUPWM_BASE, AUPWM_0, tmpVal);

        tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_CKG_ENA, 1);
        BL_WR_REG(AUPWM_BASE, AUPWM_0, tmpVal);

    }else{

        tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_CKG_ENA, 0);
        BL_WR_REG(AUPWM_BASE, AUPWM_0, tmpVal);
    }

    #if 0//ndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(AUDIO_IRQn, AUPWM_IRQHandler);
    #endif
}

/****************************************************************************/ /**
 * @brief  aupwm init
 *
 * @param  cfg: cfg
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_0_EN, 1);

    BL_WR_REG(AUPWM_BASE, AUPWM_0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  aupwm enable
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_0);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_0_EN, 0);

    BL_WR_REG(AUPWM_BASE, AUPWM_0, tmpVal);
}


/****************************************************************************/ /**
 * @brief  aupwm Disable
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_FifoInit(AUPWM_FifoCfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_MIX_SEL, cfg->mixType);
    BL_WR_REG(AUPWM_BASE, AUPWM_1, tmpVal);

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_ITF_EN, cfg->dmaEn);
    BL_WR_REG(AUPWM_BASE, AUPWM_0, tmpVal);

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_TX_DATA_MODE, cfg->ailgnMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_TX_TRG_LEVEL, cfg->FifoIntThreshold);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_TX_DRQ_CNT, cfg->dmaThresholdMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_TX_DRQ_EN, cfg->dmaEn);
    BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);
}

void AUPWM_FifoDisable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_TX_DRQ_EN, DISABLE);
    BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);
}

void AUPWM_FifoEnable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_TX_DRQ_EN, ENABLE);
    BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);
}

/****************************************************************************/ /**
 * @brief  AUPWM_SetMixerChannel
 *
 * @param  mixerSrc: if FIFO data generate at {L,L,L,L,L..} by AUPWM_Start
 *                   if FIFO data generate at {R,R,R,R,R..} by AUPWM_Start
 *                   if FIFO data generate at {L,R,L,R....} by AUPWM_Start
 *                   user can choose L or R or L+R/2 into adc by
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_SetMixerChannel(AUPWM_Mixer_Type mixerSrc){

    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_MIX_SEL, mixerSrc);
    BL_WR_REG(AUPWM_BASE, AUPWM_1, tmpVal);

}
/****************************************************************************/ /**
 * @brief  aupwm Start
 *
 * @param  chan: if enable AUPWM_MONO_LEFT , FIFO data will generate at {L,L,L,L,L..}
 *               if enable AUPWM_MONO_RIGHT, FIFO data will generate at {R,R,R,R,R..}
 *               if enable AUPWM_STEREO    , FIFO data will generate at {L,R,L,R....}
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_Start(AUPWM_Channel_Mode chan){
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_TX_CH_EN, chan);
    BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);
}
/****************************************************************************/ /**
 * @brief  aupwm Stop
 *
 * @param  chan:
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_Stop(AUPWM_Channel_Mode chan){

    uint32_t tmpVal=0;
    uint32_t tmpVal2=0;

    tmpVal = BL_RD_REG(AUPWM_BASE,AUPWM_FIFO_CTRL);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal,AUPWM_TX_CH_EN);

    tmpVal2 &= ~chan;

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal,AUPWM_TX_CH_EN,tmpVal2);

    BL_WR_REG(AUPWM_BASE,AUPWM_FIFO_CTRL,tmpVal);
}

/****************************************************************************/ /**
 * @brief  aupwm clear tx fifo
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_FifoClear(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_TX_FIFO_FLUSH, 1);
    BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);
}


/****************************************************************************/ /**
 * @brief  aupwm write data to fifo
 *
 * @param  data
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_SendData(uint32_t data)
{
    BL_WR_REG(AUPWM_BASE,AUPWM_FIFO_DATA,data);
}


/****************************************************************************/ /**
 * @brief  Get Tx FIFO Count
 *
 * @param  None
 *
 * @return fifi count
 *
*******************************************************************************/
uint32_t AUPWM_GetFifoCount(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_STATUS);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, AUPWM_TXA_CNT);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  aupwm configure zero detect function
 *
 * @param  enable: enable or not
 * @param  time: zero time to stop output
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_ZeroDetectInit(uint8_t enable, uint16_t time)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_ZD_0);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_ZD_EN, enable);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_ZD_TIME, time);

    BL_WR_REG(AUPWM_BASE, AUPWM_ZD_0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  get zero detect status
 *
 * @param  None
 *
 * @return zeroDetect status
 *
*******************************************************************************/
uint8_t AUPWM_GetZeroDetectStatus(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_STATUS);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, AUPWM_ZD_AMUTE);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  set dac amp
 *
 * @param  enable: enable or not
 * @param  prbsMode: prbs mode
 * @param  amp: amp
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_DitherInit(uint8_t enable, AUPWM_PRBS_Mode_Type prbsMode, uint8_t amp)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_1);

    if (enable) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_DSM_DITHER_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_DSM_DITHER_AMP, amp);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_DSM_DITHER_PRBS_MODE, prbsMode);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_DSM_DITHER_EN, 0);
    }

    BL_WR_REG(AUPWM_BASE, AUPWM_1, tmpVal);
}

/****************************************************************************/ /**
 * @brief  set dac scaling
 *
 * @param  enable: enable or noy
 * @param  scaling: scaling
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_ScalingInit(uint8_t enable, uint8_t scaling)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_1);
    if (enable) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_DSM_SCALING_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_DSM_SCALING_MODE, scaling);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_DSM_SCALING_EN, 0);
    }
    BL_WR_REG(AUPWM_BASE, AUPWM_1, tmpVal);
}

/****************************************************************************/ /**
 * @brief  set order
 *
 * @param  order: order
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_OrderInit(AUPWM_DAC_Order_Type order)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_AUPWM_DAC_ORDER_TYPE(order));

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_DSM_ORDER, order);
    BL_WR_REG(AUPWM_BASE, AUPWM_1, tmpVal);
}

/****************************************************************************/ /**
 * @brief  set AUPWM int mask
 *
 * @param  intType: intType
 * @param  intMask: intMask
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_IntMask(AUPWM_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_AUPWM_INT_TYPE(intType));
    CHECK_PARAM(IS_BL_MASK_TYPE(intMask));

    switch (intType) {
        case AUPWM_INT_TX_FIFO_THR:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
            if (intMask) {
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUPWM_TXA_INT_EN);
            } else {
                tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXA_INT_EN);
            }
            BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);
            break;
        case AUPWM_INT_TX_FIFO_OVERRUN:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
            if (intMask) {
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUPWM_TXO_INT_EN);
            } else {
                tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXO_INT_EN);
            }
            BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);
            break;
        case AUPWM_INT_TX_FIFO_UNDERRUN:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
            if (intMask) {
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUPWM_TXU_INT_EN);
            } else {
                tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXU_INT_EN);
            }
            BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);
            break;

        case AUPWM_INT_SET_VOLUME_DONE:

            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_STATUS);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_INT_CLR, intMask);
            BL_WR_REG(AUPWM_BASE, AUPWM_STATUS, tmpVal);
            break;
        case AUPWM_INT_NUM_ALL:
            if (intMask) {
                tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUPWM_TXA_INT_EN);
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUPWM_TXU_INT_EN);
                tmpVal = BL_CLR_REG_BIT(tmpVal, AUPWM_TXO_INT_EN);
                BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);

                tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_STATUS);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_INT_CLR, 1);
                BL_WR_REG(AUPWM_BASE, AUPWM_STATUS, tmpVal);

            } else {
                tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_CTRL);
                tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXA_INT_EN);
                tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXU_INT_EN);
                tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXO_INT_EN);
                BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_CTRL, tmpVal);

                tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_STATUS);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_INT_CLR, 1);
                BL_WR_REG(AUPWM_BASE, AUPWM_STATUS, tmpVal);
            }

        default:
            break;
    }
}

/****************************************************************************/ /**
 * @brief  clear AUPWM int flag
 *
 * @param  intType: intType
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_IntClear(AUPWM_INT_Type intType)
{
    uint32_t tmpVal = 0,tmpVal2=0;

    CHECK_PARAM(IS_AUPWM_INT_TYPE(intType));

    switch (intType) {
        case AUPWM_INT_TX_FIFO_THR:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_STATUS);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXA_INT);
            BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_STATUS, tmpVal);
            break;

        case AUPWM_INT_TX_FIFO_OVERRUN:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_STATUS);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXO_INT);
            BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_STATUS, tmpVal);
            break;

        case AUPWM_INT_TX_FIFO_UNDERRUN:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_STATUS);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXU_INT);
            BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_STATUS, tmpVal);
            break;

        case AUPWM_INT_SET_VOLUME_DONE:

            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_STATUS);
            tmpVal2 = tmpVal;
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_INT_CLR, 1);
            BL_WR_REG(AUPWM_BASE, AUPWM_STATUS, tmpVal);

            BL_WR_REG(AUPWM_BASE, AUPWM_STATUS, tmpVal2);
            break;
        case AUPWM_INT_NUM_ALL:

            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_STATUS);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXO_INT);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXU_INT);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_TXA_INT);
            BL_WR_REG(AUPWM_BASE, AUPWM_FIFO_STATUS, tmpVal);

            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_STATUS);
            tmpVal = BL_SET_REG_BIT(tmpVal, AUPWM_DAC_S0_INT_CLR);
            BL_WR_REG(AUPWM_BASE, AUPWM_STATUS, tmpVal);

            break;

        default:
            break;
    }
}
/****************************************************************************/ /**
 * @brief  register callback function
 *
 * @param  intType: intType
 * @param  cbFun: cbFun
 *
 * @return success or not
 *
*******************************************************************************/
#if 0
BL_Err_Type AUPWM_Int_Callback_Install(AUPWM_INT_Type intType, intCallback_Type *cbFun)
{
    AUPWMIntCbfArra[intType] = cbFun;

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
BL_Sts_Type AUPWM_GetIntStatus(AUPWM_INT_Type intType)
{
    uint32_t tmpVal = 0;
    BL_Sts_Type rlt = RESET;

    switch (intType) {
        case AUPWM_INT_TX_FIFO_THR:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_STATUS);
            rlt = BL_IS_REG_BIT_SET(tmpVal, AUPWM_TXA_INT);
            break;
        case AUPWM_INT_TX_FIFO_OVERRUN:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_STATUS);
            rlt = BL_IS_REG_BIT_SET(tmpVal, AUPWM_TXO_INT);
            break;
        case AUPWM_INT_TX_FIFO_UNDERRUN:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_FIFO_STATUS);
            rlt = BL_IS_REG_BIT_SET(tmpVal, AUPWM_TXU_INT);
            break;
        case AUPWM_INT_SET_VOLUME_DONE:
            tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_STATUS);
            rlt = BL_IS_REG_BIT_SET(tmpVal, AUPWM_DAC_S0_INT);
            break;
        default:
            break;
    }

    return rlt;
}

/****************************************************************************/ /**
 * @brief  AUPWM interrupt function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#if 0//ndef BFLB_USE_HAL_DRIVER
void AUPWM_IRQHandler(void)
{
    uint8_t intIndex = 0;

    for (intIndex = 0; intIndex < AUPWM_INT_NUM_ALL; intIndex++) {
        if (AUPWM_GetIntStatus(intIndex) == SET) {
            if (AUPWMIntCbfArra[intIndex] != NULL) {
                AUPWMIntCbfArra[intIndex]();
            }
            AUPWM_IntClear(intIndex);
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  AUPWM adc Mute
 *
 * @param  mute: Mute Or Not
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_SetMute(char mute_en)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_S0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_MUTE, mute_en);
    BL_WR_REG(AUPWM_BASE, AUPWM_S0, tmpVal);
}

void AUPWM_GetMute(char *mute_en)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_S0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_MUTE);
    if (mute_en) {
        if (tmpVal) {
            *mute_en = 1;
        } else {
            *mute_en = 0;
        }
    }
}

/****************************************************************************/ /**
 * @brief  get mute status
 *
 * @param  None
 *
 * @return Status
 *
*******************************************************************************/
char AUPWM_GetMuteStatus(void)
{
    uint32_t tmpVal = 0;
    uint8_t rlt = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_STATUS);
    rlt = BL_IS_REG_BIT_SET(tmpVal, AUPWM_DAC_H0_MUTE_DONE);

    return rlt;
}

/****************************************************************************/ /**
 * @brief  get mute status
 *
 * @param  None
 *
 * @return Status
 *
*******************************************************************************/
uint8_t AUPWM_GetVolumeStatus(void)
{
    uint32_t tmpVal = 0;
    uint8_t rlt = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_STATUS);
    rlt = BL_IS_REG_BIT_SET(tmpVal, AUPWM_DAC_H0_BUSY);

    return rlt;
}

/****************************************************************************/ /**
 * @brief  AUPWM DAC Volume Config
 *
 * @param  dac_id: dac id
 * @param  cfg: volume config cfg pointer
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_VolumeConfig(AUPWM_Volume_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_S0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_MUTE_SOFTMODE, cfg->muteMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_MUTE_RMPDN_RATE, cfg->muteRmpdnRate);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_MUTE_RMPUP_RATE, cfg->muteRmpupRate);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_CTRL_MODE, cfg->volumeCtrMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_CTRL_ZCD_RATE, cfg->volumeZeroCrossRate);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_CTRL_RMP_RATE, cfg->volumeRmpRate);
    BL_WR_REG(AUPWM_BASE, AUPWM_S0, tmpVal);

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_S0_MISC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT, cfg->volumeZeroCrossTimeout);
    BL_WR_REG(AUPWM_BASE, AUPWM_S0_MISC, tmpVal);
}

/****************************************************************************/ /**
 * @brief  AUPWM set adc volume
 *
 * @param  volumeSyncAllAdc: sync volume configuration for three adc channel
 * @param  adc_id: adc id
 * @param  volume: volume
 *
 * @return None
 *
*******************************************************************************/
void AUPWM_SetVolume(uint16_t volume)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_S0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_VOLUME, volume);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_VOLUME_UPDATE, 1);
    BL_WR_REG(AUPWM_BASE, AUPWM_S0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  AUPWM DAC Digital Gain Config
 *
 * @param  gaindb: unit dbm
 *
 * @return 0:success
 *
*******************************************************************************/
int AUPWM_DigitalGainSet(float gaindb)
{
    uint32_t tmpVal = 0;
    uint16_t regval = 0;

    regval = (uint16_t)(512 + gaindb*2);

    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_S0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_VOLUME, regval);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_VOLUME_UPDATE, 1);
    BL_WR_REG(AUPWM_BASE, AUPWM_S0, tmpVal);

    return 0;
}

/****************************************************************************/ /**
 * @brief  AUPWM DAC Digital Gain Config
 *
 * @param  gaindb: unit dbm
 *
 * @return 0:success
 *
*******************************************************************************/
int AUPWM_DigitalGainGet(float *gaindb)
{
    uint32_t tmpVal = 0;
    uint16_t regval = 0;
    int16_t cal = 0;
    int16_t cal2 = 0;
    float db = 0.00;


    tmpVal = BL_RD_REG(AUPWM_BASE, AUPWM_S0);
    regval = BL_GET_REG_BITS_VAL(tmpVal, AUPWM_DAC_S0_VOLUME);

    // get cal2 unit is 0.5db
    if (regval & 0x100) {
        cal = ((int16_t)regval) | 0xFF00;
        cal2 = ~(cal - 1);
        cal2 = cal2 *(-1);
    } else {
        cal = ((int16_t)regval);
        cal2 = cal;
    }

    // real db unit is 1db
    db = (float)cal2 * 0.5;

    if (gaindb) {
        *gaindb = db;
    }

    return 0;
}

/*@} end of group AUPWM_Public_Functions */

/*@} end of group AUPWM */

/*@} end of group BL616_Peripheral_Driver */
