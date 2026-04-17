#include <assert.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <lmac154.h>

#include <openthread_port.h>
#include <openthread/tasklet.h>
#include <ot_radio_trx.h>

ot_system_event_t                   ot_system_event_var = OT_SYSTEM_EVENT_NONE;
static SemaphoreHandle_t            ot_extLock          = NULL;
static TaskHandle_t                 ot_taskHandle       = NULL;

uint32_t otrEnterCrit(void) 
{
    if (xPortIsInsideInterrupt()) {
        return taskENTER_CRITICAL_FROM_ISR();
    }
    else {
        taskENTER_CRITICAL();
        return 0;
    }
}

void otrExitCrit(uint32_t tag) 
{
    if (xPortIsInsideInterrupt()) {
        taskEXIT_CRITICAL_FROM_ISR(tag);
    }
    else {
        taskEXIT_CRITICAL();
    }
}

ot_system_event_t otrGetNotifyEvent(void) 
{
    ot_system_event_t sevent = 0;

    taskENTER_CRITICAL();
    sevent = ot_system_event_var;
    ot_system_event_var = 0;
    taskEXIT_CRITICAL();

    return sevent;
}

void otrLock(void)
{
    xSemaphoreTakeRecursive(ot_extLock, portMAX_DELAY);
}

void otrUnlock(void)
{
    xSemaphoreGiveRecursive(ot_extLock);
}

bool otrIsThreadTask(void) 
{
    return ot_taskHandle == xTaskGetCurrentTaskHandle();
}

void otSysEventSignalPending(void)
{
    if (xPortIsInsideInterrupt())
    {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR( ot_taskHandle, &pxHigherPriorityTaskWoken);
    }
    else
    {
        xTaskNotifyGive(ot_taskHandle);
    }
}

void otrNotifyEvent(ot_system_event_t sevent) 
{
    if (xPortIsInsideInterrupt()) {
        ot_system_event_var |= sevent;
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR( ot_taskHandle, &pxHigherPriorityTaskWoken);
    }
    else {
        taskENTER_CRITICAL();
        ot_system_event_var |= sevent;
        taskEXIT_CRITICAL();
        xTaskNotifyGive(ot_taskHandle);
    }
}

__attribute__((weak)) void ot_serialProcess(ot_system_event_t sevent) {}
__attribute__((weak)) void otrAppProcess(ot_system_event_t sevent) {}
__attribute__((weak)) void otrInitUser(otInstance * instance) {}
__attribute__((weak)) void otbr_netif_process(otInstance *aInstance) {}
__attribute__((weak)) void otbr_event_process(ot_system_event_t sevent) {}

void otTaskletsSignalPending(otInstance *aInstance)
{
    if (aInstance) {
        otrNotifyEvent(OT_SYSTEM_EVENT_OT_TASKLET);
    }
}

otInstance *otrGetInstance()
{
    return ot_radio_ctx.instance;
}

void otSysProcessDrivers(otInstance *aInstance) 
{
    ot_system_event_t sevent = otrGetNotifyEvent();

    ot_alarmTask(sevent);
    ot_radioTask(sevent);
    ot_serialProcess(sevent);
    otbr_event_process(sevent);
    otrAppProcess(sevent);
}

void otrStackInit(void)
{
    ot_radio_ctx.instance = otInstanceInitSingle();
    assert(ot_radio_ctx.instance);
}

static void otrStackTask(void *p_arg)
{
    /** This task is an example to handle both main event loop of openthread task lets and
     * hardware drivers for openthread, such as radio, alarm timer and also uart shell.
     * Customer can implement own task for both of two these missions with other privoded APIs.  */
    (void)p_arg;

    otrLock();
    otrUnlock();

    ot_system_event_var = OT_SYSTEM_EVENT_NONE;

    OT_THREAD_SAFE (
        ot_alarmInit();
        ot_radioInit();
        otrStackInit();

        otrInitUser(ot_radio_ctx.instance);
    );

    while (true)
    {
        OT_THREAD_SAFE (
            otTaskletsProcess(ot_radio_ctx.instance);
            otSysProcessDrivers(ot_radio_ctx.instance);
            otbr_netif_process(ot_radio_ctx.instance);
        );
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }

    otInstanceFinalize(ot_radio_ctx.instance);
    ot_radio_ctx.instance = NULL;

    vTaskDelete(NULL);
}

void otrStart(void)
{
    ot_extLock = xSemaphoreCreateMutex();
    assert(ot_extLock != NULL);

    xTaskCreate(otrStackTask, "threadTask", OT_TASK_SIZE, NULL,
                OT_TASK_PRORITY, &ot_taskHandle);
}
