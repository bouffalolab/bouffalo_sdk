
#include <bflb_mtimer.h>
#include <lmac154.h>
#include <zb_timer.h>

#include <openthread_port.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/alarm-micro.h>

#include <timers.h>
#include <ot_utils_ext.h>

#define OT_TIMER_MS_TO_TICKS(xTimeInMs) ((xTimeInMs) / 1000 * configTICK_RATE_HZ + ((xTimeInMs) % 1000) * configTICK_RATE_HZ / 1000)

TimerHandle_t otAlarm_timerHandle = NULL;
uint32_t      otAlarm_offset = 0;

void otPlatALarm_msTimerCallback( TimerHandle_t xTimer ) 
{
    otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_MS_EXPIRED);
}

void ot_alarmInit(void) 
{
    otAlarm_timerHandle = xTimerCreate("ot_alarm", 1, pdFALSE, (void *)otAlarm_timerHandle, otPlatALarm_msTimerCallback);
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    BaseType_t ret;
    uint32_t elapseTime = (uint32_t)(bflb_mtimer_get_time_us() / 1000) - aT0;

    if (elapseTime < aDt) {
        ret = xTimerChangePeriod( otAlarm_timerHandle, OT_TIMER_MS_TO_TICKS(aDt - elapseTime), 0 );
        configASSERT(ret == pdPASS);
    }
    else {
        otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_MS_EXPIRED);
    }
}

void otPlatAlarmMilliStop(otInstance *aInstance) 
{
    if (xTimerIsTimerActive(otAlarm_timerHandle) == pdTRUE) {
        xTimerStop(otAlarm_timerHandle , 0 );
    }
}

uint32_t otPlatAlarmMilliGetNow(void) 
{
    return (uint32_t)(bflb_mtimer_get_time_us() / 1000);
}

uint64_t otPlatTimeGet(void)
{
    return bflb_mtimer_get_time_us();
}

static void otPlatALarm_usTimerCallback(void) 
{
    zb_timer_stop(LMAC154_ALARM_MICRO_TIMER_ID);
    otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    uint32_t tag, now, target;

    tag = otrEnterCrit();

    now = zb_timer_get_current_time();
    if ((now << LMAC154_US_PER_SYMBOL_BITS) - (aT0 & 0xfffffff0) < aDt) {

        /** get target time on zb_timer metric */
        target = aT0 + aDt - (now << LMAC154_US_PER_SYMBOL_BITS) + LMAC154_US_PER_SYMBOL / 2;
        target = (target >> LMAC154_US_PER_SYMBOL_BITS) + now;

        zb_timer_start(LMAC154_ALARM_MICRO_TIMER_ID, target, otPlatALarm_usTimerCallback);

        /** check whether to be expired again */
        now = zb_timer_get_current_time();
        if ((now << LMAC154_US_PER_SYMBOL_BITS) - (aT0 & 0xfffffff0) < aDt) {
            otrExitCrit(tag);
            return;
        }
    }

    zb_timer_stop(LMAC154_ALARM_MICRO_TIMER_ID);
    otrExitCrit(tag);

    otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    zb_timer_stop(LMAC154_ALARM_MICRO_TIMER_ID);
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    return zb_timer_get_current_time() << LMAC154_US_PER_SYMBOL_BITS;
}

void ot_alarmTask(ot_system_event_t sevent) 
{
    if (!(OT_SYSTEM_EVENT_ALARM_ALL_MASK & sevent)) {
        return;
    }

    if (OT_SYSTEM_EVENT_ALARM_MS_EXPIRED & sevent) 
    {
        otPlatAlarmMilliFired(otrGetInstance());
    }

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    if (OT_SYSTEM_EVENT_ALARM_US_EXPIRED & sevent) 
    {
        otPlatAlarmMicroFired(otrGetInstance());
    }
#endif
}
