#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <lwip/tcpip.h>

#include <bl_lp.h>
#include <bflb_clock.h>
#include <bflb_gpio.h>
#include <bflb_irq.h>
#include <bflb_uart.h>
#include <board.h>
#include <board_rf.h>
#include <clock_manager.h>
#include <pm_manager.h>
#include <pm_helper_cli.h>
#include <shell.h>
#include <tickless.h>
#include <wifi_mgmr.h>
#include <wifi_mgmr_ext.h>
#include <nethub.h>

#ifdef BL616
#include <bl616_glb.h>
#include <bl616_hbn.h>
#include <bl616_pm.h>
#elif defined(BL616CL)
#include <bl616cl_aon.h>
#include <bl616cl_glb.h>
#include <bl616cl_hbn.h>
#include <bl616cl_pm.h>
#elif defined(BL618DG)
#include <bl618dg_hbn.h>
#include <bl618dg_pm.h>
#endif

#define APP_PM_IDLE_TASK_STACK_SIZE (1024)
#define SLEEP_PDS_UA                (60)
#define ACTIVE_LPFW_UA              (35000)
#define ACTIVE_APP_UA               (55000)

static TimerHandle_t keepalive_timer;
static uint32_t lp_timeouts_ms;
#if defined(BL616)
static bl_lp_hbn_fw_cfg_t hbn_test_cfg = {
    .hbn_sleep_cnt = 0,
    .hbn_level = 0,
};
#endif

extern void uart_shell_isr(void);
extern struct bflb_device_s *uart_shell;
extern void vPortSetupTimerInterrupt(void);
extern int enable_multicast_broadcast;
extern bl_lp_fw_cfg_t lpfw_cfg;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[APP_PM_IDLE_TASK_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = APP_PM_IDLE_TASK_STACK_SIZE;
}

static int lp_enter(void *arg)
{
    (void)arg;
#if defined(CONFIG_MR_SDIO_DRIVER) && defined(CONFIG_NETHUB_PROFILE_SDIO)
#endif
    return 0;
}

