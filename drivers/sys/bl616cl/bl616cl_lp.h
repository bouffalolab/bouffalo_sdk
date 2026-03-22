#ifndef __BL616CL_LP_H__
#define __BL616CL_LP_H__
#include <stdint.h>
#include "bl616cl_xip_recovery.h"

typedef int (*bl_lp_cb_t)(void *arg);

#define BL_LP_SOFT_INT_TRIG  (csi_vic_set_pending_irq(MSOFT_IRQn))
#define BL_LP_SOFT_INT_CLEAR (csi_vic_clear_pending_irq(MSOFT_IRQn))

#define IOT2LP_PARA_ADDR     (0x20010000 + 0x0400)
// #define LP_FW_MAX_SIZE            30 * 1024

#define PROTECT_AF_MS        (10)
#define PROTECT_BF_MS        (5)

enum PSM_EVENT {
    PSM_EVENT_SCAN = 0,
    PSM_EVENT_CONNECT,
    PSM_EVENT_DISCONNECT,
    PSM_EVENT_PS,
    PSM_EVENT_AP,
    PSM_EVENT_APP,
    PSM_EVENT_LP_BUF_REUSED,
};

/******************** io wakeup cfg ********************/

#define BL_LP_WAKEUP_IO_MAX_NUM             52

#define BL_LP_IO_WAKEUP_MODE_LOW            1
#define BL_LP_IO_WAKEUP_MODE_HIGH           2
#define BL_LP_IO_WAKEUP_MODE_FALLING        3
#define BL_LP_IO_WAKEUP_MODE_RISING         4
#define BL_LP_IO_WAKEUP_MODE_RISING_FALLING 5

/** @defgroup BL_LP_AON_IO_TRIG aon io wakeup trigger mode
  * @{
  */
#define BL_LP_AON_IO_TRIG_SYNC_FALLING_EDGE (0x0) /*!< AON_IO INT trigger type: sync falling edge trigger */
#define BL_LP_AON_IO_TRIG_SYNC_RISING_EDGE  (0x1) /*!< AON_IO INT trigger type: sync rising edge trigger */
#define BL_LP_AON_IO_TRIG_SYNC_LOW_LEVEL    (0x2) /*!< AON_IO INT trigger type: sync low level trigger */
#define BL_LP_AON_IO_TRIG_SYNC_HIGH_LEVEL   (0x3) /*!< AON_IO INT trigger type: sync high level trigger */
#define BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE \
    (0x4)                                          /*!< AON_IO INT trigger type: sync rising falling edge trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_FALLING_EDGE (0x8) /*!< AON_IO INT trigger type: async falling edge trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_RISING_EDGE  (0x9) /*!< AON_IO INT trigger type: async rising edge trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_LOW_LEVEL    (0xA) /*!< AON_IO INT trigger type: async low level trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_HIGH_LEVEL   (0xB) /*!< AON_IO INT trigger type: async high level trigger */
#define BL_LP_AON_IO_TRIG_NONE               (0xF) /*!< AON_IO INT trigger type: none */
/**
  * @}
  */

/** @defgroup BL_LP_PDS_IO_TRIG aon io wakeup trigger mode
  * @{
  */
#define BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE  (0x0) /*!< PDS_IO INT trigger type: sync falling edge trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_RISING_EDGE   (0x1) /*!< PDS_IO INT trigger type: sync rising edge trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_LOW_LEVEL     (0x2) /*!< PDS_IO INT trigger type: sync low level trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL    (0x3) /*!< PDS_IO INT trigger type: sync high level trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_RISING_FALLING_EDGE \
    (0x4)                                          /*!< PDS_IO INT trigger type: sync rising falling edge trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_FALLING_EDGE (0x8) /*!< PDS_IO INT trigger type: async falling edge trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_RISING_EDGE  (0x9) /*!< PDS_IO INT trigger type: async rising edge trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_LOW_LEVEL    (0xA) /*!< PDS_IO INT trigger type: async low level trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_HIGH_LEVEL   (0xB) /*!< PDS_IO INT trigger type: async high level trigger */
