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
#if defined(CFG_BLE_ENABLE)
#include "bluetooth.h"
#include "btble_lib_api.h"
#endif
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

#if defined(CFG_BLE_ENABLE) && defined(BL618DG)
static void bl_lp_ble_parameter_invalidate(void)
{
    iot2lp_para_t *lp = (iot2lp_para_t *)IOT2LP_PARA_ADDR;
    lp_fw_ble_para_t *ble;

    if (lp == NULL || lp->ble_parameter == NULL) {
        return;
    }

    ble = lp->ble_parameter;
    ble->magic = 0U;
    ble->conn_ind_rx_desc_off = 0U;
    ble->sleep_duration = 0U;
    ble->ble_activity_state = LP_FW_BLE_ST_NONE;
    ble->lpfw_ble_awake = 0U;
}


int bl_lp_prepare_ble_parameter_before_sleep(void)
{
    iot2lp_para_t *lp = (iot2lp_para_t *)IOT2LP_PARA_ADDR;
    const btble_controller_lp_fw_info_t *info;
    lp_fw_ble_para_t *ble;

    if (lp == NULL || lp->ble_parameter == NULL) {
        tickless_debugf("[BLE_LP][para] ble_parameter is NULL");
        return -1;
    }

    ble = lp->ble_parameter;
    bl_lp_ble_parameter_invalidate();

    info = btble_controller_get_lp_fw_info();
    if (info == NULL) {
        tickless_debugf("[BLE_LP][para] controller info is NULL");
        return -2;
    }

    if (info->state != BTBLE_ST_ADV && info->state != BTBLE_ST_CONN) {
        tickless_debugf("[BLE_LP][para] unsupported controller state=%u", info->state);
        return -3;
    }

    if (!info->is_arbTarget) {
        tickless_debugf("[BLE_LP][para] controller deadline is not an arbiter target");
        return -4;
    }

    if (info->saved_blecore == NULL || info->saved_ipcore == NULL ||
        (((uintptr_t)info->saved_blecore | (uintptr_t)info->saved_ipcore) & 0x3U) != 0U ||
        info->saved_blecore_count < LP_FW_BLECORE_REG_SAVE_COUNT ||
        info->saved_ipcore_count < LP_FW_IPCORE_REG_SAVE_COUNT) {
        tickless_debugf("[BLE_LP][para] invalid snapshot ble=%p/%u ip=%p/%u", (const void *)info->saved_blecore,
                        info->saved_blecore_count, (const void *)info->saved_ipcore, info->saved_ipcore_count);
        return -5;
    }

    ble->rc_calibration = info->rc_calibration;
    ble->wakeup_app     = info->wakeup_app;
    if (info->state == BTBLE_ST_CONN) {
        const btble_controller_lp_fw_con_info_t *c = &info->con;
        ble->ble_activity_state = LP_FW_BLE_ST_CONN;
        ble->version            = LP_FW_BLE_PARA_VERSION;
        ble->cs_off             = c->cs_off;
        ble->rx_desc_off        = info->rx_desc_off;
        ble->rx_data_off        = info->rx_data_off;
        ble->rx_data_len        = info->rx_data_len;
        ble->saved_blecore_addr = (uint32_t)(uintptr_t)info->saved_blecore;
        ble->saved_ipcore_addr  = (uint32_t)(uintptr_t)info->saved_ipcore;
        ble->saved_blecore_count = info->saved_blecore_count;
        ble->saved_ipcore_count  = info->saved_ipcore_count;
        ble->dfe_mode     = (uint8_t)(((*(volatile uint32_t *)0x20004640u) >> 28) & 7u);
        ble->spdt_enabled = 0U;
        ble->spdt_gpio    = LP_FW_BLE_SPDT_GPIO_INVALID;
        ble->con_interval        = c->interval;
        ble->con_latency         = c->latency;
        ble->con_latency_applied = c->latency_applied;
        ble->con_evt_cnt         = c->evt_cnt;
        ble->con_evt_inc         = c->evt_inc;
        ble->con_master_sca      = c->master_sca;
        ble->con_local_drift     = c->local_drift;
        ble->con_last_cs_ch_idx  = c->last_cs_ch_idx;
        ble->con_prog_hus        = c->prog_hus;
        ble->con_prog_hs         = c->prog_hs;
        ble->con_next_ts         = c->next_ts;
        ble->con_next_bit_off    = c->next_bit_off;
        ble->con_sync_win_hus    = c->sync_win_size;
        ble->con_last_sync_ts    = c->last_sync_ts;
        ble->con_last_sync_bit_off = c->last_sync_bit_off;
        ble->con_last_crc_ok_ts  = c->last_crc_ok_ts;
        ble->con_timeout_hs      = c->timeout;
        ble->con_duration_min_hus = c->duration_min;
        ble->con_rx_rate         = c->rx_rate;
        ble->con_hop_sel_1       = c->hop_sel_1;
        ble->con_hop_inc         = c->hop_inc;
        ble->con_prio            = c->current_prio;
        ble->con_state           = BTBLE_LPFW_CON_STATE_WAIT_NEXT_PROG;
        ble->con_proged_in_lpfw   = 0U;
        ble->con_rsvd0           = 0U;
        ble->con_early_us        = 0U;
        ble->con_wake_cause      = 0U;
        ble->con_out_next_ts     = 0U;
        ble->con_out_rx_desc_off = 0U;
        ble->con_events_run      = 0U;
        ble->con_events_synced   = 0U;
        ble->con_last_rxstat     = 0U;
        ble->con_last_rxphce     = 0U;
        memset(ble->con_dbg, 0, sizeof(ble->con_dbg));
        memset(ble->con_elog, 0, sizeof(ble->con_elog));
        ble->con_sync_evt_cnt    = c->evt_cnt;   /* latency applied => the last event synced */
        ble->con_retry_max       = 4U;
        ble->con_retries         = 0U;
        ble->con_rsvd1           = 0U;
        ble->magic = LP_FW_BLE_PARA_MAGIC;
        return 0;
    }


    if (info->adv.adv_pdu_len < 6U) {
        tickless_debugf("[BLE_LP][para] invalid advertising PDU length=%u", info->adv.adv_pdu_len);
        return -6;
    }

    ble->ble_activity_state = LP_FW_BLE_ST_ADV;

    /* Only APP-owned input fields are reset. LPFW diagnostic output survives. */
    ble->version = LP_FW_BLE_PARA_VERSION;
    ble->cs_off = info->adv.cs_off;
    ble->tx0_off = info->adv.tx0_off;
    ble->tx1_off = info->adv.tx1_off;
    ble->adv_data_off = info->adv.adv_data_off;
    ble->adv_pdu_len = info->adv.adv_pdu_len;
    ble->adv_data_len = (uint8_t)(info->adv.adv_pdu_len - 6U);
    ble->rx_desc_off = info->rx_desc_off;
    ble->rx_data_off = info->rx_data_off;
    ble->rx_data_len = info->rx_data_len;
    ble->adv_interval_min = info->adv.adv_interval;

    // if (info->adv.next_hs > ble->adv_sched_next_hs) {
    ble->adv_sched_next_hs = info->adv.next_hs;
    ble->adv_sched_next_hus = info->adv.next_hus;
    // }
    ble->adv_sched_delta_hs = 0U;
    ble->saved_blecore_addr = (uint32_t)(uintptr_t)info->saved_blecore;
    ble->saved_ipcore_addr = (uint32_t)(uintptr_t)info->saved_ipcore;
    ble->saved_blecore_count = info->saved_blecore_count;
    ble->saved_ipcore_count = info->saved_ipcore_count;
    ble->consecutive_no_adv = 0U;

    ble->dfe_mode = ((*(volatile uint32_t *)0x20004640) >> 28) & 7;
    ble->spdt_enabled = 0;
    ble->spdt_gpio = 0xFF;

    ble->magic = LP_FW_BLE_PARA_MAGIC;

    tickless_debugf(
        "[BLE_LP][para] cs=0x%04x tx0=0x%04x tx1=0x%04x adv=0x%04x/%u intv=%u rx=0x%04x/0x%04x/%u ble=0x%08lx/%u ip=0x%08lx/%u",
        ble->cs_off, ble->tx0_off, ble->tx1_off, ble->adv_data_off, ble->adv_pdu_len, ble->adv_interval_min,
        ble->rx_desc_off, ble->rx_data_off, ble->rx_data_len, (unsigned long)ble->saved_blecore_addr,
        ble->saved_blecore_count, (unsigned long)ble->saved_ipcore_addr, ble->saved_ipcore_count);

    return 0;
}