static int lp_exit(void *arg)
{
    int wakeup_reason;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    extern TaskHandle_t rxl_process_task_hd;
    (void)arg;

    /* recovery system_clock_init\peripheral_clock_init\console_init*/
    board_recovery();

#if defined(BL616) || defined(BL616CL)
    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
#endif

    board_rf_ctl(BRD_CTL_RF_RESET_DEFAULT, 0);

    vPortSetupTimerInterrupt();

    bflb_uart_rxint_mask(uart_shell, false);
    bflb_irq_attach(uart_shell->irq_num, (irq_callback)uart_shell_isr, NULL);
    bflb_irq_enable(uart_shell->irq_num);

#if defined(CONFIG_MR_SDIO_DRIVER) && defined(CONFIG_NETHUB_PROFILE_SDIO)
#endif

    wakeup_reason = bl_lp_get_wake_reason();
    if (wakeup_reason & LPFW_WAKEUP_WIFI_BROADCAST) {
        vTaskNotifyGiveFromISR(rxl_process_task_hd, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        pm_alloc_mem_reset();
    }

    return 0;
}

int pwr_info_clear(void)
{
#ifdef BL616
    bl_lp_info_clear();
#else
    bl_lp_info_clear(iot2lp_para->lp_info);
#endif

    return 0;
}

uint64_t pwr_info_get(void)
{
    bl_lp_info_t lp_info;
    uint64_t current = 0;

    bl_lp_info_get(&lp_info);

    printf("\r\nVirtual time: %llu us\r\n", bl_lp_get_virtual_us());
    printf("Power info dump:\r\n");
    printf("LPFW try recv bcn: %d, loss %d\r\n", lp_info.lpfw_recv_cnt, lp_info.lpfw_loss_cnt);
    printf("Total time %lldms\r\n", lp_info.time_total_us / 1000);
    printf("PDS sleep: %lldms\r\n", lp_info.sleep_pds_us / 1000);
    printf("LPFW active: %lldms\r\n", lp_info.active_lpfw_us / 1000);
    printf("APP active: %lldms\r\n", lp_info.active_app_us / 1000);

    if (lp_info.time_total_us != 0) {
        current = (lp_info.sleep_pds_us * SLEEP_PDS_UA + lp_info.active_lpfw_us * ACTIVE_LPFW_UA +
                   lp_info.active_app_us * ACTIVE_APP_UA) /
                  lp_info.time_total_us;
    }

    printf("Predict current: %llduA\r\n", current);

    return current;
}

static void app_pm_timeout_callback(TimerHandle_t xTimer)
{
    (void)xTimer;

    pwr_info_get();
    pm_disable_tickless();
    xTimerDelete(xTimer, portMAX_DELAY);
}

static void app_pm_start_timeout_timer(uint32_t timeout_ms)
{
    char timer_name[32];
    TimerHandle_t timer;

    snprintf(timer_name, sizeof(timer_name), "PwrTimer_%u", (unsigned int)timeout_ms);
    timer = xTimerCreate(timer_name, pdMS_TO_TICKS(timeout_ms), pdTRUE, NULL, app_pm_timeout_callback);
    if (timer == NULL) {
        printf("Failed to create timer.\n");
        return;
    }

    if (xTimerStart(timer, 0) != pdPASS) {
        printf("Failed to start timer.\n");
    }
}

int app_lp_timer_config(int mode, uint32_t ms)
{
    if (mode == 0 || mode == 2) {
        lp_timeouts_ms = ms;
    }
#if defined(BL616)
    else if (mode == 1) {
        hbn_test_cfg.hbn_sleep_cnt = (32768 * ms) / 1000;
    }
#endif
    else {
        printf("config lp level fail:%d\r\n", mode);
        return -1;
    }

    return 0;
}

void app_pm_enter_hbn(int level)
{
#if defined(BL616)
    bl_lp_hbn_init(0, 0, 0, 0);

    if (level > 0 && level < 3) {
        hbn_test_cfg.hbn_level = level;
    }

    bl_lp_hbn_enter(&hbn_test_cfg);
#else
    (void)level;
    printf("HBN entry is not implemented on this chip yet.\r\n");
#endif
}

static int app_pm_enter_pds15_impl(void)
{
    if (lp_timeouts_ms != 0) {
        app_pm_start_timeout_timer(lp_timeouts_ms);
        lp_timeouts_ms = 0;
    }

    return pm_enable_tickless();
}

void app_pm_enter_pds15(void)
{
    (void)app_pm_enter_pds15_impl();
}

void app_pm_exit_pds15(void)
{
    pm_disable_tickless();
    wifi_mgmr_sta_ps_exit();
}

static void keepalive_callback(TimerHandle_t xTimer)
{
    (void)xTimer;
    wifi_mgmr_null_data_send();
}

int app_create_keepalive_timer(uint32_t period_seconds)
{
    TickType_t timer_period = pdMS_TO_TICKS(period_seconds * 1000);

    if (keepalive_timer == NULL) {
        keepalive_timer = xTimerCreate("keepalive_timer", timer_period, pdTRUE, NULL, keepalive_callback);
        if (keepalive_timer == NULL) {
            printf("Failed to create timer.\n");
            return -1;
        }

        if (xTimerStart(keepalive_timer, 0) != pdPASS) {
            printf("Failed to start timer.\n");
            return -1;
        }

        return 0;
    }

    if (xTimerChangePeriod(keepalive_timer, timer_period, 0) != pdPASS) {
        printf("Failed to change timer period.\n");
        return -1;
    }

    return 0;
}

int app_delete_keepalive_timer(void)
{
    if (keepalive_timer == NULL) {
        printf("No timer to delete.\n");
        return 0;
    }

    if (xTimerStop(keepalive_timer, 0) != pdPASS) {
        printf("Failed to stop timer.\n");
        return -1;
    }

    if (xTimerDelete(keepalive_timer, 0) != pdPASS) {
        printf("Failed to delete timer.\n");
        return -1;
    }

    keepalive_timer = NULL;
    printf("Timer successfully deleted.\n");
    return 0;
}

int app_pm_twt_param_set(int s, int t, int e, int n, int m)
{
    twt_setup_params_struct_t param = {
        .setup_type = s,
        .flow_type = t,
        .wake_int_exp = e,
        .wake_dur_unit = 0,
        .min_twt_wake_dur = n,
        .wake_int_mantissa = m,
    };

    return wifi_mgmr_sta_twt_setup(&param);
}

/**********************************************************
    io wakeup configuration
 ***********************************************************/
static void test_wakeup_io_callback(uint64_t wake_up_io_bits)
{
    printf("io wakeup callback, bits: 0x%llX\r\n", wake_up_io_bits);
    for (uint8_t i = 0; i < BL_LP_WAKEUP_IO_MAX_NUM; i++) {
        if (wake_up_io_bits & ((uint64_t)1 << i)) {
            int mode = bl_lp_wakeup_io_get_mode(i);
            const char *mode_str = "unknown";
            switch (mode) {
                case BL_LP_IO_WAKEUP_MODE_FALLING:
                    mode_str = "falling edge";
                    break;
                case BL_LP_IO_WAKEUP_MODE_RISING:
                    mode_str = "rising edge";
                    break;
                case BL_LP_IO_WAKEUP_MODE_HIGH:
                    mode_str = "high level";
                    break;
                case BL_LP_IO_WAKEUP_MODE_LOW:
                    mode_str = "low level";
                    break;
                default:
                    break;
            }
            printf("  gpio%d wakeup, mode: %s\r\n", i, mode_str);
        }
    }
    tickless_exit();
}

#if defined(BL616)
static bl_lp_io_cfg_t lp_wake_io_cfg;
#define IO_WAKEUP_MAX 35
#elif defined(BL616CL)
static lp_gpio_cfg_type lp_wake_io_cfg;
#define IO_WAKEUP_MAX 37
#elif defined(BL618DG)
static lp_gpio_cfg_type lp_wake_io_cfg;
#define IO_WAKEUP_MAX 52
#endif

static void cmd_io_wakeup(int argc, char **argv)
{
    uint32_t trig_mode = 0;
    uint32_t io_num = 0;
    uint32_t pull = 0;

    if (argc < 3 || argc > 4) {
        printf("Usage: io_wakeup <io_num> <trigger_mode> [pull]\r\n");
        printf("  io_num:       GPIO number (0-%d)\r\n", IO_WAKEUP_MAX - 1);
        printf("  trigger_mode:\r\n");
        printf("    0  - sync falling edge\r\n");
        printf("    1  - sync rising edge\r\n");
        printf("    2  - sync low level\r\n");
        printf("    3  - sync high level\r\n");
        printf("    4  - sync rising & falling edge\r\n");
        printf("    8  - async falling edge\r\n");
        printf("    9  - async rising edge\r\n");
        printf("    0xA- async low level\r\n");
        printf("    0xB- async high level\r\n");
        printf("  pull (optional):\r\n");
        printf("    0  - none (default)\r\n");
        printf("    1  - pull up\r\n");
        printf("    2  - pull down\r\n");
        printf("\r\nExample: io_wakeup 10 0 2\r\n");
        printf("  -> Enable GPIO10 with falling edge trigger and pull-down\r\n");
        return;
    }

    io_num = strtoul(argv[1], NULL, 0);
    trig_mode = strtoul(argv[2], NULL, 0);
    if (argc == 4) {
        pull = strtoul(argv[3], NULL, 0);
    }

    if (io_num >= IO_WAKEUP_MAX) {
        printf("[ERR] io_num must be 0-%d\r\n", IO_WAKEUP_MAX - 1);
        return;
    }
    if (pull >= 3) {
        printf("[ERR] pull must be 0, 1, or 2\r\n");
        return;
    }

    memset((void *)&lp_wake_io_cfg, 0x00, sizeof(lp_wake_io_cfg));

    printf("io_wakeup config:\r\n");
    printf("  io_num: %d\r\n", io_num);

#if defined(BL616)
    /* BL616: bl_lp_io_cfg_t with ie/res group fields */
    lp_wake_io_cfg.io_wakeup_unmask = (uint64_t)1 << io_num;

    if (io_num <= 15) {
        lp_wake_io_cfg.io_0_15_ie = BL_LP_IO_INPUT_ENABLE;
        if (io_num <= 7) {
            lp_wake_io_cfg.io_0_7_pds_trig_mode = trig_mode;
        } else {
            lp_wake_io_cfg.io_8_15_pds_trig_mode = trig_mode;
        }
    } else if (io_num <= 19) {
        /* AON GPIO */
        lp_wake_io_cfg.io_16_ie = BL_LP_IO_INPUT_ENABLE;
        lp_wake_io_cfg.io_16_19_aon_trig_mode = trig_mode;
    } else {
        lp_wake_io_cfg.io_20_34_ie = BL_LP_IO_INPUT_ENABLE;
        if (io_num <= 27) {
            lp_wake_io_cfg.io_20_27_pds_trig_mode = trig_mode;
        } else {
            lp_wake_io_cfg.io_28_34_pds_trig_mode = trig_mode;
        }
    }

    if (1 == pull) {
        printf("  pull: up\r\n");
        if (io_num <= 15) {
            lp_wake_io_cfg.io_0_15_res = BL_LP_IO_RES_PULL_UP;
        } else if (io_num <= 19) {
            lp_wake_io_cfg.io_16_res = BL_LP_IO_RES_PULL_UP;
        } else {
            lp_wake_io_cfg.io_20_34_res = BL_LP_IO_RES_PULL_UP;
        }
    } else if (2 == pull) {
        printf("  pull: down\r\n");
        if (io_num <= 15) {
            lp_wake_io_cfg.io_0_15_res = BL_LP_IO_RES_PULL_DOWN;
        } else if (io_num <= 19) {
            lp_wake_io_cfg.io_16_res = BL_LP_IO_RES_PULL_DOWN;
        } else {
            lp_wake_io_cfg.io_20_34_res = BL_LP_IO_RES_PULL_DOWN;
        }
    } else {
        printf("  pull: none\r\n");
    }

    bl_lp_io_wakeup_cfg((void *)&lp_wake_io_cfg);
#elif defined(BL616CL)
    /* BL616CL: lp_gpio_cfg_type with per-GPIO trig_mode */
    lp_wake_io_cfg.io_ie = (uint64_t)1 << io_num;
    lp_wake_io_cfg.io_wakeup_unmask = (uint64_t)1 << io_num;

    lp_wake_io_cfg.io_0_36_trig_mode[io_num] = trig_mode;

    if (1 == pull) {
        printf("  pull: up\r\n");
        lp_wake_io_cfg.io_pu = (uint64_t)1 << io_num;
    } else if (2 == pull) {
        printf("  pull: down\r\n");
        lp_wake_io_cfg.io_pd = (uint64_t)1 << io_num;
    } else {
        printf("  pull: none\r\n");
    }

    bl_lp_io_wakeup_cfg((void *)&lp_wake_io_cfg);
#elif defined(BL618DG)
    /* BL618DG: lp_gpio_cfg_type with 8-GPIO-group trig_mode */
    lp_wake_io_cfg.io_ie = (uint64_t)1 << io_num;
    lp_wake_io_cfg.io_wakeup_unmask = (uint64_t)1 << io_num;

    if (io_num <= 7) {
        lp_wake_io_cfg.io_0_7_trig_mode = trig_mode;
    } else if (io_num <= 15) {
        lp_wake_io_cfg.io_8_15_trig_mode = trig_mode;
    } else if (io_num <= 23) {
        lp_wake_io_cfg.io_16_23_trig_mode = trig_mode;
    } else if (io_num <= 31) {
        lp_wake_io_cfg.io_24_31_trig_mode = trig_mode;
    } else if (io_num <= 39) {
        lp_wake_io_cfg.io_32_39_trig_mode = trig_mode;
    } else if (io_num <= 47) {
        lp_wake_io_cfg.io_40_47_trig_mode = trig_mode;
    } else {
        lp_wake_io_cfg.io_48_52_trig_mode = trig_mode;
    }

    if (1 == pull) {
        printf("  pull: up\r\n");
        lp_wake_io_cfg.io_pu = (uint64_t)1 << io_num;
    } else if (2 == pull) {
        printf("  pull: down\r\n");
        lp_wake_io_cfg.io_pd = (uint64_t)1 << io_num;
    } else {
        printf("  pull: none\r\n");
    }

    bl_lp_io_wakeup_cfg((void *)&lp_wake_io_cfg);
#endif

    bl_lp_wakeup_io_int_register(test_wakeup_io_callback);
    printf("io wakeup configured, will take effect on next low power entry\r\n");
}

int lp_set_wakeup_by_io(uint8_t io, uint8_t mode)
{
#if defined(BL616)
    static bl_lp_io_cfg_t bl616_wake_io_cfg = {
        .io_20_34_ie = BL_LP_IO_INPUT_ENABLE,
    };

    if (io < 28 || io > 29) {
        printf("only support gpio 28, 29 now.\r\n");
        return -1;
    }

    if (mode > 1) {
        printf("not support mode:%d\r\n", mode);
        return -1;
    }

    if (mode == 0) {
        bl616_wake_io_cfg.io_28_34_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL;
    } else {
        bl616_wake_io_cfg.io_28_34_pds_trig_mode = BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE;
    }

    bl616_wake_io_cfg.io_20_34_res = BL_LP_IO_RES_PULL_DOWN;
    bl616_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << io);

    bl_lp_io_wakeup_cfg(&bl616_wake_io_cfg);
    return 0;
#else
    (void)io;
    (void)mode;
    printf("wake-up GPIO configuration is not implemented on this chip yet, please use io_wakeup CLI.\r\n");
    return -1;
#endif
}

