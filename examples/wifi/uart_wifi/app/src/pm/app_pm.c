#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <lwip/etharp.h>
#include <lwip/netif.h>
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
#include <shell.h>
#include <tickless.h>
#include <wifi_mgmr.h>
#include <wifi_mgmr_ext.h>

#ifdef BL616
#include <bl616_glb.h>
#include <bl616_hbn.h>
#include <bl616_pm.h>
#elif defined(BL616CL)
#include <bl616cl_aon.h>
#include <bl616cl_hbn.h>
#include <bl616cl_pds.h>
#include <bl616cl_pm.h>
#elif defined(BL618DG)
#include <bl618dg_hbn.h>
#include <bl618dg_pm.h>
#endif

#define APP_PM_IDLE_TASK_STACK_SIZE (1024)
#define SLEEP_PDS_UA                (60)
#define ACTIVE_LPFW_UA              (35000)
#define ACTIVE_APP_UA               (55000)

static TimerHandle_t xArpTimer;
static TimerHandle_t keepalive_timer;
static uint32_t lp_timeouts_ms;
#if defined(BL616CL) || defined(BL618DG)
static struct bflb_device_s *rtc;
#endif
#if defined(BL616)
static bl_lp_hbn_fw_cfg_t hbn_test_cfg = {
    .hbn_sleep_cnt = 0,
    .hbn_level = 0,
};
#elif defined(BL616CL)
static uint64_t hbn_sleep_cnt;
#endif

#if defined(BL616)
static bl_lp_io_cfg_t lp_wake_io_cfg;
#define IO_WAKEUP_MAX 35
#elif defined(BL616CL)
static lp_gpio_cfg_type lp_wake_io_cfg;
static uint8_t lp_wake_io_configured;
#define IO_WAKEUP_MAX 37
#elif defined(BL618DG)
static lp_gpio_cfg_type lp_wake_io_cfg;
#define IO_WAKEUP_MAX 52
#endif

extern void uart_shell_isr(void);
extern struct bflb_device_s *uart_shell;
extern void vPortSetupTimerInterrupt(void);
extern int netbus_uart_recovery(void);
extern void board_uartx_gpio_init(void);

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
    return 0;
}

static void set_cpu_bclk_80m_and_gate_clk(void)
{
#if defined(BL616)
    uint32_t tmpVal = 0;

    GLB_Set_MCU_System_CLK_Div(0, 3);
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_CLK,
                       Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK) / 1000000 - 1);

    tmpVal = 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_CPU, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_DMA, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_SEC, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_SDU, 1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG0, tmpVal);

    tmpVal = 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EF_CTRL, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_SF_CTRL, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_DMA, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_UART0, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_UART1, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_SEC_ENG, 1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

    tmpVal = 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S2_WIFI, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_EMI_MISC, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_PIO, 1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpVal);
#endif
}

