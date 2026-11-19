#include "bflb_uart.h"
#include "shell.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "board.h"


#include "aot_store.h"

#if defined(CONFIG_WL80211)
#include "async_event.h"
#include "lwip/tcpip.h"
#include "rfparam_adapter.h"
#include "task.h"
#include "timers.h"
#include "wifi_mgmr.h"
#include "wl80211.h"

extern void wifi_task_create(void);
#endif

extern void shell_init_with_task(struct bflb_device_s *shell);

#if defined(CONFIG_WL80211)
static void async_event_handler(void *arg1, uint32_t arg2)
{
    (void)arg1;
    (void)arg2;
    vTaskSuspendAll();
    async_event_loop();
    xTaskResumeAll();
}

static void async_event_loop_wake(void)
{
    BaseType_t ret;
    TickType_t wait = portMAX_DELAY;

    if (xTimerGetTimerDaemonTaskHandle() == xTaskGetCurrentTaskHandle())
        wait = 0;
    ret = xTimerPendFunctionCall(async_event_handler, NULL, 0, wait);
    configASSERT(ret == pdPASS);
}

static void wifi_start_firmware_task(void *param)
{
    (void)param;
    wifi_task_create();
    vTaskDelay(pdMS_TO_TICKS(500));
    tcpip_init(NULL, NULL);
    wl80211_init();
    wifi_mgmr_init();
    vTaskDelete(NULL);
}
#endif

int main(void)
{
    struct bflb_device_s *uart0;

    board_init();
    configASSERT(configMAX_PRIORITIES > 4);

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

#if defined(CONFIG_WL80211)
    if (rfparam_init(0, NULL, 0) != 0) {
        printf("WiFi: PHY RF init failed\r\n");
    } else {
        async_event_init(async_event_loop_wake);
        xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);
    }
#endif

#if defined(CONFIG_WAMR_AOT)
    /* Probe only after the scheduler has started; the first AOT command mounts
     * LittleFS lazily so WiFi startup is not competing for early heap. */
#endif

    vTaskStartScheduler();

    while (1) {
    }
}
