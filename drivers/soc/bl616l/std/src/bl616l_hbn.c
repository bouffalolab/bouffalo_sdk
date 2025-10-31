/**
  ******************************************************************************
  * @file    bl616l_hbn.c
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

#include "bl616l_hbn.h"
#include "bl616l_glb.h"
#include "bl616l_ef_cfg.h"
#include "bflb_xip_sflash.h"

/** @addtogroup  BL616L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  HBN
 *  @{
 */

/** @defgroup  HBN_Private_Macros
 *  @{
 */
#define HBN_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }

/*@} end of group HBN_Private_Macros */

/** @defgroup  HBN_Private_Types
 *  @{
 */

/*@} end of group HBN_Private_Types */

/** @defgroup  HBN_Private_Variables
 *  @{
 */

/*@} end of group HBN_Private_Variables */

/** @defgroup  HBN_Global_Variables
 *  @{
 */

/*@} end of group HBN_Global_Variables */

/** @defgroup  HBN_Private_Fun_Declaration
 *  @{
 */

/*@} end of group HBN_Private_Fun_Declaration */

/** @defgroup  HBN_Private_Functions
 *  @{
 */

/*@} end of group HBN_Private_Functions */

/** @defgroup  HBN_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  HBN set LDO_AON voltage out and trim in HBN mode
 *
 * @param  ldo_level: LDO_AON volatge level, this parameter can be one of the following values:
 *          @arg HBN_LDO_AON_LEVEL_0P600V
 *          @arg HBN_LDO_AON_LEVEL_0P625V
 *          @arg HBN_LDO_AON_LEVEL_0P650V
 *          @arg HBN_LDO_AON_LEVEL_0P675V
 *          @arg HBN_LDO_AON_LEVEL_0P700V
 *          @arg HBN_LDO_AON_LEVEL_0P725V
 *          @arg HBN_LDO_AON_LEVEL_0P750V
 *          @arg HBN_LDO_AON_LEVEL_0P775V
 *          @arg HBN_LDO_AON_LEVEL_0P800V
 *          @arg HBN_LDO_AON_LEVEL_0P825V
 *          @arg HBN_LDO_AON_LEVEL_0P850V
 *          @arg HBN_LDO_AON_LEVEL_0P875V
 *          @arg HBN_LDO_AON_LEVEL_0P900V
 *          @arg HBN_LDO_AON_LEVEL_0P925V
 *          @arg HBN_LDO_AON_LEVEL_0P950V
 *          @arg HBN_LDO_AON_LEVEL_0P975V
 * @param  ldo_trim:LDO volatge trim value, this parameter can be one of the following values:
 *          @arg HBN_LDO_AON_TRIM_0P92
 *          @arg HBN_LDO_AON_TRIM_0P93
 *          @arg HBN_LDO_AON_TRIM_0P94
 *          @arg HBN_LDO_AON_TRIM_0P95
 *          @arg HBN_LDO_AON_TRIM_0P96
 *          @arg HBN_LDO_AON_TRIM_0P97
 *          @arg HBN_LDO_AON_TRIM_0P98
 *          @arg HBN_LDO_AON_TRIM_0P99
 *          @arg HBN_LDO_AON_TRIM_1P00
 *          @arg HBN_LDO_AON_TRIM_1P01
 *          @arg HBN_LDO_AON_TRIM_1P02
 *          @arg HBN_LDO_AON_TRIM_1P03
 *          @arg HBN_LDO_AON_TRIM_1P04
 *          @arg HBN_LDO_AON_TRIM_1P05
 *          @arg HBN_LDO_AON_TRIM_1P06
 *          @arg HBN_LDO_AON_TRIM_1P07
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Ctrl_Ldo_Aon_Vout_in_HBN(uint8_t ldo_level, uint8_t ldo_trim)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO08AON_VOUT_SEL_AON, ldo_level);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO08AON_VOUT_TRIM_AON, ldo_trim);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down and switch clock
 *
 * @param  hbn_level: hbn mode level, this parameter can be one of the following values:
 *          @arg HBN_LEVEL_0
 *          @arg HBN_LEVEL_1
 *          @arg HBN_LEVEL_2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Level_Select(uint8_t hbn_level)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);

    /* Set HBN level */
    switch (hbn_level) {
        case HBN_LEVEL_0:
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;
        case HBN_LEVEL_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;
        case HBN_LEVEL_2:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;
    }

    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWR_ON_OPTION);

    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /*  clear all hbn irq state*/
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0xffffffff);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0);

    /* Enter HBN */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  power down and switch clock
 *
 * @param  flashCfg: None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Power_Down_Flash(spi_flash_cfg_type *flashCfg)
{
    if (flashCfg == NULL) {
        return;
    } else {
        bflb_sf_ctrl_set_owner(SF_CTRL_OWNER_SAHB);
        bflb_sflash_reset_continue_read(flashCfg);
    }

    bflb_sflash_powerdown();
}

/****************************************************************************/ /**
 * @brief  Reset HBN mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Reset(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Reset HBN mode */
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_SW_RST);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_SW_RST);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_SW_RST);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get HBN por out state
 *
 * @param  None
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type HBN_Get_POR_OUT_State(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_BOR_CFG), HBN_POR_OUT) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  get HBN bod out state
 *
 * @param  None
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type HBN_Get_BOD_OUT_State(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_BOR_CFG), HBN_BOD_OUT) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  set HBN bod config
 *
 * @param  enable: ENABLE or DISABLE, if enable, Power up Brown Out Reset
 * @param  threshold: bod threshold, this parameter can be one of the following values:
 *           @arg HBN_BOD_THRES_2P33
 *           @arg HBN_BOD_THRES_2P37
 *           @arg HBN_BOD_THRES_2P42
 *           @arg HBN_BOD_THRES_2P46
 *           @arg HBN_BOD_THRES_2P51
 *           @arg HBN_BOD_THRES_2P56
 *           @arg HBN_BOD_THRES_2P61
 *           @arg HBN_BOD_THRES_2P67
 * @param  mode: bod work mode with por, this parameter can be one of the following values:
 *           @arg HBN_BOD_MODE_POR_INDEPENDENT
 *           @arg HBN_BOD_MODE_POR_RELEVANT
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_BOD_Config(uint8_t enable, uint8_t threshold, uint8_t mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_BOD_THRES_TYPE(threshold));
    CHECK_PARAM(IS_HBN_BOD_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_BOR_CFG);
    if (enable) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PU_BOD_AON, 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PU_BOD_AON, 0);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_TH_AON, threshold);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_SEL_AON, mode);
    BL_WR_REG(HBN_BASE, HBN_BOR_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set BOD Config
 *
 * @param  cfg:
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_BOD_Cfg(HBN_BOD_CFG_Type *cfg)
{
    uint32_t tmpVal = 0;

    if (cfg->enableBodInt) {
        HBN_Enable_BOD_IRQ();
    } else {
        HBN_Disable_BOD_IRQ();
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_BOR_CFG);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_TH_AON, cfg->bodThreshold);
    if (cfg->enablePorInBod) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_BOD_SEL_AON);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_BOD_SEL_AON);
    }

    if (cfg->enableBod) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_BOD_AON);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_BOD_AON);
    }

    BL_WR_REG(HBN_BASE, HBN_BOR_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN get reset event
 *
 * @param[out]  event
 * [5] : hbn watch dog reset
 * [4] : bor_out_ event
 * [3] : pwr_rst_n event
 * [2] : sw_rst event
 * [1] : por_out event
 * [0] : watch dog reset
 *
*******************************************************************************/
void HBN_Get_Reset_Event(uint8_t *event)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    *event = BL_GET_REG_BITS_VAL(tmpVal, HBN_RESET_EVENT);
}

