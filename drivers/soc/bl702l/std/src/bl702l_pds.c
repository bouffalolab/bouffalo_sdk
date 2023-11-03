/**
  ******************************************************************************
  * @file    bl702l_pds.c
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

#include "bl702l.h"
#include "bl702l_pds.h"
#include "bl702l_hbn.h"
#include "bl702l_ef_cfg.h"

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  PDS
 *  @{
 */

/** @defgroup  PDS_Private_Macros
 *  @{
 */

/*@} end of group PDS_Private_Macros */

/** @defgroup  PDS_Private_Types
 *  @{
 */

/*@} end of group PDS_Private_Types */

/** @defgroup  PDS_Private_Variables
 *  @{
 */
#ifndef BFLB_USE_HAL_DRIVER
static intCallback_Type *pdsIntCbfArra[PDS_INT_MAX] = { NULL };
#endif

/*@} end of group PDS_Private_Variables */

/** @defgroup  PDS_Global_Variables
 *  @{
 */

/*@} end of group PDS_Global_Variables */

/** @defgroup  PDS_Private_Fun_Declaration
 *  @{
 */

/*@} end of group PDS_Private_Fun_Declaration */

/** @defgroup  PDS_Private_Functions
 *  @{
 */

/*@} end of group PDS_Private_Functions */

