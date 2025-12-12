#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

#include "bl_lp.h"
#include "bl616_clock.h"
#include "bflb_rtc.h"
#include "bflb_mtimer.h"
#include "bl616.h"
#include "bflb_irq.h"
#if defined(CFG_BLE_ENABLE)
#include "btble_lib_api.h"
#endif
#include "wifi_mgmr_ext.h"
#include "macsw.h"

#include "board_rf.h"
#include "tickless.h"
#include "pm_manager.h"

//#define TICKLESS_DEBUG 0

// #include "time_statics.h"
/*****************************************************************************
 *                           MAGIC SHIT WARNING
 *
 *     !! DO NOT modify the code! Unless you know what you are doing !!
 *****************************************************************************/

#define tickless_println(fmt, ...) printf("[LP]: " fmt "\r\n", ##__VA_ARGS__)

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG >= 4
#define tickless_debug(fmt, ...) printf("[TS][D]: " fmt "\r\n", ##__VA_ARGS__)
#else
#define tickless_debug(...)
#endif

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG >= 3
#define tickless_info(fmt, ...) printf("[LP][I]: " fmt "\r\n", ##__VA_ARGS__)
#else
#define tickless_info(...)
#endif

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG >= 2
#define tickless_warn(fmt, ...) printf("[LP][W]: " fmt "\r\n", ##__VA_ARGS__)
#else
#define tickless_warn(...)
#endif

#if defined(TICKLESS_DEBUG) && TICKLESS_DEBUG >= 1
#define tickless_error(fmt, ...) printf("[LP][E]: " fmt "\r\n", ##__VA_ARGS__)
#else
#define tickless_error(...)
#endif

#if defined(TICKLESS_DEBUG) || TICKLESS_DEBUG >= 0
#define tickless_fatal(fmt, ...) printf("[LP][F]: " fmt "\r\n", ##__VA_ARGS__)
#else
#define tickless_fatal(...)
#endif

#define WAKEUP_AHEAD_US         5000

#define FIRST_RTC_EPILOGUE_TIME 180

static int g_lp_ps_wakeup_configuration = 0;

volatile int _ffag = 0;

volatile uint32_t *const pulTimeHigh = (volatile uint32_t *const)((configMTIME_BASE_ADDRESS) + 4UL); /* 8-byte typer so high 32-bit word is 4 bytes up. */
volatile uint32_t *const pulTimeLow = (volatile uint32_t *const)(configMTIME_BASE_ADDRESS);
extern const size_t uxTimerIncrementsForOneTick;
extern char __lp_mon_start[], __lp_mon_end[];

static uint64_t get_mtime(void)
{
    uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
    uint64_t current_mtime;

    do {
        ulCurrentTimeHigh = *pulTimeHigh;
        ulCurrentTimeLow = *pulTimeLow;
    } while (ulCurrentTimeHigh != *pulTimeHigh);

    current_mtime = (uint64_t)ulCurrentTimeHigh;
    current_mtime <<= 32ULL; /* High 4-byte word is 32-bits up. */
    current_mtime |= (uint64_t)ulCurrentTimeLow;

    return current_mtime;
}

#define INTERVAL(ms, p) ({                                                            \
    static uint64_t __attribute__((section(".lp_mon_ctx"))) __last_time = 0;          \
    uint64_t __current_time;                                                          \
    int ret = 0;                                                                      \
    uint64_t *q = p;                                                                  \
    if (q)                                                                            \
        *q = &__last_time;                                                            \
    __current_time = get_mtime();                                                     \
    if (__last_time == 0) {                                                           \
        __last_time = __current_time;                                                 \
    } else if (__current_time > (__last_time + uxTimerIncrementsForOneTick * (ms))) { \
        __last_time = __current_time;                                                 \
        ret = 1;                                                                      \
    }                                                                                 \
    ret;                                                                              \
})

#if 0
#define ___WFI()                                                                                                                   \
    do {                                                                                                                           \
        int irq_state;                                                                                                             \
        __asm__ volatile("csrrci %0, mstatus, %1" : "=r"(irq_state) : "i"(8));                                                     \
        GLB_Simple_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M, 7, 0);                                                     \
        CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_CLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK) / 1000000 - 1); \
        __WFI();                                                                                                                   \
        GLB_Simple_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M, 0, 3);                                                     \
        CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_CLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK) / 1000000 - 1); \
        __asm__ volatile("csrw mstatus, %0" : /* no output */ : "r"(irq_state));                                                   \
    } while (0)
