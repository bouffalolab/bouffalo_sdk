/**
  ******************************************************************************
  * @file    bl616d_clock.c
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

#include "bl616d_clock.h"

/** @addtogroup  BL616D_Peripheral_Driver
 *  @{
 */

/** @addtogroup  CLOCK
 *  @{
 */

/** @defgroup  CLOCK_Private_Macros
 *  @{
 */

/*@} end of group CLOCK_Private_Macros */

/** @defgroup  CLOCK_Private_Types
 *  @{
 */

/*@} end of group CLOCK_Private_Types */

/** @defgroup  CLOCK_Private_Variables
 *  @{
 */

/*@} end of group CLOCK_Private_Variables */

/** @defgroup  CLOCK_Global_Variables
 *  @{
 */

/*@} end of group CLOCK_Global_Variables */

/** @defgroup  CLOCK_Private_Fun_Declaration
 *  @{
 */

/*@} end of group CLOCK_Private_Fun_Declaration */

/** @defgroup  CLOCK_Private_Functions
 *  @{
 */

/*@} end of group CLOCK_Private_Functions */

/** @defgroup  CLOCK_Public_Functions
 *  @{
 */

static inline uint32_t ATTR_CLOCK_SECTION Clock_Get_WIFI_PLL_Output(uint32_t pllOut)
{
    return pllOut;
}

static uint32_t ATTR_CLOCK_SECTION Clock_Get_CPUPLL_Output(CLOCK_CPUPLL_Type Type)
{
    uint32_t vcoFreq;
    uint32_t tmpVal, refdiv, sdmin, xtal_value;
    HBN_Get_Xtal_Value(&xtal_value);
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET);
    refdiv = BL_GET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_REFCLK_DIV_RATIO);
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET);
    sdmin = BL_GET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_SDM_IN);
    float vco_float = (float)sdmin / 2048.0f * ((float)xtal_value / (float)refdiv);
    vcoFreq = (uint32_t)vco_float;

    switch (Type) {
        case CLOCK_CPUPLL_DIV1:
            return (vcoFreq);
        case CLOCK_CPUPLL_DIV2:
            return (vcoFreq / 2);
        case CLOCK_CPUPLL_DIV3:
            return (vcoFreq / 3);
        case CLOCK_CPUPLL_DIV4:
            return (vcoFreq / 4);
        case CLOCK_CPUPLL_DIV5:
            return (vcoFreq / 5);
        case CLOCK_CPUPLL_DIV6:
            return (vcoFreq / 6);
        case CLOCK_CPUPLL_DIV8:
            return (vcoFreq / 8);
        case CLOCK_CPUPLL_DIV10:
            return (vcoFreq / 10);
        case CLOCK_CPUPLL_DIV12:
            return (vcoFreq / 12);
        case CLOCK_CPUPLL_DIV20:
            return (vcoFreq / 20);
        case CLOCK_CPUPLL_DIV30:
            return (vcoFreq / 30);
        default:
            return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_Xtal_Output(void)
{
    uint8_t xtalType = 0;

    if (HBN_Get_Xtal_Type(&xtalType) == SUCCESS) {
        switch (xtalType) {
            case GLB_XTAL_NONE:
                return 0;
            case GLB_XTAL_24M:
                return 24 * 1000 * 1000;
            case GLB_XTAL_32M:
                return 32 * 1000 * 1000;
            case GLB_XTAL_38P4M:
                return 38.4 * 1000 * 1000;
            case GLB_XTAL_40M:
                return 40 * 1000 * 1000;
            case GLB_XTAL_26M:
                return 26 * 1000 * 1000;
            case GLB_XTAL_RC32M:
                return 32 * 1000 * 1000;
            default:
                return 0;
        }
    } else {
        return 40 * 1000 * 1000;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_XClk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* rc32m */
        return (32 * 1000 * 1000);
    } else if (sel == 1) {
        /* xtal */
        return Clock_Xtal_Output();
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_F32k_Mux_Output(uint8_t sel)
{
    uint32_t tmpVal;
    uint32_t div = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    div = BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_32K_DIV);

    if (sel == 0) {
        /* src32k */
        return (32 * 1000);
    } else if (sel == 1) {
        /* xtal 32K */
        return (32 * 1000);
    } else {
        return Clock_Xtal_Output() / (div + 1);
    }
}

static inline uint8_t ATTR_CLOCK_SECTION Clock_Get_F32k_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);

    return BL_GET_REG_BITS_VAL(tmpVal, HBN_F32K_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_160M_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 160m */
        return Clock_Get_WIFI_PLL_Output(160 * 1000 * 1000);
    } else {
        return 0;
    }
}

static inline uint8_t ATTR_CLOCK_SECTION Clock_Get_Muxpll_160M_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_TOP_MUXPLL_160M_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_80M_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 80m */
        return Clock_Get_WIFI_PLL_Output(80 * 1000 * 1000);
    } else {
        return 0;
    }
}

