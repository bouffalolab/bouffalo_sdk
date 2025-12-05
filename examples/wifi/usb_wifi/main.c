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

#define INIT_STACK_SIZE    (2048)
#define INIT_TASK_PRIORITY (16)

static struct bflb_device_s *uart0;
extern void shell_init_with_task(struct bflb_device_s *shell);

void app_init_entry(void *param)
{
    app_usbwifi_init();

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

#ifdef LP_APP
    app_pm_init();
#endif

#ifdef BOARD_USB_VIA_GPIO
    board_usb_gpio_init();
#endif

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    xTaskCreate(app_init_entry, (char *)"init", INIT_STACK_SIZE, NULL, INIT_TASK_PRIORITY, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}