/** @defgroup  PDS_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  set gpio pad pull type in pds
 *
 * @param  pin: gpio type
 * @param  cfg: pull up en & pull down en & ie en & oe en
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_Type pin, PDS_GPIO_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_PDS_GPIO_TYPE(pin));

    /* pu/pd/ie config */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PU_SET);
    if (cfg->pu_en) {
        tmpVal |= (1 << pin);
    } else {
        tmpVal &= ~(1 << pin);
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_PU_SET, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_SET);
    if (cfg->pd_en) {
        tmpVal |= (1 << pin);
    } else {
        tmpVal &= ~(1 << pin);
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_PD_SET, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_IE_SET);
    if (cfg->ie_en) {
        tmpVal |= (1 << pin);
    } else {
        tmpVal &= ~(1 << pin);
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_IE_SET, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_OE_SET);
    if (cfg->oe_en) {
        tmpVal |= (1 << pin);
    } else {
        tmpVal &= ~(1 << pin);
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_OE_SET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad value in pds
 *
 * @param  pad: gpio type
 * @param  val: GPIO value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_GPIO_Write(PDS_GPIO_GROUP_SET_Type grp, uint32_t val)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN);

    if (val > 0) {
        tmpVal |= (1 << grp);
    } else {
        tmpVal &= ~(1 << grp);
    }
    BL_WR_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad int mask type in pds
 *
 * @param  pad: gpio type
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMask(PDS_GPIO_GROUP_SET_Type grp, BL_Mask_Type intMask)
{
    uint32_t tmpVal;
    uint32_t tmpValMask;

    CHECK_PARAM(IS_PDS_GPIO_GROUP_SET_TYPE(grp));

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN);
    tmpValMask = BL_GET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_SET_INT_MASK);
    if (intMask) {
        tmpValMask |= (1 << grp);
    } else {
        tmpValMask &= ~(1 << grp);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_SET_INT_MASK, tmpValMask);
    BL_WR_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad trig type in pds
 *
 * @param  set: set type
 * @param  trig: trig type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_GROUP_SET_Type grp, PDS_GPIO_INT_TRIG_Type trig)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_PDS_GPIO_GROUP_SET_TYPE(grp));
    CHECK_PARAM(IS_PDS_GPIO_INT_TRIG_TYPE(trig));

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT_SET);
    tmpVal &= ~(0xF << (grp << 2));
    tmpVal |= (trig << (grp << 2));
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT_SET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad int clear in pds
 *
 * @param  set: set type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_GROUP_SET_Type grp)
{
    uint32_t tmpVal = 0;
    uint32_t offset = 0;

    CHECK_PARAM(IS_PDS_GPIO_INT_SET_TYPE(grp));

    offset = grp + PDS_CR_PDS_GPIO0_SET_INT_CLR_POS;

    /* pds_gpio_setx_int_clr = 0 */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN);
    tmpVal &= ~(1 << offset);
    BL_WR_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN, tmpVal);

    /* pds_gpio_setx_int_clr = 1 */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN);
    tmpVal |= (1 << offset);
    BL_WR_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN, tmpVal);

    /* pds_gpio_setx_int_clr = 0 */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN);
    tmpVal &= ~(1 << offset);
    BL_WR_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PDS gpio interrupt clear
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_GPIO_IntClear(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN);
    tmpVal &= ~(0xFF << PDS_CR_PDS_GPIO0_SET_INT_CLR_POS);
    BL_WR_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN);
    tmpVal |= (0xFF << PDS_CR_PDS_GPIO0_SET_INT_CLR_POS);
    BL_WR_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN);
    tmpVal &= ~(0xFF << PDS_CR_PDS_GPIO0_SET_INT_CLR_POS);
    BL_WR_REG(PDS_BASE, PDS_CFG_PDS_KEY_SCAN, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get gpio pad int status
 *
 * @param  pad: gpio type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Sts_Type ATTR_TCM_SECTION PDS_Get_GPIO_Pad_IntStatus(PDS_GPIO_Type pin)
{
    CHECK_PARAM(IS_PDS_GPIO_TYPE(pin));

    return (BL_RD_REG(PDS_BASE, PDS_GPIO_STAT) & (1 << pin)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  set flash pad pull none
 *
 * @param  pinCfg: flash pin type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None(SF_Ctrl_Pin_Select pinCfg)
{
    PDS_GPIO_Cfg_Type pds_io_cfg = {
        .pd_en = 0,
        .pu_en = 0,
        .ie_en = 0,
        .oe_en = 0,
    };

    if (pinCfg == SF_CTRL_SEL_EXTERNAL_FLASH) {
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_23, &pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_24, &pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_25, &pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_26, &pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_27, &pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_28, &pds_io_cfg);
    } else {
        return INVALID;
    }

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  set flash pad pull none
 *
 * @param  pinCfg: flash pin type
 *
 * @return SUCCESS or ERROR
 *
 * @note ext_flash need call this function after pds mode
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None_Fast(SF_Ctrl_Pin_Select pinCfg)
{
    uint32_t tmpVal;

    if (pinCfg == SF_CTRL_SEL_EXTERNAL_FLASH) {
        /* pd config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_SET);
        /* GPIO23~28 [23]~[28] = 0 */
        tmpVal &= ~(0x1F800000);
        BL_WR_REG(PDS_BASE, PDS_GPIO_PD_SET, tmpVal);

        /* pu config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PU_SET);
        /* GPIO23~28 [23]~[28] = 0 */
        tmpVal &= ~(0x1F800000);
        BL_WR_REG(PDS_BASE, PDS_GPIO_PU_SET, tmpVal);

        /* ie config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_IE_SET);
        /* GPIO23~28 [23]~[28] = 0 */
        tmpVal &= ~(0x1F800000);
        BL_WR_REG(PDS_BASE, PDS_GPIO_IE_SET, tmpVal);

        /* oe config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_OE_SET);
        /* GPIO23~28 [23]~[28] = 0 */
        tmpVal &= ~(0x1F800000);
        BL_WR_REG(PDS_BASE, PDS_GPIO_OE_SET, tmpVal);
    } else {
        return INVALID;
    }
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable PDS GPIO Keep
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Disable_GPIO_Keep(void)
{
    uint32_t tmpVal = 0;

    /* PDS_IO keep disable */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_GPIO_ISO_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_GPIO_KEEP_EN);
    /* don't entry PDS */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_START_PS);
    BL_WR_REG(PDS_BASE, PDS_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enabel PDS GPIO Keep
 *
 * @param  keepSel:
 * @return SUCCESS
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION PDS_Enable_PDS_Pad_Keep(uint32_t keepSel)
{
    uint32_t tmpVal = 0;

    /* PDS_IO keep disable */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_GPIO_ISO_MODE);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_SW_GPIO_ISO_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_KEEP_EN,keepSel);
    /* don't entry PDS */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_START_PS);
    BL_WR_REG(PDS_BASE, PDS_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable power down sleep
 *
 * @param  cfg: power down sleep configuration 1
 * @param  cfg4: power down sleep configuration 2
 * @param  pdsSleepCnt: power down sleep count cycle
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt)
{
    uint32_t tmpVal = 0;
    uint32_t valLow = 0, valHigh = 0;
    uint64_t val;

    /* PDS sleep time 1~PDS_WARMUP_LATENCY_CNT <=> error */
    /* PDS sleep time >PDS_WARMUP_LATENCY_CNT <=> correct */
    if ((pdsSleepCnt) && (pdsSleepCnt <= PDS_WARMUP_LATENCY_CNT)) {
        return ERROR;
    } else if (pdsSleepCnt > PDS_WARMUP_LATENCY_CNT) {
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        val = valLow + ((uint64_t)valHigh << 32);
        val += pdsSleepCnt - PDS_WARMUP_LATENCY_CNT;
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, val & 0xffffffff, val >> 32, HBN_RTC_COMP_BIT0_39);
        HBN_Enable_RTC_Counter();
    } else {
        /* PDS sleep time 0 ,means mask pds_timer wakeup  */
    }

    /* PDS_CTL4 config */
    BL_WR_REG(PDS_BASE, PDS_CTL4, *(uint32_t *)cfg4);

    /* PDS_CTL config */
    if (cfg->pdsStart) {
        /* clear pds int */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
        tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
        BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

        tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
        tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
        BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

        BL_WR_REG(PDS_BASE, PDS_CTL, (*(uint32_t *)cfg & ~(1 << 0)));
        BL_WR_REG(PDS_BASE, PDS_CTL, (*(uint32_t *)cfg | (1 << 0)));
    } else {
        BL_WR_REG(PDS_BASE, PDS_CTL, *(uint32_t *)cfg);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep force configure
 *
 * @param  cfg2: power down sleep force configuration 1
 * @param  cfg3: power down sleep force configuration 2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3)
{
    /* PDS_CTL2 config */
    BL_WR_REG(PDS_BASE, PDS_CTL2, *(uint32_t *)cfg2);

    /* PDS_CTL3 config */
    BL_WR_REG(PDS_BASE, PDS_CTL3, *(uint32_t *)cfg3);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep ram configure
 *
 * @param  ramCfg: power down sleep force ram configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_RAM_Config(PDS_RAM_CFG_Type *ramCfg)
{
    if (NULL == ramCfg) {
        return ERROR;
    }

    /* PDS_RAM1 config */
    BL_WR_REG(PDS_BASE, PDS_RAM1, *(uint32_t *)ramCfg);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep force configure
 *
 * @param  defaultLvCfg: power down sleep default level configuration
 * @param  pdsSleepCnt: power down sleep time count
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt)
{
    PDS_Force_Config((PDS_CTL2_Type *)&(defaultLvCfg->pdsCtl2), (PDS_CTL3_Type *)&(defaultLvCfg->pdsCtl3));
    PDS_Enable((PDS_CTL_Type *)&(defaultLvCfg->pdsCtl), (PDS_CTL4_Type *)&(defaultLvCfg->pdsCtl4), pdsSleepCnt);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep wakeup event enable
 *
 * @param  intType: PDS Wakeup Source type
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_Type intType, BL_Fun_Type enable)
{
    uint32_t offset = 0;
    uint32_t tmpVal = 0;

    if (intType > PDS_WAKEUP_SRC_WDG_TIMEOUT) {
        return ERROR;
    }

    offset = intType + PDS_CR_PDS_WAKEUP_SRC_EN_POS;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);

    if (enable) {
        tmpVal = tmpVal | (1 << offset);
    } else {
        tmpVal = tmpVal & ~(1 << offset);
    }

    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get power down sleep Wakeup Source type
 *
 * @param  intType: PDS int type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type PDS_Get_Wakeup_Src(PDS_WAKEUP_SRC_Type intType)
{
    uint32_t offset = 0;

    if (intType > PDS_WAKEUP_SRC_WDG_TIMEOUT) {
        return RESET;
    }

    offset = intType + PDS_RO_PDS_WAKEUP_EVENT_POS;

    return (BL_RD_REG(PDS_BASE, PDS_INT) & (1 << offset)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  power down sleep int mask
 *
 * @param  intType: PDS int type
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_IntMask(PDS_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_PDS_INT_TYPE(intType));

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);

    if (intMask != UNMASK) {
        tmpVal = tmpVal | (1 << (intType + PDS_CR_PDS_WAKE_INT_MASK_POS));
    } else {
        tmpVal = tmpVal & ~(1 << (intType + PDS_CR_PDS_WAKE_INT_MASK_POS));
    }

    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get power down sleep int status
 *
 * @param  intType: PDS int type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type PDS_Get_IntStatus(PDS_INT_Type intType)
{
    CHECK_PARAM(IS_PDS_INT_TYPE(intType));

    return (BL_RD_REG(PDS_BASE, PDS_INT) & (1 << intType)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  clear power down sleep int status
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_IntClear(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get power down sleep RF status
 *
 * @param  None
 *
 * @return PDS RF status
 *
*******************************************************************************/
PDS_RF_STS_Type PDS_Get_PdsRfStstus(void)
{
    return (PDS_RF_STS_Type)BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE, PDS_STAT), PDS_RO_PDS_RF_STATE);
}

/****************************************************************************/ /**
 * @brief  get power down sleep status
 *
 * @param  None
 *
 * @return PDS status
 *
*******************************************************************************/
PDS_STS_Type PDS_Get_PdsStstus(void)
{
    return (PDS_STS_Type)BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE, PDS_STAT), PDS_RO_PDS_STATE);
}

/****************************************************************************/ /**
 * @brief  power down sleep clear reset event
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Clear_Reset_Event(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CLR_RESET_EVENT);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CLR_RESET_EVENT);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CLR_RESET_EVENT);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get power down sleep reset event
 *
 * @param  event: power down sleep reset event
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type PDS_Get_Reset_Event(PDS_RST_EVENT_Type event)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_RESET_EVENT);

    return (tmpVal & (1 << event)) ? SET : RESET;
}

#if 0
/****************************************************************************/ /**
 * @brief  PDS Auto mode wake up counter config
 *
 * @param  sleepDuration: sleep time, total pds = sleep_duration + max_warmup_cnt (32K clock cycles),
 *                        recommend maxWarmCnt*N+2
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION PDS_Auto_Time_Config(uint32_t sleepDuration)
{
    /* PDS_TIME1 */
    BL_WR_REG(PDS_BASE, PDS_TIME1, sleepDuration);
}
#endif
#endif
/****************************************************************************/ /**
 * @brief  PDS Auto mode config and enable
 *
 * @param  powerCfg: PDS Auto mode power domain config
 * @param  normalCfg: PDS Auto mode power normal config
 * @param  enable: PDS Auto mode Enable or Disable
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION PDS_Auto_Enable(PDS_AUTO_POWER_DOWN_CFG_Type *powerCfg, PDS_AUTO_NORMAL_CFG_Type *normalCfg, BL_Fun_Type enable)
{
    uint32_t pdsCtl = 0;

    CHECK_PARAM(IS_PDS_LDO_VOLTAGE_TYPE(normalCfg->vddcoreVol));

    /* power config */
    pdsCtl |= (powerCfg->mbgPower << 31) |
              (powerCfg->ldo18rfPower << 30) |
              (powerCfg->sfregPower << 29) |
              (powerCfg->pllPower << 28) |
              (powerCfg->cpu0Power << 19) |
              (powerCfg->rc32mPower << 17) |
              (powerCfg->xtalPower << 14) |
              (powerCfg->allPower << 13) |
              (powerCfg->isoPower << 11) |
              (powerCfg->bzPower << 10) |
              (powerCfg->sramDisStanby << 9) |
              (powerCfg->cgPower << 8) |
              (powerCfg->cpu1Power << 7) |
              (powerCfg->usbPower << 3);
    // pdsCtl = BL_SET_REG_BITS_VAL(pdsCtl, PDS_CR_PDS_LDO_VOL, normalCfg->vddcoreVol);
    pdsCtl |= (normalCfg->vddcoreVolEn << 18) |
              (normalCfg->cpu0NotNeedWFI << 21) |
              (normalCfg->cpu1NotNeedWFI << 20) |
              (normalCfg->busReset << 16) |
              (normalCfg->disIrqWakeUp << 15) |
              (normalCfg->powerOffXtalForever << 2) |
              (normalCfg->sleepForever << 1);
    BL_WR_REG(PDS_BASE, PDS_CTL, pdsCtl);

    pdsCtl = BL_RD_REG(PDS_BASE, PDS_CTL);

    if (enable) {
        pdsCtl |= (1 << 0);
    } else {
        pdsCtl &= ~(1 << 0);
    }

    BL_WR_REG(PDS_BASE, PDS_CTL, pdsCtl);
}
#endif

/****************************************************************************/ /**
 * @brief  PDS force turn off XXX domain
 *
 * @param  domain: PDS domain
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION PDS_Manual_Force_Turn_Off(PDS_FORCE_Type domain)
{
    uint32_t tmpVal = 0;

    /* memory sleep */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal |= 1 << (domain + PDS_FORCE_MEM_STBY_OFFSET);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    /* gate clock */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal |= 1 << (domain + PDS_FORCE_GATE_CLK_OFFSET);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    /* pds reset */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal |= 1 << (domain + PDS_FORCE_PDS_RST_OFFSET);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    /* isolation on */
    // tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    // tmpVal |= 1 << (domain + PDS_FORCE_ISO_EN_OFFSET);
    // BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    /* power off */
    // tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    // tmpVal |= 1 << (domain + PDS_FORCE_PWR_OFF_OFFSET);
    // BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  PDS force turn on XXX domain
 *
 * @param  domain: PDS domain
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION PDS_Manual_Force_Turn_On(PDS_FORCE_Type domain)
{
    uint32_t tmpVal = 0;

    /* power on */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal &= ~(1 << (domain + PDS_FORCE_PWR_OFF_OFFSET));
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    /* isolation off */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal &= ~(1 << (domain + PDS_FORCE_ISO_EN_OFFSET));
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    /* pds de_reset */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal &= ~(1 << (domain + PDS_FORCE_PDS_RST_OFFSET));
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    /* memory active */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal &= ~(1 << (domain + PDS_FORCE_MEM_STBY_OFFSET));
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    /* clock on */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal &= ~(1 << (domain + PDS_FORCE_GATE_CLK_OFFSET));
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);
}
#endif