static inline uint8_t ATTR_CLOCK_SECTION Clock_Get_Muxpll_80M_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_TOP_MUXPLL_80M_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_MCU_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* cpupll div2 clk */
        return Clock_Get_CPUPLL_Output(CLOCK_CPUPLL_DIV2);
    } else if (sel == 1) {
        /* cpupll div3 clk */
        return Clock_Get_CPUPLL_Output(CLOCK_CPUPLL_DIV3);
    } else if (sel == 2) {
        /* wifi pll 240m */
        return Clock_Get_WIFI_PLL_Output(240 * 1000 * 1000);
    } else if (sel == 3) {
        /* wifi pll 320m */
        return Clock_Get_WIFI_PLL_Output(320 * 1000 * 1000);
    } else {
        return 0;
    }
}

static inline uint8_t ATTR_CLOCK_SECTION Clock_Get_MCU_XClk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);

    return (tmpVal & 0x1);
}

static inline uint8_t ATTR_CLOCK_SECTION Clock_Get_WL_XClk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_WL_REG_XCLK_CLK_SEL);

    return (tmpVal & 0x1);
}

static inline uint8_t ATTR_CLOCK_SECTION Clock_Get_MCU_Root_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);

    return ((tmpVal >> 1) & 0x1);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_MCU_Root_Clk_Mux_Output(uint8_t sel)
{
    uint32_t tmpVal;

    if (sel == 0) {
        /* xclk */
        return Clock_XClk_Mux_Output(Clock_Get_MCU_XClk_Sel_Val());
    } else if (sel == 1) {
        /* pll */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
        sel = BL_GET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL);
        return Clock_MCU_Clk_Mux_Output(sel);
    } else {
        return 0;
    }
}

static inline uint8_t ATTR_CLOCK_SECTION Clock_Get_MCU_HClk_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_MAIN_FCLK_DIV);
}

static inline uint8_t ATTR_CLOCK_SECTION Clock_Get_Peri_BClk_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV);
}

/****************************************************************************/ /**
 * @brief  Get System Clock
 *
 * @param  type: System clock type
 *
 * @return System clock value
 *
*******************************************************************************/
uint32_t ATTR_CLOCK_SECTION Clock_System_Clock_Get(BL_System_Clock_Type type)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    switch (type) {
        case BL_SYSTEM_CLOCK_MCU_ROOT_CLK:
            /*!< mcu root clock */
            return Clock_MCU_Root_Clk_Mux_Output(Clock_Get_MCU_Root_Clk_Sel_Val());

        case BL_SYSTEM_CLOCK_MCU_CLK:
            /*!< mcu fast clock/cpu clock */
            clock = Clock_MCU_Root_Clk_Mux_Output(Clock_Get_MCU_Root_Clk_Sel_Val());
            div = Clock_Get_MCU_HClk_Div_Val();
            return clock / (div + 1);

        case BL_SYSTEM_CLOCK_MCU_BCLK:
            /*!< mcu bus clock */
            ATTR_FALLTHROUGH();

        case BL_SYSTEM_CLOCK_MCU_PBCLK:
            /*!< mcu peri bus clock */
            clock = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK);
            div = Clock_Get_Peri_BClk_Div_Val();
            return clock / (div + 1);

        case BL_SYSTEM_CLOCK_F32K:
            /*!< f32k clock */
            return Clock_F32k_Mux_Output(Clock_Get_F32k_Sel_Val());

        case BL_SYSTEM_CLOCK_XCLK:
            /*!< xclk:rc32M or xtal */
            return Clock_XClk_Mux_Output(Clock_Get_MCU_XClk_Sel_Val());

        case BL_SYSTEM_CLOCK_WL_XCLK:
            /*!< wifi xclk */
            return Clock_XClk_Mux_Output(Clock_Get_WL_XClk_Sel_Val());

        case BL_SYSTEM_CLOCK_XTAL:
            /*!< xtal clock */
            return Clock_Xtal_Output();

        default:
            return 0;
    }
}

static inline uint32_t Clock_PARAMB_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* bclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);
    } else if (sel == 1) {
        /* wifi pll 480m */
        return Clock_Get_WIFI_PLL_Output(480 * 1000 * 1000);
    } else if (sel == 2) {
        /* wifi pll 320m */
        return Clock_Get_WIFI_PLL_Output(320 * 1000 * 1000);
    } else if (sel == 3) {
        /* wifi pll 240m */
        return Clock_Get_WIFI_PLL_Output(240 * 1000 * 1000);
    } else {
        return 0;
    }
}

static inline uint8_t Clock_Get_PSRAMB_Clk_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_PSRAM_CLK_DIV);
}

static inline uint8_t Clock_Get_PSRAMB_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_PSRAM_CLK_SEL);
}

