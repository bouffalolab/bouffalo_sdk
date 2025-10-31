/**
  ******************************************************************************
  * @file    bl616d_aon.h
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
#ifndef __BL616D_AON_H__
#define __BL616D_AON_H__

#include "aon_reg.h"
#include "glb_reg.h"
#include "hbn_reg.h"
#include "pds_reg.h"
#include "bl616d_common.h"
#include "bflb_efuse.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616D_Peripheral_Driver
 *  @{
 */

/** @addtogroup  AON
 *  @{
 */

/** @defgroup  AON_Public_Types
 *  @{
 */

/** @defgroup AON LDO18_AON level type definition
  * @{
  */
#define AON_LDO18_AON_LEVEL_1P65V         (0) /*!< AON LDO18 AON voltage 1.65V */
#define AON_LDO18_AON_LEVEL_1P70V         (1) /*!< AON LDO18 AON voltage 1.70V */
#define AON_LDO18_AON_LEVEL_1P75V         (2) /*!< AON LDO18 AON voltage 1.75V */
#define AON_LDO18_AON_LEVEL_1P80V         (3) /*!< AON LDO18 AON voltage 1.80V */
#define AON_LDO18_AON_LEVEL_1P85V         (4) /*!< AON LDO18 AON voltage 1.85V */
#define AON_LDO18_AON_LEVEL_1P90V         (5) /*!< AON LDO18 AON voltage 1.90V */
#define AON_LDO18_AON_LEVEL_1P95V         (6) /*!< AON LDO18 AON voltage 1.95V */
#define AON_LDO18_AON_LEVEL_2P00V         (7) /*!< AON LDO18 AON voltage 2.00V */
/**
  * @}
  */

/** @defgroup AON LDO08_AON level type definition
  * @{
  */
#define AON_LDO08_AON_LEVEL_0P575V        (0)  /*!< AON LDO08 AON voltage 0.575V */
#define AON_LDO08_AON_LEVEL_0P600V        (1)  /*!< AON LDO08 AON voltage 0.600V */
#define AON_LDO08_AON_LEVEL_0P625V        (2)  /*!< AON LDO08 AON voltage 0.625V */
#define AON_LDO08_AON_LEVEL_0P650V        (3)  /*!< AON LDO08 AON voltage 0.650V */
#define AON_LDO08_AON_LEVEL_0P675V        (4)  /*!< AON LDO08 AON voltage 0.675V */
#define AON_LDO08_AON_LEVEL_0P700V        (5)  /*!< AON LDO08 AON voltage 0.700V */
#define AON_LDO08_AON_LEVEL_0P725V        (6)  /*!< AON LDO08 AON voltage 0.725V */
#define AON_LDO08_AON_LEVEL_0P750V        (7)  /*!< AON LDO08 AON voltage 0.750V */
#define AON_LDO08_AON_LEVEL_0P775V        (8)  /*!< AON LDO08 AON voltage 0.775V */
#define AON_LDO08_AON_LEVEL_0P800V        (9)  /*!< AON LDO08 AON voltage 0.800V */
#define AON_LDO08_AON_LEVEL_0P825V        (10) /*!< AON LDO08 AON voltage 0.825V */
#define AON_LDO08_AON_LEVEL_0P850V        (11) /*!< AON LDO08 AON voltage 0.850V */
#define AON_LDO08_AON_LEVEL_0P875V        (12) /*!< AON LDO08 AON voltage 0.875V */
#define AON_LDO08_AON_LEVEL_0P900V        (13) /*!< AON LDO08 AON voltage 0.900V */
#define AON_LDO08_AON_LEVEL_0P925V        (14) /*!< AON LDO08 AON voltage 0.925V */
#define AON_LDO08_AON_LEVEL_0P950V        (15) /*!< AON LDO08 AON voltage 0.950V */
/**
  * @}
  */

/** @defgroup AON DCDC_SYS level type definition
  * @{
  */
