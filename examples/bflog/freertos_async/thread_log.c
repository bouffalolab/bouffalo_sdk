#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "example_log.h"

BFLB_LOG_DEFINE_TAG(LOG, "log", true);

#undef BFLB_LOG_TAG
#define BFLB_LOG_TAG BFLB_LOG_GET_TAG(LOG)

/* flush notice ------------------------------------------------------------------*/

static EventGroupHandle_t event_group_server_log_flush_notice;
static int
log_flush_notice(void)
{
    xEventGroupSetBits(event_group_server_log_flush_notice, 0x00000001);
    return 0;
}

/* Task Function ------------------------------------------------------------------*/

TaskHandle_t task_server_log;
void task_function_server_log(void *param)
{
    LOG_I("start server log task\r\n");

    while (1) {
        xEventGroupWaitBits(event_group_server_log_flush_notice, 0x00000001, pdTRUE, pdFALSE, portMAX_DELAY);
        LOG_FLUSH();
    }

    LOG_E("delete server_log task\r\n");
    vTaskDelete(NULL);
}

/* BFLB_LOG Varibale  ------------------------------------------------------------------*/

extern bflb_log_t example_recorder;
extern bflb_log_direct_stream_t example_uart_stream;

#ifdef EXAMPLE_ENABLE_FILESYSTEM
static bflb_log_direct_file_time_t bflb_log_direct_filetime;
static bflb_log_direct_file_size_t bflb_log_direct_filesize;
#endif

/* BFLB_LOG Mutex  ------------------------------------------------------------------*/
static SemaphoreHandle_t sem_server_log;

static int
bflb_log_enter_critical(void)
{
    if (pdTRUE == xSemaphoreTake(sem_server_log, (TickType_t)200)) {
        return 0;
    } else {
        return -1;
    }
}

static int
bflb_log_exit_critical(void)
{
    xSemaphoreGive(sem_server_log);
    return 0;
}

/* BFLB_LOG Stream Mutex  ------------------------------------------------------------------*/
SemaphoreHandle_t sem_server_uart0;

static int
bflb_log_direct_stream_lock(void)
{
    if (pdTRUE == xSemaphoreTake(sem_server_uart0, (TickType_t)200)) {
        return 0;
    } else {
        return -1;
    }
}

static int
bflb_log_direct_stream_unlock(void)
{
    xSemaphoreGive(sem_server_uart0);
    return 0;
}

/* APP init ------------------------------------------------------------------*/

void app_server_log_init(void)
{
    LOG_I("app server log start init\r\n");

    LOG_FLUSH();

    sem_server_uart0 = xSemaphoreCreateMutex();
    _ASSERT_PARAM(NULL != sem_server_uart0);

    /*!< create event flag */
    event_group_server_log_flush_notice = xEventGroupCreate();
    _ASSERT_PARAM(NULL != event_group_server_log_flush_notice);

    /*!< create mutex */
    sem_server_log = xSemaphoreCreateMutex();
    _ASSERT_PARAM(NULL != sem_server_log);

    /*!< suspen recorder */
    _ASSERT_FUNC(0 == bflb_log_suspend(&example_recorder));

    /*!< reconfig async mode */
    _ASSERT_FUNC(0 == bflb_log_control(&example_recorder, BFLB_LOG_CMD_FLUSH_NOTICE, (uint32_t)log_flush_notice));
    _ASSERT_FUNC(0 == bflb_log_control(&example_recorder, BFLB_LOG_CMD_MODE, BFLB_LOG_MODE_ASYNC));

    /*!< recofig uart0 direct stream, set lock unlock function */
    _ASSERT_FUNC(0 == bflb_log_direct_suspend((void *)&example_uart_stream));
    _ASSERT_FUNC(0 == bflb_log_direct_control((void *)&example_uart_stream, BFLB_LOG_DIRECT_CMD_LOCK, (uint32_t)bflb_log_direct_stream_lock));
    _ASSERT_FUNC(0 == bflb_log_direct_control((void *)&example_uart_stream, BFLB_LOG_DIRECT_CMD_UNLOCK, (uint32_t)bflb_log_direct_stream_unlock));
    _ASSERT_FUNC(0 == bflb_log_direct_resume((void *)&example_uart_stream));

#ifdef EXAMPLE_ENABLE_FILESYSTEM
    /*!< create and config file time direct, keep common log info */
    /*!< lock unlcok function not need, fatfs support re-entrancy mode */
    _ASSERT_FUNC(0 == bflb_log_direct_create((void *)&bflb_log_direct_filetime, BFLB_LOG_DIRECT_TYPE_FILE_TIME, BFLB_LOG_DIRECT_COLOR_DISABLE, NULL, NULL));
    /*!< 6 * 60 * 60 second create a file */
    /*!< keep max 120 files */
    _ASSERT_FUNC(0 == bflb_log_direct_init_file_time((void *)&bflb_log_direct_filetime, "sd:/log/common", 6 * 60 * 60, 120));
    _ASSERT_FUNC(0 == bflb_log_append(&example_recorder, (void *)&bflb_log_direct_filetime));
    _ASSERT_FUNC(0 == bflb_log_direct_resume((void *)&bflb_log_direct_filetime));

    /*!< create and config file time direct, only keep error and warning log info */
    /*!< lock unlcok function not need, fatfs support re-entrancy mode */
    _ASSERT_FUNC(0 == bflb_log_direct_create((void *)&bflb_log_direct_filesize, BFLB_LOG_DIRECT_TYPE_FILE_SIZE, BFLB_LOG_DIRECT_COLOR_DISABLE, NULL, NULL));
    /*!< 128 * 1024 bytes create a file */
    /*!< keep max 16 files */
    _ASSERT_FUNC(0 == bflb_log_direct_init_file_size((void *)&bflb_log_direct_filesize, "sd:/log/error", 128 * 1024, 16));
    _ASSERT_FUNC(0 == bflb_log_direct_control((void *)&bflb_log_direct_filesize, BFLB_LOG_DIRECT_CMD_LEVEL, BFLB_LOG_LEVEL_WARN));
    _ASSERT_FUNC(0 == bflb_log_append(&example_recorder, (void *)&bflb_log_direct_filesize));
    _ASSERT_FUNC(0 == bflb_log_direct_resume((void *)&bflb_log_direct_filesize));
#endif

    /*!< config mutex */
    _ASSERT_FUNC(0 == bflb_log_control(&example_recorder, BFLB_LOG_CMD_ENTER_CRITICAL, (uint32_t)bflb_log_enter_critical));
    _ASSERT_FUNC(0 == bflb_log_control(&example_recorder, BFLB_LOG_CMD_EXIT_CRITICAL, (uint32_t)bflb_log_exit_critical));

    /*!< resume recorder */
    _ASSERT_FUNC(0 == bflb_log_resume(&example_recorder));

    /*!< create flush task */
    _ASSERT_FUNC(pdPASS == xTaskCreate(
                               /*!< task function */ task_function_server_log,
                               /*!< task name     */ "LOG",
                               /*!< stack size    */ 1024,
                               /*!< task param    */ NULL,
                               /*!< task priority */ 3,
                               /*!< task handle   */ &task_server_log));

    LOG_I("app server log init success\r\n");
}
