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
#include <bflb_mtimer.h>
#include <board.h>
#include <bl_lp.h>
#include <bl616_pm.h>
#include <bflb_uart.h>
#include <bl616_glb.h>
#include <bl616_glb_gpio.h>
#include <bl616_hbn.h>
#include "pm_manager.h"

#include <board.h>
#include <board_rf.h>
#include <shell.h>
#include <bl616_glb.h>


extern int tickless_enter(void);
extern int tickless_exit(void);

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

extern bl_lp_fw_cfg_t lpfw_cfg;

static void cmd_tickless(int argc, char **argv)
{
    int broadcast = 0;

    if (argc > 2) {
        if (argv[2] != NULL) {
            broadcast = atoi(argv[2]);
        } else {
            broadcast = 0;
        }

        if (broadcast == 0) {
            if (argv[1] != NULL) {
                lpfw_cfg.dtim_origin = atoi(argv[1]);
            } else {
                lpfw_cfg.dtim_origin = 10;
            }
        }
    } else if (argc > 1) {
        broadcast = 0;
        if (argv[1] != NULL) {
            lpfw_cfg.dtim_origin = atoi(argv[1]);
        } else {
            lpfw_cfg.dtim_origin = 10;
        }
    } else {
        lpfw_cfg.dtim_origin = 10;
        broadcast = 0;
    }

    printf("dtim_origin: %d\r\n", lpfw_cfg.dtim_origin);
    printf("broadcast: %d\r\n", broadcast);

    if (broadcast) {
        enable_multicast_broadcast = 1;
        lpfw_cfg.bcmc_dtim_mode = 1;
    } else {
        enable_multicast_broadcast = 0;
        lpfw_cfg.bcmc_dtim_mode = 0;
    }

    pm_enable_tickless();
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

#ifdef LP_APP
    if (argc > 2) {
        bl_lp_fw_bcn_loss_cfg_dtim_default(lpfw_cfg.dtim_origin);

        printf("sta_ps %ld\r\n", wifi_mgmr_sta_ps_enter());
        tickless_enter();
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

static void cmd_io_dbg(int argc, char **argv)
{
    if (argc != 2) {
        printf("cmd_io_dbg err\r\n");
        return;
    }

    if (atoi(argv[1]) <= 34) {
        iot2lp_para->debug_io = atoi(argv[1]);
    } else {
        iot2lp_para->debug_io = 0xFF;
    }
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

    bl_lp_io_wakeup_cfg(&lp_wake_io_cfg);
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

SHELL_CMD_EXPORT_ALIAS(cmd_tickless, tickless, cmd tickless);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_lp, wifi_lp_test, wifi low power test);
SHELL_CMD_EXPORT_ALIAS(test_tcp_keepalive, lpfw_tcp_keepalive, tcp keepalive test);
SHELL_CMD_EXPORT_ALIAS(cmd_io_dbg, io_debug, cmd io_debug);
#endif

static TaskHandle_t rc32k_coarse_trim_task_hd = NULL;
static TaskHandle_t xtal32k_check_entry_task_hd = NULL;

/**********************************************************
    rc32k coarse trim task func
 **********************************************************/
static void rc32k_coarse_trim_task(void *pvParameters)
{
    uint32_t retry_cnt = 0;
    uint64_t timeout_start;

    uint64_t rtc_cnt, rtc_record_us, rtc_now_us;
    uint64_t mtimer_record_us, mtimer_now_us;

    uint32_t rtc_us, mtimer_us;
    int error_ppm;

    printf("rc32k_coarse_trim task enable, freq_mtimer must be 1MHz!\r\n");
    timeout_start = bflb_mtimer_get_time_us();

    vTaskDelay(20);

    while(1){
        retry_cnt += 1;

        /* disable irq */
        __disable_irq();

        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_record_us = BL_PDS_CNT_TO_US(rtc_cnt);

        /* delay */
        vTaskDelay(50);

        /* disable irq */
        __disable_irq();

        mtimer_now_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

        /* calculate */
        rtc_us = (uint32_t)(rtc_now_us - rtc_record_us);
        mtimer_us = (uint32_t)(mtimer_now_us - mtimer_record_us);
        /* call coarse_adj */
        error_ppm = bl_lp_rtc_rc32k_coarse_adj(mtimer_us, rtc_us);

        printf("rc32k_coarse_trim: mtimer_us:%d, rtc_us:%d\r\n", mtimer_us, rtc_us);

        if(error_ppm > 2000 || error_ppm < -2000){
            /*  */
            printf("rc32k_coarse_trim: retry_cnt:%d, ppm:%d, continue...\r\n", retry_cnt, error_ppm);
            vTaskDelay(5);
        }else{
            /* finish */
            printf("rc32k_coarse_trim: retry_cnt:%d, ppm:%d, finish!\r\n", retry_cnt, error_ppm);
            break;
        }
    }

    printf("rc32k coarse trim success!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);

    /* coarse_adj success */
    if(xtal32k_check_entry_task_hd){
        /* resume xtal32k_check task */
        printf("rc32k_coarse_trim: Resume xtal32k_check task!\r\n");
        xTaskNotifyGive(xtal32k_check_entry_task_hd);
    }else{
        /* set bl_lp 32k clock ready */
        printf("rc32k_coarse_trim: set lp_32k ready!\r\n");
        bl_lp_set_32k_clock_ready(1);
    }

    printf("rc32k_coarse_trim: rc32k code:%d\r\n", iot2lp_para->rc32k_fr_ext);

    printf("rc32k_coarse task: vTaskDelete\r\n");
    vTaskDelete(NULL);
}

/**********************************************************
    xtal32k check task func
 **********************************************************/
static void xtal32k_check_entry_task(void *pvParameters)
{
    uint32_t xtal32_regulator_flag = 0;

    uint64_t timeout_start;

    uint32_t retry_cnt = 0;

    uint64_t rtc_cnt, rtc_record_us, rtc_now_us;
    uint64_t mtimer_record_us, mtimer_now_us;

    uint32_t rtc_us, mtimer_us;
    int32_t diff_us;

    uint32_t success_flag = 0;

    vTaskDelay(10);
    printf("xtal32k_check_entry task enable, freq_mtimer must be 1MHz!\r\n");

    GLB_GPIO_Cfg_Type gpioCfg = {
        .gpioPin = GLB_GPIO_PIN_0,
        .gpioFun = GPIO_FUN_ANALOG,
        .gpioMode = GPIO_MODE_ANALOG,
        .pullType = GPIO_PULL_NONE,
        .drive = 1,
        .smtCtrl = 1
    };
    gpioCfg.gpioPin = 16;
    GLB_GPIO_Init(&gpioCfg);
    gpioCfg.gpioPin = 17;
    GLB_GPIO_Init(&gpioCfg);

    /* power on */
    HBN_Set_Xtal_32K_Inverter_Amplify_Strength(3);
    HBN_Power_On_Xtal_32K();

    timeout_start = bflb_mtimer_get_time_us();

    printf("xtal32k_check: delay 100 ms\r\n");
    vTaskDelay(500);

    if(rc32k_coarse_trim_task_hd){
        printf("xtal32k_check: wait rc32k_coarse_trim finish\r\n");
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }

    printf("xtal32k_check: start check\r\n");

    HBN_32K_Sel(1);
    vTaskDelay(2);

    while(1){
        retry_cnt += 1;

        /* disable irq */
        __disable_irq();

        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_record_us = BL_PDS_CNT_TO_US(rtc_cnt);

        /* delay */
        vTaskDelay(10);

         /* disable irq */
        __disable_irq();

        mtimer_now_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

        /* calculate */
        rtc_us = (uint32_t)(rtc_now_us - rtc_record_us);
        mtimer_us = (uint32_t)(mtimer_now_us - mtimer_record_us);
        diff_us = rtc_us - mtimer_us;

        printf("xtal32k_check: mtimer_us:%d, rtc_us:%d\r\n", mtimer_us, rtc_us);

        if(diff_us < -100 || diff_us > 100){
            /* continue */
            printf("xtal32k_check: retry_cnt:%d, diff_us:%d, continue...\r\n", retry_cnt, diff_us);
            vTaskDelay(10);
        }else{
            /* finish */
            printf("xtal32k_check: retry_cnt:%d, diff_us:%d, finish!\r\n", retry_cnt, diff_us);
            success_flag = 1;
            break;
        }

        /* 1sec, set xtal regulator */
        if((xtal32_regulator_flag == 0) && (bflb_mtimer_get_time_us() - timeout_start > 1000*1000)){
            printf("xtal32K_check: reset xtal32k regulator\r\n");
            xtal32_regulator_flag = 1;

            HBN_32K_Sel(0);
            HBN_Power_Off_Xtal_32K();

            vTaskDelay(10);

            HBN_Set_Xtal_32K_Regulator(3);
            HBN_Power_On_Xtal_32K();
            HBN_32K_Sel(1);
        }

        if(bflb_mtimer_get_time_us() - timeout_start > 3 * 1000 * 1000){
            success_flag = 0;
            break;
        }
    }

    if(success_flag){
        printf("xtal32k_check: success!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);

        /* GPIO17 no pull */
        *((volatile uint32_t *)0x2000F014) &= ~(1 << 16);

        printf("select xtal32k\r\n");

    }else{
        printf("xtal32k_check: failure!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);
        printf("xtal32k_check: select rc32k, and xtal32k poweroff \r\n");
        HBN_32K_Sel(0);
        HBN_Power_Off_Xtal_32K();
    }

    /* */
    printf("xtal32k_check: set lp_32k ready!\r\n");
    bl_lp_set_32k_clock_ready(1);

    printf("xtal32k_check task: vTaskDelete\r\n");
    vTaskDelete(NULL);
}

void timerCallback(TimerHandle_t xTimer)
{
    tickless_exit();
    xTimerDelete(xTimer, portMAX_DELAY);

    //if (wifi_mgmr_sta_state_get()) {
    //    wifi_mgmr_sta_ps_exit();
    //}
    spisync_wakeup(NULL);
}

void createAndStartTimer(const char* timerName, TickType_t timerPeriod)
{
    TimerHandle_t timer = xTimerCreate(timerName,
                                       timerPeriod,
                                       pdTRUE,
                                       0,
                                       timerCallback
                                    );

    if (timer == NULL)
    {
        printf("Failed to create timer.\n");
        return;
    }

    if (xTimerStart(timer, 0) != pdPASS)
    {
        printf("Failed to start timer.\n");
        return;
    }
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
        TickType_t timerPeriod = pdMS_TO_TICKS(lp_timerouts_ms);

        char timerName[32];
        snprintf(timerName, sizeof(timerName), "PwrTimer_%u", (unsigned int)timerPeriod);

        createAndStartTimer(timerName, timerPeriod);
        lp_timerouts_ms = 0;
    }

    tickless_enter();
}

int app_pm_init(void)
{
    uint8_t soc_v, rt_v, aon_v;

    hal_pm_ldo11_cfg(PM_PDS_LDO_LEVEL_SOC_DEFAULT, PM_PDS_LDO_LEVEL_RT_DEFAULT, PM_PDS_LDO_LEVEL_AON_DEFAULT);
    hal_pm_ldo11_cfg_get(&soc_v, &rt_v, &aon_v);
    printf("SOC:%d RT:%d AON:%d\r\n", soc_v, rt_v, aon_v);

    HBN_Enable_RTC_Counter();
    pm_rc32k_auto_cal_init();

#ifdef LP_APP
#if defined(CFG_BL_WIFI_PS_ENABLE) || defined(CFG_WIFI_PDS_RESUME)
    bl_lp_init();
#endif
    bl_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);
#endif

    /* coarse trim rc32k */
    puts("[OS] Create rc32k_coarse_trim task...\r\n");
    xTaskCreate(rc32k_coarse_trim_task, (char*)"rc32k_coarse_trim", 512, NULL, 31, &rc32k_coarse_trim_task_hd);

    /* auto check xtal32k, only test */
    puts("[OS] Create xtal32k_check_entry task...\r\n");
    xTaskCreate(xtal32k_check_entry_task, (char*)"xtal32k_check_entry", 512, NULL, 31, &xtal32k_check_entry_task_hd);

    return 0;
}
