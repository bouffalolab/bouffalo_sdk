#include <assert.h>
#include <stdint.h>
#include <stdarg.h>

#include "platform_al.h"
#include "async_event.h"
#include "wifi_mgmr_ext.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "macsw.h"

#include <bflb_efuse.h>
#include <bflb_sec_trng.h>
#include "mm.h"

#if defined(BL616)
#include <bl616_mfg_media.h>
#elif defined(BL616CL)
#include <bl616cl_mfg_media.h>
#elif defined(BL618DG)
#include <bl618dg_mfg_media.h>
#endif

#ifdef LP_APP
#include "bl_lp.h"
#include "macsw.h"
#endif

/**
 ****************************************************************************************
 * @brief Get MAC Address
 *
 * @param[out] mac Buffer of hold mac address.
 * @return 0 on success and OTHERS if error occurred.
 ****************************************************************************************
 */

int platform_get_mac(uint8_t *mac)
{
    if (0 == mfg_media_read_macaddr_with_lock(mac, 1)) {
        return 0;
    }
    return -1;
}

int platform_get_random(unsigned char *buf, size_t len)
{
    int ret;

    taskENTER_CRITICAL();
    ret = bflb_trng_readlen(buf, len);
    taskEXIT_CRITICAL();
    return ret;
}

/**
****************************************************************************************
* @brief Wrapper of platform malloc/free
****************************************************************************************
*/
void *rtos_malloc(uint32_t size)
{
    void *ptr;

    ptr = kmalloc(size, 0);
    if (NULL == ptr) {
        abort();
    }
    return ptr;
}

void *rtos_calloc(uint32_t nb_elt, uint32_t size)
{
    void *ptr;

    ptr = kmalloc(nb_elt * size, MM_FLAG_PROP_ZERO);
    if (NULL == ptr) {
        abort();
    }
    return ptr;
}

void rtos_free(void *ptr)
{
    kfree(ptr);
}

void *platform_malloc(uint32_t size)
{
    void *ret = kmalloc(size, 0);
    // TODO sleep and retry able
    if (ret == NULL) {
        abort();
    }
    return ret;
}

void platform_free(void *mem_ptr)
{
    kfree(mem_ptr);
}

/* async event handler */
static void async_event_handler(void *arg1, uint32_t arg2)
{
    /* XXX: Prevents blocking operations in the Timer context. */
    vTaskSuspendAll();
    async_event_loop();
    xTaskResumeAll();
}

static void async_event_loop_wake(void)
{
    BaseType_t xReturn;
    TickType_t wait = portMAX_DELAY;

    if (xTimerGetTimerDaemonTaskHandle() == xTaskGetCurrentTaskHandle()) {
        wait = 0;
    }

    xReturn = xTimerPendFunctionCall(async_event_handler, (void *)NULL, 0, wait);
    configASSERT(xReturn == pdPASS);
}

/**
****************************************************************************************
* @brief Post Event to upper layer
*
* @param[in] catalogue Type of event.
* @param[in] code Code of event.
****************************************************************************************
*/

void platform_post_event(int catalogue, int code1, int code2)
{
    async_event_init(async_event_loop_wake);
    async_post_event(EV_WIFI, code1, code2);
}

#ifdef CFG_LPM
uint8_t platform_get_chip_version(void)
{
    bflb_efuse_device_info_type device_info;

    bflb_efuse_get_device_info(&device_info);
    return device_info.version;
}
#endif
