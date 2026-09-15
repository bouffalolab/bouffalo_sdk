#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include <bl_fw_api.h>
#include <wifi_mgmr_ext.h>
#include <wifi_mgmr.h>

#include <bflb_irq.h>
#include <board.h>
#include <bl_lp.h>
#include <bl616_pm.h>
#include <bflb_uart.h>
#include <bl616_glb.h>
#include <bl616_glb_gpio.h>
#include <bl616_hbn.h>
#include "clock_manager.h"
#include "pm_helper_cli.h"
#include "pm_manager.h"
#include "tickless.h"
#include "tickless_hook.h"

#include <board.h>
#include <board_rf.h>
#include <shell.h>
#include <bl616_glb.h>


extern void vPortSetupTimerInterrupt(void);
void __attribute__((weak)) spisync_wakeup(void *arg) { (void)arg; }

#ifdef BL_HOSTROUTER_ENABLE
#include "sdiowifi_mgmr.h"
#endif

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[1024];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = 1024;
}

GLB_GPIO_Type pinList[4] = {
  GLB_GPIO_PIN_0,
  GLB_GPIO_PIN_1,
  GLB_GPIO_PIN_2,
  GLB_GPIO_PIN_3,
};

extern void uart_shell_isr();
extern struct bflb_device_s *uart_shell;

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
    set_cpu_bclk_80M_and_gate_clk();

    /* recovery system_clock_init\peripheral_clock_init\console_init*/
    board_recovery();

    //GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
    //bl_sys_em_config();

    board_rf_ctl(BRD_CTL_RF_RESET_DEFAULT, 0);

    vPortSetupTimerInterrupt();

    bflb_uart_rxint_mask(uart_shell, false);
    bflb_irq_attach(uart_shell->irq_num, uart_shell_isr, NULL);
    bflb_irq_enable(uart_shell->irq_num);

    //GLB_GPIO_Func_Init(GPIO_FUN_JTAG, pinList, 4);

    return 0;
}

#if 0
static int lp_exit(void *arg)
{
    printf("start board recovery.\r\n");
    /* recovery system_clock_init\peripheral_clock_init\console_init*/
    board_recovery();
    printf("board recovery.\r\n");

    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);

    board_rf_ctl(BRD_CTL_RF_RESET_DEFAULT, 0);

    vPortSetupTimerInterrupt();

    bl_uart_rxint_mask(uart_shell, false);
    bl_irq_attach(uart_shell->irq_num, uart_shell_isr, NULL);
    bl_irq_enable(uart_shell->irq_num);
    printf("init uart.\r\n");

    GLB_GPIO_Func_Init(GPIO_FUN_JTAG, pinList, 4);

    return 0;
}
#endif

static int lp_enter(void *arg)
{
    return 0;
}

#ifdef CONFIG_SHELL
int cmd_wifi_lp(int argc, char **argv)
{
    int ret = 0;
    printf("enter wireless low power!\r\n");

    bl_lp_init();
    // bl_lp_fw_init();
    bl_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);

    while (1) {
        // lp_exit(0);
        ret = bl_lp_fw_enter(&lpfw_cfg);
        if (ret < 0) {
            printf("[E]bflb_lpfw_enter Fail,ErrId:%d\r\n", ret);
        } else {
            printf("bflb_lpfw_enter Success\r\n");
        }
        arch_delay_ms(1000);
    }

    return 0;
}

static int test_tcp_keepalive(int argc, char **argv)
{
    int sockfd;
    // uint8_t *recv_buffer;
    struct sockaddr_in dest, my_addr;
    char buffer[51];
    uint32_t pck_cnt = 0;
    uint32_t pck_total = 0;
    uint8_t tcp_keepalive_period = 60;

    /* Create a socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error in socket\r\n");
        return -1;
    }

    /*---Initialize server address/port struct---*/
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(50001);

    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(50001);
    inet_aton(argv[1], &dest.sin_addr);

    if (argc >= 4) {
        lpfw_cfg.dtim_origin = atoi(argv[3]);
    }

    if (argc >= 5) {
        tcp_keepalive_period = atoi(argv[4]);
    }

    if (argc >= 6) {
        pck_cnt = atoi(argv[5]);
        printf("keep alive pck:%ld\r\n");
    }

    printf("tcp server ip: %s\r\n", argv[1]);

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0) {
        printf("Error in bind\r\n");
        close(sockfd);
        return -1;
    }

    /*---Connect to server---*/
    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) != 0) {
        printf("Error in connect\r\n");
        close(sockfd);
        return -1;
    }

    /*---Get "Hello?"---*/
    memset(buffer, 'A', sizeof(buffer) - 1);

#ifdef CONFIG_LPAPP
    if (argc > 2) {
        pm_enable_tickless();
    }
