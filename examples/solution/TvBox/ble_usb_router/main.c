#include "bflb_mtimer.h"
#include "bflb_mtd.h"
#include "board.h"
#include "rfparam_adapter.h"
#include "usbd_core.h"

#include "FreeRTOS.h"
#include "task.h"

#include "usb_hid_bridge.h"
#include "ble_common.h"
#include "usb_composite.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define DBG_TAG "MAIN"
#include "log.h"

static void start_main(void *param)
{
    (void)param;

#ifdef CONFIG_SHELL
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);
    LOG_I("Shell ready\r\n");
#endif

    usb_hid_bridge_init();

    LOG_I("Start BLE communication channel\r\n");
    int ble_ret = ble_common_stack_init();
    if (ble_ret) {
        LOG_E("BLE startup request failed: %d\r\n", ble_ret);
    }
#if defined(BL616)
    /* Controller PDS reset also resets USB PHY; initialize it before USB. */
    if (ble_ret) {
        LOG_E("Controller startup incomplete; USB not started, reboot required\r\n");
        vTaskDelete(NULL);
        return;
    }
#endif

    LOG_I("Start USB ECM + UAC composite device\r\n");
    usb_composite_start();

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    bflb_mtd_init();

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_E("PHY RF init failed\r\n");
        return 0;
    }

#if defined(BOARD_USB_VIA_GPIO)
    board_usb_gpio_init();
#endif

    xTaskCreate(start_main, (char *)"start_task", 2048, NULL, configMAX_PRIORITIES - 1, NULL);

    LOG_I("[OS] start scheduler\r\n");
    vTaskStartScheduler();

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
