#include "mimiclaw_port.h"

#include "FreeRTOS.h"
#include "task.h"
#include "bflb_mtimer.h"

uint32_t mimiclaw_port_uptime_ms(void)
{
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

void mimiclaw_port_sleep_ms(uint32_t ms)
{
    BaseType_t sched = xTaskGetSchedulerState();

    if (sched == taskSCHEDULER_RUNNING) {
        vTaskDelay(pdMS_TO_TICKS(ms));
        return;
    }

    /* Some callbacks run with scheduler suspended, where vTaskDelay asserts. */
    bflb_mtimer_delay_ms(ms);
}