static int lp_exit(void *arg)
{
    int wakeup_reason;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    extern TaskHandle_t rxl_process_task_hd;

    (void)arg;
    set_cpu_bclk_80m_and_gate_clk();

    board_recovery();

#if defined(BL616)
    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
#endif

    board_rf_ctl(BRD_CTL_RF_RESET_DEFAULT, 0);

    vPortSetupTimerInterrupt();

    if (uart_shell != NULL) {
        bflb_uart_feature_control(uart_shell, UART_CMD_CLR_RX_FIFO, 0);
        bflb_uart_rxint_mask(uart_shell, false);
        bflb_irq_attach(uart_shell->irq_num, (irq_callback)uart_shell_isr, NULL);
        bflb_irq_enable(uart_shell->irq_num);
    }

    pm_disable_gpio_keep(GPIO_PIN_6);
    pm_disable_gpio_keep(GPIO_PIN_8);
    board_uartx_gpio_init();
    if (netbus_uart_recovery() < 0) {
        printf("AT Module Uart recovery failed\r\n");
    }

    wakeup_reason = bl_lp_get_wake_reason();
    if (wakeup_reason & LPFW_WAKEUP_WIFI_BROADCAST) {
        vTaskNotifyGiveFromISR(rxl_process_task_hd, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        pm_alloc_mem_reset();
    }

    return 0;
}

static void app_arp_send(void)
{
    if (!wifi_mgmr_sta_state_get()) {
        return;
    }

    if (netif_default == NULL || ip4_addr_isany_val(*netif_ip4_addr(netif_default))) {
        printf("IP address not assigned. ARP announcement skipped.\n");
        return;
    }

    LOCK_TCPIP_CORE();
    do {
        ip_addr_t src_ip = netif_default->ip_addr;
        ip_addr_t dest_ip;
        err_t result;

        ip_addr_copy(dest_ip, src_ip);
        result = etharp_request(netif_default, ip_2_ip4(&dest_ip));
        if (result != ERR_OK) {
            printf("Failed to send ARP request. Error: %d\n", result);
        }
    } while (0);
    UNLOCK_TCPIP_CORE();
}

static void arp_send(TimerHandle_t xTimer)
{
    (void)xTimer;
    app_arp_send();
}

int app_pm_create_arp_announce_timer(uint32_t seconds)
{
    if (xArpTimer != NULL) {
        return -1;
    }

    app_arp_send();

    xArpTimer = xTimerCreate("traffic probe", pdMS_TO_TICKS(seconds * 1000), pdTRUE, NULL, arp_send);
    if (xArpTimer == NULL) {
        return -1;
    }

    return xTimerStart(xArpTimer, 0) == pdPASS ? 0 : -1;
}

int app_pm_delete_arp_announce_timer(void)
{
    if (xArpTimer == NULL) {
        printf("Arp timer has not been created.\r\n");
        return 0;
    }

    xTimerStop(xArpTimer, 0);
    xTimerDelete(xArpTimer, portMAX_DELAY);
    xArpTimer = NULL;
    printf("Delete arp timer.\r\n");

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
#elif defined(BL616CL)
    else if (mode == 1) {
        hbn_sleep_cnt = ((uint64_t)32768 * ms) / 1000;
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
#elif defined(BL616CL)
    uint32_t hbn_level = PM_HBN_LEVEL_0;

    if (level == PM_HBN_LEVEL_1) {
        hbn_level = PM_HBN_LEVEL_1;
    } else if (level != PM_HBN_LEVEL_0) {
        printf("BL616CL only supports HBN level 0 or 1, use HBN_0.\r\n");
    }

    if (lp_wake_io_configured) {
        pm_lowpower_gpio_cfg(&lp_wake_io_cfg);
    }

    pm_hbn_mode_enter(hbn_level, hbn_sleep_cnt);
#else
    (void)level;
    printf("HBN entry is not implemented on this chip yet.\r\n");
#endif
}

void app_pm_enter_pds15(void)
{
    if (lp_timeouts_ms != 0) {
        app_pm_start_timeout_timer(lp_timeouts_ms);
        lp_timeouts_ms = 0;
    }

#if defined(BL616CL)
    PDS_Set_All_GPIO_Pad_IntClr();
#endif
    pm_enable_tickless();
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
#elif defined(BL616CL)
    if (io >= IO_WAKEUP_MAX) {
        printf("only support gpio 0-%d now.\r\n", IO_WAKEUP_MAX - 1);
        return -1;
    }

    if (mode > 1) {
        printf("not support mode:%d\r\n", mode);
        return -1;
    }

    lp_wake_io_cfg.io_ie |= ((uint64_t)1 << io);
    lp_wake_io_cfg.io_pd |= ((uint64_t)1 << io);
    lp_wake_io_cfg.io_pu &= ~((uint64_t)1 << io);
    lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << io);
    if (mode == 0) {
        lp_wake_io_cfg.io_0_36_trig_mode[io] = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL;
    } else {
        lp_wake_io_cfg.io_0_36_trig_mode[io] = BL_LP_PDS_IO_TRIG_SYNC_RISING_FALLING_EDGE;
    }

    bl_lp_io_wakeup_cfg((void *)&lp_wake_io_cfg);
    bl_lp_wakeup_io_int_register(test_wakeup_io_callback);
    lp_wake_io_configured = 1;

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
#elif defined(BL616CL)
    if (io >= IO_WAKEUP_MAX) {
        printf("only support gpio 0-%d now.\r\n", IO_WAKEUP_MAX - 1);
        return -1;
    }

    lp_wake_io_cfg.io_ie &= ~((uint64_t)1 << io);
    lp_wake_io_cfg.io_pu &= ~((uint64_t)1 << io);
    lp_wake_io_cfg.io_pd &= ~((uint64_t)1 << io);
    lp_wake_io_cfg.io_wakeup_unmask &= ~((uint64_t)1 << io);
    lp_wake_io_cfg.io_0_36_trig_mode[io] = BL_LP_PDS_IO_TRIG_NONE;

    bl_lp_io_wakeup_cfg((void *)&lp_wake_io_cfg);
    if (lp_wake_io_cfg.io_wakeup_unmask == 0) {
        lp_wake_io_configured = 0;
    }

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

int app_pm_init(void)
{
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

#if defined(BL616)
    uint8_t soc_v, rt_v, aon_v;
    hal_pm_ldo11_cfg(PM_PDS_LDO_LEVEL_SOC_DEFAULT, PM_PDS_LDO_LEVEL_RT_DEFAULT, PM_PDS_LDO_LEVEL_AON_DEFAULT);
    hal_pm_ldo11_cfg_get(&soc_v, &rt_v, &aon_v);
    printf("SOC:%d RT:%d AON:%d\r\n", soc_v, rt_v, aon_v);
#endif

    HBN_Enable_RTC_Counter();
#if defined(BL616)
    pm_rc32k_auto_cal_init();
#endif
    pm_sys_init();
    bl_lp_init();
    bl_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);

    printf("[OS] Create f32k_clk_init task...\r\n");
    xTaskCreate(f32k_clk_init_task, (char *)"f32k_clk_init", 1024, NULL, 12, NULL);

    return 0;
}