int lp_delete_wakeup_by_io(uint8_t io)
{
#if defined(BL616)
    static bl_lp_io_cfg_t bl616_wake_io_cfg = {
        .io_20_34_ie = BL_LP_IO_INPUT_ENABLE,
    };

    if (io < 28 || io > 29) {
        printf("only support gpio 28, 29 now.\r\n");
        return -1;
    }

    bl616_wake_io_cfg.io_wakeup_unmask &= ~((uint64_t)1 << io);
    bl_lp_io_wakeup_cfg(&bl616_wake_io_cfg);

    return 0;
#else
    (void)io;
    printf("wake-up GPIO configuration is not implemented on this chip yet, please use io_wakeup CLI.\r\n");
    return -1;
#endif
}

static void f32k_clk_init_task(void *pvParameters)
{
    (void)pvParameters;

    if (app_clock_init() != 0) {
        printf("F32K clock initialization failed!\r\n");
    } else {
        printf("F32K clock initialization success!\r\n");
    }

    vTaskDelete(NULL);
}

static int nethub_wakeup_timer_start(uint32_t timeout_ms, int broadcast, void *arg)
{
    (void)broadcast;
    (void)arg;

    if (app_lp_timer_config(0, timeout_ms) != 0) {
        return -1;
    }
    if (app_pm_enter_pds15_impl() != 0) {
        return -1;
    }
    pwr_info_clear();

    return 0;
}