/**
 * @brief
 *
 * @param col_size
 * @param row_size
 *
 * @return BL_Err_Type
 */
BL_Err_Type PDS_Set_KYD_Matrix_Size(uint8_t col_size, uint8_t row_size)
{
    uint32_t tmpVal = 0;

    /* set col size */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_KYD_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_KYD_COL_MATRIX_SIZE, col_size);
    BL_WR_REG(PDS_BASE, PDS_KYD_CTL, tmpVal);

    /* set row size */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_KYD_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_KYD_ROW_MATRIX_SIZE, row_size);
    BL_WR_REG(PDS_BASE, PDS_KYD_CTL, tmpVal);

    return SUCCESS;
}
/**
 * @brief
 *
 * @param val
 * @return BL_Err_Type
 */
BL_Err_Type PDS_Set_KYD_Col_Value(uint8_t val)
{
    uint32_t tmpVal = 0;

    /* set col output value */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_KYD_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_KYD_COL_O_VAL, val);
    BL_WR_REG(PDS_BASE, PDS_KYD_CTL, tmpVal);

    return SUCCESS;
}

/**
 * @brief
 *
 * @param en
 * @return BL_Err_Type
 */
BL_Err_Type PDS_Set_KYD_Row_Pull(uint8_t en)
{
    uint32_t tmpVal = 0;

    /* disable/enable pullup or pulldown */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_KYD_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_KYD_ROW_I_PULL_EN, en);
    BL_WR_REG(PDS_BASE, PDS_KYD_CTL, tmpVal);

    return SUCCESS;
}

