/**
  ******************************************************************************
  * @file    bl702l_glb.c
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

#include "bl702l_glb.h"
#include "bl702l_hbn.h"

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  GLB
 *  @{
 */

/** @defgroup  GLB_Private_Macros
 *  @{
 */
#define GLB_CLK_SET_DUMMY_WAIT \
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
#define GLB_REG_BCLK_DIS_TRUE  (*(volatile uint32_t *)(0x40000FFC) = (0x00000001))
#define GLB_REG_BCLK_DIS_FALSE (*(volatile uint32_t *)(0x40000FFC) = (0x00000000))

/*@} end of group GLB_Private_Macros */

/** @defgroup  GLB_Private_Types
 *  @{
 */

/*@} end of group GLB_Private_Types */

/** @defgroup  GLB_Private_Variables
 *  @{
 */
static intCallback_Type *glbBmxErrIntCbfArra[BMX_ERR_INT_ALL] = { NULL };
static intCallback_Type *glbBmxToIntCbfArra[BMX_TO_INT_ALL] = { NULL };
/*@} end of group GLB_Private_Variables */

/** @defgroup  GLB_Global_Variables
 *  @{
 */

/*@} end of group GLB_Global_Variables */

/** @defgroup  GLB_Private_Fun_Declaration
 *  @{
 */

/*@} end of group GLB_Private_Fun_Declaration */

/** @defgroup  GLB_Private_Functions
 *  @{
 */

/*@} end of group GLB_Private_Functions */

/** @defgroup  GLB_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  get root clock selection
 *
 * @param  None
 *
 * @return root clock selection
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
GLB_ROOT_CLK_Type ATTR_CLOCK_SECTION GLB_Get_Root_CLK_Sel(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);

    switch (BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_ROOT_CLK_SEL)) {
        case 0:
            return GLB_ROOT_CLK_RC32M;
        case 1:
            return GLB_ROOT_CLK_XTAL;
        case 2:
            return GLB_ROOT_CLK_DLL;
        case 3:
            return GLB_ROOT_CLK_DLL;
        default:
            return GLB_ROOT_CLK_RC32M;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Set System clock divider
 *
 * @param  hclkDiv: HCLK divider
 * @param  bclkDiv: BCLK divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_System_CLK_Div(uint8_t hclkDiv, uint8_t bclkDiv)
{
    /***********************************************************************************/
    /*                                 NOTE                                            */
    /* "GLB_REG_BCLK_DIS_TRUE + GLB_REG_BCLK_DIS_FALSE" will stop bclk a little while. */
    /* OCRAM use bclk as source clock. Pay attention to risks when using this API.     */
    /***********************************************************************************/
    uint32_t tmpVal;

    /* recommend: fclk<=128MHz, bclk<=64MHz */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV, hclkDiv);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV, bclkDiv);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG0, tmpVal);
    GLB_REG_BCLK_DIS_TRUE;
    GLB_REG_BCLK_DIS_FALSE;
    GLB_CLK_SET_DUMMY_WAIT;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_HCLK_EN);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_BCLK_EN);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG0, tmpVal);
    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Bus clock divider
 *
 * @param  None
 *
 * @return Clock Divider
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
uint8_t ATTR_CLOCK_SECTION GLB_Get_BCLK_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV);
}
#endif

