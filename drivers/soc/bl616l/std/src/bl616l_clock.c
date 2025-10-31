/**
  ******************************************************************************
  * @file    bl616l_clock.c
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

#include "bl616l_clock.h"

/** @addtogroup  BL616L_Peripheral_Driver
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
        if (HBN_RCM_8M == HBN_Get_RC32M_Speed()) {
            return (8 * 1000 * 1000);
        } else if (HBN_RCM_16M == HBN_Get_RC32M_Speed()) {
            return (16 * 1000 * 1000);
        } else if (HBN_RCM_32M == HBN_Get_RC32M_Speed()) {
            return (32 * 1000 * 1000);
        } else {
            return (32 * 1000 * 1000);
        }
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

    if (sel == 0 || sel == 2) {
        /* rc32k */
        return (32768);
    } else if (sel == 1) {
        /* xtal 32K */
        return (32768);
    } else {
        /* dig32k */
        if (BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_32K_EN) == 0) {
            return 0;
        } else {
            sel = BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_CLK_SRC_SEL);
            if (sel == 0) {
                return Clock_Get_WIFI_PLL_Output(32 * 1000 * 1000) / div;
            } else if (sel == 1) {
                return Clock_Xtal_Output() / div;
            } else {
                return Clock_Get_WIFI_PLL_Output(160 * 1000 * 1000) / div;
            }
        }
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
    } else if (sel == 1) {
        /* wifi pll 240m */
        return Clock_Get_WIFI_PLL_Output(240 * 1000 * 1000);
    } else if (sel == 2) {
        /* wifi pll 120m */
        return Clock_Get_WIFI_PLL_Output(120 * 1000 * 1000);
    } else if (sel == 3) {
        /* wifi pll 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
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
    } else if (sel == 1) {
        /* wifi pll 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    } else if (sel == 2 || sel == 3) {
        /* wifi pll 48m */
        return Clock_Get_WIFI_PLL_Output(48 * 1000 * 1000);
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
        /* wifi pll 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    } else if (sel == 1) {
        /* wifi pll 192m */
        return Clock_Get_WIFI_PLL_Output(192 * 1000 * 1000);
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

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV);
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
        /* wifi pll 320m */
        return Clock_Get_WIFI_PLL_Output(320 * 1000 * 1000);
    } else if (sel == 1) {
        /* wifi pll 240m */
        return Clock_Get_WIFI_PLL_Output(240 * 1000 * 1000);
    } else {
        return 0;
    }
}

static inline uint8_t Clock_Get_PSRAMB_Clk_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PSRAM_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_PSRAMB_CLK_DIV);
}

static inline uint8_t Clock_Get_PSRAMB_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PSRAM_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_PSRAMB_CLK_SEL);
}

uint32_t Clock_Get_PSRAMB_Clk(void)
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
        /* mux pbclk / 3 */
        return (Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK) / 3);
    } else if (sel == 2) {
        /* mux 160m / 3 */
        return (Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val()) / 3);
    } else if (sel == 3) {
        /* wifi pll 320m / 3 */
        return (Clock_Get_WIFI_PLL_Output(320 * 1000 * 1000) / 3);
    } else {
        return 0;
    }
}

static inline uint8_t Clock_Get_PKA_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = (*((volatile uint32_t *)0x20004310));
    if (tmpVal & (1 << 3U)) {
        return 0;
    } else {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_PKA_CLK_SEL);
        return (tmpVal + 1);
    }
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
    } else if (sel == 3) {
        /* wifi pll 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    } else {
        return 0;
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
        /* wifi 160m */
        return Clock_Get_WIFI_PLL_Output(160 * 1000 * 1000);
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

static inline uint8_t Clock_Get_SPI1_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI1_CLK_SEL);
}

static inline uint8_t Clock_Get_SPI1_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI1_CLK_DIV);
}

static inline uint8_t Clock_Get_SPI0_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_SEL);
}

static inline uint8_t Clock_Get_SPI0_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_DIV);
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
        /* wifi pll 32m */
        return Clock_Get_WIFI_PLL_Output(32 * 1000 * 1000);
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
    uint32_t div = Clock_Get_GPADC_Div_Val();
    return Clock_GPADC_Clk_Output(sel) / (div + 1);
}

static inline uint32_t Clock_I2S_Clk_Mux_Output(void)
{
    return Clock_Get_WIFI_PLL_Output(320 * 1000 * 1000);
}