/**
 * @brief
 *
 * @param en
 * @return BL_Err_Type
 */
BL_Err_Type PDS_Set_KYD_Wakeup_En(uint8_t en)
{
    uint32_t tmpVal = 0;

    /* disable/enable cr_pds_kyd_en  */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_KYD_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_KYD_EN, en);
    BL_WR_REG(PDS_BASE, PDS_KYD_CTL, tmpVal);

    PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_KYD_WAKEUP, en);

    return SUCCESS;
}

/**
 * @brief PDS Clear KYD Wakeup Flag
 *
 * @return BL_Err_Type
 */
__WEAK
BL_Err_Type PDS_Clear_KYD_Wakeup(void)
{
    uint32_t tmpVal = 0;

    /* disable/enable cr_pds_kyd_en  */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_KYD_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CLR_PDS_KYD_WAKEUP);
    BL_WR_REG(PDS_BASE, PDS_KYD_CTL, tmpVal);

    return SUCCESS;
}

/**
 * @brief set gpio x binding to keyscan row input lines
 *
 * @param row_x
 * @param sel_io
 * @return BL_Err_Type
 */
BL_Err_Type PDS_Set_KYS_ROW_IN_GPIO(PDS_KEY_ROW_Type row_x, PDS_KYS_GPIO_Type sel_io)
{
    CHECK_PARAM(IS_PDS_KYS_ROW_TYPE(row_x));
    CHECK_PARAM(IS_PDS_KYS_GPIO_TYPE(sel_io));

    uint32_t tmp_val = 0;
    /* select  gpio x binding to keyscan row input lines. */
    switch (row_x) {
        case PDS_KEY_ROW_0:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL0);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_ROW_I_0_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL0, tmp_val);
            break;
        case PDS_KEY_ROW_1:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL0);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_ROW_I_1_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL0, tmp_val);
            break;
        case PDS_KEY_ROW_2:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL0);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_ROW_I_2_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL0, tmp_val);
            break;
        case PDS_KEY_ROW_3:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL0);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_ROW_I_3_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL0, tmp_val);
            break;
        case PDS_KEY_ROW_4:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL1);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_ROW_I_4_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL1, tmp_val);
            break;
        case PDS_KEY_ROW_5:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL1);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_ROW_I_5_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL1, tmp_val);
            break;
        case PDS_KEY_ROW_6:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL1);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_ROW_I_6_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL1, tmp_val);
            break;
        case PDS_KEY_ROW_7:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL1);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_ROW_I_7_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_ROW_I_GPIO_SEL1, tmp_val);
            break;

        default:
            break;
    }
    return SUCCESS;
}

