#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "example_log.h"
#include "static_inline.h"

BFLOG_DEFINE_TAG(TX, "tx", false);

#undef BFLOG_TAG
#define BFLOG_TAG BFLOG_GET_TAG(TX)

TaskHandle_t producer_handle;

extern QueueHandle_t queue;

void producer_task(void *pvParameters)
{
    uint8_t time = 100;

    LOG_I("Producer task enter \r\n");
    vTaskDelay(1000);
    LOG_I("Producer task start \r\n");

    uint32_t count = 0;

    while (1) {
        time = rand() % 255;

        count++;
        LOG_I("Producer send >> %d\r\n", count);
        LOG_D("Producer send >> %d\r\n", count);
        LOG_T("Producer send >> %d\r\n", count);
        test_log_static_inline();
        if (pdTRUE != xQueueSend(queue, (void *)&count, 1000)) {
            LOG_E("queue send faild\r\n");
        }

        vTaskDelay(time);
    }

    vTaskDelete(NULL);
}