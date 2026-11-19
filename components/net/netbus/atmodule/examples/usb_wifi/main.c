#include <stdlib.h>
#include "async_event.h"
#include "wifi_mgmr_ext.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include <board.h>
#include <board_gpio.h>
#include <board_rf.h>
#include <shell.h>

#include "bflb_core.h"
#include "bflb_mtimer.h"
#include "bflb_uart.h"

#include <app_usbwifi.h>

#define INIT_STACK_SIZE    (2048)
#define INIT_TASK_PRIORITY (16)

static struct bflb_device_s *uart0;
extern void shell_init_with_task(struct bflb_device_s *shell);
extern void wifi_event_handler(async_input_event_t ev, void *priv);

void app_init_entry(void *param)
{
    if (app_usbwifi_init() != 0) {
        abort();
    }

    vTaskDelete(NULL);
}

int main(void)
{
    if (async_register_event_filter(EV_WIFI, wifi_event_handler, NULL) != 0) {
        abort();
    }

    if (xTaskCreate(app_init_entry, (char *)"init", INIT_STACK_SIZE, NULL, INIT_TASK_PRIORITY, NULL) != pdPASS) {
        abort();
    }

#ifdef CONFIG_LPAPP
    app_pm_init();
#endif

#ifdef BOARD_USB_VIA_GPIO
    board_usb_gpio_init();
#endif

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    vTaskStartScheduler();

    while (1) {
    }
}