#define BL_LP_PDS_IO_TRIG_NONE               (0xF) /*!< PDS_IO INT trigger type: none */
/**
  * @}
  */

#define TIME_DEBUG_NUM_MAX                   20

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
    uint8_t ap_channel;
    uint8_t tim_wakeup_en;
    uint16_t aid;

    uint8_t bssid[6];
    uint8_t local_mac[6];
    uint8_t bcmc_dtim_mode;
    uint8_t last_beacon_dtim_count; /* last beacon dtim count */

    uint8_t beacon_dtim_period; /* beacon dtim period */
    int8_t bcn_target_level;
    uint8_t dtim_num;
    uint8_t beacon_leg_rate;

    uint32_t beacon_interval_tu; /* beacon interval tu */
    uint8_t *wifi_rx_buff;
    uint32_t buf_addr;
    uint32_t pack_env;
} lp_fw_wifi_para_t;

// typedef struct {
//     uint64_t io_ie;
//     uint64_t io_pu;
//     uint64_t io_pd;
//     uint8_t io_0_5_trig_mode;
//     uint8_t io_6_36_trig_mode[31];
//     uint64_t io_wakeup_unmask;
// } lp_fw_gpio_cfg_t;

typedef struct {
    void *io_wakeup_parameter;
    uint8_t wifi_wakeup_en;
    uint8_t ble_wakeup_en;
    uint8_t rtc_wakeup_en;
    uint64_t rtc_wakeup_cnt; /* The value of the next rtc wake up */
} lp_fw_wakeup_source_t;

typedef struct {
    int32_t wakeup_reason;        /* reason of wake up */
    uint64_t wakeup_io_bits;      /* wake io bits */
    uint64_t wakeup_io_edge_bits; /* wake edge bits */
} lp_fw_wakeup_reason_t;

typedef struct {
    uint32_t mtimer_timeout_mini_us;
    uint32_t mtimer_timeout_max_us;
    uint8_t mtimer_timeout_en;
} lp_fw_mtimer_t;

typedef struct {
    uint8_t rc32k_auto_cal_en;
    uint8_t rc32k_clock_ready;
    uint8_t rc32k_trim_ready;
    uint8_t last_rc32trim_stamp_valid;
    uint64_t last_rc32trim_stamp_rtc_us;
    uint64_t last_rc32trim_stamp_beacon_us;
    int32_t rc32k_fr_ext;
    int32_t rtc32k_error_ppm;
} lp_fw_rc32k_trim_t;

typedef struct {
    int32_t *bcn_delay_sliding_win_buff;
    uint8_t bcn_delay_sliding_win_size;
    uint8_t bcn_delay_sliding_win_point;
    uint8_t bcn_delay_sliding_win_status;
    int32_t last_beacon_delay_us; /* beacon delay */
    int32_t bcn_delay_offset;
} lp_fw_bcn_delay_t;

typedef struct {
    uint32_t continuous_loss_cnt;
    uint32_t continuous_loss_cnt_max;
    /* beacon loss ctrl */
    lp_fw_bcn_loss_level_t *bcn_loss_cfg_table;
    int32_t bcn_loss_level;
    int32_t bcn_loss_loop_start;
    int32_t bcn_loss_level_max;
} lp_fw_bcn_loss_t;

typedef struct {
    uint8_t jtag_en;
    uint8_t jtag_io[4];
} lp_fw_jtag_t;

typedef struct {
    uint8_t debug_log_en;
    uint8_t uart_tx_io;
    uint8_t uart_rx_io;
    uint32_t baudrate;
} lp_fw_uart_t;

typedef struct {
    uint8_t mcu_clk_sel;
    uint8_t hclk_div;
    uint8_t bclk_div;
    uint8_t xclk_sel;
} lp_fw_clock_t;

