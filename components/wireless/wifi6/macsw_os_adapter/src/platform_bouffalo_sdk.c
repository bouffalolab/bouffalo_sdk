#include <assert.h>
#include <stdint.h>
#include <stdarg.h>

#include "platform_al.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "macsw.h"

#include <bflb_efuse.h>
#include <bflb_sec_trng.h>

#include "mm.h"

#if defined(BL616)
#include <bl616_mfg_media.h>
#elif defined(BL616L)
#include <bl616l_mfg_media.h>
#elif defined(BL616D)
#include <bl616d_mfg_media.h>
#endif

#ifdef LP_APP
#include "bl_lp.h"
#include "bl616_clock.h"
#include "macsw.h"

extern int lpfw_recal_rc32k(uint64_t beacon_timestamp_now_us, uint64_t rtc_timestamp_now_us, uint32_t mode, int clock_ready_check);
extern int32_t lpfw_calculate_beacon_delay(uint64_t beacon_timestamp_us, uint64_t rtc_timestamp_us, uint32_t mode);
// extern int32_t lpfw_beacon_delay_sliding_win_update(int32_t beacon_delay_us, uint64_t beacon_timestamp_us);

extern int bl_lp_beacon_interval_update(uint16_t beacon_interval_tu);
extern int bl_lp_beacon_tim_update(uint8_t *tim, uint8_t mode);
#endif

/* User defined wifi event handler */
extern void wifi_event_handler(uint32_t code1, uint32_t code2);

/* FIXME: Registers should not be read directly */
/// Address of the MONOTONIC_COUNTER_2_LO register
#define MACSW_MAC_TSF_TIMER_LO_ADDR 0x24B080A4
#define REG_PL_RD(addr)             (*(volatile uint32_t *)(addr))
#define MAC_TSF_TIMER_LOW           REG_PL_RD(MACSW_MAC_TSF_TIMER_LO_ADDR)

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

    wifi_event_handler(arg2, (uint32_t)arg1);

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

void platform_post_event(int catalogue, int code1, int code2)
{
    BaseType_t xReturn;

    xReturn = xTimerPendFunctionCall(async_event_handler, (void *)code2, code1, portMAX_DELAY);
    configASSERT(xReturn == pdPASS);
}

/**
****************************************************************************************
* @brief hook for receive beacon
****************************************************************************************
*/
#ifdef LP_APP
uint8_t platform_get_leg_rate(uint8_t rate)
{
    // Retrieve the rate values from the RX vector
    // printf("rate:%d\r\n",rate);
    uint8_t leg_rate = 0;

    switch (rate) {
        case 0: //LM_1M:
            leg_rate = 2;
            break;
        case 1: //LM_2M:
            leg_rate = 4;
            break;
        case 2: //LM_5_5M:
            leg_rate = 11;
            break;
        case 3: //LM_11M:
            leg_rate = 22;
            break;
        case 11: //LM_6MBPS:
            leg_rate = 12;
            break;
        case 15: //LM_9MBPS:
            leg_rate = 18;
            break;
        case 10: //LM_12MBPS:
            leg_rate = 24;
            break;
        case 14: //LM_18MBPS:
            leg_rate = 36;
            break;
        case 9: //LM_24MBPS:
            leg_rate = 48;
            break;
        case 13: //LM_36MBPS:
            leg_rate = 72;
            break;
        case 8: //LM_48MBPS:
            leg_rate = 96;
            break;
        case 12: //LM_54MBPS:
            leg_rate = 108;
            break;
        default:
            leg_rate = 2;
            break;
    }
    return leg_rate;
}

