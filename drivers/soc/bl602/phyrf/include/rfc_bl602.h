#ifndef _RFC_BL602_H_
#define _RFC_BL602_H_

#include <stdint.h>
#include <stdbool.h>

enum
{
    RFC_OFF = 0,
    RFC_ON  = 1
};

enum 
{
    RFC_PM_MUX_TO_ADC = 0,
    RFC_PM_MUX_TO_IQCOMP = 1,
    RFC_PM_MUX_TO_BLE = 2
};

enum 
{
    RFC_TXDFE_IN_MUX_TO_BB = 0,
    RFC_TXDFE_IN_MUX_TO_SRAM = 1,
    RFC_TXDFE_IN_MUX_TO_SINGEN = 2,
    RFC_TXDFE_IN_MUX_TO_PAD = 3
};

enum
{
    RFC_RC_IDLE = 0,
    RFC_RC_RX2ON = 1,
    RFC_RC_TX2ON = 2,
    RFC_RC_RXON = 3,
    RFC_RC_TX2PAON = 4,
    RFC_RC_TXPAON = 7,
    RFC_RC_FORCE_OFF = 15
};


enum 
{
    RFC_FSM_PD = 0,
    RFC_FSM_SB = 1,
    RFC_FSM_LO = 2,
    RFC_FSM_RX = 3,
    RFC_FSM_TX = 4,
    RFC_FSM_FORCE_OFF = 15
};

enum 
{
    RFC_BBMODE_WLAN = 0,
    RFC_BBMODE_BLE = 1
};

enum 
{
	RFC_SG_SINGLE_TONE = 0,
	RFC_SG_TWO_TONE,
	RFC_SG_RAMP
};

enum {
    RFC_PC_AUTO = 0,
    RFC_PC_WLAN_11B,
    RFC_PC_WLAN_11G,
    RFC_PC_WLAN_11N,
    RFC_PC_BT_BLE
};

enum {
    RFC_FORMATMOD_11B = 0,
    RFC_FORMATMOD_11G = 1,
    RFC_FORMATMOD_11N = 2
};

enum {
    RFC_BW_20M = 0,
    RFC_BW_10M = 1
};

#define RFC_FORCE_OFF       (15)

struct rfc_status_tag {
    /** test read 1 */
    uint32_t pkdet_out_raw            : 1;  //[0]
    uint32_t dig_xtal_clk_dbg         : 1;  //[1]
    uint32_t clk_ble_16m_dbg          : 1;  //[2]
    uint32_t clk_rc_dbg0              : 1;  //[3]
    uint32_t clk_adcpow_dbg           : 1;  //[4]
    uint32_t clk_fetx_dbg             : 1;  //[5]
    uint32_t clk_ferx_dbg             : 1;  //[6]
    uint32_t clkpll_postdiv_outclk_dbg: 1;  //[7]
    uint32_t clk_soc_480m_dbg         : 1;  //[8]
    uint32_t clk_soc_240m_dbg         : 1;  //[9]
    uint32_t clk_soc_192m_dbg         : 1;  //[10]
    uint32_t clk_soc_160m_dbg         : 1;  //[11]
    uint32_t clk_soc_120m_dbg         : 1;  //[12]
    uint32_t clk_soc_96m_dbg          : 1;  //[13]
    uint32_t clk_soc_80m_dbg          : 1;  //[14]
    uint32_t clk_soc_48m_dbg          : 1;  //[15]
    uint32_t clk_soc_32m_dbg          : 1;  //[16]
    uint32_t pad_pkdet_out            : 1;  //[17]
    uint32_t pad_agc_ctrl             :10;  //[27:18]
    uint32_t rf_pkdet_rst_hw          : 1;  //[28]
    uint32_t rf_cbw_wifi              : 2;  //[30:29]
    uint32_t lo_unlocked              : 1;  //[31]

    /** test read 2 */
    uint32_t fsm_pu_txbuf             : 1;  //[0]
    uint32_t fsm_pu_rxbuf             : 1;  //[1]
    uint32_t fsm_pu_tosdac            : 1;  //[2]
    uint32_t fsm_pu_dac               : 1;  //[3]
    uint32_t fsm_trsw_en              : 1;  //[4]
    uint32_t fsm_pu_adc               : 1;  //[5]
    uint32_t fsm_pu_pkdet             : 1;  //[6]
    uint32_t fsm_pu_rbb               : 1;  //[7]
    uint32_t fsm_pu_rmx               : 1;  //[8]
    uint32_t fsm_pu_rmxgm             : 1;  //[9]
    uint32_t fsm_pu_lna               : 1;  //[10]
    uint32_t clk_rc_dbg2              : 1;  //[11]
    uint32_t rf_lna_ind_hw            : 4;  //[15:12]
    uint32_t rf_rbb_ind_hw            : 4;  //[19:16]
    uint32_t rf_tx_pow_lvl_hw         : 4;  //[23:20]
    uint32_t rf_rc_lo_rdy             : 1;  //[24]
    uint32_t rf_fsm_state             : 3;  //[27:25]
    uint32_t rf_rc_state              : 3;  //[30:28]
    uint32_t clk_rc_dbg               : 1;  //[31]
};

