/**
  ******************************************************************************
  * @file    bl702l_kys.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2022 Bouffalo Lab</center></h2>
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

#include "bl702l_kys.h"

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  KYS
 *  @{
 */

/** @defgroup  KYS_Private_Macros
 *  @{
 */

/*@} end of group KYS_Private_Macros */

/** @defgroup  KYS_Private_Types
 *  @{
 */

/*@} end of group KYS_Private_Types */

/** @defgroup  KYS_Private_Variables
 *  @{
 */
static intCallback_Type *KYSIntCbfArra[KYS_INT_ALL] = { NULL };

/*@} end of group KYS_Private_Variables */

/** @defgroup  KYS_Global_Variables
 *  @{
 */

/*@} end of group KYS_Global_Variables */

/** @defgroup  KYS_Private_Fun_Declaration
 *  @{
 */

/*@} end of group KYS_Private_Fun_Declaration */

/** @defgroup  KYS_Private_Functions
 *  @{
 */

/*@} end of group KYS_Private_Functions */

/** @defgroup  KYS_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  KYS initialization function
 *
 * @param  kysCfg: KYS configuration structure pointer
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_Init(KYS_CFG_Type *kysCfg)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
    /* Set col and row */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_COL_NUM, kysCfg->col - 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_ROW_NUM, kysCfg->row - 1);

    /* Enable or disable fifo mode for keycode */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_FIFO_MODE, kysCfg->fifo_mode);

    /* Set idle duration between column scans */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_RC_EXT, kysCfg->idle_duration);

    /* Enable or disable ghost key event detection */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_GHOST_EN, kysCfg->ghost_en);

    /* Enable or disable deglitch function */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_DEG_EN, kysCfg->deglitch_en);

    /* Set deglitch count */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_DEG_CNT, kysCfg->deglitch_cnt);

    /* Write back */
    BL_WR_REG(KYS_BASE, KYS_KS_CTRL, tmpVal);

    // tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL2);

    // /* Set skip col and row */
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_COL_START, kysCfg->col_start);
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_ROW_START, kysCfg->row_start);

    // BL_WR_REG(KYS_BASE, KYS_KS_CTRL2, tmpVal);

