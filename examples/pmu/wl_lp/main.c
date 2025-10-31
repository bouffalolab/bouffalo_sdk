#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "mem.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/etharp.h>

#include "bl_fw_api.h"
#include "fhost_api.h"
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

#include "rfparam_adapter.h"

#include "wl_lp_app.h"

#include "board.h"
#include "board_rf.h"
#include "shell.h"
#include "assert.h"

#define DBG_TAG "MAIN"
#include "log.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct bflb_device_s *uart0;

static wifi_conf_t conf = {
    .country_code = "CN",
};

extern void shell_init_with_task(struct bflb_device_s *shell);

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/

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

void wifi_start_firmware_task(void *param)
{
    LOG_I("Starting wifi ...\r\n");

    /* set ble controller EM Size */

    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        vTaskDelete(NULL);
    }

    LOG_I("PHY RF init success!\r\n");

    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");

    fhost_init();

    vTaskDelete(NULL);
}

/**********************************************************
  * wifi event handler
 ***********************************************************/
void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_init(&conf);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            LOG_I("[SYS] Memory left is %d Bytes\r\n", kfree_size());
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

/**********************************************************
    lp enter callback func
 **********************************************************/
static int lp_enter(void *arg)
{
    return 0;
}

/***********************************************************
    shell
 ***********************************************************/
extern void uart_shell_isr();
extern struct bflb_device_s *uart_shell;
extern void vPortSetupTimerInterrupt(void);
static int lp_exit(void *arg)
{
    /* recovery system_clock_init\peripheral_clock_init\console_init*/
    board_recovery();

    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);

    board_rf_ctl(BRD_CTL_RF_RESET_DEFAULT, 0);

    vPortSetupTimerInterrupt();

    bflb_uart_rxint_mask(uart_shell, false);
    bflb_irq_attach(uart_shell->irq_num, uart_shell_isr, NULL);
    bflb_irq_enable(uart_shell->irq_num);

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

extern int enable_tickless;
extern bl_lp_fw_cfg_t lpfw_cfg;

static void cmd_tickless(int argc, char **argv)
{
#if 0
    uint32_t tmpVal;
    /* Set RTC compare mode, and enable rtc */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = tmpVal & 0xfffffff1;
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal | 0x00000003);
#endif

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
        printf("keep alive pck:%ld\r\n", pck_cnt);
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

    printf("Connect tcp server success.\r\n");

    /*---Get "Hello?"---*/
    memset(buffer, 'A', sizeof(buffer) - 1);


