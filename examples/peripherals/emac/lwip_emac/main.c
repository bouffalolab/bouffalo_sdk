#include "FreeRTOS.h"
#include "task.h"

#include "board.h"
#include "shell.h"

#include "http_server.h"
#include "lwip_emac_start.h"

#define DBG_TAG "MAIN"
#include "log.h"

static void app_start_task(void *pvParameters)
{
    struct bflb_device_s *uart0;

    (void)pvParameters;

    LOG_I("app_start_task Run...\r\n");
    vTaskDelay(10);

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);
    LOG_I("Shell Ready...\r\n");

    /* Kconfig defaults: port count from CONFIG_EMAC_DEV_COUNT, per-port
     * RMII/MDIO ids from CONFIG_EMAC0_* / CONFIG_EMAC1_*. */
    if (lwip_emac_start(NULL, 0) < 0) {
        LOG_E("lwip_emac_start failed\r\n");
        vTaskDelete(NULL);
        return;
    }

    vTaskDelay(10);

    if (http_server_init() < 0) {
        LOG_E("http_server_init failed\r\n");
        vTaskDelete(NULL);
        return;
    }

    LOG_I("TCP/UDP services are ready for Shell start commands.\r\n");

    LOG_I("app_start_task Delete...\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    printf("EMAC lwIP unified TCP/UDP/HTTP demo\r\n");

    LOG_I("Create app_start task.\r\n");
    xTaskCreate(app_start_task, (char *)"app_start_task", 512, NULL, configMAX_PRIORITIES - 1, NULL);

    LOG_I("Start Scheduler.\r\n");
    vTaskStartScheduler();

    LOG_E("vTaskStart failed.\r\n");

    while (1) {
    }
}
