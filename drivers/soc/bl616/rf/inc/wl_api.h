#ifndef __WL_API_H__
#define __WL_API_H__

#include <stdint.h>
#include "wl_config.h"

#define NUM_WLAN_CHANNELS           (14)
#define NUM_BZ_CH_PWRCOMP           (5)

enum
{
    WL_API_MODE_WLAN = 1,
    WL_API_MODE_BZ = 2,
    WL_API_MODE_ALL = 3
};

enum
{
    WL_API_STATUS_OK = 0,
    WL_API_STATUS_INITED = 0xacde,
};

enum
{
    WL_WLAN_MODE_NON_HT = 0,
    WL_WLAN_MODE_NON_HT_DUP_OFDM,
    WL_WLAN_MODE_HT_MF,
    WL_WLAN_MODE_HT_GF,
    WL_WLAN_MODE_VHT,
    WL_WLAN_MODE_HE_SU,
    WL_WLAN_MODE_HE_MU,
    WL_WLAN_MODE_HE_ER,
    WL_WLAN_MODE_MAX
};

enum
{
    WL_BZ_MODE_802154 = 0,
    WL_BZ_MODE_BLE,
    WL_BZ_MODE_BT_BR,
    WL_BZ_MODE_BT_EDR,
    WL_BZ_MODE_MAX
};

enum wl_xtalfreq_enum
{
    WL_XTAL_24M = 0,
    WL_XTAL_26M,
    WL_XTAL_32M,
    WL_XTAL_38M4,
    WL_XTAL_40M,
    WL_XTAL_52M,
    WL_NUM_XTAL
};

struct wl_param_tcal_t
{
    uint8_t     en_tcal;
    uint8_t     linear_or_follow;
    uint16_t    Tchannels[5];
    int16_t     Tchannel_os[5];
    int16_t     Tchannel_os_low[5];
    int16_t     Troom_os;
};

struct wl_param_pwrcal_t
{
    int8_t      channel_pwrcomp_wlan[NUM_WLAN_CHANNELS];
    int8_t      channel_lp_pwrcomp_wlan[NUM_WLAN_CHANNELS]; // power compensation at low power mode
    int8_t      Temperature_MP; // temperature of sensor while power cal at production line
    int8_t      channel_pwrcomp_bz[NUM_BZ_CH_PWRCOMP];
};

struct wl_param_pwrtarget_t
{
    int8_t     pwr_11b[4];
    int8_t     pwr_11g[8];
    int8_t     pwr_11n_ht20[8];
    int8_t     pwr_11n_ht40[8];
    int8_t     pwr_11ac_vht20[10];
    int8_t     pwr_11ac_vht40[10];
    int8_t     pwr_11ac_vht80[10];
    int8_t     pwr_11ax_he20[12];
    int8_t     pwr_11ax_he40[12];
    int8_t     pwr_11ax_he80[12];
    int8_t     pwr_11ax_he160[12];
    int8_t     pwr_ble;
    int8_t     pwr_bt[3];
    int8_t     pwr_zigbee;
};

struct wl_efuse_t
{
    uint8_t     device_info;
    uint8_t     chip_version;
    uint8_t     iptat_code;
    uint8_t     icx_code;
    uint8_t     dcdc_vout_trim_aon;
};

struct wl_param_t
{
    uint32_t                     xtalfreq_hz;     // multi source driven (efuse/flash/dts...)
    // enum wl_xtalfreq_enum     xtalfreq_enum;
    uint8_t                      xtalcapcode_in;  // multi source driven (efuse/flash/dts...)
    uint8_t                      xtalcapcode_out; // multi source driven (efuse/flash/dts...)
    uint16_t                     country_code;    // multi source driven (efuse/flash/dts...)
    struct wl_param_tcal_t       tcal;            // multi source driven (efuse/flash/dts...)
    struct wl_param_pwrtarget_t  pwrtarget;       // multi source driven (efuse/flash/dts...)
    struct wl_param_pwrcal_t     pwrcal;          // multi source driven (efuse/flash/dts...)
    struct wl_efuse_t            ef;
};

struct wl_cfg_t
{
    uint32_t    status;
    uint8_t     mode;            // 0b01: wlan; 0b10: bz; 0b11: dual mode
    uint8_t     en_param_load;   // 0: param is in rentention, no read required; 1: read param during init
    uint8_t     en_full_cal;     // 0: cal is ready, no full calibration required; 1: do full cal during init
    
    struct wl_param_t param;

    /* platform api to load param data which will be called during init */
    int8_t (*param_load)(struct wl_param_t* par);
    /* platform api to set capcode register */
    void (*capcode_set)(uint8_t capcode_in, uint8_t capcode_out);
    /* platform api to get capcode register */
    void (*capcode_get)(uint8_t* capcode_in, uint8_t* capcode_out);
};

/* Wireless driver init example
 * struct wl_cfg_t* cfg = wl_cfg_get(RETENSION_MEM_ALLOCATED_FOR_WL);
 * cfg->capcode_set = function_handler_to_set_capcode;
 * cfg->capcode_get = function_handler_to_get_capcode;
 * cfg->param_load = platform_dependent_param_load_function_handler;
 * cfg->en_param_load = 1; 
 * cfg->en_full_cal = 1;
 * cfg->mode = WL_API_MODE_WLAN;
 * 
 * int status = wl_init();
 */
#if WL_API_RMEM_EN
/* Get retention memory size in bytes for wl driver */
uint32_t wl_rmem_size_get();
/* Get wireless driver cfg structure:retention sram to save calibration data */
struct wl_cfg_t* wl_cfg_get(uint8_t* rmem);
int8_t wl_lp_init(uint8_t* rmem, uint16_t channelfreq_MHz);
#else
struct wl_cfg_t* wl_cfg_get();
int8_t wl_lp_init(uint16_t channelfreq_MHz);
#endif

/* 
 * Wireless driver init with param loading and full calibration which can be
 * skipped by configure en_param_load, en_full_cal in cfg structure
 */
int8_t wl_init();

/* BB API */
void wl_wlan_bb_reset();
void wl_wlan_bb_pre_proc(void* rvec_ptr);
void wl_wlan_bb_post_proc(void* rvec_ptr, uint16_t type_subtype);
/* Get power cfg for rate indicated by formatmod and mcs.
 * The return value will be filled in transmit descriptor */
uint8_t wl_wlan_power_cfg_get(uint8_t formatmod, uint8_t rate);
void wl_wlan_power_table_update();
/* A helper function to read rssi from rvec */
int8_t wl_wlan_rssi_get(void* rvec_ptr);
/* A helper function to read ppm from rvec */
float wl_wlan_ppm_get(void* rvec_ptr);
/* Get power cfg for bluetooth/802154 phymode which filled in transmit descriptor */
uint8_t wl_bz_power_cfg_get(uint8_t phymode, uint8_t phyrate);
/* Temperature calibration should be scheduled periodically */
int8_t wl_rf_tcal_handler(int16_t temperature);
/* Get tcal period in seconds */
uint32_t wl_rf_tcal_period_get();

extern struct wl_cfg_t* wl_cfg;
extern uint8_t*         wl_cal;

/*
* rf driver api start
*/
void wl_bz_rx_optimize(uint16_t channel_freq);
void wl_bz_rx_optimize_restore();
void wl_rf_set_bz_target_power_table(int8_t target_pwr_dbm);// modified the bz power table according to the target power of bz before set bz tx
void wl_rf_cfg_init(void);//set default values to rf members of struct, //by Lx
void wl_rf_set_channel_pwr_comp(uint8_t channel_idx);
/*
* rf driver api end
*/

#endif
