#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

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
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"

#ifdef BL616
#include "bl616_glb.h"
#include "bl616_pm.h"
#include "bl616_hbn.h"
#elif defined(BL616CL)
#include "bl616cl_pm.h"
#include "bl616cl_hbn.h"
#elif defined(BL618DG)
#include "bl618dg_pm.h"
#include "bl618dg_hbn.h"
#endif

#include "bflb_rtc.h"

#include "rfparam_adapter.h"

#include "wl_lp_app.h"

#include "board.h"
#include "board_rf.h"
#include "shell.h"
#include "assert.h"

#include "pm_manager.h"
#include "clock_manager.h"

#define DBG_TAG "MAIN"
#include "log.h"
#include "async_event.h"

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

extern void shell_init_with_task(struct bflb_device_s *shell);
extern void wifi_event_handler(async_input_event_t ev, void *priv);

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
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

#if defined(BL616)
    /* set ble controller EM Size */
    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
#endif

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        vTaskDelete(NULL);
    }

    LOG_I("PHY RF init success!\r\n");

    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);


    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");

    fhost_init();

    vTaskDelete(NULL);
}

/**********************************************************
  * wifi event handler
 ***********************************************************/
void wifi_event_handler(async_input_event_t ev, void *priv)
{
    uint32_t code = ev->code;

    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_task_start();
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
            LOG_I("[SYS] Memory left is %d Bytes\r\n", kfree_size(0));
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
    int wakeup_reason;
    extern TaskHandle_t rxl_process_task_hd;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* recovery system_clock_init\peripheral_clock_init\console_init*/
    board_recovery();

#if defined(BL616)
    /* set ble controller EM Size */
    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
#endif

    board_rf_ctl(BRD_CTL_RF_RESET_DEFAULT, 0);

    vPortSetupTimerInterrupt();

    bflb_uart_rxint_mask(uart_shell, false);
    bflb_irq_attach(uart_shell->irq_num, uart_shell_isr, NULL);
    bflb_irq_enable(uart_shell->irq_num);

    wakeup_reason = bl_lp_get_wake_reason();
    if (wakeup_reason & LPFW_WAKEUP_WIFI_BROADCAST) {
        vTaskNotifyGiveFromISR(rxl_process_task_hd, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        pm_alloc_mem_reset();
    }

    return 0;
}

#ifdef CONFIG_SHELL
// int cmd_wifi_lp(int argc, char **argv)
// {
//     int ret = 0;
//     printf("enter wireless low power!\r\n");

//     bl_lp_init();
//     // bl_lp_fw_init();
//     bl_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);

//     while (1) {
//         // lp_exit(0);
//         ret = bl_lp_fw_enter(&lpfw_cfg);
//         if (ret < 0) {
//             printf("[E]bflb_lpfw_enter Fail,ErrId:%d\r\n", ret);
//         } else {
//             printf("bflb_lpfw_enter Success\r\n");
//         }
//         arch_delay_ms(1000);
//     }

//     return 0;
// }

extern int enable_tickless;
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

static void cmd_hbn_test(int argc, char **argv)
{
}

static void cmd_io_dbg(int argc, char **argv)
{
    iot2lp_para_t *iot2lp_para = (iot2lp_para_t *)IOT2LP_PARA_ADDR;

    if (argc != 2) {
        printf("cmd_io_dbg err\r\n");
        return;
    }

    if (atoi(argv[1]) <= 34) {
#ifdef BL616
        iot2lp_para->debug_io = atoi(argv[1]);
#else
        iot2lp_para->wifi_debug_io = atoi(argv[1]);
#endif
    } else {
#ifdef BL616
        iot2lp_para->debug_io = 0xFF;
#else
        iot2lp_para->wifi_debug_io = 0xFF;
#endif
    }
}

TimerHandle_t xArpTimer = NULL;
static void arp_send(TimerHandle_t xTimer)
{
    if (!wifi_mgmr_sta_state_get()) {
        return;
    }

    LOCK_TCPIP_CORE();
    do {
        assert(netif_default != NULL);
        etharp_request(netif_default, &netif_default->gw);
    } while (0);
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
        xArpTimer = xTimerCreate("traffic probe", pdMS_TO_TICKS(55 * 1000), pdFALSE, (void *)0, arp_send);

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

#if !defined(BL616)
static void cmd_lpfw_uart_cfg(int argc, char **argv)
{
    if (argc != 5) {
        printf("Need param\r\n");
        return;
    }

    iot2lp_para->uart_config->debug_log_en = atoi(argv[1]);
    iot2lp_para->uart_config->uart_tx_io = atoi(argv[2]);
    iot2lp_para->uart_config->uart_rx_io = atoi(argv[3]);
    iot2lp_para->uart_config->baudrate = atoi(argv[4]);
}

static void cmd_lpfw_clock_cfg(int argc, char **argv)
{
    if (argc != 5) {
        printf("Need param\r\n");
        printf("mcu_clk_sel:\n\t0:GLB_MCU_SYS_CLK_RC32M\r");
        printf("\n\t1:GLB_MCU_SYS_CLK_XTAL\r");
        printf("\n\t2:GLB_MCU_SYS_CLK_WIFIPLL_96M\r");
        printf("\n\t3:GLB_MCU_SYS_CLK_WIFIPLL_192M\r");
        printf("\n\t4:GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M\r");
        printf("\n\t5:GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M\r\n");

        printf("xclk_sel:\n\t0:HBN_MCU_XCLK_RC32M\r");
        printf("\n\t1:HBN_MCU_XCLK_XTAL\r");

        return;
    }

    iot2lp_para->clock_config->mcu_clk_sel = atoi(argv[1]);
    iot2lp_para->clock_config->hclk_div = atoi(argv[2]);
    iot2lp_para->clock_config->bclk_div = atoi(argv[3]);
    iot2lp_para->clock_config->xclk_sel = atoi(argv[4]);
}
#endif

SHELL_CMD_EXPORT_ALIAS(cmd_tickless, tickless, cmd tickless);
SHELL_CMD_EXPORT_ALIAS(cmd_hbn_test, hbn_test, hbn test);
SHELL_CMD_EXPORT_ALIAS(cmd_io_dbg, io_debug, cmd io_debug);
#if !defined(BL616)
SHELL_CMD_EXPORT_ALIAS(cmd_lpfw_uart_cfg, lpfw_uart, cmd lpfw_uart);
SHELL_CMD_EXPORT_ALIAS(cmd_lpfw_clock_cfg, lpfw_clock, cmd lpfw_clock);
#endif

#endif /* CONFIG_SHELL */

/**********************************************************
    proc_hellow_entry task func
 **********************************************************/
#if 0
static void proc_hellow_entry(void *pvParameters)
{
    vTaskDelay(100);

    while (1) {
        printf("%s: RISC-V rv64imafc\r\n", __func__);

        // #if (BL_LP_TIME_DEBUG)
        bl_lp_debug_dump_time(iot2lp_para);
        // #endif
        // 打印 RF_BASE 和 RF2_BASE 寄存器内容
        // print_rf_registers();
#ifdef LP_APP
        bl_lp_info_t lp_info;
        /* get lp info */
        bl_lp_info_get(&lp_info);
        /* clear lp info */
        bl_lp_info_clear(iot2lp_para->lp_info);

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
        vTaskDelay(60000);
    }
    vTaskDelete(NULL);
}
#endif

#if defined(BL616CL) || defined(BL618DG)
static struct bflb_device_s *rtc = NULL;

static TaskHandle_t rc32k_coarse_trim_task_hd = NULL;
static TaskHandle_t xtal32k_check_entry_task_hd = NULL;
extern int32_t rc32k_accuracy_ppm_calculate(uint32_t expect_time, uint32_t rc32k_actual_time);
extern bool rc32k_check_accuracy(int32_t actual_ppm, int32_t threshold_ppm);
extern void rc32k_get_trim_code(uint32_t* c_code, uint32_t* r_code);
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
    bool ret = false;

    uint32_t c_code, r_code = 0;
    uint32_t rc32k_code;
    int last_error_ppm = 0;
    int last_diff_code = 0;
    int first_measure = 1;

    /* hardware trim rc32k */
    HBN_Trim_RC32K();
    rc32k_get_trim_code(&c_code, &r_code);
    printf("c_code=%d,r_code=%d\r\n",c_code,r_code);

    /* select rtc clock */
    HBN_32K_Sel(HBN_32K_RC);
#if defined(BL616CL)
    HBN_Set_RTC_CLK_Sel(HBN_RTC_CLK_F32K);
#endif

    /* rtc start */
    rtc = bflb_device_get_by_name("rtc");
    bflb_rtc_set_time(rtc, 0);

    printf("rc32k_coarse_trim task enable, freq_mtimer must be 1MHz!\r\n");
    timeout_start = bflb_mtimer_get_time_us();

    vTaskDelay(20);

    while(retry_cnt < 100){
        retry_cnt++;

        /* disable irq */
        __disable_irq();

        mtimer_record_us = bflb_mtimer_get_time_us();
        HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);

        /* enable irq */
        __enable_irq();

        rtc_record_us = BL_PDS_CNT_TO_US(rtc_cnt);

        /* delay */
        vTaskDelay(100);

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
        error_ppm = rc32k_accuracy_ppm_calculate(mtimer_us, rtc_us);

        printf("rc32k_coarse_trim: mtimer_us:%d, rtc_us:%d\r\n", mtimer_us, rtc_us);

        ret = rc32k_check_accuracy(error_ppm,200);
        rc32k_get_trim_code(&c_code, &r_code);
        printf("rc32k_coarse_trim: retry_cnt:%d, ppm:%d, r_code=%d", retry_cnt, error_ppm,r_code);

        if (ret == true) {
            /* finish */
            printf(", finish!\r\n");
            break;
        }

        /* Improved coarse trim with dynamic step adjustment */
        if (abs(error_ppm) > 500) {
            int diff_code;
            int is_diverging = 0;

            /* Divergence detection: only when error absolute value increases */
            if (!first_measure && abs(error_ppm) > abs(last_error_ppm)) {
                if (abs(last_error_ppm) > 1) {
                    is_diverging = 1;
                }
            }

            if (is_diverging) {
                /* Diverging, rollback to last code and adjust by ±1 */
                printf(" (diverging, rollback)\r\n");

                rc32k_code = HBN_Get_RC32K_R_Code();
                rc32k_code = (int)rc32k_code - (int)last_diff_code;

                /* Rollback to last code and adjust by ±1 */
                if (last_error_ppm < 0) {
                    diff_code = -1;
                } else {
                    diff_code = 1;
                }
                rc32k_code += diff_code;
                HBN_Set_RC32K_R_Code(rc32k_code);

                printf("rc32k_coarse_trim: adjust code=%u (diff=%d)\r\n",
                    rc32k_code, diff_code);

                /* Diverging diff_code not Record */
                last_diff_code = diff_code;
                first_measure = 0;

                /* Wait for RC32K frequency to settle */
                vTaskDelay(10);

            } else {
                /* Normal calculation of adjustment code */
                /* For coarse trim, use larger step (800ppm per step) */
                diff_code = error_ppm / 800;

                /* Limit max adjustment to avoid jumping at fluctuation points */
                if (diff_code > 5) diff_code = 5;
                if (diff_code < -5) diff_code = -5;

                /* Ensure at least ±1 adjustment if error is significant */
                if (diff_code == 0 && abs(error_ppm) > 400) {
                    diff_code = (error_ppm > 0) ? 1 : -1;
                }

                rc32k_code = HBN_Get_RC32K_R_Code();
                rc32k_code += diff_code;

                HBN_Set_RC32K_R_Code(rc32k_code);

                printf(" (adjust code=%u, diff=%d)\r\n", rc32k_code, diff_code);

                /* Record current error for next divergence detection */
                last_error_ppm = error_ppm;
                last_diff_code = diff_code;
                first_measure = 0;

                /* Wait for RC32K frequency to settle */
                vTaskDelay(10);
            }
        } else {
            printf("\r\n");
            break;
        }
    }

    if (retry_cnt >= 100) {
        printf("rc32k_coarse_trim: timeout!\r\n");
    }

    printf("rc32k coarse trim success!, total time:%dms\r\n", (int)(bflb_mtimer_get_time_us() - timeout_start) / 1000);

    /* coarse_adj success */
    // if(xtal32k_check_entry_task_hd){
    //     /* resume xtal32k_check task */
    //     printf("rc32k_coarse_trim: Resume xtal32k_check task!\r\n");
    //     xTaskNotifyGive(xtal32k_check_entry_task_hd);
    // }else{
    //     /* set bl_lp 32k clock ready */
    //     printf("rc32k_coarse_trim: set lp_32k ready!\r\n");
        bl_lp_set_32k_clock_ready(1);
    // }

    // printf("rc32k_coarse_trim: rc32k code:%d\r\n", iot2lp_para->rc32k_fr_ext);

    printf("rc32k_coarse task: vTaskDelete\r\n");
    vTaskDelete(NULL);
}
#elif defined(BL616)
static void f32k_clk_init_task(void *pvParameters)
{
    LOG_I("F32K clock init task started\r\n");

    /* Initialize F32K clock */
    if (app_clock_init() != 0) {
        LOG_E("F32K clock initialization failed!\r\n");
    } else {
        LOG_I("F32K clock initialization success!\r\n");
    }

    /* Task done, delete itself */
    vTaskDelete(NULL);
}
#endif