/****************************************************************************/ /**
 * @brief  Get CPU clock divider
 *
 * @param  None
 *
 * @return Clock Divider
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
uint8_t ATTR_CLOCK_SECTION GLB_Get_HCLK_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV);
}
#endif

/****************************************************************************/ /**
 * @brief  Set System clock
 *
 * @param  xtalType: XTAL frequency type
 * @param  clkFreq: clock frequency selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_System_CLK(GLB_DLL_XTAL_Type xtalType, GLB_SYS_CLK_Type clkFreq)
{
    uint32_t tmpVal;
    HBN_XCLK_CLK_Type xclkSel;

    CHECK_PARAM(IS_GLB_DLL_XTAL_TYPE(xtalType));
    CHECK_PARAM(IS_GLB_SYS_CLK_TYPE(clkFreq));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);
    xclkSel = (HBN_XCLK_CLK_Type)(tmpVal & 0x01);

    /* reg_bclk_en = reg_hclk_en = reg_fclk_en = 1, cannot be zero */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_BCLK_EN);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_HCLK_EN);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_FCLK_EN);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG0, tmpVal);

    /* Before config XTAL and DLL ,make sure root clk is from RC32M */
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);
    GLB_Set_System_CLK_Div(0, 0);

    if (xtalType == GLB_DLL_XTAL_NONE) {
        if (clkFreq == GLB_SYS_CLK_RC32M) {
            return SUCCESS;
        } else {
            return ERROR;
        }
    }

    if (xtalType != GLB_DLL_XTAL_RC32M) {
        /* power on xtal first */
        AON_Power_On_XTAL();
    }

    /* Bl702 make PLL Setting out of RF, so following setting can be removed*/
    //AON_Power_On_MBG();
    //AON_Power_On_LDO15_RF();

    /* always power up PLL and enable all PLL clock output */
    //PDS_Power_On_PLL((PDS_PLL_XTAL_Type)xtalType);
    //BL702L_Delay_US(55);
    //PDS_Enable_PLL_All_Clks();

    /* always power up DLL and enable all DLL clock output */
    GLB_Power_Off_DLL();
    BL702L_Delay_US(5);
    GLB_Power_On_DLL(xtalType);
    GLB_Enable_DLL_All_Clks();

    /* reg_pll_en = 1, cannot be zero */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_PLL_EN);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG0, tmpVal);

    /* select pll output clock before select root clock */
    if (clkFreq >= GLB_SYS_CLK_DLL25P6M) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_PLL_SEL, clkFreq - GLB_SYS_CLK_DLL25P6M);
        BL_WR_REG(GLB_BASE, GLB_CLK_CFG0, tmpVal);
    }
    /* select root clock */
    switch (clkFreq) {
        case GLB_SYS_CLK_RC32M:
            break;
        case GLB_SYS_CLK_XTAL:
            HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);
            break;
        case GLB_SYS_CLK_DLL25P6M:
            HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_DLL);
            HBN_Set_XCLK_CLK_Sel(xclkSel);
            break;
        case GLB_SYS_CLK_DLL42P67M:
            HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_DLL);
            HBN_Set_XCLK_CLK_Sel(xclkSel);
            break;
        case GLB_SYS_CLK_DLL64M:
            HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_DLL);
            HBN_Set_XCLK_CLK_Sel(xclkSel);
            break;
        case GLB_SYS_CLK_DLL128M:
            L1C_IROM_2T_Access_Set(ENABLE);
            GLB_Set_System_CLK_Div(0, 1);
            HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_DLL);
            HBN_Set_XCLK_CLK_Sel(xclkSel);
            break;
        default:
            break;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  This is demo for user that use RC32M as default bootup clock instead of DLL,when APP is
 *         started, this function can be called to set DLL to 128M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION System_Core_Clock_Update_From_RC32M(void)
{
    SF_Ctrl_Cfg_Type sfCtrlCfg = {
        .owner = SF_CTRL_OWNER_IAHB,
        .clkDelay = 1,
        .clkInvert = 1,
        .rxClkInvert = 1,
        .doDelay = 0,
        .diDelay = 0,
        .oeDelay = 0,
    };
    /* Use RC32M as DLL ref source to set up DLL to 128M */
    GLB_Set_System_CLK(GLB_DLL_XTAL_RC32M, GLB_SYS_CLK_DLL128M);
    /* Flash controller also need changes since system (bus) clock changed */
    SF_Ctrl_Enable(&sfCtrlCfg);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  set mac154 and zigbee clock
 *
 * @param  enable: Enable or disable mac154 and zigbee clock
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_MAC154_ZIGBEE_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_M154_ZBEN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_M154_ZBEN);
    }
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG1, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG2);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CGEN_S300);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CGEN_S300);
    }
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set BLE clock
 *
 * @param  enable: Enable or disable BLE clock
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_BLE_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_BLE_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_BLE_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG1, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG2);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CGEN_S301);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CGEN_S301);
    }
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set AUDIO clock divider
 *
 * @param  clkDivEn: enable or disable AUDIO clock div
 * @param  autoDivEn: enable or disable AUDIO auto div
 * @param  clkSel: AUDIO clock source type
 * @param  div: divider, 0~15 stand for 1~16 divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_AUDIO_CLK(uint8_t clkDivEn, uint8_t autoDivEn, GLB_AUDIO_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_AUDIO_CLK_SRC_TYPE(clkSel));
    CHECK_PARAM((div <= 0xF));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_CLK_SRC_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG1, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);
    if (clkDivEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_AUDIO_CLK_DIV_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_CLK_DIV_EN);
    }
    if (autoDivEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set KYS clock
 *
 * @param  clkSel: KYS clock source type
 * @param  div: clock divider, 0~31 stand for 1~32 divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_KYS_CLK(GLB_KYS_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_KYS_CLK_SRC_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_KYS_1M_CLK_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_KYS_1M_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set DMA clock
 *
 * @param  enable: Enable or disable DMA clock
 * @param  clk: DMA ID type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_DMA_CLK(uint8_t enable, GLB_DMA_CLK_ID_Type clk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, GLB_DMA_CLK_EN);
    if (enable) {
        tmpVal2 |= (1 << clk);
    } else {
        tmpVal2 &= (~(1 << clk));
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DMA_CLK_EN, tmpVal2);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set IR clock divider
 *
 * @param  enable: enable or disable IR clock
 * @param  clkSel: IR clock type, only XCLK could be selected
 * @param  div: divider, 0~63 stand for 1~64 divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_IR_CLK(uint8_t enable, GLB_IR_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_IR_CLK_SRC_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_IR_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG2, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_IR_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_IR_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set sflash clock
 *
 * @param  enable: Enable or disable sflash clock
 * @param  clkSel: sflash clock type
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_SF_CLK(uint8_t enable, GLB_SFLASH_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_SFLASH_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x7));

    /* disable SFLASH clock first */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SF_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG2, tmpVal);

    /* enable relevant DLL clock */
    if (clkSel == GLB_SFLASH_CLK_64M) {
        GLB_Enable_DLL_Clk(GLB_DLL_CLK_64M);
    } else if (clkSel == GLB_SFLASH_CLK_42P67M) {
        GLB_Enable_DLL_Clk(GLB_DLL_CLK_42P67M);
    } else if (clkSel == GLB_SFLASH_CLK_BCLK) {
        GLB_Enable_DLL_Clk(GLB_DLL_CLK_128M);
        GLB_Enable_DLL_Clk(GLB_DLL_CLK_64M);
        GLB_Enable_DLL_Clk(GLB_DLL_CLK_42P67M);
        GLB_Enable_DLL_Clk(GLB_DLL_CLK_25P6M);
    }

    /* clock divider and source */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG2, tmpVal);

    /* enable or disable flash clock */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SF_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SF_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG2, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  set UART clock
 *
 * @param  enable: Enable or disable UART clock
 * @param  clkSel: UART clock type
 * @param  div: clock divider, 0~7 stand for 1~8 divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_UART_CLK(uint8_t enable, HBN_UART_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((div <= 0x7));
    CHECK_PARAM(IS_HBN_UART_CLK_TYPE(clkSel));

    /* disable UART clock first */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_UART_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG2, tmpVal);

    /* Set div */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_UART_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG2, tmpVal);

    /* Select clock source for uart */
    HBN_Set_UART_CLK_Sel(clkSel);

    /* Set enable or disable */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_UART_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_UART_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  gpio timer clock select
 *
 * @param  gpioPin: gpio pin number
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Sel_TMR_GPIO_Clock(GLB_GPIO_Type gpioPin)
{
    uint32_t tmpVal, tmpValEn;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpValEn = BL_GET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_EN);
    switch (gpioPin & 0x3) {
        case 0: /* inout_sig_0 */
            tmpValEn &= ~(1 << 0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPIO_TMR_CLK_SEL, 0);
            break;
        case 1: /* inout_sig_1 */
            tmpValEn &= ~(1 << 1);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPIO_TMR_CLK_SEL, 1);
            break;
        case 2: /* inout_sig_2 */
            tmpValEn &= ~(1 << 2);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPIO_TMR_CLK_SEL, 2);
            break;
        case 3: /* inout_sig_3 */
            tmpValEn &= ~(1 << 3);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPIO_TMR_CLK_SEL, 3);
            break;
        default:
            break;
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_EN, tmpValEn);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  select chip clock out 0 type
 *
 * @param  clkSel: chip clock out type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Out_0_CLK_Sel(GLB_CHIP_CLK_OUT_0_Type clkSel)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_CHIP_CLK_OUT_0_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_0_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  select chip clock out 1 type
 *
 * @param  clkSel: chip clock out type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Out_1_CLK_Sel(GLB_CHIP_CLK_OUT_1_Type clkSel)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_CHIP_CLK_OUT_1_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_1_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  enable or disable clock out
 *
 * @param  enable: ENABLE or DISABLE
 * @param  gpioPin: pin index assert clock output
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Out_CLK_Enable(uint8_t enable, uint8_t pin)
{
    uint32_t tmpVal = 0;
    uint32_t tmpValEn = 0;
    uint32_t shift = pin & 3;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpValEn = BL_GET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_EN);
    if (enable) {
        tmpValEn |= 1 << shift;
    } else {
        tmpValEn &= ~(1 << shift);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_EN, tmpValEn);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set I2C clock
 *
 * @param  enable: Enable or disable I2C clock
 * @param  div: clock divider, 0~255 stand for 1~256 divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_I2C_CLK_SRC_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG3, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_I2C_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_I2C_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set SPI clock
 *
 * @param  enable: Enable or disable SPI clock
 * @param  div: clock divider, 0~31 stand for 1~32 divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_SPI_CLK_SRC_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG3, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SPI_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SPI_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  select PKA clock source
 *
 * @param  clkSel: PKA clock selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_SRC_Type clkSel)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PKA_CLK_SRC_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PKA_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Software system reset
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION GLB_SW_System_Reset(void)
{
    /***********************************************************************************/
    /*                                 NOTE                                            */
    /* "GLB_REG_BCLK_DIS_TRUE + GLB_REG_BCLK_DIS_FALSE" will stop bclk a little while. */
    /* OCRAM use bclk as source clock. Pay attention to risks when using this API.     */
    /***********************************************************************************/
    uint32_t tmpVal;

    /* Swicth clock to 32M as default */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, 0);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    GLB_CLK_SET_DUMMY_WAIT;

    /* HCLK is RC32M , so BCLK/HCLK no need divider */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV, 0);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG0, tmpVal);
    GLB_REG_BCLK_DIS_TRUE;
    GLB_REG_BCLK_DIS_FALSE;
    GLB_CLK_SET_DUMMY_WAIT;

    /* Do reset */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_PWRON_RST);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,GLB_REG_CTRL_PWRON_RST);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    /* waiting for reset */
    while (1) {
        BL702L_Delay_US(10);
    }

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Software CPU reset
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION GLB_SW_CPU_Reset(void)
{
    /***********************************************************************************/
    /*                                 NOTE                                            */
    /* "GLB_REG_BCLK_DIS_TRUE + GLB_REG_BCLK_DIS_FALSE" will stop bclk a little while. */
    /* OCRAM use bclk as source clock. Pay attention to risks when using this API.     */
    /***********************************************************************************/
    uint32_t tmpVal;

    /* Swicth clock to 32M as default */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, 0);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    GLB_CLK_SET_DUMMY_WAIT;

    /* HCLK is RC32M , so BCLK/HCLK no need divider */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV, 0);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG0, tmpVal);
    GLB_REG_BCLK_DIS_TRUE;
    GLB_REG_BCLK_DIS_FALSE;
    GLB_CLK_SET_DUMMY_WAIT;

    /* Do reset */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_PWRON_RST);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,GLB_REG_CTRL_SYS_RESET);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,GLB_REG_CTRL_PWRON_RST);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    /* waiting for reset */
    while (1) {
        BL702L_Delay_US(10);
    }

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Software power on reset
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION GLB_SW_POR_Reset(void)
{
    /***********************************************************************************/
    /*                                 NOTE                                            */
    /* "GLB_REG_BCLK_DIS_TRUE + GLB_REG_BCLK_DIS_FALSE" will stop bclk a little while. */
    /* OCRAM use bclk as source clock. Pay attention to risks when using this API.     */
    /***********************************************************************************/
    uint32_t tmpVal;

    /* Swicth clock to 32M as default */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, 0);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    GLB_CLK_SET_DUMMY_WAIT;

    /* HCLK is RC32M , so BCLK/HCLK no need divider */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV, 0);
    BL_WR_REG(GLB_BASE, GLB_CLK_CFG0, tmpVal);
    GLB_REG_BCLK_DIS_TRUE;
    GLB_REG_BCLK_DIS_FALSE;
    GLB_CLK_SET_DUMMY_WAIT;

    /* Do reset */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_PWRON_RST);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_PWRON_RST);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    /* waiting for reset */
    while (1) {
        BL702L_Delay_US(10);
    }

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  software reset
 *
 * @param  swrst: reset num
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_Type swrst)
{
    uint32_t tmpVal = 0;
    uint32_t bit = 0;
    uint32_t regAddr = 0;

    CHECK_PARAM(IS_GLB_AHB_MCU_SW_TYPE(swrst));

    if (swrst == GLB_AHB_MCU_SW_PWRON_RST) {
        return GLB_SW_POR_Reset();
    } else if (swrst == GLB_AHB_MCU_SW_CPU_RESET) {
        return GLB_SW_CPU_Reset();
    } else if (swrst == GLB_AHB_MCU_SW_SYS_RESET) {
        return GLB_SW_System_Reset();
    }

    if (swrst < 32) {
        bit = swrst;
        regAddr = GLB_BASE + GLB_SWRST_CFG0_OFFSET;
    } else if (swrst < 64) {
        bit = swrst - 32;
        regAddr = GLB_BASE + GLB_SWRST_CFG1_OFFSET;
    } else if (swrst < 96) {
        bit = swrst - 64;
        regAddr = GLB_BASE + GLB_SWRST_CFG2_OFFSET;
    }

    tmpVal = BL_RD_WORD(regAddr);
    tmpVal &= ~(1 << bit);
    BL_WR_WORD(regAddr, tmpVal);
    BL_DRV_DUMMY;
    tmpVal = BL_RD_WORD(regAddr);
    tmpVal |= (1 << bit);
    BL_WR_WORD(regAddr, tmpVal);
    BL_DRV_DUMMY;
    tmpVal = BL_RD_WORD(regAddr);
    tmpVal &= ~(1 << bit);
    BL_WR_WORD(regAddr, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  dis reset
 *
 * @param  enable: ENABLE or DISABLE
 * @param  disrst: disrst macro
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Disrst_Set(uint8_t enable, GLB_DISRST_Type disrst)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_DISRST_TYPE(disrst));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG3);
    if (enable) {
        tmpVal |= (1 << disrst);
    } else {
        tmpVal &= ~(1 << disrst);
    }
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  MAC154_ZIGBEE reset
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_MAC154_ZIGBEE_Reset(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG0);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SWRST_S300);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG0, tmpVal);
    BL_DRV_DUMMY;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SWRST_S300);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  BLE reset
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_BLE_Reset(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG0);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SWRST_S301);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG0, tmpVal);
    BL_DRV_DUMMY;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SWRST_S301);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get first 1 from u64, then clear it
 *
 * @param  val: target value
 * @param  bit: first 1 in bit
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type GLB_Get_And_Clr_First_Set_From_U64(uint64_t *val, uint32_t *bit)
{
    if (!*val) {
        return ERROR;
    }

    for (uint8_t i = 0; i < 64; i++) {
        if ((*val) & ((uint64_t)1 << i)) {
            *bit = i;
            (*val) &= ~((uint64_t)1 << i);
            break;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  hold IPs clock
 *
 * @param  ips: GLB_AHB_CLOCK_xxx | GLB_AHB_CLOCK_xxx | ......
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_PER_Clock_Gate(uint64_t ips)
{
    /* api request from cjy */

    uint32_t tmpValCfg0 = 0;
    uint32_t tmpValCfg1 = 0;
    uint32_t tmpValCfg2 = 0;
    uint32_t bitfield = 0;

    tmpValCfg0 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG0);
    tmpValCfg1 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    tmpValCfg2 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG2);
    while (ips) {
        if (SUCCESS == GLB_Get_And_Clr_First_Set_From_U64(&ips, &bitfield)) {
            switch (bitfield) {
                case GLB_AHB_CLOCK_IP_CPU:
                    tmpValCfg0 &= ~(1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_SEC:
                    tmpValCfg0 &= ~(1 << 2);
                    tmpValCfg1 &= ~(1 << 3);
                    tmpValCfg1 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_0:
                    tmpValCfg0 &= ~(1 << 3);
                    tmpValCfg1 &= ~(1 << 12);
                    break;
                case GLB_AHB_CLOCK_IP_CCI:
                    tmpValCfg0 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_GPIP:
                    tmpValCfg1 &= ~(1 << 2);
                    break;
                case GLB_AHB_CLOCK_IP_TZC:
                    tmpValCfg1 &= ~(1 << 5);
                    break;
                case GLB_AHB_CLOCK_IP_EF_CTRL:
                    tmpValCfg1 &= ~(1 << 7);
                    break;
                case GLB_AHB_CLOCK_IP_SF_CTRL:
                    tmpValCfg1 &= ~(1 << 11);
                    break;
                case GLB_AHB_CLOCK_IP_UART0:
                    tmpValCfg1 &= ~(1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_SPI:
                    tmpValCfg1 &= ~(1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_I2C:
                    tmpValCfg1 &= ~(1 << 19);
                    break;
                case GLB_AHB_CLOCK_IP_PWM:
                    tmpValCfg1 &= ~(1 << 20);
                    break;
                case GLB_AHB_CLOCK_IP_TIMER:
                    tmpValCfg1 &= ~(1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_IR:
                    tmpValCfg1 &= ~(1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_CHECKSUM:
                    tmpValCfg1 &= ~(1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_KYS:
                    tmpValCfg1 &= ~(1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_AUDIO:
                    tmpValCfg1 &= ~(1 << 29);
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_NORMAL:
                    tmpValCfg2 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_ZB_NORMAL:
                    tmpValCfg2 &= ~(1 << 0);
                    break;
                default:
                    break;
            }
        }
    }
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG0, tmpValCfg0);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpValCfg1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpValCfg2);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  release IPs clock
 *
 * @param  ips: GLB_AHB_CLOCK_xxx | GLB_AHB_CLOCK_xxx | ......
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_PER_Clock_UnGate(uint64_t ips)
{
    /* api request from cjy */

    uint32_t tmpValCfg0 = 0;
    uint32_t tmpValCfg1 = 0;
    uint32_t tmpValCfg2 = 0;
    uint32_t bitfield = 0;

    tmpValCfg0 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG0);
    tmpValCfg1 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    tmpValCfg2 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG2);
    while (ips) {
        if (SUCCESS == GLB_Get_And_Clr_First_Set_From_U64(&ips, &bitfield)) {
            switch (bitfield) {
                case GLB_AHB_CLOCK_IP_CPU:
                    tmpValCfg0 |= (1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_SEC:
                    tmpValCfg0 |= (1 << 2);
                    tmpValCfg1 |= (1 << 3);
                    tmpValCfg1 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_0:
                    tmpValCfg0 |= (1 << 3);
                    tmpValCfg1 |= (1 << 12);
                    break;
                case GLB_AHB_CLOCK_IP_CCI:
                    tmpValCfg0 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_GPIP:
                    tmpValCfg1 |= (1 << 2);
                    break;
                case GLB_AHB_CLOCK_IP_TZC:
                    tmpValCfg1 |= (1 << 5);
                    break;
                case GLB_AHB_CLOCK_IP_EF_CTRL:
                    tmpValCfg1 |= (1 << 7);
                    break;
                case GLB_AHB_CLOCK_IP_SF_CTRL:
                    tmpValCfg1 |= (1 << 11);
                    break;
                case GLB_AHB_CLOCK_IP_UART0:
                    tmpValCfg1 |= (1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_SPI:
                    tmpValCfg1 |= (1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_I2C:
                    tmpValCfg1 |= (1 << 19);
                    break;
                case GLB_AHB_CLOCK_IP_PWM:
                    tmpValCfg1 |= (1 << 20);
                    break;
                case GLB_AHB_CLOCK_IP_TIMER:
                    tmpValCfg1 |= (1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_IR:
                    tmpValCfg1 |= (1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_CHECKSUM:
                    tmpValCfg1 |= (1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_KYS:
                    tmpValCfg1 |= (1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_AUDIO:
                    tmpValCfg1 |= (1 << 29);
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_NORMAL:
                    tmpValCfg2 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_ZB_NORMAL:
                    tmpValCfg2 |= (1 << 0);
                    break;
                default:
                    break;
            }
        }
    }
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG0, tmpValCfg0);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpValCfg1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpValCfg2);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  RTC mode set
 *
 * @param  rtcMode: RTC_MODE_LOWPOWER or RTC_MODE_NORMAL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_RTC_Mode(RTC_MODE_Type rtcMode)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_RTC_MODE_TYPE(rtcMode));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_NEW_RTC_TIME_H);
    if (rtcMode == RTC_MODE_LOWPOWER) {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_RTC_TIMER_LATCH_EN);
    } else {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_RTC_TIMER_LATCH_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_NEW_RTC_TIME_H, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  BMX init
 *
 * @param  BmxCfg: BMX config
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_BMX_Init(BMX_Cfg_Type *BmxCfg)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((BmxCfg->timeoutEn) <= 0xF);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_BMX_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_BMX_TIMEOUT_EN, BmxCfg->timeoutEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_BMX_ERR_EN, BmxCfg->errEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_BMX_ARB_MODE, BmxCfg->arbMod);
    BL_WR_REG(GLB_BASE, GLB_BMX_CFG1, tmpVal);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(BMX_ERR_IRQn, BMX_ERR_IRQHandler);
    Interrupt_Handler_Register(BMX_TO_IRQn, BMX_TO_IRQHandler);
#endif

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  BMX address monitor enable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_BMX_Addr_Monitor_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_BMX_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_BMX_ERR_ADDR_DIS);
    BL_WR_REG(GLB_BASE, GLB_BMX_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  BMX address monitor disable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_BMX_Addr_Monitor_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_BMX_CFG2);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_BMX_ERR_ADDR_DIS);
    BL_WR_REG(GLB_BASE, GLB_BMX_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  BMX bus error response enable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_BMX_BusErrResponse_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_BMX_CFG1);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_BMX_ERR_EN);
    BL_WR_REG(GLB_BASE, GLB_BMX_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  BMX bus error response disable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_BMX_BusErrResponse_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_BMX_CFG1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_BMX_ERR_EN);
    BL_WR_REG(GLB_BASE, GLB_BMX_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get BMX error status
 *
 * @param  errType: BMX error status type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type GLB_BMX_Get_Status(BMX_BUS_ERR_Type errType)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_BMX_BUS_ERR_TYPE(errType));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_BMX_CFG2);
    if (errType == BMX_BUS_ERR_TRUSTZONE_DECODE) {
        return BL_GET_REG_BITS_VAL(tmpVal, GLB_BMX_ERR_TZ) ? SET : RESET;
    } else {
        return BL_GET_REG_BITS_VAL(tmpVal, GLB_BMX_ERR_DEC) ? SET : RESET;
    }
}

/****************************************************************************/ /**
 * @brief  Get BMX error address
 *
 * @param  None
 *
 * @return NP BMX error address
 *
*******************************************************************************/
uint32_t GLB_BMX_Get_Err_Addr(void)
{
    return BL_RD_REG(GLB_BASE, GLB_BMX_ERR_ADDR);
}

/****************************************************************************/ /**
 * @brief  BMX bus error clear set
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_BMX_BusErrClr_Set(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_BMX_CFG2);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_BMX_ERR_CLR);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_BMX_ERR_CLR);
    }
    BL_WR_REG(GLB_BASE, GLB_BMX_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  BMX error interrupt callback install
 *
 * @param  intType: BMX error interrupt type
 * @param  cbFun: callback
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type BMX_ERR_INT_Callback_Install(BMX_ERR_INT_Type intType, intCallback_Type *cbFun)
{
    CHECK_PARAM(IS_BMX_ERR_INT_TYPE(intType));

    glbBmxErrIntCbfArra[intType] = cbFun;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  BMX ERR interrupt IRQ handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void BMX_ERR_IRQHandler(void)
{
    BMX_ERR_INT_Type intType;

    for (intType = BMX_ERR_INT_ERR; intType < BMX_ERR_INT_ALL; intType++) {
        if (glbBmxErrIntCbfArra[intType] != NULL) {
            glbBmxErrIntCbfArra[intType]();
        }
    }

    while (1) {
        //MSG("BMX_ERR_IRQHandler\r\n");
        BL702L_Delay_MS(1000);
    }
}
#endif

/****************************************************************************/ /**
 * @brief  BMX timeout interrupt callback install
 *
 * @param  intType: BMX timeout interrupt type
 * @param  cbFun: callback
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type BMX_TIMEOUT_INT_Callback_Install(BMX_TO_INT_Type intType, intCallback_Type *cbFun)
{
    CHECK_PARAM(IS_BMX_TO_INT_TYPE(intType));

    glbBmxToIntCbfArra[intType] = cbFun;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  BMX Time Out interrupt IRQ handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void BMX_TO_IRQHandler(void)
{
    BMX_TO_INT_Type intType;

    for (intType = BMX_TO_INT_TIMEOUT; intType < BMX_TO_INT_ALL; intType++) {
        if (glbBmxToIntCbfArra[intType] != NULL) {
            glbBmxToIntCbfArra[intType]();
        }
    }

    while (1) {
        //MSG("BMX_TO_IRQHandler\r\n");
        BL702L_Delay_MS(1000);
    }
}
#endif

/****************************************************************************/ /**
 * @brief  set sram_param value
 *
 * @param  value: value, 18bits
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SRAM_PARM(uint32_t value)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SRAM_PARM);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SRAM_PARM, value);
    BL_WR_REG(GLB_BASE, GLB_SRAM_PARM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get sram_parm value
 *
 * @param  None
 *
 * @return value
 *
*******************************************************************************/
uint32_t GLB_Get_SRAM_PARM(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SRAM_PARM), GLB_REG_SRAM_PARM);
}

/****************************************************************************/ /**
 * @brief  set ocram_param value
 *
 * @param  value: value, 11bits
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_OCRAM_PARM(uint32_t value)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SRAM_PARM);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_OCRAM_PARM, value);
    BL_WR_REG(GLB_BASE, GLB_SRAM_PARM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get ocram_parm value
 *
 * @param  None
 *
 * @return value
 *
*******************************************************************************/
uint32_t GLB_Get_OCRAM_PARM(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SRAM_PARM), GLB_REG_OCRAM_PARM);
}

