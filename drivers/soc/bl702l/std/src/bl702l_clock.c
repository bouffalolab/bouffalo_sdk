/**
  ******************************************************************************
  * @file    bl702l_clock.c
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

#include "bl702l_clock.h"
// #include "pwm_reg.h"
// #include "timer_reg.h"
// #include "risc-v/Core/Include/clic.h"

/** @addtogroup  BL702L_Peripheral_Driver
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

/******************************* clock source *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_RC_LowFreq_Output(void)
{
    return (32768); /* RC32K */
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_RC_HighFreq_Output(void)
{
    return (32 * 1000 * 1000); /* RC32M */
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_XTAL_LowFreq_Output(void)
{
    return (32768); /* 32.768KHz */
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_XTAL_HighFreq_Output(void)
{
    return (32 * 1000 * 1000); /* 32MHz */
}

/******************************* dll clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_DLL_Output(uint32_t div)
{
    if (div == 1) {
        return (128 * 1000 * 1000);
    } else if (div == 2) {
        return (64 * 1000 * 1000);
    } else if (div == 3) {
        return (42666667);
    } else if (div == 5) {
        return (25600000);
    } else if (div == 21) {
        return (6095238);
    } else if (div == 63) {
        return (2031746);
    } else {
        return 0;
    }
}

/******************************* xclk clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_XCLK_Output(uint32_t sel)
{
    if (sel == 0) {
        return Clock_RC_HighFreq_Output();
    } else if (sel == 1) {
        return Clock_XTAL_HighFreq_Output();
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_XCLK_Sel(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_ROOT_CLK_SEL);

    return (tmpVal & 0x1);
}

/******************************* dig32k clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_DIG32K_Output(uint32_t sel, uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_32K_EN) == 0) {
        return 0;
    }
    if (sel == 0) {
        return (Clock_XTAL_HighFreq_Output() / div); /* XTAL32M source */
    } else if (sel == 1) {
        return (Clock_RC_HighFreq_Output() / div); /* RC32M source */
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_DIG32K_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_CLK_SRC_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_DIG32K_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_32K_DIV);
}