static void ble_lp_controller_sleep_abort(bool *controller_sleeping)
{
    if (*controller_sleeping) {
        btble_controller_sleep_restore();
        *controller_sleeping = false;
    }
}

#endif /* CFG_BLE_ENABLE && BL618DG */

/* Define the function that is called by portSUPPRESS_TICKS_AND_SLEEP(). */
void vApplicationSleep(TickType_t xExpectedIdleTime)
{
    eSleepModeStatus eSleepStatus;
    int32_t real_rtc_tick = 0;
    int connected, wake_reason;
    bool wifi_connected;
    bool is_ready = true;
#if defined(CFG_BLE_ENABLE) && defined(BL618DG)
    extern uint32_t rw_main_task_hdl;
    uint64_t ble_sleep_pre_rtc_cnt = 0U;
    int32_t ble_sleep_pds_timer = 0;
    bool ble_controller_sleeping = false;
#endif
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
        portENABLE_INTERRUPTS();
        return;
    }

#if defined(CFG_BLE_ENABLE) && defined(BL618DG)
extern void btblecontroller_rc32k_xtal_count_trigger(void);
extern uint16_t btblecontroller_rc32k_xtal_count_wait_result(void);
    lpfw_cfg.ble_wakeup_en = 0U;
    lpfw_cfg.ble_pds_sleep_us = 0U;
    if (likely(rw_main_task_hdl != 0)) {
        uint16_t b_rc32k_trim_result;
        btblecontroller_rc32k_xtal_count_trigger();
        b_rc32k_trim_result = btblecontroller_rc32k_xtal_count_wait_result();
        b_rc32k_trim_result = b_rc32k_trim_result;
        tickless_debugf("[RC32K_K][test] wait_result=%u\r\n", b_rc32k_trim_result);

        HBN_Get_RTC_Timer_Val((uint32_t *)&ble_sleep_pre_rtc_cnt, (uint32_t *)&ble_sleep_pre_rtc_cnt + 1);
        ble_sleep_pds_timer = btble_controller_sleep(0);
        if (ble_sleep_pds_timer < 0) {
            tickless_abort_wait_for_interrupt();
            return;
        }
        ble_controller_sleeping = true;

        if (ble_sleep_pds_timer == 0) {
            bl_lp_ble_parameter_invalidate();
            tickless_debugf("BLE has no wakeup deadline");
        } else {
            uint64_t ble_sleep_elapsed_rtc_cnt;
            int ble_para_ret = bl_lp_prepare_ble_parameter_before_sleep();
            if (ble_para_ret != 0) {
                tickless_debugf("[TIME] ble_parameter prepare ret=%d, abort this sleep", ble_para_ret);
                ble_lp_controller_sleep_abort(&ble_controller_sleeping);
                tickless_abort_wait_for_interrupt();
                return;
            }

            ble_sleep_elapsed_rtc_cnt = ble_sleep_pds_timer + (uint64_t)ble_sleep_pre_rtc_cnt;
            lpfw_cfg.ble_pds_sleep_us = BL_PDS_CNT_TO_US(ble_sleep_pre_rtc_cnt);
            bl_lp_sched_publish(2,1000, 10000, BL_PDS_CNT_TO_US(ble_sleep_elapsed_rtc_cnt));
            lpfw_cfg.ble_wakeup_en = 1;
            tickless_debugf("ble sleep duration: %d", lpfw_cfg.ble_pds_sleep_us);
        }
    }
