/**
 * @file main.c
 * @brief CherryUSB ADB device demo entry point.
 */

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"

#include "adb_demo.h"

#define DBG_TAG "MAIN"
#include "log.h"

static TaskHandle_t start_handle;

/**
 * @brief Start the ADB demo once the scheduler is running.
 * @param[in] param Unused FreeRTOS task parameter.
 */
static void start_main(void *param)
{
    (void)param;

    LOG_I("[OS] start_main task run\r\n");

    adb_demo_init();

    vTaskDelete(NULL);
}

/**
 * @brief Initialize the board and start the ADB demo scheduler.
 * @return This function does not return after the scheduler starts.
 */
int main(void)
{
    board_init();

    LOG_I("CherryUSB ADB device demo\r\n");

#if defined(BOARD_USB_VIA_GPIO)
    board_usb_gpio_init();
#endif

    xTaskCreate(start_main, (char *)"start_task", 1024, NULL, configMAX_PRIORITIES - 2, &start_handle);

    LOG_I("[OS] start scheduler\r\n");
    vTaskStartScheduler();

    while (1) {
    }
}
