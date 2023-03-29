
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>

#include "platform_al.h"

#include "FreeRTOS.h"
#include "timers.h"

#include <bflb_efuse.h>

#ifdef LP_APP
#include "bl_lp.h"
#include "bl616_clock.h"
#include "mac_frame.h"

extern int lpfw_recal_rc32k(uint64_t beacon_timestamp_now_us, uint64_t rtc_timestamp_now_us, uint32_t mode);
extern int32_t lpfw_calculate_beacon_delay(uint64_t beacon_timestamp_us, uint64_t rtc_timestamp_us, uint32_t mode);

#endif

/* User defined wifi event handler */
extern void wifi_event_handler(uint32_t code);

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
    uint8_t tmp[8];

    bflb_efuse_get_chipid(tmp);

    mac[0] = tmp[0];
    mac[1] = tmp[1];
    mac[2] = tmp[2];
    mac[3] = tmp[3];
    mac[4] = tmp[4];
    mac[5] = tmp[5];

    return 0;
}

/**
****************************************************************************************
* @brief Wrapper of platform malloc/free
****************************************************************************************
*/
void *platform_malloc(uint32_t size)
{
    return malloc(size);
}

void platform_free(void* mem_ptr)
{
    free(mem_ptr);
}

/* async event handler */
static void async_event_handler(void *arg1, uint32_t arg2)
{
    /* XXX: Prevents blocking operations in the Timer context. */
    vTaskSuspendAll();

    wifi_event_handler(arg2);

    xTaskResumeAll();
}

/**
****************************************************************************************
* @brief Post Event to upper layer
*
* @param[in] catalogue Type of event.
* @param[in] code Code of event.
****************************************************************************************
*/

void platform_post_event(int catalogue, int code, ...)
{
    BaseType_t xReturn;

    xReturn = xTimerPendFunctionCall(async_event_handler, NULL, code, portMAX_DELAY);
    configASSERT(xReturn == pdPASS);
}

/**
****************************************************************************************
* @brief hook for receive beacon
****************************************************************************************
*/
#ifdef LP_APP

void platform_hook_beacon(uint32_t rhd, uint32_t tim)
{
    uint64_t rtc_cnt, rtc_stamp_us;
    uint64_t beacon_stamp_us;

    struct bcn_frame *bcn = (void *)rhd;

    /* get rtc timestamp */
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    rtc_stamp_us = BL_PDS_CNT_TO_US(rtc_cnt);

    /* get beacon timestamp */
    beacon_stamp_us = bcn->tsf;

    /* rc32k recal */
    lpfw_recal_rc32k(beacon_stamp_us, rtc_stamp_us, BEACON_STAMP_APP);

    /* calculate_beacon_delay, and update rtc timestamp */
    lpfw_calculate_beacon_delay(beacon_stamp_us, rtc_stamp_us, BEACON_STAMP_APP);
}

void platform_hook_prevent_sleep(enum PSM_EVENT event, uint8_t prevent)
{
    if (prevent) {
        bl_pm_event_bit_set(event);
    } else {
        bl_pm_event_bit_clear(event);
    }
}
#endif
