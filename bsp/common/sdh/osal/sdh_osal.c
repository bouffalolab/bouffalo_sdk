#include "sdh_osal.h"

#if (SDH_OSAL_TYPE == 1)
/* FreeRTOS */
#include <FreeRTOS.h>
#include "semphr.h"
#else
/* no os */
#include "bflb_mtimer.h"
#include "bflb_irq.h"
#endif

void sdh_osal_delay_ms(uint32_t delay_ms)
{
#if (SDH_OSAL_TYPE == 1)
    /* FreeRTOS */
    vTaskDelay(delay_ms);
#else
    /* no os */
    bflb_mtimer_delay_ms(delay_ms);
#endif
}

#if (CONFIG_BSP_SDH_OSAL_POLLING_MODE == 0)

void *sdh_osal_semb_create(void)
{
    void *semb;
#if (SDH_OSAL_TYPE == 1)
    /* FreeRTOS */
    semb = (void *)xSemaphoreCreateBinary();
#else
    /* no os */
    semb = malloc(sizeof(bool));
    *((volatile bool *)semb) = false;
#endif

    return semb;
}

int sdh_osal_semb_delete(void *semb)
{
#if (SDH_OSAL_TYPE == 1)
    /* FreeRTOS */
    vSemaphoreDelete((SemaphoreHandle_t)semb);
#else
    /* no os */
    free((void *)semb);
#endif
    return 0;
}

int sdh_osal_semb_take(void *semb, uint32_t timeout_ms)
{
    int ret = 0;
#if (SDH_OSAL_TYPE == 1)
    /* FreeRTOS */
    if (xPortIsInsideInterrupt()) {
        ret = -1;
    } else {
        if (xSemaphoreTake((SemaphoreHandle_t)semb, (timeout_ms ? timeout_ms : portMAX_DELAY)) != pdTRUE) {
            ret = -1;
        }
    }

#else
    /* no os */
    uint32_t time = bflb_mtimer_get_time_ms();
    while (*((volatile bool *)semb) == false) {
        if (timeout_ms && (bflb_mtimer_get_time_ms() - time > timeout_ms)) {
            break;
        }
    };
    uintptr_t irq_flag = bflb_irq_save();
    if (*((volatile bool *)semb) == true) {
        *((volatile bool *)semb) = false;
    } else {
        ret = -1;
    }
    bflb_irq_restore(irq_flag);
#endif

    return ret;
}

int sdh_osal_semb_give(void *semb)
{
#if (SDH_OSAL_TYPE == 1)
    /* FreeRTOS */
    if (xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR((SemaphoreHandle_t)semb, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xSemaphoreGive((SemaphoreHandle_t)semb);
    }
#else
    /* no os */
    *((volatile bool *)semb) = true;
#endif
    return 0;
}

#endif
