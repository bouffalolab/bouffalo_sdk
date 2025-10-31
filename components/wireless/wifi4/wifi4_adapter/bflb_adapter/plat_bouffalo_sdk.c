#include <assert.h>
#include <stdio.h>
#include "wifi_mgmr_ext.h"

#include "FreeRTOS.h"
#include "task.h"
#include "platform_al.h"
#include "stdbool.h"

#ifdef CFG_CHIP_BL602 
#include "bl602.h"
#endif
/* User defined wifi event handler */
extern void wifi_event_handler(uint32_t code1, uint32_t code2);
static pfn_wifi_event pfn_event_handler;
static void *p_event_arg;

volatile bool sys_log_all_enable = 1;

/// Stack size of the WIFI task
#define WIFI_TASK_STACK_SIZE 1024

/// Handle of the WiFi task
static TaskHandle_t wifi_task_handle;

/* Main wifi stack entry point */
extern void wifi_main(void *param);

void wifi_task_create(void) {
  BaseType_t res;

  res = xTaskCreate(wifi_main, "wifi fw", WIFI_TASK_STACK_SIZE, NULL, 30,
                    &wifi_task_handle);

  configASSERT(res == pdTRUE);

  vTaskDelay(100);
  platform_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);
}

/* async event handler */
static void async_event_handler(void *arg1, uint32_t arg2)
{
    /* XXX: Prevents blocking operations in the Timer context. */
    vTaskSuspendAll();

    wifi_event_handler(arg2, (uint32_t)arg1);

    xTaskResumeAll();

    input_event_t event = {
        .code = arg2,
    };
    if (pfn_event_handler) {
        pfn_event_handler(&event, (uint32_t)p_event_arg);
    }
}

int platform_register_event(int catalogue, pfn_wifi_event cb, void *arg)
{
    pfn_event_handler = cb;
    p_event_arg = arg;
}

int platform_unregister_event(int catalogue, pfn_wifi_event cb, void *arg)
{
    pfn_event_handler = NULL;
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

int platform_wifi_enable_irq(void)
{
extern void mac_irq(void);
extern void bl_irq_handler(void);
    bflb_irq_attach(WIFI_IRQn, mac_irq, NULL);
    bflb_irq_attach(WIFI_IPC_PUBLIC_IRQn, bl_irq_handler, NULL);
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

