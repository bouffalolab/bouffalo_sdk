
#include <FreeRTOS.h>
#include <task.h>
#include "timers.h"
#include "stream_buffer.h"
#include "sdiowifi_platform_adapt.h"


typedef struct tmr_adapter {
    TimerHandle_t timer;
    void (*func)(void *, void *);
    void *func_arg;
    uint8_t bIsRepeat;
} tmr_adapter_t;

static void tmr_adapt_cb(TimerHandle_t xTimer)
{
    tmr_adapter_t *pTmrObj = (tmr_adapter_t *)pvTimerGetTimerID(xTimer);

    if(pTmrObj->func)
        pTmrObj->func(pTmrObj, pTmrObj->func_arg);
    return;
}

sdiowifi_timer_handle_t sdiowifi_timer_create(void (*fn)(void *, void *),
                      void *arg, int ms, int repeat, unsigned char auto_run)
{
    /* verify param */
    if ( 0 == ms|| NULL == fn) {
        return NULL;
    }

    /* create timer wrap object ,then initlize timer object */
    tmr_adapter_t *tmr_adapter = pvPortMalloc(sizeof(tmr_adapter_t));

    if (NULL == tmr_adapter) {
        return NULL;
    }

    tmr_adapter->func = fn;
    tmr_adapter->func_arg = arg;
    tmr_adapter->bIsRepeat = repeat;

    /* create timer by kernel api */
    TimerHandle_t ptimer = xTimerCreate("Timer", pdMS_TO_TICKS(ms), repeat, tmr_adapter, tmr_adapt_cb);

    if (NULL == ptimer) {
        vPortFree(tmr_adapter);
        return NULL;
    }

    tmr_adapter->timer = ptimer;

    /* start timer if auto_run == TRUE */
    if(auto_run) {
        if(sdiowifi_timer_start(tmr_adapter) != 0) {
            return NULL;
        }
    }

    return (sdiowifi_timer_handle_t)tmr_adapter;
}

void sdiowifi_timer_free(sdiowifi_timer_handle_t timer)
{
    if (NULL == timer) {
        return;
    }

    tmr_adapter_t *tmr_adapter = timer;
    int ret = xTimerDelete(tmr_adapter->timer, 0);

    if (!ret) {
        return ;
    }

    vPortFree(tmr_adapter);
    timer = NULL;

    return ;
}

int sdiowifi_timer_start(sdiowifi_timer_handle_t timer)
{
    /* verify param */
    if (NULL == timer ) {
        return -1;
    }

    /* start timer  */
    tmr_adapter_t *tmr_adapter = timer;
    int ret;

    if(xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        ret = xTimerStartFromISR(tmr_adapter->timer, &xHigherPriorityTaskWoken);
    } else {
        ret = xTimerStart(tmr_adapter->timer, 0);
    }
    if (ret != pdPASS) {
        return -1;
    }

    return 0;
}

int sdiowifi_timer_stop(sdiowifi_timer_handle_t timer)
{
    /* verify param */
    if (NULL == timer ) {
        return -1;
    }

    /* stop timer */
    tmr_adapter_t *tmr_adapter = timer;
    int ret;

    if(xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        ret = xTimerStopFromISR(tmr_adapter->timer, &xHigherPriorityTaskWoken);
    } else {
        ret = xTimerStop(tmr_adapter->timer, 0);
    }

    if (ret != pdPASS) {
        return -1;
    }

    return 0;
}

int sdiowifi_timer_change(sdiowifi_timer_handle_t timer, int ms)
{
    /* verify param */
    if (NULL == timer ) {
        return -1;
    }
    BaseType_t  xHigherProTskWoken  = pdFALSE;
    /* change timer period value */
    tmr_adapter_t *pTimer = timer;
    int ret;
    if(xPortIsInsideInterrupt()) {
        ret = xTimerChangePeriodFromISR(pTimer->timer,pdMS_TO_TICKS(ms), &xHigherProTskWoken);
    } else {
        ret = xTimerChangePeriod(pTimer->timer, pdMS_TO_TICKS(ms), 10);
    }

    if(ret != pdPASS) {
        return -1;
    }

    return 0;
}

int sdiowifi_timer_is_valid(sdiowifi_timer_handle_t timer)
{
    if (NULL == timer ) {
        return -1;
    }
    tmr_adapter_t *pTimer = timer;
    if( xTimerIsTimerActive(pTimer->timer) != pdFALSE ) {
        // pTimer is active, do something.
        return 1;
    }
    else {
        // pTimer is not active, do something else.
        return 0;
    }
}

int sdiowifi_timer_change_once(sdiowifi_timer_handle_t timer, int ms)
{
    int ret = -1;
    if (NULL == timer ) {
        return -1;
    }
    //msp_check_return_einval(time);
    tmr_adapter_t *pTimer = timer;
    if( xTimerIsTimerActive(pTimer->timer) != pdFALSE ) {
        xTimerStop(pTimer->timer, 100);
    }
    if( xTimerIsTimerActive(pTimer->timer) == pdFALSE ) {

        if(xPortIsInsideInterrupt()) {
            BaseType_t xHigherProTskWoken = pdFALSE;
            ret = xTimerChangePeriodFromISR(pTimer->timer, pdMS_TO_TICKS(ms), &xHigherProTskWoken);
        } else {
            ret = xTimerChangePeriod(pTimer->timer, pdMS_TO_TICKS(ms), 100);
        }

        if (ret == pdPASS) {
            return 0;
        } else {
            return -1;
        }
    }

    return ret;
}


sdiowifi_streambuf_handle_t sdiowifi_streambuf_create(size_t buf_size, size_t trigger_bytes)
{
    return xStreamBufferCreate(buf_size, trigger_bytes);
}

int sdiowifi_streambuf_send(sdiowifi_streambuf_handle_t handle, const void *data, size_t len, size_t timeout_ms)
{
    return xStreamBufferSend(handle, data, len, timeout_ms);
}

int sdiowifi_streambuf_receive(sdiowifi_streambuf_handle_t handle, void *data, size_t len, size_t timeout_ms)
{
    return xStreamBufferReceive(handle, data, len, timeout_ms);
}

int sdiowifi_streambuf_spaces_available(sdiowifi_streambuf_handle_t handle)
{
    return xStreamBufferSpacesAvailable(handle);
}

sdiowifi_tick_t sdiowifi_ms2ticks(sdiowifi_tick_t ms)
{
    return pdMS_TO_TICKS(ms);
}

