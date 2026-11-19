#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "wifi_mgmr_ext.h"
#include "lwip/tcpip.h"
#include "rfparam_adapter.h"
#include "rfparam_adapter_internal.h"
#include "sysinit.h"

#define DBG_TAG "WIFI_INIT"
#include "log.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "platform_al.h"
#include "bflb_irq.h"
#include "bl602_aon.h"
#include "bflb_sec_trng.h"
#include <stdbool.h>

#ifdef CFG_CHIP_BL602
#include "bl602.h"
#endif
/* User defined wifi event handler */
extern void wifi_event_handler(uint32_t code1, uint32_t code2);
static pfn_wifi_event pfn_event_handler;

volatile bool sys_log_all_enable = 1;

/// Stack size of the WIFI task
#define WIFI_TASK_STACK_SIZE 1024

/* Priority window used while the SYSINIT continuation task starts Wi-Fi. The
 * Wi-Fi firmware task runs at priority 30, so the continuation task is lowered
 * below it until both the firmware and the manager report ready. */
#ifndef WIFI4_WIFI_STARTUP_PRIORITY
#define WIFI4_WIFI_STARTUP_PRIORITY 10
#endif
#ifndef WIFI4_APP_EVENTS_PRIORITY
#define WIFI4_APP_EVENTS_PRIORITY 1
#endif

#if !defined(INCLUDE_vTaskPrioritySet) || (INCLUDE_vTaskPrioritySet != 1)
#error "WiFi4 SYSINIT bring-up requires INCLUDE_vTaskPrioritySet = 1 in FreeRTOSConfig.h"
#endif

/// Handle of the WiFi task
static TaskHandle_t wifi_task_handle;

/* Main wifi stack entry point */
extern void wifi_main(void *param);

#if IS_ENABLED(CONFIG_COMPAT_V2)
__attribute__((deprecated("handled by SYSINIT; will be removed in Bouffalo SDK v3")))
void wifi_task_create(void)
{
    printf("[W] deprecated: %s() is handled by SYSINIT, will be removed in Bouffalo SDK v3\r\n", __func__);
}

#endif /* IS_ENABLED(CONFIG_COMPAT_V2) */
static void _wifi_task_create(void) {
  BaseType_t res;

  wifi_hosal_pm_init();
  res = xTaskCreate(wifi_main, "wifi fw", WIFI_TASK_STACK_SIZE, NULL, 30,
                    &wifi_task_handle);

  configASSERT(res == pdTRUE);

  vTaskDelay(100);
  platform_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE_SYS, 0);
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

static void wifi_async_event(async_input_event_t event, void *code2)
{
    platform_event_t ev = {
        .code = event->code,
    };
    if (pfn_event_handler) {
        pfn_event_handler(&ev, code2);
    }
}

int platform_register_event(int catalogue, pfn_wifi_event cb, void *arg)
{
    pfn_event_handler = cb;
    async_register_event_filter(EV_WIFI, wifi_async_event, arg);
    return 0;
}

int platform_unregister_event(int catalogue, pfn_wifi_event cb, void *arg)
{
    async_unregister_event_filter(EV_WIFI, wifi_async_event, arg);
    pfn_event_handler = NULL;
    return 0;
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

int platform_wifi_enable_irq(void)
{
extern void mac_irq(void);
extern void bl_irq_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)mac_irq, NULL);
    bflb_irq_attach(WIFI_IPC_PUBLIC_IRQn, (irq_callback)bl_irq_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);
    bflb_irq_enable(WIFI_IPC_PUBLIC_IRQn);

    //NVIC_SetPriority((IRQn_Type)5, 0);
    //NVIC_EnableIRQ((IRQn_Type)5);
    puts("Enable BMX IRQ\r\n");
    //NVIC_EnableIRQ((IRQn_Type)0);
    //NVIC_EnableIRQ((IRQn_Type)1);
    //NVIC_EnableIRQ((IRQn_Type)2);
    //NVIC_EnableIRQ((IRQn_Type)3);
    //NVIC_EnableIRQ((IRQn_Type)4);
    //*(uint32_t*)0x40000050 = ((0xF << 28) | (1 << 24));

    return 0;
}

void platform_post_delayed_action(int delay_ms, void (*callback)(void* arg), void *arg)
{

}

