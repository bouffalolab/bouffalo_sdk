#define DBG_TAG "MAIN"

#include <stdlib.h>
#include "bflb_mtimer.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "board.h"
#include "log.h"
#include "bflb_block_pool.h"

BFLOG_DEFINE_TAG(MAIN, DBG_TAG, true);
#undef BFLOG_TAG
#define BFLOG_TAG BFLOG_GET_TAG(MAIN)

#define BLK_SIZE        128
#define BLK_MEMORY_SIZE 16 * (BLK_SIZE + sizeof(void *))
static bflb_block_pool_t block_pool;
__attribute__((aligned(8))) static uint8_t block_pool_memory[BLK_MEMORY_SIZE];

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
    uint32_t total;

    while (1) {
        time = rand() % 64;

        if (bflb_block_pool_alloc(&block_pool, &block_ptr, 100)) {
            LOG_W("alloc block pool faild\r\n");

            bflb_block_pool_info_get(&block_pool, &total, &free);
            LOG_W("total [%4ld] free [%4ld]\r\n", total, free);
        } else {
            bflb_block_pool_info_get(&block_pool, &total, &free);
            LOG_I("total [%4ld] free [%4ld]\r\n", total, free);

            cnt++;
            snprintf((char *)block_ptr, BLK_SIZE, "this is [%ld] info in block [%08lx]\r\n", cnt, (uint32_t)block_ptr);
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

static int block_pool_sem_get(uint32_t wait)
{
    if (pdTRUE != xSemaphoreTake(sem, wait)) {
        return -1;
    }
    return 0;
}
static void block_pool_sem_put(void)
{
    xSemaphoreGive(sem);
}
static int block_pool_mtx_get(uint32_t wait)
{
    if (pdTRUE != xSemaphoreTake(mtx, wait)) {
        return -1;
    }
    return 0;
}
static void block_pool_mtx_put(void)
{
    xSemaphoreGive(mtx);
}

int main(void)
{
    uint32_t block_count;

    board_init();

    configASSERT((configMAX_PRIORITIES > 4));

    _ASSERT_FUNC(0 == bflb_block_pool_create(&block_pool, BLK_SIZE, BFLB_BLOCK_POOL_ALIGN_8, block_pool_memory, BLK_MEMORY_SIZE));
    bflb_block_pool_info_get(&block_pool, &block_count, NULL);

    LOG_I("block pool total block count is %d\r\n", block_count);

    queue = xQueueCreate(block_count, sizeof(void *));
    _ASSERT_PARAM(NULL != queue);

    sem = xSemaphoreCreateCounting(block_count, block_count);
    _ASSERT_PARAM(NULL != sem);

    mtx = xSemaphoreCreateMutex();
    _ASSERT_PARAM(NULL != sem);

    _ASSERT_FUNC(0 == bflb_block_pool_add_mtx(&block_pool, block_pool_mtx_get, block_pool_mtx_put));
    _ASSERT_FUNC(0 == bflb_block_pool_add_sem(&block_pool, block_pool_sem_get, block_pool_sem_put));

    LOG_I("Starting consumer task...\r\n");
    xTaskCreate(consumer_task, (char *)"consumer_task", 512, NULL, configMAX_PRIORITIES - 2, &consumer_handle);
    LOG_I("Starting producer task...\r\n");
    xTaskCreate(producer_task, (char *)"producer_task", 512, NULL, configMAX_PRIORITIES - 3, &producer_handle);

#ifdef CONFIG_BFLOG
    log_restart();
#endif

    vTaskStartScheduler();

    while (1) {
    }
}
