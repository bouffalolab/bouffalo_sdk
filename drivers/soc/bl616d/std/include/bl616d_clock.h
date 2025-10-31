/**
  ******************************************************************************
  * @file    bl616d_clock.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
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
#ifndef __BL616D_CLOCK_H__
#define __BL616D_CLOCK_H__

#include "glb_reg.h"
#include "mcu_misc_reg.h"
#include "pds_reg.h"
#include "bl616d_glb.h"
#include "bl616d_hbn.h"
#include "bl616d_common.h"
#include "hardware/timer_reg.h"
#include "hardware/pwm_v2_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616D_Peripheral_Driver
 *  @{
 */

/** @addtogroup  CLOCK
 *  @{
 */

/** @defgroup  CLOCK_Public_Types
 *  @{
 */

/**
 *  @brief System clock type definition
 */
typedef enum {
    BL_SYSTEM_CLOCK_MCU_ROOT_CLK, /*!< MCU root clock */
    BL_SYSTEM_CLOCK_MCU_CLK,      /*!< MCU Fast clock/CPU clock */
    BL_SYSTEM_CLOCK_MCU_BCLK,     /*!< MCU BUS clock */
    BL_SYSTEM_CLOCK_MCU_PBCLK,    /*!< MCU peri BUS clock */
    BL_SYSTEM_CLOCK_F32K,         /*!< F32K clock */
    BL_SYSTEM_CLOCK_XCLK,         /*!< XCLK:RC32M or XTAL */
    BL_SYSTEM_CLOCK_XTAL,         /*!< XTAL clock */
    BL_SYSTEM_CLOCK_WL_XCLK,      /*!< WIFI XCLK:RC32M or XTAL */
    BL_SYSTEM_CLOCK_MAX,          /*!< MAX type of system clock */
} BL_System_Clock_Type;

typedef enum {
    BL_MTIMER_SOURCE_CLOCK_MCU_XCLK, /*!< MCU xclk clock */
    BL_MTIMER_SOURCE_CLOCK_MCU_CLK,  /*!< MCU root clock */
} BL_MTimer_Source_Clock_Type;

typedef enum {
    CLOCK_CPUPLL_DIV1,
    CLOCK_CPUPLL_DIV2,
    CLOCK_CPUPLL_DIV3,
    CLOCK_CPUPLL_DIV4,
    CLOCK_CPUPLL_DIV5,
    CLOCK_CPUPLL_DIV6,
    CLOCK_CPUPLL_DIV8,
    CLOCK_CPUPLL_DIV10,
    CLOCK_CPUPLL_DIV12,
    CLOCK_CPUPLL_DIV20,
    CLOCK_CPUPLL_DIV30,
} CLOCK_CPUPLL_Type;

/**
 *  @brief Peripheral clock type definition
 */
typedef enum {
    BL_PERIPHERAL_CLOCK_UART0,    /*!< UART0 clock */
    BL_PERIPHERAL_CLOCK_UART1,    /*!< UART1 clock */
    BL_PERIPHERAL_CLOCK_UART2,    /*!< UART2 clock */
    BL_PERIPHERAL_CLOCK_UART3,    /*!< UART3 clock */
    BL_PERIPHERAL_CLOCK_SPI0,     /*!< SPI0 clock */
    BL_PERIPHERAL_CLOCK_SPI1,     /*!< SPI1 clock */
    BL_PERIPHERAL_CLOCK_RESERVED, /*!< reserved clock */
    BL_PERIPHERAL_CLOCK_DBI,      /*!< DBI clock */
    BL_PERIPHERAL_CLOCK_EMI,      /*!< EMI clock */
    BL_PERIPHERAL_CLOCK_I2C0,     /*!< I2C0 clock */
    BL_PERIPHERAL_CLOCK_I2C1,     /*!< I2C1 clock */
    BL_PERIPHERAL_CLOCK_I2C2,     /*!< I2C2 clock */
    BL_PERIPHERAL_CLOCK_PSRAMB,   /*!< PSRAMB clock */
    BL_PERIPHERAL_CLOCK_FLASH,    /*!< FLASH clock */
    BL_PERIPHERAL_CLOCK_I2S,      /*!< I2S clock */
    BL_PERIPHERAL_CLOCK_IR,       /*!< IR clock */
    BL_PERIPHERAL_CLOCK_AUADC,    /*!< ADC clock */
    BL_PERIPHERAL_CLOCK_GPADC,    /*!< GPADC clock */
    BL_PERIPHERAL_CLOCK_GPDAC,    /*!< GPDAC clock */
    BL_PERIPHERAL_CLOCK_CAM_REF,  /*!< CAM_REF clock */
    BL_PERIPHERAL_CLOCK_CAM,      /*!< CAM clock */
    BL_PERIPHERAL_CLOCK_SDH,      /*!< SDH clock */
    BL_PERIPHERAL_CLOCK_SEC_PKA,  /*!< PKA clock */
    BL_PERIPHERAL_CLOCK_RTC,      /*!< RTC clock */
    BL_PERIPHERAL_CLOCK_PEC,      /*!< PEC clock  */
    BL_PERIPHERAL_CLOCK_TIMER0,   /*!< TIMER0 clock */
    BL_PERIPHERAL_CLOCK_TIMER1,   /*!< TIMER1 clock */
    BL_PERIPHERAL_CLOCK_TIMER2,   /*!< TIMER2 clock */
    BL_PERIPHERAL_CLOCK_TIMER3,   /*!< TIMER3 clock */
    BL_PERIPHERAL_CLOCK_PWM0,     /*!< PWM0 clock */
    BL_PERIPHERAL_CLOCK_PWM1,     /*!< PWM1 clock */
    BL_PERIPHERAL_CLOCK_MAX,
} BL_Peripheral_Type;

/*@} end of group CLOCK_Public_Types */

/** @defgroup  CLOCK_Public_Constants
 *  @{
 */

/** @defgroup  BL_SYSTEM_CLOCK_TYPE
 *  @{
 */

/*@} end of group CLOCK_Public_Constants */

/** @defgroup  CLOCK_Public_Macros
 *  @{
 */


/*@} end of group CLOCK_Public_Macros */

/** @defgroup  CLOCK_Public_Functions
 *  @{
 */

uint32_t Clock_System_Clock_Get(BL_System_Clock_Type type);
uint32_t Clock_Peripheral_Clock_Get(BL_Peripheral_Type type);
uint32_t SystemCoreClockGet(void);
BL_Err_Type CPU_Set_MTimer_RST(uint8_t rstEn);
BL_Err_Type CPU_Reset_MTimer(void);
BL_Err_Type CPU_Set_MTimer_CLK(uint8_t enable, uint8_t clock_src, uint16_t div);
uint32_t CPU_Get_MTimer_Source_Clock(void);
uint32_t CPU_Get_MTimer_Clock(void);
uint64_t CPU_Get_MTimer_Counter(void);
uint64_t CPU_Get_CPU_Cycle(void);
uint64_t CPU_Get_MTimer_US(void);
uint64_t CPU_Get_MTimer_MS(void);
BL_Err_Type CPU_MTimer_Delay_US(uint32_t cnt);
BL_Err_Type CPU_MTimer_Delay_MS(uint32_t cnt);
uint32_t bflb_peripheral_clock_get_by_id(uint8_t peri);
int bflb_peripheral_clock_control_by_id(uint8_t peri, bool enable);
int bflb_peripheral_clock_status_get_by_id(uint8_t peri);

/*@} end of group CLOCK_Public_Functions */

/*@} end of group CLOCK */

/*@} end of group BL616D_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616D_CLOCK_H__ */
