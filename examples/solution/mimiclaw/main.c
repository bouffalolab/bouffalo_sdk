#include "FreeRTOS.h"
#include "task.h"

#include <lwip/tcpip.h>

#include "fhost_api.h"
#include "wifi_mgmr_ext.h"

#include "bflb_uart.h"

#include "rfparam_adapter.h"

#include "board.h"
#include "shell.h"

#include <easyflash.h>
#include <bflb_mtd.h>

#include "async_event.h"

#include "mimiclaw_app.h"

#define DBG_TAG "MIMICLAW_MAIN"
#include "log.h"

static struct bflb_device_s *uart0;

static wifi_conf_t conf = {
    .country_code = "CN",
};

extern void shell_init_with_task(struct bflb_device_s *shell);

static void wifi_event_handler(async_input_event_t ev, void *priv)
{
    (void)priv;

    switch (ev->code) {
        case CODE_WIFI_ON_INIT_DONE:
            LOG_I("wifi init done\r\n");
            wifi_mgmr_init(&conf);
            break;
        case CODE_WIFI_ON_MGMR_DONE:
            LOG_I("wifi manager ready\r\n");
            break;
        case CODE_WIFI_ON_CONNECTED:
            LOG_I("wifi connected\r\n");
            break;
        case CODE_WIFI_ON_GOT_IP:
            LOG_I("wifi got ip\r\n");
            mimiclaw_app_on_network_ready();
            mimiclaw_app_start();
            break;
        case CODE_WIFI_ON_DISCONNECT:
            LOG_W("wifi disconnected\r\n");
            mimiclaw_app_on_network_lost();
            break;
        default:
            break;
    }
}

static void wifi_start_firmware_task(void *param)
{
    (void)param;

    LOG_I("Starting wifi firmware task\r\n");

    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    wifi_task_create();
    fhost_init();

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    if (rfparam_init(0, NULL, 0) != 0) {
        LOG_E("PHY RF init failed\r\n");
        return 0;
    }

    bflb_mtd_init();
    easyflash_init();

    tcpip_init(NULL, NULL);

    mimiclaw_app_init();

    xTaskCreate(wifi_start_firmware_task, "wifi_init", 1024, NULL, 10, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}