/****************************************************************************/ /**
 * @brief  HBN clear reset event
 *
*******************************************************************************/
void HBN_Clr_Reset_Event(void)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_CLR_RESET_EVENT);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_CLR_RESET_EVENT);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
}

/****************************************************************************/ /**
 * @brief  HBN set LDO_AON voltage out
 *
 * @param  ldoLevel: LDO_AON volatge level, this parameter can be one of the following values:
 *           @arg HBN_LDO_AON_LEVEL_0P600V
 *           @arg HBN_LDO_AON_LEVEL_0P625V
 *           @arg HBN_LDO_AON_LEVEL_0P650V
 *           @arg HBN_LDO_AON_LEVEL_0P675V
 *           @arg HBN_LDO_AON_LEVEL_0P700V
 *           @arg HBN_LDO_AON_LEVEL_0P725V
 *           @arg HBN_LDO_AON_LEVEL_0P750V
 *           @arg HBN_LDO_AON_LEVEL_0P775V
 *           @arg HBN_LDO_AON_LEVEL_0P800V
 *           @arg HBN_LDO_AON_LEVEL_0P825V
 *           @arg HBN_LDO_AON_LEVEL_0P850V
 *           @arg HBN_LDO_AON_LEVEL_0P875V
 *           @arg HBN_LDO_AON_LEVEL_0P900V
 *           @arg HBN_LDO_AON_LEVEL_0P925V
 *           @arg HBN_LDO_AON_LEVEL_0P950V
 *           @arg HBN_LDO_AON_LEVEL_0P975V
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_SW_Set_Ldo_Aon_Vout(uint8_t ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_AON_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO08AON_VOUT_SEL_AON, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set LDO_AON voltage trim
 *
 * @param  trim:LDO_AON volatge trim value, this parameter can be one of the following values:
 *          @arg HBN_LDO_AON_TRIM_0P92
 *          @arg HBN_LDO_AON_TRIM_0P93
 *          @arg HBN_LDO_AON_TRIM_0P94
 *          @arg HBN_LDO_AON_TRIM_0P95
 *          @arg HBN_LDO_AON_TRIM_0P96
 *          @arg HBN_LDO_AON_TRIM_0P97
 *          @arg HBN_LDO_AON_TRIM_0P98
 *          @arg HBN_LDO_AON_TRIM_0P99
 *          @arg HBN_LDO_AON_TRIM_1P00
 *          @arg HBN_LDO_AON_TRIM_1P01
 *          @arg HBN_LDO_AON_TRIM_1P02
 *          @arg HBN_LDO_AON_TRIM_1P03
 *          @arg HBN_LDO_AON_TRIM_1P04
 *          @arg HBN_LDO_AON_TRIM_1P05
 *          @arg HBN_LDO_AON_TRIM_1P06
 *          @arg HBN_LDO_AON_TRIM_1P07
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_SW_Trim_Ldo_Aon_Vout(uint8_t trim)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_AON_TRIM_TYPE(trim));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO08AON_VOUT_TRIM_AON, trim);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN select RTC_CLK (PDS_TIMER & HBN_RTC)
 *
 * @param  clkType: HBN Slow_CLK clock type, this parameter can be one of the following values:
 *           @arg HBN_RTC_CLK_F32K
 *           @arg HBN_RTC_CLK_XTAL_DIVIED_32K
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_RTC_CLK_Sel(uint8_t clkType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_RTC_CLK_TYPE(clkType));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_TIME_H);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PDS_HBN_RTC_SLOW_CLK_SEL, clkType);
    BL_WR_REG(HBN_BASE, HBN_TIME_H, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN Get RTC_CLK TYPE (PDS_TIMER & HBN_RTC)
 *
 * @param  clkType: HBN RTC_CLK clock type, this parameter can be one of the following values:
 *           @arg HBN_RTC_CLK_F32K
 *           @arg HBN_RTC_CLK_XTAL_DIVIED_32K
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Get_RTC_CLK_Sel(uint8_t *clkType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_TIME_H);
    *clkType = BL_GET_REG_BITS_VAL(tmpVal, HBN_PDS_HBN_RTC_SLOW_CLK_SEL);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set XTAL Divied 32K clock
 *
 * @param  enable: Enable or disable
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_Xtal_Divied_CLK(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_TIME_H);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PDS_HBN_XTAL_DIV_EN, enable);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PDS_HBN_XTAL_DIV, div);
    BL_WR_REG(HBN_BASE, HBN_TIME_H, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN select 32K
 *
 * @param  clkType: HBN 32k clock type, this parameter can be one of the following values:
 *           @arg HBN_32K_RC
 *           @arg HBN_32K_XTAL
 *           @arg HBN_32K_DIG
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_32K_Sel(uint8_t clkType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_32K_CLK_TYPE(clkType));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_F32K_SEL, clkType);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select uart clock source
 *
 * @param  clkSel: uart clock type selection, this parameter can be one of the following values:
 *           @arg HBN_UART_CLK_MCU_BCLK
 *           @arg HBN_UART_CLK_MUXPLL_160M
 *           @arg HBN_UART_CLK_XCLK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_UART_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_UART_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    if ((HBN_UART_CLK_MCU_BCLK == clkSel) || (HBN_UART_CLK_MUXPLL_160M == clkSel)) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL2, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL, clkSel);
    } else {
        /* mcu_bclk as default */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL2, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL, 0);
    }

    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select gpio int det clock source
 *
 * @param  clkSel: gpio clock type selection, this parameter can be one of the following values:
 *           @arg HBN_GPIO_INT_DET_CLK_F32K
 *           @arg HBN_GPIO_INT_DET_CLK_XCLK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_GPIO_INT_DET_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_GPIO_INT_DET_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_INT_DET_CLK_SEL, clkSel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get xclk clock source Select
 *
 * @param  None
 *
 * @return xclk clock type selection
 *
*******************************************************************************/
uint8_t ATTR_CLOCK_SECTION HBN_Get_MCU_XCLK_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);

    return (uint8_t)(tmpVal & 0x1);
}

/****************************************************************************/ /**
 * @brief  Select xclk clock source
 *
 * @param  xclk: xclk clock type selection, this parameter can be one of the following values:
 *           @arg HBN_MCU_XCLK_RC32M
 *           @arg HBN_MCU_XCLK_XTAL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_MCU_XCLK_Sel(uint8_t xclk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_MCU_XCLK_TYPE(xclk));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);
    switch (xclk) {
        case HBN_MCU_XCLK_RC32M:
            tmpVal2 &= (~(1 << 0));
            break;
        case HBN_MCU_XCLK_XTAL:
            tmpVal2 |= (1 << 0);
            break;
        default:
            break;
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get root clock selection
 *
 * @param  None
 *
 * @return root clock selection, this return value can be one of the following values:
 *           @arg HBN_MCU_ROOT_CLK_XCLK
 *           @arg HBN_MCU_ROOT_CLK_PLL
 *
*******************************************************************************/
uint8_t ATTR_CLOCK_SECTION HBN_Get_MCU_Root_CLK_Sel(void)
{
    uint32_t tmpVal = 0;
    uint32_t rootClkSel = 0;

    /* root_clock_select = hbn_root_clk_sel[1] */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    rootClkSel = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL) >> 1;
    if (0 == rootClkSel) {
        return HBN_MCU_ROOT_CLK_XCLK;
    } else {
        return HBN_MCU_ROOT_CLK_PLL;
    }
}