static int nethub_tickless_start(void *arg)
{
    (void)arg;
    return app_pm_enter_pds15_impl();
}

int app_pm_create_arp_announce_timer(uint32_t seconds)
{
    return pm_helper_cli_arp_timer_start(seconds);
}

int app_pm_delete_arp_announce_timer(void)
{
    return pm_helper_cli_arp_timer_stop();
}

int app_pm_init(void)
{
    static const pm_helper_cli_cfg_t helper_cfg = {
        .wakeup_timer_cb = nethub_wakeup_timer_start,
        .tickless_cb = nethub_tickless_start,
        .arg = NULL,
        .arp_target = PM_HELPER_CLI_ARP_LOCAL_IP,
        .arp_period_seconds = 55,
        .arp_send_immediately = true,
        .arp_periodic = true,
    };

#if defined(BL616CL)
    uint32_t tmpVal;

    AON_Trim_Ldo18_IO_Vout();
    HBN_Trim_Ldo_Sys_Vout();
    HBN_Trim_Ldo_Aon_Vout();
    HBN_Trim_Ldo_Soc_Vout();

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_SW_LDO08AON_VOUT_TRIM_AON);
    printf("ldo08aon value:%d\r\n", tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_LDO_SOC_VOUT_SEL_AON);
    printf("ldo08soc value:%d\r\n", tmpVal);

    HBN_SW_Set_Ldo_Soc_Vout(HBN_LDO_SOC_LEVEL_0P900V);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_LDO_SOC_VOUT_TRIM_AON);
    printf("ldo08soc value:%d\r\n", tmpVal);