#endif

    int ret = 0;

    while (1) {
        pck_total++;
        snprintf(buffer, sizeof(buffer), "SEQ = %ld  ", pck_total);

        buffer[sizeof(buffer) - 2] = '\n';
        ret = write(sockfd, buffer, sizeof(buffer) - 1);
        if (ret != sizeof(buffer) - 1) {
            printf("write error: %d\n", ret);
            break;
        }
        printf("**********************************\n");
        printf("SEQ:%ld WRITE SUCCESS %d\n", pck_total, ret);

        if (pck_cnt && (pck_total >= pck_cnt)) {
            bl_pm_event_bit_set(PSM_EVENT_APP);
            break;
        }
#if 0
        ret = read(sockfd, buffer, sizeof(buffer)-1);
        buffer[sizeof(buffer) -1] = 0;
        printf("read ret: %d, %s\r\n", ret, buffer);
#endif
        vTaskDelay(pdMS_TO_TICKS(tcp_keepalive_period * 1000));
    }

    close(sockfd);
    return 0;
}

static void lp_io_wakeup_callback(uint64_t wake_up_io_bits)
{
    tickless_exit();

     //TODO ;can not call in interupt context
    //if (wifi_mgmr_sta_state_get()) {
    //    wifi_mgmr_sta_ps_exit();
    //}

    //call resume spi
    spisync_wakeup(NULL);
}

static bl_lp_io_cfg_t lp_wake_io_cfg = {
    .io_20_34_ie = BL_LP_IO_INPUT_ENABLE,
};

int lp_set_wakeup_by_io(uint8_t io, uint8_t mode)
{
    if (io <28 || io > 29) {
        printf("only support gpio 28, 29 now.\r\n");
        return -1;
    }
    
    if (mode > 1) {
        printf("not support mode:%d\r\n", mode);
        return -1;
    } 

    if (mode == 0) {
        lp_wake_io_cfg.io_28_34_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL;
    } else if (mode == 1) {
        lp_wake_io_cfg.io_28_34_pds_trig_mode = BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE;
    } else {
    }

#if 0
    if (mode == 1) {
        lp_wake_io_cfg.io_20_34_res = BL_LP_IO_RES_PULL_UP;
    } else {
    }
#endif

    lp_wake_io_cfg.io_20_34_res = BL_LP_IO_RES_PULL_DOWN;

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

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_lp, wifi_lp_test, wifi low power test);
SHELL_CMD_EXPORT_ALIAS(test_tcp_keepalive, lpfw_tcp_keepalive, tcp keepalive test);
#endif

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

void app_pm_exit_pds15(void)
{
    pm_disable_tickless();
}

int app_pm_create_arp_announce_timer(uint32_t seconds)
{
    return pm_helper_cli_arp_timer_start(seconds);
}

int app_pm_delete_arp_announce_timer(void)
{
    return pm_helper_cli_arp_timer_stop();
}

int app_create_keepalive_timer(uint32_t period_seconds)
{
    return pm_helper_cli_keepalive_timer_start(period_seconds);
}

int app_delete_keepalive_timer(void)
{
    return pm_helper_cli_keepalive_timer_stop();
}

int app_pm_twt_param_set(int s, int t, int e, int n, int m)
{
    return pm_helper_cli_twt_setup(s, t, e, n, m);
}

static int sdio_wifi_wakeup_timer_start(uint32_t timeout_ms, int broadcast, void *arg)
{
    (void)broadcast;
    (void)arg;

    if (app_lp_timer_config(0, timeout_ms) != 0) {
        return -1;
    }
    app_pm_enter_pds15();
    return 0;
}

static int sdio_wifi_tickless_start(void *arg)
{
    (void)arg;
    app_pm_enter_pds15();
    return 0;
}

#ifdef BL_HOSTROUTER_ENABLE
static bool sdio_wifi_prepare_sleep(void *arg)
{
    (void)arg;
    return sdiowifi_mgmr_ps_prepare() == 0;
}
#endif

int app_pm_init(void)
{
    static const pm_helper_cli_cfg_t helper_cfg = {
        .wakeup_timer_cb = sdio_wifi_wakeup_timer_start,
        .tickless_cb = sdio_wifi_tickless_start,
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

#ifdef CONFIG_LPAPP
    bl_lp_init();
    bl_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);
#endif

#ifdef BL_HOSTROUTER_ENABLE
    tickless_hooks_register(NULL, NULL, sdio_wifi_prepare_sleep, NULL, NULL, NULL);
#endif

    if (pm_helper_cli_init(&helper_cfg) != 0) {
        printf("pm helper cli init failed.\r\n");
        return -1;
    }

    puts("[OS] Create f32k_clk_init task...\r\n");
    xTaskCreate(f32k_clk_init_task, (char *)"f32k_clk_init", 1024, NULL, 12, NULL);

    return 0;
}