/****************************************************************************/ /**
 * @brief  Select root clk source
 *
 * @param  rootClk: root clock type selection, this parameter can be one of the following values:
 *           @arg HBN_MCU_ROOT_CLK_XCLK
 *           @arg HBN_MCU_ROOT_CLK_PLL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_MCU_Root_CLK_Sel(uint8_t rootClk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_MCU_ROOT_CLK_TYPE(rootClk));

    /* root_clock_select = hbn_root_clk_sel[1] */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);
    switch (rootClk) {
        case HBN_MCU_ROOT_CLK_XCLK:
            tmpVal2 &= 0x1; //[1]=0
            break;
        case HBN_MCU_ROOT_CLK_PLL:
            tmpVal2 |= 0x2; //[1]=1
            break;
        default:
            break;
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set HBN_RAM sleep mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_HRAM_slp(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_SRAM);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RETRAM_SLP);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RETRAM_RET);
    BL_WR_REG(HBN_BASE, HBN_SRAM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set HBN_RAM retension mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_HRAM_Ret(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_SRAM);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RETRAM_SLP);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RETRAM_RET);
    BL_WR_REG(HBN_BASE, HBN_SRAM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set LDO soc voltage out
 *
 * @param  ldoLevel: LDO_SOC volatge level, this parameter can be one of the following values:
 *           @arg HBN_LDO_SOC_LEVEL_0P650V
 *           @arg HBN_LDO_SOC_LEVEL_0P700V
 *           @arg HBN_LDO_SOC_LEVEL_0P750V
 *           @arg HBN_LDO_SOC_LEVEL_0P775V
 *           @arg HBN_LDO_SOC_LEVEL_0P800V
 *           @arg HBN_LDO_SOC_LEVEL_0P825V
 *           @arg HBN_LDO_SOC_LEVEL_0P850V
 *           @arg HBN_LDO_SOC_LEVEL_0P875V
 *           @arg HBN_LDO_SOC_LEVEL_0P900V
 *           @arg HBN_LDO_SOC_LEVEL_0P925V
 *           @arg HBN_LDO_SOC_LEVEL_0P950V
 *           @arg HBN_LDO_SOC_LEVEL_1P000V
 *           @arg HBN_LDO_SOC_LEVEL_1P100V
 *           @arg HBN_LDO_SOC_LEVEL_1P300V
 *           @arg HBN_LDO_SOC_LEVEL_1P350V
 *           @arg HBN_LDO_SOC_LEVEL_1P400V
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_SW_Set_Ldo_Soc_Vout(uint8_t ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_SOC_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO_SOC_VOUT_SEL_AON, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_LDO_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN trim LDO SOC voltage
 *
 * @param  trim:LDO_SOC volatge trim value, this parameter can be one of the following values:
 *          @arg HBN_LDO_SOC_TRIM_0P93
 *          @arg HBN_LDO_SOC_TRIM_0P94
 *          @arg HBN_LDO_SOC_TRIM_0P95
 *          @arg HBN_LDO_SOC_TRIM_0P96
 *          @arg HBN_LDO_SOC_TRIM_0P97
 *          @arg HBN_LDO_SOC_TRIM_0P98
 *          @arg HBN_LDO_SOC_TRIM_0P99
 *          @arg HBN_LDO_SOC_TRIM_1P00
 *          @arg HBN_LDO_SOC_TRIM_1P01
 *          @arg HBN_LDO_SOC_TRIM_1P02
 *          @arg HBN_LDO_SOC_TRIM_1P03
 *          @arg HBN_LDO_SOC_TRIM_1P04
 *          @arg HBN_LDO_SOC_TRIM_1P05
 *          @arg HBN_LDO_SOC_TRIM_1P06
 *          @arg HBN_LDO_SOC_TRIM_1P07
 *          @arg HBN_LDO_SOC_TRIM_1P08
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_SW_Trim_Ldo_Soc_Vout(uint8_t trim)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_SOC_TRIM_TYPE(trim));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO_SOC_VOUT_TRIM_AON, trim);
    BL_WR_REG(HBN_BASE, HBN_LDO_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set LDO soc voltage out in lowpower mode
 *
 * @param  ldoLevel: LDO_SOC volatge level, this parameter can be one of the following values:
 *           @arg HBN_LDO_SOC_LEVEL_0P650V
 *           @arg HBN_LDO_SOC_LEVEL_0P700V
 *           @arg HBN_LDO_SOC_LEVEL_0P750V
 *           @arg HBN_LDO_SOC_LEVEL_0P775V
 *           @arg HBN_LDO_SOC_LEVEL_0P800V
 *           @arg HBN_LDO_SOC_LEVEL_0P825V
 *           @arg HBN_LDO_SOC_LEVEL_0P850V
 *           @arg HBN_LDO_SOC_LEVEL_0P875V
 *           @arg HBN_LDO_SOC_LEVEL_0P900V
 *           @arg HBN_LDO_SOC_LEVEL_0P925V
 *           @arg HBN_LDO_SOC_LEVEL_0P950V
 *           @arg HBN_LDO_SOC_LEVEL_1P000V
 *           @arg HBN_LDO_SOC_LEVEL_1P100V
 *           @arg HBN_LDO_SOC_LEVEL_1P300V
 *           @arg HBN_LDO_SOC_LEVEL_1P350V
 *           @arg HBN_LDO_SOC_LEVEL_1P400V
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo_Soc_Vout_in_Lowpower(uint8_t ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_SOC_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PDS_LDO_SOC_VOUT_SEL_AON, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_LDO_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN trim LDO SOC voltage in lowpower mode
 *
 * @param  ldo_trim:LDO_SOC volatge trim value, this parameter can be one of the following values:
 *          @arg HBN_LDO_SOC_TRIM_0P93
 *          @arg HBN_LDO_SOC_TRIM_0P94
 *          @arg HBN_LDO_SOC_TRIM_0P95
 *          @arg HBN_LDO_SOC_TRIM_0P96
 *          @arg HBN_LDO_SOC_TRIM_0P97
 *          @arg HBN_LDO_SOC_TRIM_0P98
 *          @arg HBN_LDO_SOC_TRIM_0P99
 *          @arg HBN_LDO_SOC_TRIM_1P00
 *          @arg HBN_LDO_SOC_TRIM_1P01
 *          @arg HBN_LDO_SOC_TRIM_1P02
 *          @arg HBN_LDO_SOC_TRIM_1P03
 *          @arg HBN_LDO_SOC_TRIM_1P04
 *          @arg HBN_LDO_SOC_TRIM_1P05
 *          @arg HBN_LDO_SOC_TRIM_1P06
 *          @arg HBN_LDO_SOC_TRIM_1P07
 *          @arg HBN_LDO_SOC_TRIM_1P08
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Trim_Ldo_Soc_Vout_in_Lowpower(uint8_t trim)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_SOC_TRIM_TYPE(trim));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PDS_LDO_SOC_VOUT_TRIM_AON, trim);
    BL_WR_REG(HBN_BASE, HBN_LDO_0, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION HBN_Ctrl_Ldo_Soc_Mode_by_HW(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_LDO_SOC_LP_EN_AON_CTRL_HW);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_LDO_SOC_LP_EN_AON_CTRL_HW);
    }

    BL_WR_REG(HBN_BASE, HBN_LDO_0, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo_Soc_Mode(uint8_t mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_SOC_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);

    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_LDO_SOC_LP_EN_AON_CTRL_HW);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO_SOC_LP_EN_AON, mode);

    BL_WR_REG(HBN_BASE, HBN_LDO_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set LDO SYS voltage
 *
 * @param  ldolevel:LDO_SYS volatge Vout value, this parameter can be one of the following values:
 *          @arg HBN_LDO_SYS_LEVEL_1P200
 *          @arg HBN_LDO_SYS_LEVEL_1P225
 *          @arg HBN_LDO_SYS_LEVEL_1P250
 *          @arg HBN_LDO_SYS_LEVEL_1P275
 *          @arg HBN_LDO_SYS_LEVEL_1P300
 *          @arg HBN_LDO_SYS_LEVEL_1P325
 *          @arg HBN_LDO_SYS_LEVEL_1P350
 *          @arg HBN_LDO_SYS_LEVEL_1P400
 *          @arg HBN_LDO_SYS_LEVEL_1P450
 *          @arg HBN_LDO_SYS_LEVEL_1P500
 *          @arg HBN_LDO_SYS_LEVEL_1P550
 *          @arg HBN_LDO_SYS_LEVEL_1P750
 *          @arg HBN_LDO_SYS_LEVEL_1P800
 *          @arg HBN_LDO_SYS_LEVEL_1P850
 *          @arg HBN_LDO_SYS_LEVEL_1P900
 *          @arg HBN_LDO_SYS_LEVEL_2P000
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_SW_Set_Ldo_Sys_Vout(uint8_t ldolevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_SYS_LEVEL_TYPE(ldolevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO13SYS_VOUT_SEL_AON, ldolevel);
    BL_WR_REG(HBN_BASE, HBN_LDO_1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN trim LDO SYS voltage in lowpower mode
 *
 * @param  trim:LDO_SYS voltage trim value, this parameter can be one of the following values:
 *          @arg HBN_LDO_SYS_TRIM_0P93
 *          @arg HBN_LDO_SYS_TRIM_0P94
 *          @arg HBN_LDO_SYS_TRIM_0P95
 *          @arg HBN_LDO_SYS_TRIM_0P96
 *          @arg HBN_LDO_SYS_TRIM_0P97
 *          @arg HBN_LDO_SYS_TRIM_0P98
 *          @arg HBN_LDO_SYS_TRIM_0P99
 *          @arg HBN_LDO_SYS_TRIM_1P00
 *          @arg HBN_LDO_SYS_TRIM_1P01
 *          @arg HBN_LDO_SYS_TRIM_1P02
 *          @arg HBN_LDO_SYS_TRIM_1P03
 *          @arg HBN_LDO_SYS_TRIM_1P04
 *          @arg HBN_LDO_SYS_TRIM_1P05
 *          @arg HBN_LDO_SYS_TRIM_1P06
 *          @arg HBN_LDO_SYS_TRIM_1P07
 *          @arg HBN_LDO_SYS_TRIM_1P08
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_SW_Trim_Ldo_Sys_Vout(uint8_t trim)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_HBN_LDO_SYS_TRIM_TYPE(trim));

    tmpVal = BL_RD_REG(AON_BASE, HBN_LDO_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO13SYS_VOUT_TRIM_AON, trim);
    BL_WR_REG(AON_BASE, HBN_LDO_1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set LDO SYS voltage in lowpower mode
 *
 * @param  ldolevel:LDO_SYS volatge Vout value, this parameter can be one of the following values:
 *          @arg HBN_LDO_SYS_LEVEL_1P200
 *          @arg HBN_LDO_SYS_LEVEL_1P225
 *          @arg HBN_LDO_SYS_LEVEL_1P250
 *          @arg HBN_LDO_SYS_LEVEL_1P275
 *          @arg HBN_LDO_SYS_LEVEL_1P300
 *          @arg HBN_LDO_SYS_LEVEL_1P325
 *          @arg HBN_LDO_SYS_LEVEL_1P350
 *          @arg HBN_LDO_SYS_LEVEL_1P400
 *          @arg HBN_LDO_SYS_LEVEL_1P450
 *          @arg HBN_LDO_SYS_LEVEL_1P500
 *          @arg HBN_LDO_SYS_LEVEL_1P550
 *          @arg HBN_LDO_SYS_LEVEL_1P750
 *          @arg HBN_LDO_SYS_LEVEL_1P800
 *          @arg HBN_LDO_SYS_LEVEL_1P850
 *          @arg HBN_LDO_SYS_LEVEL_1P900
 *          @arg HBN_LDO_SYS_LEVEL_2P000
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo_Sys_Vout_in_Lowpower(uint8_t ldolevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_SYS_LEVEL_TYPE(ldolevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PDS_LDO13SYS_VOUT_SEL_AON, ldolevel);
    BL_WR_REG(HBN_BASE, HBN_LDO_1, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION HBN_Ctrl_Ldo_Sys_Mode_by_HW(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_1);
    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_LDO13SYS_LP_EN_AON_CTRL_HW);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_LDO13SYS_LP_EN_AON_CTRL_HW);
    }

    BL_WR_REG(HBN_BASE, HBN_LDO_1, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo_Sys_Mode(uint8_t mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_SYS_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_1);

    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_LDO13SYS_LP_EN_AON_CTRL_HW);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO13SYS_LP_EN_AON, mode);

    BL_WR_REG(HBN_BASE, HBN_LDO_1, tmpVal);

    return SUCCESS;
}