/**
 * @brief select gpio x binding to keyscan col output lines
 *
 * @param col_x
 * @param sel_io
 * @return BL_Err_Type
 */
BL_Err_Type PDS_Set_KYS_COL_OUT_GPIO(PDS_KEY_COL_Type col_x, PDS_KYS_GPIO_Type sel_io)
{
    CHECK_PARAM(IS_PDS_KYS_COL_TYPE(col_x));
    CHECK_PARAM(IS_PDS_KYS_GPIO_TYPE(sel_io));

    uint32_t tmp_val = 0;
    /* select  gpio x binding to keyscan col output lines. */

    switch (col_x) {
        case PDS_KEY_COL_0:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL0);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_COL_O_0_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL0, tmp_val);
            break;
        case PDS_KEY_COL_1:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL0);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_COL_O_1_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL0, tmp_val);
            break;
        case PDS_KEY_COL_2:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL0);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_COL_O_2_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL0, tmp_val);
            break;
        case PDS_KEY_COL_3:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL0);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_COL_O_3_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL0, tmp_val);
            break;
        case PDS_KEY_COL_4:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL1);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_COL_O_4_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL1, tmp_val);
            break;
        case PDS_KEY_COL_5:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL1);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_COL_O_5_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL1, tmp_val);
            break;
        case PDS_KEY_COL_6:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL1);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_COL_O_6_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL1, tmp_val);
            break;
        case PDS_KEY_COL_7:
            tmp_val = BL_RD_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL1);
            tmp_val = BL_SET_REG_BITS_VAL(tmp_val, PDS_CFG_KEY_COL_O_7_GPIO_SEL, sel_io);
            BL_WR_REG(PDS_BASE, PDS_KYS_COL_O_GPIO_SEL1, tmp_val);
            break;

        default:
            break;
    }
    return SUCCESS;
}

