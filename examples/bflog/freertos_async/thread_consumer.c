#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "example_log.h"
#include "static_inline.h"

BFLOG_DEFINE_TAG(RX, "rx", false);

#undef BFLOG_TAG
#define BFLOG_TAG BFLOG_GET_TAG(RX)

TaskHandle_t consumer_handle;

extern QueueHandle_t queue;

void consumer_task(void *pvParameters)
{
    uint8_t time = 200;

    LOG_I("Consumer task enter \r\n");
    vTaskDelay(1000);
    LOG_I("Consumer task start \r\n");

    uint32_t count;

    while (1) {
        time += 20;
        time %= 64;
        if (pdTRUE != xQueueReceive(queue, &count, portMAX_DELAY)) {
            LOG_E("queue recv faild\r\n");
        } else {
            LOG_I("Consumer recv << %d\r\n", count);
            LOG_D("Consumer recv << %d\r\n", count);
            LOG_T("Consumer recv << %d\r\n", count);
            test_log_static_inline();
        }

        vTaskDelay(time);
    }

    vTaskDelete(NULL);
}