/****************************************************************************/ /**
 * @brief  select EM type
 *
 * @param  emType: EM type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_EM_Sel(GLB_EM_Type emType)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_EM_TYPE(emType));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SEAM_MISC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, emType);
    BL_WR_REG(GLB_BASE, GLB_SEAM_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Key Scan Column Drive
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Kys_Drv_Col(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_KYS_DRV_VAL);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_KYS_DRV_VAL);
    }
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GPIO_O latch mode set
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_O_Latch_Mode_Set(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_EN_GPIO_O_LATCH_MODE);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_EN_GPIO_O_LATCH_MODE);
    }
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  swap JTAG gpio pins function
 *
 * @param  swapSel: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_JTAG_Sig_Swap_Set(uint8_t swapSel)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_JTAG_SWAP_SET, swapSel);
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  CCI use GPIO 0 1 2 7
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_CCI_Use_IO_0_1_2_7(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_P3_CCI_USE_IO_0_2_7);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_P3_CCI_USE_IO_0_2_7);
    }
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  CCI use JTAG pin
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_CCI_Use_Jtag_Pin(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CCI_USE_JTAG_PIN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CCI_USE_JTAG_PIN);
    }
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  swap SPI0 MOSI with MISO
 *
 * @param  newState: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Swap_SPI_0_MOSI_With_MISO(BL_Fun_Type newState)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SPI_0_SWAP, newState);
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select SPI_0 act mode
 *
 * @param  mod: SPI work mode
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_Type mod)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_SPI_PAD_ACT_AS_TYPE(mod));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SPI_0_MASTER_MODE, mod);
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable or disable flash gpio scenario
 *
 * @param  ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION GLB_Set_Flash_Scenario(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CFG_FLASH2_SCENARIO);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CFG_FLASH2_SCENARIO);
    }
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Set embedded flash io parameter
 *
 * @param  reverse: Select embedded flash interface reverse
 * @param  swapIo3Io0: Select embedded flash swap io0 with io3
 * @param  swapIo2Cs: Select embedded flash swap cs with io2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION GLB_Set_Embedded_FLash_IO_PARM(uint8_t reverse, uint8_t swapIo3Io0, uint8_t swapIo2Cs)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CFG_SFLASH_REVERSE, reverse);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CFG_SFLASH_SWAP_IO0_IO3, swapIo3Io0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CFG_SFLASH_SWAP_CS_IO2, swapIo2Cs);

    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  set MTimer clock
 *
 * @param  enable: enable or disable MTimer clock
 * @param  clkSel: clock selection
 * @param  div: divider, 0~0x1F stand for 1~0x20 divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_MTimer_CLK(uint8_t enable, GLB_MTIMER_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_MTIMER_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    /* disable MTimer clock first */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CPU_CLK_CFG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CPU_RTC_EN);
    BL_WR_REG(GLB_BASE, GLB_CPU_CLK_CFG, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CPU_CLK_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CPU_RTC_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CPU_RTC_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_CPU_CLK_CFG, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CPU_CLK_CFG);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CPU_RTC_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CPU_RTC_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CPU_CLK_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set ADC clock
 *
 * @param  enable: enable or disable ADC clock
 * @param  clkSel: ADC clock selection
 * @param  div: divider, 0~63 stand for 1~64 divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_ADC_CLK_SRC_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3F));

    /* disable ADC clock first */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_GPADC_32M_DIV_EN);
    BL_WR_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_GPADC_32M_DIV_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_GPADC_32M_DIV_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set DIG 32K clock
 *
 * @param  enable: enable or disable DIG 32K clock
 * @param  compensation: enable or disable DIG 32K clock compensation
 * @param  clkSel: clock selection
 * @param  div: divider, 2 ~ 0x7FF
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensation, GLB_DIG_CLK_SRC_Type clkSel, uint16_t div)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_DIG_CLK_SRC_TYPE(clkSel));
    CHECK_PARAM((div <= 0x7FF));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);
    if (compensation) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_DIG_32K_COMP);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DIG_32K_COMP);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DIG_32K_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DIG_CLK_SRC_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_DIG_32K_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DIG_32K_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  SW wakeup BLE even BLE in infinite sleep set
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_SW_BLE_WAKEUP_REQ_Set(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SW_BLE_WAKEUP_REQ);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SW_BLE_WAKEUP_REQ);
    }
    BL_WR_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select UART signal function
 *
 * @param  sig: UART signal
 * @param  fun: UART function
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_UART_Fun_Sel(GLB_UART_SIG_Type sig, GLB_UART_SIG_FUN_Type fun)
{
    uint32_t sig_pos = 0;
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_UART_SIG_TYPE(sig));
    CHECK_PARAM(IS_GLB_UART_SIG_FUN_TYPE(fun));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_UART_SIG_SEL_0);
    sig_pos = (sig * 4);
    /* Clear original val */
    tmpVal &= (~(0xf << sig_pos));
    /* Set new value */
    tmpVal |= (fun << sig_pos);
    BL_WR_REG(GLB_BASE, GLB_UART_SIG_SEL_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power off DLL
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_Off_DLL(void)
{
    uint32_t tmpVal = 0;

    /* GLB->dll.BF.ppu_dll = 0;   */
    /* GLB->dll.BF.pu_dll = 0;    */
    /* GLB->dll.BF.dll_reset = 1; */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PPU_DLL, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_DLL, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_RESET, 1);
    BL_WR_REG(GLB_BASE, GLB_DLL, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  power on DLL
 *
 * @param  xtalType: DLL xtal type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_On_DLL(GLB_DLL_XTAL_Type xtalType)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_DLL_XTAL_TYPE(xtalType));

    /* GLB->dll.BF.dll_refclk_sel = XXX; */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL);
    switch (xtalType) {
        case GLB_DLL_XTAL_NONE:
            return ERROR;
        case GLB_DLL_XTAL_32M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_REFCLK_SEL, 0);
            break;
        case GLB_DLL_XTAL_RC32M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_REFCLK_SEL, 1);
            break;
        default:
            break;
    }
    BL_WR_REG(GLB_BASE, GLB_DLL, tmpVal);

    /* GLB->dll.BF.dll_prechg_sel = 1;   */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_PRECHG_SEL, 1);
    BL_WR_REG(GLB_BASE, GLB_DLL, tmpVal);

    /* GLB->dll.BF.ppu_dll = 1;   */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PPU_DLL, 1);
    BL_WR_REG(GLB_BASE, GLB_DLL, tmpVal);

    BL702L_Delay_US(2);

    /* GLB->dll.BF.pu_dll = 1; */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_DLL, 1);
    BL_WR_REG(GLB_BASE, GLB_DLL, tmpVal);

    BL702L_Delay_US(2);

    /* GLB->dll.BF.dll_reset = 0; */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_RESET, 0);
    BL_WR_REG(GLB_BASE, GLB_DLL, tmpVal);

    /* delay for settling */
    BL702L_Delay_US(5);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  enable all DLL output clock
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION GLB_Enable_DLL_All_Clks(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV1_RF, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV63, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV21, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV5, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV3, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV2, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV1, 1);
    BL_WR_REG(GLB_BASE, GLB_DLL2, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  enable one of DLL output clock
 *
 * @param  dllClk: None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION GLB_Enable_DLL_Clk(GLB_DLL_CLK_Type dllClk)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_DLL_CLK_TYPE(dllClk));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL2);
    switch (dllClk) {
        case GLB_DLL_CLK_RF:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV1_RF, 1);
            break;
        case GLB_DLL_CLK_2P032M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV63, 1);
            break;
        case GLB_DLL_CLK_6P095M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV21, 1);
            break;
        case GLB_DLL_CLK_25P6M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV5, 1);
            break;
        case GLB_DLL_CLK_42P67M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV3, 1);
            break;
        case GLB_DLL_CLK_64M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV2, 1);
            break;
        case GLB_DLL_CLK_128M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV1, 1);
            break;
        default:
            break;
    }
    BL_WR_REG(GLB_BASE, GLB_DLL2, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  disable all DLL output clock
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION GLB_Disable_DLL_All_Clks(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV1_RF, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV63, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV21, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV5, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV3, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV2, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV1, 0);
    BL_WR_REG(GLB_BASE, GLB_DLL2, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  disable one of DLL output clock
 *
 * @param  dllClk: None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION GLB_Disable_DLL_Clk(GLB_DLL_CLK_Type dllClk)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_DLL_CLK_TYPE(dllClk));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DLL2);
    switch (dllClk) {
        case GLB_DLL_CLK_RF:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV1_RF, 0);
            break;
        case GLB_DLL_CLK_2P032M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV63, 0);
            break;
        case GLB_DLL_CLK_6P095M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV21, 0);
            break;
        case GLB_DLL_CLK_25P6M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV5, 0);
            break;
        case GLB_DLL_CLK_42P67M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV3, 0);
            break;
        case GLB_DLL_CLK_64M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV2, 0);
            break;
        case GLB_DLL_CLK_128M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DLL_EN_DIV1, 0);
            break;

        default:
            break;
    }
    BL_WR_REG(GLB_BASE, GLB_DLL2, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  set flash id value
 *
 * @param  idValue: flash id value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Set_Flash_Id_Value(uint32_t idValue)
{
    BL_WR_REG(GLB_BASE, GLB_RSV0, (idValue | BFLB_FLASH_ID_VALID_FLAG));

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get flash id value
 *
 * @param  None
 *
 * @return flash id
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION GLB_Get_Flash_Id_Value(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_RSV0);
    if ((tmpVal & BFLB_FLASH_ID_VALID_FLAG) != 0) {
        return (tmpVal & BFLB_FLASH_ID_VALID_MASK);
    }

    return 0x00000000;
}

/****************************************************************************/ /**
 * @brief  Trim RC32M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Trim_RC32M(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;

    bflb_ef_ctrl_read_common_trim(NULL, "rc32m", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            tmpVal = BL_RD_REG(GLB_BASE, GLB_RC32M_CTRL0);
            tmpVal = BL_SET_REG_BIT(tmpVal, GLB_RC32M_EXT_CODE_EN);
            BL_WR_REG(GLB_BASE, GLB_RC32M_CTRL0, tmpVal);
            arch_delay_us(2);
            tmpVal = BL_RD_REG(GLB_BASE, GLB_RC32M_CTRL1);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_RC32M_CODE_FR_EXT2, trim.value);
            BL_WR_REG(GLB_BASE, GLB_RC32M_CTRL1, tmpVal);
            tmpVal = BL_RD_REG(GLB_BASE, GLB_RC32M_CTRL1);
            tmpVal = BL_SET_REG_BIT(tmpVal, GLB_RC32M_EXT_CODE_SEL);
            BL_WR_REG(GLB_BASE, GLB_RC32M_CTRL1, tmpVal);
            /* hw_5T + sw_5T  */
            arch_delay_us(1);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Set xtal_cnt_32k_process(0x4000E04C[29])
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Xtal_Cnt32k_Process(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_XTAL_CNT_32K_SW_TRIG_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CLR_XTAL_CNT_32K_DONE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_START_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_END_PS);
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Clear xtal_cnt_32k_done(0x4000E04C[30])
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Clear_Xtal_Cnt32k_Done(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_XTAL_CNT_32K_SW_TRIG_PS);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CLR_XTAL_CNT_32K_DONE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_START_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_END_PS);
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB start RC32K deg
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_RC32K_Deg_Start(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_XTAL_CNT_32K_SW_TRIG_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CLR_XTAL_CNT_32K_DONE);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_RC32K_DEG_START_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_END_PS);
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB end RC32K deg
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_RC32K_Deg_End(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_XTAL_CNT_32K_SW_TRIG_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CLR_XTAL_CNT_32K_DONE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_START_PS);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_RC32K_DEG_END_PS);
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB enable or disable RC32K deg
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_RC32K_Deg_Enable(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_RC32K_DEG_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_RC32K_DEG_EN);
    }
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_XTAL_CNT_32K_SW_TRIG_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CLR_XTAL_CNT_32K_DONE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_START_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_END_PS);
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB set XTAL deg cnt limit value
 *
 * @param  cnt: 0~255
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Xtal_Deg_Cnt_Limit_Set(uint8_t cnt)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_XTAL_DEG_CNT_LIMIT, cnt);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_XTAL_CNT_32K_SW_TRIG_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CLR_XTAL_CNT_32K_DONE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_START_PS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32K_DEG_END_PS);
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable ir led driver
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_IR_LED_Driver_Enable(void)
{
    uint32_t tmpVal = 0;

    /* Enable led driver */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_LED_DRIVER);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_PU_LEDDRV);
    BL_WR_REG(GLB_BASE, GLB_LED_DRIVER, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable ir led driver
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_IR_LED_Driver_Disable(void)
{
    uint32_t tmpVal = 0;

    /* Disable led driver */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_LED_DRIVER);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PU_LEDDRV);
    BL_WR_REG(GLB_BASE, GLB_LED_DRIVER, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable ir led driver gpio output
 *
 * @param  gpio: IR gpio selected
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_IR_LED_Driver_Output_Enable(GLB_IR_LED_Type led)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_IR_LED_TYPE(led));

    if (led == GLB_IR_LED0) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_LED_DRIVER);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LEDDRV_OUT_EN, BL_GET_REG_BITS_VAL(tmpVal, GLB_LEDDRV_OUT_EN) | 1);
        BL_WR_REG(GLB_BASE, GLB_LED_DRIVER, tmpVal);
    } else if (led == GLB_IR_LED1) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_LED_DRIVER);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LEDDRV_OUT_EN, BL_GET_REG_BITS_VAL(tmpVal, GLB_LEDDRV_OUT_EN) | 2);
        BL_WR_REG(GLB_BASE, GLB_LED_DRIVER, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable ir led driver gpio output
 *
 * @param  gpio: IR gpio selected
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_IR_LED_Driver_Output_Disable(GLB_IR_LED_Type led)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_IR_LED_TYPE(led));

    if (led == GLB_IR_LED0) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_LED_DRIVER);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LEDDRV_OUT_EN, BL_GET_REG_BITS_VAL(tmpVal, GLB_LEDDRV_OUT_EN) & ~1);
        BL_WR_REG(GLB_BASE, GLB_LED_DRIVER, tmpVal);
    } else if (led == GLB_IR_LED1) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_LED_DRIVER);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LEDDRV_OUT_EN, BL_GET_REG_BITS_VAL(tmpVal, GLB_LEDDRV_OUT_EN) & ~2);
        BL_WR_REG(GLB_BASE, GLB_LED_DRIVER, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set ir led driver ibias
 *
 * @param  ibias: Ibias value,0x0:0mA~0xf:120mA,8mA/step
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_IR_LED_Driver_Ibias(uint8_t ibias)
{
    uint32_t tmpVal = 0;

    /* Set driver ibias */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_LED_DRIVER);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LEDDRV_IBIAS, ibias & 0x7);
    BL_WR_REG(GLB_BASE, GLB_LED_DRIVER, tmpVal);

    return SUCCESS;
}

/*@} end of group GLB_Public_Functions */

/*@} end of group GLB */

/*@} end of group BL702L_Peripheral_Driver */