#else
#define ___WFI __WFI
#endif

#define PDS_MIN_SLEEP_TIME 1500

static int enable_tickless = 0;
int g_tpre = 0;

static uint64_t rtc_enter_tickless, rtc_before_sleep;
static uint64_t rtc_after_sleep;

static struct {
    uint32_t ble_wake : 1;
} pds_flags = { 0 };

/* Enter low power and exit low power software maximum processing time. */
/*
 * ----------------------------rtc timer------------------------------->
 * | <---------prologue---------> | <--pds--> | <-----epilogue----> |
 * | <------------------------total sleep time -------------------> |
 * ^
 * |
 * enter tickless
 */
static uint64_t rtc_prologue_time_max = 0, rtc_epilogue_time_max = 0;

/* Sleep time remaining. */
static uint64_t rtc_sleep_remain;

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

int *tickless_handle_get(void)
{
    return &enable_tickless;
}

void set_wifi_ps_wakeup_configuration(int dtim_wakeup)
{
    if (dtim_wakeup == 0) {
        g_lp_ps_wakeup_configuration = 0;
    } else if (dtim_wakeup == 1) {
        g_lp_ps_wakeup_configuration = 1;
    }
}

/* Get the dynamic desired minimum sleep time. return ms */
uint32_t expected_idle_before_sleep(void)
{
    if (rtc_epilogue_time_max) {
        return BL_PDS_CNT_TO_MS(rtc_prologue_time_max) + (PDS_MIN_SLEEP_TIME / 1000) + BL_PDS_CNT_TO_MS(rtc_epilogue_time_max);
    } else {
        return BL_PDS_CNT_TO_MS(rtc_prologue_time_max) + (PDS_MIN_SLEEP_TIME / 1000) + BL_PDS_CNT_TO_MS(FIRST_RTC_EPILOGUE_TIME);
    }
}

/* Foreach all task handle */
static void handle_suspend(TaskHandle_t tsk, eTaskState state)
{
    uint8_t *vendor_flag = pcTaskGetVendorFlags(tsk);

    if (state == eDeleted) {
        return;
    }

    if (xTaskGetIdleTaskHandle() == tsk) {
        return;
    }

    *vendor_flag = 1;
    tickless_debug("Hold task:%p", tsk);
}

static void handle_resume(TaskHandle_t tsk, eTaskState state)
{
    uint8_t *vendor_flag = pcTaskGetVendorFlags(tsk);

    if (state == eDeleted) {
        return;
    }

    if (xTaskGetIdleTaskHandle() == tsk) {
        return;
    }

    if (*vendor_flag != 0) {
        tickless_debug("Release task:%p", tsk);
        *vendor_flag = 0;
    }
}

#if defined(CFG_BLE_ENABLE)
/* do recovery when ble crashed */
static void recovery_ble(void)
{
    tickless_error("recovery ble");
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_PDS);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_BTDM);

    extern void rwip_restore_ble_reg(void);
    extern void rwip_prevent_sleep_clear(uint16_t prv_slp_bit);
    extern void rwip_reset(void);
    rwip_restore_ble_reg();
    rwip_prevent_sleep_clear(0xffff);
    //rwip_reset();
}
#endif

void lp_hook_pre_user(void *env)
{
    (void)env;

    bl_lp_call_user_pre_enter();

    /* XXX: Next, interrupts will be turned off and pds must be entered. */
}

void lp_hook_pre_sys(void *env)
{
    (void)env;

    bl_lp_call_sys_pre_enter();
}

