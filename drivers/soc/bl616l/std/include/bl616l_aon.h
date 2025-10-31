/**
  ******************************************************************************
  * @file    bl616l_aon.h
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
#ifndef __BL616L_AON_H__
#define __BL616L_AON_H__

#include "aon_reg.h"
#include "glb_reg.h"
#include "hbn_reg.h"
#include "pds_reg.h"
#include "bflb_efuse.h"
#include "bl616l_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  AON
 *  @{
 */

/** @defgroup  AON_Public_Types
 *  @{
 */

/*@} end of group AON_Public_Types */

/** @defgroup  AON_Public_Constants
 *  @{
 */

/*@} end of group AON_Public_Constants */

/** @defgroup  AON_Public_Macros
 *  @{
 */

#define AON_LDO18_IO_LEVEL_1P65          (0)  /*!< AON LDO18 IO voltage 1.65V */
#define AON_LDO18_IO_LEVEL_1P70          (1)  /*!< AON LDO18 IO voltage 1.70V */
#define AON_LDO18_IO_LEVEL_1P75          (2)  /*!< AON LDO18 IO voltage 1.75V */
#define AON_LDO18_IO_LEVEL_1P80          (3)  /*!< AON LDO18 IO voltage 1.80V */
#define AON_LDO18_IO_LEVEL_1P85          (4)  /*!< AON LDO18 IO voltage 1.85V */
#define AON_LDO18_IO_LEVEL_1P90          (5)  /*!< AON LDO18 IO voltage 1.90V */
#define AON_LDO18_IO_LEVEL_1P95          (6)  /*!< AON LDO18 IO voltage 1.95V */
#define AON_LDO18_IO_LEVEL_2P00          (7)  /*!< AON LDO18 IO voltage 2.00V */
#define AON_LDO18_IO_LEVEL_2P60          (8)  /*!< AON LDO18 IO voltage 2.60V */
#define AON_LDO18_IO_LEVEL_2P70          (9)  /*!< AON LDO18 IO voltage 2.70V */
#define AON_LDO18_IO_LEVEL_2P80          (10) /*!< AON LDO18 IO voltage 2.80V */
#define AON_LDO18_IO_LEVEL_2P90          (11) /*!< AON LDO18 IO voltage 2.90V */
#define AON_LDO18_IO_LEVEL_2P95          (12) /*!< AON LDO18 IO voltage 2.95V */
#define AON_LDO18_IO_LEVEL_3P00          (13) /*!< AON LDO18 IO voltage 3.00V */
#define AON_LDO18_IO_LEVEL_3P05          (14) /*!< AON LDO18 IO voltage 3.05V */
#define AON_LDO18_IO_LEVEL_3P10          (15) /*!< AON LDO18 IO voltage 3.10V */
/**
  * @}
  */

/** @defgroup AON LDO18 IO TRIM type definition
  * @{
  */
#define AON_LDO18_IO_TRIM_0P93           (0)  /*!< AON LDO18 IO Trim 93% */
#define AON_LDO18_IO_TRIM_0P94           (1)  /*!< AON LDO18 IO Trim 94% */
#define AON_LDO18_IO_TRIM_0P95           (2)  /*!< AON LDO18 IO Trim 95% */
#define AON_LDO18_IO_TRIM_0P96           (3)  /*!< AON LDO18 IO Trim 96% */
#define AON_LDO18_IO_TRIM_0P97           (4)  /*!< AON LDO18 IO Trim 97% */
#define AON_LDO18_IO_TRIM_0P98           (5)  /*!< AON LDO18 IO Trim 98% */
#define AON_LDO18_IO_TRIM_0P99           (6)  /*!< AON LDO18 IO Trim 99% */
#define AON_LDO18_IO_TRIM_1P00           (7)  /*!< AON LDO18 IO Trim 100% */
#define AON_LDO18_IO_TRIM_1P01           (8)  /*!< AON LDO18 IO Trim 101% */
#define AON_LDO18_IO_TRIM_1P02           (9)  /*!< AON LDO18 IO Trim 102% */
#define AON_LDO18_IO_TRIM_1P03           (10) /*!< AON LDO18 IO Trim 103% */
#define AON_LDO18_IO_TRIM_1P04           (11) /*!< AON LDO18 IO Trim 104% */
#define AON_LDO18_IO_TRIM_1P05           (12) /*!< AON LDO18 IO Trim 105% */
#define AON_LDO18_IO_TRIM_1P06           (13) /*!< AON LDO18 IO Trim 106% */
#define AON_LDO18_IO_TRIM_1P07           (14) /*!< AON LDO18 IO Trim 107% */
#define AON_LDO18_IO_TRIM_1P08           (15) /*!< AON LDO18 IO Trim 108% */
/**
  * @}
  */