static inline uint8_t Clock_Get_I2S_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2S_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_I2S_REF_CLK_DIV);
}

static inline uint32_t Clock_Audio_ADC_Clk_Mux_Output(void)
{
    return Clock_Get_WIFI_PLL_Output(320 * 1000 * 1000);
}

static inline uint8_t Clock_Get_AUSOLO_Div_Val(void)
{
    uint32_t tmpVal = 0;
    uint32_t div = 0;

    // tmpVal = BL_RD_REG(AUADC_BASE, AUSOLO_AUDPDM_TOP);
    // tmpVal = BL_GET_REG_BITS_VAL(tmpVal, AUSOLO_ADC_RATE);

    tmpVal = (*((volatile uint32_t *)0x2000ac00));
    tmpVal = (tmpVal >> 28) & 0xF;

    if (tmpVal == 0) {
        div = 23;
    } else if (tmpVal == 1) {
        div = 11;
    } else if (tmpVal == 2) {
        div = 7;
    } else if (tmpVal == 3) {
        div = 5;
    } else if (tmpVal == 4) {
        div = 3;
    } else if (tmpVal == 5) {
        div = 1;
    } else if (tmpVal == 6) {
        div = 0;
    } else if (tmpVal == 7) {
        div = 1;
    } else if (tmpVal == 8) {
        div = 191;
    } else if (tmpVal == 9) {
        div = 95;
    } else if (tmpVal == 10) {
        div = 47;
    } else {
        div = 0;
    }

    return div;
}

static inline uint8_t Clock_Get_Audio_ADC_Div_Val(void)
{
    uint32_t tmpVal;
    uint32_t div_sel = 0;
    uint32_t div = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG0);
    div_sel = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);

    if (div_sel == 0) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG1);
        div = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_SOLO_CLK_DIV);
    } else {
        div = Clock_Get_AUSOLO_Div_Val();
    }

    return div;
}

static inline uint32_t Clock_SDH_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    } else {
        /* wifi pll 120m */
        return Clock_Get_WIFI_PLL_Output(120 * 1000 * 1000);
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
        /* wifi pll 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    } else if (sel == 2 || sel == 3) {
        /* wifi pll 120m */
        return Clock_Get_WIFI_PLL_Output(120 * 1000 * 1000);
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

static inline uint32_t Clock_CAM_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 80m */
        return Clock_Get_WIFI_PLL_Output(80 * 1000 * 1000);
    } else if (sel == 1) {
        /* wifi pll 120m */
        return Clock_Get_WIFI_PLL_Output(120 * 1000 * 1000);
    } else if (sel == 2 || sel == 3) {
        /* wifi pll 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    } else {
        return 0;
    }
}

static inline uint8_t Clock_Get_CAM_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_ISP_MUXPLL_80M_SEL);
}

static inline uint8_t Clock_Get_CAM_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ISP_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_ISP_CORE_CLK_DIV);
}

/****************************************************************************/ /**

 * @brief  Get Audio ADC Clock
 *
 * @return Audio ADC clock value
 *
*******************************************************************************/
uint32_t Clock_Audio_ADC_Clock_Get(void)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    clock = Clock_Audio_ADC_Clk_Mux_Output();
    div = Clock_Get_Audio_ADC_Div_Val();

    return clock / (div + 1);
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

static inline uint32_t Clock_PWM_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
    } else if (sel == 1) {
        /* mcu pblck */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);
    } else {
        /*!< f32k clock */
        return Clock_F32k_Mux_Output(Clock_Get_F32k_Sel_Val());
    }
}

static inline uint8_t Clock_Get_PWM_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    // tmpVal = BL_RD_REG(PWM_BASE, PWM_MC0_CONFIG0);

    // return BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL);

    tmpVal = (*((volatile uint32_t *)0x2000a440));

    return (tmpVal >> 30) & 0x3;
}

uint32_t Clock_PWM_Clk_Get(void)
{
    uint32_t sel = 0;
    uint32_t clock = 0;

    sel = Clock_Get_PWM_Clk_Sel_Val();
    clock = Clock_PWM_Clk_Mux_Output(sel);

    return clock;
}

static inline uint32_t Clock_AHB2QSPI_Clk_Mux_Output(void)
{
    return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);
}

static inline uint8_t Clock_Get_AHB2QSPI_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AHB2QSPI_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_QSPI_CLK_DIV);
}

static inline uint32_t Clock_Audio_DAC_Clk_Get(void)
{
    /* wifi pll 96m */
    return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
}