#define AON_DCDC_SYS_LEVEL_1P125V         (0)  /*!< AON DCDC SYS voltage 1.125V */
#define AON_DCDC_SYS_LEVEL_1P150V         (1)  /*!< AON DCDC SYS voltage 1.150V */
#define AON_DCDC_SYS_LEVEL_1P175V         (2)  /*!< AON DCDC SYS voltage 1.175V */
#define AON_DCDC_SYS_LEVEL_1P200V         (3)  /*!< AON DCDC SYS voltage 1.200V */
#define AON_DCDC_SYS_LEVEL_1P225V         (4)  /*!< AON DCDC SYS voltage 1.225V */
#define AON_DCDC_SYS_LEVEL_1P250V         (5)  /*!< AON DCDC SYS voltage 1.250V */
#define AON_DCDC_SYS_LEVEL_1P275V         (6)  /*!< AON DCDC SYS voltage 1.275V */
#define AON_DCDC_SYS_LEVEL_1P300V         (7)  /*!< AON DCDC SYS voltage 1.300V */
#define AON_DCDC_SYS_LEVEL_1P325V         (8)  /*!< AON DCDC SYS voltage 1.325V */
#define AON_DCDC_SYS_LEVEL_1P350V         (9)  /*!< AON DCDC SYS voltage 1.350V */
#define AON_DCDC_SYS_LEVEL_1P375V         (10) /*!< AON DCDC SYS voltage 1.375V */
#define AON_DCDC_SYS_LEVEL_1P400V         (11) /*!< AON DCDC SYS voltage 1.400V */
#define AON_DCDC_SYS_LEVEL_1P425V         (12) /*!< AON DCDC SYS voltage 1.425V */
#define AON_DCDC_SYS_LEVEL_1P450V         (13) /*!< AON DCDC SYS voltage 1.450V */
#define AON_DCDC_SYS_LEVEL_1P475V         (14) /*!< AON DCDC SYS voltage 1.475V */
#define AON_DCDC_SYS_LEVEL_1P500V         (15) /*!< AON DCDC SYS voltage 1.500V */
#define AON_DCDC_SYS_LEVEL_1P525V         (16) /*!< AON DCDC SYS voltage 1.525V */
#define AON_DCDC_SYS_LEVEL_1P550V         (17) /*!< AON DCDC SYS voltage 1.550V */
#define AON_DCDC_SYS_LEVEL_1P575V         (18) /*!< AON DCDC SYS voltage 1.575V */
#define AON_DCDC_SYS_LEVEL_1P600V         (19) /*!< AON DCDC SYS voltage 1.600V */
#define AON_DCDC_SYS_LEVEL_1P625V         (20) /*!< AON DCDC SYS voltage 1.625V */
#define AON_DCDC_SYS_LEVEL_1P650V         (21) /*!< AON DCDC SYS voltage 1.650V */
#define AON_DCDC_SYS_LEVEL_1P675V         (22) /*!< AON DCDC SYS voltage 1.675V */
#define AON_DCDC_SYS_LEVEL_1P700V         (23) /*!< AON DCDC SYS voltage 1.700V */
#define AON_DCDC_SYS_LEVEL_1P725V         (24) /*!< AON DCDC SYS voltage 1.725V */
#define AON_DCDC_SYS_LEVEL_1P750V         (25) /*!< AON DCDC SYS voltage 1.750V */
#define AON_DCDC_SYS_LEVEL_1P775V         (26) /*!< AON DCDC SYS voltage 1.775V */
#define AON_DCDC_SYS_LEVEL_1P800V         (27) /*!< AON DCDC SYS voltage 1.800V */
#define AON_DCDC_SYS_LEVEL_1P825V         (28) /*!< AON DCDC SYS voltage 1.825V */
#define AON_DCDC_SYS_LEVEL_1P850V         (29) /*!< AON DCDC SYS voltage 1.850V */
#define AON_DCDC_SYS_LEVEL_1P875V         (30) /*!< AON DCDC SYS voltage 1.875V */
#define AON_DCDC_SYS_LEVEL_1P900V         (31) /*!< AON DCDC SYS voltage 1.900V */
/**
  * @}
  */

/** @defgroup AON LDO0_SOC level type definition
  * @{
  */