/******************************************************************************
 * @brief  HBN Power Off LDO18_IO
 *
 * @return SUCCESS or ERROR
 *
 *******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Power_Off_LDO18_IO(void)
{
    uint32_t tmpVal = 0;

    /* set pu_ldo18io_aon = 0 */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_LDO18IO_AON);
    BL_WR_REG(HBN_BASE, HBN_LDO_2, tmpVal);

    return SUCCESS;
}

/******************************************************************************
 * @brief  HBN Power On LDO18_IO
 *
 * @return SUCCESS or ERROR
 *
 *******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Power_On_LDO18_IO(void)
{
    uint32_t tmpVal = 0;

    /* set pu_ldo18io_aon = 1 */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_2);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_LDO18IO_AON);
    BL_WR_REG(HBN_BASE, HBN_LDO_2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set xtal32k's inverter amplify strength
 *
 * @param  value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_Xtal_32K_Inverter_Amplify_Strength(uint8_t value)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_XTAL32K_INV_STRE_AON, value);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set xtal32k_regulator
 *
 * @param  level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_Xtal_32K_Regulator(uint8_t level)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_XTAL32K_REG_AON, level);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Start XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 * @note can't use GPIO4&5 after calling this function
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Start_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);

    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_XTAL32K_HIZ_EN_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_XTAL32K_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_XTAL32K_EN_VCLAMP_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_XTAL32K_RDY_RSTB_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_XTAL32K_FAST_STARTUP_AON);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    /* Delay >1us */
    arch_delay_us(2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    /* Delay >1us */
    arch_delay_us(2);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Clear Xtal_32K_Fast_Startup
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 * @note  500uS after starting XTAL32K, call this function
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Clear_Xtal_32K_Fast_Startup(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_XTAL32K_FAST_STARTUP_AON);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Xtal 32K Ready State
 *
 * @param  None
 *
 * @return SET or RESET
 * @note  At least 100ms after starting XTAL32K, Xtal 32K Ready State changes from RESET to SET
 *
*******************************************************************************/
BL_Sts_Type ATTR_CLOCK_SECTION HBN_Get_Xtal_32K_Ready_State(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    return BL_IS_REG_BIT_SET(tmpVal, HBN_XTAL32K_CLK_RDY);
}

/****************************************************************************/ /**
 * @brief  Power on XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 * @note can't use GPIO4&5 after calling this function
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_On_Xtal_32K(void)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    HBN_Start_Xtal_32K();

    arch_delay_us(500);

    HBN_Clear_Xtal_32K_Fast_Startup();

    /* Polling for ready */
    do {
        arch_delay_us(10);
        timeOut++;
        tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    } while (!BL_IS_REG_BIT_SET(tmpVal, HBN_XTAL32K_CLK_RDY) && timeOut < 120000);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 * @note can use GPIO4&5 after calling this function
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_Off_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_XTAL32K_HIZ_EN_AON);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_XTAL32K_AON);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_XTAL32K_EN_VCLAMP_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    /* Delay >1us */
    arch_delay_us(2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    /* Delay >1us */
    arch_delay_us(2);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Keep RC32K On during rtc power domain off
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 * @note  Don't turn off rc32k, which move to RTC Domain
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Keep_On_RC32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_RC32K);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off RC3K during rtc power domain off
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_Off_RC32K(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_RC32K);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Trim RC32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Trim_RC32K(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "rc32k_cap", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RC32K_CAP_SEL_AON, trim.value);
            BL_WR_REG(HBN_BASE, HBN_RC32K_0, tmpVal);
            arch_delay_us(2);
            tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_1);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RC32K_EXT_CODE_EN);
            BL_WR_REG(HBN_BASE, HBN_RC32K_1, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim Ldo_SOC According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Trim_Ldo_Soc_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldosoc_trim", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO_SOC_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(HBN_BASE, HBN_LDO_0, tmpVal);
            tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PDS_LDO_SOC_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(HBN_BASE, HBN_LDO_0, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim Ldo_Aon According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Trim_Ldo_Aon_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo08_trim", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO08AON_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
            tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO08AON_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim Ldo_Sys According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Trim_Ldo_Sys_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo13_trim", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, HBN_LDO_1);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO13SYS_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, HBN_LDO_1, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set RC32M speed
 *
 * @param  rc32m_speed rc32m speed type selection, this parameter can be one of the following values:
 *           @arg HBN_RCM_8M
 *           @arg HBN_RCM_16M
 *           @arg HBN_RCM_32M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_RC32M_Speed(uint8_t rc32m_speed)
{
    int32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32M_CTRL0);

    switch (rc32m_speed) {
        case HBN_RCM_8M:
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RCHF_16M_SEL);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RCHF_CLKX2_EN);
            break;
        case HBN_RCM_16M:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RCHF_16M_SEL);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RCHF_CLKX2_EN);
            break;
        case HBN_RCM_32M:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RCHF_16M_SEL);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RCHF_CLKX2_EN);
            /* For bootrom case, must clear code sel */
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RC32M_EXT_CODE_SEL);
            break;
        default:
            break;
    }

    BL_WR_REG(HBN_BASE, HBN_RC32M_CTRL0, tmpVal);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get RC32M speed
 *
 * @param  void
 *
 * @return rc32m speed type,HBN_RCM_8M,HBN_RCM_16M,HBN_RCM_32M
 *
