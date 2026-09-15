#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

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
#include "bl616_pm.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bl616_glb.h"
#include "bl616_glb_gpio.h"
#include "bl616_hbn.h"
#include "bflb_rtc.h"
#include "assert.h"
#include "rfparam_adapter.h"

#include "board.h"
#include "board_rf.h"
#include "shell.h"
#if defined(CONFIG_BLE_ENABLE)
#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "btble_lib_api.h"
#include "hci_driver.h"
#include "hci_core.h"
#endif
#include "bl616_glb.h"
//#include "spisync.h"
#include "clock_manager.h"
#include "pm_helper_cli.h"
#include "pm_manager.h"
#include <nxspi.h>
#include <lwip/etharp.h>

extern void vPortSetupTimerInterrupt(void);


#define APP_PM_IELD_TASK_STACK_SIZE (512)

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[APP_PM_IELD_TASK_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = APP_PM_IELD_TASK_STACK_SIZE;
}


#ifdef CONFIG_SHELL
extern void uart_shell_isr();
extern struct bflb_device_s *uart_shell;
GLB_GPIO_Type pinList[4] = {
  GLB_GPIO_PIN_0,
  GLB_GPIO_PIN_1,
  GLB_GPIO_PIN_2,
  GLB_GPIO_PIN_3,
};

static void set_cpu_bclk_80M_and_gate_clk(void)
{
    uint32_t tmpVal = 0;

    GLB_Set_MCU_System_CLK_Div(0, 3);
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_CLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK) / 1000000 - 1);

    /* clk gate,except DMA&CPU&UART0&SF&EMI&WIFI&EFUSE */
    tmpVal = 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_CPU, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_DMA, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_SEC, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_SDU, 1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG0,tmpVal);

    tmpVal = 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EF_CTRL, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_SF_CTRL, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_DMA, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_UART0, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_UART1, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_SEC_ENG, 1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1,tmpVal);

    tmpVal = 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S2_WIFI, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_EMI_MISC, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_PIO, 1);

    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpVal);
}

static int lp_exit(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    extern TaskHandle_t rxl_process_task_hd;
    int wakeup_reason;

    nxspi_ps_exit();

    set_cpu_bclk_80M_and_gate_clk();

    /* recovery system_clock_init\peripheral_clock_init\console_init*/
    board_recovery();

    //GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
    //bflb_sys_em_config();

    //board_rf_ctl(BRD_CTL_RF_RESET_DEFAULT, 0);

    vPortSetupTimerInterrupt();

    bflb_uart_rxint_mask(uart_shell, false);
    bflb_irq_attach(uart_shell->irq_num, uart_shell_isr, NULL);
    bflb_irq_enable(uart_shell->irq_num);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    wakeup_reason = bl_lp_get_wake_reason();
    if (wakeup_reason & LPFW_WAKEUP_WIFI_BROADCAST) {
        vTaskNotifyGiveFromISR(rxl_process_task_hd, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        pm_alloc_mem_reset();
    }

    //GLB_GPIO_Func_Init(GPIO_FUN_JTAG, pinList, 4);

    return 0;
}


static int lp_enter(void *arg)
{
    nxspi_ps_enter();
    return 0;
}

#define BASE_ADDRESS 0x2000f000
#define OFFSET 0x204

void modify_register_bits_incremental(int increment)
{
    volatile uint32_t *reg_address = (volatile uint32_t *)(BASE_ADDRESS + OFFSET);

    uint32_t mask = (0x3F << 11);
    uint32_t current_value = *reg_address;
    uint32_t target_bits = (current_value & mask) >> 11;

    /*
    printf("Before change:\r\n");
    printf("  Register value (full): 0x%08X\r\n", current_value);
    printf("  bit11 to bit16 value: 0x%X\r\n", target_bits);
    */

    target_bits = (int32_t)(target_bits) + increment;
    if (target_bits > 0x3F) {
        target_bits = 0x3F;
    } else if (target_bits < 0) {
        target_bits = 0;
    }

    current_value = (current_value & ~mask) | (target_bits << 11);
    *reg_address = current_value;
    current_value = *reg_address;
    target_bits = (current_value & mask) >> 11;

    /*
    printf("After change:\r\n");
    printf("  Register value (full): 0x%08X\r\n", current_value);
    printf("  bit11 to bit16 value: 0x%X\r\n", target_bits);
    */
}

static void lp_io_wakeup_callback(uint64_t wake_up_io_bits)
{

}

static bl_lp_io_cfg_t lp_wake_io_cfg;
int lp_set_wakeup_by_io(uint8_t io, uint8_t mode)
{
    if (io != 16 && io != 28) {
        printf("only support gpio 16, 28\r\n");
        return -1;
    }

    if (mode > 1) {
        printf("not support mode:%d\r\n", mode);
        return -1;
    }

    if (io == 16) {
        lp_wake_io_cfg.io_16_ie = BL_LP_IO_INPUT_ENABLE;
        if (mode == 0) {
            lp_wake_io_cfg.io_16_19_aon_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL;
        } else{
            lp_wake_io_cfg.io_16_19_aon_trig_mode = BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE;
        }
        lp_wake_io_cfg.io_16_res = BL_LP_IO_RES_PULL_DOWN;
    } else {
        lp_wake_io_cfg.io_20_34_ie = BL_LP_IO_INPUT_ENABLE;
        if (mode == 0) {
            lp_wake_io_cfg.io_28_34_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL;
        } else if (mode == 1) {
            lp_wake_io_cfg.io_28_34_pds_trig_mode = BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE;
        }
        lp_wake_io_cfg.io_20_34_res = BL_LP_IO_RES_PULL_DOWN;
    }

    lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << io);

    bl_lp_io_wakeup_cfg((void *)&lp_wake_io_cfg);
    bl_lp_wakeup_io_int_register(lp_io_wakeup_callback);

    return 0;

}

