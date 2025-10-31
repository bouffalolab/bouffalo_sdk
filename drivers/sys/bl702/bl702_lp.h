#ifndef __BL702_LP_H__
#define __BL702_LP_H__
#include <stdint.h>

#if 0
#define BL_LP_LOG        printf
#define BL_LP_TIME_DEBUG 1
#else
#define BL_LP_LOG(...)
#define BL_LP_TIME_DEBUG 0
#endif

#ifndef LP_FW_START_ADDR
#define LP_FW_START_ADDR          0x68012800//
#endif

#define IOT2LP_PARA_ADDR          0x20010400
#define LP_FW_MAX_SIZE            23 * 1024


#define iot2lp_para               ((iot2lp_para_t *)IOT2LP_PARA_ADDR)

#define PDS_WAKEUP_MINI_LIMIT_US  (1220)
#define PDS_WAKEUP_MINI_LIMIT_CNT (40)
#define PDS_WAKEUP_DELAY_US  (1150)
#define PDS_WAKEUP_DELAY_CNT (38)

#define TIME_DEBUG_NUM_MAX 10

typedef struct {
    uint64_t time_stamp_us;
    char *info_str;
    uint32_t trig_cnt;
} lp_fw_time_debug_t;


typedef struct {
    uint32_t rsv;
} lp_fw_variables_t;

typedef struct {
    uint32_t rsv;
} lp_fw_constants_t;

typedef struct {
    lp_fw_variables_t lpfw_variables;
    lp_fw_constants_t lpfw_constants;
} lp_fw_static_variable_t;

typedef struct {
    uint32_t pattern; /*0xAA5555AA*/
    lp_fw_static_variable_t *lpfw_static_val;
    /* flash recovery */
    uint32_t mcu_sts;
    void *flash_cfg;
    uint32_t flash_jdec_id;
    uint32_t encrypt_type;
    uint32_t xts_mode;
    uint32_t img_len;
    uint8_t aesiv[16];
    uint8_t flash_clk;
    uint8_t flash_clk_div;
    uint8_t do_xip_recovery;
    uint8_t xip_rsvd;

    /* wifi para */

    /* system para */
    uint32_t wakeup_flag;
    uint32_t flash_offset;
    uint32_t app_entry;
    uint32_t args[4];
    uint32_t cpu_regs[32];
    uint32_t l1c_way_dis;
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


    lp_fw_time_debug_t *time_debug;

    /* The parameter that remains after the reset, it has to be at the back */
    struct {
        /* reset reason */
        uint32_t reset_reason;
        uint32_t reset_reason_chk;
    } reset_keep;

} iot2lp_para_t;

typedef int (*bl_lp_cb_t)(void *arg);

/* Cause of wakeup */
#define LPFW_WAKEUP_UNKOWN    0
#define LPFW_WAKEUP_PDSTIMER  (1 << 0)
#define LPFW_WAKEUP_RTC       (1 << 1)
#define LPFW_WAKEUP_IO        (1 << 2)
#define LPFW_WAKEUP_ACOMP     (1 << 3)


/* Conversion of units */
#define BL_US_TO_PDS_CNT(us)  ((us) * 512 / 15625)  /* us * 32768 / 1000 /1000 */
#define BL_MS_TO_PDS_CNT(ms)  ((ms) * 4096 / 125)   /* ms * 32768 / 1000 */
#define BL_PDS_CNT_TO_US(cnt) ((cnt) * 15625 / 512) /* cnt / 32768 * 1000 /1000 */
#define BL_PDS_CNT_TO_MS(cnt) ((cnt) * 125 / 4096)  /* cnt / 32768 * 1000 */



typedef struct {
    uint32_t mcu_sts;
    uint32_t rsv;

    uint64_t rtc_timeout_us;     /* 0: disable rtc wakeup */

    uint64_t pds_timeout_us;     /* 0: disable pds wakeup */

    int32_t wakeup_reason;  /* Cause of wakeup */

} bl_lp_fw_cfg_t;

extern bl_lp_fw_cfg_t lpfw_cfg;

#if 0
typedef struct {
    uint32_t hbn_pattern; /*0x55AAAA55*/
    uint32_t wakeup_reason;
    uint64_t hbn_sleep_period;
    uint8_t hbn_level;
    uint8_t hal_boot2_lp_ret;
    uint8_t iot2boot2_para_rsv[2];
    uint8_t wkup_io_bits;
    uint8_t wkup_io_edge_bits;
    uint8_t wkup_acomp_bits;
    uint8_t wkup_acomp_edge_bits;
    uint8_t hbn_io_trigger_type;
    uint64_t wkup_rtc_cnt;
    uint32_t wdt_pattern; /*0xAAAA5555*/
    uint8_t feed_wdt_io;
    uint8_t feed_wdt_type;
    uint32_t feed_wdt_continue_times;
    uint32_t feed_wdt_max_continue_times;
} iot2boot2_para_t;