/******************************* f32k clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_F32K_Output(uint32_t sel)
{
    if (sel == 0 || sel == 2) {
        return Clock_RC_LowFreq_Output();
    } else if (sel == 1) {
        return Clock_XTAL_LowFreq_Output();
    } else if (sel == 3) {
        return Clock_DIG32K_Output(Clock_DIG32K_Sel(), Clock_DIG32K_Div());
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_F32K_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);

    return BL_GET_REG_BITS_VAL(tmpVal, HBN_F32K_SEL);
}

/******************************* root clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_ROOT_Output(uint32_t selRoot, uint32_t selPll)
{
    if (selRoot == 0) {
        return Clock_RC_HighFreq_Output();
    } else if (selRoot == 1) {
        return Clock_XTAL_HighFreq_Output();
    } else if (selRoot == 2 || selRoot == 3) {
        if (selPll == 0) {
            return Clock_DLL_Output(5);
        } else if (selPll == 1) {
            return Clock_DLL_Output(3);
        } else if (selPll == 2) {
            return Clock_DLL_Output(2);
        } else if (selPll == 3) {
            return Clock_DLL_Output(1);
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_ROOT_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_ROOT_CLK_SEL);

    return tmpVal;
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_ROOT_DLL_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_PLL_SEL);

    return tmpVal;
}

/******************************* fclk clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_FCLK_Output(uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_FCLK_EN) == 0) {
        return 0;
    } else {
        return Clock_ROOT_Output(Clock_ROOT_Sel(), Clock_ROOT_DLL_Sel()) / (div + 1);
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_FCLK_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV);
}

/******************************* hclk clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_HCLK_Output(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_EN) == 0) {
        return 0;
    } else {
        return Clock_FCLK_Output(Clock_FCLK_Div());
    }
}

/******************************* bclk clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_BCLK_Output(uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_EN) == 0) {
        return 0;
    } else {
        return Clock_HCLK_Output() / (div + 1);
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_BCLK_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV);
}

/******************************* mtimer clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_MTIMER_Output(uint32_t sel, uint32_t div)
{
    uint32_t tmpVal;

    if (sel == 0) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_CPU_CLK_CFG);
        if (BL_GET_REG_BITS_VAL(tmpVal, GLB_CPU_RTC_EN) == 0) {
            return 0;
        } else {
            return Clock_XCLK_Output(Clock_XCLK_Sel()) / (div + 1);
        }
    } else if (sel == 1) {
        return Clock_F32K_Output(Clock_F32K_Sel()) / (div + 1);
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_MTIMER_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CPU_CLK_CFG);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_CPU_RTC_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_MTIMER_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CPU_CLK_CFG);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_CPU_RTC_DIV);
}

/****************************************************************************/ /**
 * @brief  Get System Clock
 *
 * @param  type: System clock type
 *
 * @return System clock value
 *
*******************************************************************************/
uint32_t Clock_System_Clock_Get(BL_System_Clock_Type type)
{
    switch (type) {
        case BL_SYSTEM_CLOCK_XCLK:
            /*!< mcu xclk clock */
            return Clock_XCLK_Output(Clock_XCLK_Sel());

        case BL_SYSTEM_CLOCK_ROOT:
            /*!< mcu root clock */
            return Clock_ROOT_Output(Clock_ROOT_Sel(), Clock_ROOT_DLL_Sel());

        case BL_SYSTEM_CLOCK_FCLK:
            return Clock_FCLK_Output(Clock_FCLK_Div());

        case BL_SYSTEM_CLOCK_HCLK:
            return Clock_HCLK_Output();

        case BL_SYSTEM_CLOCK_BCLK:
            /*!< mcu peri bus clock */
            return Clock_BCLK_Output(Clock_BCLK_Div());

        case BL_SYSTEM_CLOCK_XTAL:
            /*!< xtal clock */
            return Clock_XTAL_HighFreq_Output();

        case BL_SYSTEM_CLOCK_F32K:
            /*!< f32k clock */
            return Clock_F32K_Output(Clock_F32K_Sel());

        case BL_SYSTEM_CLOCK_DIG32K:
            return Clock_DIG32K_Output(Clock_DIG32K_Sel(), Clock_DIG32K_Div());

        case BL_SYSTEM_CLOCK_MTIMER:
            /*!< mtimer clock */
            return Clock_MTIMER_Output(Clock_MTIMER_Sel(), Clock_MTIMER_Div());

        default:
            return 0;
    }
}