#endif

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
#if defined(CFG_BLE_ENABLE) && defined(BL618DG)
        ble_lp_controller_sleep_abort(&ble_controller_sleeping);
#endif
        tickless_abort_wait_for_interrupt();
        return;
    }

    /* Recheck after the early screening in case ringbuffer data arrives during the setup window. */
    if (!tickless_ringbuffer_is_empty()) {
#if defined(CFG_BLE_ENABLE) && defined(BL618DG)
        ble_lp_controller_sleep_abort(&ble_controller_sleeping);
#endif
        tickless_abort_wait_for_interrupt();
        return;
    }

    if (hooks->prepare_enter != NULL && !hooks->prepare_enter(hooks->prepare_enter_arg)) {
        tickless_debugf("Sleep Abort! prepare_enter");
#if defined(CFG_BLE_ENABLE) && defined(BL618DG)
        ble_lp_controller_sleep_abort(&ble_controller_sleeping);
#endif
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

#if defined(CFG_BLE_ENABLE) && defined(BL618DG)
    ble_lp_controller_sleep_abort(&ble_controller_sleeping);
    lp_fw_ble_para_t *ble = iot2lp_para->ble_parameter;

    if (ble->ble_activity_state == LP_FW_BLE_ST_ADV || ble->ble_activity_state == LP_FW_BLE_ST_RX_CONN_IND) {
        btble_controller_lp_fw_activity_t restore = {
            .state = ble->ble_activity_state,
            .next_hs = ble->adv_sched_next_hs,
            .next_hus = ble->adv_sched_next_hus,
            .conn_ind_rx_desc_addr = ble->conn_ind_rx_desc_off,
            .sleep_duration = ble->sleep_duration,
            .lpfw_ble_awake = !!ble->lpfw_ble_awake,
        };
        int ret = btble_controller_lp_fw_activity_restore(&restore);
        if (ret != 0) {
            tickless_debugf("[BLE_LP][restore] activity state=%u ret=%d", restore.state, ret);
        }
    }else if (ble->ble_activity_state == LP_FW_BLE_ST_CONN) {
            btble_controller_lp_fw_activity_t restore = {
            .state                 = BTBLE_ST_CONN,
            .con_state             = ble->con_state,
            .next_hs               = 0U,
            .next_hus              = 0U,
            .conn_ind_rx_desc_addr = 0U,
            .con_proged_in_lpfw     = ble->con_proged_in_lpfw,
            .sleep_duration        = ble->sleep_duration,
            .lpfw_ble_awake        = ble->lpfw_ble_awake != 0U,
            .con_wake_cause        = ble->con_wake_cause,
            .con_evt_cnt           = ble->con_evt_cnt,
            .con_evt_inc           = ble->con_evt_inc,
            .con_next_ts           = ble->con_out_next_ts,
            .con_next_bit_off      = ble->con_out_next_bit_off,
            .con_prog_hs           = ble->con_prog_hs,
            .con_prog_hus          = ble->con_prog_hus,
            .con_last_sync_ts      = ble->con_last_sync_ts,
            .con_last_sync_bit_off = ble->con_last_sync_bit_off,
            .con_last_crc_ok_ts    = ble->con_last_crc_ok_ts,
            .con_last_cs_ch_idx    = ble->con_last_cs_ch_idx,
            .con_rx_desc_addr      = ble->rx_desc_off,
        };
        int ret = btble_controller_lp_fw_activity_restore(&restore);
        if (ret != 0) {
            tickless_debugf("[BLE_LP][restore] activity state=%u ret=%d", restore.state, ret);
        }
    }
#endif

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
