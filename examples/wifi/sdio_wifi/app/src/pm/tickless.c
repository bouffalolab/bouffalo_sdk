#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>

#include "bl_lp.h"
#include "bl616_clock.h"
#include "bflb_rtc.h"
#include "bflb_mtimer.h"
#include "bl616.h"
#include "bflb_irq.h"

#include "wifi_mgmr_ext.h"
#include "macsw.h"

#ifdef BL_HOSTROUTER_ENABLE
#include "sdiowifi_mgmr.h"
#endif

// #include "time_statics.h"

#ifdef TICKLESS_DEBUG
#define tickless_debugf(fmt, ...) printf("[TICKLESS]: " fmt "\r\n", ##__VA_ARGS__)
#else
#define tickless_debugf(...)
#endif

static int enable_tickless = 0;

extern int ps_resume_flag;

extern int bl_pm_wifi_config_get(bl_lp_fw_cfg_t *pcfg);

int pds_wakeup_overhead = 0;
static uint64_t ulLowPowerTimeEnterFunction;
static uint64_t ulLowPowerTimeAfterSleep;

int debug_abort_tickless = 0;

static inline TickType_t rtc_diff_to_tick(uint64_t before, uint64_t after)
{
    return ((after - before) * 1000 / 32768);
}

#ifdef TICKLESS_DEBUG
const char *wake_task_name = NULL;
TickType_t wake_next_tick = 0;
void tickless_debug_who_wake_me(const char *name, TickType_t ticks)
{
    wake_task_name = name;
    wake_next_tick = ticks;
}
#endif


int tickless_enter(void)
{
    enable_tickless = 1;
}

int tickless_exit(void)
{
    enable_tickless = 0;
}


void lp_hook_pre_user(void *env)
{
    (void)env;

    bl_lp_call_user_pre_enter();
}

void lp_hook_pre_sys(void *env)
{
    (void)env;
    bl_lp_call_sys_pre_enter();
}

/* wakeup schdule */
void lp_hook_post_sys(iot2lp_para_t *param)
{
    bl_lp_call_sys_after_exit();
    bflb_irq_enable(7);
}

/* Define the function that is called by portSUPPRESS_TICKS_AND_SLEEP(). */
void vApplicationSleep(TickType_t xExpectedIdleTime)
{
    eSleepModeStatus eSleepStatus;
    int32_t real_rtc_tick = 0;
    int connected, wake_reason;

    if (unlikely(!enable_tickless)) {
        __WFI();
        return;
    }

    /* Enter a critical section that will not effect interrupts bringing the MCU
     * out of sleep mode. */
    portDISABLE_INTERRUPTS();

    ulLowPowerTimeEnterFunction = bflb_rtc_get_time(NULL);
    //TS_RECORD(TS_WANT_TO_SLEEP);

    /* Ensure it is still ok to enter the sleep mode. */
    eSleepStatus = eTaskConfirmSleepModeStatus();

    if (unlikely(eSleepStatus == eAbortSleep)) {
        /* A task has been moved out of the Blocked state since this macro was
         * executed, or a context siwth is being held pending.  Do not enter a
         * sleep state.  Restart the tick and exit the critical section. */
        /* fixup mtimecmp */
        portENABLE_INTERRUPTS();

        tickless_debugf("Sleep Abort!, reason: %d", debug_abort_tickless);
        return;
    }

    /* common wifi status, which will prevent sleep, mainly TX/RX */
    if (macswl_ps_sleep_common_check() != 0) {
        __WFI();
        return;
    }

    /* check ringbuffer is empty */
    #define HW2CPU(ptr) ((void *)(((uint32_t)(ptr)) ))
    if (*(volatile uint32_t *)(HW2CPU(0x24B081D0)) != *(volatile uint32_t *)(HW2CPU(0x24B081D4))) {
        __WFI();
        return;
    }

    /* check wifi connected whether or not */
    connected = wifi_mgmr_sta_get_bssid(lpfw_cfg.bssid);

    if (unlikely(eSleepStatus == eNoTasksWaitingTimeout)) {
        if (connected == 0) { /* wifi connected */
            lpfw_cfg.tim_wakeup_en = 1;
            lpfw_cfg.rtc_timeout_us = 0;
            goto __enter_connected;
        } else {
            lpfw_cfg.tim_wakeup_en = 0;
            lpfw_cfg.rtc_timeout_us = (60 * 1000 - pds_wakeup_overhead) * 1000; // wakeup every minute
            goto __enter_disconnected;
        }
    } else {
        /* The actual sleep time and the expected sleep time are 14ms more.
       * Compensate back. */
        lpfw_cfg.rtc_timeout_us = (xExpectedIdleTime - pds_wakeup_overhead) * 1000;

        if (connected == 0) {
            lpfw_cfg.tim_wakeup_en = 1;
            goto __enter_connected;
        } else {
            lpfw_cfg.tim_wakeup_en = 0;
            goto __enter_disconnected;
        }
    }

__enter_connected:
    /* 32k clock and trim check */
    if (bl_lp_get_32k_clock_ready() == 0 || bl_lp_get_32k_trim_ready() == 0) {
        tickless_debugf("Sleep Abort!, lpfw 32k_trim not ready!\r\n");
        __WFI();
        return;
    }

    if(bl_lp_get_bcn_delay_ready() == 0) {
        tickless_debugf("Sleep Abort!, bcn offset calculate not ready!\r\n");
        __WFI();
        return;
    }

    if (bl_pm_event_get() || (xTaskGetHandle("CONNECT") != NULL)) {
        tickless_debugf("Sleep Abort! %d", __LINE__);
        __WFI();
        return;
    }

    /* check wifi PS state */
    if (macswl_ps_sleep_check() != 0) {
        tickless_debugf("Sleep Abort! ps_sleep_check %d", __LINE__);
        __WFI();
        return;
    }
    //TS_RECORD(TS_ALLOW_SLEEP_WIFI);

    macswl_connected_enter_ops();
    if (bl_pm_wifi_config_get(&lpfw_cfg) != 0) {
        tickless_debugf("Sleep Abort! wifi_config %d", __LINE__);
        __WFI();
        return;
    }
    //TS_RECORD(TS_ALLOW_SLEEP_WIFI_DONE);

__enter_disconnected:

    /* 32k clock check */
    if (lpfw_cfg.tim_wakeup_en == 0 && bl_lp_get_32k_clock_ready() == 0) {
        tickless_debugf("Sleep Abort!, lpfw 32k_clock not ready!\r\n");
        __WFI();
        return;
    }

    tickless_debugf("Next wake: %s, next tick:%ld, current tick:%ld", wake_task_name, wake_next_tick, xTaskGetTickCount());

    if (bl_pm_event_get() || (xTaskGetHandle("CONNECT") != NULL)) {
        tickless_debugf("Sleep Abort! %d", __LINE__);
        __WFI();
        return;
    }
    macswl_regs_save_ops();

    /* copy lpfw */
    bl_lpfw_ram_load();
    /* disable copy */
    lpfw_cfg.lpfw_copy = 0;
    /* disable verify */
    lpfw_cfg.lpfw_verify = 0;

#if 0
    if(lpfw_cfg.rtc_timeout_us){
        /* get pds cnt */
        uint64_t rtc_current_cnt;
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_current_cnt, (uint32_t *)&rtc_current_cnt + 1);
        lpfw_cfg.rtc_wakeup_cmp_cnt = rtc_current_cnt + BL_US_TO_PDS_CNT(lpfw_cfg.rtc_timeout_us);
    }else{
        lpfw_cfg.rtc_wakeup_cmp_cnt = 0;
    }