/******************************* SF clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_SF_Output(uint32_t sel, uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_EN) == 0) {
        return 0;
    }
    if (sel == 0) {
        return Clock_XCLK_Output(Clock_XCLK_Sel()) / (div + 1);
    } else if (sel == 1) {
        return Clock_DLL_Output(2) / (div + 1);
    } else if (sel == 2) {
        return Clock_BCLK_Output(Clock_BCLK_Div()) / (div + 1);
    } else if (sel == 3) {
        return Clock_DLL_Output(3) / (div + 1);
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_SF_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_SF_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV);
}

/******************************* PKA clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_PKA_Output(uint32_t sel)
{
    if (sel == 0) {
        return Clock_HCLK_Output();
    } else if (sel == 1) {
        return Clock_DLL_Output(1);
    } else if (sel == 2) {
        return Clock_DLL_Output(2);
    } else if (sel == 3) {
        return Clock_DLL_Output(3);
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_PKA_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_PKA_CLK_SEL);
}

/******************************* UART clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_UART_Output(uint32_t sel, uint32_t sel2, uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A0) == 0) {
        return 0;
    }
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_UART_CLK_EN) == 0) {
        return 0;
    }
    if (sel2 == 0) {
        if (sel == 0) {
            return Clock_FCLK_Output(Clock_FCLK_Div()) / (div + 1);
        } else if (sel == 1) {
            return Clock_DLL_Output(2) / (div + 1);
        } else {
            return 0;
        }
    } else if (sel2 == 1) {
        return Clock_XCLK_Output(Clock_XCLK_Sel()) / (div + 1);
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_UART_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_UART_CLK_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_UART_Sel2(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_UART_CLK_SEL2);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_UART_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_UART_CLK_DIV);
}

/******************************* SPI clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_SPI_Output(uint32_t sel, uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_EN) == 0) {
        return 0;
    }
    if (sel == 0) {
        return Clock_BCLK_Output(Clock_BCLK_Div()) / (div + 1);
    } else if (sel == 1) {
        return Clock_XCLK_Output(Clock_XCLK_Sel()) / (div + 1);
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_SPI_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_SPI_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_DIV);
}

/******************************* I2C clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_I2C_Output(uint32_t sel, uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A3) == 0) {
        return 0;
    }
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_EN) == 0) {
        return 0;
    }
    if (sel == 0) {
        return Clock_BCLK_Output(Clock_BCLK_Div()) / (div + 1);
    } else if (sel == 1) {
        return Clock_XCLK_Output(Clock_XCLK_Sel()) / (div + 1);
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_I2C_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_I2C_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV);
}

/******************************* IR clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_IR_Output(uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_IR_CLK_EN) == 0) {
        return 0;
    }
    return Clock_XCLK_Output(Clock_XCLK_Sel()) / (div + 1);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_IR_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_IR_CLK_DIV);
}

/******************************* ADC clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_GPADC_Output(uint32_t sel, uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_DIV_EN) == 0) {
        return 0;
    }
    if (sel == 0) {
        return Clock_F32K_Output(Clock_F32K_Sel()) / (div + 1);
    } else if (sel == 1) {
        return Clock_XCLK_Output(Clock_XCLK_Sel()) / (div + 1);
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_GPADC_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_GPADC_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_DIV);
}

/******************************* pwm clock *******************************/
// static inline uint32_t ATTR_CLOCK_SECTION Clock_PWM_Output(uint32_t sel, uint32_t div)
// {
//     if (div == 0) {
//         div = 1;
//     }

//     if (sel == 0) {
//         return Clock_XCLK_Output(Clock_XCLK_Sel()) / div;
//     } else if (sel == 1) {
//         return Clock_BCLK_Output(Clock_BCLK_Div()) / div;
//     } else if (sel == 2 || sel == 3) {
//         return Clock_F32K_Output(Clock_F32K_Sel()) / div;
//     } else {
//         return 0;
//     }
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_PWM_SC0_Sel(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(PWM_BASE, PWM0_SC0_CONFIG);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_SC0_REG_CLK_SEL);

//     return tmpVal;
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_PWM_SC0_Div(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(PWM_BASE, PWM0_SC0_CLKDIV);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_SC0_CLK_DIV);

//     return tmpVal;
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_PWM_MC0_Sel(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(PWM_BASE, PWM_MC0_CONFIG0);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL);

//     return tmpVal;
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_PWM_MC0_Div(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(PWM_BASE, PWM_MC0_CONFIG0);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_CLK_DIV);

//     return tmpVal;
// }

/******************************* key scan clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_KYS_Output(uint32_t sel, uint32_t div)
{
    if (sel == 0) {
        return Clock_XCLK_Output(Clock_XCLK_Sel()) / (div + 1);
    } else if (sel == 1) {
        return Clock_F32K_Output(Clock_F32K_Sel()) / (div + 1);
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_KYS_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_KYS_1M_CLK_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_KYS_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_KYS_1M_CLK_DIV);
}

/******************************* audio clock *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_AUDIO_Output(uint32_t sel, uint32_t div)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);
    if (BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_CLK_DIV_EN) == 0) {
        return 0;
    }
    if (sel == 0) {
        return Clock_DLL_Output(63) / (div + 1);
    } else if (sel == 1) {
        return Clock_DLL_Output(21) / (div + 1);
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_AUDIO_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_CLK_SRC_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_AUDIO_Div(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_CLK_DIV);
}

/******************************* chip clock out *******************************/
static inline uint32_t ATTR_CLOCK_SECTION Clock_CHIP0_Output(uint32_t sel)
{
    if (sel == 1) {
        return Clock_DLL_Output(63);
    } else if (sel == 2) {
        return Clock_DLL_Output(21);
    } else if (sel == 3) {
        return Clock_XCLK_Output(Clock_XCLK_Sel());
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_CHIP1_Output(uint32_t sel)
{
    if (sel == 1) {
        return Clock_F32K_Output(Clock_F32K_Sel());
    } else if (sel == 2 || sel == 3) {
        return Clock_XTAL_HighFreq_Output();
    } else {
        return 0;
    }
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_CHIP0_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_0_SEL);
}

static inline uint32_t ATTR_CLOCK_SECTION Clock_CHIP1_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_1_SEL);
}

