/**
  ******************************************************************************
  * @file    bl616l_pds.c
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

#include "bl616l.h"
#include "bl616l_pds.h"
#include "bflb_gpio.h"

/** @addtogroup  BL616L_Peripheral_Driver
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
 * @brief  set gpio pad int mask type in pds
 *
 * @param  pad: gpio type, this parameter can be GPIO_PIN_xx where xx is 6~36
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMask(uint8_t pad, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;
    uint32_t pos = 0;

    if (pad < GPIO_PIN_MAX) {
        pos = pad - 6;
    } else {
        return ERROR;
    }

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT_MSK_SET);
    if (UNMASK != intMask) {
        /* MASK */
        tmpVal |= (1 << pos);
    } else {
        /* UNMASK */
        tmpVal &= ~(1 << pos);
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT_MSK_SET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad trig type in pds
 *
 * @param  pad: gpio type, this parameter can be GPIO_PIN_xx where xx is 0~15,20~34
 *
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
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMode(uint8_t pad, uint8_t trig)
{
    uint32_t tmpVal = 0;
    uint32_t pos = 0;

    CHECK_PARAM(IS_PDS_GPIO_INT_TRIG_TYPE(trig));

    if (pad < GPIO_PIN_MAX) {
        pos = pad - 6;
    } else {
        return ERROR;
    }

    if (pad <= GPIO_PIN_13) {
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO13_6_INT_MODE);
        tmpVal &= ~(0xF << ((pos % 8) << 2));
        tmpVal |= (trig << ((pos % 8) << 2));
        BL_WR_REG(PDS_BASE, PDS_GPIO13_6_INT_MODE, tmpVal);

    } else if ((pad >= GPIO_PIN_14) && (pad <= GPIO_PIN_21)) {
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO21_14_INT_MODE);
        tmpVal &= ~(0xF << ((pos % 8) << 2));
        tmpVal |= (trig << ((pos % 8) << 2));
        BL_WR_REG(PDS_BASE, PDS_GPIO21_14_INT_MODE, tmpVal);

    } else if ((pad >= GPIO_PIN_22) && (pad <= GPIO_PIN_29)) {
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO29_22_INT_MODE);
        tmpVal &= ~(0xF << ((pos % 8) << 2));
        tmpVal |= (trig << ((pos % 8) << 2));
        BL_WR_REG(PDS_BASE, PDS_GPIO29_22_INT_MODE, tmpVal);

    } else if ((pad >= GPIO_PIN_30) && (pad <= GPIO_PIN_36)) {
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO36_30_INT_MODE);
        tmpVal &= ~(0xF << ((pos % 8) << 2));
        tmpVal |= (trig << ((pos % 8) << 2));
        BL_WR_REG(PDS_BASE, PDS_GPIO36_30_INT_MODE, tmpVal);

    } else {
        return ERROR;
    }

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  set gpio pad int clear in pds
 *
 * @param  pad: gpio
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntClr(uint8_t pad)
{
    uint32_t tmpVal = 0;
    uint32_t pos = 0;

    if (pad < GPIO_PIN_MAX) {
        pos = pad - 6;
    } else {
        return ERROR;
    }

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT_CLR);
    tmpVal |= (1 << pos);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT_CLR, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT_CLR);
    tmpVal &= ~(1 << pos);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT_CLR, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get gpio pad int status
 *
 * @param  pad: gpio type, this parameter can be GPIO_PIN_xx where xx is 0~15,20~34
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Sts_Type ATTR_TCM_SECTION PDS_Get_GPIO_Pad_IntStatus(uint8_t pad)
{
    uint32_t pos = 0;

    if (pad < GPIO_PIN_MAX) {
        pos = pad - 6;
    } else {
        return ERROR;
    }

    return (BL_RD_REG(PDS_BASE, PDS_GPIO_STAT) & (1 << pos)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  set all gpio pad int clear in pds
 *
 * @param  NULL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_GPIO_Pad_IntClr(void)
{
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT_CLR, 0x7FFFFFFF);

    __NOP();
    __NOP();
    __NOP();

    BL_WR_REG(PDS_BASE, PDS_GPIO_INT_CLR, 0);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get all gpio pad int status in pds
 *
 * @param  NULL
 *
 * @return pds_gpio_int_stat
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION PDS_Get_All_GPIO_Pad_IntStatus(void)
{
    return BL_RD_REG(PDS_BASE, PDS_GPIO_STAT);
}

/****************************************************************************/ /**
 * @brief  Disable PDS GPIO Keep
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Disable_ALL_GPIO_Keep(void)
{
    /* all PDS_IO latch disable */
    BL_WR_REG(PDS_BASE, PDS_GPIO_LAT_EN, 0);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable PDS GPIO Latch
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Enable_GPIO_Keep(uint32_t pin)
{
    uint32_t tmpVal = 0;
    uint32_t pos = 0;

    if (pin < GPIO_PIN_MAX) {
        if (pin > GPIO_PIN_5) {
            pos = pin - 6;
        } else {
            return ERROR;
        }
    } else {
        return ERROR;
    }

    /* PDS_IO latch enable */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_LAT_EN);
    tmpVal |= (1 << pos);
    BL_WR_REG(PDS_BASE, PDS_GPIO_LAT_EN, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select PDS GPIO int det clock source
 *
 * @param  clkSel: pds gpio clock type selection, this parameter can be one of the following values:
 *           @arg PDS_GPIO_INT_DET_CLK_F32K
 *           @arg PDS_GPIO_INT_DET_CLK_XCLK
 *           @arg PDS_GPIO_INT_DET_CLK_RC32M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_PDS_GPIO_INT_DET_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_PDS_GPIO_INT_DET_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_DET_CLK_SEL, clkSel);
    /* don't entry PDS */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_START_PS);
    BL_WR_REG(PDS_BASE, PDS_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable Directly Access wram bypass WB
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Enable_WRAM_Access_Direct(void)
{
    uint32_t tmpVal = 0;

    /* directly access wram enable */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_ACCESS_WRAM_PATH);
    /* don't entry PDS */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_START_PS);
    BL_WR_REG(PDS_BASE, PDS_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable Directly Access wram bypass WB
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Disable_WRAM_Access_Direct(void)
{
    uint32_t tmpVal = 0;

    /* directly access wram disable */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_ACCESS_WRAM_PATH);
    /* don't entry PDS */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_START_PS);
    BL_WR_REG(PDS_BASE, PDS_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS set LDO SOC delay when entering Lowpower Mode
 *
 * @param  cnt
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Set_Ldo_Soc_Enter_lp_delay(uint16_t cnt)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_LDO_SOC_LP_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_LDO_SOC_LP_EN_CNT, cnt);
    BL_WR_REG(PDS_BASE, PDS_LDO_SOC_LP_CTRL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS set LDO SOC delay when exiting Lowpower Mode
 *
 * @param  cnt
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Set_Ldo_Soc_Exit_lp_delay(uint16_t cnt)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_LDO_SOC_LP_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT, cnt);
    BL_WR_REG(PDS_BASE, PDS_LDO_SOC_LP_CTRL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS set LDO SYS delay when entering Lowpower Mode
 *
 * @param  cnt
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Set_Ldo_Sys_Enter_lp_delay(uint16_t cnt)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_LDO13_LP_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_LDO13_LP_EN_CNT, cnt);
    BL_WR_REG(PDS_BASE, PDS_LDO13_LP_CTRL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS set LDO SYS delay when exiting Lowpower Mode
 *
 * @param  cnt
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Set_Ldo_Sys_Exit_lp_delay(uint16_t cnt)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_LDO13_LP_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_LDO13_EXIT_LP_CNT, cnt);
    BL_WR_REG(PDS_BASE, PDS_LDO13_LP_CTRL, tmpVal);

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
BL_Err_Type ATTR_TCM_SECTION PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5, PDS_CTL6_Type *cfg6)
{
    /* PDS_CTL2 config */
    BL_WR_REG(PDS_BASE, PDS_CTL2, *(uint32_t *)cfg2);

    /* PDS_CTL3 config */
    BL_WR_REG(PDS_BASE, PDS_CTL3, *(uint32_t *)cfg3);

    /* PDS_CTL5 config */
    BL_WR_REG(PDS_BASE, PDS_CTL5, *(uint32_t *)cfg5);

    /* PDS_CTL6 config */
    BL_WR_REG(PDS_BASE, PDS_SLP_CONFIG, *(uint32_t *)cfg6);

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
 * @brief  PDS Set All WRAM(160KB) Retention
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_WRAM_Retention(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    /* retention signal must be 0 ahead, then enter pds state */
    /* fsm will first disassert CS signal and send clock(cr_pds_ctrl_ram_clk must be 1) to enter standby state */
    /* then pullup retention signal(cr_pds_misc_mem_stby must be 1) to enter retention state */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Set All OCRAM(320KB) Retention
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_OCRAM_Retention(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM4, tmpVal);

    /* retention signal must be 0 ahead, then enter pds state */
    /* fsm will first disassert CS signal and send clock(cr_pds_ctrl_ram_clk must be 1) to enter standby state */
    /* then pullup retention signal(cr_pds_misc_mem_stby must be 1) to enter retention state */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Set All WRAM(160KB) Sleep
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_WRAM_Sleep(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_SLP, 0x3FF);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Set All OCRAM(320KB) Sleep
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_OCRAM_Sleep(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM3, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_SLP, 0xFFFFF);
    BL_WR_REG(PDS_BASE, PDS_RAM4, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable PDS Control WRAM and OCRAM
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Disable_Ctrl_Ram(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM3, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM4, tmpVal);

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
    PDS_Force_Config((PDS_CTL2_Type *)&(defaultLvCfg->pdsCtl2), (PDS_CTL3_Type *)&(defaultLvCfg->pdsCtl3), (PDS_CTL5_Type *)&(defaultLvCfg->pdsCtl5), (PDS_CTL6_Type *)&(defaultLvCfg->pdsCtl6));
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
 * @brief  unmask or mask PDS wakeup source
 *
 * @param  WakeupType: wakeup pds source, this parameter can be one of the following values:
 *           @arg PDS_WAKEUP_BY_PDS_TIMER
 *           @arg PDS_WAKEUP_BY_HBN_IRQ_OUT
 *           @arg PDS_WAKEUP_BY_GPIO_IRQ
 *           @arg PDS_WAKEUP_BY_PDS_GPIO_INT
 *           @arg PDS_WAKEUP_BY_TOUCH_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USBH_WKP_DET
 *           @arg PDS_WAKEUP_BY_USBH_CONN_DET
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type PDS_Set_Wakeup_Src_IntMask(uint32_t WakeupType, BL_Mask_Type intMask)
{
    CHECK_PARAM(IS_PDS_WAKEUP_EVENT_TYPE(WakeupType));

    uint32_t tmpVal = 0;
    uint32_t pds_wakeup_src_en_bits = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);

    pds_wakeup_src_en_bits = BL_GET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN);

    if (intMask != UNMASK) {
        pds_wakeup_src_en_bits &= ~((uint32_t)1 << WakeupType);
    } else {
        pds_wakeup_src_en_bits |= ((uint32_t)1 << WakeupType);
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN, pds_wakeup_src_en_bits);

    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get wakeup source
 *
 * @param  WakeupType: wakeup pds source, this parameter can be one of the following values:
 *           @arg PDS_WAKEUP_BY_PDS_TIMER
 *           @arg PDS_WAKEUP_BY_HBN_IRQ_OUT
 *           @arg PDS_WAKEUP_BY_GPIO_IRQ
 *           @arg PDS_WAKEUP_BY_PDS_GPIO_INT
 *           @arg PDS_WAKEUP_BY_TOUCH_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USBH_WKP_DET
 *           @arg PDS_WAKEUP_BY_USBH_CONN_DET
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type PDS_Get_Wakeup_Src(uint32_t WakeupType)
{
    CHECK_PARAM(IS_PDS_WAKEUP_EVENT_TYPE(WakeupType));

    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_WAKEUP_EVENT);
    tmpVal &= ((uint32_t)1 << WakeupType);

    return tmpVal ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  Get wakeup source
 *
 * @return all Wakeup source state
 *
*******************************************************************************/
uint32_t PDS_Get_All_Wakeup_Src(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_WAKEUP_EVENT);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  Mask All Wakeup Source
 *
*******************************************************************************/
void PDS_Mask_All_Wakeup_Src(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_RF_DONE_INT_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);
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
 * @brief  Trim RC32M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Trim_USB20(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "usb20", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 7)) {
            tmpVal = BL_RD_REG(PDS_BASE, PDS_U2PHY_2);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_USB20_RCAL_COARSE, (trim.value >> 5) & 0x3);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_USB20_RCAL_CODE, trim.value & 0x1F);
            BL_WR_REG(PDS_BASE, PDS_U2PHY_2, tmpVal);
            return SUCCESS;
        }
    }

    return ERROR;
}


/*@} end of group PDS_Public_Functions */

/*@} end of group PDS */

/*@} end of group BL616L_Peripheral_Driver */