#ifndef BFLB_USE_HAL_DRIVER
    // Interrupt_Handler_Register(KYS_IRQn, KYS_IRQHandler);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable KYS
 *
 * @param  None
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
    BL_WR_REG(KYS_BASE, KYS_KS_CTRL, BL_SET_REG_BIT(tmpVal, KYS_KS_EN));

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable KYS
 *
 * @param  None
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
    BL_WR_REG(KYS_BASE, KYS_KS_CTRL, BL_CLR_REG_BIT(tmpVal, KYS_KS_EN));

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  KYS mask or unmask interrupt
 *
 * @param  intMask: KYS interrupt mask value( MASK:disbale interrupt,UNMASK:enable interrupt )
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_IntMask(KYS_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_KYS_INT_TYPE(intType));

    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_INT_EN);

    switch (intType) {
        case KYS_INT_KS_DONE:
            if (MASK == intMask) {
                /* MASK(Disable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (BL_CLR_REG_BIT(tmpVal, KYS_KS_DONE_INT_EN)));
            } else {
                /* UNMASK(Enable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (BL_SET_REG_BIT(tmpVal, KYS_KS_DONE_INT_EN)));
            }
            break;

        case KYS_INT_KEYFIFO_FULL:
            if (MASK == intMask) {
                /* MASK(Disable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (tmpVal & (~(0x1 << KYS_KEYFIFO_FULL_POS))));
            } else {
                /* UNMASK(Enable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (BL_SET_REG_BITS_VAL(tmpVal, KYS_KEYFIFO_INT_EN, 0x1)));
            }
            break;

        case KYS_INT_KEYFIFO_HALF:
            if (MASK == intMask) {
                /* MASK(Disable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (tmpVal & (~(0x1 << KYS_KEYFIFO_HALF_POS))));
            } else {
                /* UNMASK(Enable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (BL_SET_REG_BITS_VAL(tmpVal, KYS_KEYFIFO_INT_EN, 0x2)));
            }
            break;

        case KYS_INT_KEYFIFO_QUARTER:
            if (MASK == intMask) {
                /* MASK(Disable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (tmpVal & (~(0x1 << KYS_KEYFIFO_QUARTER_POS))));
            } else {
                /* UNMASK(Enable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (BL_SET_REG_BITS_VAL(tmpVal, KYS_KEYFIFO_INT_EN, 0x4)));
            }
            break;

        case KYS_INT_KEYFIFO_NONEMPTY:
            if (MASK == intMask) {
                /* MASK(Disable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (tmpVal & (~(0x1 << KYS_KEYFIFO_NONEMPTY_POS))));
            } else {
                /* UNMASK(Enable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (BL_SET_REG_BITS_VAL(tmpVal, KYS_KEYFIFO_INT_EN, 0x8)));
            }
            break;

        case KYS_INT_GHOST_DET:
            if (MASK == intMask) {
                /* MASK(Disable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, BL_CLR_REG_BIT(tmpVal, KYS_GHOST_INT_EN));
            } else {
                /* UNMASK(Enable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, BL_SET_REG_BIT(tmpVal, KYS_GHOST_INT_EN));
            }
            break;

        case KYS_INT_ALL:
            if (MASK == intMask) {
                /* MASK(Disable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (tmpVal & (~((0x1 << KYS_KS_DONE_INT_EN_POS) | (0x7 << KYS_KEYFIFO_INT_EN_POS) | (0x1 << KYS_GHOST_INT_EN_POS)))));
            } else {
                /* UNMASK(Enable) this interrupt */
                BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, (tmpVal | ((0x1 << KYS_KS_DONE_INT_EN_POS) | (0x7 << KYS_KEYFIFO_INT_EN_POS) | (0x1 << KYS_GHOST_INT_EN_POS))));
            }
            break;
        default:
            break;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  KYS clear interrupt
 *
 * @param  intType
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_IntClear(KYS_INT_CLR_Type intType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_KYS_INT_CLR_TYPE(intType));

    /* Clear certain or all interrupt */
    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_INT_STS);

    if (KYS_INT_CLR_ALL == intType) {
        tmpVal |= 0x1180 << KYS_KS_DONE_CLR_POS;
    } else {
        tmpVal |= 1 << (intType);
    }

    BL_WR_REG(KYS_BASE, KYS_KEYCODE_CLR, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Install KYS interrupt callback function
 *
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_Int_Callback_Install(KYS_INT_Type intType, intCallback_Type *cbFun)
{
    KYSIntCbfArra[intType] = cbFun;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  KYS get interrupt status
 *
 * @param  None
 *
 * @return Status of interrupt
 *
*******************************************************************************/
BL_Sts_Type KYS_GetIntStatus(KYS_INT_Type intType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_KYS_INT_TYPE(intType));

    /* Get certain or all interrupt status */
    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_INT_STS);

    if (KYS_INT_ALL == intType) {
        if ((tmpVal & 0x1f80) != 0) {
            return SET;
        } else {
            return RESET;
        }
    } else {
        if ((tmpVal & (1U << (intType + 7U))) != 0) {
            return SET;
        } else {
            return RESET;
        }
    }
}

void KYS_Get_FIFO_Idx(uint8_t *fifo_head, uint8_t *fifo_tail)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(KYS_BASE, KYS_KEYFIFO_IDX);
    *fifo_head = (uint8_t)(tmpVal & 0x7);
    *fifo_tail = (uint8_t)((tmpVal & 0x700) >> 8);
}

uint8_t KYS_ReadKeyfifo(void)
{
    return ((uint8_t)(BL_RD_REG(KYS_BASE, KYS_KEYFIFO_VALUE)));
}