static inline uint32_t Clock_Get_PSRAMB_Clk(void)
{
    uint32_t div = 0;

    div = Clock_Get_PSRAMB_Clk_Div_Val();

    return Clock_PARAMB_Clk_Mux_Output(Clock_Get_PSRAMB_Clk_Sel_Val()) / (div + 1);
}

static inline uint32_t Clock_UART_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mcu bclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);
    } else if (sel == 1) {
        /* mux 160Mkz */
        return Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val());
    } else {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    }
}

static inline uint8_t Clock_Get_UART_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_UART_CFG0);

    return ((BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_UART_CLK_SEL2) << 1) | BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_UART_CLK_SEL));
}

static inline uint8_t Clock_Get_UART_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_UART_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_UART_CLK_DIV);
}

static inline uint32_t Clock_PKA_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mcu pbclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);
    } else if (sel == 1) {
        /* mux 160m */
        return Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val());
    } else {
        return 0;
    }
}

static inline uint8_t Clock_Get_PKA_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_PKA_CLK_SEL);
}

static inline uint32_t Clock_IR_Clk_Mux_Output(void)
{
    /* xclk */
    return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
}

static inline uint8_t Clock_Get_IR_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_IR_CLK_DIV);
}

static inline uint8_t Clock_Get_SF_Clk_Sel2_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL2);
}

static inline uint32_t Clock_SF_SEL2_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 120m */
        return Clock_Get_WIFI_PLL_Output(120 * 1000 * 1000);
    } else if (sel == 1) {
        /* xtal */
        return Clock_Xtal_Output();
    } else if (sel == 2) {
        /* wifi pll 120m */
        return Clock_Get_WIFI_PLL_Output(120 * 1000 * 1000);
    } else {
        /* cpupll div5 clk */
        return Clock_Get_CPUPLL_Output(CLOCK_CPUPLL_DIV5);
    }
}

static inline uint32_t Clock_SF_SEL_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* sf sel2 */
        return Clock_SF_SEL2_Clk_Mux_Output(Clock_Get_SF_Clk_Sel2_Val());
    } else if (sel == 1) {
        /* mux 80m */
        return Clock_80M_Clk_Mux_Output(Clock_Get_Muxpll_80M_Sel_Val());
    } else if (sel == 2) {
        /* mcu pbclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);
    } else {
        /* wifi 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    }
}

static inline uint8_t Clock_Get_SF_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL);
}

static inline uint8_t Clock_Get_SF_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV);
}

static inline uint32_t Clock_SPI_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mux 160Mkz */
        return Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val());
    } else {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    }
}

static inline uint8_t Clock_Get_SPI_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_SEL);
}

static inline uint8_t Clock_Get_SPI1_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1);

    return BL_GET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_SPI_CLK_SEL);
}

static inline uint8_t Clock_Get_SPI_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_DIV);
}

static inline uint8_t Clock_Get_SPI1_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1);

    return BL_GET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_SPI_CLK_DIV);
}

static inline uint32_t Clock_DBI_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mux 160Mkz */
        return Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val());
    } else {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    }
}

static inline uint8_t Clock_Get_DBI_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DBI_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_DBI_CLK_SEL);
}

static inline uint8_t Clock_Get_DBI_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DBI_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_DBI_CLK_DIV);
}

static inline uint32_t Clock_I2C_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mcu pblck */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);
    } else if (sel == 1) {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    } else {
        return 0;
    }
}

static inline uint8_t Clock_Get_I2C_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2C_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_SEL);
}

static inline uint8_t Clock_Get_I2C_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2C_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV);
}

static inline uint32_t Clock_GPADC_Clk_Output(uint8_t sel)
{
    if (sel == 0) {
        /* cpu pll clk */
        return Clock_Get_CPUPLL_Output(CLOCK_CPUPLL_DIV1);
    } else {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    }
}

static inline uint8_t Clock_Get_GPADC_32M_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ADC_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_SEL);
}

static inline uint8_t Clock_Get_GPADC_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ADC_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_DIV);
}

static inline uint32_t Clock_GPADC_Clk_Mux_Output(uint8_t sel)
{
    uint32_t div = 0;

    if (sel == 1) {
        /* f32k bclk */
        return Clock_F32k_Mux_Output(Clock_Get_F32k_Sel_Val());
    } else {
        div = Clock_Get_GPADC_Div_Val();
        return Clock_GPADC_Clk_Output(Clock_Get_GPADC_32M_Clk_Sel_Val()) / (div + 1);
    }
}

static inline uint32_t Clock_I2S_Clk_Mux_Output(void)
{
    /* WIFI pll clk */
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(RF_BASE, RF_ANA1_WIFIPLL_CLKTREE_RF);

    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_POSTDIV);

    return Clock_Get_WIFI_PLL_Output(960 * 1000 * 1000) / tmpVal;
}

static inline uint8_t Clock_Get_I2S_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2S_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_I2S_REF_CLK_DIV);
}