/**
 * @brief PDS Get KYS Wakeup by col output and gpio x
 *
 * @param row_index
 * @param row_gpio
 * @return BL_Err_Type
 */
__WEAK
BL_Err_Type PDS_Get_KYS_Wakeup_ROW_INDEX_GPIO(PDS_KEY_ROW_Type* row_index,PDS_KYS_GPIO_Type* row_gpio)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_KYD_CTL);
    *row_index = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX);
    *row_gpio = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO);

    return SUCCESS;
}

/**
 * @brief PDS Get KYS Wakeup by row input lines and gpio x
 *
 * @param col_index
 * @param col_gpio
 * @return BL_Err_Type
 */
__WEAK
BL_Err_Type PDS_Get_KYS_Wakeup_COL_INDEX_GPIO(PDS_KEY_COL_Type* col_index,PDS_KYS_GPIO_Type* col_gpio)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_KYD_CTL);
    *col_index = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_KYD_WAKEUP_COL_INDEX);
    *col_gpio = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_KYD_WAKEUP_COL_GPIO);

    return SUCCESS;
}

/**
 * @brief PDS Set KYS White_Key
 *
 * @param white_key_i
 * @param row_index
 * @param col_index
 * @param type
 * @return BL_Err_Type
 */
__WEAK
BL_Err_Type PDS_Set_KYS_White_Key(uint8_t white_key_i ,PDS_KEY_ROW_Type row_index,PDS_KEY_COL_Type col_index,PDS_KYD_WHITE_KEY_MODE_Type type)
{
    if( white_key_i > 3 ) {
        return ERROR;
    }

    uint32_t tmpVal = 0;
    uint8_t bits_offset = 0;
    bits_offset = 8 * white_key_i;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_KYD_WHITE_SET);

    tmpVal &= ~(0xF << bits_offset );
    tmpVal |= (row_index << bits_offset );
    tmpVal |= (col_index << (3 + bits_offset));
    tmpVal |= (type << (6 + bits_offset));

    BL_WR_REG(PDS_BASE, PDS_KYD_WHITE_SET, tmpVal);

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  Install PDS interrupt callback function
 *
 * @param  intType: PDS int type
 * @param  cbFun: cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
BL_Err_Type PDS_Int_Callback_Install(PDS_INT_Type intType, intCallback_Type *cbFun)
{
#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(PDS_WAKEUP_IRQn, PDS_WAKEUP_IRQHandler);
#endif
    pdsIntCbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Power down sleep wake up interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void PDS_WAKEUP_IRQHandler(void)
{
    for (PDS_INT_Type intType = PDS_INT_WAKEUP; intType < PDS_INT_MAX; intType++) {
        if (PDS_Get_IntStatus(intType) && (pdsIntCbfArra[intType] != NULL)) {
            pdsIntCbfArra[intType]();
        }
    }

    PDS_IntClear();
}
#endif

/*@} end of group PDS_Public_Functions */

/*@} end of group PDS */

/*@} end of group BL702L_Peripheral_Driver */
