#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "bflb_rtc.h"
#include "board.h"
#include "bflog.h"
#include "example_log.h"

/*!< 定义INLINE tag标签, 默认禁用输出 */
BFLOG_DEFINE_TAG(INLINE, "inline func", false);

/*!< 定义MAIN tag标签, 默认使能输出 */
BFLOG_DEFINE_TAG(MAIN, "main", true);

/*!< 设置此文件使用的tag标签 */
#undef BFLOG_TAG
#define BFLOG_TAG BFLOG_GET_TAG(MAIN)

/*!< bflog 多线程异步例程 */
/*!< bflog multithread async example */
/*!< 多线程建议在未开始RTOS调度之前配置为同步模式, 开始RTOS调度后再切换为异步模式
    此时不管RTOS能否正常工作, LOG工作都不会异常导致无LOG输出 */

struct bflb_device_s *uart0 = NULL;
static struct bflb_device_s *rtc = NULL;

/*!< 当前的UTC时间戳 2022-12-16 17:52 */
uint32_t timestamp_base = 1671184300;

/** @defgroup   example_bflog_port port
-----------------------------------------------------------------------------
* @{
----------------------------------------------------------------------------*/
uint64_t bflog_clock(void)
{
    return bflb_mtimer_get_time_us();
}

uint32_t bflog_time(void)
{
    return BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc)) + timestamp_base;
}

char *bflog_thread(void)
{
    return "";
}
/*---------------------------------------------------------------------------
* @}            example_bflog_port port
----------------------------------------------------------------------------*/

#define EXAMPLE_LOG_POOL_SIZE 4096

bflog_t example_recorder;
void *example_recorder_pointer = &example_recorder;
static uint32_t example_pool[EXAMPLE_LOG_POOL_SIZE / 4];
bflog_direct_stream_t example_uart_stream;

/*****************************************************************************
* @brief        流输出
* 
* @param[in]    ptr         输出信息的指针
* @param[in]    size        输出信息的大小（byte）
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
* @brief        进入多线程调度前初始化
* 
* 
*****************************************************************************/
void example_log_init_before_enter(void)
{
    void *record = (void *)&example_recorder;
    void *direct = (void *)&example_uart_stream;

    /*!< 创建一个记录器, 配置内存池, 内存池大小, 模式为同步 */
    /*!< create recorder */
    if (0 != bflog_create(record, example_pool, EXAMPLE_LOG_POOL_SIZE, BFLOG_MODE_SYNC)) {
        printf("bflog_create faild\r\n");
    }

    /*!< 创建输出器, 类型为流输出器, 使能颜色输出, 互斥锁设为NULL */
    /*!< create stream direct */
    bflog_direct_create(direct, BFLOG_DIRECT_TYPE_STREAM, BFLOG_DIRECT_COLOR_ENABLE, NULL, NULL);
    /*!< 配置流输出的输出函数 */
    bflog_direct_init_stream((void *)direct, example_uart_stream_output);

    /*!< 添加输出器到记录器 */
    /*!< connect direct and recorder */
    bflog_append(record, direct);

    /*!< 恢复输出器到工作模式 */
    /*!< resume direct */
    bflog_direct_resume(direct);

    /*!< 恢复记录器到工作模式 */
    /*!< resume record */
    bflog_resume(record);
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

    LOG_I("exec bflog_global_filter(\"%s\",%d)\r\n", argv[1], atoi(argv[2]));
    ret = bflog_global_filter(argv[1], atoi(argv[2]));
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

    LOG_I("exec bflog_control(&example_recorder,BFLOG_CMD_LEVEL,%d)\r\n", atoi(argv[1]));
    ret = bflog_control(&example_recorder, BFLOG_CMD_LEVEL, atoi(argv[1]));

    if (ret) {
        LOG_E("exec result faild, %d\r\n", ret);
    } else {
        LOG_I("exec success\r\n");
    }

    return 0;
}

SHELL_CMD_EXPORT(tag_config, tag config);
SHELL_CMD_EXPORT(level_config, level config);