typedef struct {
    uint64_t hbn_sleep_cnt;
    uint8_t hbn_level;
} bl_lp_hbn_fw_cfg_t;
#endif

/******************** io wakeup cfg ********************/

#define BL_LP_WAKEUP_IO_MAX_NUM                    35

/** @defgroup BL_LP_IO_INPUT_EN input enable
  * @{
  */
#define BL_LP_IO_INPUT_DISABLE                     0
#define BL_LP_IO_INPUT_ENABLE                      1
/**
  * @}
  */

/** @defgroup BL_LP_AON_IO_TRIG aon io wakeup trigger mode
  * @{
  */
#define BL_LP_AON_IO_TRIG_SYNC_FALLING_EDGE        (0x0) /*!< AON_IO INT trigger type: sync falling edge trigger */
#define BL_LP_AON_IO_TRIG_SYNC_RISING_EDGE         (0x1) /*!< AON_IO INT trigger type: sync rising edge trigger */
#define BL_LP_AON_IO_TRIG_SYNC_LOW_LEVEL           (0x2) /*!< AON_IO INT trigger type: sync low level trigger */
#define BL_LP_AON_IO_TRIG_SYNC_HIGH_LEVEL          (0x3) /*!< AON_IO INT trigger type: sync high level trigger */
/* #define BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE (0x4) */ /*!< AON_IO INT trigger type: sync rising falling edge trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_FALLING_EDGE       (0x4) /*!< AON_IO INT trigger type: async falling edge trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_RISING_EDGE        (0x5) /*!< AON_IO INT trigger type: async rising edge trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_LOW_LEVEL          (0x6) /*!< AON_IO INT trigger type: async low level trigger */
#define BL_LP_AON_IO_TRIG_ASYNC_HIGH_LEVEL         (0x7) /*!< AON_IO INT trigger type: async high level trigger */
/*#define BL_LP_AON_IO_TRIG_NONE                     (0xF) *//*!< AON_IO INT trigger type: none */
/**
  * @}
  */

#if 0
/** @defgroup BL_LP_PDS_IO_TRIG aon io wakeup trigger mode
  * @{
  */
#define BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE        (0x0) /*!< PDS_IO INT trigger type: sync falling edge trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_RISING_EDGE         (0x1) /*!< PDS_IO INT trigger type: sync rising edge trigger */
#define BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL          (0x3) /*!< PDS_IO INT trigger type: sync high level trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_FALLING_EDGE       (0x8) /*!< PDS_IO INT trigger type: async falling edge trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_RISING_EDGE        (0x9) /*!< PDS_IO INT trigger type: async rising edge trigger */
#define BL_LP_PDS_IO_TRIG_ASYNC_HIGH_LEVEL         (0xB) /*!< PDS_IO INT trigger type: async high level trigger */
#define BL_LP_PDS_IO_TRIG_NONE                     (0xF) /*!< PDS_IO INT trigger type: none */
/**
  * @}
  */
#endif

/** @defgroup BL_LP_RES resistors select
  * @{
  */
#define BL_LP_IO_RES_PULL_UP                       1
#define BL_LP_IO_RES_PULL_DOWN                     2
#define BL_LP_IO_RES_NONE                          3
/**
  * @}
  */

typedef struct {
    /* input enable, use @ref BL_LP_IO_INPUT_EN */
    uint8_t io_9_ie;
    uint8_t io_10_ie;
    uint8_t io_11_ie;
    uint8_t io_12_ie;

    /* wake up mask */
    uint64_t io_wakeup_unmask; /* bit[0:31] --> GPIO0 ~ GPIO31 */

    /* trigger mode */
    uint8_t io_9_12_pds_trig_mode;   /* use @ref BL_LP_PDS_IO_TRIG */

} bl_lp_io_cfg_t;


typedef struct {
    uint8_t pin;
    uint8_t ie;
    uint8_t res_mode;
    uint8_t unmask;
    uint8_t trigMode;
} bl_lp_aon_io_cfg_t;


#define BL_LP_IO_WAKEUP_MODE_LOW             1
#define BL_LP_IO_WAKEUP_MODE_HIGH            2
#define BL_LP_IO_WAKEUP_MODE_FALLING         3
#define BL_LP_IO_WAKEUP_MODE_RISING          4

/******************** acomp wakeup cfg ********************/

#define BL_LP_WAKEUP_ACOMP_MAX_NUM           2

/** @defgroup BL_LP_ACOMP_EN acomp input enable
  * @{
  */
