#ifndef __WL_API_H__
#define __WL_API_H__

#include <stdint.h>

#ifndef WL_API_RMEM_EN
#define WL_API_RMEM_EN (1)
#else
#if !WL_NIC
#undef WL_API_RMEM_EN
#define WL_API_RMEM_EN (1)
#endif
#endif


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

enum
{
    WL_ENV_LP_ACQ  = 0,
    WL_ENV_LP_TRK = 1,
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

// Power vs Rate table in 0.25dBm
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

// Maximum power limit table in 1dB for different regulations (SRRC/FCC/CE...)
struct wl_param_pwrlimit_t
{
    uint8_t en;    // enable power limit
    int8_t b_dsss; // power limit for 11b 1/2Mbps
    int8_t b_cck;  // power limit for 11b 5.5/11Mbps
    int8_t g;      // power limit for 11g
    int8_t n20;    // power limit for 11n 20M BW
    int8_t ax20;   // power limit for 11ax 20M BW
    int8_t n40;    // power limit for 11n 40M BW
    int8_t ax40;   // power limit for 11ax 40M BW
};

struct wl_efuse_t
{
    uint8_t     device_info;
    uint8_t     chip_version;
    uint8_t     iptat_code;
    uint8_t     icx_code;
    uint8_t     dcdc_vout_trim_aon;
};

struct wl_param_tcap_t
{
    uint8_t     en_tcap;
    int8_t      tcap_tsen[10];
    int8_t      tcap_cap[11];
};

struct wl_param_t
{
    uint32_t                     xtalfreq_hz;     // multi source driven (efuse/flash/dts...)
    uint8_t                      xtalcapcode_in;  // multi source driven (efuse/flash/dts...)
    uint8_t                      xtalcapcode_out; // multi source driven (efuse/flash/dts...)
    uint16_t                     country_code;    // multi source driven (efuse/flash/dts...)
    struct wl_param_tcal_t       tcal;            // multi source driven (efuse/flash/dts...)
    struct wl_param_pwrtarget_t  pwrtarget;       // multi source driven (efuse/flash/dts...)
    struct wl_param_pwrcal_t     pwrcal;          // multi source driven (efuse/flash/dts...)
    struct wl_param_pwrlimit_t   pwrlim[NUM_WLAN_CHANNELS];
    struct wl_efuse_t            ef;
    #if WL_NIC
    // TODO: Review
    uint8_t                      pwr_update;      // power update flag
    #endif
    struct wl_param_tcap_t       tcap;
};

struct wl_env_t
{
    int8_t   lp_bcn_rssi;
    uint16_t lp_bcn_lost_cnt;
    uint32_t lp_bcn_time_us;
    uint8_t  lp_gain_mode;
    uint8_t  lp_status;
};

typedef enum
{
    WL_DEVICE_QFN40 = 0,
    WL_DEVICE_QFN40M,
    WL_DEVICE_QFN56
} wl_device_info_t;

typedef enum
{
    WL_LOG_LEVEL_NONE = 0, // No logging
    WL_LOG_LEVEL_CRITICAL, // Critical messages such as cli cmd response
    WL_LOG_LEVEL_TRACE,    // Trace activities
    WL_LOG_LEVEL_ERROR,    // Error conditions
    WL_LOG_LEVEL_WARN,     // Warning conditions
    WL_LOG_LEVEL_INFO,     // Informational messages
    WL_LOG_LEVEL_DEBUG,    // Debug messages
} wl_log_level_t;

/**
 * @struct wl_cfg_t
 * @brief WL CFG structure containing function pointers, etc., that are implemented by the main program and used by the wl library.
 *
 * @var log_printf
 *      Type: void (*)(const char *format, ...)
 *      Description: A function pointer that behaves similarly to the standard printf function, accepting a format string and a corresponding variable argument list.
 *                   This function is used for internal logging within the wl library. The main program implementing this function must ensure it is thread-safe and 
 *                   can handle printf-like format strings and arguments correctly.
 *                   Example: On certain IoT platforms, a tested and working approach was using 'vprint' function. It's essential that the function provided 
 *                   handles variable argument lists appropriately as in standard 'printf' function.
 *      Requirement: The main program must provide an implementation of this function and assign this function pointer to it during the initialization of the wl_api_t structure.
 *
 * @var log_level
 *      Type: uint8_t
 *      Description: Defines the logging level. The wl library will use this level to filter log messages, outputting only those that are at or above this level.
 *                   It helps in controlling the verbosity of log messages, ensuring that only relevant messages for the current debug or operational level are displayed.
 *      Requirement: Set by the main program during initialization. The value should be one of the predefined logging level constants (e.g., WL_LOG_LEVEL_INFO, WL_LOG_LEVEL_WARN, etc.).
 *                   This level will dictate the minimum threshold for log messages. Messages below this severity level will be ignored.
 */

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
    /* platform logging api */
    void (*log_printf)(const char *format, ...);

    uint8_t     log_level;
    uint8_t     device_info; // QFN40,QFN40M,QFN56
};