static inline uint32_t Clock_ADC_Clk_Mux_Output(void)
{
    /* CPU pll clk */
    return Clock_Get_CPUPLL_Output(CLOCK_CPUPLL_DIV1);
}

static inline uint8_t Clock_Get_ADC_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG1);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_SOLO_CLK_DIV);
}

static inline uint8_t Clock_Get_GPADC_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);

    return BL_GET_REG_BITS_VAL(tmpVal, HBN_GPADC_CS);
}

static inline uint32_t Clock_GPDAC_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 32m */
        return Clock_Get_WIFI_PLL_Output(32 * 1000 * 1000);
    } else if (sel == 1) {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    } else {
        /* CPU pll clk */
        return Clock_Get_CPUPLL_Output(CLOCK_CPUPLL_DIV1);
    }
}

static inline uint8_t Clock_Get_GPDAC_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_512K_DIV);
}

static inline uint8_t Clock_Get_GPDAC_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_CLK_SRC_SEL);
}

static inline uint32_t Clock_SDH_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    } else {
        /* cpupll div5 clk */
        return Clock_Get_CPUPLL_Output(CLOCK_CPUPLL_DIV5);
    }
}

static inline uint8_t Clock_Get_SDH_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDH_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_SDH_CLK_SEL);
}

static inline uint8_t Clock_Get_SDH_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDH_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_SDH_CLK_DIV);
}

static inline uint32_t Clock_CAM_REF_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    } else if (sel == 1) {
        /* wifi pull 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    } else if ((sel == 2) || (sel == 3)) {
        /* cpupll div5 clk */
        return Clock_Get_CPUPLL_Output(CLOCK_CPUPLL_DIV5);
    } else {
        return 0;
    }
}

static inline uint8_t Clock_Get_CAM_REF_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CAM_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_CAM_REF_CLK_SRC_SEL);
}

static inline uint8_t Clock_Get_CAM_REF_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CAM_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_CAM_REF_CLK_DIV);
}

static inline uint32_t Clock_CAM_Clk_Mux_Output(void)
{
    /* mcu bclk */
    return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);
}

/****************************************************************************/ /**

 * @brief  Get Audio ADC Clock
 *
 * @return Audio ADC clock value
 *
*******************************************************************************/
static inline uint32_t Clock_Audio_ADC_Clock_Get(void)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    clock = Clock_ADC_Clk_Mux_Output();
    div = Clock_Get_ADC_Div_Val();

    return clock / (div + 1);
}

/****************************************************************************/ /**

 * @brief  Get RTC Clock
 *
 * @return RTC clock value
 *
*******************************************************************************/
static inline uint32_t Clock_RTC_Clock_Get(void)
{
    return 32768;
}

static inline uint32_t Clock_PEC_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mux 160Mkz */
        return Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val());
    } else {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    }
}

static inline uint8_t Clock_Get_PEC_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_PEC_CLK_SEL);
}

static inline uint8_t Clock_Get_PEC_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_PEC_CLK_DIV);
}

static inline uint32_t Clock_TIMER_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* BCLK */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_BCLK);
    } else if (sel == 1) {
        /* F32K clock */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_F32K);
    } else if (sel == 2) {
        /* 1K clock */
        return (Clock_System_Clock_Get(BL_SYSTEM_CLOCK_F32K) / 32);
    } else if (sel == 3) {
        /* XTAL clock */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XTAL);
    } else {
        return 0;
    }
}

static inline uint8_t Clock_Get_TIMER0_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);

    return ((tmpVal & TIMER_CS_0_MASK) >> TIMER_CS_0_SHIFT);
}

static inline uint8_t Clock_Get_TIMER0_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);

    return ((tmpVal & TIMER_TCDR0_MASK) >> TIMER_TCDR0_SHIFT);
}

static inline uint8_t Clock_Get_TIMER1_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);

    return ((tmpVal & TIMER_CS_1_MASK) >> TIMER_CS_1_SHIFT);
}

static inline uint8_t Clock_Get_TIMER1_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);

    return ((tmpVal & TIMER_TCDR1_MASK) >> TIMER_TCDR1_SHIFT);
}

static inline uint8_t Clock_Get_TIMER2_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(TIMER1_BASE, TIMER_TCCR);

    return ((tmpVal & TIMER_CS_0_MASK) >> TIMER_CS_0_SHIFT);
}

static inline uint8_t Clock_Get_TIMER2_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(TIMER1_BASE, TIMER_TCDR);

    return ((tmpVal & TIMER_TCDR0_MASK) >> TIMER_TCDR0_SHIFT);
}

static inline uint8_t Clock_Get_TIMER3_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(TIMER1_BASE, TIMER_TCCR);

    return ((tmpVal & TIMER_CS_1_MASK) >> TIMER_CS_1_SHIFT);
}

static inline uint8_t Clock_Get_TIMER3_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(TIMER1_BASE, TIMER_TCDR);

    return ((tmpVal & TIMER_TCDR1_MASK) >> TIMER_TCDR1_SHIFT);
}

