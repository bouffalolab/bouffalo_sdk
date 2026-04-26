#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>

#include "bl_lp.h"

#if defined(BL616)
#include "bl616.h"
#include "bl616_clock.h"
#elif defined(BL616CL)
#include "bl616cl.h"
#include "bl616cl_clock.h"
#elif defined(BL618DG)
#include "bl618dg.h"
#include "bl618dg_clock.h"
#endif

#include "bflb_rtc.h"
#include "bflb_mtimer.h"
#include "bflb_irq.h"

#include "wifi_mgmr_ext.h"
#include "macsw.h"
#include "tickless_hook.h"

#ifdef TICKLESS_DEBUG
#define tickless_debugf(fmt, ...) printf("[TICKLESS]: " fmt "\r\n", ##__VA_ARGS__)
#else
#define tickless_debugf(...)
#endif

extern int macswl_ps_sleep_common_check(void);
extern int macswl_ps_sleep_check(void);
extern int macswl_connected_enter_ops(void);
extern void macswl_regs_save_ops(void);

static int enable_tickless = 0;

int pds_wakeup_overhead = 0;
static uint64_t ulLowPowerTimeEnterFunction;
static uint64_t ulLowPowerTimeAfterSleep;

int debug_abort_tickless = 0;

static inline TickType_t rtc_diff_to_tick(uint64_t before, uint64_t after)
{
    return ((after - before) * 1000 / 32768);
}

#define HW2CPU(ptr) ((void *)(((uint32_t)(ptr))))

static bool tickless_ringbuffer_is_empty(void)
{
    return *(volatile uint32_t *)(HW2CPU(0x24B081D0)) == *(volatile uint32_t *)(HW2CPU(0x24B081D4));
}

static void tickless_abort_wait_for_interrupt(void)
{
    __WFI();
    portENABLE_INTERRUPTS();
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
    return 0;
}