void platform_hook_beacon(uint32_t rhd, uint32_t tim, bcn_param_t *param)
{
    uint64_t rtc_cnt, rtc_stamp_us;
    uint64_t beacon_stamp_us;
    int32_t beacon_delay_us;

    // struct rx_pbd *pbd = HW2CPU(((struct rx_hd *)rhd)->first_pbd_ptr);
    // struct bcn_frame *bcn = HW2CPU(pbd->datastartptr);
    struct bcn_frame *bcn = (void *)rhd;

    /* FIXME: The method of disabling global interrupts is not appropriate */
    /* disable int */
    int irq_rest;
    asm volatile("csrrci %0, mstatus, %1" : "=r"(irq_rest) : "i"(1UL << 3));
    /* get rtc cnt */
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    /* get mac time low */
    uint32_t mac_time_now = MAC_TSF_TIMER_LOW;
    /* restore int */
    asm volatile("csrw mstatus, %0" : /* no output */ : "r"(irq_rest));

    /* update bcn rate */
    uint8_t bcn_rate = platform_get_leg_rate(param->beacon_rate);
    BEACON_DATA_RATE = bcn_rate;

    /* get rtc timestamp */
    rtc_stamp_us = BL_PDS_CNT_TO_US(rtc_cnt);

    /* get beacon timestamp */
    beacon_stamp_us = bcn->tsf;

    uint32_t rx_delay_us = (param->beacon_len - 32) * 8 * 2 / bcn_rate; /* delay of (rx_start -> rx_end) */
    uint32_t proce_delay_us = (mac_time_now - param->mac_local_tsf_l);  /* delay of (rx_end -> now) */

    /* Compensate for the delay error */
    rtc_stamp_us = rtc_stamp_us - proce_delay_us - rx_delay_us;

    /************** TODO:  rc32k debug info ******************/
#if 0
    // printf("app rc32k recal %d:\r\n", ret);
    printf("bcn len %ld, ", (uint32_t)param->beacon_len);
    printf("bcn rate %ld, ", (uint32_t)bcn_rate);

    printf("bcn tsf %ld, ", (uint32_t)beacon_stamp_us);
    printf("rtc tsf %ld, ", (uint32_t)rtc_stamp_us);
    printf("rx delay %ld, ", rx_delay_us);
    printf("proce delay %ld\r\n", proce_delay_us);
    if (iot2lp_para->last_rc32trim_stamp_valid) {
        printf("rc32k code %ld, ", iot2lp_para->rc32k_fr_ext);
        printf("rc32k ppm %ld, ", iot2lp_para->rtc32k_error_ppm);
        printf("bcn diff %lld, ", beacon_stamp_us - iot2lp_para->last_rc32trim_stamp_beacon_us);
        printf("rtc diff %lld\r\n", rtc_stamp_us - iot2lp_para->last_rc32trim_stamp_rtc_us);
    }
#endif

    /* rc32k recal */
    lpfw_recal_rc32k(beacon_stamp_us, rtc_stamp_us, BEACON_STAMP_APP, 1);

    /* calculate_beacon_delay, and update rtc timestamp */
    beacon_delay_us = lpfw_calculate_beacon_delay(beacon_stamp_us, rtc_stamp_us, BEACON_STAMP_APP);
    /* update beacon delay sliding_window  */
    // lpfw_beacon_delay_sliding_win_update(beacon_delay_us, beacon_stamp_us);

    /* update beacon interval */
    bl_lp_beacon_interval_update(bcn->bcnint);
    /* update beacon tim */
    bl_lp_beacon_tim_update((uint8_t *)tim, BEACON_STAMP_APP);

    /************** TODO:  beacon loss debug info ******************/
#if 0
    volatile iot2lp_para_t *iot2lp_para = (iot2lp_para_t *)IOT2LP_PARA_ADDR;
    static uint64_t beacon_stamp_last = 0;

    if(beacon_stamp_last == 0){
        printf("APP Frist beacon timestamp %lld\r\n", beacon_stamp_us);
        beacon_stamp_last = beacon_stamp_us;
        return;
    }

    /* interval */
    uint32_t interval = (beacon_stamp_us - beacon_stamp_last + 51200) / 102400;

    /* The number that should be received */
    iot2lp_para->app_recv_cnt += interval;

    /* loss cnt */
    iot2lp_para->app_loss_cnt += interval - 1;

    printf("APP beacon timestamp %lld, ", beacon_stamp_us);
    printf("want receive %ld, loss %ld, ", iot2lp_para->app_recv_cnt, iot2lp_para->app_loss_cnt);
    if (iot2lp_para->app_recv_cnt) {
        printf("loss rate %ld.%ld%%, ", iot2lp_para->app_loss_cnt * 100 / iot2lp_para->app_recv_cnt, (iot2lp_para->app_loss_cnt * 10000 / iot2lp_para->app_recv_cnt) % 100);
    }
    // printf("RSSI %d \r\n", ((struct rx_hd *)rhd)->rx_vec_1.rssi1);
    printf("\r\n");

    beacon_stamp_last = beacon_stamp_us;
#endif
    return;
}

void platform_hook_prevent_sleep(enum PSM_EVENT event, uint8_t prevent)
{
    if (prevent) {
        bl_pm_event_bit_set(event);
    } else {
        bl_pm_event_bit_clear(event);
    }
}

/* macsw hook when beacon received */
int macsw_hook_beacon(const char *f, uintptr_t bcn, uint32_t tim, bcn_param_t *beacon_param) {
  (void)f;
  PLATFORM_HOOK(beacon, (uintptr_t)bcn, tim, beacon_param);
  return 0;
}

/* macsw hook when disconnected */
int macsw_hook_evt_disconnected(const char *f) {
  (void)f;
  PLATFORM_HOOK(prevent_sleep, PSM_EVENT_DISCONNECT, 1);
  return 0;
}
#endif

#ifdef CFG_LPM
uint8_t platform_get_chip_version(void)
{
    bflb_efuse_device_info_type device_info;
    bflb_efuse_get_device_info(&device_info);
    return device_info.version;
}
#endif
