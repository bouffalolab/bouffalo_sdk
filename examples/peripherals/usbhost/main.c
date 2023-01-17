#include <FreeRTOS.h>
#include "semphr.h"
#include "usbh_core.h"
#include "board.h"

extern void usbh_class_test(void);

int main(void)
{
    board_init();

    printf("Starting usb host task...\r\n");
    usbh_initialize();
    usbh_class_test();
    vTaskStartScheduler();

    while (1) {
    }
}