int tickless_exit(void)
{
    enable_tickless = 0;
    return 0;
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

static bool check_system_ready_for_LowPower(uint32_t connected)
{
    /* common wifi status, which will prevent sleep, mainly TX/RX */
    if (macswl_ps_sleep_common_check() != 0) {
        tickless_debugf("Sleep Abort!, macswl ps sleep common check\r\n");
        return false;
    }

    if (!tickless_ringbuffer_is_empty()) {
        tickless_debugf("Sleep Abort! ringbuffer is not empty\r\n");
        return false;
    }

    if (connected) {
        /* 32k clock and trim check */
        if (bl_lp_get_32k_clock_ready() == 0 || bl_lp_get_32k_trim_ready() == 0) {
            tickless_debugf("Sleep Abort!, lpfw 32k_trim not ready!\r\n");
            return false;
        }

        if (bl_lp_get_bcn_delay_ready() == 0) {
            tickless_debugf("Sleep Abort!, bcn offset calculate not ready!\r\n");
            return false;
        }

        if (bl_pm_event_get() || (xTaskGetHandle("CONNECT") != NULL)) {
            tickless_debugf("Sleep Abort! %d", __LINE__);
            return false;
        }

        /* check wifi PS state */
        if (macswl_ps_sleep_check() != 0) {
            tickless_debugf("Sleep Abort! ps_sleep_check %d", __LINE__);
            return false;
        }

        macswl_connected_enter_ops();
        if (bl_pm_wifi_config_get(&lpfw_cfg) != 0) {
            tickless_debugf("Sleep Abort! wifi_config %d", __LINE__);
            return false;
        }

#if defined(BL616)
        if (bl_lp_fw_enter_check_allow() == 0) {
            tickless_debugf("Sleep Abort! bcn_dtim check %d", __LINE__);
            return false;
        }
#endif
    } else {
        /* 32k clock check */
        if (bl_lp_get_32k_clock_ready() == 0) {
            tickless_debugf("Sleep Abort!, lpfw 32k_clock not ready!\r\n");
            return false;
        }
    }

    tickless_debugf("Next wake: %s, next tick:%ld, current tick:%ld", wake_task_name, wake_next_tick,
                    xTaskGetTickCount());

    if (bl_pm_event_get() || (xTaskGetHandle("CONNECT") != NULL)) {
        tickless_debugf("Sleep Abort! %d", __LINE__);
        return false;
    }

    return true;
}

/* Define the function that is called by portSUPPRESS_TICKS_AND_SLEEP(). */
void vApplicationSleep(TickType_t xExpectedIdleTime)
{
    eSleepModeStatus eSleepStatus;
    int32_t real_rtc_tick = 0;
    int connected, wake_reason;
    bool wifi_connected;
    bool is_ready = true;
    const struct tickless_hooks *hooks = tickless_hooks_get();

    if (unlikely(!enable_tickless)) {
        __WFI();
        return;
    }

    /* Enter a critical section that will not effect interrupts bringing the MCU
     * out of sleep mode. */
    portDISABLE_INTERRUPTS();

    ulLowPowerTimeEnterFunction = bflb_rtc_get_time(NULL);

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

    /* check wifi connected whether or not */
    connected = wifi_mgmr_sta_get_bssid(lpfw_cfg.bssid);
    wifi_connected = (connected == 0);

    if (wifi_connected) {
        // tickless_debugf("bssid: %02X:%02X:%02X:%02X:%02X:%02X\r\n", lpfw_cfg.bssid[0], lpfw_cfg.bssid[1],
        //                 lpfw_cfg.bssid[2], lpfw_cfg.bssid[3], lpfw_cfg.bssid[4], lpfw_cfg.bssid[5]);
        lpfw_cfg.tim_wakeup_en = 1;
    } else {
        lpfw_cfg.tim_wakeup_en = 0;
    }

    is_ready = check_system_ready_for_LowPower(wifi_connected);
    if (is_ready == false) {
        // __WFI();
        return;
    } else {
    }


    if (unlikely(eSleepStatus == eNoTasksWaitingTimeout)) {
        lpfw_cfg.rtc_wakeup_cmp_cnt = 0;
        if (wifi_connected) {
            lpfw_cfg.tim_wakeup_en = 1;
            lpfw_cfg.rtc_timeout_us = 0;
        } else {
            lpfw_cfg.tim_wakeup_en = 0;
            lpfw_cfg.rtc_timeout_us = (60 * 1000 - pds_wakeup_overhead) * 1000;
        }
    } else {
        lpfw_cfg.rtc_wakeup_cmp_cnt = 0;
        lpfw_cfg.rtc_timeout_us = (xExpectedIdleTime - pds_wakeup_overhead) * 1000;
    }


    macswl_regs_save_ops();

    bl_lpfw_ram_load();
    lpfw_cfg.lpfw_copy = 0;
    lpfw_cfg.lpfw_verify = 0;
    lpfw_cfg.rtc_wakeup_cmp_cnt = 0;
    lpfw_cfg.mtimer_timeout_mini_us = 4500;
    lpfw_cfg.mtimer_timeout_max_us = 12000;
    lpfw_cfg.dtim_num = lpfw_cfg.dtim_origin;

    if (hooks->allow_enter != NULL && !hooks->allow_enter(hooks->allow_enter_arg)) {
        tickless_abort_wait_for_interrupt();
        return;
    }

    /* Recheck after the early screening in case ringbuffer data arrives during the setup window. */
    if (!tickless_ringbuffer_is_empty()) {
        tickless_abort_wait_for_interrupt();
        return;
    }

    if (hooks->prepare_enter != NULL && !hooks->prepare_enter(hooks->prepare_enter_arg)) {
        tickless_debugf("Sleep Abort! prepare_enter");
        tickless_abort_wait_for_interrupt();
        return;
    }

    int ret = bl_lp_fw_enter(&lpfw_cfg);
    if (ret < 0) {
        tickless_debugf("Sleep Abort! bl_lp_fw_enter ret=%d", ret);
        portENABLE_INTERRUPTS();
        return;
    }

    ulLowPowerTimeAfterSleep = bflb_rtc_get_time(NULL);
    real_rtc_tick = (int32_t)rtc_diff_to_tick(ulLowPowerTimeEnterFunction, ulLowPowerTimeAfterSleep);
    wake_reason = bl_lp_get_wake_reason();

    if (likely(wake_reason & LPFW_WAKEUP_TIME_OUT)) {
        int32_t real_tick_delta = real_rtc_tick - (lpfw_cfg.rtc_timeout_us / 1000);
        pds_wakeup_overhead = real_tick_delta;
        vTaskStepTick(xExpectedIdleTime);
    } else {
        vTaskStepTick(likely(real_rtc_tick <= xExpectedIdleTime) ? real_rtc_tick : xExpectedIdleTime);
    }

    if (wake_reason & LPFW_WAKEUP_TIME_OUT) {
        tickless_debugf("wakeup TIMEOUT\r\n");
    } else if (wake_reason & LPFW_WAKEUP_WIFI) {
        tickless_debugf("wakeup WIFI\r\n");
    } else if (wake_reason & LPFW_WAKEUP_WIFI_BROADCAST) {
        tickless_debugf("wakeup BROADCAST\r\n");
    } else if (wake_reason & LPFW_WAKEUP_AP_LOSS) {
        tickless_debugf("wakeup APLOSS\r\n");
    } else if (wake_reason & LPFW_WAKEUP_IO) {
        tickless_debugf("wakeup IO\r\n");
    } else {
        tickless_debugf("wakeup OTHERS.\r\n");
    }

    if (hooks->after_resume != NULL && !hooks->after_resume(hooks->after_resume_arg)) {
        tickless_debugf("after_resume reported failure");
    }

    tickless_debugf("E:%ld, R:%ld, O:%ld W:0x%lx", xExpectedIdleTime, real_rtc_tick,
                    pds_wakeup_overhead, wake_reason);

    /* resume wifi task must followed by vTaskStepTick */
    bl_pm_resume_wifi(1);

    tickless_debugf("wifi resume done!");

    /* Call user init callback */
    bl_lp_call_user_after_exit();

    bflb_irq_enable(MSOFT_IRQn);
    bflb_irq_enable(WIFI_IRQn);

    portENABLE_INTERRUPTS();
}
