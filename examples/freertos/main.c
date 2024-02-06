#include <FreeRTOS.h>
#include "semphr.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

static TaskHandle_t consumer_handle;
static TaskHandle_t producer_handle;

uint8_t sharedBuf[16];
SemaphoreHandle_t sem_empty = NULL;
SemaphoreHandle_t sem_full = NULL;
SemaphoreHandle_t mtx_lock = NULL;

static void consumer_task(void *pvParameters)
{
    LOG_I("Consumer task enter \r\n");
    vTaskDelay(1000);
    LOG_I("Consumer task start \r\n");
    LOG_I("begin to loop %s\r\n", __FILE__);

    while (1) {
        if (xSemaphoreTake(sem_full, portMAX_DELAY) == pdTRUE) {
            xSemaphoreTake(mtx_lock, portMAX_DELAY);
            LOG_I("Consumer get:%s\r\n", sharedBuf);
            xSemaphoreGive(mtx_lock);
            xSemaphoreGive(sem_empty);
        } else {
            LOG_I("Take sem_full fail\r\n");
        }
    }

    vTaskDelete(NULL);
}

static void producer_task(void *pvParameters)
{
    uint8_t buf = 100;

    LOG_I("Producer task enter \r\n");
    vTaskDelay(1000);
    LOG_I("Producer task start \r\n");

    while (1) {
        if (xSemaphoreTake(sem_empty, portMAX_DELAY) == pdTRUE) {
            xSemaphoreTake(mtx_lock, portMAX_DELAY);
            buf++;
            snprintf((char *)sharedBuf, sizeof(sharedBuf), "%d", buf);
            LOG_I("Producer generates:%s\r\n", sharedBuf);
            xSemaphoreGive(mtx_lock);
            xSemaphoreGive(sem_full);
            vTaskDelay(buf);
        } else {
            LOG_I("Take sem_empty fail\r\n");
        }
    }

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    configASSERT((configMAX_PRIORITIES > 4));

    /* Create semaphore */
    vSemaphoreCreateBinary(sem_empty);
    vSemaphoreCreateBinary(sem_full);
    vSemaphoreCreateBinary(mtx_lock);

    if (sem_empty == NULL || sem_full == NULL || mtx_lock == NULL) {
        LOG_I("Create sem fail\r\n");
    }

    LOG_I("[OS] Starting consumer task...\r\n");
    xTaskCreate(consumer_task, (char *)"consumer_task", 512, NULL, configMAX_PRIORITIES - 2, &consumer_handle);
    LOG_I("[OS] Starting producer task...\r\n");
    xTaskCreate(producer_task, (char *)"producer_task", 512, NULL, configMAX_PRIORITIES - 3, &producer_handle);

    vTaskStartScheduler();

    while (1) {
    }
}