void rfc_ver_set(uint8_t ver);
void rfc_init(uint32_t xtalfreq);
void rfc_reset();
void rfc_config_channel(uint32_t channel_freq);
void rfc_config_channel_sw(uint32_t channel_freq);
void rfc_write_reg(uint32_t a, uint32_t d);
uint32_t rfc_read_reg(uint32_t a);

void rfc_config_power(uint32_t mode,uint32_t tbb_boost,uint32_t tbb,uint32_t tmx);
bool rfc_config_power_ble(int32_t pwr_dbm);
void rfc_config_bandwidth(uint32_t mode);

uint32_t rfc_get_power_level(uint32_t mode, int32_t power);
void rfc_apply_tx_power_offset(uint8_t channel,int8_t power_offset[14]);

/*
 * TXDFE DECLARATIONS
 ****************************************************************************************
 */
void rfc_txdfe_start();
void rfc_txdfe_stop();
void rfc_txdfe_mux(int8_t signal_source);
void rfc_txdfe_set_dvga(int8_t dvga_qdb);
void rfc_txdfe_set_iqgaincomp(uint8_t en,uint16_t coeff);
void rfc_txdfe_set_iqphasecomp(uint8_t en,int16_t coeff);
void rfc_txdfe_set_dccomp(int16_t dcc_i,int16_t dcc_q);
void rfc_txdfe_set_iqswap(uint8_t swap_on);

/*
 * RXDFE DECLARATIONS
 ****************************************************************************************
 */
void rfc_rxdfe_start();
void rfc_rxdfe_stop();
void rfc_rxdfe_set_iqgaincomp(uint8_t en,uint16_t coeff);
void rfc_rxdfe_set_iqphasecomp(uint8_t en,int16_t coeff);
void rfc_rxdfe_set_dccomp(int16_t dcc_i,int16_t dcc_q);
void rfc_rxdfe_set_iqswap(uint8_t swap_on);
void rfc_rxdfe_set_notch0(uint8_t en,uint8_t alpha,int8_t nrmfc);
void rfc_rxdfe_set_notch1(uint8_t en,uint8_t alpha,int8_t nrmfc);

/*
 * SG DECLARATIONS
 ****************************************************************************************
 */
void rfc_sg_start(uint32_t inc_step,uint32_t gain_i,uint32_t gain_q,uint32_t addr_i,uint32_t addr_q);
void rfc_sg_stop();

/*
 * PM DECLARATIONS
 ****************************************************************************************
 */

uint32_t rfc_pm_start(uint32_t insel,int32_t freq_cw,uint32_t acclen,uint32_t rshiftlen,
                     int32_t *raw_acc_i,int32_t *raw_acc_q);
void rfc_pm_stop();

/*
 * HWCTRL DECLARATIONS
 ****************************************************************************************
 */
void rfc_hwctrl_txrfgain(uint8_t hwctrl_on);
void rfc_hwctrl_rxgain(uint8_t hwctrl_on);
void rfc_hwctrl_txdvga(uint8_t hwctrl_on);
void rfc_hwctrl_calparam(uint8_t hwctrl_on);

/*
 * FSM DECLARATIONS
 ****************************************************************************************
 */
void rfc_lo_set_regs(uint32_t xtal_freq);
void rfc_lo_set_fcal_lut(uint32_t* ptr_fcal_2404,uint32_t* ptr_fcal_2484);
void rfc_lo_set_channel(uint32_t chanfreq_MHz);

void rfc_fsm_force(uint8_t state);
void rfc_rc_fsm_force(uint8_t state);

/*
 * COEX DECLARATIONS
 ****************************************************************************************
 */
void rfc_coex_force_to(uint32_t force_enable, uint32_t bbmode);

void rfc_wlan_mode_force(uint32_t force_mode);

void rfc_dump();

void rfc_apply_tx_dvga_offset(int8_t offset);
void rfc_apply_tx_dvga(int8_t dvga_qdb[16]);

void rx_manual_incremental_cal(void);
void rx_manual_incremental_cal_stop(void);

#endif
