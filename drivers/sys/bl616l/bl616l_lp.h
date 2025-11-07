#ifndef __BL616L_LP_H__
#define __BL616L_LP_H__
#include <stdint.h>

#define IOT2LP_PARA_ADDR          0x20010400
// #define LP_FW_MAX_SIZE            30 * 1024

enum PSM_EVENT {
    PSM_EVENT_SCAN = 0,
    PSM_EVENT_CONNECT,
    PSM_EVENT_DISCONNECT,
    PSM_EVENT_PS,
    PSM_EVENT_AP,
    PSM_EVENT_APP,
    PSM_EVENT_LP_BUF_REUSED,
};

#define TIME_DEBUG_NUM_MAX 10

typedef struct {
    uint64_t time_stamp_us;
    char *info_str;
    uint32_t trig_cnt;
} lp_fw_time_debug_t;

typedef struct {
    uint32_t dtim_num;              /* dtim_num, 0:wakeup, other: dtim_num */
    uint32_t wakeup_flag;           /* wakeup now */
    int32_t win_extend_start_us;    /* extended window time (start) */
    int32_t win_extend_duration_us; /* extended duration time */
} lp_fw_bcn_loss_level_t;

typedef struct {
    /* lpfw timer */
    volatile uint32_t wifi_rx_ready_mtimer;
    volatile uint32_t wifi_rx_start_mtimer;
    volatile uint32_t interval_start_mtimer;
    volatile uint32_t lpfw_sleep_mtimer;
    volatile uint32_t mtimer_timeout;
    volatile int32_t bcn_delay_ave;
} lp_fw_variables_t;

typedef struct {
    /* wifi parse addr */
    volatile uint32_t wifi_buff_start_addr;
    volatile uint32_t wifi_beacon_start_addr;
    volatile uint32_t wifi_beacon_bssid_addr;
    volatile uint32_t wifi_beacon_fixed_part_addr;
    volatile uint32_t wifi_beacon_interval_part_addr;
    volatile uint32_t wifi_beacon_variable_part_addr;
} lp_fw_constants_t;

typedef struct {
    lp_fw_variables_t lpfw_variables;
    lp_fw_constants_t lpfw_constants;
} lp_fw_static_variable_t;

/* iot2lp_para record info (internal) */
struct bl_lp_info_s {
    /* bcn info */
    uint32_t bcn_lpfw_recv_cnt;
    uint32_t bcn_lpfw_loss_cnt;
    uint32_t bcn_app_recv_cnt;
    uint32_t bcn_app_loss_cnt;
    /* time info */
    uint64_t time_record_start_rtc_cnt;
    uint64_t time_total_rtc_cnt;
    uint64_t time_sleep_pds_rtc_cnt;
    uint64_t time_active_lpfw_rtc_cnt;
    uint64_t time_active_app_rtc_cnt;
};

typedef struct {
    uint8_t encrypt_type;
    uint8_t xts_mode;
    uint8_t r0_aes_en;
    uint8_t r0_aes_hw_key_en;
    uint8_t r0_aes_lock;
    uint32_t r0_aes_iv[4];
    uint32_t r0_aes_start;
    uint32_t r0_aes_end;
    uint8_t r1_aes_en;
    uint8_t r1_aes_hw_key_en;
    uint8_t r1_aes_lock;
    uint32_t r1_aes_iv[4];
    uint32_t r1_aes_start;
    uint32_t r1_aes_end;
    uint8_t r2_aes_en;
    uint8_t r2_aes_hw_key_en;
    uint8_t r2_aes_lock;
    uint32_t r2_aes_iv[4];
    uint32_t r2_aes_start;
    uint32_t r2_aes_end;
} lp_fw_sf_sec_t;

typedef struct {
    uint32_t tzc_ocram_tzsrg_ctrl;
    uint32_t tzc_ocram_tzsrg_r0;
    uint32_t tzc_ocram_tzsrg_r1;
    uint32_t tzc_ocram_tzsrg_r2;
    uint32_t tzc_sf_tzsrg_ctrl;
    uint32_t tzc_sf_tzsrg_r0;
    uint32_t tzc_sf_tzsrg_r1;
    uint32_t tzc_sf_tzsrg_r2;
    uint32_t tzc_sf_tzsrg_msb;
} lp_fw_tzc_t;