#define AON_LDO_SOC_LEVEL_0P700V          (0)  /*!< AON LDO SOC voltage 0.700V */
#define AON_LDO_SOC_LEVEL_0P725V          (1)  /*!< AON LDO SOC voltage 0.725V */
#define AON_LDO_SOC_LEVEL_0P750V          (2)  /*!< AON LDO SOC voltage 0.750V */
#define AON_LDO_SOC_LEVEL_0P775V          (3)  /*!< AON LDO SOC voltage 0.775V */
#define AON_LDO_SOC_LEVEL_0P800V          (4)  /*!< AON LDO SOC voltage 0.800V */
#define AON_LDO_SOC_LEVEL_0P825V          (5)  /*!< AON LDO SOC voltage 0.825V */
#define AON_LDO_SOC_LEVEL_0P850V          (6)  /*!< AON LDO SOC voltage 0.850V */
#define AON_LDO_SOC_LEVEL_0P875V          (7)  /*!< AON LDO SOC voltage 0.875V */
#define AON_LDO_SOC_LEVEL_0P900V          (8)  /*!< AON LDO SOC voltage 0.900V */
#define AON_LDO_SOC_LEVEL_0P925V          (9)  /*!< AON LDO SOC voltage 0.925V */
#define AON_LDO_SOC_LEVEL_0P950V          (10) /*!< AON LDO SOC voltage 0.950V */
#define AON_LDO_SOC_LEVEL_0P975V          (11) /*!< AON LDO SOC voltage 0.975V */
#define AON_LDO_SOC_LEVEL_1P000V          (12) /*!< AON LDO SOC voltage 1.000V */
#define AON_LDO_SOC_LEVEL_1P025V          (13) /*!< AON LDO SOC voltage 1.025V */
#define AON_LDO_SOC_LEVEL_1P050V          (14) /*!< AON LDO SOC voltage 1.050V */
#define AON_LDO_SOC_LEVEL_1P075V          (15) /*!< AON LDO SOC voltage 1.075V */
/**
  * @}
  */

/** @defgroup AON LDO18 AON TRIM type definition
  * @{
  */
#define AON_LDO18_AON_TRIM_0P92           (0)  /*!< AON LDO18 AON Trim 92% */
#define AON_LDO18_AON_TRIM_0P93           (1)  /*!< AON LDO18 AON Trim 93% */
#define AON_LDO18_AON_TRIM_0P94           (2)  /*!< AON LDO18 AON Trim 94% */
#define AON_LDO18_AON_TRIM_0P95           (3)  /*!< AON LDO18 AON Trim 95% */
#define AON_LDO18_AON_TRIM_0P96           (4)  /*!< AON LDO18 AON Trim 96% */
#define AON_LDO18_AON_TRIM_0P97           (5)  /*!< AON LDO18 AON Trim 97% */
#define AON_LDO18_AON_TRIM_0P98           (6)  /*!< AON LDO18 AON Trim 98% */
#define AON_LDO18_AON_TRIM_0P99           (7)  /*!< AON LDO18 AON Trim 99% */
#define AON_LDO18_AON_TRIM_1P00           (8)  /*!< AON LDO18 AON Trim 100% */
#define AON_LDO18_AON_TRIM_1P01           (9)  /*!< AON LDO18 AON Trim 101% */
#define AON_LDO18_AON_TRIM_1P02           (10) /*!< AON LDO18 AON Trim 102% */
#define AON_LDO18_AON_TRIM_1P03           (11) /*!< AON LDO18 AON Trim 103% */
#define AON_LDO18_AON_TRIM_1P04           (12) /*!< AON LDO18 AON Trim 104% */
#define AON_LDO18_AON_TRIM_1P05           (13) /*!< AON LDO18 AON Trim 105% */
#define AON_LDO18_AON_TRIM_1P06           (14) /*!< AON LDO18 AON Trim 106% */
#define AON_LDO18_AON_TRIM_1P07           (15) /*!< AON LDO18 AON Trim 107% */
/**
  * @}
  */

/** @defgroup AON LDO08 AON TRIM type definition
  * @{
  */
