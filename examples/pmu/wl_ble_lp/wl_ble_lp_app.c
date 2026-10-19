#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "mm.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "bl_lp.h"
#if defined(BL618DG)
#include "bl618dg_pm.h"
#include "bl618dg_glb.h"
#include "bl618dg_hbn.h"
#else
#include "bl616_pm.h"
#include "bl616_glb.h"
#include "bl616_glb_gpio.h"
#include "bl616_hbn.h"
#endif
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_rtc.h"
#include "shell.h"
#include "pm_manager.h"
#include "pm_helper_cli.h"

int pwr_info_clear(void)
{
#if defined(BL618DG)
    if (iot2lp_para->lp_info != NULL) {
        bl_lp_info_clear(iot2lp_para->lp_info);
    }
#else
    bl_lp_info_clear();
#endif

    return 0;
}

#define SLEEP_PDS_US   80
#define ACTIVE_LPFW_US 38000
#define ACTIVE_APP_US  57000

uint64_t pwr_info_get(void)
{
    bl_lp_info_t lp_info;
    bl_lp_info_get(&lp_info);

    printf("\r\nVirtual time: %llu us\r\n", bl_lp_get_virtual_us());
    printf("Power info dump:\r\n");
    printf("LPFW try recv bcn: %d, loss %d\r\n", lp_info.lpfw_recv_cnt, lp_info.lpfw_loss_cnt);
    printf("LPFW try ble adv: %u, loss %u\r\n", lp_info.ble_lpfw_adv_success_cnt, lp_info.ble_lpfw_adv_loss_cnt);
    printf("Total time %lldms\r\n", lp_info.time_total_us / 1000);
    printf("PDS sleep: %lldms\r\n", lp_info.sleep_pds_us / 1000);
    printf("LPFW active: %lldms\r\n", lp_info.active_lpfw_us / 1000);
    printf("APP active: %lldms\r\n", lp_info.active_app_us / 1000);

    uint64_t current = (lp_info.sleep_pds_us * SLEEP_PDS_US + lp_info.active_lpfw_us * ACTIVE_LPFW_US +
                        lp_info.active_app_us * ACTIVE_APP_US) /
                       lp_info.time_total_us;

    printf("Predict current: %llduA\r\n", current);

    return current;
}

static TimerHandle_t wakeup_timer;

static void timerCallback(TimerHandle_t timer)
{
    (void)timer;

    pm_disable_tickless();
    pwr_info_get();
}

static int wl_ble_wakeup_timer_start(uint32_t timeout_ms, int broadcast, void *arg)
{
    TickType_t timer_period = pdMS_TO_TICKS(timeout_ms);

    (void)arg;
    if (timer_period == 0U) {
        printf("Wakeup timeout is shorter than one OS tick.\r\n");
        return -1;
    }

    if (wakeup_timer == NULL) {
        wakeup_timer = xTimerCreate("PwrTimer", timer_period, pdFALSE, NULL, timerCallback);
        if (wakeup_timer == NULL) {
            printf("Failed to create wakeup timer.\r\n");
            return -1;
        }
    }

    /* ChangePeriod starts or restarts this persistent one-shot timer. */
    if (xTimerChangePeriod(wakeup_timer, timer_period, portMAX_DELAY) != pdPASS) {
        printf("Failed to start wakeup timer.\r\n");
        return -1;
    }

    enable_multicast_broadcast = broadcast;
    lpfw_cfg.bcmc_dtim_mode = (uint8_t)broadcast;
    if (pm_enable_tickless() != 0) {
        (void)xTimerStop(wakeup_timer, 0);
        return -1;
    }
    pwr_info_clear();
    return 0;
}

int wl_ble_cmd_init(void)
{
    static const pm_helper_cli_cfg_t cfg = {
        .wakeup_timer_cb = wl_ble_wakeup_timer_start,
        .arp_target = PM_HELPER_CLI_ARP_GATEWAY,
        .arp_period_seconds = 55U,
    };

    return pm_helper_cli_init(&cfg);
}