/****************************************************************************/ /**

 * @brief  Get RTC Clock
 *
 * @return RTC clock value
 *
*******************************************************************************/
static inline uint32_t Clock_RTC_Clock_Get(void)
{
    uint8_t clkType;
    uint32_t tmpVal;
    uint32_t divEn;
    uint32_t divVal;

    HBN_Get_RTC_CLK_Sel(&clkType);
    tmpVal = BL_RD_REG(HBN_BASE, HBN_TIME_H);
    divEn = BL_GET_REG_BITS_VAL(tmpVal, HBN_PDS_HBN_XTAL_DIV_EN);
    divVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_PDS_HBN_XTAL_DIV);

    switch (clkType) {
        case HBN_RTC_CLK_F32K:
            return 32768;

        case HBN_RTC_CLK_XTAL_DIVIED_32K:
            if (divEn) {
                return divVal ? (40 * 1000 * 1000 / divVal) : 0;
            } else {
                return 40 * 1000 * 1000;
            }

        default:
            return 0;
    }
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
            clock = Clock_UART_Clk_Mux_Output(Clock_Get_UART_Clk_Sel_Val());
            div = Clock_Get_UART_Div_Val();
            return clock / (div + 1);

        /*!< spi0 clock */
        case BL_PERIPHERAL_CLOCK_SPI0:
            clock = Clock_SPI_Clk_Mux_Output(Clock_Get_SPI0_Clk_Sel_Val());
            div = Clock_Get_SPI0_Div_Val();
            return clock / (div + 1);

        /*!< spi1 clock */
        case BL_PERIPHERAL_CLOCK_SPI1:
            clock = Clock_SPI_Clk_Mux_Output(Clock_Get_SPI1_Clk_Sel_Val());
            div = Clock_Get_SPI1_Div_Val();
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
            clock = Clock_I2C_Clk_Mux_Output(Clock_Get_I2C_Clk_Sel_Val());
            div = Clock_Get_I2C_Div_Val();
            return clock / (div + 1);

        /*!< PSRAMB clock */
        case BL_PERIPHERAL_CLOCK_PSRAMB:
            return Clock_Get_PSRAMB_Clk();

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

        /*!< GPADC clock */
        case BL_PERIPHERAL_CLOCK_GPADC:
            return Clock_GPADC_Clk_Mux_Output(Clock_Get_GPADC_32M_Clk_Sel_Val());

        /*!< CAM_REF clock */
        case BL_PERIPHERAL_CLOCK_CAM_REF:
            clock = Clock_CAM_REF_Clk_Mux_Output(Clock_Get_CAM_REF_Clk_Sel_Val());
            div = Clock_Get_CAM_REF_Div_Val();
            return clock / (div + 1);

        /*!< CAM clock */
        case BL_PERIPHERAL_CLOCK_CAM:
            clock = Clock_CAM_Clk_Mux_Output(Clock_Get_CAM_Clk_Sel_Val());
            div = Clock_Get_CAM_Div_Val();
            return clock / (div + 1);

        /*!< SDH clock */
        case BL_PERIPHERAL_CLOCK_SDH:
            clock = Clock_SDH_Clk_Mux_Output(Clock_Get_SDH_Clk_Sel_Val());
            div = Clock_Get_SDH_Div_Val();
            return clock / (div + 1);

        /*!< PKA clock */
        case BL_PERIPHERAL_CLOCK_SEC_PKA:
            return Clock_PKA_Clk_Mux_Output(Clock_Get_PKA_Clk_Sel_Val());

        /*!< pec clock */
        case BL_PERIPHERAL_CLOCK_PEC:
            clock = Clock_PEC_Clk_Mux_Output(Clock_Get_PEC_Clk_Sel_Val());
            div = Clock_Get_PEC_Div_Val();
            return clock / (div + 1);

        /*!< pwm clock */
        case BL_PERIPHERAL_CLOCK_PWM:
            return Clock_PWM_Clk_Get();

        /*!< audac clock */
        case BL_PERIPHERAL_CLOCK_AUDAC:
            return Clock_Audio_DAC_Clk_Get();

        /*!< auadc clock */
        case BL_PERIPHERAL_CLOCK_AUADC:
            return Clock_Audio_ADC_Clock_Get();

            /*!< RTC clock */
        case BL_PERIPHERAL_CLOCK_RTC:
            return Clock_RTC_Clock_Get();

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
        case GLB_CORE_ID_M0:
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

    switch (GLB_Get_Core_Type()) {
        case GLB_CORE_ID_M0:
            address = MCU_MISC_BASE + MCU_MISC_MCU_E907_RTC_OFFSET;
            break;
        default:
            address = MCU_MISC_BASE + MCU_MISC_MCU_E907_RTC_OFFSET;
            break;
    }

    /* set reset */
    /* MCU RTC_RST is [30] */
    /* DSP RTC_RST is [30] */
    /* LP  RTC_RST is [30] */
    tmpVal = BL_RD_WORD(address);
    if (rstEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, MCU_MISC_REG_MCU_RTC_RST);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, MCU_MISC_REG_MCU_RTC_RST);
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
BL_Err_Type ATTR_CLOCK_SECTION CPU_Set_MTimer_CLK(uint8_t enable, BL_MTimer_Source_Clock_Type mTimerSourceClockType, uint16_t div)
{
    uint32_t tmpVal = 0;
    uint32_t address = 0;

    CHECK_PARAM((div <= 0x3FF));

    address = MCU_MISC_BASE + MCU_MISC_MCU_E907_RTC_OFFSET;

    /* Set MTimer Source Clock */
    tmpVal = BL_RD_WORD(address);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, MCU_MISC_REG_MCU_RTC_CLK_SEL, mTimerSourceClockType);
    BL_WR_WORD(address, tmpVal);

    /* disable rtc first */
    /* MCU RTC_EN is [31] */
    /* DSP RTC_EN is [31] */
    /* LP  RTC_EN is [31] */
    tmpVal = BL_RD_WORD(address);
    tmpVal = BL_CLR_REG_BIT(tmpVal, MCU_MISC_REG_MCU_RTC_EN);
    BL_WR_WORD(address, tmpVal);

    /* set div */
    /* MCU RTC_DIV is [9:0] */
    /* DSP RTC_DIV is [9:0] */
    /* LP  RTC_DIV is [9:0] */
    tmpVal = BL_RD_WORD(address);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, MCU_MISC_REG_MCU_RTC_DIV, div);
    BL_WR_WORD(address, tmpVal);

    /* enable or not */
    /* MCU RTC_EN is [31] */
    /* DSP RTC_EN is [31] */
    /* LP  RTC_EN is [31] */
    tmpVal = BL_RD_WORD(address);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, MCU_MISC_REG_MCU_RTC_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, MCU_MISC_REG_MCU_RTC_EN);
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

    tmpVal = BL_RD_REG(MCU_MISC_BASE, MCU_MISC_MCU_E907_RTC);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, MCU_MISC_REG_MCU_RTC_CLK_SEL);

    /* get cpu core clock */
    switch (GLB_Get_Core_Type()) {
        case GLB_CORE_ID_M0:
            if (tmpVal == BL_MTIMER_SOURCE_CLOCK_MCU_XCLK) {
                coreFreq = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
            } else {
                coreFreq = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK);
            }
            break;
        default:
            coreFreq = 0;
            break;
    }
    coreFreq = coreFreq ? coreFreq : (32 * 1000 * 1000);

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
    uint32_t coreFreq = 0;
    uint32_t address = 0;
    uint32_t div = 0;
    uint32_t tmpVal = 0;

    address = MCU_MISC_BASE + MCU_MISC_MCU_E907_RTC_OFFSET;

    tmpVal = BL_RD_WORD(address);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, MCU_MISC_REG_MCU_RTC_CLK_SEL);

    /* get cpu core clock */
    /* get mtimer clock div */
    switch (GLB_Get_Core_Type()) {
        case GLB_CORE_ID_M0:
            if (tmpVal == BL_MTIMER_SOURCE_CLOCK_MCU_XCLK) {
                coreFreq = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
            } else {
                coreFreq = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK);
            }
            break;
        default:
            coreFreq = 0;
            break;
    }
    /* MCU RTC_DIV is [9:0] */
    /* DSP RTC_DIV is [9:0] */
    /* LP  RTC_DIV is [9:0] */
    div = BL_GET_REG_BITS_VAL(BL_RD_WORD(address), MCU_MISC_REG_MCU_RTC_DIV);
    div += 1;
    coreFreq = coreFreq ? coreFreq : (32 * 1000 * 1000);
    coreFreq = coreFreq / div; /* mtimer clock = cpu clock / mtimer div */

    return coreFreq;
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

/*@} end of group BL616L_Peripheral_Driver */