#define AON_LDO08_AON_TRIM_0P92           (0)  /*!< AON LDO08 AON Trim 92% */
#define AON_LDO08_AON_TRIM_0P93           (1)  /*!< AON LDO08 AON Trim 93% */
#define AON_LDO08_AON_TRIM_0P94           (2)  /*!< AON LDO08 AON Trim 94% */
#define AON_LDO08_AON_TRIM_0P95           (3)  /*!< AON LDO08 AON Trim 95% */
#define AON_LDO08_AON_TRIM_0P96           (4)  /*!< AON LDO08 AON Trim 96% */
#define AON_LDO08_AON_TRIM_0P97           (5)  /*!< AON LDO08 AON Trim 97% */
#define AON_LDO08_AON_TRIM_0P98           (6)  /*!< AON LDO08 AON Trim 98% */
#define AON_LDO08_AON_TRIM_0P99           (7)  /*!< AON LDO08 AON Trim 99% */
#define AON_LDO08_AON_TRIM_1P00           (8)  /*!< AON LDO08 AON Trim 100% */
#define AON_LDO08_AON_TRIM_1P01           (9)  /*!< AON LDO08 AON Trim 101% */
#define AON_LDO08_AON_TRIM_1P02           (10) /*!< AON LDO08 AON Trim 102% */
#define AON_LDO08_AON_TRIM_1P03           (11) /*!< AON LDO08 AON Trim 103% */
#define AON_LDO08_AON_TRIM_1P04           (12) /*!< AON LDO08 AON Trim 104% */
#define AON_LDO08_AON_TRIM_1P05           (13) /*!< AON LDO08 AON Trim 105% */
#define AON_LDO08_AON_TRIM_1P06           (14) /*!< AON LDO08 AON Trim 106% */
#define AON_LDO08_AON_TRIM_1P07           (15) /*!< AON LDO08 AON Trim 107% */
/**
  * @}
  */

/** @defgroup AON DCDC SYS TRIM type definition
  * @{
  */
#define AON_DCDC_SYS_TRIM_0P93            (0)  /*!< AON DCDC SYS Trim 93% */
#define AON_DCDC_SYS_TRIM_0P94            (1)  /*!< AON DCDC SYS Trim 94% */
#define AON_DCDC_SYS_TRIM_0P95            (2)  /*!< AON DCDC SYS Trim 95% */
#define AON_DCDC_SYS_TRIM_0P96            (3)  /*!< AON DCDC SYS Trim 96% */
#define AON_DCDC_SYS_TRIM_0P97            (4)  /*!< AON DCDC SYS Trim 97% */
#define AON_DCDC_SYS_TRIM_0P98            (5)  /*!< AON DCDC SYS Trim 98% */
#define AON_DCDC_SYS_TRIM_0P99            (6)  /*!< AON DCDC SYS Trim 99% */
#define AON_DCDC_SYS_TRIM_1P00            (7)  /*!< AON DCDC SYS Trim 100% */
#define AON_DCDC_SYS_TRIM_1P01            (8)  /*!< AON DCDC SYS Trim 101% */
#define AON_DCDC_SYS_TRIM_1P02            (9)  /*!< AON DCDC SYS Trim 102% */
#define AON_DCDC_SYS_TRIM_1P03            (10) /*!< AON DCDC SYS Trim 103% */
#define AON_DCDC_SYS_TRIM_1P04            (11) /*!< AON DCDC SYS Trim 104% */
#define AON_DCDC_SYS_TRIM_1P05            (12) /*!< AON DCDC SYS Trim 105% */
#define AON_DCDC_SYS_TRIM_1P06            (13) /*!< AON DCDC SYS Trim 106% */
#define AON_DCDC_SYS_TRIM_1P07            (14) /*!< AON DCDC SYS Trim 107% */
#define AON_DCDC_SYS_TRIM_1P08            (15) /*!< AON DCDC SYS Trim 108% */
/**
  * @}
  */

/** @defgroup AON LDO SOC TRIM type definition
  * @{
  */
