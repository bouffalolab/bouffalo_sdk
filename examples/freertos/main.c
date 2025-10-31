#include <FreeRTOS.h>
#include "semphr.h"
#include "board.h"

#define DBG_TAG "MAIN"
#define QUEUE_LENGTH 5
#define QUEUE_SIZE sizeof(struct Message)
#include "log.h"

typedef enum {
    Que_Sem_Test,
    Priority_Mutex_Test,
    End
} TestProcedure_t;
TestProcedure_t phase;

struct Message {
    uint8_t task;
    uint32_t timestamp;
};

QueueHandle_t xqueue;
SemaphoreHandle_t xsemaphore;
SemaphoreHandle_t xMutex;
static volatile uint16_t shared_counter = 0;

static TaskHandle_t TestProcess_handle;
static TaskHandle_t consumer_handle_1;
static TaskHandle_t consumer_handle_2;
static TaskHandle_t producer_handle;
static TaskHandle_t ht_task_handle;
static TaskHandle_t sharetask_handle_1;
static TaskHandle_t sharetask_handle_2;

/*******************Test Queue,Semaphore************************/
static void consumer_task(void *pvParameters) {
    struct Message msg;
    int consumer_id = (int)pvParameters;

    LOG_I("[Consumer %d]: task started\r\n", consumer_id);
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1) {
        if (phase != Que_Sem_Test) {
            LOG_I("[Consumer %d]: Exiting due to phase change\r\n", consumer_id);
            vTaskDelete(NULL);
        }

        if (xSemaphoreTake(xsemaphore, portMAX_DELAY)) {
            if (xQueueReceive(xqueue, &msg, 0) == pdPASS) {
                LOG_I("[Consumer %d]: received msg (task:%d, ts:%lu)\r\n",
                      consumer_id, msg.task, (unsigned long)msg.timestamp);
            } else {
                LOG_I("[Consumer %d]: queue empty\r\n", consumer_id);
            }
            xSemaphoreGive(xsemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL);
}

static void producer_task(void *pvParameters) {
    struct Message msg;
    int producer_id = (int)pvParameters;

    msg.task = producer_id;

    while (1) {
        if (phase != Que_Sem_Test) {
            LOG_I("[Producer %d]: Exiting due to phase change\r\n", producer_id);
            vTaskDelete(NULL);
        }

        msg.timestamp = xTaskGetTickCount();
        if (xSemaphoreTake(xsemaphore, portMAX_DELAY)) {
            if (xQueueSend(xqueue, &msg, 0) != pdPASS) {
                LOG_I("[Producer %d]: queue full\r\n", producer_id);
            }else
            {
                LOG_I("[Producer %d]: message send success\r\n", producer_id);
            }

            xSemaphoreGive(xsemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL);
}

/*******************Test Mutex************************/
static void SharedResourceTask(void *pvParameters) {
    int task_id = (int)pvParameters;
    while(1) {
        if (phase != Priority_Mutex_Test) {
            LOG_I("[Mutex Task %d]: Exiting due to phase change\r\n", task_id);
            vTaskDelete(NULL);
        }

        LOG_I("[Mutex Task %d]: Attempt to acquire mutex\r\n", task_id);
        if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            uint32_t temp = shared_counter;
            shared_counter = temp + 1;
            LOG_I("[Mutex Task %d]: Aquired.\r\n",task_id);
            LOG_I("[Mutex Task %d]: value = %d\r\n", task_id, shared_counter);
            LOG_I("[Mutex Task %d]: Holding mutex for 500ms\r\n", task_id);
            vTaskDelay(pdMS_TO_TICKS(500));
            LOG_I("[Mutex Task %d]: Release mutex\r\n", task_id);
            xSemaphoreGive(xMutex);

        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

/****************Test Preemptive scheduling*******************/
static void HighPriorityTask(void* pvParameters) {
    if (phase != Priority_Mutex_Test) {
        LOG_I("[High Priority Task]: Exiting due to phase change\r\n");
        vTaskDelete(NULL);
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
    LOG_I("[High Priority Task]: High priority task running\r\n"); // Correct spelling
    for (size_t i = 0; i < 500000; i++)
    {
    }

    LOG_I("[High Priority Task]: Completed\r\n");
    vTaskDelete(NULL);
}

static void TestProcessTask(void * pvParameters) {

    LOG_I("[Process Task]: Test Start\r\n");


    LOG_I("\n===== PHASE 1: Queue/Semaphore Test =====\r\n");
    phase = Que_Sem_Test;

    LOG_I("[Process Task]: Starting consumer tasks...\r\n");
    xTaskCreate(consumer_task, "cons1", 512, (void *)1, configMAX_PRIORITIES - 2, &consumer_handle_1);
    xTaskCreate(consumer_task, "cons2", 512, (void *)2, configMAX_PRIORITIES - 2, &consumer_handle_2);

    LOG_I("[Process Task]: Starting producer task...\r\n");
    xTaskCreate(producer_task, "prod1", 512, (void *)1, configMAX_PRIORITIES - 3, &producer_handle);

    vTaskDelay(pdMS_TO_TICKS(5000));    // Run 5s
    LOG_I("\n===== END PHASE 1 =====\r\n");


    LOG_I("\n===== PHASE 2: Priority/Mutex Test =====\r\n");
    phase = Priority_Mutex_Test;
    vTaskDelay(pdMS_TO_TICKS(200));     // ensure task already exit

    LOG_I("[Process Task] Starting High Priority task...\r\n");
    xTaskCreate(HighPriorityTask, "ht1", 512, NULL, configMAX_PRIORITIES - 1, &ht_task_handle);

    LOG_I("[Process Task] Starting Shared Resource tasks...\r\n");
    // Create different priority task to test priority convert
    xTaskCreate(SharedResourceTask, "mutex_high", 512, (void *)3, configMAX_PRIORITIES - 1, &sharetask_handle_1);
    xTaskCreate(SharedResourceTask, "mutex_low", 512, (void *)4, configMAX_PRIORITIES - 3, &sharetask_handle_2);

    vTaskDelay(pdMS_TO_TICKS(5000));    // Run 5s
    LOG_I("\n===== END PHASE 2 =====\r\n");

    // All Complete
    phase = End;
    vTaskDelay(pdMS_TO_TICKS(1000));    // Wait for exit

    LOG_I("[Process Task] All Test Complete.\r\n");

    // Stop all task
    vTaskSuspendAll();
    for(;;);
}

int main(void)
{
    board_init();
    configASSERT((configMAX_PRIORITIES > 4));

    /* Init source */
    xqueue = xQueueCreate(QUEUE_LENGTH, QUEUE_SIZE);
    xsemaphore = xSemaphoreCreateBinary();
    xMutex = xSemaphoreCreateMutex();

    if (xqueue == NULL || xsemaphore == NULL || xMutex == NULL) {
        LOG_E("Resource creation failed! Halting...\r\n");
        while(1);
    }
    xSemaphoreGive(xsemaphore);

    /* Process Task Create */
    xTaskCreate(TestProcessTask, "TestProc", 512, NULL, configMAX_PRIORITIES - 1, &TestProcess_handle);

    vTaskStartScheduler();

    while (1);
}