/**
  ******************************************************************************
  * @file    bl602_rf_private.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Bouffalo Lab</center></h2>
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
#ifndef __BL602_RF_PRIVATE_H__
#define __BL602_RF_PRIVATE_H__

#include <stdint.h>

// #include "rf_reg.h"
// #include "pds_reg.h"
// #include "aon_reg.h"
// #include "hbn_reg.h"
// #include "bl602_common.h"

/** @addtogroup  BL602_Peripheral_Driver
 *  @{
 */

/** @addtogroup  RF_PRIVATE
 *  @{
 */

/** @defgroup  RF_PRIVATE_Public_Types
 *  @{
 */
typedef volatile struct{
    uint32_t index;
    int32_t dvga;
} tx_pwr_index;
extern tx_pwr_index* tp_index;

typedef volatile struct{
    uint32_t notch_en;
    int32_t spur_freq;
} notch_param;
extern notch_param* notch_p;

// typedef volatile struct{
//     uint32_t vbcore;
//     uint32_t iet;
//     uint32_t vbcore_11n;
//     uint32_t iet_11n;
//     uint32_t vbcore_11g;
//     uint32_t iet_11g;
//     uint32_t vbcore_11b;
//     uint32_t iet_11b;  
//     uint32_t lo_fbdv_halfstep_en;
//     uint32_t lo_fbdv_halfstep_en_tx;
//     uint32_t lo_fbdv_halfstep_en_rx;
//     uint32_t clkpll_reset_postdiv;
//     uint32_t clkpll_dither_sel;
// } regs_to_opti;
// extern regs_to_opti* opti_regs;

typedef volatile struct{
    uint16_t Tchannels[5];
    int16_t Tchannel_os[5];
    int16_t Tchannel_os_low[5];
    int16_t Troom_os;
    uint8_t en_tcal;
    uint8_t linear_or_follow;
} tcal_param_struct;
extern tcal_param_struct* tcal_param;

/*@} end of group L1C_Public_Types */

/** @defgroup  L1C_Public_Constants
 *  @{
 */
enum {
    E_RF_BW_NA = 0,
    E_RF_BW_10M = 1,  
    E_RF_BW_20M = 2,
    E_RF_BW_40M = 3
};

enum {
    E_RF_GC_RRF_12DB = 0,
    E_RF_GC_RRF_18DB = 1,
    E_RF_GC_RRF_24DB = 2,
    E_RF_GC_RRF_30DB = 3,
    E_RF_GC_RRF_36DB = 4,
    E_RF_GC_RRF_42DB = 5,
    E_RF_GC_RRF_48DB = 6,
    E_RF_GC_RRF_54DB = 7,
    E_RF_GC_RRF_MIN = E_RF_GC_RRF_12DB,
    E_RF_GC_RRF_MAX = E_RF_GC_RRF_54DB
};

enum {
    E_RF_GC_RBB1_N6DB = 0,
    E_RF_GC_RBB1_0DB = 1,
    E_RF_GC_RBB1_6DB = 2,
    E_RF_GC_RBB1_12DB = 3,
    E_RF_GC_RBB1_MIN = E_RF_GC_RBB1_N6DB,
    E_RF_GC_RBB1_MAX = E_RF_GC_RBB1_12DB
};

enum {
    E_RF_GC_RBB2_N6DB = 0,
    E_RF_GC_RBB2_N4DB,
    E_RF_GC_RBB2_N2DB,
    E_RF_GC_RBB2_0DB,
    E_RF_GC_RBB2_2DB,
    E_RF_GC_RBB2_4DB,
    E_RF_GC_RBB2_6DB,
    E_RF_GC_RBB2_8DB,
    E_RF_GC_RBB2_MIN = E_RF_GC_RBB2_N6DB,
    E_RF_GC_RBB2_MAX = E_RF_GC_RBB2_8DB
};

enum {
    E_RF_DAC_IN_PAD = 0,
    E_RF_DAC_IN_SRAM = 1,
    E_RF_DAC_IN_TXSIN = 2,
    E_RF_DAC_IN_ADCLOOP = 3
};

enum {
    E_RF_TXSIN_PHASE_SAME = 0,
    E_RF_TXSIN_PHASE_ORTH = 1
};

enum {
    E_RF_GC_TBB_0DB = 0,
    E_RF_GC_TBB_6DB = 1,
    E_RF_GC_TBB_12DB = 2,
    E_RF_GC_TBB_18DB = 3,
    E_RF_GC_TBB_24DB = 4
};

enum {
    E_RF_GC_TMX_OFF = 0,
    E_RF_GC_TMX_N12DB = 1,
    E_RF_GC_TMX_N6DB = 2,
    E_RF_GC_TMX_MAXGAIN = 3
};

enum {
    E_RF_PM_ACCLEN_16 = 0,
    E_RF_PM_ACCLEN_32,
    E_RF_PM_ACCLEN_64,
    E_RF_PM_ACCLEN_128,
    E_RF_PM_ACCLEN_256,
    E_RF_PM_ACCLEN_512,
    E_RF_PM_ACCLEN_1024,
    E_RF_PM_ACCLEN_2048,
    E_RF_PM_ACCLEN_4096,
    E_RF_PM_ACCLEN_CUSTOM = 15,
    E_RF_PM_ACCLEN_DEFAULT = E_RF_PM_ACCLEN_1024
};

enum {
    E_RF_MODE_IDLE = 0,
    E_RF_MODE_TX,
    E_RF_MODE_RX,
    E_RF_MODE_ROSCAL,
    E_RF_MODE_RCCAL,
    E_RF_MODE_TXCAL,
    E_RF_MODE_LO_ACAL,
    E_RF_MODE_LO_FCAL
};