*******************************************************************************/
uint8_t ATTR_CLOCK_SECTION HBN_Get_RC32M_Speed(void)
{
    int32_t tmpVal = 0;
    uint32_t sel_16m = 0;
    uint32_t clk2x_en = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32M_CTRL0);

    sel_16m = BL_GET_REG_BITS_VAL(tmpVal, HBN_RCHF_16M_SEL);
    clk2x_en = BL_GET_REG_BITS_VAL(tmpVal, HBN_RCHF_CLKX2_EN);

    if (sel_16m == 0) {
        if (clk2x_en == 0) {
            return HBN_RCM_8M;
        } else {
            return HBN_RCM_16M;
        }
    } else {
        if (clk2x_en == 0) {
            return HBN_RCM_16M;

        } else {
            return HBN_RCM_32M;
        }
    }
}

/****************************************************************************/ /**
 * @brief  Trim RC32M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Trim_RC32M(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "rc32m", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 8)) {
            tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32M_CTRL0);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RC32M_EXT_CODE_EN);
            BL_WR_REG(HBN_BASE, HBN_RC32M_CTRL0, tmpVal);
            arch_delay_us(2);
            tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32M_CTRL0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RC32M_CODE_FR_EXT2, trim.value);
            BL_WR_REG(HBN_BASE, HBN_RC32M_CTRL0, tmpVal);
            tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32M_CTRL0);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RC32M_EXT_CODE_SEL);
            BL_WR_REG(HBN_BASE, HBN_RC32M_CTRL0, tmpVal);
            /* hw_5T + sw_5T  */
            arch_delay_us(1);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Get HBN status flag
 *
 * @param  None
 *
 * @return HBN status flag value
 *
*******************************************************************************/
uint32_t HBN_Get_Status_Flag(void)
{
    return BL_RD_REG(HBN_BASE, HBN_RSV0);
}

/****************************************************************************/ /**
 * @brief  Set HBN status flag
 *
 * @param  flag: Status Flag
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_Status_Flag(uint32_t flag)
{
    BL_WR_REG(HBN_BASE, HBN_RSV0, flag);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get HBN wakeup address
 *
 * @param  None
 *
 * @return HBN wakeup address
 *
*******************************************************************************/
uint32_t HBN_Get_Wakeup_Addr(void)
{
    return BL_RD_REG(HBN_BASE, HBN_RSV1);
}

/****************************************************************************/ /**
 * @brief  Set HBN wakeup address
 *
 * @param  addr: HBN wakeup address
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_Wakeup_Addr(uint32_t addr)
{
    BL_WR_REG(HBN_BASE, HBN_RSV1, addr);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get core reboot flag
 *
 * @param  None
 *
 * @return repower flag
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION HBN_Get_Core_Unhalt_Config(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV2);
    if (HBN_RELEASE_CORE_FLAG == BL_GET_REG_BITS_VAL(tmpVal, HBN_RELEASE_CORE)) {
        return BL_GET_REG_BITS_VAL(tmpVal, HBN_CORE_UNHALT);
    }

    return 0x00;
}

/****************************************************************************/ /**
 * @brief  Set core reboot flag
 *
 * @param  core: core ID
 * @param  hcfg: reboot cfg
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Core_Reboot_Config(uint8_t core, uint8_t hcfg)
{
    uint32_t tmpVal = 0;
    uint8_t unhalt = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RELEASE_CORE, HBN_RELEASE_CORE_FLAG);
    unhalt = BL_GET_REG_BITS_VAL(tmpVal, HBN_CORE_UNHALT);
    if (hcfg) {
        unhalt |= (1 << core);
    } else {
        unhalt &= ~(1 << core);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_CORE_UNHALT, unhalt);
    BL_WR_REG(HBN_BASE, HBN_RSV2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get user boot config
 *
 * @param  None
 *
 * @return user boot config
 *
*******************************************************************************/
uint8_t HBN_Get_User_Boot_Config(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV2);
    if (HBN_RELEASE_CORE_FLAG == BL_GET_REG_BITS_VAL(tmpVal, HBN_RELEASE_CORE)) {
        return BL_GET_REG_BITS_VAL(tmpVal, HBN_USER_BOOT_SEL);
    }

    return 0x00;
}

