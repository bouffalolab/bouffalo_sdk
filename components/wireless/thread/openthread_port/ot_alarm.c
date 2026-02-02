
#include <assert.h>

#if defined(CONFIG_BL_SDK)
#include <compiler/compiler_ld.h>
#else
#include <compiler/common.h>
#endif

#include <lmac154.h>
#include <zb_timer.h>

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE
#include <openthread_port.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/alarm-micro.h>

#define OT_ALARM_MICRO_TIMER_ID         (3)
#define OT_ALARM_MILLI_TIMER_ID         (2)
#define OT_ALARM_PDS_TIMER_ID           (1)
#define OT_ALARM_US_PER_TICK_BIT        4
#define OT_ALARM_MAX_MILLI_SEC          (((uint64_t)1 << 36) / 1000)

static int ot_alarm_timer_milli_sec_cnt = 0;

void ot_alarmInit(void) {}

ATTR_TCM_SECTION
static void otPlatALarm_msTimerCallback(void) 
{
    ot_alarm_timer_milli_sec_cnt --;

    if (ot_alarm_timer_milli_sec_cnt <= 0) {
        zb_timer_stop(OT_ALARM_MILLI_TIMER_ID);
        otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_MS_EXPIRED);
    }
    else {
        zb_timer_start(OT_ALARM_MILLI_TIMER_ID, zb_timer_get_current_time(), otPlatALarm_msTimerCallback);
    }
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    uint32_t tag, current, delay_cnt;
    uint64_t delta;

    assert(aT0 <= OT_ALARM_MAX_MILLI_SEC);

    zb_timer_stop(OT_ALARM_MILLI_TIMER_ID);

    tag = otrEnterCrit();
    current = ((uint64_t)zb_timer_get_current_time() << OT_ALARM_US_PER_TICK_BIT) / 1000;
    if (current - aT0 < aDt) {
        delta = (uint64_t)(aDt - (current - aT0));
        delay_cnt = ((delta % OT_ALARM_MAX_MILLI_SEC) * 1000) >> OT_ALARM_US_PER_TICK_BIT;
        zb_timer_start(OT_ALARM_MILLI_TIMER_ID, zb_timer_get_current_time() + delay_cnt, otPlatALarm_msTimerCallback);

        ot_alarm_timer_milli_sec_cnt = (delta + OT_ALARM_MAX_MILLI_SEC - 1) / OT_ALARM_MAX_MILLI_SEC;
    }
    else {
        otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_MS_EXPIRED);
    }
    otrExitCrit(tag);
}

void otPlatAlarmMilliStop(otInstance *aInstance) 
{
    zb_timer_stop(OT_ALARM_MILLI_TIMER_ID);
}

uint32_t otPlatAlarmMilliGetNow(void) 
{
    return (uint32_t)(((uint64_t)zb_timer_get_current_time() << OT_ALARM_US_PER_TICK_BIT) / 1000);
}

uint64_t otPlatTimeGet(void)
{
    return (uint64_t)zb_timer_get_current_time() << OT_ALARM_US_PER_TICK_BIT;
}

uint16_t otPlatTimeGetXtalAccuracy(void) { return 0; }

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    return (uint64_t)zb_timer_get_current_time() << OT_ALARM_US_PER_TICK_BIT;
}

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
ATTR_TCM_SECTION
static void otPlatALarm_usTimerCallback(void) 
{
    zb_timer_stop(OT_ALARM_MICRO_TIMER_ID);
    otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    uint32_t tag, current;
    uint64_t delta;

    zb_timer_stop(OT_ALARM_MICRO_TIMER_ID);

    tag = otrEnterCrit();
    current = zb_timer_get_current_time() << OT_ALARM_US_PER_TICK_BIT;
    if (current - aT0 < aDt) {
        delta = (current - aT0 + aDt + (1 << OT_ALARM_US_PER_TICK_BIT) / 2) >> OT_ALARM_US_PER_TICK_BIT;
        zb_timer_start(OT_ALARM_MILLI_TIMER_ID, zb_timer_get_current_time() + delta, otPlatALarm_usTimerCallback);
    }
    else {
        otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
    }
    otrExitCrit(tag);
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    zb_timer_stop(OT_ALARM_MICRO_TIMER_ID);
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    return (uint32_t)((uint64_t)zb_timer_get_current_time() << OT_ALARM_US_PER_TICK_BIT);
}
#endif

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
