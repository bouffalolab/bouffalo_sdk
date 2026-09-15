#ifndef __BL618DG_LP_H__
#define __BL618DG_LP_H__
#include <stdint.h>
#include "bl618dg_xip_recovery.h"
#include "bl618dg_psram_recovery.h"
#include "lpfw_custom_rx.h"
#include <stdbool.h>

typedef int (*bl_lp_cb_t)(void *arg);

#define IOT2LP_PARA_ADDR (0x20090000 + 0x0400)
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

/******************** io wakeup cfg ********************/

#define BL_LP_WAKEUP_IO_MAX_NUM                    52


#define BL_LP_IO_WAKEUP_MODE_LOW             1
#define BL_LP_IO_WAKEUP_MODE_HIGH            2
#define BL_LP_IO_WAKEUP_MODE_FALLING         3
#define BL_LP_IO_WAKEUP_MODE_RISING          4
#define BL_LP_IO_WAKEUP_MODE_RISING_FALLING  5

/** @defgroup BL_LP_AON_IO_TRIG aon io wakeup trigger mode
  * @{
  */
#define BL_LP_AON_IO_TRIG_SYNC_FALLING_EDGE        (0x0) /*!< AON_IO INT trigger type: sync falling edge trigger */
#define BL_LP_AON_IO_TRIG_SYNC_RISING_EDGE         (0x1) /*!< AON_IO INT trigger type: sync rising edge trigger */
#define BL_LP_AON_IO_TRIG_SYNC_LOW_LEVEL           (0x2) /*!< AON_IO INT trigger type: sync low level trigger */
#define BL_LP_AON_IO_TRIG_SYNC_HIGH_LEVEL          (0x3) /*!< AON_IO INT trigger type: sync high level trigger */
#define BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE (0x4) /*!< AON_IO INT trigger type: sync rising falling edge trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_FALLING_EDGE       (0x8) /*!< AON_IO INT trigger type: async falling edge trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_RISING_EDGE        (0x9) /*!< AON_IO INT trigger type: async rising edge trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_LOW_LEVEL          (0xA) /*!< AON_IO INT trigger type: async low level trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_HIGH_LEVEL         (0xB) /*!< AON_IO INT trigger type: async high level trigger */
#define BL_LP_AON_IO_TRIG_NONE                     (0xF) /*!< AON_IO INT trigger type: none */
/**
  * @}
  */

/** @defgroup BL_LP_PDS_IO_TRIG aon io wakeup trigger mode
  * @{
  */
#define BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE        (0x0) /*!< PDS_IO INT trigger type: sync falling edge trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_RISING_EDGE         (0x1) /*!< PDS_IO INT trigger type: sync rising edge trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_LOW_LEVEL           (0x2) /*!< PDS_IO INT trigger type: sync low level trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL          (0x3) /*!< PDS_IO INT trigger type: sync high level trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_RISING_FALLING_EDGE (0x4) /*!< PDS_IO INT trigger type: sync rising falling edge trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_FALLING_EDGE       (0x8) /*!< PDS_IO INT trigger type: async falling edge trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_RISING_EDGE        (0x9) /*!< PDS_IO INT trigger type: async rising edge trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_LOW_LEVEL          (0xA) /*!< PDS_IO INT trigger type: async low level trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_HIGH_LEVEL         (0xB) /*!< PDS_IO INT trigger type: async high level trigger */
#define BL_LP_PDS_IO_TRIG_NONE                     (0xF) /*!< PDS_IO INT trigger type: none */
/**
  * @}
  */

#define LPFW_EVENT_NONE                            0U
#define LPFW_EVENT_WIFI_RX                         (1U << 0)
#define LPFW_EVENT_BLE_ADV                         (1U << 1)

#define LPFW_RTC_SCHED_VALID                       (1U << 0)

/* Phase-preserving nominal lead from task wake to RX open, not arbitration WCET. */
#define LP_FW_WIFI_NOMINAL_RX_OPEN_LEAD_US         1000U

/* APP and LPFW share the same controller-expiry lead decomposition. */
#define BL_LP_BLE_MAINTENANCE_LEAD_US              1000U
#define BL_LP_BLE_PDS_EARLY_US                     300U
#define BL_LP_BLE_WAKE_TO_EXPIRY_US                (BL_LP_BLE_MAINTENANCE_LEAD_US + BL_LP_BLE_PDS_EARLY_US)

#define LP_FW_BLE_PARA_MAGIC                       0x424C4550UL /* "BLEP" */
#define LP_FW_BLE_PARA_VERSION                     20U
#define LP_FW_BLE_DFE_MODE_STANDALONE              0U
#define LP_FW_BLE_DFE_MODE_COMBO                   4U
#define LP_FW_BLE_SPDT_GPIO_INVALID                0xFFU

#define LP_FW_BLE_ST_ADV                           0U
#define LP_FW_BLE_ST_RX_CONN_IND                   1U
#define LP_FW_BLE_ST_CONN                          2U
#define LP_FW_BLE_ST_NONE                          0xFFU

#define LP_FW_BLECORE_REG_SAVE_COUNT               133U
#define LP_FW_IPCORE_REG_SAVE_COUNT                102U

#define LP_FW_DISPATCH_BLE_ACTIVE_WINDOW_US        10000U
#define LP_FW_DISPATCH_WIFI_ACTIVE_WINDOW_US       200000U
#define LP_FW_DISPATCH_INIT_SWITCH_GUARD_US        100U

#define TIME_DEBUG_NUM_MAX 20

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
    /* BLE ADV statistics produced by LPFW. */
    uint32_t ble_lpfw_adv_success_cnt;
    uint32_t ble_lpfw_adv_loss_cnt;
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
    uint64_t target_rtc_us;
    uint32_t flags;
    uint32_t wake_lead_us;
    uint32_t window_max_us;
} lp_fw_rtc_sched_t;