#define AON_LDO_SOC_TRIM_0P93             (0)  /*!< AON LDO SOC Trim 93% */
#define AON_LDO_SOC_TRIM_0P94             (1)  /*!< AON LDO SOC Trim 94% */
#define AON_LDO_SOC_TRIM_0P95             (2)  /*!< AON LDO SOC Trim 95% */
#define AON_LDO_SOC_TRIM_0P96             (3)  /*!< AON LDO SOC Trim 96% */
#define AON_LDO_SOC_TRIM_0P97             (4)  /*!< AON LDO SOC Trim 97% */
#define AON_LDO_SOC_TRIM_0P98             (5)  /*!< AON LDO SOC Trim 98% */
#define AON_LDO_SOC_TRIM_0P99             (6)  /*!< AON LDO SOC Trim 99% */
#define AON_LDO_SOC_TRIM_1P00             (7)  /*!< AON LDO SOC Trim 100% */
#define AON_LDO_SOC_TRIM_1P01             (8)  /*!< AON LDO SOC Trim 101% */
#define AON_LDO_SOC_TRIM_1P02             (9)  /*!< AON LDO SOC Trim 102% */
#define AON_LDO_SOC_TRIM_1P03             (10) /*!< AON LDO SOC Trim 103% */
#define AON_LDO_SOC_TRIM_1P04             (11) /*!< AON LDO SOC Trim 104% */
#define AON_LDO_SOC_TRIM_1P05             (12) /*!< AON LDO SOC Trim 105% */
#define AON_LDO_SOC_TRIM_1P06             (13) /*!< AON LDO SOC Trim 106% */
#define AON_LDO_SOC_TRIM_1P07             (14) /*!< AON LDO SOC Trim 107% */
#define AON_LDO_SOC_TRIM_1P08             (15) /*!< AON LDO SOC Trim 108% */
/**
  * @}
  */

/** @defgroup WDG RESET TIME definition
  * @{
  */
#define WDG_RESET_TIME_556US              (2)
#define WDG_RESET_TIME_867US              (4)
#define WDG_RESET_TIME_1MS7               (5)
#define WDG_RESET_TIME_3MS4               (6)
#define WDG_RESET_TIME_6MS9               (7)
#define WDG_RESET_TIME_13MS8              (8)
#define WDG_RESET_TIME_27MS6              (9)
#define WDG_RESET_TIME_55MS2              (10)
#define WDG_RESET_TIME_110MS4             (11)
#define WDG_RESET_TIME_220MS8             (12)
#define WDG_RESET_TIME_441MS6             (13)
#define WDG_RESET_TIME_883MS2             (14)
#define WDG_RESET_TIME_1S76               (15)
#define WDG_RESET_TIME_3S53               (16)
#define WDG_RESET_TIME_7S06               (17)
#define WDG_RESET_TIME_14S                (18)
#define WDG_RESET_TIME_28S                (19)
/**
  * @}
  */

#define AON_DCDC_SYS_NORMAL_MODE          (0)
#define AON_DCDC_SYS_LOWPOWER_MODE        (1)

#define AON_LDO_SOC_NORMAL_MODE           (0)
#define AON_LDO_SOC_LOWPOWER_MODE         (1)

#define AON_LDO18_AON_NORMAL_MODE         (0)
#define AON_LDO18_AON_LOWPOWER_MODE       (1)

/*@} end of group AON_Public_Types */

/** @defgroup  AON_Public_Constants
 *  @{
 */

/*@} end of group AON_Public_Constants */

/** @defgroup  AON_Public_Macros
 *  @{
 */

/** @defgroup  AON_LDO18_AON_LEVEL_TYPE
 *  @{
 */
#define IS_AON_LDO18_AON_LEVEL_TYPE(type) (((type) == AON_LDO18_AON_LEVEL_1P65V) || \
                                           ((type) == AON_LDO18_AON_LEVEL_1P70V) || \
                                           ((type) == AON_LDO18_AON_LEVEL_1P75V) || \
                                           ((type) == AON_LDO18_AON_LEVEL_1P80V) || \
                                           ((type) == AON_LDO18_AON_LEVEL_1P85V) || \
                                           ((type) == AON_LDO18_AON_LEVEL_1P90V) || \
                                           ((type) == AON_LDO18_AON_LEVEL_1P95V) || \
                                           ((type) == AON_LDO18_AON_LEVEL_2P00V))

/** @defgroup  AON_LDO08_AON_LEVEL_TYPE
 *  @{
 */
#define IS_AON_LDO08_AON_LEVEL_TYPE(type) (((type) == AON_LDO08_AON_LEVEL_0P575V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P600V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P625V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P650V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P675V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P700V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P725V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P750V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P775V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P800V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P825V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P850V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P875V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P900V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P925V) || \
                                           ((type) == AON_LDO08_AON_LEVEL_0P950V))