/**
 * Example of initializing the wireless driver:
 * 
 * This snippet demonstrates the setup required before initializing the wireless driver.
 * It involves populating the wl_cfg_t structure with appropriate function handlers and parameters.
 * Each function handler corresponds to a platform-specific implementation necessary for the wireless driver's operations.
 *
 * Retrieve the configuration structure, providing memory allocated for wireless configurations.
 * struct wl_cfg_t* cfg = wl_cfg_get(RETENSION_MEM_ALLOCATED_FOR_WL);
 *
 * Assign a function to set the capcode based on platform-specific requirements.
 * cfg->capcode_set = function_handler_to_set_capcode;
 *
 * Assign a function to retrieve the current capcode, specific to the platform's way of capcode management.
 * cfg->capcode_get = function_handler_to_get_capcode;
 *
 * Assign a platform-dependent function to load necessary parameters required for wireless operations.
 * cfg->param_load = platform_dependent_param_load_function_handler;
 *
 * Enable the loading of parameters, indicating that the driver should use the param_load function.
 * cfg->en_param_load = 1; 
 *
 * Enable full calibration mode for more accurate wireless operations.
 * cfg->en_full_cal = 1;
 *
 * Set the operating mode of the wireless driver to WLAN.
 * cfg->mode = WL_API_MODE_WLAN;
 * 
 * Additional configuration for logging and device information:
 * Set the function for logging within the wireless driver. 
 * Here, 'vprint' is a platform-specific implementation of a printf-like function.
 * cfg->log_printf = vprint;
 *
 * Define the logging level, ensuring that only messages of this level or higher are printed.
 * cfg->log_level = WL_LOG_LEVEL_CRITICAL;
 *
 * Set device-specific information.
 * cfg->device_info = 0;
 * 
 * Initialize the wireless driver, checking for successful initialization.
 * int status = wl_init();
 */

#if WL_API_RMEM_EN
/* Get retention memory size in bytes for wl driver */
uint32_t wl_rmem_size_get();
/* Get wireless driver cfg structure:retention sram to save calibration data */
struct wl_cfg_t* wl_cfg_get(uint8_t* rmem);
struct wl_env_t* wl_env_get(uint8_t* rmem);
#else
struct wl_cfg_t* wl_cfg_get();
#endif

/* 
 * Wireless driver init with param loading and full calibration which can be
 * skipped by configure en_param_load, en_full_cal in cfg structure
 */
int8_t wl_init();

/* BB API */
void wl_wlan_bb_reset();
void wl_wlan_bb_pre_proc(void* rvec_ptr);
#if WL_NIC
void wl_wlan_bb_post_proc(void* rvec_ptr, uint16_t type_subtype, int8_t gainopt);
#else
void wl_wlan_bb_post_proc(void* rvec_ptr, uint16_t type_subtype);
#endif
/* Get power cfg for rate indicated by formatmod and mcs.
 * The return value will be filled in transmit descriptor */
uint8_t wl_wlan_power_cfg_get(uint8_t formatmod, uint8_t rate);
void wl_wlan_power_table_update();
/* A helper function to read rssi from rvec */
int8_t wl_wlan_rssi_get(void* rvec_ptr);
/* A helper function to read ppm from rvec */
int8_t wl_wlan_ppm_get(void* rvec_ptr);
/* Get power cfg for bluetooth/802154 phymode which filled in transmit descriptor */
int8_t wl_154_power_cfg_get(uint8_t phyrate);
int8_t wl_bt_power_cfg_get(uint8_t phyrate);
int8_t wl_ble_power_cfg_get(uint8_t phyrate);

/* Temperature calibration should be scheduled periodically */
int8_t wl_rf_tcal_handler(int16_t temperature);
/* Get tcal period in seconds */
uint32_t wl_rf_tcal_period_get();

extern struct wl_cfg_t* wl_cfg;
extern uint8_t*         wl_cal;

/*
* rf driver api start
*/

void wl_154_optimize(uint16_t channel_freq); // only for 802154 optimize
void wl_154_optimize_restore(); // Restore default setting when exit 802154

void wl_bz_rx_optimize(uint16_t channel_freq); // only for BTBLE Rx optimize
void wl_bz_rx_optimize_restore(); // Restore default setting when exit BTBLE

void wl_rf_set_bz_target_power_table(int8_t target_pwr_dbm);// modified the bz power table according to the target power of bz before set bz tx
void wl_rf_set_154_tx_power(uint32_t target_pwr_dbm);
void wl_rf_cfg_init(void);//set default values to rf members of struct, //by Lx
void wl_rf_set_channel_pwr_comp(uint8_t channel_idx);
void wl_rf_set_bz_channel_pwr_comp();
/*
* rf driver api end
*/


/*
 * LP API DECLARATIONS
 ****************************************************************************************
 */
#if WL_API_RMEM_EN
int8_t wl_lp_init(uint8_t* rmem, uint16_t channelfreq_MHz);
#else
int8_t wl_lp_init(uint16_t channelfreq_MHz);
#endif
void wl_lp_status_update(int8_t bcn_rx_status, int8_t bcn_rssi, uint32_t bcn_hbn_time_us);

#endif