/****************************************************************************/ /**
 * @brief  Set user boot config
 *
 * @param  ubCfg: user boot config
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_User_Boot_Config(uint8_t ubCfg)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RELEASE_CORE, HBN_RELEASE_CORE_FLAG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_USER_BOOT_SEL, ubCfg);
    BL_WR_REG(HBN_BASE, HBN_RSV2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get ldo18io power delay config
 *
 * @param  pwrOffDly: power off delay
 * @param  pwrOnDly: power on delay
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Get_Ldo18io_Power_Delay_Config(uint16_t *pwrOffDly, uint16_t *pwrOnDly)
{
    uint32_t tmpVal = 0;

    if ((NULL == pwrOffDly) || (NULL == pwrOnDly)) {
        return ERROR;
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV2);
    if (HBN_LDO18IO_POWER_DLY_FLAG == BL_GET_REG_BITS_VAL(tmpVal, HBN_LDO18IO_POWER_DLY_STS)) {
        *pwrOffDly = BL_GET_REG_BITS_VAL(tmpVal, HBN_LDO18IO_POWER_OFF_DLY);
        *pwrOnDly = BL_GET_REG_BITS_VAL(tmpVal, HBN_LDO18IO_POWER_ON_DLY);
        return SUCCESS;
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Set ldo18io power delay config
 *
 * @param  pwrOffDly: power off delay
 * @param  pwrOnDly: power on delay
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo18io_Power_Delay_Config(uint16_t pwrOffDly, uint16_t pwrOnDly)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO18IO_POWER_DLY_STS, HBN_LDO18IO_POWER_DLY_FLAG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO18IO_POWER_OFF_DLY, pwrOffDly);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO18IO_POWER_ON_DLY, pwrOnDly);
    BL_WR_REG(HBN_BASE, HBN_RSV2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set Xtal type
 *
 * @param  xtalType:Xtal type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Xtal_Type(uint8_t xtalType)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_XTAL_STS, HBN_XTAL_FLAG_VALUE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_XTAL_TYPE, xtalType);
    BL_WR_REG(HBN_BASE, HBN_RSV3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Xtal type
 *
 * @param  xtalType:Xtal type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Get_Xtal_Type(uint8_t *xtalType)
{
    uint32_t tmpVal = 0;

    if (NULL == xtalType) {
        return ERROR;
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    if (HBN_XTAL_FLAG_VALUE == BL_GET_REG_BITS_VAL(tmpVal, HBN_XTAL_STS)) {
        *xtalType = BL_GET_REG_BITS_VAL(tmpVal, HBN_XTAL_TYPE);
        return SUCCESS;
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Get Xtal value
 *
 * @param  xtalVal:Xtal value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Get_Xtal_Value(uint32_t *xtalVal)
{
    uint32_t tmpVal = 0;
    uint8_t xtalType = 0;

    if (NULL == xtalVal) {
        return ERROR;
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    if (HBN_XTAL_FLAG_VALUE == BL_GET_REG_BITS_VAL(tmpVal, HBN_XTAL_STS)) {
        xtalType = BL_GET_REG_BITS_VAL(tmpVal, HBN_XTAL_TYPE);
        switch (xtalType) {
            case GLB_XTAL_NONE:
                *xtalVal = 0;
                break;
            case GLB_XTAL_24M:
                *xtalVal = 24000000;
                break;
            case GLB_XTAL_32M:
                *xtalVal = 32000000;
                break;
            case GLB_XTAL_38P4M:
                *xtalVal = 38400000;
                break;
            case GLB_XTAL_40M:
                *xtalVal = 40000000;
                break;
            case GLB_XTAL_26M:
                *xtalVal = 26000000;
                break;
            case GLB_XTAL_RC32M:
                *xtalVal = 32000000;
                break;
            default:
                *xtalVal = 0;
                break;
        }
        return SUCCESS;
    }

    *xtalVal = 0;
    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Set Flash Power Delay
 *
 * @param  flashPwrDly:flash power delay
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Flash_Power_Delay(uint8_t flashPwrDly)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_POWER_STS, HBN_FLASH_POWER_DLY_FLAG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_POWER_DLY, flashPwrDly);
    BL_WR_REG(HBN_BASE, HBN_RSV3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Flash Power Delay
 *
 * @param  flashPwrDly:flash power delay
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Get_Flash_Power_Delay(uint8_t *flashPwrDly)
{
    uint32_t tmpVal = 0;

    if (NULL == flashPwrDly) {
        return ERROR;
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    if (HBN_FLASH_POWER_DLY_FLAG == BL_GET_REG_BITS_VAL(tmpVal, HBN_FLASH_POWER_STS)) {
        *flashPwrDly = BL_GET_REG_BITS_VAL(tmpVal, HBN_FLASH_POWER_DLY);
        return SUCCESS;
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  HBN clear RTC timer counter
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Clear_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Clear RTC control bit0 */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal & 0xfffffffe);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN clear RTC timer counter
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Enable_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Set RTC control bit0 */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal | 0x01);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set RTC timer configuration
 *
 * @param  delay: RTC interrupt delay 32 clocks, this parameter can be one of the following values:
 *           @arg HBN_RTC_INT_DELAY_32T
 *           @arg HBN_RTC_INT_DELAY_0T
 * @param  compValLow: RTC interrupt commpare value low 32 bits
 * @param  compValHigh: RTC interrupt commpare value high 32 bits
 * @param  compMode: RTC interrupt commpare
 *                   mode:HBN_RTC_COMP_BIT0_47,HBN_RTC_COMP_BIT0_23,HBN_RTC_COMP_BIT13_47
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_RTC_Timer(uint8_t delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_RTC_INT_DELAY_TYPE(delay));

    BL_WR_REG(HBN_BASE, HBN_TIME_L, compValLow);
    BL_WR_REG(HBN_BASE, HBN_TIME_H, compValHigh & 0xffff);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Set interrupt delay option */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RTC_DLY_OPTION, delay);
    /* Set RTC compare mode */
    tmpVal |= (compMode << 1);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN get RTC timer count value
 *
 * @param  valLow: RTC count value pointer for low 32 bits
 * @param  valHigh: RTC count value pointer for high 16 bits
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh)
{
    uint32_t tmpVal;

    /* Tigger RTC val read */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
    BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
    BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);

    /* Read RTC val */
    *valLow = BL_RD_REG(HBN_BASE, HBN_RTC_TIME_L);
    *valHigh = (BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H) & 0xffff);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN clear RTC timer interrupt,this function must be called to clear delayed rtc IRQ
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_RTC_INT(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Clear RTC commpare:bit1-3 for clearing Delayed RTC IRQ */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal & 0xfffffff1);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable GPIO interrupt
 *
 * @param  gpioIntTrigType: HBN GPIO interrupt trigger type, this parameter can be one of the following values:
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_GPIO_INT_Enable(uint8_t gpioIntTrigType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_GPIO_INT_TRIGGER_TYPE(gpioIntTrigType));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MODE, gpioIntTrigType);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable GPIO interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_GPIO_INT_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, 0);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN get interrupt status
 *
 * @param  irqType: HBN interrupt type, this parameter can be one of the following values:
 *           @arg HBN_INT_GPIO0
 *           @arg HBN_INT_GPIO1
 *           @arg HBN_INT_GPIO2
 *           @arg HBN_INT_GPIO3
 *           @arg HBN_INT_GPIO4
 *           @arg HBN_INT_GPIO5
 *           @arg HBN_INT_RTC
 *           @arg HBN_INT_PIR
 *           @arg HBN_INT_BOD
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type HBN_Get_INT_State(uint8_t irqType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_INT_TYPE(irqType));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_STAT);

    if (tmpVal & (1 << irqType)) {
        return SET;
    } else {
        return RESET;
    }
}