typedef struct {
    uint32_t pattern; /*0xAA5555AA*/
    lp_fw_static_variable_t* lpfw_static_val;
    /* flash recovery */
    uint32_t mcu_sts;
    void *flash_cfg;
    void *flash_io_cs_clk_delay_cfg;
    uint32_t flash_jdec_id;
    lp_fw_sf_sec_t *sec_cfg;
    lp_fw_tzc_t *tzc_cfg;
    uint32_t img_len;
    uint8_t flash_clk;
    uint8_t flash_clk_div;
    uint8_t do_xip_recovery;
    uint8_t unkown_io_wakeup_en;

    /* wifi para */
    uint8_t ap_channel;
    uint8_t tim_wakeup_en;
    uint16_t aid;
    uint8_t bssid[6];
    uint8_t local_mac[6];
    uint32_t beacon_interval_tu; /* beacon interval tu */
    uint8_t* wifi_rx_buff; /* not used */
    /* system para */
    uint32_t wakeup_flag;
    uint32_t flash_offset;
    uint32_t app_entry;
    uint32_t args[4];
    uint32_t cpu_regs[32];
    uint32_t tim_wake_enable;
    uint32_t rtc_wakeup_en;  /* rtc wakeup enable */
    uint64_t rtc_wakeup_cnt; /* The value of the next rtc wake up */
    int32_t wakeup_reason;   /* reason of wake up */
    int32_t lpfw_wakeup_cnt;
    uint64_t wake_io_bits;        /* wake io bits */
    uint64_t wake_io_edge_bits;   /* wake edge bits */
    uint8_t wake_acomp_bits;      /* wake acomp bits */
    uint8_t wake_acomp_edge_bits; /* wake acomp bits */
    uint8_t transport_mode_enable;
    uint8_t feed_wdt_gpio;
    /* beacon  */
    int32_t tpre;

    int32_t last_sleep_error_us;
    uint32_t last_beacon_stamp_rtc_valid;
    uint64_t last_beacon_stamp_rtc_us;    /*  */
    uint64_t last_beacon_stamp_beacon_us;

    uint32_t mtimer_timeout_mini_us;
    uint32_t mtimer_timeout_max_us;
    uint8_t mtimer_timeout_en;

    uint8_t dtim_num;
    uint8_t beacon_leg_rate;
    uint8_t debug_io; /* io toggle after wifi init*/

    int32_t rtc32k_jitter_error_ppm; /* Jitter error compensation, ppm (10^6) */

    /* Statistical analysis */
    int32_t *bcn_delay_sliding_win_buff;
    uint8_t bcn_delay_sliding_win_size;
    uint8_t bcn_delay_sliding_win_point;
    uint8_t bcn_delay_sliding_win_status;
    int32_t last_beacon_delay_us; /* beacon delay */
    int32_t bcn_delay_offset;

    uint32_t continuous_loss_cnt;
    uint32_t continuous_loss_cnt_max;
    /* beacon loss ctrl */
    lp_fw_bcn_loss_level_t *bcn_loss_cfg_table;
    int32_t bcn_loss_level;
    int32_t bcn_loss_loop_start;
    int32_t bcn_loss_level_max;
    int8_t  bcn_target_level;

    /* rtc32k_trim */
    uint8_t rc32k_auto_cal_en;
    uint8_t rc32k_clock_ready;
    uint8_t rc32k_trim_ready;
    uint8_t last_rc32trim_stamp_valid;
    uint64_t last_rc32trim_stamp_rtc_us;
    uint64_t last_rc32trim_stamp_beacon_us;
    int32_t rc32k_fr_ext;
    int32_t rtc32k_error_ppm; /*  */

    /* iot2lp_para record info (internal) */
    struct bl_lp_info_s *lp_info;

    /* RTC-UTC Timestamp */
    uint32_t last_ntp_sync_timestamp;
    uint64_t last_ntp_sync_rtc;

    /* point of shared functions */
    uint32_t *shared_func_p;

    lp_fw_time_debug_t *time_debug;

    /* The parameter that remains after the reset, it has to be at the back */
    struct {
        /* reset reason */
        uint32_t reset_reason;
        uint32_t reset_reason_chk;
    } reset_keep;

} iot2lp_para_t;

#define iot2lp_para               ((iot2lp_para_t *)IOT2LP_PARA_ADDR)

#define HBN_SYS_RESET_REASON      (iot2lp_para->reset_keep.reset_reason)
#define HBN_SYS_RESET_REASON_CHK  (iot2lp_para->reset_keep.reset_reason_chk)

/* Cause of wakeup */
#define LPFW_WAKEUP_UNKOWN    0
#define LPFW_WAKEUP_TIME_OUT  (1 << 0)
#define LPFW_WAKEUP_WIFI      (1 << 1)
#define LPFW_WAKEUP_AP_LOSS   (1 << 2)
#define LPFW_WAKEUP_IO        (1 << 3)
#define LPFW_WAKEUP_ACOMP     (1 << 4)
#define LPFW_WAKEUP_BLE       (1 << 5)
#define LPFW_WAKEUP_LOSS_CFG_OVER   (1 << 6)

int bl_lp_get_wake_reason(void);
void bl_lp_set_resume_wifi(void);
void bl_lp_clear_resume_wifi(void);
int bl_lp_get_resume_wifi(void);

#endif