/****************************************************************************/ /**
 * @brief  KYS get keycode value
 *
 * @param  keycode: KYS keycode type
 * @param  col: Col of key
 * @param  row: Row of key
 *
 * @return Keycode value
 *
*******************************************************************************/
// uint8_t KYS_GetKeycode(KYS_Keycode_Type keycode, uint8_t *col, uint8_t *row)
// {
//     uint32_t tmpVal;
//     uint8_t keyValue;

//     /* Get keycode value */
//     keyValue = BL_RD_REG(KYS_BASE, KYS_KEYCODE_VALUE) >> (8 * keycode) & 0xff;

//     /* Get total row number of keyboard */
//     tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, KYS_ROW_NUM);

//     /* Calculate col and row of the key */
//     *col = keyValue / (tmpVal + 1);
//     *row = keyValue % (tmpVal + 1);

//     return keyValue;
// }

/****************************************************************************/ /**
 * @brief  KYS interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
// extern void dv_bflb_platform_printf();
#ifndef BFLB_USE_HAL_DRIVER
void KYS_IRQHandler(void)
{
    // dv_bflb_platform_printf("kys int sts:%lx\r\n", BL_RD_REG(KYS_BASE, KYS_KS_INT_STS));

    if ((KYS_GetIntStatus(KYS_INT_KS_DONE) == SET) && (0x80 & BL_RD_REG(KYS_BASE, KYS_KS_INT_EN))) {
        KYS_IntClear(KYS_INT_KS_DONE_CLR);

        if (KYSIntCbfArra[KYS_INT_KS_DONE] != NULL) {
            KYSIntCbfArra[KYS_INT_KS_DONE]();
        }
    }

    if ((KYS_GetIntStatus(KYS_INT_KEYFIFO_FULL) == SET) && (0x100 & BL_RD_REG(KYS_BASE, KYS_KS_INT_EN))) {
        // KYS_IntClear(KYS_INT_KEYFIFO_CLR);

        if (KYSIntCbfArra[KYS_INT_KEYFIFO_FULL] != NULL) {
            KYSIntCbfArra[KYS_INT_KEYFIFO_FULL]();
        }
    }

    if ((KYS_GetIntStatus(KYS_INT_KEYFIFO_HALF) == SET) && (0x200 & BL_RD_REG(KYS_BASE, KYS_KS_INT_EN))) {
        // KYS_IntClear(KYS_INT_KEYFIFO_CLR);

        if (KYSIntCbfArra[KYS_INT_KEYFIFO_HALF] != NULL) {
            KYSIntCbfArra[KYS_INT_KEYFIFO_HALF]();
        }
    }

    if ((KYS_GetIntStatus(KYS_INT_KEYFIFO_QUARTER) == SET) && (0x400 & BL_RD_REG(KYS_BASE, KYS_KS_INT_EN))) {
        // KYS_IntClear(KYS_INT_KEYFIFO_CLR);

        if (KYSIntCbfArra[KYS_INT_KEYFIFO_QUARTER] != NULL) {
            KYSIntCbfArra[KYS_INT_KEYFIFO_QUARTER]();
        }
    }

    if ((KYS_GetIntStatus(KYS_INT_KEYFIFO_NONEMPTY) == SET) && (0x800 & BL_RD_REG(KYS_BASE, KYS_KS_INT_EN))) {
        // KYS_IntClear(KYS_INT_KEYFIFO_CLR);

        if (KYSIntCbfArra[KYS_INT_KEYFIFO_NONEMPTY] != NULL) {
            KYSIntCbfArra[KYS_INT_KEYFIFO_NONEMPTY]();
        }
    }

    if ((KYS_GetIntStatus(KYS_INT_GHOST_DET) == SET) && (0x1000 & BL_RD_REG(KYS_BASE, KYS_KS_INT_EN))) {
        KYS_IntClear(KYS_INT_GHOST_CLR);

        if (KYSIntCbfArra[KYS_INT_GHOST_DET] != NULL) {
            KYSIntCbfArra[KYS_INT_GHOST_DET]();
        }
    }
}

#endif

/*@} end of group KYS_Public_Functions */

/*@} end of group KYS */

/*@} end of group BL702_Peripheral_Driver */
