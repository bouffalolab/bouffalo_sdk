/**
  ******************************************************************************
  * @file    bl616d_hbn.c
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

#include "bl616d_hbn.h"
#include "bl616d_glb.h"
#include "bflb_xip_sflash.h"
#include "bflb_gpio.h"

/** @addtogroup  BL616D_Peripheral_Driver
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
 * @brief  power down and switch clock
 *
 * @param  hbn_level: hbn mode level, this parameter can be one of the following values:
 *          @arg HBN_LEVEL_0
 *          @arg HBN_LEVEL_1
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
            break;
        case HBN_LEVEL_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
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
 * @brief  Select GPADC clk source
 *
 * @param  clkSel: GPADC clock type selection, this parameter can be one of the following values:
 *           @arg HBN_GPADC_CLK_32M
 *           @arg HBN_GPADC_CLK_F32K
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_GPADC_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_GPADC_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPADC_CS, clkSel);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable HBN PIR
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PIR_EN);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable HBN PIR
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PIR_EN);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Config HBN PIR interrupt
 *
 * @param  pirIntCfg: HBN PIR interrupt configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg)
{
    uint32_t tmpVal;
    uint32_t bit4 = 0;
    uint32_t bit5 = 0;
    uint32_t bitVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);

    /* low trigger interrupt */
    if (pirIntCfg->lowIntEn == ENABLE) {
        bit5 = 0;
    } else {
        bit5 = 1;
    }

    /* high trigger interrupt */
    if (pirIntCfg->highIntEn == ENABLE) {
        bit4 = 0;
    } else {
        bit4 = 1;
    }

    bitVal = bit4 | (bit5 << 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_DIS, bitVal);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select HBN PIR low pass filter
 *
 * @param  lpf: HBN PIR low pass filter selection, this parameter can be one of the following values:
 *           @arg HBN_PIR_LPF_DIV1
 *           @arg HBN_PIR_LPF_DIV2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_LPF_Sel(uint8_t lpf)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_PIR_LPF_TYPE(lpf));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_LPF_SEL, lpf);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select HBN PIR high pass filter
 *
 * @param  hpf: HBN PIR high pass filter selection, this parameter can be one of the following values:
 *           @arg HBN_PIR_HPF_METHOD0
 *           @arg HBN_PIR_HPF_METHOD1
 *           @arg HBN_PIR_HPF_METHOD2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_HPF_Sel(uint8_t hpf)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_PIR_HPF_TYPE(hpf));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_HPF_SEL, hpf);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set HBN PIR threshold value
 *
 * @param  threshold: HBN PIR threshold value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_PIR_Threshold(uint16_t threshold)
{
    uint32_t tmpVal;

    CHECK_PARAM((threshold <= 0x3FFF));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_VTH);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_VTH, threshold);
    BL_WR_REG(HBN_BASE, HBN_PIR_VTH, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get HBN PIR threshold value
 *
 * @param  None
 *
 * @return HBN PIR threshold value
 *
*******************************************************************************/
uint16_t HBN_Get_PIR_Threshold(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_VTH);

    return BL_GET_REG_BITS_VAL(tmpVal, HBN_PIR_VTH);
}

