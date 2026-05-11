#include <assert.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <lmac154.h>

#include <openthread_port.h>
#include <openthread/tasklet.h>
#include <ot_radio_trx.h>

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
    if (xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(ot_taskHandle, 0, eSetBits, &xHigherPriorityTaskWoken);
    }
    else {
        xTaskNotify(ot_taskHandle, 0, eSetBits);
    }
}

void otrNotifyEvent(ot_system_event_t sevent)
{
    if (xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(ot_taskHandle, (uint32_t)sevent, eSetBits, &xHigherPriorityTaskWoken);
    }
    else {
        xTaskNotify(ot_taskHandle, (uint32_t)sevent, eSetBits);
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

void otrStackInit(void)
{
    ot_radio_ctx.instance = otInstanceInitSingle();
    assert(ot_radio_ctx.instance);
}

static void otrStackTask(void *p_arg)
{
    (void)p_arg;

    otrLock();
    otrUnlock();

    OT_THREAD_SAFE (
        ot_alarmInit();
        ot_radioInit();
        otrStackInit();

        otrInitUser(ot_radio_ctx.instance);
    );

    while (true)
    {
        ot_system_event_t sevent = OT_SYSTEM_EVENT_NONE;
        xTaskNotifyWait(0, 0xFFFFFFFF, (uint32_t *)&sevent, portMAX_DELAY);

        OT_THREAD_SAFE (
            otTaskletsProcess(ot_radio_ctx.instance);
            ot_alarmTask(sevent);
            ot_radioTask(sevent);
            ot_serialProcess(sevent);
            otbr_event_process(sevent);
            otrAppProcess(sevent);
            otbr_netif_process(ot_radio_ctx.instance);
        );
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
