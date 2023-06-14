#define DBG_TAG "Svc LOG"

#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "log.h"

BFLOG_DEFINE_TAG(SVC_LOG, DBG_TAG, true);
#undef BFLOG_TAG
#define BFLOG_TAG BFLOG_GET_TAG(SVC_LOG)

#ifndef LOG_THREAD_PRIO
#define LOG_THREAD_PRIO 3
#endif

#ifndef LOG_THREAD_STACK_SIZE
#define LOG_THREAD_STACK_SIZE 1024
#endif

/* flush notice ------------------------------------------------------------------*/

static EventGroupHandle_t event_group_server_log_flush_notice;
static int log_flush_notice(void)
{
    xEventGroupSetBits(event_group_server_log_flush_notice, 0x00000001);
    return 0;
}

/* Task Function ------------------------------------------------------------------*/

static TaskHandle_t task_log;
static void task_function_log(void *param)
{
    LOG_I("start log task\r\n");

    while (1) {
        xEventGroupWaitBits(event_group_server_log_flush_notice, 0x00000001, pdTRUE, pdFALSE, portMAX_DELAY);
        LOG_FLUSH();
    }

    LOG_E("delete log task\r\n");
    vTaskDelete(NULL);
}

/* BFLOG Varibale  ------------------------------------------------------------------*/

extern bflog_t __bflog_recorder;
extern bflog_direct_stream_t bflog_direct_stream;

#ifdef LOG_ENABLE_FILESYSTEM
static bflog_direct_file_time_t bflog_direct_filetime;
static bflog_direct_file_size_t bflog_direct_filesize;
#endif

/* BFLOG Mutex  ------------------------------------------------------------------*/
static SemaphoreHandle_t sem_server_log;

static int
bflog_enter_critical(void)
{
    if (pdTRUE == xSemaphoreTake(sem_server_log, (TickType_t)200)) {
        return 0;
    } else {
        return -1;
    }
}

static int
bflog_exit_critical(void)
{
    xSemaphoreGive(sem_server_log);
    return 0;
}

/* BFLOG Stream Mutex  ------------------------------------------------------------------*/
SemaphoreHandle_t sem_uart0;

static int
bflog_direct_stream_lock(void)
{
    if (pdTRUE == xSemaphoreTake(sem_uart0, (TickType_t)200)) {
        return 0;
    } else {
        return -1;
    }
}

static int
bflog_direct_stream_unlock(void)
{
    xSemaphoreGive(sem_uart0);
    return 0;
}

/* APP init ------------------------------------------------------------------*/
extern bflog_direct_stream_t bflog_direct_stream;

void log_restart(void)
{
    LOG_I("log restart\r\n");

    LOG_FLUSH();

    sem_uart0 = xSemaphoreCreateMutex();
    _ASSERT_PARAM(NULL != sem_uart0);

    /*!< create event flag */
    event_group_server_log_flush_notice = xEventGroupCreate();
    _ASSERT_PARAM(NULL != event_group_server_log_flush_notice);

    /*!< create mutex */
    sem_server_log = xSemaphoreCreateMutex();
    _ASSERT_PARAM(NULL != sem_server_log);

    /*!< suspen recorder */
    _ASSERT_FUNC(0 == bflog_suspend(&__bflog_recorder));

    /*!< reconfig async mode */
    _ASSERT_FUNC(0 == bflog_control(&__bflog_recorder, BFLOG_CMD_FLUSH_NOTICE, (uint32_t)log_flush_notice));
    _ASSERT_FUNC(0 == bflog_control(&__bflog_recorder, BFLOG_CMD_MODE, BFLOG_MODE_ASYNC));

    /*!< recofig uart0 direct stream, set lock unlock function */
    _ASSERT_FUNC(0 == bflog_direct_suspend((void *)&bflog_direct_stream));
    _ASSERT_FUNC(0 == bflog_direct_control((void *)&bflog_direct_stream, BFLOG_DIRECT_CMD_LOCK, (uint32_t)bflog_direct_stream_lock));
    _ASSERT_FUNC(0 == bflog_direct_control((void *)&bflog_direct_stream, BFLOG_DIRECT_CMD_UNLOCK, (uint32_t)bflog_direct_stream_unlock));
    _ASSERT_FUNC(0 == bflog_direct_resume((void *)&bflog_direct_stream));

#ifdef LOG_ENABLE_FILESYSTEM
    /*!< create and config file time direct, keep common log info */
    /*!< lock unlcok function not need, fatfs support re-entrancy mode */
    _ASSERT_FUNC(0 == bflog_direct_create((void *)&bflog_direct_filetime, BFLOG_DIRECT_TYPE_FILE_TIME, BFLOG_DIRECT_COLOR_DISABLE, NULL, NULL));
    /*!< 6 * 60 * 60 second create a file */
    /*!< keep max 120 files */
    _ASSERT_FUNC(0 == bflog_direct_init_file_time((void *)&bflog_direct_filetime, "/sd/log/common", 6 * 60 * 60, 120));
    _ASSERT_FUNC(0 == bflog_append(&__bflog_recorder, (void *)&bflog_direct_filetime));
    _ASSERT_FUNC(0 == bflog_direct_resume((void *)&bflog_direct_filetime));

    /*!< create and config file time direct, only keep error and warning log info */
    /*!< lock unlcok function not need, fatfs support re-entrancy mode */
    _ASSERT_FUNC(0 == bflog_direct_create((void *)&bflog_direct_filesize, BFLOG_DIRECT_TYPE_FILE_SIZE, BFLOG_DIRECT_COLOR_DISABLE, NULL, NULL));
    /*!< 128 * 1024 bytes create a file */
    /*!< keep max 16 files */
    _ASSERT_FUNC(0 == bflog_direct_init_file_size((void *)&bflog_direct_filesize, "/sd/log/error", 128 * 1024, 16));
    _ASSERT_FUNC(0 == bflog_direct_control((void *)&bflog_direct_filesize, BFLOG_DIRECT_CMD_LEVEL, BFLOG_LEVEL_WARN));
    _ASSERT_FUNC(0 == bflog_append(&__bflog_recorder, (void *)&bflog_direct_filesize));
    _ASSERT_FUNC(0 == bflog_direct_resume((void *)&bflog_direct_filesize));
#endif

    /*!< config mutex */
    _ASSERT_FUNC(0 == bflog_control(&__bflog_recorder, BFLOG_CMD_ENTER_CRITICAL, (uint32_t)bflog_enter_critical));
    _ASSERT_FUNC(0 == bflog_control(&__bflog_recorder, BFLOG_CMD_EXIT_CRITICAL, (uint32_t)bflog_exit_critical));

    /*!< resume recorder */
    _ASSERT_FUNC(0 == bflog_resume(&__bflog_recorder));

    /*!< create flush task */
    _ASSERT_FUNC(pdPASS == xTaskCreate(
                               /*!< task function */ task_function_log,
                               /*!< task name     */ DBG_TAG,
                               /*!< stack size    */ LOG_THREAD_STACK_SIZE,
                               /*!< task param    */ NULL,
                               /*!< task priority */ LOG_THREAD_PRIO,
                               /*!< task handle   */ &task_log));

    LOG_I("log restart success\r\n");
}