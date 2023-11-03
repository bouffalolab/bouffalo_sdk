/**
  ******************************************************************************
  * @file    bl702l_clock.h
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
#ifndef __BL702L_CLOCK_H__
#define __BL702L_CLOCK_H__

#include "glb_reg.h"
#include "bl702l_hbn.h"
#include "bl702l_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL702L_Peripheral_Driver
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
    BL_SYSTEM_CLOCK_ROOT,   /*!< ROOT clock */
    BL_SYSTEM_CLOCK_FCLK,   /*!< Fast clock/CPU clock */
    BL_SYSTEM_CLOCK_HCLK,   /*!< HCLK */
    BL_SYSTEM_CLOCK_BCLK,   /*!< BUS clock */
    BL_SYSTEM_CLOCK_F32K,   /*!< F32K clock */
    BL_SYSTEM_CLOCK_XCLK,   /*!< XCLK:RC32M or XTAL */
    BL_SYSTEM_CLOCK_XTAL,   /*!< XTAL clock */
    BL_SYSTEM_CLOCK_DIG32K, /*!< DIG_32K clock */
    BL_SYSTEM_CLOCK_MTIMER, /*!< MTIMER clock */
    BL_SYSTEM_CLOCK_MAX,    /*!< MAX type of system clock */
} BL_System_Clock_Type;

typedef enum {
    BL_PERIPHERAL_CLOCK_UART0,   /*!< UART0 clock */
    BL_PERIPHERAL_CLOCK_SPI0,    /*!< SPI0 clock */
    BL_PERIPHERAL_CLOCK_I2C0,    /*!< I2C0 clock */
    BL_PERIPHERAL_CLOCK_GPADC,   /*!< GPADC clock */
    BL_PERIPHERAL_CLOCK_PWMSC,   /*!< PWMSC clock */
    BL_PERIPHERAL_CLOCK_PWM,     /*!< PWM clock */
    BL_PERIPHERAL_CLOCK_IR,      /*!< IR clock */
    BL_PERIPHERAL_CLOCK_FLASH,   /*!< FLASH clock */
    BL_PERIPHERAL_CLOCK_TIMER0_CH0,/*!< TIMER0 channel0 clock */
    BL_PERIPHERAL_CLOCK_TIMER0_CH1,/*!< TIMER0 channel1 clock */
    BL_PERIPHERAL_CLOCK_WDT,     /*!< WDT clock */
    BL_PERIPHERAL_CLOCK_AUDIO,   /*!< Audio clock */
    BL_PERIPHERAL_CLOCK_CHIPOUT0,/*!< chip out 0 */
    BL_PERIPHERAL_CLOCK_CHIPOUT1,/*!< chip out 1 */
    BL_PERIPHERAL_CLOCK_PKA,     /*!< PKA clock */
    BL_PERIPHERAL_CLOCK_KYS,     /*!< KYS clock */
    BL_PERIPHERAL_CLOCK_MAX,
} BL_Peripheral_Type;

/*@} end of group CLOCK_Public_Types */

/** @defgroup  CLOCK_Public_Constants
 *  @{
 */

/** @defgroup  BL_SYSTEM_CLOCK_TYPE
 *  @{
 */
#define IS_BL_SYSTEM_CLOCK_TYPE(type) (((type) == BL_SYSTEM_CLOCK_ROOT) ||   \
                                       ((type) == BL_SYSTEM_CLOCK_FCLK) ||   \
                                       ((type) == BL_SYSTEM_CLOCK_HCLK) ||   \
                                       ((type) == BL_SYSTEM_CLOCK_BCLK) ||   \
                                       ((type) == BL_SYSTEM_CLOCK_F32K) ||   \
                                       ((type) == BL_SYSTEM_CLOCK_XCLK) ||   \
                                       ((type) == BL_SYSTEM_CLOCK_XTAL) ||   \
                                       ((type) == BL_SYSTEM_CLOCK_DIG32K) || \
                                       ((type) == BL_SYSTEM_CLOCK_MTIMER) || \
                                       ((type) == BL_SYSTEM_CLOCK_MAX))

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
/*----------*/
uint32_t SystemCoreClockGet(void);
/*----------*/
uint32_t CPU_Get_MTimer_Source_Clock(void);
uint32_t CPU_Get_MTimer_Clock(void);
uint64_t CPU_Get_MTimer_Counter(void);
uint64_t CPU_Get_CPU_Cycle(void);
uint64_t CPU_Get_MTimer_US(void);
uint64_t CPU_Get_MTimer_MS(void);
BL_Err_Type CPU_MTimer_Delay_US(uint32_t cnt);
BL_Err_Type CPU_MTimer_Delay_MS(uint32_t cnt);

/*@} end of group CLOCK_Public_Functions */

/*@} end of group CLOCK */

/*@} end of group BL702L_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL702L_CLOCK_H__ */