void lp_hook_pre_sleep(iot2lp_para_t *param)
{
    uint64_t delta;

    //TS_RECORD(TS_ALLOW_SLEEP_PDS);
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_before_sleep, (uint32_t *)&rtc_before_sleep + 1);

    /* measura prologue time spend  */
    delta = rtc_before_sleep - rtc_enter_tickless;
    tickless_debug("rtc_prologue_time_max: %" __PRI64(u), delta);
    /* update pds prologue max time */
    if (delta > rtc_prologue_time_max) {
        rtc_prologue_time_max = delta;
        tickless_info("rtc_prologue_time_max: %" __PRI64(u), rtc_prologue_time_max);
    }

    if (unlikely(lpfw_cfg.rtc_wakeup_cmp_cnt < (BL_US_TO_PDS_CNT(PDS_MIN_SLEEP_TIME) + rtc_before_sleep))) {
        param->wakeup_flag = 1;
        /* set wakeup reason to RTC */
        param->wakeup_reason = LPFW_WAKEUP_TIME_OUT;
        tickless_warn("rtc_sleep_remain too small, dont enter pds");
        return;
    }

    /* clear lp monitor context */
    memset(__lp_mon_start, 0, __lp_mon_end - __lp_mon_start);

    tickless_info("Next wake: %s, next tick:%" __PRI64(u) ", current tick:%" __PRI64(u),
                  wake_task_name, (uint64_t)wake_next_tick, (uint64_t)xTaskGetTickCount());
    tickless_info("Zzz. Sleep %" __PRI64(u) " (rtc cnt).", rtc_sleep_remain);
}

extern void SystemInit(void);
extern void board_init(void);
extern int bflb_sys_em_config(void);
extern void vPortSetupTimerInterrupt(void);
/* ble controller handler */
#if defined(CFG_BLE_ENABLE)
extern uint32_t rw_main_task_hdl;
#endif
extern TaskHandle_t wifi_fw_task;

volatile int _flag = 1;

/* wakeup schdule */
void lp_hook_post_sys(iot2lp_para_t *param)
{
    //TS_RECORD(TS_WAKEUP_RESTORE);

    /* Restore CPU:320M and Mtimer:1M */
    GLB_Simple_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M, 0, 3);
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_CLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK) / 1000000 - 1);

    /* { -- 0.15ms */
    SystemInit(); // <-- will disable irq
    //board_init();

    board_recovery();

    //Interrupt_Handler_Register(WIFI_IRQn, interrupt0_handler);
    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);

    //bflb_sys_em_config();
    /* } */

    if ((param->wakeup_reason & LPFW_WAKEUP_TIME_OUT) && pds_flags.ble_wake) {
        /* fixup ble wake reason */
        param->wakeup_reason |= LPFW_WAKEUP_BLE;
        pds_flags.ble_wake = 0;
    }

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);

    /* RF Restore { -- 1.25ms */
    board_rf_ctl(BRD_CTL_RF_RESET_DEFAULT, 0);
    /* } */
    tickless_info("Restore ALL RF");

    bl_lp_call_sys_after_exit();

    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_after_sleep, (uint32_t *)&rtc_after_sleep + 1);

    /* Compensation Tick */
    tickless_info("early compensate tick: %" __PRI64(u), (uint64_t)pdMS_TO_TICKS(BL_PDS_CNT_TO_MS(rtc_after_sleep - rtc_enter_tickless)));
    vTaskStepTick(pdMS_TO_TICKS(BL_PDS_CNT_TO_MS(rtc_after_sleep - rtc_enter_tickless)));

    /* Init mtimer handler */
    vPortSetupTimerInterrupt();
    bflb_irq_enable(7);  // <-- mtimer
    bflb_irq_enable(44); // <-- uart0 irq

    /* Suspend all running task */
    vTaskHandleForeachFromISR(handle_suspend);

#if defined(CFG_BLE_ENABLE)
    /* Resume blecontroller task */
    if (rw_main_task_hdl != 0) {
        /* Restore BLE */
        btble_controller_sleep_restore();

        uint8_t *vendor_flag = pcTaskGetVendorFlags((TaskHandle_t)(uintptr_t)rw_main_task_hdl);
        *vendor_flag = 0;
    }
#endif
    /* Resume wifi task */
#if 0
  if (wifi_task_handle != NULL) {
    uint8_t *vendor_flag = pcTaskGetVendorFlags((TaskHandle_t)wifi_task_handle);
    *vendor_flag = 0;
  }
#endif
    if (xTaskResumeAll()) {
        taskYIELD();
    }
}

