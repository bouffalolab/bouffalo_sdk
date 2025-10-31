#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "bflb_rtc.h"
#include "board.h"
#include "bflb_log.h"
#include "example_log.h"

/*!< Define the INLINE tag, output is disabled by default. */
BFLB_LOG_DEFINE_TAG(INLINE, "inline func", false);

/*!< Define the MAIN tag, output is enabled by default. */
BFLB_LOG_DEFINE_TAG(MAIN, "main", true);

/*!< Set the tag used by this file */
#undef BFLB_LOG_TAG
#define BFLB_LOG_TAG BFLB_LOG_GET_TAG(MAIN)

/*!< bflb_log multithread async example */
/*!< For multi-threading, it is recommended to configure it as synchronous mode before RTOS scheduling starts, and then switch to asynchronous mode after RTOS scheduling begins.
At this point, regardless of whether the RTOS can work normally, the LOG operation will not be abnormal and cause no LOG output. */

struct bflb_device_s *uart0 = NULL;
static struct bflb_device_s *rtc = NULL;

/*!< Current UTC Timestamp 2022-12-16 17:52 */
uint32_t timestamp_base = 1671184300;

/** @defgroup   example_bflb_log_port port
-----------------------------------------------------------------------------
* @{
----------------------------------------------------------------------------*/
uint64_t bflb_log_clock(void)
{
    return bflb_mtimer_get_time_us();
}

uint32_t bflb_log_time(void)
{
    return BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc)) + timestamp_base;
}

char *bflb_log_thread(void)
{
    return "";
}
/*---------------------------------------------------------------------------
* @}            example_bflb_log_port port
----------------------------------------------------------------------------*/

#define EXAMPLE_LOG_POOL_SIZE 4096

bflb_log_t example_recorder;
void *example_recorder_pointer = &example_recorder;
static uint32_t example_pool[EXAMPLE_LOG_POOL_SIZE / 4];
bflb_log_direct_stream_t example_uart_stream;

/*****************************************************************************
* @brief        Stream output
*
* @param[in]    ptr         Pointer to the output information
* @param[in]    size        Size of the output information（byte）
*
* @retval uint16_t
*****************************************************************************/
uint16_t example_uart_stream_output(void *ptr, uint16_t size)
{
    for (size_t i = 0; i < size; i++) {
        bflb_uart_putchar(uart0, ((char *)ptr)[i]);
    }
    return size;
}

/*****************************************************************************
* @brief        Initialize before entering multi-thread scheduling
*
*
*****************************************************************************/
void example_log_init_before_enter(void)
{
    void *record = (void *)&example_recorder;
    void *direct = (void *)&example_uart_stream;

    /*!< Create a logger, configure the memory pool and the size of the memory pool, with the mode set to synchronous. */
    /*!< create recorder */
    if (0 != bflb_log_create(record, example_pool, EXAMPLE_LOG_POOL_SIZE, BFLB_LOG_MODE_SYNC)) {
        printf("bflb_log_create faild\r\n");
    }

    /*!< Create an outputter, of the stream outputter type, enable color output, and set the mutex to NULL. */
    /*!< create stream direct */
    bflb_log_direct_create(direct, BFLB_LOG_DIRECT_TYPE_STREAM, BFLB_LOG_DIRECT_COLOR_ENABLE, NULL, NULL);
    /*!< Configure the output function of the stream output */
    bflb_log_direct_init_stream((void *)direct, example_uart_stream_output);

    /*!< Add the outputter to the logger */
    /*!< connect direct and recorder */
    bflb_log_append(record, direct);

    /*!< Restore the outputter to working mode */
    /*!< resume direct */
    bflb_log_direct_resume(direct);

    /*!< Restore the logger to working mode */
    /*!< resume record */
    bflb_log_resume(record);
}

/** @defgroup   freertos_base
-----------------------------------------------------------------------------
* @{
----------------------------------------------------------------------------*/
#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

void vApplicationTickHook(void)
{
}

void vApplicationIdleHook(void)
{
}

