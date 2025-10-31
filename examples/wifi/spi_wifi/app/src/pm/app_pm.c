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
#if defined(CFG_BLE_ENABLE)
#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "btble_lib_api.h"
#include "hci_driver.h"
#include "hci_core.h"
#endif
#include "bl616_glb.h"
//#include "spisync.h"
#include "app_clock_manager.h"

extern int enable_tickless;

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
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    nxspi_ps_exit(NULL);

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

    bflb_uart_rxint_mask(uart_shell, false);
    bflb_irq_attach(uart_shell->irq_num, uart_shell_isr, NULL);
    bflb_irq_enable(uart_shell->irq_num);
    printf("init uart.\r\n");

    GLB_GPIO_Func_Init(GPIO_FUN_JTAG, pinList, 4);

    return 0;
}
#endif

static int lp_enter(void *arg)
{
    nxspi_ps_enter(NULL);
    return 0;
}

#ifdef CONFIG_SHELL
int cmd_wifi_lp(int argc, char **argv)
{
    int ret = 0;
    printf("enter wireless low power!\r\n");

    bflb_lp_init();
    // bflb_lp_fw_init();
    bflb_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);

    while (1) {
        // lp_exit(0);
        ret = bflb_lp_fw_enter(&lpfw_cfg);
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

void set_dtim_config(int dtim)
{
    lpfw_cfg.dtim_origin = dtim;
    bl_lp_fw_bcn_loss_cfg_dtim_default(lpfw_cfg.dtim_origin);

    wifi_mgmr_sta_ps_enter();
}

void clear_dtim_config(void)
{
    wifi_mgmr_sta_ps_exit();
}

static void cmd_tickless(int argc, char **argv)
{
    if ((argc > 1) && (argv[1] != NULL)) {
        printf("%s\r\n", argv[1]);
        lpfw_cfg.dtim_origin = atoi(argv[1]);
    } else {
        lpfw_cfg.dtim_origin = 10;
    }

    bl_lp_fw_bcn_loss_cfg_dtim_default(lpfw_cfg.dtim_origin);
    printf("sta_ps %ld\r\n", wifi_mgmr_sta_ps_enter());
    enable_tickless = 1;
}

static void cmd_twt(int argc, char **argv)
{
    lpfw_cfg.dtim_origin = 0;

    bl_lp_fw_bcn_loss_cfg_dtim_default(lpfw_cfg.dtim_origin);
    printf("sta_ps %ld\r\n", wifi_mgmr_sta_ps_enter());

    enable_tickless = 1;
}

int pm_status_update(int status)
{
    //update status
    if (status) {
        enable_tickless = 0;
    } else {
        enable_tickless = 1;
    }
}

static int test_tcp_keepalive(int argc, char **argv)
{
    int sockfd;
    // uint8_t *recv_buffer;
    struct sockaddr_in dest, my_addr;
    char buffer[51];
    uint32_t pck_cnt = 0;
    uint32_t pck_total = 0;

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

    if (argc == 4) {
        pck_cnt = atoi(argv[3]);
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
        cmd_tickless(0, NULL);
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
        vTaskDelay(pdMS_TO_TICKS(30 * 1000));
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

static void cmd_xtal32k_calibration(int argc, char **argv)
{
    uint32_t offset;

    offset = atoi(argv[1]);
    printf("offset:%d\r\n", offset);
    modify_register_bits_incremental(offset);
}

static void cmd_xtal32k_calc(int argc, char **argv)
{
    uint64_t rtc_us, rtc_cal_us;
    uint64_t mtimer_us, mtimer_cal_us;
    uint64_t mius;


    uint32_t delay;

    delay = atoi(argv[1]);
    printf("delay:%ld\r\n", delay);

    //xtal 32k calibration
    __disable_irq();
    rtc_us = bflb_rtc_get_time(NULL);
    mtimer_us = bflb_mtimer_get_time_us();
    __enable_irq();

    vTaskDelay(delay);

    __disable_irq();
    rtc_cal_us = bflb_rtc_get_time(NULL);
    mtimer_cal_us = bflb_mtimer_get_time_us();
    __enable_irq();

    rtc_us = ((rtc_cal_us - rtc_us) * 1000000) / 32768;
    mtimer_us = mtimer_cal_us - mtimer_us;

    if (rtc_us > mtimer_us) {
        mius = rtc_us - mtimer_us;
    } else {
        mius = mtimer_us - rtc_us;
    }
    printf("mtimer:%llu rtc:%llu minus:%llu\r\n", mtimer_us, rtc_us, mius);
}

static void lp_io_wakeup_callback(uint64_t wake_up_io_bits)
{
    //enable_tickless = 0;

     //TODO ;can not call in interupt context
    //if (wifi_mgmr_sta_state_get()) {
    //    wifi_mgmr_sta_ps_exit();
    //}

    //call resume spi
    //spisync_wakeuparg_t wakeup_arg;
    //wakeup_arg.wakeup_reason = 0;
    //spisync_ps_wakeup(NULL, &wakeup_arg);
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

    bflb_lp_io_wakeup_cfg(&lp_wake_io_cfg);
    bflb_lp_wakeup_io_int_register(lp_io_wakeup_callback);

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

void modify_bit(uint32_t *reg_addr, uint8_t bit_position, uint8_t bit_value) {
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
    modify_bit(0x2000F204, 22, 0x1);
    write_register(0x200002E8, 0x4000);
    write_register(0x200002F0, 0x1);
    write_register(0x20000930, 0x40000F02);
}

void app_arp_send(void)
{
    if (!wifi_mgmr_sta_state_get()) {
        return;
    }

    if (ip4_addr_isany_val(*netif_ip4_addr(netif_default))) {
        printf("IP address not assigned. ARP announcement skipped.\n");
        return;
    }

    LOCK_TCPIP_CORE();
    do {
        ip_addr_t src_ip = netif_default->ip_addr;
        ip_addr_t dest_ip;
        ip_addr_copy(dest_ip, src_ip);
        err_t result = etharp_request(netif_default, &dest_ip);

        if (result != ERR_OK) {
            printf("Failed to send ARP request. Error: %d\n", result);
        }
    } while (0);
    UNLOCK_TCPIP_CORE();
}

TimerHandle_t xArpTimer = NULL;
static void arp_send(TimerHandle_t xTimer) {

    app_arp_send();
}

int app_pm_create_arp_announce_timer(uint32_t seconds)
{
    if (xArpTimer) {
        return -1;
    }

    app_arp_send();

    xArpTimer = xTimerCreate("traffic probe",  pdMS_TO_TICKS(seconds * 1000), pdTRUE, (void*)0, arp_send);
    xTimerStart(xArpTimer, 0);

    return 0;
}

int app_pm_delete_arp_announce_timer(void)
{
    if (xArpTimer) {
        xTimerStop(xArpTimer, 0);
        xTimerDelete(xArpTimer, portMAX_DELAY);
        xArpTimer = NULL;
        printf("Delete arp timer.\r\n");
    } else {
        printf("Arp timer has not been create.\r\n");
    }

    return 0;
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

SHELL_CMD_EXPORT_ALIAS(cmd_tickless, tickless, cmd tickless);
SHELL_CMD_EXPORT_ALIAS(cmd_twt, twt, cmd twt);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_lp, wifi_lp_test, wifi low power test);
SHELL_CMD_EXPORT_ALIAS(test_tcp_keepalive, lpfw_tcp_keepalive, tcp keepalive test);
SHELL_CMD_EXPORT_ALIAS(cmd_io_dbg, io_debug, cmd io_debug);
SHELL_CMD_EXPORT_ALIAS(cmd_32k_output, output_32k, cmd 32k output);
SHELL_CMD_EXPORT_ALIAS(cmd_xtal32k_calibration, xtal_calibration, cmd xtal calibration);
SHELL_CMD_EXPORT_ALIAS(cmd_xtal32k_calc, calc, cmd xtal32k calc);
SHELL_CMD_EXPORT_ALIAS(cmd_create_arp_timer, create_arp_timer, cmd create arp timer);
SHELL_CMD_EXPORT_ALIAS(cmd_delete_arp_timer, delete_arp_timer, cmd delete arp timer);
#endif

static TaskHandle_t xtal32k_check_entry_task_hd = NULL;

void timerCallback(TimerHandle_t xTimer)
{
    enable_tickless = 0;
    xTimerDelete(xTimer, portMAX_DELAY);

    //if (wifi_mgmr_sta_state_get()) {
    //    wifi_mgmr_sta_ps_exit();
    //}
    //spisync_wakeuparg_t wakeup_arg;
    //wakeup_arg.wakeup_reason = 2;
    //spisync_ps_wakeup(NULL, &wakeup_arg);
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
    bflb_lp_hbn_init(0,0,0,0);

    if (level > 0 && level < 3) {
        hbn_test_cfg.hbn_level=level;
    }

    bflb_lp_hbn_enter(&hbn_test_cfg);
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

    enable_tickless = 1;
}

TimerHandle_t keepalive_timer = NULL;

void keepalive_callback(TimerHandle_t xTimer)
{
    wifi_mgmr_null_data_send();
}

void app_pm_twt_enter(void)
{
    lpfw_cfg.dtim_origin = 0;

    bl_lp_fw_bcn_loss_cfg_dtim_default(lpfw_cfg.dtim_origin);
    printf("sta_ps %ld\r\n", wifi_mgmr_sta_ps_enter());
    enable_tickless = 1;
}

int app_create_keepalive_timer(uint32_t periods)
{
    TickType_t timerPeriod = pdMS_TO_TICKS(periods * 1000);

    if (keepalive_timer == NULL) {
        keepalive_timer = xTimerCreate("keepalive_timer", timerPeriod, pdTRUE, 0, keepalive_callback);
        if (keepalive_timer == NULL)
        {
            printf("Failed to create timer.\n");
            return -1;
        }

        if (xTimerStart(keepalive_timer, 0) != pdPASS)
        {
            printf("Failed to start timer.\n");
            return -1;
        }
    } else {
        if (xTimerChangePeriod(keepalive_timer, timerPeriod, 0) != pdPASS)
        {
            printf("Failed to change timer period.\n");
            return -1;
        }
    }

    return 0;
}

int app_delete_keepalive_timer(void)
{
    if (keepalive_timer != NULL) {
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
    } else {
        printf("No timer to delete.\n");
        return 0;
    }

    return 0;
}

void app_pm_exit_pds15(void)
{
    enable_tickless = 0;
    wifi_mgmr_sta_ps_exit();
}

int bflb_pm_app_check(void)
{
    return nxspi_ps_get();
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
#if defined(CFG_BFLB_WIFI_PS_ENABLE) || defined(CFG_WIFI_PDS_RESUME)
    bflb_lp_init();
#endif
    bflb_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);
#endif

    app_clock_init();
    app_atmodule_init();

    return 0;
}