void platform_sys_capcode_update(uint8_t capin, uint8_t capout)
{

}

uint8_t platform_sys_capcode_get(void)
{
    return AON_Get_Xtal_CapCode();
}

int platform_get_random(unsigned char *buf, size_t len)
{
    int ret;

    taskENTER_CRITICAL();
    ret = bflb_trng_readlen(buf, len);
    taskEXIT_CRITICAL();
    return ret;
}

long platform_rand(void)
{
    return random();
}

static bool wifi_firmware_ready;
static bool wifi_manager_ready;

static void wifi_sysinit_event_handler(async_input_event_t event, void *priv)
{
    (void)priv;

    if (event->code == CODE_WIFI_ON_INIT_DONE_SYS) {
        __atomic_store_n(&wifi_firmware_ready, true, __ATOMIC_RELEASE);
    }
    if (event->code == CODE_WIFI_ON_MGMR_DONE_SYS) {
        __atomic_store_n(&wifi_manager_ready, true, __ATOMIC_RELEASE);
    }
}

static void wifi_sysinit_events(const void *arg)
{
    (void)arg;
    /* The async event loop must be ready before the synchronous Wi-Fi
     * bring-up and the application main(). */
    async_event_init(async_event_loop_wake);
    if (async_register_event_filter(EV_WIFI, wifi_sysinit_event_handler, NULL) != 0) {
        abort();
    }
}
SYSINIT(wifi_events, SI_SUB_EVENTS, SI_ORDER_FIRST, wifi_sysinit_events, NULL);

static void wifi_sysinit_rf(void)
{
    if (_rfparam_init(0, NULL, 0) != 0) {
        LOG_E("PHY RF init failed!\r\n");
        abort();
    }
}

extern int _wifi_mgmr_task_start(void);

static void wifi_sysinit_start(const void *arg)
{
    (void)arg;

    /* Runs synchronously before the application main(); the async event loop is
     * already initialized (SI_SUB_EVENTS). */
    wifi_sysinit_rf();

    /* The SYSINIT continuation task runs at configMAX_PRIORITIES - 1, above the
     * Wi-Fi firmware task (30). Lower it so the firmware preempts this task
     * right after _wifi_task_create() and completes its own initialization
     * before the host sends the first command. */
    vTaskPrioritySet(NULL, WIFI4_WIFI_STARTUP_PRIORITY);

    LOG_I("Starting wifi ...\r\n");
    /* The SDK owns TCP/IP initialization; WIRELESS runs before SI_SUB_APP. */
    _tcpip_init(NULL, NULL);
    _wifi_task_create();

    while (!__atomic_load_n(&wifi_firmware_ready, __ATOMIC_ACQUIRE)) {
        vTaskDelay(1);
    }
    if (_wifi_mgmr_task_start() != 0) {
        abort();
    }
    /* Wait for the manager so main() sees a fully initialized Wi-Fi stack. */
    while (!__atomic_load_n(&wifi_manager_ready, __ATOMIC_ACQUIRE)) {
        vTaskDelay(1);
    }
    /* Back to the priority used by sysinit_freertos.c for the continuation
     * task; the remaining WIRELESS/NET/APP records must not be preempted by
     * application tasks. */
    vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);
}
SYSINIT(wifi_start, SI_SUB_WIRELESS, SI_ORDER_FIRST, wifi_sysinit_start, NULL);

/* Application-facing compatibility events. This record runs after the APP
 * main() record (SI_SUB_APP/MIDDLE), so application filters and tasks created
 * by main() exist. The SYSINIT task is temporarily lowered to let application
 * tasks run while the events are queued, then restored for the remaining
 * records. The events stay isolated from the internal CODE_WIFI_ON_*_SYS
 * events. */
#if IS_ENABLED(CONFIG_COMPAT_V2)
static void wifi_app_events(const void *arg)
{
    (void)arg;

    vTaskPrioritySet(NULL, WIFI4_APP_EVENTS_PRIORITY);
    if (async_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0) != 0 ||
        async_post_event(EV_WIFI, CODE_WIFI_ON_MGMR_DONE, 0) != 0) {
        abort();
    }
    vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);
}
SYSINIT(wifi_app_events, SI_SUB_APP, SI_ORDER_LAST, wifi_app_events, NULL);
#endif /* IS_ENABLED(CONFIG_COMPAT_V2) */