int lp_delete_wakeup_by_io(uint8_t io)
{
    if (io <28 || io > 29) {
        printf("only support gpio 28, 29 now.\r\n");
        return -1;
    }

    lp_wake_io_cfg.io_wakeup_unmask &= ~((uint64_t)1 << io);

    return 0;
}

static void modify_bit(uint32_t *reg_addr, uint8_t bit_position, uint8_t bit_value) {
    if (bit_value) {
        *reg_addr |= (1U << bit_position);
    } else {
        *reg_addr &= ~(1U << bit_position);
    }
}

void write_register(uint32_t *reg_addr, uint32_t value) {
    *reg_addr = value;
}

static void cmd_32k_output(int argc, char **argv)
{
    modify_bit((uint32_t *)0x2000F204, 22, 0x1);
    write_register((uint32_t *)0x200002E8, 0x4000);
    write_register((uint32_t *)0x200002F0, 0x1);
    write_register((uint32_t *)0x20000930, 0x40000F02);
}

int app_pm_create_arp_announce_timer(uint32_t seconds)
{
    return pm_helper_cli_arp_timer_start(seconds);
}

int app_pm_delete_arp_announce_timer(void)
{
    return pm_helper_cli_arp_timer_stop();
}

static void cmd_create_arp_timer(int argc, char **argv)
{
    if (argc != 2) {
        printf("Need param\r\n");
        return;
    }

    printf("set arp interval :%d s\r\n",atoi(argv[1]));
    app_pm_create_arp_announce_timer(atoi(argv[1]));

    return;
}

static void cmd_delete_arp_timer(int argc, char **argv)
{
    app_pm_delete_arp_announce_timer();
    return;
}

SHELL_CMD_EXPORT_ALIAS(cmd_32k_output, output_32k, cmd 32k output);
SHELL_CMD_EXPORT_ALIAS(cmd_create_arp_timer, create_arp_timer, cmd create arp timer);
SHELL_CMD_EXPORT_ALIAS(cmd_delete_arp_timer, delete_arp_timer, cmd delete arp timer);
#endif

static void app_pm_timeout_callback(TimerHandle_t timer)
{
    pm_disable_tickless();
    xTimerDelete(timer, 0);
}

static int app_pm_start_timeout_timer(uint32_t timeout_ms)
{
    TimerHandle_t timer;

    timer = xTimerCreate("PwrTimer", pdMS_TO_TICKS(timeout_ms), pdFALSE, NULL,
                         app_pm_timeout_callback);
    if (timer == NULL) {
        return -1;
    }

    if (xTimerStart(timer, 0) != pdPASS) {
        xTimerDelete(timer, 0);
        return -1;
    }

    return 0;
}

static bl_lp_hbn_fw_cfg_t hbn_test_cfg={
    //.hbn_sleep_cnt = (32768 * ms) / 1000,
    .hbn_sleep_cnt = 0,
    .hbn_level=0,
};

static uint32_t lp_timerouts_ms;

int app_lp_timer_config(int mode, uint32_t ms)
{
    if (0 == mode) {
        lp_timerouts_ms = ms;
    } else if (1 == mode){
        hbn_test_cfg.hbn_sleep_cnt = (32768 * ms) / 1000;
    } else {
        printf("config lp level fail:%d\r\n", mode);

        return -1;
    }

    return 0;
}