#ifdef LP_APP
    if (argc > 2) {
        bl_lp_fw_bcn_loss_cfg_dtim_default(lpfw_cfg.dtim_origin);

        printf("sta_ps %ld\r\n", wifi_mgmr_sta_ps_enter());
        enable_tickless = 1;
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
            enable_tickless = 0;
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

static void cmd_hbn_test(int argc, char **argv)
{
    if (argc <= 5) {
        bl_lp_hbn_fw_cfg_t hbn_test_cfg = {
            .hbn_sleep_cnt = 32768 * 5,
            .hbn_level = 0,
        };
        bl_lp_hbn_enter(&hbn_test_cfg);
    } else {
        if ((argv[1] != NULL) && (argv[2] != NULL) && (argv[3] != NULL)) {
            printf("wdt_pin %d\r\n", atoi(argv[1]));
            printf("max_continue_times %d\r\n", atoi(argv[3]));
            bl_lp_hbn_init(1, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
        }

        if ((argv[4] != NULL) && (argv[5] != NULL)) {
            printf("hbn sleep %ds\r\n", atoi(argv[4]));
            printf("hbn_level %d\r\n", atoi(argv[5]));
            vTaskDelay(1);
            bl_lp_hbn_fw_cfg_t hbn_test_cfg = {
                .hbn_sleep_cnt = 32768 * atoi(argv[4]),
                .hbn_level = atoi(argv[5]),
            };
            bl_lp_hbn_enter(&hbn_test_cfg);
        }
    }
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

TimerHandle_t xArpTimer = NULL;
static void arp_send(TimerHandle_t xTimer) {

    if (!wifi_mgmr_sta_state_get()) {
        return;
    }

    LOCK_TCPIP_CORE();
    do {
        assert(netif_default != NULL);
        etharp_request(netif_default, &netif_default->gw);
    } while(0);
    UNLOCK_TCPIP_CORE();
}

static void cmd_send_arp(int argc, char **argv)
{
    if (argc != 2) {
    printf("Need param\r\n");
    return;
    }

    if (atoi(argv[1])) {
        if (xArpTimer) {
            printf("Arp timer already created.\r\n");
            return;
        }
        xArpTimer = xTimerCreate("traffic probe",  pdMS_TO_TICKS(55*1000), pdFALSE, (void*)0, arp_send);

        xTimerStart(xArpTimer, 0);
        printf("create period 55s arp timer success.\r\n");
    } else {
        if (xArpTimer) {
            xTimerDelete(xArpTimer, portMAX_DELAY);
            xArpTimer = NULL;
            printf("Delete arp timer.\r\n");
        }
    }

    return;

}

SHELL_CMD_EXPORT_ALIAS(cmd_tickless, tickless, cmd tickless);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_lp, wifi_lp_test, wifi low power test);
SHELL_CMD_EXPORT_ALIAS(test_tcp_keepalive, lpfw_tcp_keepalive, tcp keepalive test);
SHELL_CMD_EXPORT_ALIAS(cmd_hbn_test, hbn_test, hbn test);
SHELL_CMD_EXPORT_ALIAS(cmd_io_dbg, io_debug, cmd io_debug);
SHELL_CMD_EXPORT_ALIAS(cmd_send_arp, arp_send, cmd send arp);
#endif

/**********************************************************
    proc_hellow_entry task func
 **********************************************************/
#if 0
static void proc_hellow_entry(void *pvParameters)
{
    vTaskDelay(500);

    while (1) {
        printf("%s: RISC-V rv64imafc\r\n", __func__);

#ifdef LP_APP
        bl_lp_info_t lp_info;
        /* get lp info */
        bl_lp_info_get(&lp_info);
        /* clear lp info */
        bl_lp_info_clear();

        printf("\r\nVirtual time: %llu us\r\n", bl_lp_get_virtual_us());
        printf("LowPower info dump:\r\n");
        printf("LPFW try recv bcn: %d, loss %d\r\n", lp_info.lpfw_recv_cnt, lp_info.lpfw_loss_cnt);
        printf("Total time %lldms\r\n", lp_info.time_total_us / 1000);
        printf("PDS sleep: %lldms\r\n", lp_info.sleep_pds_us / 1000);
        printf("LPFW active: %lldms\r\n", lp_info.active_lpfw_us / 1000);
        printf("APP active: %lldms\r\n", lp_info.active_app_us / 1000);

        uint64_t current = (lp_info.sleep_pds_us * 40 + lp_info.active_lpfw_us * 40000 + lp_info.active_app_us * 65000) / lp_info.time_total_us;
        printf("Predict current: %llduA\r\n", current);
#endif

        vTaskDelay(10000);
    }
    vTaskDelete(NULL);
}
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

void tcpip_init_done(void *arg)
{
}

int main(void)
{
    uint8_t soc_v, rt_v, aon_v;

    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    tcpip_init(tcpip_init_done, NULL);

    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

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

    ci_pm_test_init();

#if 1
    /* coarse trim rc32k */
    puts("[OS] Create rc32k_coarse_trim task...\r\n");
    xTaskCreate(rc32k_coarse_trim_task, (char*)"rc32k_coarse_trim", 512, NULL, 11, &rc32k_coarse_trim_task_hd);
#endif

#if 1
    /* auto check xtal32k, only test */
    puts("[OS] Create xtal32k_check_entry task...\r\n");
    xTaskCreate(xtal32k_check_entry_task, (char*)"xtal32k_check_entry", 512, NULL, 10, &xtal32k_check_entry_task_hd);
#endif

#if 0
    printf("[OS] Starting proc_hellow_entry task...\r\n");
    xTaskCreate(proc_hellow_entry, (char*)"hellow", 512, NULL, 10, NULL);
#endif

#if 0
 xTaskCreate(usage_task, (char *)"usage_task", 512, NULL, configMAX_PRIORITIES - 6, &usage_handle);
#endif

    vTaskStartScheduler();

    while (1) {
    }
}