void tcpip_init_done(void *arg)
{
}

int main(void)
{
    uint32_t tmpVal = 0;

    board_init();

#if defined(BL616CL)
    AON_Trim_Ldo18_IO_Vout();
    HBN_Trim_Ldo_Sys_Vout();
    HBN_Trim_Ldo_Aon_Vout();
    HBN_Trim_Ldo_Soc_Vout();

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_SW_LDO08AON_VOUT_TRIM_AON);
    printf("ldo08aon value:%d\r\n",tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_LDO_SOC_VOUT_SEL_AON);
    printf("ldo08soc value:%d\r\n",tmpVal);

    // HBN_SW_Trim_Ldo_Aon_Vout();
    HBN_SW_Set_Ldo_Soc_Vout(HBN_LDO_SOC_LEVEL_0P900V);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_LDO_SOC_VOUT_TRIM_AON);
    printf("ldo08soc value:%d\r\n",tmpVal);
#endif

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    tcpip_init(tcpip_init_done, NULL);

    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

    HBN_Enable_RTC_Counter();
    // pm_rc32k_auto_cal_init();

    pm_sys_init();

    bl_lp_init();

    bl_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);
    // ci_pm_test_init();
#if defined(BL616CL) || defined(BL618DG)
    /* coarse trim rc32k */
    puts("[OS] Create rc32k_coarse_trim task...\r\n");
    xTaskCreate(rc32k_coarse_trim_task, (char *)"rc32k_coarse_trim", 512, NULL, 11, &rc32k_coarse_trim_task_hd);
#elif defined(BL616)
    /* Create F32K clock init task */
    printf("[OS] Create f32k_clk_init task...\r\n");
    xTaskCreate(f32k_clk_init_task, (char *)"f32k_clk_init", 1024, NULL, 12, NULL);
#endif

#if 0
    printf("[OS] Starting proc_hellow_entry task...\r\n");
    xTaskCreate(proc_hellow_entry, (char *)"hellow", 512, NULL, 10, NULL);
#endif

#if 0
 xTaskCreate(usage_task, (char *)"usage_task", 512, NULL, configMAX_PRIORITIES - 6, &usage_handle);
#endif

    vTaskStartScheduler();

    while (1) {}
}