/* Define the function that is called by portSUPPRESS_TICKS_AND_SLEEP(). */
void vApplicationSleep(TickType_t xExpectedIdleTime)
{
    eSleepModeStatus eSleepStatus;
    int32_t ble_sleep_rtc = 0;
    uint64_t delta;
    uint32_t wake_reason;
    int connected;

    if (unlikely(!enable_tickless)) {
        __WFI();
        return;
    }

    /* disable interrupt */
    portDISABLE_INTERRUPTS();

    /* Record current time */
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_enter_tickless, (uint32_t *)&rtc_enter_tickless + 1);

    /* 1 Tick must equal to 1 ms */
    rtc_sleep_remain = BL_MS_TO_PDS_CNT(xExpectedIdleTime);

    /* Enable DTIM when WiFi connected*/
    connected = !!(wifi_mgmr_sta_get_bssid(lpfw_cfg.bssid) == 0);

    //if (connected && wifi_mgmr_sta_twt_flow_get() == 0) {
    if (connected) {
        lpfw_cfg.tim_wakeup_en = 1;
    } else {
        lpfw_cfg.tim_wakeup_en = 0;
    }
#if defined(CFG_BLE_ENABLE)
    if (likely(rw_main_task_hdl != 0)) {
        /* Get BLE sleep time */
        ble_sleep_rtc = btble_controller_sleep(0);
        int64_t *p = NULL;
        int status = INTERVAL(500, &p);
        if (ble_sleep_rtc < 0) {
            if (status) {
                recovery_ble();
            }
            portENABLE_INTERRUPTS();
            ___WFI();
            return;
        } else {
            configASSERT(p != NULL);
            *p = 0;
        }

        tickless_info("ble sleep duration: %d", ble_sleep_rtc);

        /* Find min sleep time */
        if (ble_sleep_rtc > 0 && ble_sleep_rtc <= rtc_sleep_remain) {
            rtc_sleep_remain = ble_sleep_rtc;
            pds_flags.ble_wake = 1;
        }

        /* convert to absolute time */
        ble_sleep_rtc = rtc_enter_tickless + ble_sleep_rtc;
    }
#endif
    if (BL_PDS_CNT_TO_MS(rtc_sleep_remain) <= configEXPECTED_IDLE_TIME_BEFORE_SLEEP) {
        portENABLE_INTERRUPTS();
        tickless_info("Sleep Abort! %d, %" __PRI32(u), __LINE__, configEXPECTED_IDLE_TIME_BEFORE_SLEEP);
        ___WFI();
        return;
    }

    /* Check pm stauts */
    if (bl_pm_event_get() || pm_sleep_check_dispatch()) {
        portENABLE_INTERRUPTS();
        tickless_info("Sleep Abort! %d", __LINE__);
        ___WFI();
        return;
    }

    /* Ensure it is still ok to enter the sleep mode. */
    eSleepStatus = eTaskConfirmSleepModeStatus();

    if (unlikely(eSleepStatus == eAbortSleep)) {
        /* A task has been moved out of the Blocked state since this macro was
     * executed, or a context siwth is being held pending.  Do not enter a
     * sleep state.  Restart the tick and exit the critical section. */
        /* fixup mtimecmp */
        portENABLE_INTERRUPTS();

        tickless_info("Sleep Abort! %d", __LINE__);
        return;
    }

    /* wifi connected */
    if (likely(connected)) {
        /* check wifi PS state */
        if (macswl_ps_sleep_check() != 0) {
            if (INTERVAL(5000, NULL)) {
                tickless_error("!!! WiFi stuck again, Reset System !!!");
                /* TODO Reboot */
                configASSERT(0);
            }
            portENABLE_INTERRUPTS();
            ___WFI();
            return;
        }

        macswl_connected_enter_ops();
        if (bl_pm_wifi_config_get(&lpfw_cfg) != 0) {
            tickless_info("Sleep Abort! WiFi Config %d", __LINE__);
            ___WFI();
            return;
        }
        //if (rwnxl_pds_wifi_config(&lpfw_cfg) != 0) {
        //  configASSERT(0); /* should not reach here */
        //}

        if (bl_lp_fw_enter_check_allow() == 0) {
            tickless_info("Sleep Abort! bcn_dtim check");
            ___WFI();
            return;
        }
    }

    //uint32_t wifi_get_next_wakeup_timer_time(void);
    //uint64_t twt_wakeup = (uint64_t)wifi_get_next_wakeup_timer_time();
    //if (twt_wakeup) {
    //  if (twt_wakeup < WAKEUP_AHEAD_US) {
    //      portENABLE_INTERRUPTS();
    //      tickless_info("Sleep Abort! %d", __LINE__);
    //      ___WFI();
    //      return;
    //  }
    //  twt_wakeup = BL_US_TO_PDS_CNT(twt_wakeup);

    //  if (twt_wakeup < rtc_sleep_remain) {
    //      rtc_sleep_remain = twt_wakeup;
    //  }
    //}

    macswl_regs_save_ops();

    uint64_t ahead_wakeup_cost;

    if (rtc_epilogue_time_max) {
        ahead_wakeup_cost = rtc_epilogue_time_max + BL_US_TO_PDS_CNT(WAKEUP_AHEAD_US);
    } else {
        ahead_wakeup_cost = FIRST_RTC_EPILOGUE_TIME + BL_US_TO_PDS_CNT(WAKEUP_AHEAD_US);
    }

    if (rtc_sleep_remain < (ahead_wakeup_cost + 1)) {
        portENABLE_INTERRUPTS();
        tickless_info("Sleep Abort! %d", __LINE__);
        ___WFI();
        return;
    }