/** @defgroup  AON_DCDC_SYS_LEVEL_TYPE
 *  @{
 */
#define IS_AON_DCDC_SYS_LEVEL_TYPE(type) (((type) == AON_DCDC_SYS_LEVEL_1P125V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P150V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P175V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P200V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P225V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P250V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P275V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P300V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P325V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P350V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P375V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P400V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P425V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P450V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P475V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P500V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P525V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P550V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P575V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P600V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P625V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P650V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P675V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P700V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P725V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P750V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P775V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P800V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P825V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P850V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P875V) || \
                                          ((type) == AON_DCDC_SYS_LEVEL_1P900V))

/** @defgroup  AON_LDO_SOC_LEVEL_TYPE
 *  @{
 */
#define IS_AON_LDO_SOC_LEVEL_TYPE(type) (((type) == AON_LDO_SOC_LEVEL_0P700V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P725V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P750V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P775V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P800V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P825V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P850V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P875V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P900V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P925V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P950V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_0P975V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_1P000V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_1P025V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_1P050V) || \
                                         ((type) == AON_LDO_SOC_LEVEL_1P075V))

/** @defgroup  AON_LDO18_AON_TRIM_TYPE
 *  @{
 */
#define IS_AON_LDO18_AON_TRIM_TYPE(type) ((type == AON_LDO18_AON_TRIM_0P92) || \
                                          (type == AON_LDO18_AON_TRIM_0P93) || \
                                          (type == AON_LDO18_AON_TRIM_0P94) || \
                                          (type == AON_LDO18_AON_TRIM_0P95) || \
                                          (type == AON_LDO18_AON_TRIM_0P96) || \
                                          (type == AON_LDO18_AON_TRIM_0P97) || \
                                          (type == AON_LDO18_AON_TRIM_0P98) || \
                                          (type == AON_LDO18_AON_TRIM_0P99) || \
                                          (type == AON_LDO18_AON_TRIM_1P00) || \
                                          (type == AON_LDO18_AON_TRIM_1P01) || \
                                          (type == AON_LDO18_AON_TRIM_1P02) || \
                                          (type == AON_LDO18_AON_TRIM_1P03) || \
                                          (type == AON_LDO18_AON_TRIM_1P04) || \
                                          (type == AON_LDO18_AON_TRIM_1P05) || \
                                          (type == AON_LDO18_AON_TRIM_1P06) || \
                                          (type == AON_LDO18_AON_TRIM_1P07))

/** @defgroup  AON_LDO08_AON_TRIM_TYPE
 *  @{
 */
#define IS_AON_LDO08_AON_TRIM_TYPE(type) ((type == AON_LDO08_AON_TRIM_0P92) || \
                                          (type == AON_LDO08_AON_TRIM_0P93) || \
                                          (type == AON_LDO08_AON_TRIM_0P94) || \
                                          (type == AON_LDO08_AON_TRIM_0P95) || \
                                          (type == AON_LDO08_AON_TRIM_0P96) || \
                                          (type == AON_LDO08_AON_TRIM_0P97) || \
                                          (type == AON_LDO08_AON_TRIM_0P98) || \
                                          (type == AON_LDO08_AON_TRIM_0P99) || \
                                          (type == AON_LDO08_AON_TRIM_1P00) || \
                                          (type == AON_LDO08_AON_TRIM_1P01) || \
                                          (type == AON_LDO08_AON_TRIM_1P02) || \
                                          (type == AON_LDO08_AON_TRIM_1P03) || \
                                          (type == AON_LDO08_AON_TRIM_1P04) || \
                                          (type == AON_LDO08_AON_TRIM_1P05) || \
                                          (type == AON_LDO08_AON_TRIM_1P06) || \
                                          (type == AON_LDO08_AON_TRIM_1P07))

/** @defgroup  AON_DCDC_SYS_TRIM_TYPE
 *  @{
 */