/******************************* timer clock *******************************/
// static inline uint32_t ATTR_CLOCK_SECTION Clock_TIMER_Output(uint32_t sel, uint32_t div)
// {
//     if (sel == 0) {
//         return Clock_FCLK_Output(Clock_FCLK_Div()) / (div + 1);
//     } else if (sel == 1) {
//         return Clock_F32K_Output(Clock_F32K_Sel()) / (div + 1);
//     } else if (sel == 2) {
//         return Clock_F32K_Output(Clock_F32K_Sel()) * 1000 / (div + 1) / 32768;
//     } else if (sel == 3) {
//         return Clock_XTAL_HighFreq_Output() / (div + 1);
//     } else {
//         return 0;
//     }
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_TIMER0_CH0_Sel(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, TIMER_CS_2);

//     return tmpVal;
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_TIMER0_CH0_Div(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, TIMER_TCDR2);

//     return tmpVal;
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_TIMER0_CH1_Sel(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, TIMER_CS_3);

//     return tmpVal;
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_TIMER0_CH1_Div(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, TIMER_TCDR3);

//     return tmpVal;
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_WDT_Sel(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, TIMER_CS_WDT);

//     return tmpVal;
// }

// static inline uint32_t ATTR_CLOCK_SECTION Clock_WDT_Div(void)
// {
//     uint32_t tmpVal;

//     tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
//     tmpVal = BL_GET_REG_BITS_VAL(tmpVal, TIMER_WCDR);

//     return tmpVal;
// }

/****************************************************************************/ /**
 * @brief  Get Peripheral Clock
 *
 * @param  type: Peripheral clock type
 *
 * @return Peripheral clock value
 *
*******************************************************************************/
uint32_t Clock_Peripheral_Clock_Get(BL_Peripheral_Type type)
{
    uint32_t clock = 0;

    switch (type) {
        case BL_PERIPHERAL_CLOCK_UART0:
            clock = Clock_UART_Output(Clock_UART_Sel(), Clock_UART_Sel2(), Clock_UART_Div());
            break;

        case BL_PERIPHERAL_CLOCK_SPI0:
            clock = Clock_SPI_Output(Clock_SPI_Sel(), Clock_SPI_Div());
            break;

        case BL_PERIPHERAL_CLOCK_I2C0:
            clock = Clock_I2C_Output(Clock_I2C_Sel(), Clock_I2C_Div());
            break;

        case BL_PERIPHERAL_CLOCK_GPADC:
            clock = Clock_GPADC_Output(Clock_GPADC_Sel(), Clock_GPADC_Div());
            break;

        // case BL_PERIPHERAL_CLOCK_PWMSC:
        //     clock = Clock_PWM_Output(Clock_PWM_SC0_Sel(), Clock_PWM_SC0_Div());
        //     break;

        // case BL_PERIPHERAL_CLOCK_PWM:
        //     clock = Clock_PWM_Output(Clock_PWM_MC0_Sel(), Clock_PWM_MC0_Div());
        //     break;

        case BL_PERIPHERAL_CLOCK_IR:
            clock = Clock_IR_Output(Clock_IR_Div());
            break;

        case BL_PERIPHERAL_CLOCK_FLASH:
            clock = Clock_SF_Output(Clock_SF_Sel(), Clock_SF_Div());
            break;

        // case BL_PERIPHERAL_CLOCK_TIMER0_CH0:
        //     clock = Clock_TIMER_Output(Clock_TIMER0_CH0_Sel(), Clock_TIMER0_CH0_Div());
        //     break;

        // case BL_PERIPHERAL_CLOCK_TIMER0_CH1:
        //     clock = Clock_TIMER_Output(Clock_TIMER0_CH1_Sel(), Clock_TIMER0_CH1_Div());
        //     break;

        // case BL_PERIPHERAL_CLOCK_WDT:
        //     clock = Clock_TIMER_Output(Clock_WDT_Sel(), Clock_WDT_Div());
        //     break;

        case BL_PERIPHERAL_CLOCK_AUDIO:
            clock = Clock_AUDIO_Output(Clock_AUDIO_Sel(), Clock_AUDIO_Div());
            break;

        case BL_PERIPHERAL_CLOCK_CHIPOUT0:
            clock = Clock_CHIP0_Output(Clock_CHIP0_Sel());
            break;

        case BL_PERIPHERAL_CLOCK_CHIPOUT1:
            clock = Clock_CHIP1_Output(Clock_CHIP1_Sel());
            break;

        case BL_PERIPHERAL_CLOCK_PKA:
            clock = Clock_PKA_Output(Clock_PKA_Sel());
            break;

        case BL_PERIPHERAL_CLOCK_KYS:
            clock = Clock_KYS_Output(Clock_KYS_Sel(), Clock_KYS_Div());
            break;

        default:
            clock = 0;
    }
    return clock;
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

    clockVal = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_FCLK);

    clockVal = clockVal ? clockVal : (32 * 1000 * 1000);

    return clockVal;
}