#if 0
  tickless_fatal("Next wake: %s, next tick:%" __PRI64(u) ", current tick:%" __PRI64(u),
                  wake_task_name, (uint64_t)wake_next_tick, (uint64_t)xTaskGetTickCount());
#endif

    uint64_t should_wakeup = rtc_sleep_remain;
    should_wakeup += rtc_enter_tickless;

    /* How much time do we have to sleep? */
    rtc_sleep_remain -= ahead_wakeup_cost;
    tickless_info("rtc_sleep_remain: %" __PRI64(d), rtc_sleep_remain);
    configASSERT((signed)rtc_sleep_remain > 0);

    /* Convert to absolute time */
    lpfw_cfg.rtc_wakeup_cmp_cnt = rtc_enter_tickless + rtc_sleep_remain;
    lpfw_cfg.mtimer_timeout_mini_us = 4500;
    lpfw_cfg.mtimer_timeout_max_us = 12000;
    lpfw_cfg.dtim_num = lpfw_cfg.dtim_origin;

    /* copy lpfw */
    lpfw_cfg.lpfw_copy = 1;
    wake_reason = bl_lp_fw_enter(&lpfw_cfg);

    /* !!! Schdule Started !!! */

    /* resume wifi task { -- 3.4ms */
    bl_pm_resume_wifi(0);
    bflb_irq_enable(WIFI_IRQn);
    tickless_info("resume wifi");
    /* } */

    delta = rtc_after_sleep - lpfw_cfg.rtc_wakeup_cmp_cnt;

    /* update pds prologue max time */
    /* N.B. delta may be negative when "rtc_sleep_remain too small, dont enter pds" */
    if ((signed)delta > (signed)rtc_epilogue_time_max && (signed)delta < 300) {
        rtc_epilogue_time_max = delta;
        tickless_info("rtc_epilogue_time_max: %" __PRI64(u), rtc_epilogue_time_max);
    }

    tickless_info("Wakeup Reason: %x", wake_reason);
    tickless_info("Exp:%" __PRI64(u) ",Real:%" __PRI64(u) ",Min:%" __PRI32(u) ",BLE:%" __PRI32(u) ",P&E:[%" __PRI64(u) ",%" __PRI64(u) "]",
                  lpfw_cfg.rtc_wakeup_cmp_cnt, rtc_after_sleep, configEXPECTED_IDLE_TIME_BEFORE_SLEEP, ble_sleep_rtc, rtc_prologue_time_max, rtc_epilogue_time_max);
#if defined(CFG_BLE_ENABLE)
    if (wake_reason & LPFW_WAKEUP_BLE) {
        if (rtc_after_sleep > ble_sleep_rtc) {
            printf("!! BLE Crashed !! real_sleep(%" __PRI64(u) ") > bleSleepDuration(%" __PRI32(u) ")", rtc_after_sleep, ble_sleep_rtc);
            recovery_ble();
        }
    }
#endif
    /* Call user init callback */
    bl_lp_call_user_after_exit();

    /* Reusume other task */
    vTaskHandleForeachFromISR(handle_resume);

    /* enable soft int */
    bflb_irq_enable(MSOFT_IRQn);

#if defined(TICKLESS_RECORD)
    time_static_record_dump();
#endif

    /* There must be a context switch here. */
    taskYIELD();

    tickless_debug("tickless return");

    /* When this function return, freertos will call xTaskResumeAll,
   * so call vTaskSuspendAll again */
    vTaskSuspendAll();
}