static inline uint32_t Clock_PWM_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    } else if (sel == 1) {
        /* BCLK */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_BCLK);
    } else if (sel == 2 || sel == 3) {
        /* F32K clock */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_F32K);
    } else {
        return 0;
    }
}

static inline uint8_t Clock_Get_PWM_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(PWM_BASE, PWM_MC0_CONFIG0);

    return ((tmpVal & PWM_REG_CLK_SEL_MASK) >> PWM_REG_CLK_SEL_SHIFT);
}

static inline uint8_t Clock_Get_PWM_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(PWM_BASE, PWM_MC0_CONFIG0);

    return ((tmpVal & PWM_CLK_DIV_MASK) >> PWM_CLK_DIV_SHIFT);
}

static inline uint8_t Clock_Get_PWM1_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(PWM1_BASE, PWM_MC0_CONFIG0);

    return ((tmpVal & PWM_REG_CLK_SEL_MASK) >> PWM_REG_CLK_SEL_SHIFT);
}

static inline uint8_t Clock_Get_PWM1_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(PWM1_BASE, PWM_MC0_CONFIG0);

    return ((tmpVal & PWM_CLK_DIV_MASK) >> PWM_CLK_DIV_SHIFT);
}

/****************************************************************************/ /**

 * @brief  Get Peripheral1 Clock
 *
 * @param  type: Peripheral1 clock type
 *
 * @return Peripheral1 clock value
 *
*******************************************************************************/
uint32_t Clock_Peripheral_Clock_Get(BL_Peripheral_Type type)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    switch (type) {
        /*!< uart0 clock */
        case BL_PERIPHERAL_CLOCK_UART0:
            ATTR_FALLTHROUGH();

        /*!< uart1 clock */
        case BL_PERIPHERAL_CLOCK_UART1:
            ATTR_FALLTHROUGH();

        /*!< uart2 clock */
        case BL_PERIPHERAL_CLOCK_UART2:
            ATTR_FALLTHROUGH();

        /*!< uart3 clock */
        case BL_PERIPHERAL_CLOCK_UART3:
            clock = Clock_UART_Clk_Mux_Output(Clock_Get_UART_Clk_Sel_Val());
            div = Clock_Get_UART_Div_Val();
            return clock / (div + 1);

        /*!< spi0 clock */
        case BL_PERIPHERAL_CLOCK_SPI0:
            clock = Clock_SPI_Clk_Mux_Output(Clock_Get_SPI_Clk_Sel_Val());
            div = Clock_Get_SPI_Div_Val();
            return clock / (div + 1);

        /*!< spi1 clock */
        case BL_PERIPHERAL_CLOCK_SPI1:
            clock = Clock_SPI_Clk_Mux_Output(Clock_Get_SPI1_Clk_Sel_Val());
            div = Clock_Get_SPI1_Div_Val();
            return clock / (div + 1);

        /*!< pec clock */
        case BL_PERIPHERAL_CLOCK_PEC:
            clock = Clock_PEC_Clk_Mux_Output(Clock_Get_PEC_Clk_Sel_Val());
            div = Clock_Get_PEC_Div_Val();
            return clock / (div + 1);

        /*!< dbi clock */
        case BL_PERIPHERAL_CLOCK_DBI:
            clock = Clock_DBI_Clk_Mux_Output(Clock_Get_DBI_Clk_Sel_Val());
            div = Clock_Get_DBI_Div_Val();
            return clock / (div + 1);

        /*!< i2c0 clock */
        case BL_PERIPHERAL_CLOCK_I2C0:
            ATTR_FALLTHROUGH();

        /*!< i2c1 clock */
        case BL_PERIPHERAL_CLOCK_I2C1:
            ATTR_FALLTHROUGH();

        /*!< i2c2 clock */
        case BL_PERIPHERAL_CLOCK_I2C2:
            clock = Clock_I2C_Clk_Mux_Output(Clock_Get_I2C_Clk_Sel_Val());
            div = Clock_Get_I2C_Div_Val();
            return clock / (div + 1);

        /*!< PSRAMB clock */
        case BL_PERIPHERAL_CLOCK_PSRAMB:
            return Clock_Get_PSRAMB_Clk();

        /*!< emi clock */
        case BL_PERIPHERAL_CLOCK_EMI:
            /* mcu pblck */
            return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);

        /*!< FLASH clock */
        case BL_PERIPHERAL_CLOCK_FLASH:
            clock = Clock_SF_SEL_Clk_Mux_Output(Clock_Get_SF_Clk_Sel_Val());
            div = Clock_Get_SF_Div_Val();
            return clock / (div + 1);

        /*!< I2S clock */
        case BL_PERIPHERAL_CLOCK_I2S:
            clock = Clock_I2S_Clk_Mux_Output();
            div = Clock_Get_I2S_Div_Val();
            return clock / (div + 1);

        /*!< IR clock */
        case BL_PERIPHERAL_CLOCK_IR:
            clock = Clock_IR_Clk_Mux_Output();
            div = Clock_Get_IR_Div_Val();
            return clock / (div + 1);

        /*!< ADC clock */
        case BL_PERIPHERAL_CLOCK_AUADC:
            return Clock_Audio_ADC_Clock_Get();

        /*!< GPADC clock */
        case BL_PERIPHERAL_CLOCK_GPADC:
            return Clock_GPADC_Clk_Mux_Output(Clock_Get_GPADC_Clk_Sel_Val());

        /*!< GPDAC clock */
        case BL_PERIPHERAL_CLOCK_GPDAC:
            clock = Clock_GPDAC_Clk_Mux_Output(Clock_Get_GPDAC_Clk_Sel_Val());
            div = Clock_Get_GPDAC_Div_Val();
            return clock / (div + 1);

        /*!< CAM_REF clock */
        case BL_PERIPHERAL_CLOCK_CAM_REF:
            clock = Clock_CAM_REF_Clk_Mux_Output(Clock_Get_CAM_REF_Clk_Sel_Val());
            div = Clock_Get_CAM_REF_Div_Val();
            return clock / (div + 1);

        /*!< CAM clock */
        case BL_PERIPHERAL_CLOCK_CAM:
            return Clock_CAM_Clk_Mux_Output();

        /*!< SDH clock */
        case BL_PERIPHERAL_CLOCK_SDH:
            clock = Clock_SDH_Clk_Mux_Output(Clock_Get_SDH_Clk_Sel_Val());
            div = Clock_Get_SDH_Div_Val();
            return clock / (div + 1);

        /*!< PKA clock */
        case BL_PERIPHERAL_CLOCK_SEC_PKA:
            return Clock_PKA_Clk_Mux_Output(Clock_Get_PKA_Clk_Sel_Val());

        /*!< RTC clock */
        case BL_PERIPHERAL_CLOCK_RTC:
            return Clock_RTC_Clock_Get();

        /*!< timer0 clock */
        case BL_PERIPHERAL_CLOCK_TIMER0:
            clock = Clock_TIMER_Clk_Mux_Output(Clock_Get_TIMER0_Clk_Sel_Val());
            div = Clock_Get_TIMER0_Div_Val();
            return clock / (div + 1);

        /*!< timer1 clock */
        case BL_PERIPHERAL_CLOCK_TIMER1:
            clock = Clock_TIMER_Clk_Mux_Output(Clock_Get_TIMER1_Clk_Sel_Val());
            div = Clock_Get_TIMER1_Div_Val();
            return clock / (div + 1);

        /*!< timer2 clock */
        case BL_PERIPHERAL_CLOCK_TIMER2:
            clock = Clock_TIMER_Clk_Mux_Output(Clock_Get_TIMER2_Clk_Sel_Val());
            div = Clock_Get_TIMER2_Div_Val();
            return clock / (div + 1);

        /*!< timer3 clock */
        case BL_PERIPHERAL_CLOCK_TIMER3:
            clock = Clock_TIMER_Clk_Mux_Output(Clock_Get_TIMER3_Clk_Sel_Val());
            div = Clock_Get_TIMER3_Div_Val();
            return clock / (div + 1);

        /*!< pwm0 clock */
        case BL_PERIPHERAL_CLOCK_PWM0:
            clock = Clock_PWM_Clk_Mux_Output(Clock_Get_PWM_Clk_Sel_Val());
            div = Clock_Get_PWM_Div_Val();
            div = (div < 1) ? 1 : div;
            return (clock + div / 2) / div;

        /*!< pwm1 clock */
        case BL_PERIPHERAL_CLOCK_PWM1:
            clock = Clock_PWM_Clk_Mux_Output(Clock_Get_PWM1_Clk_Sel_Val());
            div = Clock_Get_PWM1_Div_Val();
            div = (div < 1) ? 1 : div;
            return (clock + div / 2) / div;

        default:
            return 0;
    }
}

