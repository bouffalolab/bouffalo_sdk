/**
 * @file main.c
 * @brief CherryUSB device and host demonstration application entry point.
 */

#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

#include "usbd_core.h"

#include "FreeRTOS.h"
#include "task.h"

#ifdef CONFIG_LWIP
#include "lwip/opt.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#endif

#define DBG_TAG "MAIN"
#include "log.h"

static TaskHandle_t start_handle;

/**
 * @brief Initialize the command shell and optional lwIP stack.
 *
 * @param[in] param Unused FreeRTOS task parameter.
 * @note The task deletes itself after initialization is complete.
 */
void start_main(void *param)
{
    LOG_I("[OS] start_main task run\r\n");

    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    void shell_init_with_task(struct bflb_device_s * shell);
    shell_init_with_task(uart0);
    LOG_I("Shell ready\r\n");

#ifdef CONFIG_LWIP
    LOG_I("lwip statck init\r\n");
    /* Initialize the LwIP stack */
    tcpip_init(NULL, NULL);
#endif

    vTaskDelete(NULL);
}

/**
 * @brief Initialize the board and start the CherryUSB demonstration scheduler.
 *
 * @return This function does not return after the scheduler starts.
 */
int main(void)
{
    board_init();

    LOG_I("CherryUSB Device/Host demo, CherryUSB version: %s (0x%X)\r\n", CHERRYUSB_VERSION_STR, CHERRYUSB_VERSION);

#if defined(BOARD_USB_VIA_GPIO)
    board_usb_gpio_init();
#endif

    xTaskCreate(start_main, (char *)"start_task", 1024, NULL, configMAX_PRIORITIES - 1, &start_handle);

    LOG_I("[OS] start scheduler\r\n");
    vTaskStartScheduler();
}