typedef struct {
    uint32_t pattern; /*0xAA5555AA*/
    lp_fw_static_variable_t *lpfw_static_val;
    /* flash recovery */
    sf_recovery_para_t *flash_parameter;
    /* sec information */
    lp_fw_sf_sec_t *sec_cfg;
    /* ram protect information */
    lp_fw_tzc_t *tzc_cfg;
    /* wifi para */
    lp_fw_wifi_para_t *wifi_parameter;
    lp_fw_jtag_t *jtag_parameter;
    lp_fw_uart_t *uart_config;
    lp_fw_clock_t *clock_config;

    uint8_t em_size;

    uint32_t mcu_sts;

    /* system para */
    uint32_t wakeup_flag;
    uint32_t app_entry;
    uint32_t args[4];
    uint32_t cpu_regs[32];

    lp_fw_wakeup_source_t *wakeup_source_parameter;
    lp_fw_wakeup_reason_t *wakeup_reason_info;

    lp_fw_mtimer_t *mtimer_parameter;

    int32_t lpfw_wakeup_cnt;

    uint8_t transport_mode_enable;
    uint8_t feed_wdt_gpio;
    /* beacon  */
    int32_t tpre;

    int32_t last_sleep_error_us;
    uint32_t last_beacon_stamp_rtc_valid;
    uint64_t last_beacon_stamp_rtc_us; /*  */
    uint64_t last_beacon_stamp_beacon_us;

    uint8_t wifi_debug_io; /* io toggle after wifi init*/

    int32_t rtc32k_jitter_error_ppm; /* Jitter error compensation, ppm (10^6) */

    lp_fw_bcn_delay_t *bcn_delay_info;

    lp_fw_bcn_loss_t *bcn_loss_info;

    /* rtc32k_trim */
    lp_fw_rc32k_trim_t *rc32k_trim_parameter;

    /* iot2lp_para record info (internal) */
    struct bl_lp_info_s *lp_info;

    /* RTC-UTC Timestamp */
    uint32_t last_ntp_sync_timestamp;
    uint64_t last_ntp_sync_rtc;

    lp_fw_time_debug_t *time_debug;

    /* The parameter that remains after the reset, it has to be at the back */
    struct {
        /* reset reason */
        uint32_t reset_reason;
        uint32_t reset_reason_chk;
    } reset_keep;

} iot2lp_para_t;

typedef struct {
    uint8_t tim_wakeup_en : 1; /* 1: enable tim wakeup */
    uint8_t lpfw_copy     : 1; /* copy lpfw or not */
    uint8_t lpfw_verify   : 1; /* verify lpfw or not */
    uint8_t channel;
    int8_t rssi;
    uint8_t bssid[6];
    uint8_t mac[6];
    uint8_t bcmc_dtim_mode;
    uint8_t dtim_num;
    uint8_t dtim_origin;
    uint32_t mtimer_timeout_mini_us;
    uint32_t mtimer_timeout_max_us;

    uint32_t mcu_sts;
    uint32_t aid;

    uint64_t rtc_wakeup_cmp_cnt; /* 0: disable, use rtc_timeout_us */
    uint64_t rtc_timeout_us;     /* 0: disable rtc wakeup */

    int32_t wakeup_reason;  /* Cause of wakeup */
    uint32_t lpfw_recv_cnt; /* count of loss packet during rtc_timeout_ms */
    uint32_t lpfw_loss_cnt; /* count of wakeup during rtc_timeout_ms */

    uint32_t buf_addr;
    uint32_t pack_env;
} bl_lp_fw_cfg_t;

/******************** lp fw  header ********************/
typedef struct {
    uint32_t jump_code;
    uint32_t magic_code;
    uint32_t lpfw_memory_start;
    uint32_t lpfw_code_end;
    uint32_t lpfw_memory_end;
    char lpfw_version_str[];
} bl_lp_fw_info_t;

/* statistics info */
typedef struct {
    int32_t lpfw_wakeup_cnt;
    uint32_t lpfw_recv_cnt;
    uint32_t lpfw_loss_cnt;

    uint64_t time_total_us;
    uint64_t sleep_pds_us;
    uint64_t active_lpfw_us;
    uint64_t active_app_us;
} bl_lp_info_t;

