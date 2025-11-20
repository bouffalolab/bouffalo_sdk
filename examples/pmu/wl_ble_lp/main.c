#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "mem.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"
#include "fhost_api.h"

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

#include "board.h"
#include "board_rf.h"
#include "shell.h"

#include "bflb_mtd.h"
#include "easyflash.h"
#include "clock_manager.h"
#include "pm_manager.h"

#define DBG_TAG "MAIN"
#include "log.h"

#if defined(CFG_BLE_ENABLE)
#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "btble_lib_api.h"
#include "bl616_glb.h"
#include "hci_driver.h"
#include "hci_core.h"
#endif

//#include "bflb_mtd.h"
//#include "easyflash.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

#define PM_MEM_POOL_SIZE    (1460 *2)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
static struct bflb_device_s *uart0;

TaskHandle_t wifi_fw_task;
static TaskHandle_t app_start_handle;
struct bt_conn *bleapp_default_conn;

static wifi_conf_t conf = {
    .country_code = "US",
};
#if defined(CFG_BLE_ENABLE)
static void ble_connected(struct bt_conn *conn, u8_t err)
{
    if(err || conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }
    if (!bleapp_default_conn) {
        bleapp_default_conn = conn;
    }
}

static void ble_disconnected(struct bt_conn *conn, u8_t reason)
{
    int ret;
    if(conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }
    if (bleapp_default_conn == conn) {
        bleapp_default_conn = NULL;
    }
    printf("%s",__func__);
}
static void ble_conn_param_updated(struct bt_conn *conn, u16_t interval,
			     u16_t latency, u16_t timeout)
{
    if(conn == bleapp_default_conn)
    {
        printf("%s: int 0x%04x lat %d to %d \r\n", __func__, interval, latency, timeout);
    }
}
static struct bt_conn_cb ble_conn_callbacks = {
    .connected  =   ble_connected,
    .disconnected   =   ble_disconnected,
    .le_param_updated = ble_conn_param_updated,
};

void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
                bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);
        bt_conn_cb_register(&ble_conn_callbacks);
        //ble_cli_register();
    }
}
#endif
static void app_start_task(void *pvParameters)
{
    app_clock_init();
#if defined(CFG_BLE_ENABLE)
    btble_controller_init(configMAX_PRIORITIES - 1);
    hci_driver_init();
    bt_enable(bt_enable_cb);
#endif
    vTaskDelete(NULL);
}

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

int wifi_start_firmware_task(void)
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

    vTaskDelay(500);

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

GLB_GPIO_Type pinList[4] = {
    GLB_GPIO_PIN_0,
    GLB_GPIO_PIN_1,
    GLB_GPIO_PIN_2,
    GLB_GPIO_PIN_3,
};