#else
    lpfw_cfg.rtc_wakeup_cmp_cnt = 0;
#endif

    lpfw_cfg.mtimer_timeout_mini_us = 4500;
    lpfw_cfg.mtimer_timeout_max_us = 12000;
    lpfw_cfg.dtim_num = lpfw_cfg.dtim_origin;

    if (*(volatile uint32_t *)(HW2CPU(0x24B081D0)) != *(volatile uint32_t *)(HW2CPU(0x24B081D4))) {
        __WFI();
        return;
    }

#ifdef BL_HOSTROUTER_ENABLE
    sdiowifi_mgmr_ps_prepare();
#endif
    bl_lp_fw_enter(&lpfw_cfg);

    /* measure overhead */
    ulLowPowerTimeAfterSleep = bflb_rtc_get_time(NULL);

    /* The difference between the actual sleep time and the expected time obtained from actual tests. */
    real_rtc_tick = (int32_t)rtc_diff_to_tick(ulLowPowerTimeEnterFunction, ulLowPowerTimeAfterSleep);

    wake_reason = bl_lp_get_wake_reason();

    // vTaskStepTick(likely(real_rtc_tick <= xExpectedIdleTime) ? real_rtc_tick : xExpectedIdleTime);

    /* find largest pds overhead time */
    if (likely(wake_reason & LPFW_WAKEUP_TIME_OUT)) { /* RTC Wakeup */
        int32_t real_tick_delta = real_rtc_tick - (lpfw_cfg.rtc_timeout_us / 1000);
        pds_wakeup_overhead = real_tick_delta;
        vTaskStepTick(xExpectedIdleTime);
    } else {
        vTaskStepTick(likely(real_rtc_tick <= xExpectedIdleTime) ? real_rtc_tick : xExpectedIdleTime);
    }

    if (wake_reason & LPFW_WAKEUP_TIME_OUT) {
        tickless_debugf("wakeup TIMEOUT\r\n");
    } else if(wake_reason & LPFW_WAKEUP_WIFI) {
        tickless_debugf("wakeup WIFI\r\n");
    } else if (wake_reason & LPFW_WAKEUP_AP_LOSS) {
        tickless_debugf("wakeup APLOSS\r\n");
    } else if (wake_reason & LPFW_WAKEUP_IO) {
        tickless_debugf("wakeup IO\r\n");
    } else {
        tickless_debugf("wakeup OTHERS.\r\n");
    }

#ifdef CONFIG_BLE_ENABLE
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_PDS);
#endif

    tickless_debugf("E:%ld, R:%ld, O:%ld W:0x%lx", xExpectedIdleTime, real_rtc_tick, pds_wakeup_overhead, wake_reason);

    /* resume wifi task must followed by vTaskStepTick */
    bl_pm_resume_wifi(1);

    tickless_debugf("wifi resume done!");

    /* Call user init callback */
    bl_lp_call_user_after_exit();

    bflb_irq_enable(MSOFT_IRQn);
    bflb_irq_enable(WIFI_IRQn);

    //TS_RECORD(TS_WAKEUP_EXIT_TICKLESS);

    portENABLE_INTERRUPTS();
}