/* Wireless schedules shared by APP and LPFW. */
typedef struct {
    lp_fw_rtc_sched_t wifi_sched;
    lp_fw_rtc_sched_t ble_sched;
    uint32_t mcu_init_lead_us;
} lp_fw_wake_sched_t;

/* Wireless events selected for one LPFW wake round. */
typedef uint8_t lpfw_event_mask_t;


/* Retained decision for exactly one LPFW wake round. */
typedef struct {
    uint64_t wake_rtc_us;
    lpfw_event_mask_t event_mask;
} lp_fw_wake_plan_t;

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
    uint32_t magic;
    uint16_t version;
    uint16_t cs_off;
    uint16_t tx0_off;
    uint16_t tx1_off;
    uint16_t adv_data_off;
    uint8_t adv_pdu_len;
    uint8_t adv_data_len;
    uint16_t rx_desc_off;
    uint16_t rx_data_off;
    uint16_t rx_data_len;
    uint16_t adv_interval_min;
    uint32_t adv_sched_next_hs;
    uint16_t adv_sched_next_hus;
    uint16_t adv_sched_delta_hs;
    uint32_t saved_blecore_addr;
    uint32_t saved_ipcore_addr;
    uint16_t saved_blecore_count;
    uint16_t saved_ipcore_count;
    uint32_t conn_ind_rx_desc_off;
    uint32_t consecutive_no_adv;
    uint32_t sleep_duration;
    uint8_t ble_activity_state;
    uint8_t lpfw_ble_awake;
    uint8_t dfe_mode;
    uint8_t spdt_enabled;
    uint8_t spdt_gpio;
    uint8_t reserved;
} lp_fw_ble_para_t;

typedef struct {
    uint32_t pattern; /*0xAA5555AA*/
    lp_fw_static_variable_t *lpfw_static_val;
    /* psram recovery */
    bl_lp_psram_cfg_t *psram_parameter;
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
    lp_fw_ble_para_t *ble_parameter;

    uint8_t em_size;

    uint32_t mcu_sts;
    uint32_t img_len;

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

    lp_fw_wake_sched_t wake_sched;

    /* One-wake-round plan retained in no-cache RAM. */
    lp_fw_wake_plan_t *wake_plan;

    uint64_t last_ntp_sync_rtc;

    lp_fw_time_debug_t *time_debug;

    /* Optional APP-owned Custom RX configuration in retained no-cache RAM. */
    lp_fw_custom_rx_cfg_t *custom_rx_parameter;

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
    uint8_t ble_wakeup_en : 1; /* 1: enable ble pds timer wakeup */
    uint8_t channel;
    int8_t rssi;
    uint8_t bssid[6];
    uint8_t mac[6];
    uint8_t bcmc_dtim_mode;
    uint8_t dtim_num;
    uint8_t dtim_origin;
    uint16_t rsv;
    uint32_t mtimer_timeout_mini_us;
    uint32_t mtimer_timeout_max_us;
    uint32_t ble_pds_sleep_us; /* APP -> LPFW first BLE PDS wakeup time */

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
    uint32_t ble_lpfw_adv_success_cnt;
    uint32_t ble_lpfw_adv_loss_cnt;

    uint64_t time_total_us;
    uint64_t sleep_pds_us;
    uint64_t active_lpfw_us;
    uint64_t active_app_us;
} bl_lp_info_t;

typedef struct {
    void (*wakeup_io_callback)(uint64_t wake_up_io_bits);
} bl_lp_soft_irq_callback_t;

#define LP_FW_PRE_JUMP_ADDR 0x20010000

extern uint32_t __attribute__((weak)) __lpfw_load_addr[];     /* ld symbol */
extern uint32_t __attribute__((weak)) __lpfw_share_start__[]; /* ld symbol */
extern uint32_t __attribute__((weak)) __lpfw_share_used__[];  /* ld symbol */
extern uint32_t __attribute__((weak)) __lpfw_share_end__[];   /* ld symbol */
extern iot2lp_para_t *const iot2lp_para;
extern bl_lp_fw_cfg_t lpfw_cfg;

/* internal api */
void bl_lp_time_info_update_app(struct bl_lp_info_s *lp_info);
void bl_lp_time_info_update_pds(struct bl_lp_info_s *lp_info);
void bl_lp_time_info_update_lpfw(struct bl_lp_info_s *lp_info);
/* user api */
void bl_lp_info_clear(struct bl_lp_info_s *lp_info);

#if defined(CONFIG_LPFW_CUSTOM_RX)
int bl_lp_custom_rx_config_set(lp_fw_custom_rx_cfg_t *cfg);
lp_fw_custom_rx_cfg_t *bl_lp_custom_rx_config_get(void);
#endif

void bl_lp_rc32k_save_code(uint32_t code);
void bl_lp_rc32k_restore_code(uint32_t state);

int bl_lp_pds_enter_with_restore(uint32_t pds_level, uint32_t sleep_time);
/** Return wakeup-source bits captured by the latest restored PDS entry. */
uint32_t bl_lp_pds_get_wakeup_source(void);
/** Return GPIO pad bits captured by the latest restored PDS entry. */
uint64_t bl_lp_pds_get_wakeup_gpio(void);
uint8_t lp_fw_wake_plan_capability_get(const uint8_t wifi_channel, const uint8_t dfe_mode);
int lp_fw_wake_plan_build(const lp_fw_wake_sched_t *sched, uint8_t capability_flags, lp_fw_wake_plan_t *plan);
void bl_lp_sched_publish(uint32_t id,uint32_t lead_us, uint32_t window_us, uint64_t target_at);

#endif
