#include "shell.h"
#include <FreeRTOS.h>
#include "task.h"
#include "board.h"

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "ble_lib_api.h"
#include "bl602_glb.h"
#include "rfparam_adapter.h"

#include "ble_cli_cmds.h"
#include "hci_driver.h"
#include "hci_core.h"

#include "bflb_mtd.h"
#include "easyflash.h"

#include "wifi_mgmr_ext.h"

#define DBG_TAG "MAIN"
#include "log.h"
#include "async_event.h"

static struct bflb_device_s *uart0;

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

extern void shell_init_with_task(struct bflb_device_s *shell);
extern void wifi_event_handler(async_input_event_t ev, void *priv);

static void ble_connected(struct bt_conn *conn, u8_t err)
{
    if(err || conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }
    printf("%s",__func__);
}

static void ble_disconnected(struct bt_conn *conn, u8_t reason)
{
    int ret;

    if(conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }

    printf("%s",__func__);

    // enable adv
    ret = set_adv_enable(true);
    if(ret) {
        printf("Restart adv fail. \r\n");
    }
}

static struct bt_conn_cb ble_conn_callbacks = {
	.connected	=   ble_connected,
	.disconnected	=   ble_disconnected,
};

void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
               bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);
        bt_conn_cb_register(&ble_conn_callbacks);
        ble_cli_register();
    }
}

static void ble_start_task_entry(void *pvParameters)
{
    // Initialize BLE controller
    #if defined(BL702) || defined(BL602)
    ble_controller_init(configMAX_PRIORITIES - 1);
    #else
    btble_controller_init(configMAX_PRIORITIES - 1);
    #endif
    // Initialize BLE Host stack
    hci_driver_init();
    bt_enable(bt_enable_cb);

    vTaskDelete(NULL);
}

static void ble_start_task(void)
{
    static TaskHandle_t app_start_handle;
    xTaskCreate(ble_start_task_entry, (char *)"ble_start", 1024, NULL, configMAX_PRIORITIES - 2, &app_start_handle);
}

void wifi_start_firmware_task(void *param)
{
    LOG_I("Starting wifi ...\r\n");

    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);


    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    vTaskDelete(NULL);
}

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

int main(void)
{
    board_init();

    configASSERT((configMAX_PRIORITIES > 4));

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    bflb_mtd_init();
    /* ble stack need easyflash kv */
    easyflash_init();

    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }

    tcpip_init(NULL, NULL);
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);
    ble_start_task();

    vTaskStartScheduler();

    while (1) {
    }
}