#define IS_AON_DCDC_SYS_TRIM_TYPE(type) ((type == AON_DCDC_SYS_TRIM_0P93) || \
                                         (type == AON_DCDC_SYS_TRIM_0P94) || \
                                         (type == AON_DCDC_SYS_TRIM_0P95) || \
                                         (type == AON_DCDC_SYS_TRIM_0P96) || \
                                         (type == AON_DCDC_SYS_TRIM_0P97) || \
                                         (type == AON_DCDC_SYS_TRIM_0P98) || \
                                         (type == AON_DCDC_SYS_TRIM_0P99) || \
                                         (type == AON_DCDC_SYS_TRIM_1P00) || \
                                         (type == AON_DCDC_SYS_TRIM_1P01) || \
                                         (type == AON_DCDC_SYS_TRIM_1P02) || \
                                         (type == AON_DCDC_SYS_TRIM_1P03) || \
                                         (type == AON_DCDC_SYS_TRIM_1P04) || \
                                         (type == AON_DCDC_SYS_TRIM_1P05) || \
                                         (type == AON_DCDC_SYS_TRIM_1P06) || \
                                         (type == AON_DCDC_SYS_TRIM_1P07) || \
                                         (type == AON_DCDC_SYS_TRIM_1P08))

/** @defgroup  AON_LDO_SOC_TRIM_TYPE
 *  @{
 */
#define IS_AON_LDO_SOC_TRIM_TYPE(type) ((type == AON_LDO_SOC_TRIM_0P93) || \
                                        (type == AON_LDO_SOC_TRIM_0P94) || \
                                        (type == AON_LDO_SOC_TRIM_0P95) || \
                                        (type == AON_LDO_SOC_TRIM_0P96) || \
                                        (type == AON_LDO_SOC_TRIM_0P97) || \
                                        (type == AON_LDO_SOC_TRIM_0P98) || \
                                        (type == AON_LDO_SOC_TRIM_0P99) || \
                                        (type == AON_LDO_SOC_TRIM_1P00) || \
                                        (type == AON_LDO_SOC_TRIM_1P01) || \
                                        (type == AON_LDO_SOC_TRIM_1P02) || \
                                        (type == AON_LDO_SOC_TRIM_1P03) || \
                                        (type == AON_LDO_SOC_TRIM_1P04) || \
                                        (type == AON_LDO_SOC_TRIM_1P05) || \
                                        (type == AON_LDO_SOC_TRIM_1P06) || \
                                        (type == AON_LDO_SOC_TRIM_1P07) || \
                                        (type == AON_LDO_SOC_TRIM_1P08))

#define IS_AON_LDO18_AON_MODE_TYPE(type) ((type == AON_LDO18_AON_NORMAL_MODE) || \
                                          (type == AON_LDO18_AON_LOWPOWER_MODE))

#define IS_AON_DCDC_SYS_MODE_TYPE(type) ((type == AON_DCDC_SYS_NORMAL_MODE) || \
                                         (type == AON_DCDC_SYS_LOWPOWER_MODE))

#define IS_AON_LDO_SOC_MODE_TYPE(type) ((type == AON_LDO_SOC_NORMAL_MODE) || \
                                        (type == AON_LDO_SOC_LOWPOWER_MODE))

/*@} end of group AON_Public_Macros */

/** @defgroup  AON_Public_Functions
 *  @{
 */