void vAssertCalled(void)
{
    LOG_F("vAssertCalled\r\n");
    _CALL_ERROR();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    LOG_F("vApplicationStackOverflowHook\r\n");
    LOG_F("Taskname [%s]\r\n", pcTaskName);
    _CALL_ERROR();
}

void vApplicationMallocFailedHook(void)
{
    LOG_F("vApplicationMallocFailedHook\r\n");
    _CALL_ERROR();
}

/*---------------------------------------------------------------------------
* @}            freertos_base
----------------------------------------------------------------------------*/

/** @defgroup   thread_start
-----------------------------------------------------------------------------
* @{
----------------------------------------------------------------------------*/
#include "shell.h"
extern void shell_init_with_task(struct bflb_device_s *shell);

extern void app_server_log_init(void);

extern TaskHandle_t consumer_handle;
extern TaskHandle_t producer_handle;
extern void consumer_task(void *pvParameters);
extern void producer_task(void *pvParameters);

static TaskHandle_t task_start;
static void
task_function_start(void *param)
{
    LOG_I("start start task\r\n");

    app_server_log_init();

    shell_init_with_task(uart0);

    xTaskCreate(consumer_task, (char *)"consumer_task", 512, NULL, 4, &consumer_handle);
    xTaskCreate(producer_task, (char *)"producer_task", 512, NULL, 5, &producer_handle);

    LOG_I("delete start task\r\n");
    vTaskDelete(NULL);
}

/*---------------------------------------------------------------------------
* @}            thread_start
----------------------------------------------------------------------------*/

QueueHandle_t queue;

int main(void)
{
    board_init();

    /*!< uart0 already initialized in bsp/board */
    uart0 = bflb_device_get_by_name("uart0");
    rtc = bflb_device_get_by_name("rtc");
    bflb_rtc_set_time(rtc, 0);

    example_log_init_before_enter();

    LOG_I("app start init\r\n");

    _ASSERT_PARAM(configMAX_PRIORITIES >= 7);

    queue = xQueueCreate(32, sizeof(void *));
    _ASSERT_PARAM(NULL != queue);

    _ASSERT_FUNC(pdPASS == xTaskCreate(
                               /*!< task function */ task_function_start,
                               /*!< task name     */ "START",
                               /*!< stack size    */ 1024,
                               /*!< task param    */ NULL,
                               /*!< task priority */ 6,
                               /*!< task handle   */ &task_start));

    LOG_I("app start init success\r\n");

    vTaskStartScheduler();

    while (1) {
        bflb_mtimer_delay_ms(200);
    }
}

void error_handler(void)
{
    volatile unsigned char dummy = 0;

    printf("(enter error handler)\r\n");
    LOG_F("enter error handler\r\n");
    LOG_FLUSH();

    uint32_t flag = bflb_irq_save();

    while (dummy == 0) {
    }

    bflb_irq_restore(flag);
}

static int tag_config(int argc, char **argv)
{
    int ret = 0;

    if (argc < 3) {
        LOG_I("tag_config <your tag> 1\r\n");
        LOG_I("tag_config <your tag> 0\r\n");
        return 0;
    }

    LOG_I("exec bflb_log_global_filter(\"%s\",%d)\r\n", argv[1], atoi(argv[2]));
    ret = bflb_log_global_filter(argv[1], atoi(argv[2]));
    if (ret) {
        LOG_E("exec result faild, %d\r\n", ret);
    } else {
        LOG_I("exec success\r\n");
    }

    return 0;
}

static int level_config(int argc, char **argv)
{
    int ret = 0;

    if (argc < 2) {
        LOG_I("level_config <your level>\r\n");
        return 0;
    }

    LOG_I("exec bflb_log_control(&example_recorder,BFLB_LOG_CMD_LEVEL,%d)\r\n", atoi(argv[1]));
    ret = bflb_log_control(&example_recorder, BFLB_LOG_CMD_LEVEL, atoi(argv[1]));

    if (ret) {
        LOG_E("exec result faild, %d\r\n", ret);
    } else {
        LOG_I("exec success\r\n");
    }

    return 0;
}

SHELL_CMD_EXPORT(tag_config, tag config);
SHELL_CMD_EXPORT(level_config, level config);