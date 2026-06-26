#ifndef __BL_LP_H__
#define __BL_LP_H__
#include <stdint.h>
#include "bflb_irq.h"

#if defined(BL702)
#include "bl702_lp.h"
#elif defined(BL602)
#include "bl602_lp.h"
#elif defined(BL702L)
#include "bl702l_lp.h"
#elif defined(BL616)
#include "bl616_lp.h"
#elif defined(BL616CL)
#include "bl616cl_lp.h"
#elif defined(BL618DG)
#include "bl618dg_lp.h"
#endif

#ifdef SHARED_FUNC_EN
    #define SHARE_HOOK(func_name, ...)   shared_##func_name(__VA_ARGS__)
#else
    #define SHARE_HOOK(func_name, ...)   func_name(__VA_ARGS__)
#endif

#define LP_HOOK(x, ...)           \
    if (&lp_hook_##x) {           \
        lp_hook_##x(__VA_ARGS__); \
    }

/* Cause of wakeup */
#define LPFW_WAKEUP_UNKOWN              (0)
#define LPFW_WAKEUP_TIME_OUT            (1 << 0)
#define LPFW_WAKEUP_WIFI                (1 << 1)
#define LPFW_WAKEUP_WIFI_BROADCAST      (1 << 2)
#define LPFW_WAKEUP_AP_LOSS             (1 << 3)
#define LPFW_WAKEUP_IO                  (1 << 4)
#define LPFW_WAKEUP_ACOMP               (1 << 5)
#define LPFW_WAKEUP_BLE                 (1 << 6)
#define LPFW_WAKEUP_LOSS_CFG_OVER       (1 << 7)

/* Conversion of units */
#define BL_US_TO_PDS_CNT(us)  ((us) * 512 / 15625)  /* us * 32768 / 1000 /1000 */
#define BL_MS_TO_PDS_CNT(ms)  ((ms) * 4096 / 125)   /* ms * 32768 / 1000 */
#define BL_PDS_CNT_TO_US(cnt) ((cnt) * 15625 / 512) /* cnt / 32768 * 1000 /1000 */
#define BL_PDS_CNT_TO_MS(cnt) ((cnt) * 125 / 4096)  /* cnt / 32768 * 1000 */

/* beacon stamp valid type */
#define BEACON_STAMP_LPFW   1
#define BEACON_STAMP_APP    2

/* soft interrupt */
#if defined(BL618DG) && !defined(CPU_MODEL_A0)
#define BL_LP_SOFT_INT_TRIG  (bflb_irq_set_pending(MSOFT_IRQn))
#define BL_LP_SOFT_INT_CLEAR (bflb_irq_clear_pending(MSOFT_IRQn))
#else
#define BL_LP_SOFT_INT_TRIG  (csi_vic_set_pending_irq(MSOFT_IRQn))
#define BL_LP_SOFT_INT_CLEAR (csi_vic_clear_pending_irq(MSOFT_IRQn))
#endif

#define PROTECT_AF_MS        (10)
#define PROTECT_BF_MS        (5)

int bl_lpfw_ram_load(void);
int bl_pm_wifi_config_get(bl_lp_fw_cfg_t *pcfg);

/* LP_HOOK (weak declarations) */
void lp_hook_pre_sys(void *) __attribute__((weak));
void lp_hook_pre_user(void *) __attribute__((weak));
void lp_hook_pre_sleep(iot2lp_para_t *) __attribute__((weak));
void lp_hook_post_sys(iot2lp_para_t *) __attribute__((weak));

/* Callback register */
int bl_lp_sys_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg);
int bl_lp_user_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg);

void bl_lp_call_user_pre_enter(void);
void bl_lp_call_sys_pre_enter(void);

void bl_lp_call_user_after_exit(void);
void bl_lp_call_sys_after_exit(void);

/* Core LP functions */
int bl_lp_init(void);
int bl_lp_fw_enter(bl_lp_fw_cfg_t *bl_lp_fw_cfg);

/* WiFi resume / fw ready */
void bl_lp_set_resume_wifi(void);
void bl_lp_clear_resume_wifi(void);
int bl_lp_get_resume_wifi(void);
void bl_set_fw_ready(void);
void bl_clear_fw_ready(void);
int bl_check_fw_ready(void);
int bl_lp_get_wake_reason(void);

/* LPFW info */
char *bl_lpfw_bin_get_version_str(void);

/* PM events */
void bl_pm_event_bit_set(enum PSM_EVENT event_bit);
void bl_pm_event_bit_clear(enum PSM_EVENT event_bit);
uint32_t bl_pm_event_get(void);
void bl_pm_enter_ps(void);
void bl_pm_exit_ps(void);
void bl_pm_resume_wifi(bool isr);

/* Debug */
void bl_lp_debug_record_time(iot2lp_para_t *iot_lp_para, char *info_str);
void bl_lp_debug_clean_time(iot2lp_para_t *iot_lp_para);
void bl_lp_debug_dump_time(iot2lp_para_t *iot_lp_para);

/* LP info */
void bl_lp_info_get(bl_lp_info_t *lp_info);

/* Beacon */
void bl_lp_bcn_loss_cnt_clear(void);
void bl_lp_bcn_timestamp_update(uint64_t beacon_timestamp_us, uint64_t rtc_timestamp_us, uint32_t mode);
uint32_t bl_lp_get_beacon_interval_tu(void);
uint32_t bl_lp_get_dtim_num(void);
void bl_lp_fw_bcn_loss_cfg(lp_fw_bcn_loss_level_t *cfg_table, uint16_t table_num, uint16_t loop_start, uint16_t loss_max);
void bl_lp_fw_bcn_loss_cfg_dtim_default(uint8_t dtim_num);
int bl_lp_beacon_interval_update(uint16_t beacon_interval_tu);

/* Virtual time & 32k clock */
uint64_t bl_lp_get_virtual_us(void);
int bl_lp_set_32k_clock_ready(uint8_t ready_val);
int bl_lp_get_32k_clock_ready(void);
int bl_lp_set_32k_trim_ready(uint8_t ready_val);
int bl_lp_get_32k_trim_ready(void);
int bl_lp_get_bcn_delay_ready(void);
int bl_lp_rtc_rc32k_coarse_adj(uint32_t expect_time, uint32_t rc32k_actual_time);

/* IO wakeup */
int bl_lp_io_wakeup_cfg(void *io_wakeup_cfg);
void bl_lp_wakeup_io_int_register(void (*wakeup_io_callback)(uint64_t wake_up_io_bits));
int bl_lp_wakeup_io_get_mode(uint8_t io_num);

#endif