#define IS_AON_LDO18_IO_LEVEL_TYPE(type) ((type == AON_LDO18_IO_LEVEL_1P65) || \
                                          (type == AON_LDO18_IO_LEVEL_1P70) || \
                                          (type == AON_LDO18_IO_LEVEL_1P75) || \
                                          (type == AON_LDO18_IO_LEVEL_1P80) || \
                                          (type == AON_LDO18_IO_LEVEL_1P85) || \
                                          (type == AON_LDO18_IO_LEVEL_1P90) || \
                                          (type == AON_LDO18_IO_LEVEL_1P95) || \
                                          (type == AON_LDO18_IO_LEVEL_2P00) || \
                                          (type == AON_LDO18_IO_LEVEL_2P60) || \
                                          (type == AON_LDO18_IO_LEVEL_2P70) || \
                                          (type == AON_LDO18_IO_LEVEL_2P80) || \
                                          (type == AON_LDO18_IO_LEVEL_2P90) || \
                                          (type == AON_LDO18_IO_LEVEL_2P95) || \
                                          (type == AON_LDO18_IO_LEVEL_3P00) || \
                                          (type == AON_LDO18_IO_LEVEL_3P05) || \
                                          (type == AON_LDO18_IO_LEVEL_3P10))

/** @defgroup AON_LDO18_IO_TRIM_TYPE
 *  @{
 */
#define IS_AON_LDO18_IO_TRIM_TYPE(type) ((type == AON_LDO18_IO_TRIM_0P93) || \
                                         (type == AON_LDO18_IO_TRIM_0P94) || \
                                         (type == AON_LDO18_IO_TRIM_0P95) || \
                                         (type == AON_LDO18_IO_TRIM_0P96) || \
                                         (type == AON_LDO18_IO_TRIM_0P97) || \
                                         (type == AON_LDO18_IO_TRIM_0P98) || \
                                         (type == AON_LDO18_IO_TRIM_0P99) || \
                                         (type == AON_LDO18_IO_TRIM_1P00) || \
                                         (type == AON_LDO18_IO_TRIM_1P01) || \
                                         (type == AON_LDO18_IO_TRIM_1P02) || \
                                         (type == AON_LDO18_IO_TRIM_1P03) || \
                                         (type == AON_LDO18_IO_TRIM_1P04) || \
                                         (type == AON_LDO18_IO_TRIM_1P05) || \
                                         (type == AON_LDO18_IO_TRIM_1P06) || \
                                         (type == AON_LDO18_IO_TRIM_1P07) || \
                                         (type == AON_LDO18_IO_TRIM_1P08))

/*@} end of group AON_Public_Macros */

/** @defgroup  AON_Public_Functions
 *  @{
 */
BL_Err_Type AON_Power_On_RF(void);
BL_Err_Type AON_Power_Off_RF(void);
BL_Err_Type AON_Power_On_XTAL(void);
BL_Err_Type AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut);
uint8_t AON_Get_Xtal_CapCode(void);
BL_Err_Type AON_Power_Off_XTAL(void);
BL_Err_Type AON_XTAL_Enter_LowPower(void);
BL_Err_Type AON_XTAL_Exit_LowPower(void);
BL_Err_Type AON_LowPower_Enter_PDS0(void);
BL_Err_Type AON_LowPower_Exit_PDS0(void);
BL_Err_Type AON_Set_LDO11_SOC_Sstart_Delay(uint8_t delay);
BL_Err_Type AON_LDO18_IO_Switch_Efuse(uint8_t en);
BL_Err_Type AON_LDO18_IO_Switch_Psram(uint8_t en);
BL_Err_Type AON_LDO18_IO_Switch_Flash(uint8_t en);
BL_Err_Type AON_Set_LDO18_IO_Sstart_Delay(uint8_t delay);
BL_Err_Type AON_SW_Set_Ldo18_IO_Vout(uint8_t ldoLevel);
BL_Err_Type AON_SW_Trim_Ldo18_IO_Vout(uint8_t trim);
BL_Err_Type AON_Trim_Ldo18_IO_Vout(void);
/*----------*/

/*@} end of group AON_Public_Functions */

/*@} end of group AON */

/*@} end of group BL616L_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616L_AON_H__ */