static int lp_exit(void *arg)
{
    int wakeup_reason;
    extern TaskHandle_t rxl_process_task_hd;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

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

int bflb_pm_app_check(void)
{
    return pm_pbufc_check();
}

#ifdef CONFIG_SHELL
static void cmd_tickless(int argc, char **argv)
{	
    int broadcast = 0;

    if ((argc > 1) && (argv[1] != NULL)) {
        printf("%s\r\n", argv[1]);
        broadcast = atoi(argv[1]);
    } else {
        broadcast = 0;
    }

    if (broadcast) {
        enable_multicast_broadcast = 1;
    } else {
        enable_multicast_broadcast = 0;
    }

    pm_enable_tickless();
}

static void cmd_set_dtim(int argc, char **argv)
{	
	int dtim = 10;
    int broadcast = 0;

    if ((argc > 1) && (argv[1] != NULL)) {
        printf("%s\r\n", argv[1]);
        dtim = atoi(argv[1]);
    } else {
        dtim = 10;
    }
    
    set_dtim_config(dtim);
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

static void cmd_set_clock_source(int argc, char **argv)
{
    uint8_t source;

    // Check argument count
    if (argc != 2) {
        printf("Usage: set_clock_source <source>\r\n");
        printf("  1: Internal RC oscillator\r\n");
        printf("  2: External passive crystal\r\n");
        printf("  3: External active crystal\r\n");
        return;
    }

    // Parse source value from argument
    source = (uint8_t)atoi(argv[1]);

    // Validate source value
    if (source < 1 || source > 3) {
        printf("Error: Invalid clock source value. Must be 1, 2, or 3.\r\n");
        return;
    }

    // Call the API function to set clock source
    int ret = app_set_clock_source(source);
    if (ret == 0) {
        printf("Clock source updated to %d.\r\n", source);
        // The system should reboot automatically after this
    } else {
        printf("Failed to set clock source, error code: %d\r\n", ret);
    }
}

static void cmd_get_clock_source(int argc, char **argv)
{
    uint8_t source;
    int ret;

    // Call the API function to get current clock source
    ret = app_get_clock_source(&source);

    if (ret == 0) {
        printf("Current clock source: %d (", source);

        // Print descriptive name of the clock source
        switch (source) {
            case 1:
                printf("Internal RC oscillator");
                break;
            case 2:
                printf("External passive crystal");
                break;
            case 3:
                printf("External active crystal");
                break;
            default:
                printf("Unknown");
                break;
        }
        printf(")\r\n");
    } else {
        printf("Failed to get clock source, error code: %d\r\n", ret);
        printf("Using default source: Internal RC oscillator\r\n");
    }
}

SHELL_CMD_EXPORT_ALIAS(cmd_tickless, pm_enter_lp, cmd tickless);
SHELL_CMD_EXPORT_ALIAS(cmd_set_dtim, wifi_lp_set_dtim, cmd_set_dtim);
SHELL_CMD_EXPORT_ALIAS(test_tcp_keepalive, lpfw_tcp_keepalive, tcp keepalive test);
SHELL_CMD_EXPORT_ALIAS(cmd_hbn_test, hbn_test, hbn test);
SHELL_CMD_EXPORT_ALIAS(cmd_io_dbg, io_debug, cmd io_debug);
SHELL_CMD_EXPORT_ALIAS(cmd_set_clock_source, set_clock_source, Set system clock source (1:RC, 2:Passive XTAL, 3:Active XTAL));
SHELL_CMD_EXPORT_ALIAS(cmd_get_clock_source, get_clock_source, Get current system clock source);
#endif

/**********************************************************
    proc_hellow_entry task func
 **********************************************************/
#if 0
static void proc_hellow_entry(void *pvParameters)
{
    vTaskDelay(500);

    while (1) {
        printf("task run.\r\n");
        vTaskDelay(5000);
    }
    vTaskDelete(NULL);
}
#endif

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
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 27, NULL);

    hal_pm_ldo11_cfg(PM_PDS_LDO_LEVEL_SOC_DEFAULT, PM_PDS_LDO_LEVEL_RT_DEFAULT, PM_PDS_LDO_LEVEL_AON_DEFAULT);
    hal_pm_ldo11_cfg_get(&soc_v, &rt_v, &aon_v);
    printf("SOC:%d RT:%d AON:%d\r\n", soc_v, rt_v, aon_v);

    HBN_Enable_RTC_Counter();
    pm_rc32k_auto_cal_init();

    bflb_mtd_init();
    easyflash_init();

    pm_sys_init();

#ifdef LP_APP
    bl_lp_init(); //wifi lowpower
    bl_lp_sys_callback_register(lp_enter, NULL, lp_exit, NULL);
#endif
    app_set_clock_source(CLOCK_SOURCE_PASSIVE);

#if 0
    printf("[OS] Starting proc_hellow_entry task...\r\n");
    xTaskCreate(proc_hellow_entry, (char*)"hellow", 512, NULL, 10, NULL);
#endif

    xTaskCreate(app_start_task, (char *)"app_start", 1024, NULL, 15, &app_start_handle);
    
    vTaskStartScheduler();

    while (1) {
    }
}