/****************************************************************************/ /**
 * @brief  get mcu rtc clock
 *
 * @param  None
 *
 * @return mcu rtc counter clock
 *
*******************************************************************************/
uint32_t CPU_Get_MTimer_Clock(void)
{
    return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MTIMER);
}

/****************************************************************************/ /**
 * @brief  get mcu rtc counter
 *
 * @param  None
 *
 * @return mcu rtc counter
 *
*******************************************************************************/
// uint64_t CPU_Get_MTimer_Counter(void)
// {
//     volatile uint64_t tmpValLow, tmpValHigh, tmpValLow1, tmpValHigh1;
//     uint8_t cnt = 0;

//     do {
//         tmpValHigh = (uint64_t)(*(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME + 4));
//         tmpValLow = (uint64_t)(*(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME));
//         tmpValLow1 = (uint64_t)(*(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME));
//         tmpValHigh1 = (uint64_t)(*(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME + 4));
//         cnt++;
//         if (cnt > 4) {
//             break;
//         }
//     } while (tmpValLow > tmpValLow1 || tmpValHigh != tmpValHigh1);

//     return (uint64_t)((tmpValHigh1 << 32) + tmpValLow1);
// }

/****************************************************************************/ /**
 * @brief  get mcu cycle
 *
 * @param  None
 *
 * @return mcu cycle
 *
*******************************************************************************/
uint64_t CPU_Get_CPU_Cycle(void)
{
#if (__riscv_xlen == 64)
    return (uint64_t)__get_MCYCLE();
#else
    volatile uint64_t tmpValLow, tmpValHigh, tmpValLow1, tmpValHigh1;
    uint32_t result;
    uint8_t cnt = 0;

    do {
        __ASM volatile("csrr %0, mcycleh"
                       : "=r"(result));
        tmpValHigh = (uint64_t)result;
        __ASM volatile("csrr %0, mcycle"
                       : "=r"(result));
        tmpValLow = (uint64_t)result;
        __ASM volatile("csrr %0, mcycle"
                       : "=r"(result));
        tmpValLow1 = (uint64_t)result;
        __ASM volatile("csrr %0, mcycleh"
                       : "=r"(result));
        tmpValHigh1 = (uint64_t)result;
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
uint64_t CPU_Get_MTimer_US(void)
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
uint64_t CPU_Get_MTimer_MS(void)
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
BL_Err_Type CPU_MTimer_Delay_US(uint32_t cnt)
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
BL_Err_Type CPU_MTimer_Delay_MS(uint32_t cnt)
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

/*@} end of group BL702L_Peripheral_Driver */