/****************************************************************************/ /**
 * @brief  HBN get pin wakeup mode value
 *
 * @param  None
 *
 * @return HBN pin wakeup mode value
 *
*******************************************************************************/
uint8_t HBN_Get_Pin_Wakeup_Mode(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_IRQ_MODE), HBN_PIN_WAKEUP_MODE);
}

/****************************************************************************/ /**
 * @brief  HBN clear interrupt status
 *
 * @param  irqType: HBN interrupt type, this parameter can be one of the following values:
 *           @arg HBN_INT_GPIO0
 *           @arg HBN_INT_GPIO1
 *           @arg HBN_INT_GPIO2
 *           @arg HBN_INT_GPIO3
 *           @arg HBN_INT_GPIO4
 *           @arg HBN_INT_GPIO5
 *           @arg HBN_INT_RTC
 *           @arg HBN_INT_PIR
 *           @arg HBN_INT_BOD
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_IRQ(uint8_t irqType)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_INT_TYPE(irqType));

    /* set clear bit */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
    tmpVal |= (1 << irqType);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

    /* unset clear bit */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
    tmpVal &= (~(1 << irqType));
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN hardware pullup or pulldown configuration
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Hw_Pu_Pd_Cfg(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    }
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set AON_IO interrupt Trigger Mode
 *
 * @param  gpioIntTrigType: HBN GPIO interrupt trigger type, this parameter can be one of the following values:
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Aon_Pad_IntMode(uint8_t gpioIntTrigType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_GPIO_INT_TRIGGER_TYPE(gpioIntTrigType));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MODE, gpioIntTrigType);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN wakeup pin mask configuration
 *
 * @param  maskVal: mask value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Pin_WakeUp_Mask(uint8_t maskVal)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad int mask type in hbn
 *
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~5
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Aon_Pad_IntMask(uint8_t pad, BL_Mask_Type intMask)
{
    uint32_t tmpVal;
    uint32_t maskVal;
    uint8_t aonPadID;

    if ((pad >= GPIO_PIN_0) && (pad <= GPIO_PIN_5)) {
        aonPadID = pad - GPIO_PIN_0;
    } else {
        return ERROR;
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    maskVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK);

    if (intMask == UNMASK) {
        maskVal = maskVal & ~(1 << aonPadID);
    } else {
        maskVal = maskVal | (1 << aonPadID);
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN aon pad ctrl configuration
 *
 * @param  aonPadCtl1: ctrl value 1
 * @param  aonpadCtl2: ctrl value 2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Aon_Pad_Ctrl(uint32_t aonPadCtl1, uint32_t aonPadCtl2)
{
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, aonPadCtl1);
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_1, aonPadCtl2);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN aon pad cfg configuration
 *
 * @param  aonGpio: aon pad number, this parameter can be one of the following values:
 *           @arg HBN_AON_PAD_GPIO0
 *           @arg HBN_AON_PAD_GPIO1
 *           @arg HBN_AON_PAD_GPIO2
 *           @arg HBN_AON_PAD_GPIO3
 *           @arg HBN_AON_PAD_GPIO4
 *           @arg HBN_AON_PAD_GPIO5
 *
 * @param  aonPadCfg: aon pad configuration
 *
 * @return SUCCESS or ERROR
 *
 * @note   Pu and Pd not depend on IE. IE ,Pu and Pd depend on ctrlEn
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Aon_Pad_Cfg(uint8_t aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg)
{
    uint32_t tmpVal0;
    uint32_t tmpVal1;
    uint32_t enAonCtrlGpio;
    uint32_t aonPadIeSmt;
    uint32_t aonPadPu;
    uint32_t aonPadPd;
    uint32_t aonPadOe;
    uint32_t aonledsel;

    CHECK_PARAM(IS_HBN_AON_PAD_TYPE(aonGpio));

    if (NULL == aonPadCfg) {
        return ERROR;
    }

    /* hbn_pad_ctrl_0 */
    tmpVal0 = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_0);
    enAonCtrlGpio = BL_GET_REG_BITS_VAL(tmpVal0, HBN_REG_EN_AON_CTRL_GPIO);
    if (aonPadCfg->ctrlEn) {
        enAonCtrlGpio |= (1 << aonGpio);
    } else {
        enAonCtrlGpio &= ~(1 << aonGpio);
    }
    tmpVal0 = BL_SET_REG_BITS_VAL(tmpVal0, HBN_REG_EN_AON_CTRL_GPIO, enAonCtrlGpio);
    aonPadIeSmt = BL_GET_REG_BITS_VAL(tmpVal0, HBN_REG_AON_PAD_IE_SMT);
    if (aonPadCfg->ie) {
        aonPadIeSmt |= (1 << aonGpio);
    } else {
        aonPadIeSmt &= ~(1 << aonGpio);
    }
    tmpVal0 = BL_SET_REG_BITS_VAL(tmpVal0, HBN_REG_AON_PAD_IE_SMT, aonPadIeSmt);
    aonledsel = BL_GET_REG_BITS_VAL(tmpVal0, HBN_REG_AON_LED_SEL);
    if (aonPadCfg->outValue) {
        aonledsel |= (0x7 << (aonGpio%3*3));
    } else {
        aonledsel &= ~(0x7 << (aonGpio%3*3));
    }
    tmpVal0 = BL_SET_REG_BITS_VAL(tmpVal0, HBN_REG_AON_LED_SEL, aonledsel);
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, tmpVal0);

    /* hbn_pad_ctrl_1 */
    tmpVal1 = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_1);
    aonPadPu = BL_GET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_PAD_PU);
    if (aonPadCfg->pullUp) {
        aonPadPu |= (1 << aonGpio);
    } else {
        aonPadPu &= ~(1 << aonGpio);
    }
    tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_PAD_PU, aonPadPu);
    aonPadPd = BL_GET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_PAD_PD);
    if (aonPadCfg->pullDown) {
        aonPadPd |= (1 << aonGpio);
    } else {
        aonPadPd &= ~(1 << aonGpio);
    }
    tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_PAD_PD, aonPadPd);
    aonPadOe = BL_GET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_PAD_OE);
    if (aonPadCfg->oe) {
        aonPadOe |= (1 << aonGpio);
    } else {
        aonPadOe &= ~(1 << aonGpio);
    }
    tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_PAD_OE, aonPadOe);
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_1, tmpVal1);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN Set AON_HW control aon pad or not
 *
 * @param  aonPadHwCtrlEn: hw control aon pad enable
 * @param  aonGpio: aon pad number, this parameter can be one of the following values:
 *           @arg HBN_AON_PAD_GPIO0
 *           @arg HBN_AON_PAD_GPIO1
 *           @arg HBN_AON_PAD_GPIO2
 *           @arg HBN_AON_PAD_GPIO3
 *           @arg HBN_AON_PAD_GPIO4
 *           @arg HBN_AON_PAD_GPIO5
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Aon_Pad_Cfg_Set(uint8_t aonPadHwCtrlEn, uint8_t aonGpio)
{
    uint32_t tmpVal;
    uint32_t enAonCtrlGpio;

    CHECK_PARAM(IS_HBN_AON_PAD_TYPE(aonGpio));

    /* hbn_pad_ctrl_0 */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_0);
    enAonCtrlGpio = BL_GET_REG_BITS_VAL(tmpVal, HBN_REG_EN_AON_CTRL_GPIO);

    if (aonPadHwCtrlEn) {
        enAonCtrlGpio |= (1 << aonGpio);
    } else {
        enAonCtrlGpio &= ~(1 << aonGpio);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_EN_AON_CTRL_GPIO, enAonCtrlGpio);

    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable BOD interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_BOD_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_IRQ_BOR_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable BOD interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable_BOD_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_IRQ_BOR_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN aon pad debbug pull config
 *
 * @param  puPdEn: Enable or disable aon pad pull down and pull up
 * @param  trigMode: trigger mode, this parameter can be one of the following values:
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL
 * @param  maskVal: int mask
 * @param  dlyEn: Enable or disable aon pad wakeup delay function
 * @param  dlySec: aon pad wakeup delay sec 1 to 7
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Aon_Pad_WakeUpCfg(BL_Fun_Type puPdEn, uint8_t trigMode, uint32_t maskVal, BL_Fun_Type dlyEn, uint8_t dlySec)
{
    uint32_t tmpVal;

    CHECK_PARAM(((dlySec >= 1) && (dlySec <= 7)));
    CHECK_PARAM(IS_HBN_GPIO_INT_TRIGGER_TYPE(trigMode));
    CHECK_PARAM((maskVal <= 0x3FF));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_EN, dlyEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_SEL, dlySec);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_EN_HW_PU_PD, puPdEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MODE, trigMode);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Init WatchDog in hbn domain
 *
 * @param  mode:
 * @param  comp_val:
 *
 * @return NULL
 *