void app_pm_enter_hbn(int level)
{
    bl_lp_hbn_init(0,0,0,0);

    if (level > 0 && level < 3) {
        hbn_test_cfg.hbn_level=level;
    }

    bl_lp_hbn_enter(&hbn_test_cfg);
}

void app_pm_enter_pds15(void)
{
    if (lp_timerouts_ms) {
        (void)app_pm_start_timeout_timer(lp_timerouts_ms);
        lp_timerouts_ms = 0;
    }

    pm_enable_tickless();
}

int app_pm_twt_param_set(int s, int t, int e, int n, int m)
{
    return pm_helper_cli_twt_setup(s, t, e, n, m);
}

int app_create_keepalive_timer(uint32_t periods)
{
    return pm_helper_cli_keepalive_timer_start(periods);
}

int app_delete_keepalive_timer(void)
{
    return pm_helper_cli_keepalive_timer_stop();
}

void app_pm_exit_pds15(void)
{
    pm_disable_tickless();
}

/**
 * @brief Sleep check callback for nxspi protocol state
 * @return nxspi_ps_get() result - 0 if sleep allowed, non-zero to prevent sleep
 */
static int nxspi_sleep_check_cb(void)
{
    return nxspi_ps_get();
}

static int spi_wifi_wakeup_timer_start(uint32_t timeout_ms, int broadcast, void *arg)
{
    (void)broadcast;
    (void)arg;

    if (app_lp_timer_config(0, timeout_ms) != 0) {
        return -1;
    }
    app_pm_enter_pds15();
    return 0;
}

static int spi_wifi_tickless_start(void *arg)
{
    (void)arg;
    app_pm_enter_pds15();
    return 0;
}

int pwr_info_clear(void)
{
    bl_lp_info_clear();

    return 0;
}

#define SLEEP_PDS_US        80
#define ACTIVE_LPFW_US      38000
#define ACTIVE_APP_US       57000

uint64_t pwr_info_get(void)
{
    bl_lp_info_t lp_info;
    bl_lp_info_get(&lp_info);

    printf("\r\nVirtual time: %llu us\r\n", bl_lp_get_virtual_us());
    printf("Power info dump:\r\n");
    printf("LPFW try recv bcn: %d, loss %d\r\n", lp_info.lpfw_recv_cnt, lp_info.lpfw_loss_cnt);
    printf("Total time %lldms\r\n", lp_info.time_total_us / 1000);
    printf("PDS sleep: %lldms\r\n", lp_info.sleep_pds_us / 1000);
    printf("LPFW active: %lldms\r\n", lp_info.active_lpfw_us / 1000);
    printf("APP active: %lldms\r\n", lp_info.active_app_us / 1000);

    uint64_t current = (lp_info.sleep_pds_us * SLEEP_PDS_US + lp_info.active_lpfw_us * ACTIVE_LPFW_US + lp_info.active_app_us * ACTIVE_APP_US) / lp_info.time_total_us;

    printf("Predict current: %llduA\r\n", current);

    return current;
}

int app_pm_init(void)
{
    static const pm_helper_cli_cfg_t helper_cfg = {
        .wakeup_timer_cb = spi_wifi_wakeup_timer_start,
        .tickless_cb = spi_wifi_tickless_start,
        .arg = NULL,
        .arp_target = PM_HELPER_CLI_ARP_LOCAL_IP,
        .arp_period_seconds = 55,
        .arp_send_immediately = true,
        .arp_periodic = true,
    };
    uint8_t soc_v, rt_v, aon_v;

    hal_pm_ldo11_cfg(PM_PDS_LDO_LEVEL_SOC_DEFAULT, PM_PDS_LDO_LEVEL_RT_DEFAULT, PM_PDS_LDO_LEVEL_AON_DEFAULT);
    hal_pm_ldo11_cfg_get(&soc_v, &rt_v, &aon_v);
    printf("SOC:%d RT:%d AON:%d\r\n", soc_v, rt_v, aon_v);

    HBN_Enable_RTC_Counter();
    pm_rc32k_auto_cal_init();

    pm_sys_init();

    /* Register nxspi sleep check callback */
    pm_sleep_check_register("nxspi", nxspi_sleep_check_cb, 10);

#ifdef CONFIG_LPAPP
    bl_lp_init();
    bl_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);
#endif

    if (pm_helper_cli_init(&helper_cfg) != 0) {
        printf("pm helper cli init failed.\r\n");
        return -1;
    }

    app_clock_init();

    return 0;
}
