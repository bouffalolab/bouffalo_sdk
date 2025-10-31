/**
  ******************************************************************************
  * @file    bl616l_ef_cfg.h
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
#ifndef __BL616L_EF_CFG_H__
#define __BL616L_EF_CFG_H__

#include "bl616l_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  EF_CTRL
 *  @{
 */

/** @defgroup  EF_CTRL_Public_Types
 *  @{
 */

typedef struct {
    // ef_ana_trim1 (0x1c)
    uint32_t mac_addr2_low : 32; // Bits [31:0]
    // End of ef_ana_trim1

    // ef_ana_trim2 (0x20)
    uint32_t mac_addr3_low : 32; // Bits [31:0]
    // End of ef_ana_trim2

    // ef_ana_trim3 (0x24)
    uint32_t mac_addr3_high : 16; // Bits [15:0]
    uint32_t mac_addr2_high : 16; // Bits [31:16]
    // End of ef_ana_trim3

    // ef_ana_trim4 (0x28)
    uint32_t xtal_capcode_2_inout        : 8; // Bits [7:0]
    uint32_t xtal_capcode_2_inout_parity : 1; // Bits [8]
    uint32_t xtal_capcode_2_inout_en     : 1; // Bits [9]
    uint32_t xtal_capcode_1_inout        : 8; // Bits [17:10]
    uint32_t xtal_capcode_1_inout_parity : 1; // Bits [18]
    uint32_t xtal_capcode_1_inout_en     : 1; // Bits [19]
    uint32_t mac_addr3_crc               : 6; // Bits [25:20]
    uint32_t mac_addr2_crc               : 6; // Bits [31:26]
    // End of ef_ana_trim4

    // ef_ana_trim5 (0x2c)
    uint32_t tx_wf_hp_offset_qdb_low_2    : 5; // Bits [4:0]
    uint32_t tx_wf_hp_offset_qdb_mid_2    : 5; // Bits [9:5]
    uint32_t tx_wf_hp_offset_qdb_high_2   : 5; // Bits [14:10]
    uint32_t tx_wf_hp_offset_qdb_low_1    : 5; // Bits [19:15]
    uint32_t tx_wf_hp_offset_qdb_mid_1    : 5; // Bits [24:20]
    uint32_t tx_wf_hp_offset_qdb_high_1   : 5; // Bits [29:25]
    uint32_t tx_wf_hp_offset_qdb_1_parity : 1; // Bits [30]
    uint32_t tx_wf_hp_offset_qdb_1_en     : 1; // Bits [31]
    // End of ef_ana_trim5

    // ef_ana_trim6 (0x30)
    uint32_t tx_wf_lp_offset_qdb_low_2    : 5; // Bits [4:0]
    uint32_t tx_wf_lp_offset_qdb_mid_2    : 5; // Bits [9:5]
    uint32_t tx_wf_lp_offset_qdb_high_2   : 5; // Bits [14:10]
    uint32_t tx_wf_lp_offset_qdb_low_1    : 5; // Bits [19:15]
    uint32_t tx_wf_lp_offset_qdb_mid_1    : 5; // Bits [24:20]
    uint32_t tx_wf_lp_offset_qdb_high_1   : 5; // Bits [29:25]
    uint32_t tx_wf_hp_offset_qdb_2_parity : 1; // Bits [30]
    uint32_t tx_wf_hp_offset_qdb_2_en     : 1; // Bits [31]
    // End of ef_ana_trim6

    // ef_ana_trim7 (0x34)
    uint32_t tx_bz_offset_qdb_2_en        : 1; // Bits [0]
    uint32_t tx_bz_offset_qdb_2412_1      : 5; // Bits [5:1]
    uint32_t tx_bz_offset_qdb_2432_1      : 5; // Bits [10:6]
    uint32_t tx_bz_offset_qdb_2448_1      : 5; // Bits [15:11]
    uint32_t tx_bz_offset_qdb_2464_1      : 5; // Bits [20:16]
    uint32_t tx_bz_offset_qdb_2476_1      : 5; // Bits [25:21]
    uint32_t tx_bz_offset_qdb_1_parity    : 1; // Bits [26]
    uint32_t tx_bz_offset_qdb_1_en        : 1; // Bits [27]
    uint32_t tx_wf_lp_offset_qdb_2_parity : 1; // Bits [28]
    uint32_t tx_wf_lp_offset_qdb_2_en     : 1; // Bits [29]
    uint32_t tx_wf_lp_offset_qdb_1_parity : 1; // Bits [30]
    uint32_t tx_wf_lp_offset_qdb_1_en     : 1; // Bits [31]
    // End of ef_ana_trim7

    // ef_ana_trim8 (0x38)
    uint32_t temperature_mp_2_parity     : 1; // Bits [0]
    uint32_t temperature_mp_2_en         : 1; // Bits [1]
    uint32_t temperature_mp_1_parity     : 1; // Bits [2]
    uint32_t temperature_mp_1_en         : 1; // Bits [3]
    uint32_t xtal_capcode_3_inout_parity : 1; // Bits [4]
    uint32_t xtal_capcode_3_inout_en     : 1; // Bits [5]
    uint32_t tx_bz_offset_qdb_2412_2     : 5; // Bits [10:6]
    uint32_t tx_bz_offset_qdb_2432_2     : 5; // Bits [15:11]
    uint32_t tx_bz_offset_qdb_2448_2     : 5; // Bits [20:16]
    uint32_t tx_bz_offset_qdb_2464_2     : 5; // Bits [25:21]
    uint32_t tx_bz_offset_qdb_2476_2     : 5; // Bits [30:26]
    uint32_t tx_bz_offset_qdb_2_parity   : 1; // Bits [31]
    // End of ef_ana_trim8

    // ef_ana_trim9 (0x3c)
    uint32_t temperature_mp_3     : 8; // Bits [7:0]
    uint32_t temperature_mp_2     : 8; // Bits [15:8]
    uint32_t temperature_mp_1     : 8; // Bits [23:16]
    uint32_t xtal_capcode_3_inout : 8; // Bits [31:24]
    // End of ef_ana_trim9

    // ef_ana_trim10 (0x40)
    uint32_t tx_wf_lp_offset_qdb_low_3  : 5; // Bits [4:0]
    uint32_t tx_wf_lp_offset_qdb_mid_3  : 5; // Bits [9:5]
    uint32_t tx_wf_lp_offset_qdb_high_3 : 5; // Bits [14:10]
    uint32_t tx_wf_hp_offset_qdb_low_3  : 5; // Bits [19:15]
    uint32_t tx_wf_hp_offset_qdb_mid_3  : 5; // Bits [24:20]
    uint32_t tx_wf_hp_offset_qdb_high_3 : 5; // Bits [29:25]
    uint32_t temperature_mp_3_parity    : 1; // Bits [30]
    uint32_t temperature_mp_3_en        : 1; // Bits [31]
    // End of ef_ana_trim10

    // ef_ana_trim11 (0x44)
    uint32_t reserved_0x44                : 1; // Bits [0]
    uint32_t tx_bz_offset_qdb_2412_3      : 1; // Bits [1]
    uint32_t tx_bz_offset_qdb_2432_3      : 1; // Bits [2]
    uint32_t tx_bz_offset_qdb_2448_3      : 5; // Bits [7:3]
    uint32_t tx_bz_offset_qdb_2464_3      : 5; // Bits [12:8]
    uint32_t tx_bz_offset_qdb_2476_3      : 5; // Bits [17:13]
    uint32_t tx_bz_offset_qdb_3_parity    : 5; // Bits [22:18]
    uint32_t tx_bz_offset_qdb_3_en        : 5; // Bits [27:23]
    uint32_t tx_wf_lp_offset_qdb_3_parity : 1; // Bits [28]
    uint32_t tx_wf_lp_offset_qdb_3_en     : 1; // Bits [29]
    uint32_t tx_wf_hp_offset_qdb_3_parity : 1; // Bits [30]
    uint32_t tx_wf_hp_offset_qdb_3_en     : 1; // Bits [31]
    // End of ef_ana_trim11

    // ef_ana_trim12 (0x48)
    uint32_t tx_wf_hp_offset_qdb_low_5    : 5; // Bits [4:0]
    uint32_t tx_wf_hp_offset_qdb_mid_5    : 5; // Bits [9:5]
    uint32_t tx_wf_hp_offset_qdb_high_5   : 5; // Bits [14:10]
    uint32_t tx_wf_hp_offset_qdb_low_4    : 5; // Bits [19:15]
    uint32_t tx_wf_hp_offset_qdb_mid_4    : 5; // Bits [24:20]
    uint32_t tx_wf_hp_offset_qdb_high_4   : 5; // Bits [29:25]
    uint32_t tx_wf_hp_offset_qdb_4_parity : 1; // Bits [30]
    uint32_t tx_wf_hp_offset_qdb_4_en     : 1; // Bits [31]
    // End of ef_ana_trim12

    // ef_ana_trim13 (0x4c)
    uint32_t tx_wf_lp_offset_qdb_low_4    : 5; // Bits [4:0]
    uint32_t tx_wf_lp_offset_qdb_mid_4    : 5; // Bits [9:5]
    uint32_t tx_wf_lp_offset_qdb_high_4   : 5; // Bits [14:10]
    uint32_t tx_wf_hp_offset_qdb_low_6    : 5; // Bits [19:15]
    uint32_t tx_wf_hp_offset_qdb_mid_6    : 5; // Bits [24:20]
    uint32_t tx_wf_hp_offset_qdb_high_6   : 5; // Bits [29:25]
    uint32_t tx_wf_hp_offset_qdb_5_parity : 1; // Bits [30]
    uint32_t tx_wf_hp_offset_qdb_5_en     : 1; // Bits [31]
    // End of ef_ana_trim13

    // ef_ana_trim14 (0x50)
    uint32_t tx_wf_lp_offset_qdb_6_en     : 1; // Bits [0]
    uint32_t tx_wf_lp_offset_qdb_mid_6    : 5; // Bits [5:1]
    uint32_t tx_wf_lp_offset_qdb_high_6   : 5; // Bits [10:6]
    uint32_t tx_wf_lp_offset_qdb_low_5    : 5; // Bits [15:11]
    uint32_t tx_wf_lp_offset_qdb_mid_5    : 5; // Bits [20:16]
    uint32_t tx_wf_lp_offset_qdb_high_5   : 5; // Bits [25:21]
    uint32_t tx_wf_lp_offset_qdb_5_parity : 1; // Bits [26]
    uint32_t tx_wf_lp_offset_qdb_5_en     : 1; // Bits [27]
    uint32_t tx_wf_lp_offset_qdb_4_parity : 1; // Bits [28]
    uint32_t tx_wf_lp_offset_qdb_4_en     : 1; // Bits [29]
    uint32_t tx_wf_hp_offset_qdb_6_parity : 1; // Bits [30]
    uint32_t tx_wf_hp_offset_qdb_6_en     : 1; // Bits [31]
    // End of ef_ana_trim14

    // ef_ana_trim15 (0x54)
    uint32_t temperature_mp_4_parity      : 1; // Bits [0]
    uint32_t temperature_mp_4_en          : 1; // Bits [1]
    uint32_t tx_bz_offset_qdb_5_parity    : 1; // Bits [2]
    uint32_t tx_bz_offset_qdb_5_en        : 1; // Bits [3]
    uint32_t tx_bz_offset_qdb_2432_4      : 5; // Bits [8:4]
    uint32_t tx_bz_offset_qdb_2448_4      : 5; // Bits [13:9]
    uint32_t tx_bz_offset_qdb_2464_4      : 5; // Bits [18:14]
    uint32_t tx_bz_offset_qdb_2476_4      : 5; // Bits [23:19]
    uint32_t tx_bz_offset_qdb_4_parity    : 1; // Bits [24]
    uint32_t tx_bz_offset_qdb_4_en        : 1; // Bits [25]
    uint32_t tx_wf_lp_offset_qdb_low_6    : 5; // Bits [30:26]
    uint32_t tx_wf_lp_offset_qdb_6_parity : 1; // Bits [31]
    // End of ef_ana_trim15

    // ef_ana_trim16 (0x58)
    uint32_t temperature_mp_5_parity : 1; // Bits [0]
    uint32_t temperature_mp_5_en     : 1; // Bits [1]
    uint32_t tx_bz_offset_qdb_2412_5 : 5; // Bits [6:2]
    uint32_t tx_bz_offset_qdb_2432_5 : 5; // Bits [11:7]
    uint32_t tx_bz_offset_qdb_2448_5 : 5; // Bits [16:12]
    uint32_t tx_bz_offset_qdb_2464_5 : 5; // Bits [21:17]
    uint32_t tx_bz_offset_qdb_2476_5 : 5; // Bits [26:22]
    uint32_t tx_bz_offset_qdb_2412_4 : 5; // Bits [31:27]
    // End of ef_ana_trim16

    // ef_ana_trim17 (0x5c)
    uint32_t reserved_0x5c           : 6; // Bits [5:0]
    uint32_t temperature_mp_6        : 8; // Bits [13:6]
    uint32_t temperature_mp_6_parity : 1; // Bits [14]
    uint32_t temperature_mp_6_en     : 1; // Bits [15]
    uint32_t temperature_mp_4        : 8; // Bits [23:16]
    uint32_t temperature_mp_5        : 8; // Bits [31:24]
    // End of ef_ana_trim17

} ef_ana_trim_all_t;

/*@} end of group EF_CTRL_Public_Types */

/** @defgroup  EF_CTRL_Public_Constants
 *  @{
 */

/*@} end of group EF_CTRL_Public_Constants */

/** @defgroup  EF_CTRL_Public_Macros
 *  @{
 */

/*@} end of group EF_CTRL_Public_Macros */

/** @defgroup  EF_CTRL_Public_Functions
 *  @{
 */

/*@} end of group EF_CTRL_Public_Functions */

/*@} end of group EF_CTRL */

/*@} end of group BL616L_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616L_EF_CFG_H__ */