enum
{
	E_RF_CHANNEL_2404M = 0,
    E_RF_CHANNEL_2408M,
    E_RF_CHANNEL_2412M,
    E_RF_CHANNEL_2416M,
    E_RF_CHANNEL_2420M,
    E_RF_CHANNEL_2424M,
    E_RF_CHANNEL_2428M,
    E_RF_CHANNEL_2432M,
    E_RF_CHANNEL_2436M,
    E_RF_CHANNEL_2440M,
    E_RF_CHANNEL_2444M,
    E_RF_CHANNEL_2448M,
    E_RF_CHANNEL_2452M,
    E_RF_CHANNEL_2456M,
    E_RF_CHANNEL_2460M,
    E_RF_CHANNEL_2464M,
    E_RF_CHANNEL_2468M,
    E_RF_CHANNEL_2472M,
    E_RF_CHANNEL_2476M,
    E_RF_CHANNEL_2480M,
    E_RF_CHANNEL_2484M,
    E_RF_CHANNEL_NUM
};

enum {
    E_RF_BRANCH_I = 0,
    E_RF_BRANCH_Q = 1,
    E_RF_GAIN = 2,
    E_RF_PHASE = 3
};

enum {
    E_RF_DISABLE = 0,
    E_RF_ENABLE = 1,
    E_RF_OFF = E_RF_DISABLE,
    E_RF_ON  = E_RF_ENABLE
};

enum {
    E_RF_TXCAL_SEQ_IQGAIN  = 1,
    E_RF_TXCAL_SEQ_IQPHASE = 2,
    E_RF_TXCAL_SEQ_LOL     = 0
};

enum {
    E_RF_RXCAL_GAIN_CNT = 4,
    E_RF_TXPWR_TBL_BLE_CNT = 9,
    E_RF_TXCAL_GAIN_CNT = 8,
    E_RF_TXPWR_TBL_CNT = 16,
};

enum {
    E_RF_MODE_11B = 0,
    E_RF_MODE_11G = 1,
    E_RF_MODE_11N = 2,
};

enum {
    E_RF_XTAL_24M = 0,
    E_RF_XTAL_26M = 1,
    E_RF_XTAL_32M = 2,
    E_RF_XTAL_38M4 = 3,
    E_RF_XTAL_40M = 4,
    E_RF_XTAL_52M = 5,
};

enum {
    E_RF_TCAL_UPDATE_PARAM = 0,
};

/*@} end of group L1C_Public_Constants */

/** @defgroup  L1C_Public_Macros
 *  @{
 */

/*@} end of group L1C_Public_Macros */

/** @defgroup  L1C_Public_Functions
 *  @{
 */

#if 0
uint32_t rf_pri_channel_freq_to_index(uint32_t freq);
// void rf_pri_wait_us(uint32_t us);
// void rf_pri_wait_ms(uint32_t ms);
void rf_pri_init(void);
// void rf_pri_config_mode(uint32_t mode);
void rf_pri_config_bandwidth(uint32_t bw);
// void rf_pri_config_channel(uint32_t channel_index);
// void rf_pri_manu_pu(uint32_t mode);
// void rf_pri_auto_gain(void);
// void rf_pri_save_state_for_cal();
// void rf_pri_restore_state_for_cal();
// uint16_t rf_pri_fcal_meas(uint32_t cw);
void rf_pri_fcal(void);
void rf_pri_full_cal(void);
// void rf_pri_set_cal_reg(void);
void rf_pri_lo_acal(void);
void rf_pri_roscal(void);
void rf_pri_rccal(void);
void rf_pri_txcal(void);
#endif

void rf_pri_init(uint8_t reset, uint8_t chipv);                         // set OR, do full calibration
void rf_pri_update_tx_power_offset(uint8_t channel,int8_t power_offset[14]);
void rf_pri_update_power_offset(int32_t power_offset[14]);
void rf_pri_update_param(uint32_t chanfreq_MHz);     // update OR for specified channel
uint32_t rf_pri_get_vco_freq_cw(uint32_t chanfreq_MHz);  // get calibration data
uint32_t rf_pri_get_vco_idac_cw(uint32_t chanfreq_MHz);  // get calibration data
// void rf_pri_get_txgain_settings(int32_t pwr,uint32_t mode,uint32_t* rfg, uint32_t *dg);     //get power index, pwr = power in dBm * 10
void rf_pri_update_txgain_tempos(int16_t tempos);
int32_t rf_pri_get_txgain_max();
int32_t rf_pri_get_txgain_min();
uint32_t rf_pri_get_txgain_index(int32_t pwr,uint32_t mode); 
void rf_pri_query_txgain_table(uint32_t index, uint32_t *rfg_index, uint32_t *dg);
#ifdef RF_IN_SDK
void rf_pri_get_notch_param(uint32_t chanfreq_MHz, uint8_t *ncf_on, int32_t *ncf_freq_Hz, int8_t *ncf_code);
#else
void rf_pri_notch_param(uint32_t chanfreq_MHz);
#endif
void rf_pri_init_fast(uint32_t flag);
void rf_pri_xtalfreq(uint32_t xtalfreq);
void rf_pri_tx_gain_comp(int32_t Tsens);
void rf_pri_update_tcal_param(uint8_t operation);
void rf_pri_update_tx_power_offset_res(int8_t power_offset);
void rf_pri_ble_pwr_dbm(int16_t pwr_dbm);
void rf_pri_txgain_ble(int16_t pwr_dbm);
void rf_pri_manual_incremental_cal(void);
void rf_pri_manual_incremental_cal_stop(void);

/*@} end of group L1C_Public_Functions */

/*@} end of group L1C */

/*@} end of group BL602_Peripheral_Driver */

#endif /* __BL602_L1C_H__ */
