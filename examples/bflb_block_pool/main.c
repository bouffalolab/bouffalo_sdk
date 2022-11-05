#include "bflb_mtimer.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "log.h"
#include "bflb_block_pool.h"
#include "stdlib.h"

extern void board_init(void);

#define BLK_COUNT 16
#define BLK_SIZE  128

static bflb_block_pool_t block_pool;
static bflb_block_pool_node_t block_nodes[BLK_COUNT];
static uint8_t block_pool_memory[BLK_COUNT * BLK_SIZE];

static uint8_t freertos_heap[configTOTAL_HEAP_SIZE];

static HeapRegion_t xHeapRegions[] = {
    { (uint8_t *)freertos_heap, 0 },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};

static TaskHandle_t consumer_handle;
static TaskHandle_t producer_handle;

QueueHandle_t queue;

static void consumer_task(void *pvParameters)
{
    uint8_t time = 200;

    LOG_I("Consumer task enter \r\n");
    vTaskDelay(1000);
    LOG_I("Consumer task start \r\n");

    void *block_ptr;

    while (1) {
        time += 20;
        if (pdTRUE != xQueueReceive(queue, &block_ptr, portMAX_DELAY)) {
            LOG_E("queue recv faild\r\n");
        } else {
            LOG_I("Consumer recv << %s\r\n", (char *)block_ptr);
            if (bflb_block_pool_free(&block_pool, block_ptr)) {
                LOG_E("free block heap faild\r\n");
            }
        }

        vTaskDelay(time);
    }

    vTaskDelete(NULL);
}

static void producer_task(void *pvParameters)
{
    uint8_t time = 100;
    uint32_t cnt = 0;

    LOG_I("Producer task enter \r\n");
    vTaskDelay(1000);
    LOG_I("Producer task start \r\n");

    void *block_ptr;
    uint32_t free;
    uint32_t busy;

    while (1) {
        time = rand() % 64;

        if (bflb_block_pool_alloc_wait(&block_pool, &block_ptr, 100)) {
            LOG_W("alloc block pool faild\r\n");

            bflb_block_pool_get_info(&block_pool, &busy, &free);
            LOG_W("free [%4ld] busy [%4ld]\r\n", free, busy);
        } else {
            bflb_block_pool_get_info(&block_pool, &busy, &free);
            LOG_I("free [%4ld] busy [%4ld]\r\n", free, busy);

            cnt++;
            sprintf((char *)block_ptr, "this is [%ld] info in block [%08lx]\r\n", cnt, (uint32_t)block_ptr);
            LOG_I("Producer send >> %s\r\n", (char *)block_ptr);
            if (pdTRUE != xQueueSend(queue, (void *)&block_ptr, 1000)) {
                LOG_E("queue send faild\r\n");
            }
        }

        vTaskDelay(time);
    }

    vTaskDelete(NULL);
}

SemaphoreHandle_t sem;
SemaphoreHandle_t mtx;

static int block_pool_semaphore_get(uint32_t wait)
{
    if (pdTRUE != xSemaphoreTake(sem, wait)) {
        return -1;
    }
    return 0;
}
static int block_pool_semaphore_put(void)
{
    if (pdTRUE != xSemaphoreGive(sem)) {
        return -1;
    }
    return 0;
}
static int block_pool_mutex_get(uint32_t wait)
{
    if (pdTRUE != xSemaphoreTake(mtx, wait)) {
        return -1;
    }
    return 0;
}
static int block_pool_mutex_put(void)
{
    if (pdTRUE != xSemaphoreGive(mtx)) {
        return -1;
    }
    return 0;
}

int main(void)
{
    board_init();
    xHeapRegions[0].xSizeInBytes = configTOTAL_HEAP_SIZE;
    vPortDefineHeapRegions(xHeapRegions);

    configASSERT((configMAX_PRIORITIES > 4));

    queue = xQueueCreate(BLK_COUNT, sizeof(void *));
    _ASSERT_PARAM(NULL != queue);

    sem = xSemaphoreCreateCounting(BLK_COUNT, BLK_COUNT);
    _ASSERT_PARAM(NULL != sem);

    mtx = xSemaphoreCreateMutex();
    _ASSERT_PARAM(NULL != sem);

    _ASSERT_FUNC(0 == bflb_block_pool_create(&block_pool, block_nodes, block_pool_memory, BLK_COUNT, BLK_SIZE));
    _ASSERT_FUNC(0 == bflb_block_pool_register_mutex(&block_pool, block_pool_mutex_get, block_pool_mutex_put));
    _ASSERT_FUNC(0 == bflb_block_pool_register_semaphore(&block_pool, block_pool_semaphore_get, block_pool_semaphore_put));

    LOG_I("[OS] Starting consumer task...\r\n");
    xTaskCreate(consumer_task, (char *)"consumer_task", 512, NULL, configMAX_PRIORITIES - 2, &consumer_handle);
    LOG_I("[OS] Starting producer task...\r\n");
    xTaskCreate(producer_task, (char *)"producer_task", 512, NULL, configMAX_PRIORITIES - 3, &producer_handle);

    vTaskStartScheduler();

    while (1) {
    }
}