/****************************************************************************/ /**
 * @brief  Set HBN PIR interval value
 *
 * @param  interval: HBN PIR interval value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_PIR_Interval(uint16_t interval)
{
    uint32_t tmpVal;

    CHECK_PARAM((interval <= 0xFFF));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_INTERVAL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_INTERVAL, interval);
    BL_WR_REG(HBN_BASE, HBN_PIR_INTERVAL, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get HBN PIR interval value
 *
 * @param  None
 *
 * @return HBN PIR interval value
 *
*******************************************************************************/
uint16_t HBN_Get_PIR_Interval(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_INTERVAL);

    return BL_GET_REG_BITS_VAL(tmpVal, HBN_PIR_INTERVAL);
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
 *           @arg HBN_UART_CLK_WIFIPLL_160M
 *           @arg HBN_UART_CLK_XCLK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_UART_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_UART_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    if ((HBN_UART_CLK_MCU_BCLK == clkSel) || (HBN_UART_CLK_WIFIPLL_160M == clkSel)) {
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
 * @brief  Trim RC32K According to Efuse
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
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "rc32k", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_RC32K_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_RC32K_CAP_SEL_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_RC32K_0, tmpVal);
            arch_delay_us(2);
            tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_CTRL0);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RC32K_EXT_CODE_EN);
            BL_WR_REG(HBN_BASE, HBN_RC32K_CTRL0, tmpVal);
            arch_delay_us(2);
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
 * @brief  Set HBN Gpio Keep
 *
 * @param  gpioKeep:HBN gpio keep reg cfg
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Gpio_Keep(uint8_t gpioKeep)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_KEEP_STS, HBN_GPIO_KEEP_FLAG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_KEEP_PIN, gpioKeep);
    BL_WR_REG(HBN_BASE, HBN_RSV3, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get HBN GPIO Keep bit
 *
 * @param  gpioKeep:HBN gpio keep reg cfg
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Get_Gpio_Keep(uint8_t *gpioKeep)
{
    uint32_t tmpVal = 0;

    if (NULL == gpioKeep) {
        return ERROR;
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    if (HBN_GPIO_KEEP_FLAG == BL_GET_REG_BITS_VAL(tmpVal, HBN_GPIO_KEEP_STS)) {
        *gpioKeep = BL_GET_REG_BITS_VAL(tmpVal, HBN_GPIO_KEEP_PIN);
        return SUCCESS;
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Clear HBN GPIO Keep bit
 *
 * @param  gpioKeep:HBN gpio keep reg cfg
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Clear_Gpio_Keep(uint8_t gpioKeep)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_0);
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_CR_GPIO_KEEP_EN, (gpioKeep & 0x7));
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_AON_GPIO_ISO_MODE, ((gpioKeep >> 3) & 0x1));
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set PDS Gpio Keep
 *
 * @param  gpioKeep:PDS gpio keep reg cfg
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_PDS_Gpio_Keep(uint8_t gpioKeep)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_KEEP_STS, PDS_GPIO_KEEP_FLAG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_KEEP_PIN, gpioKeep);
    BL_WR_REG(HBN_BASE, HBN_RSV3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get PDS GPIO Keep bit
 *
 * @param  gpioKeep:PDS gpio keep reg cfg
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Get_PDS_Gpio_Keep(uint8_t *gpioKeep)
{
    uint32_t tmpVal = 0;

    if (NULL == gpioKeep) {
        return ERROR;
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV3);
    if (PDS_GPIO_KEEP_FLAG == BL_GET_REG_BITS_VAL(tmpVal, PDS_GPIO_KEEP_STS)) {
        *gpioKeep = BL_GET_REG_BITS_VAL(tmpVal, PDS_GPIO_KEEP_PIN);
        return SUCCESS;
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Clear PDS GPIO Keep bit
 *
 * @param  gpioKeep:PDS gpio keep reg cfg
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Clear_PDS_Gpio_Keep(uint8_t gpioKeep)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL5);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_KEEP_EN, (gpioKeep & 0x7));
    BL_WR_REG(PDS_BASE, PDS_CTL5, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_GPIO_ISO_MODE, ((gpioKeep >> 3) & 0x1));
    /* don't entry PDS */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_START_PS);
    BL_WR_REG(PDS_BASE, PDS_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable GPIO Keep,include PDS_IO and HBN_IO
 *
 * @param  pin: gpio number
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Disable_Gpio_Keep(int pin)
{
    uint32_t tmpVal = 0;

    if (pin <= 7) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_0);
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_AON_GPIO_ISO_MODE);
        BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, tmpVal);
    } else if (pin < GPIO_PIN_MAX) {
        /* PDS_IO keep disable */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL);
        tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_GPIO_ISO_MODE);
        /* don't entry PDS */
        tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_START_PS);
        BL_WR_REG(PDS_BASE, PDS_CTL, tmpVal);

        tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL5);
        tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_GPIO_KEEP_EN);
        BL_WR_REG(PDS_BASE, PDS_CTL5, tmpVal);
    } else {
        return ERROR;
    }

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
BL_Err_Type ATTR_TCM_SECTION HBN_Clear_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Clear RTC control bit0 */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal & 0xfffffffe);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable RTC timer counter
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
 *                   mode:HBN_RTC_COMP_BIT0_39,HBN_RTC_COMP_BIT0_23,HBN_RTC_COMP_BIT13_39
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
    BL_WR_REG(HBN_BASE, HBN_TIME_H, compValHigh);

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
 * @param  valHigh: RTC count value pointer for high 8 bits
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
    *valHigh = (BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H) & 0xffffffff);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN Re-Cal RC32K
 *
 * @param  expected_counter: Expected rtc counter
 * @param  actual_counter: Actual rtc counter
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Recal_RC32K(int32_t expected_counter, int32_t actual_counter)
{
    int32_t tmpVal = 0;
    int32_t current = 0;
    int32_t delta = 0;

    delta = actual_counter - expected_counter;

    /* normalize to 1s count */
    delta = (delta * 32768) / expected_counter;

    if ((delta < 32) && (delta > -32)) {
        return -1;
    }
    if (delta < -320) {
        delta = -320;
    } else if (delta > 320) {
        delta = 320;
    }

    if (delta >= 64 || delta <= -64) {
        delta = delta / 64;
    } else {
        delta = delta / 32;
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_CTRL0);
    current = BL_GET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_EXT);
    current += delta;
    current &= ((1U << HBN_RC32K_CODE_FR_EXT_LEN) - 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_EXT, current);
    BL_WR_REG(HBN_BASE, HBN_RC32K_CTRL0, tmpVal);

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
 *           @arg HBN_INT_GPIO6
 *           @arg HBN_INT_GPIO7
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
 * @brief  HBN get all interrupt status
 *
 * @return IntStatus
 *
*******************************************************************************/
uint32_t HBN_Get_All_INT_State(void)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_INT_TYPE(irqType));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_STAT);

    return tmpVal;
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
 *           @arg HBN_INT_GPIO6
 *           @arg HBN_INT_GPIO7
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
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 16~19
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

    if (pad <= 7) {
        aonPadID = pad;
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
 *           @arg HBN_AON_PAD_GPIO6
 *           @arg HBN_AON_PAD_GPIO7
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
 *           @arg HBN_AON_PAD_GPIO16
 *           @arg HBN_AON_PAD_GPIO17
 *           @arg HBN_AON_PAD_GPIO18
 *           @arg HBN_AON_PAD_GPIO19
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

/*@} end of group HBN_Public_Functions */

/*@} end of group HBN */

/*@} end of group BL616D_Peripheral_Driver */