/****************************************************************************/ /**
 * @brief  get system core clock
 *
 * @param  None
 *
 * @return system core clock value
 *
*******************************************************************************/
uint32_t ATTR_CLOCK_SECTION SystemCoreClockGet(void)
{
    uint32_t clockVal = 0;

    switch (GLB_Get_Core_Type()) {
        case GLB_CORE_ID_AP:
            clockVal = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK);
            break;
        default:
            clockVal = 0;
            break;
    }
    clockVal = clockVal ? clockVal : (32 * 1000 * 1000);

    return clockVal;
}

/****************************************************************************/ /**
 * @brief  mcu rtc reset
 *
 * @param  rstEn: hold reset or not
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION CPU_Set_MTimer_RST(uint8_t rstEn)
{
    uint32_t tmpVal = 0;
    uint32_t address = 0;
    uint32_t mask = 0;

    switch (GLB_Get_Core_Type()) {
        case GLB_CORE_ID_AP:
            address = 0x20009014;
            mask = (1 << 30);
            break;
        case GLB_CORE_ID_NP:
            address = 0x24001014;
            mask = (1 << 30);
            break;
        case GLB_CORE_ID_LP:
            address = 0x00080080;
            mask = (1 << 13);
            break;
        default:
            address = 0x20009014;
            mask = (1 << 30);
            break;
    }
    /* AP & NP*/
    tmpVal = BL_RD_WORD(address);
    if (rstEn) {
        tmpVal |= mask;
    } else {
        tmpVal &= (~mask);
    }
    BL_WR_WORD(address, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  mcu rtc reset
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION CPU_Reset_MTimer(void)
{
    CPU_Set_MTimer_RST(1);
    CPU_Set_MTimer_RST(0);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set mcu rtc clock
 *
 * @param  enable: ENABLE or DISABLE
 * @param  mTimerSourceClockType: Souce Clock Type
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION CPU_Set_MTimer_CLK(uint8_t enable, uint8_t clock_src, uint16_t div)
{
    uint32_t tmpVal = 0;
    uint32_t address = 0;
    uint32_t src_mask = 0;
    uint32_t en_mask = 0;
    uint32_t div_mask = 0;

    CHECK_PARAM((div <= 0x3FF));

    switch (GLB_Get_Core_Type()) {
        case GLB_CORE_ID_AP:
            address = 0x20009014;
            src_mask = (1 << 29);
            en_mask = (1 << 31);
            div_mask = (0x3FF << 0);
            break;
        case GLB_CORE_ID_NP:
            address = 0x24001014;
            src_mask = (1 << 29);
            en_mask = (1 << 31);
            div_mask = (0x3FF << 0);
            break;
        case GLB_CORE_ID_LP:
            address = 0x00080080;
            src_mask = 0;
            en_mask = (1 << 12);
            div_mask = (0x3FF << 0);
            break;
        default:
            address = 0x20009014;
            src_mask = (1 << 29);
            en_mask = (1 << 31);
            div_mask = (0x3FF << 0);
            break;
    }

    /* Set MTimer Source Clock */
    tmpVal = BL_RD_WORD(address);
    if (0 == clock_src) {
        tmpVal &= (~src_mask);
    } else {
        tmpVal |= src_mask;
    }
    BL_WR_WORD(address, tmpVal);

    /* disable rtc first */
    tmpVal = BL_RD_WORD(address);
    tmpVal &= (~en_mask);
    BL_WR_WORD(address, tmpVal);

    /* set div */
    tmpVal = BL_RD_WORD(address);
    tmpVal &= (~div_mask);
    tmpVal |= (div & div_mask);
    BL_WR_WORD(address, tmpVal);

    /* enable or not */
    tmpVal = BL_RD_WORD(address);
    if (enable) {
        tmpVal |= en_mask;
    }
    BL_WR_WORD(address, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get cpu rtc source clock
 *
 * @param  None
 *
 * @return cpu rtc source clock
 *
*******************************************************************************/
uint32_t ATTR_CLOCK_SECTION CPU_Get_MTimer_Source_Clock(void)
{
    uint32_t coreFreq = 0;
    uint32_t tmpVal = 0;
    uint32_t address = 0;
    uint32_t src_mask = 0;

    CHECK_PARAM((div <= 0x3FF));

    switch (GLB_Get_Core_Type()) {
        case GLB_CORE_ID_AP:
            address = 0x20009014;
            src_mask = (1 << 29);
            break;
        case GLB_CORE_ID_NP:
            address = 0x24001014;
            src_mask = (1 << 29);
            break;
        case GLB_CORE_ID_LP:
            address = 0x00080080;
            coreFreq = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK);
            return coreFreq;
            break;
        default:
            address = 0x20009014;
            src_mask = (1 << 29);
            break;
    }

    tmpVal = BL_RD_WORD(address);
    if (tmpVal & src_mask) {
        coreFreq = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK);
    } else {
        coreFreq = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    }

    return coreFreq;
}

/****************************************************************************/ /**
 * @brief  get mcu rtc clock
 *
 * @param  None
 *
 * @return mcu rtc counter clock
 *
*******************************************************************************/
uint32_t ATTR_CLOCK_SECTION CPU_Get_MTimer_Clock(void)
{
    uint32_t div = 0;
    uint32_t tmpVal = 0;
    uint32_t address = 0;

    switch (GLB_Get_Core_Type()) {
        case GLB_CORE_ID_AP:
            address = 0x20009014;
            break;
        case GLB_CORE_ID_NP:
            address = 0x24001014;
            break;
        case GLB_CORE_ID_LP:
            address = 0x00080080;
            break;
        default:
            address = 0x20009014;
            break;
    }

    tmpVal = BL_RD_WORD(address);
    div = tmpVal & 0x3FF;
    div += 1;

    return CPU_Get_MTimer_Source_Clock() / div;
}

/****************************************************************************/ /**
 * @brief  get mcu rtc counter
 *
 * @param  None
 *
 * @return mcu rtc counter
 *
*******************************************************************************/
uint64_t ATTR_CLOCK_SECTION CPU_Get_MTimer_Counter(void)
{
#if defined(CPU_MODEL_A0) || defined(CPU_LP)
    volatile uint64_t tmpValLow, tmpValHigh, tmpValLow1, tmpValHigh1;
    uint8_t cnt = 0;

    do {
        tmpValHigh = (uint64_t)csi_coret_get_valueh();
        tmpValLow = (uint64_t)csi_coret_get_value();
        tmpValLow1 = (uint64_t)csi_coret_get_value();
        tmpValHigh1 = (uint64_t)csi_coret_get_valueh();
        cnt++;
        if (cnt > 4) {
            break;
        }
    } while (tmpValLow > tmpValLow1 || tmpValHigh != tmpValHigh1);

    return (uint64_t)((tmpValHigh1 << 32) + tmpValLow1);
#else
    return (SysTimer_GetLoadValue());
#endif
}

/****************************************************************************/ /**
 * @brief  get mcu cycle
 *
 * @param  None
 *
 * @return mcu cycle
 *
*******************************************************************************/
uint64_t ATTR_CLOCK_SECTION CPU_Get_CPU_Cycle(void)
{
#if defined(CPU_MODEL_A0) || defined(CPU_LP)
#if (__riscv_xlen == 64)
    return (uint64_t)__get_MCYCLE();
#else
    volatile uint64_t tmpValLow, tmpValHigh, tmpValLow1, tmpValHigh1;
    uint8_t cnt = 0;

    do {
        tmpValHigh = (uint64_t)__get_MCYCLEH();
        tmpValLow = (uint64_t)__get_MCYCLE();
        tmpValLow1 = (uint64_t)__get_MCYCLE();
        tmpValHigh1 = (uint64_t)__get_MCYCLEH();
        cnt++;
        if (cnt > 4) {
            break;
        }
    } while (tmpValLow > tmpValLow1 || tmpValHigh != tmpValHigh1);

    return (uint64_t)((tmpValHigh1 << 32) + tmpValLow1);
#endif
#else
    return (__get_rv_cycle());
#endif
}

/****************************************************************************/ /**
 * @brief  get mcu rtc counter in us
 *
 * @param  None
 *
 * @return mcu rtc counter in us
 *
*******************************************************************************/
uint64_t ATTR_CLOCK_SECTION CPU_Get_MTimer_US(void)
{
    uint32_t clk = CPU_Get_MTimer_Clock();
    uint64_t mtimer_counter = CPU_Get_MTimer_Counter();
    if (mtimer_counter == 0) {
        mtimer_counter = CPU_Get_CPU_Cycle();
        clk = SystemCoreClockGet();
    }

    if (clk < 1000 * 1000) {
        return mtimer_counter * 1000 / (clk / 1000);
    } else {
        return mtimer_counter / (clk / 1000 / 1000);
    }
}

/****************************************************************************/ /**
 * @brief  get mcu rtc counter in ms
 *
 * @param  None
 *
 * @return mcu rtc counter in ms
 *
*******************************************************************************/
uint64_t ATTR_CLOCK_SECTION CPU_Get_MTimer_MS(void)
{
    uint32_t clk = CPU_Get_MTimer_Clock();
    uint64_t mtimer_counter = CPU_Get_MTimer_Counter();
    if (mtimer_counter == 0) {
        mtimer_counter = CPU_Get_CPU_Cycle();
        clk = SystemCoreClockGet();
    }

    return mtimer_counter / (clk / 1000);
}

/****************************************************************************/ /**
 * @brief  mcu timer delay us
 *
 * @param  cnt: us
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION CPU_MTimer_Delay_US(uint32_t cnt)
{
    volatile uint64_t us1 = 0;
    volatile uint64_t us2 = 0;

    us1 = CPU_Get_MTimer_US();
    do {
        us2 = CPU_Get_MTimer_US();
    } while (((us2 - us1) < (uint64_t)(cnt)));

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  mcu timer delay ms
 *
 * @param  cnt: ms
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION CPU_MTimer_Delay_MS(uint32_t cnt)
{
    volatile uint64_t ms1 = 0;
    volatile uint64_t ms2 = 0;

    ms1 = CPU_Get_MTimer_MS();
    do {
        ms2 = CPU_Get_MTimer_MS();
    } while (((ms2 - ms1) < (uint64_t)(cnt)));

    return SUCCESS;
}

/*@} end of group CLOCK_Public_Functions */

/*@} end of group CLOCK */

/*@} end of group BL616D_Peripheral_Driver */