/*----------*/
BL_Err_Type AON_Power_On_MBG(void);
BL_Err_Type AON_Power_Off_MBG(void);
/*----------*/
BL_Err_Type AON_Power_On_SFReg(void);
BL_Err_Type AON_Power_Off_SFReg(void);
/*----------*/
BL_Err_Type AON_Power_On_RF(void);
BL_Err_Type AON_Power_Off_RF(void);
/*----------*/
BL_Err_Type AON_Power_On_XTAL(void);
BL_Err_Type AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut);
uint8_t AON_Get_Xtal_CapCode(void);
BL_Err_Type AON_Power_Off_XTAL(void);
/*----------*/
BL_Err_Type AON_Power_On_BG(void);
BL_Err_Type AON_Power_Off_BG(void);
/*----------*/
BL_Err_Type AON_Power_On_LDO15_RF(void);
BL_Err_Type AON_Power_Off_LDO15_RF(void);
BL_Err_Type AON_Output_Float_LDO15_RF(void);
BL_Err_Type AON_Output_Pulldown_LDO15_RF(void);
BL_Err_Type AON_Output_LDO15_RF_Ctrl_by_Pd(void);
/*----------*/
// BL_Err_Type AON_Power_On_MicBias(void);
// BL_Err_Type AON_Power_Off_MicBias(void);
/*----------*/
// BL_Err_Type AON_LowPower_Enter_PDS0(void);
// BL_Err_Type AON_LowPower_Exit_PDS0(void);
/*----------*/
BL_Err_Type AON_Set_Xtal_32K_Inverter_Amplify_Strength(uint8_t value);
BL_Err_Type AON_Set_Xtal_32K_Regulator(uint8_t level);
BL_Err_Type AON_Start_Xtal_32K(void);
BL_Err_Type AON_Clear_Xtal_32K_Fast_Startup(void);
BL_Sts_Type AON_Get_Xtal_32K_Ready_State(void);
BL_Err_Type AON_Power_On_Xtal_32K(void);
BL_Err_Type AON_Power_Off_Xtal_32K(void);
/*----------*/
BL_Err_Type AON_Set_Ldo18_Aon_Vout(uint8_t level);
BL_Err_Type AON_SW_Trim_Ldo18_Aon_Vout(uint8_t trim);
BL_Err_Type AON_Set_Ldo08_Aon_Vout(uint8_t level);
BL_Err_Type AON_SW_Trim_Ldo08_Aon_Vout(uint8_t trim);
BL_Err_Type AON_Set_Ldo_Soc_Vout(uint8_t level);
BL_Err_Type AON_SW_Trim_Ldo_Soc_Vout(uint8_t trim);
BL_Err_Type AON_Set_Dcdc_Sys_Vout(uint8_t level);
BL_Err_Type AON_SW_Trim_Dcdc_Sys_Vout(uint8_t trim);
/*----------*/
BL_Err_Type AON_Set_Switch_For_Efuse(uint8_t enable);
BL_Err_Type AON_Set_Ldo18_AON_Power_Switch_For_FLASH(uint8_t enable);
BL_Err_Type AON_Set_Ldo18_AON_Power_Switch_For_PSRAM(uint8_t enable);
/*----------*/
BL_Err_Type AON_Set_Ldo18_Aon_Vout_in_Lowpower(uint8_t level);
BL_Err_Type AON_Set_Dcdc_Sys_Vout_in_Lowpower(uint8_t level);
BL_Err_Type AON_Set_Ldo_Soc_Vout_in_Lowpower(uint8_t level);
/*----------*/
BL_Err_Type AON_Ctrl_Ldo18_Aon_Mode_by_HW(uint8_t enable);
BL_Err_Type AON_Ctrl_Dcdc_Sys_Mode_by_HW(uint8_t enable);
BL_Err_Type AON_Ctrl_Ldo_Soc_Mode_by_HW(uint8_t enable);
/*----------*/
BL_Err_Type AON_Set_Ldo18_Aon_Mode(uint8_t mode);
BL_Err_Type AON_Set_Dcdc_Sys_Mode(uint8_t mode);
BL_Err_Type AON_Set_Ldo_Soc_Mode(uint8_t mode);
/*----------*/
BL_Err_Type AON_Set_Dcdc_Sys_Sstart_Delay(uint8_t delay);
BL_Sts_Type AON_Get_BOD_OUT_State(void);
BL_Err_Type AON_Set_BOD_Config(uint8_t enable, uint8_t threshold, uint8_t mode);
/*----------*/
void AON_Wdg_Init(uint16_t comp_val);
void AON_Wdg_Start(void);
BL_Sts_Type AON_Wdg_Status_Get(void);
void AON_Wdg_Status_Clear(void);
void AON_Wdg_Kick(void);
/*----------*/
void AON_Set_RC32M_Speed_As_8M(uint8_t enable);
void AON_Set_RC32M_Code_Fr_Ext(uint8_t code);
/*----------*/
BL_Err_Type AON_Trim_Ldo_Soc_Vout(void);
BL_Err_Type AON_Trim_Ldo08_Aon_Vout(void);
BL_Err_Type AON_Trim_Ldo18_Aon_Vout(void);
BL_Err_Type HBN_Trim_Dcdc12_Sys_Vout(void);

/*@} end of group AON_Public_Functions */

/*@} end of group AON */

/*@} end of group BL616D_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616D_AON_H__ */