*******************************************************************************/
void HBN_Wdg_Init(uint8_t mode, uint16_t comp_val)
{
    uint32_t tmpVal = 0;

    BL_WR_REG(HBN_BASE, HBN_WFAR, 0xBABA);
    BL_WR_REG(HBN_BASE, HBN_WSAR, 0xEB10);
    tmpVal = BL_RD_REG(HBN_BASE, HBN_WMER);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_WE);
    if (mode == HBN_WDG_MODE_INTERRUPT) {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_WRIE);
    } else {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_WRIE);
    }
    BL_WR_REG(HBN_BASE, HBN_WMER, tmpVal);

    BL_WR_REG(HBN_BASE, HBN_WFAR, 0xBABA);
    BL_WR_REG(HBN_BASE, HBN_WSAR, 0xEB10);
    tmpVal = BL_RD_REG(HBN_BASE, HBN_WCR);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_WCR);
    BL_WR_REG(HBN_BASE, HBN_WCR, tmpVal);

    BL_WR_REG(HBN_BASE, HBN_WFAR, 0xBABA);
    BL_WR_REG(HBN_BASE, HBN_WSAR, 0xEB10);
    tmpVal = BL_RD_REG(HBN_BASE, HBN_WMR);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_WMR, comp_val);
    BL_WR_REG(HBN_BASE, HBN_WMR, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Start WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return NULL
 *
*******************************************************************************/
void HBN_Wdg_Start(void)
{
    uint32_t tmpVal = 0;

    BL_WR_REG(HBN_BASE, HBN_WFAR, 0xBABA);
    BL_WR_REG(HBN_BASE, HBN_WSAR, 0xEB10);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_WMER);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_WE);
    BL_WR_REG(HBN_BASE, HBN_WMER, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Stop WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return NULL
 *
*******************************************************************************/
void HBN_Wdg_Stop(void)
{
    uint32_t tmpVal = 0;

    BL_WR_REG(HBN_BASE, HBN_WFAR, 0xBABA);
    BL_WR_REG(HBN_BASE, HBN_WSAR, 0xEB10);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_WMER);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_WE);
    BL_WR_REG(HBN_BASE, HBN_WMER, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Get Counter Value of WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return Counter Value
 *
*******************************************************************************/
uint16_t HBN_Wdg_Get_Countervalue(void)
{
    return BL_RD_REG(HBN_BASE, HBN_WVR);
}

/****************************************************************************/ /**
 * @brief  Set Counter Value of WatchDog in hbn domain
 *
 * @param  value: Counter Value
 *
 * @return NULL
 *
*******************************************************************************/
void HBN_Wdg_Set_Countervalue(uint16_t value)
{
    BL_WR_REG(HBN_BASE, HBN_WFAR, 0xBABA);
    BL_WR_REG(HBN_BASE, HBN_WSAR, 0xEB10);

    BL_WR_REG(HBN_BASE, HBN_WVR, value & (0xFFFF));
}

/****************************************************************************/ /**
 * @brief  Reset Counter Value of WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return NULL
 *
*******************************************************************************/
void HBN_Wdg_Reset_Countervalue(void)
{
    uint32_t tmpVal = 0;

    BL_WR_REG(HBN_BASE, HBN_WFAR, 0xBABA);
    BL_WR_REG(HBN_BASE, HBN_WSAR, 0xEB10);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_WCR);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_WCR);
    BL_WR_REG(HBN_BASE, HBN_WCR, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Clear Interrupt of WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return NULL
 *
*******************************************************************************/
void HBN_Wdg_Compint_Clear(void)
{
    uint32_t tmpVal = 0;

    BL_WR_REG(HBN_BASE, HBN_WFAR, 0xBABA);
    BL_WR_REG(HBN_BASE, HBN_WSAR, 0xEB10);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_WICR);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_WICLR);
    BL_WR_REG(HBN_BASE, HBN_WICR, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Get hbn version register value
 *
 * @param  NULL
 *
 * @return version register value
 *
*******************************************************************************/
uint32_t HBN_Get_Version(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_VERSION);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_RESV_FOR_SOFTWARE);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  Set hbn version register value
 *
 * @param  version register value
 *
 * @return NULL
 *
*******************************************************************************/
void HBN_Set_Version(uint32_t version)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_VERSION);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RESV_FOR_SOFTWARE, version);
    BL_WR_REG(HBN_BASE, HBN_VERSION, tmpVal);
}

/*@} end of group HBN_Public_Functions */

/*@} end of group HBN */

/*@} end of group BL616L_Peripheral_Driver */
