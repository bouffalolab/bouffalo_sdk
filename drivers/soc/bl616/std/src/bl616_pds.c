/**
  ******************************************************************************
  * @file    bl616_pds.c
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

#include "bl616.h"
#include "bl616_pds.h"

/** @addtogroup  BL616_Peripheral_Driver
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
 * @param  grp: this parameter can be one of the following values:
 *           @arg PDS_GPIO_GROUP_SET_GPIO0_GPIO15
 *           @arg PDS_GPIO_GROUP_SET_GPIO20_GPIO36
 * @param  pu: power up
 * @param  pd: power down
 * @param  ie: Active IE (interrupt)
 *
 * @return SUCCESS or ERROR
 *
 * @note   Pu and Pd not depend on IE
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(uint8_t grp, uint8_t pu, uint8_t pd, uint8_t ie)
{
    uint32_t tmpVal;
    uint32_t tmpValPu;
    uint32_t tmpValPd;
    uint32_t tmpValIe;

    CHECK_PARAM(IS_PDS_GPIO_GROUP_SET_TYPE(grp));

    /* pu/pd/ie config */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_I_SET);
    tmpValPu = BL_GET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_PU_SET);
    if (pu) {
        if (GLB_PACKAGE_TYPE_QFN56 != GLB_Get_Package_Type()) {
            if (SET == GLB_Get_PAD_Bonging_to_GND_Sts()) {
                /* IF it is 40 PACKAGE TYPE and PAD Bonging to GND, Pull-UP is forbidden */
                tmpValPu &= ~(1 << grp);
            } else {
                tmpValPu |= (1 << grp);
            }
        } else {
            tmpValPu |= (1 << grp);
        }
    } else {
        tmpValPu &= ~(1 << grp);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_PU_SET, tmpValPu);
    tmpValPd = BL_GET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_PD_SET);
    if (pd) {
        tmpValPd |= (1 << grp);
    } else {
        tmpValPd &= ~(1 << grp);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_PD_SET, tmpValPd);
    tmpValIe = BL_GET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_IE_SET);
    if (ie) {
        tmpValIe |= (1 << grp);
    } else {
        tmpValIe &= ~(1 << grp);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_IE_SET, tmpValIe);
    BL_WR_REG(PDS_BASE, PDS_GPIO_I_SET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad int mask type in pds
 *
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~34
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMask(uint8_t pad, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;
    uint32_t pos = 0;

    /* [15:0] : GPIO0~15   */
    /* [30:16] : GPIO20~34 */
    if (pad <= GLB_GPIO_PIN_15) {
        pos = pad;
    } else if ((pad >= GLB_GPIO_PIN_20) && (pad <= GLB_GPIO_PIN_34)) {
        pos = pad - 4;
    } else {
        return ERROR;
    }

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_SET);
    if (UNMASK != intMask) {
        /* MASK */
        tmpVal |= (1 << pos);
    } else {
        /* UNMASK */
        tmpVal &= ~(1 << pos);
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_PD_SET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad trig type in pds
 *
 * @param  set: set type, this parameter can be one of the following values:
 *           @arg PDS_GPIO_INT_SET_1_GPIO0_GPIO7
 *           @arg PDS_GPIO_INT_SET_2_GPIO8_GPIO15
 *           @arg PDS_GPIO_INT_SET_3_GPIO20_GPIO27
 *           @arg PDS_GPIO_INT_SET_4_GPIO28_GPIO34
 * @param  trig: trig type, this parameter can be one of the following values:
 *           @arg PDS_GPIO_INT_SYNC_FALLING_EDGE
 *           @arg PDS_GPIO_INT_SYNC_RISING_EDGE
 *           @arg PDS_GPIO_INT_SYNC_HIGH_LEVEL
 *           @arg PDS_GPIO_INT_ASYNC_FALLING_EDGE
 *           @arg PDS_GPIO_INT_ASYNC_RISING_EDGE
 *           @arg PDS_GPIO_INT_ASYNC_HIGH_LEVEL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMode(uint8_t set, uint8_t trig)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_PDS_GPIO_INT_SET_TYPE(set));
    CHECK_PARAM(IS_PDS_GPIO_INT_TRIG_TYPE(trig));

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    switch (set) {
        case PDS_GPIO_INT_SET_1_GPIO0_GPIO7:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_SET1_INT_MODE, trig);
            break;
        case PDS_GPIO_INT_SET_2_GPIO8_GPIO15:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_SET2_INT_MODE, trig);
            break;
        case PDS_GPIO_INT_SET_3_GPIO20_GPIO27:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_SET3_INT_MODE, trig);
            break;
        case PDS_GPIO_INT_SET_4_GPIO28_GPIO34:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_SET4_INT_MODE, trig);
            break;
        default:
            break;
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad int clear in pds
 *
 * @param  set: set type, this parameter can be one of the following values:
 *           @arg PDS_GPIO_INT_SET_1_GPIO0_GPIO7
 *           @arg PDS_GPIO_INT_SET_2_GPIO8_GPIO15
 *           @arg PDS_GPIO_INT_SET_3_GPIO20_GPIO27
 *           @arg PDS_GPIO_INT_SET_4_GPIO28_GPIO34
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntClr(uint8_t set)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_PDS_GPIO_INT_SET_TYPE(set));

    /* pds_gpio_setx_int_clr = 0 */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    switch (set) {
        case PDS_GPIO_INT_SET_1_GPIO0_GPIO7:
            tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET1_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_2_GPIO8_GPIO15:
            tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET2_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_3_GPIO20_GPIO27:
            tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET3_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_4_GPIO28_GPIO34:
            tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET4_INT_CLR);
            break;
        default:
            break;
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal);

    /* pds_gpio_setx_int_clr = 1 */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    switch (set) {
        case PDS_GPIO_INT_SET_1_GPIO0_GPIO7:
            tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET1_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_2_GPIO8_GPIO15:
            tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET2_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_3_GPIO20_GPIO27:
            tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET3_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_4_GPIO28_GPIO34:
            tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET4_INT_CLR);
            break;
        default:
            break;
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal);

    /* pds_gpio_setx_int_clr = 0 */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    switch (set) {
        case PDS_GPIO_INT_SET_1_GPIO0_GPIO7:
            tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET1_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_2_GPIO8_GPIO15:
            tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET2_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_3_GPIO20_GPIO27:
            tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET3_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_4_GPIO28_GPIO34:
            tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET4_INT_CLR);
            break;
        default:
            break;
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get gpio pad int status
 *
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~34
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Sts_Type ATTR_TCM_SECTION PDS_Get_GPIO_Pad_IntStatus(uint8_t pad)
{
    uint32_t pos = 0;

    /* [15:0]  : GPIO0~15  */
    /* [30:16] : GPIO20~34 */
    if (pad <= GLB_GPIO_PIN_15) {
        pos = pad;
    } else if ((pad >= GLB_GPIO_PIN_20) && (pad <= GLB_GPIO_PIN_34)) {
        pos = pad - 4;
    } else {
        return RESET;
    }

    return (BL_RD_REG(PDS_BASE, PDS_GPIO_STAT) & (1 << pos)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  set flash pad pull none
 *
 * @param  pinCfg: flash pin type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None(uint8_t pinCfg)
{
    if (pinCfg >= SF_IO_EXT_SF2_SWAP_IO3IO0) {
        PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(PDS_GPIO_GROUP_SET_GPIO0_GPIO15, 0, 0, 0);
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
BL_Err_Type ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None_Fast(uint8_t pinCfg)
{
    uint32_t tmpVal;

    if (pinCfg >= SF_IO_EXT_SF2_SWAP_IO3IO0) {
        /* pu/pd/ie config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_I_SET);
        /* GPIO4~9 [6][3][0] = 0 */
        tmpVal &= ~(0x49);
        BL_WR_REG(PDS_BASE, PDS_GPIO_I_SET, tmpVal);
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
    /* don't entry PDS */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_START_PS);
    BL_WR_REG(PDS_BASE, PDS_CTL, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL5);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_GPIO_KEEP_EN);
    BL_WR_REG(PDS_BASE, PDS_CTL5, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MCU0 clock enable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_MCU0_Clock_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_REG_MCU1_CLK_EN);
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MCU0 clock disable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_MCU0_Clock_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_MCU1_CLK_EN);
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MCU0 reset address
 *
 * @param  addr: address
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_MCU0_Reset_Address(uint32_t addr)
{
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG14, addr);

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  Enable PDS power on PLL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Pu_PLL_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_PU_RST_CLKPLL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    BL_WR_REG(PDS_BASE, PDS_PU_RST_CLKPLL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable PDS power on PLL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Pu_PLL_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_PU_RST_CLKPLL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    BL_WR_REG(PDS_BASE, PDS_PU_RST_CLKPLL, tmpVal);

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
    /* sleepForever set 1, chip will sleep forever */
    /* PDS sleep time 1~PDS_WARMUP_LATENCY_CNT <=> error */
    /* PDS sleep time >PDS_WARMUP_LATENCY_CNT <=> correct */
    if ((pdsSleepCnt) && (pdsSleepCnt <= PDS_WARMUP_LATENCY_CNT)) {
        return ERROR;
    } else if (pdsSleepCnt > PDS_WARMUP_LATENCY_CNT) {
        BL_WR_REG(PDS_BASE, PDS_TIME1, pdsSleepCnt - PDS_WARMUP_LATENCY_CNT);
    } else {
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
BL_Err_Type ATTR_TCM_SECTION PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5)
{
    /* PDS_CTL2 config */
    BL_WR_REG(PDS_BASE, PDS_CTL2, *(uint32_t *)cfg2);

    /* PDS_CTL3 config */
    BL_WR_REG(PDS_BASE, PDS_CTL3, *(uint32_t *)cfg3);

    /* PDS_CTL5 config */
    BL_WR_REG(PDS_BASE, PDS_CTL5, *(uint32_t *)cfg5);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep ram configure
 *
 * @param  ocramCfg: power down sleep force ocram configuration
 * @param  wramCfg: power down sleep force wram configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_RAM_Config(PDS_CTRL_RAM1_Type *ram1Cfg, PDS_CTRL_RAM2_Type *ram2Cfg,
                                            PDS_CTRL_RAM3_Type *ram3Cfg, PDS_CTRL_RAM4_Type *ram4Cfg)
{
    if ((NULL == ram1Cfg) || (NULL == ram2Cfg) || (NULL == ram3Cfg) || (NULL == ram4Cfg)) {
        return ERROR;
    }

    BL_WR_REG(PDS_BASE, PDS_RAM1, *(uint32_t *)ram1Cfg);
    BL_WR_REG(PDS_BASE, PDS_RAM2, *(uint32_t *)ram2Cfg);
    BL_WR_REG(PDS_BASE, PDS_RAM3, *(uint32_t *)ram3Cfg);
    BL_WR_REG(PDS_BASE, PDS_RAM4, *(uint32_t *)ram4Cfg);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep force configure
 *
 * @param  defaultLvCfg: power down sleep default level configuration
 * @param  ramCfg: ram configuration
 * @param  pdsSleepCnt: power down sleep time count
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt)
{
    PDS_Force_Config((PDS_CTL2_Type *)&(defaultLvCfg->pdsCtl2), (PDS_CTL3_Type *)&(defaultLvCfg->pdsCtl3), (PDS_CTL5_Type *)&(defaultLvCfg->pdsCtl5));
    PDS_Enable((PDS_CTL_Type *)&(defaultLvCfg->pdsCtl), (PDS_CTL4_Type *)&(defaultLvCfg->pdsCtl4), pdsSleepCnt);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep int mask
 *
 * @param  intType: PDS int type, this parameter can be one of the following values:
 *           @arg PDS_INT_WAKEUP
 *           @arg PDS_INT_RF_DONE
 *           @arg PDS_INT_WIFI_TBTT_SLEEP
 *           @arg PDS_INT_WIFI_TBTT_WAKEUP
 *           @arg PDS_INT_MAX
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_IntMask(uint8_t intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_PDS_INT_TYPE(intType));

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    if (intMask != UNMASK) {
        tmpVal = tmpVal | (1 << (intType + PDS_INT_MASK_BIT_OFFSET));
    } else {
        tmpVal = tmpVal & ~(1 << (intType + PDS_INT_MASK_BIT_OFFSET));
    }
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get power down sleep int status
 *
 * @param  intType: PDS int type, this parameter can be one of the following values:
 *           @arg PDS_INT_WAKEUP
 *           @arg PDS_INT_RF_DONE
 *           @arg PDS_INT_WIFI_TBTT_SLEEP
 *           @arg PDS_INT_WIFI_TBTT_WAKEUP
 *           @arg PDS_INT_MAX
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type PDS_Get_IntStatus(uint8_t intType)
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
 * @brief  Install PDS interrupt callback function
 *
 * @param  intType: PDS int type, this parameter can be one of the following values:
 *           @arg PDS_INT_WAKEUP
 *           @arg PDS_INT_RF_DONE
 *           @arg PDS_INT_WIFI_TBTT_SLEEP
 *           @arg PDS_INT_WIFI_TBTT_WAKEUP
 *           @arg PDS_INT_MAX
 * @param  cbFun: cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
BL_Err_Type PDS_Int_Callback_Install(uint8_t intType, intCallback_Type *cbFun)
{
#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(PDS_WAKEUP_IRQn, PDS_WAKEUP_IRQHandler);
#endif

    pdsIntCbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Trim RC32M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Trim_RC32M(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "rc32m", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 8)) {
            tmpVal = BL_RD_REG(PDS_BASE, PDS_RC32M_CTRL0);
            tmpVal = BL_SET_REG_BIT(tmpVal, PDS_RC32M_EXT_CODE_EN);
            BL_WR_REG(PDS_BASE, PDS_RC32M_CTRL0, tmpVal);
            arch_delay_us(2);
            tmpVal = BL_RD_REG(PDS_BASE, PDS_RC32M_CTRL2);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_RC32M_CODE_FR_EXT2, trim.value);
            BL_WR_REG(PDS_BASE, PDS_RC32M_CTRL2, tmpVal);
            tmpVal = BL_RD_REG(PDS_BASE, PDS_RC32M_CTRL2);
            tmpVal = BL_SET_REG_BIT(tmpVal, PDS_RC32M_EXT_CODE_SEL);
            BL_WR_REG(PDS_BASE, PDS_RC32M_CTRL2, tmpVal);
            /* hw_5T + sw_5T  */
            arch_delay_us(1);
            return SUCCESS;
        }
    }

    return ERROR;
}
/****************************************************************************/ /**
 * @brief  PDS Power Off PD_WB
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Power_Off_WB(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_FORCE_WB_ISO_EN);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_FORCE_WB_PWR_OFF);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Power On PD_WB
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Power_On_WB(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_FORCE_WB_PWR_OFF);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_FORCE_WB_ISO_EN);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS turn on USB
 *
 * @param  waitReady : wait turn on usb finish
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Turn_On_USB(uint8_t waitReady)
{
    uint32_t tmpVal = 0;

    /* USB_PHY_CTRL[3:2] reg_usb_phy_xtlsel=0                             */
    /* 2000e504 = 0x40; #100; USB_PHY_CTRL[6] reg_pu_usb20_psw=1 (VCC33A) */
    /* 2000e504 = 0x41; #500; USB_PHY_CTRL[0] reg_usb_phy_ponrst=1        */
    /* 2000e500 = 0x20; #100; USB_CTL[0] reg_usb_sw_rst_n=0               */
    /* 2000e500 = 0x22; #500; USB_CTL[1] reg_usb_ext_susp_n=1             */
    /* 2000e500 = 0x23; #100; USB_CTL[0] reg_usb_sw_rst_n=1               */
    /* #1.2ms; wait UCLK                                                  */
    /* wait(soc616_b0.usb_uclk);                                          */

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_PHY_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_USB_PHY_XTLSEL, 0);
    BL_WR_REG(PDS_BASE, PDS_USB_PHY_CTRL, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_PHY_CTRL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_REG_PU_USB20_PSW);
    BL_WR_REG(PDS_BASE, PDS_USB_PHY_CTRL, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_PHY_CTRL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_REG_USB_PHY_PONRST);
    BL_WR_REG(PDS_BASE, PDS_USB_PHY_CTRL, tmpVal);

    /* greater than 5T */
    arch_delay_us(1);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_CTL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_USB_SW_RST_N);
    BL_WR_REG(PDS_BASE, PDS_USB_CTL, tmpVal);

    /* greater than 5T */
    arch_delay_us(1);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_REG_USB_EXT_SUSP_N);
    BL_WR_REG(PDS_BASE, PDS_USB_CTL, tmpVal);

    /* wait UCLK 1.2ms */
    arch_delay_ms(3);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_REG_USB_SW_RST_N);
    BL_WR_REG(PDS_BASE, PDS_USB_CTL, tmpVal);

    if (waitReady) {
        arch_delay_ms(2);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS turn off USB
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Turn_Off_USB(void)
{
    uint32_t tmpVal = 0;

    /* USB_PHY_CTRL[3:2] reg_usb_phy_xtlsel=0                             */
    /* 2000e504 = 0x40; #100; USB_PHY_CTRL[6] reg_pu_usb20_psw=1 (VCC33A) */
    /* 2000e504 = 0x41; #500; USB_PHY_CTRL[0] reg_usb_phy_ponrst=1        */
    /* 2000e500 = 0x20; #100; USB_CTL[0] reg_usb_sw_rst_n=0               */
    /* 2000e500 = 0x22; #500; USB_CTL[1] reg_usb_ext_susp_n=1             */
    /* 2000e500 = 0x23; #100; USB_CTL[0] reg_usb_sw_rst_n=1               */
    /* #1.2ms; wait UCLK                                                  */
    /* wait(soc616_b0.usb_uclk);                                          */

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_PHY_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_USB_PHY_XTLSEL, 0);
    BL_WR_REG(PDS_BASE, PDS_USB_PHY_CTRL, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_PHY_CTRL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_PU_USB20_PSW);
    BL_WR_REG(PDS_BASE, PDS_USB_PHY_CTRL, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_PHY_CTRL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_USB_PHY_PONRST);
    BL_WR_REG(PDS_BASE, PDS_USB_PHY_CTRL, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_CTL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_USB_SW_RST_N);
    BL_WR_REG(PDS_BASE, PDS_USB_CTL, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_CTL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_USB_EXT_SUSP_N);
    BL_WR_REG(PDS_BASE, PDS_USB_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  USB phy suspend
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Set_USB_Suspend(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_CTL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_USB_EXT_SUSP_N);
    BL_WR_REG(PDS_BASE, PDS_USB_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  USB phy resume
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Set_USB_Resume(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_REG_USB_EXT_SUSP_N);
    BL_WR_REG(PDS_BASE, PDS_USB_CTL, tmpVal);

    return SUCCESS;
}

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
    for (uint8_t intType = PDS_INT_WAKEUP; intType < PDS_INT_MAX; intType++) {
        if (PDS_Get_IntStatus(intType) && (pdsIntCbfArra[intType] != NULL)) {
            pdsIntCbfArra[intType]();
        }
    }

    PDS_IntClear();
}
#endif

/*@} end of group PDS_Public_Functions */

/*@} end of group PDS */

/*@} end of group BL616_Peripheral_Driver */
