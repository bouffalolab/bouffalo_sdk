#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <utils_list.h>
#if defined(CONFIG_BL_SDK)
#include <bflb_mtimer.h>
#include <bflb_timer.h>
#else
#include <bl_timer.h>
#endif
#include <openthread_port.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/alarm-micro.h>

#include <timers.h>
#include <ot_utils_ext.h>

#define OT_TIMER_MS_TO_TICKS(xTimeInMs) ((xTimeInMs) / 1000 * configTICK_RATE_HZ + ((xTimeInMs) % 1000) * configTICK_RATE_HZ / 1000)

#define OT_ALARM_TIMER_MARGIN                   30   /** 30 us */
#define OT_ALARM_TIMER_ID                       0

TimerHandle_t otAlarm_timerHandle = NULL;
uint32_t      otAlarm_offset = 0;
#if defined(CONFIG_BL_SDK)
struct bflb_device_s *bflb_timer = NULL;
#endif

static void otPlatALarm_msTimerCallback( TimerHandle_t xTimer ) 
{
    otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_MS_EXPIRED);
}

#if defined(CONFIG_BL_SDK)
void otAlarm_microTimerCallback(int irq, void *arg)
{
    if (bflb_timer && bflb_timer_get_compint_status(bflb_timer, OT_ALARM_TIMER_ID)) {
        bflb_timer_compint_mask(bflb_timer, OT_ALARM_TIMER_ID, true);
        bflb_timer_compint_clear(bflb_timer, OT_ALARM_TIMER_ID);
        otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
    }
}
#else
void otAlarm_microTimerCallback(void) 
{
    otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
}
#endif

void ot_alarmInit(void) 
{
    otAlarm_timerHandle = xTimerCreate("ot_timer", 1, pdFALSE, (void *)otAlarm_timerHandle, otPlatALarm_msTimerCallback);

    uint32_t tag = otrEnterCrit();

#if defined(CONFIG_BL_SDK)

    bflb_timer = bflb_device_get_by_name("timer0");

    struct bflb_timer_config_s cfg0;
    cfg0.counter_mode = TIMER_COUNTER_MODE_PROLOAD;
    cfg0.clock_source = TIMER_CLKSRC_XTAL;
    cfg0.clock_div = 31;
    cfg0.trigger_comp_id = TIMER_COMP_NONE;
    cfg0.comp0_val = 0xFFFFFFFF;
    cfg0.comp1_val = 0xFFFFFFFF;
    cfg0.comp2_val = 0xFFFFFFFF;
    cfg0.preload_val = bflb_mtimer_get_time_us();

    bflb_timer_init(bflb_timer, &cfg0);

    bflb_irq_attach(bflb_timer->irq_num, otAlarm_microTimerCallback, NULL);
    bflb_irq_enable(bflb_timer->irq_num);

    bflb_timer_start(bflb_timer);

    otAlarm_offset = (uint32_t)bflb_mtimer_get_time_us() - bflb_timer_get_countervalue(bflb_timer);
#else
    bl_timer_restore_time(bl_timer_now_us64());
    otAlarm_offset = (uint32_t)bl_timer_now_us64() - bl_timer_get_current_time();
#endif

    otrExitCrit(tag);
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    BaseType_t ret;
#if defined(CONFIG_BL_SDK)
    uint32_t elapseTime = (uint32_t)(bflb_mtimer_get_time_us() / 1000) - aT0;
#else
    uint32_t elapseTime = (uint32_t)(bl_timer_now_us64() / 1000) - aT0;
#endif

    if (elapseTime < aDt) {

        ret = xTimerChangePeriod( otAlarm_timerHandle, OT_TIMER_MS_TO_TICKS(aDt - elapseTime), 0 );
        configASSERT(ret == pdPASS);

        return;
    }

    otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_MS_EXPIRED);
}

void otPlatAlarmMilliStop(otInstance *aInstance) 
{
    if (xTimerIsTimerActive(otAlarm_timerHandle) == pdTRUE) {
        xTimerStop(otAlarm_timerHandle, 0 );
    }
}

uint32_t otPlatAlarmMilliGetNow(void) 
{
#if defined(CONFIG_BL_SDK)
    return (uint32_t)(bflb_mtimer_get_time_us() / 1000);
#else
    return (uint32_t)(bl_timer_now_us64() / 1000);
#endif
}

uint64_t otPlatTimeGet(void)
{
#if defined(CONFIG_BL_SDK)
    return bflb_mtimer_get_time_us();
#else
    return bl_timer_now_us64();
#endif
}

uint16_t otPlatTimeGetXtalAccuracy(void)
{
    return OPENTHREAD_CONFIG_PLATFORM_XTAL_ACCURACY;
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    uint32_t tag = otrEnterCrit();
#if defined(CONFIG_BL_SDK)
    uint32_t now = bflb_mtimer_get_time_us();

    if (aDt > OT_ALARM_TIMER_MARGIN &&  now - aT0 < aDt - OT_ALARM_TIMER_MARGIN) {
        bflb_timer_set_compvalue(bflb_timer, OT_ALARM_TIMER_ID, aT0 + aDt - otAlarm_offset);
        bflb_timer_compint_mask(bflb_timer, OT_ALARM_TIMER_ID, false);
    }
#else
    uint32_t now = bl_timer_now_us64();

    if (aDt > OT_ALARM_TIMER_MARGIN &&  now - aT0 < aDt - OT_ALARM_TIMER_MARGIN) {
        bl_timer_start(OT_ALARM_TIMER_ID, aT0 + aDt - otAlarm_offset, otAlarm_microTimerCallback);
    }
#endif

    otrExitCrit(tag);
    otrNotifyEvent(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
#if defined(CONFIG_BL_SDK)
    bflb_timer_compint_mask(bflb_timer, OT_ALARM_TIMER_ID, true);
    bflb_timer_compint_clear(bflb_timer, OT_ALARM_TIMER_ID);
#else
    bl_timer_stop(OT_ALARM_TIMER_ID);
#endif
}

uint32_t otPlatAlarmMicroGetNow(void)
{
#if defined(CONFIG_BL_SDK)
    return bflb_mtimer_get_time_us();
#else
    return bl_timer_now_us64();
#endif
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
