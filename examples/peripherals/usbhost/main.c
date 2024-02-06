#include <FreeRTOS.h>
#include "semphr.h"
#include "usbh_core.h"
#include "bflb_uart.h"
#include "board.h"
#include "shell.h"

extern void usbh_class_test(void);

static struct bflb_device_s *uart0;

extern void shell_init_with_task(struct bflb_device_s *shell);

int main(void)
{
    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    printf("Starting usb host task...\r\n");
    usbh_initialize();
    usbh_class_test();
    vTaskStartScheduler();

    while (1) {
    }
}

int usbh_deinit(int argc, char **argv)
{
    printf("usbh_deinit\r\n");
    usbh_deinitialize();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(usbh_deinit, usbh_deinit, usbh deinit);

int usbh_init(int argc, char **argv)
{
    printf("usbh_init\r\n");
    usbh_initialize();
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(usbh_init, usbh_init, usbh init);

SHELL_CMD_EXPORT_ALIAS(lsusb, lsusb, ls usb);