typedef struct {
    void (*wakeup_io_callback)(uint64_t wake_up_io_bits);
} bl_lp_soft_irq_callback_t;

/* beacon stamp valid type */
#define BEACON_STAMP_LPFW   1
#define BEACON_STAMP_APP    2

#define LP_FW_PRE_JUMP_ADDR 0x20010000

extern uint32_t __lpfw_load_addr[];   /* ld symbol */
extern uint32_t __lpfw_share_start[]; /* ld symbol */
extern uint32_t __lpfw_share_used[];  /* ld symbol */
extern uint32_t __lpfw_share_end[];   /* ld symbol */
extern iot2lp_para_t *const iot2lp_para;
extern bl_lp_fw_cfg_t lpfw_cfg;

/* LP_HOOK */
void lp_hook_pre_sys(void *) __attribute__((weak));
void lp_hook_pre_user(void *) __attribute__((weak));
void lp_hook_pre_sleep(iot2lp_para_t *) __attribute__((weak));
void lp_hook_post_sys(iot2lp_para_t *) __attribute__((weak));

int bl_lp_sys_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg);
int bl_lp_user_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg);

void bl_lp_call_user_pre_enter(void);
void bl_lp_call_sys_pre_enter(void);

void bl_lp_call_user_after_exit(void);
void bl_lp_call_sys_after_exit(void);

int bl_lp_init(void);
int bl_lp_fw_enter(bl_lp_fw_cfg_t *bl_lp_fw_cfg);

void bl_lp_set_resume_wifi(void);
void bl_lp_clear_resume_wifi(void);
int bl_lp_get_resume_wifi(void);
void bl_set_fw_ready(void);
void bl_clear_fw_ready(void);
int bl_check_fw_ready(void);
int bl_lp_get_wake_reason(void);

char *bl_lpfw_bin_get_version_str(void);

void bl_pm_event_bit_set(enum PSM_EVENT event_bit);
void bl_pm_event_bit_clear(enum PSM_EVENT event_bit);
uint32_t bl_pm_event_get(void);
void bl_pm_enter_ps(void);
void bl_pm_exit_ps(void);

void lp_fw_print_cpu_para(uint32_t save_addr);

void bl_lp_debug_record_time(iot2lp_para_t *iot_lp_para, char *info_str);
void bl_lp_debug_clean_time(iot2lp_para_t *iot_lp_para);
void bl_lp_debug_dump_time(iot2lp_para_t *iot_lp_para);

void bl_lp_bcn_loss_cnt_clear(void);
void bl_lp_bcn_timestamp_update(uint64_t beacon_timestamp_us, uint64_t rtc_timestamp_us, uint32_t mode);
uint32_t bl_lp_get_beacon_interval_tu(void);
uint32_t bl_lp_get_dtim_num(void);

uint64_t bl_lp_get_virtual_us(void);
int bl_lp_set_32k_clock_ready(uint8_t ready_val);
int bl_lp_get_32k_clock_ready(void);
int bl_lp_set_32k_trim_ready(uint8_t ready_val);
int bl_lp_get_32k_trim_ready(void);
int bl_lp_get_bcn_delay_ready(void);

int bl_lp_beacon_interval_update(uint16_t beacon_interval_tu);
int bl_lp_beacon_tim_update(uint8_t *tim, uint8_t mode);

void bl_lp_time_info_update_app(struct bl_lp_info_s *lp_info);
void bl_lp_time_info_update_pds(struct bl_lp_info_s *lp_info);
void bl_lp_time_info_update_lpfw(struct bl_lp_info_s *lp_info);
void bl_lp_info_get(bl_lp_info_t *lp_info);
void bl_lp_info_clear(struct bl_lp_info_s *lp_info);
void bl_lp_fw_bcn_loss_cfg_dtim_default(uint8_t dtim_num);
void bl_lp_fw_bcn_loss_cfg(lp_fw_bcn_loss_level_t *cfg_table, uint16_t table_num, uint16_t loop_start,
                           uint16_t loss_max);

#endif