#endif

    HBN_Enable_RTC_Counter();
    pm_sys_init();
    bl_lp_init();
    bl_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);

    if (pm_helper_cli_init(&helper_cfg) != 0) {
        printf("pm helper cli init failed.\r\n");
        return -1;
    }

    printf("[OS] Create f32k_clk_init task...\r\n");
    xTaskCreate(f32k_clk_init_task, "f32k_clk_init", 1024, NULL, 12, NULL);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_io_wakeup, io_wakeup, configure io wakeup source);

#if defined(CONFIG_NETHUB_LOWPOWER_ENABLE)
static void __enter_pds15_witharg(uint8_t dtm, int broadcast)
{
    lpfw_cfg.dtim_origin = dtm;
    if (broadcast != 0) {
        enable_multicast_broadcast = 1;
        lpfw_cfg.bcmc_dtim_mode = 1;
    } else {
        enable_multicast_broadcast = 0;
        lpfw_cfg.bcmc_dtim_mode = 0;
    }

    printf("dtim_origin: %d, broadcast: %d\r\n", lpfw_cfg.dtim_origin, broadcast);

    pm_enable_tickless();
}
static void lp_test_window_entry(void *pvParameters)
{
    if (!nethub_host_link_is_idle()) {
        printf("nethub host link is busy, skip low power window.\r\n");
        vTaskDelete(NULL);
        return;
    }

    if (nethub_host_link_lowpower_prepare() != NETHUB_OK) {
        printf("nethub host low power prepare failed.\r\n");
        vTaskDelete(NULL);
        return;
    }

    __enter_pds15_witharg(10, 1);

    printf("=================== pm_disable_tickless 1\r\n");
    vTaskDelay(pdMS_TO_TICKS(10000));// 10 S

    printf("=================== pm_disable_tickless 2\r\n");
    pm_disable_tickless();
    if (nethub_host_link_lowpower_resume() != NETHUB_OK) {
        printf("nethub host low power resume failed.\r\n");
    }

    vTaskDelete(NULL);
}

static void cmd_tickless_window(int argc, char **argv)
{
    BaseType_t ret;

    ret = xTaskCreate(lp_test_window_entry, "lp_test_window", 2048, NULL,
                      15, NULL);
    if (ret != pdPASS) {
        printf("Failed to create low power test window task.\r\n");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_tickless_window, tickless_window, timed tickless test window);
#endif