#define BL_LP_ACOMP_DISABLE                  0
#define BL_LP_ACOMP_ENABLE                   1
/**
  * @}
  */

/** @defgroup BL_LP_ACOMP_TRIG acomp wakeup trigger mode
  * @{
  */
#define BL_LP_ACOMP_TRIG_EDGE_FALLING        1
#define BL_LP_ACOMP_TRIG_EDGE_RISING         2
#define BL_LP_ACOMP_TRIG_EDGE_FALLING_RISING 3
/**
  * @}
  */

typedef struct {
    /* input enable, use @ref BL_LP_ACOMP_EN */
    uint8_t acomp0_en;
    uint8_t acomp1_en;

    /* Map to pins num, range: 2, 3, 10, 12, 13, 14, 19 */
    uint8_t acomp0_io_num;
    uint8_t acomp1_io_num;

    /* trigger mode, use @ref BL_LP_ACOMP_TRIG  */
    uint8_t acomp0_trig_mode;
    uint8_t acomp1_trig_mode;

} bl_lp_acomp_cfg_t;

#define BL_LP_ACOMP_WAKEUP_MODE_FALLING 1
#define BL_LP_ACOMP_WAKEUP_MODE_RISING  2

/******************** acomp end ********************/

typedef struct {
    void (*wakeup_io_callback)(uint64_t wake_up_io_bits);
    void (*wakeup_acomp_callback)(uint32_t wake_up_acomp);
} bl_lp_soft_irq_callback_t;

/* statistics info */
typedef struct {
    int32_t lpfw_wakeup_cnt;
    uint32_t lpfw_recv_cnt;
    uint32_t lpfw_loss_cnt;
} bl_lp_statistics_t;

/* LP_HOOK */
void lp_hook_pre_sys(void *) __attribute__((weak));
void lp_hook_pre_user(void *) __attribute__((weak));
void lp_hook_pre_sleep(iot2lp_para_t *) __attribute__((weak));
void lp_hook_post_sys(iot2lp_para_t *) __attribute__((weak));
void lp_hook_post_user(iot2lp_para_t *) __attribute__((weak));

int bl_lp_init(void);
int bl_lp_sys_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg);
int bl_lp_user_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg);

void bl_lp_call_user_pre_enter(void);
void bl_lp_call_sys_pre_enter(void);

void bl_lp_call_user_after_exit(void);
void bl_lp_call_sys_after_exit(void);

int bl_lp_beacon_interval_update(uint16_t beacon_interval_tu);

int bl_lp_fw_enter(bl_lp_fw_cfg_t *bl_lp_fw_cfg);
uint8_t bl_lp_get_hal_boot2_lp_ret(void);
uint32_t bl_lp_get_hbn_wakeup_reason(void);
void bl_lp_get_hbn_wakeup_io_bits(uint8_t *io_bits, uint8_t *io_edge_bits);
void bl_lp_get_hbn_wakeup_acomp_bits(uint8_t *acomp_bits, uint8_t *acomp_edge_bits);
int bl_lp_fw_enter_check_allow(void);
int bl_lp_get_next_beacon_time(uint8_t mode);
int bl_lp_get_wake_reason(void);
int bl_lp_get_wake_io_state(void);

uint64_t bl_lp_get_virtual_us(void);



int bl_lp_io_wakeup_cfg(bl_lp_io_cfg_t *io_wakeup_cfg);
void bl_lp_wakeup_io_int_register(void (*wakeup_io_callback)(uint64_t wake_up_io_bits));
int bl_lp_wakeup_io_get_mode(uint8_t io_num);

int bl_lp_acomp_wakeup_cfg(bl_lp_acomp_cfg_t *acomp_wakeup_cfg);
void bl_lp_wakeup_acomp_int_register(void (*wakeup_acomp_callback)(uint32_t wake_up_acomp));
int bl_lp_wakeup_acomp_get_mode(uint8_t acomp_num);

void bl_lp_debug_record_time(iot2lp_para_t *iot_lp_para, char *info_str);
void bl_lp_debug_clean_time(iot2lp_para_t *iot_lp_para);
void bl_lp_debug_dump_time(iot2lp_para_t *iot_lp_para);

void bl_set_fw_ready(void);
void bl_clear_fw_ready(void);
int bl_check_fw_ready(void);

uint32_t bl_pm_event_get(void);
void bl_pm_enter_ps(void);
void bl_pm_exit_ps(void);

void bl_lp_xip_para_save(void);
int bl_lp_sf_cfg_init_ext_flash_gpio(uint8_t ext_flash_pin);
int bl_lp_sf_cfg_init_flash_gpio(uint8_t flash_pin_cfg);
void bl_lp_xip_recovery(void);

#endif
