/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "hal_uart.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "bluetooth.h"
#include "gap.h"
#include "bl702_glb.h"
#include "ble_peripheral_tp_server.h"

extern uint8_t _heap_start;
extern uint8_t _heap_size; // @suppress("Type cannot be resolved")
static HeapRegion_t xHeapRegions[] =
{
       { &_heap_start, (unsigned int) &_heap_size },
       { NULL, 0 }, /* Terminates the array. */
       { NULL, 0 } /* Terminates the array. */
};

uint8_t sharedBuf[16];
void user_vAssertCalled(void) __attribute__ ((weak, alias ("vAssertCalled")));
void vAssertCalled(void)
{
    MSG("vAssertCalled\r\n");
    while( 1 );
}

void vApplicationTickHook(void)
{
    //MSG("vApplicationTickHook\r\n");
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName )
{
    MSG("vApplicationStackOverflowHook\r\n");
    if(pcTaskName){
		MSG("Stack name %s\r\n", pcTaskName);
	}
    while( 1 );
}

void vApplicationMallocFailedHook(void)
{
    MSG("vApplicationMallocFailedHook\r\n");
    while( 1 );
}
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

int ble_start_adv(void)
{
    struct bt_le_adv_param adv_param = {
        //options:3, connectable undirected, adv one time
        .options = 3, \
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_3, \
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_3, \
    };


    char *adv_name = "BL_TEST_01"; // This name must be the same as adv_name in ble_central
    uint8_t data[1] = {(BT_LE_AD_LIMITED | BT_LE_AD_NO_BREDR)};
    uint8_t data_uuid[2] = {0x12, 0x18};//0x1812
    uint8_t data_appearance[2] = {0x80, 0x01};//0x0180
    uint8_t data_manu[4] = {0x71, 0x01, 0x04, 0x13};
    struct bt_data adv_data[] = {
            BT_DATA(BT_DATA_FLAGS, data, 1),
            BT_DATA(BT_DATA_UUID16_ALL, data_uuid, sizeof(data_uuid)),
            BT_DATA(BT_DATA_GAP_APPEARANCE, data_appearance, sizeof(data_appearance)),
            BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
            BT_DATA(BT_DATA_MANUFACTURER_DATA, data_manu, sizeof(data_manu))
        };


    return bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
}

void bt_enable_cb(int err)
{
    ble_tp_init();
    ble_start_adv();
}
extern void ble_controller_init(uint8_t task_priority);
#if defined(BFLB_BLE)
extern int hci_driver_init(void);
#endif

void ble_stack_start(void)
{

    MSG("[OS] ble_controller_init...\r\n");
    GLB_Set_EM_Sel(GLB_EM_8KB);
    ble_controller_init(configMAX_PRIORITIES - 1);

    // Initialize BLE Host stack
    MSG("[OS] hci_driver_init...\r\n");
    hci_driver_init();

   MSG("[OS] bt_enable...\r\n");
    bt_enable(bt_enable_cb);
}

void ble_init(void)
{
      extern void ble_stack_start(void);
      ble_stack_start();
}

static void ble_init_task(void *pvParameters)
{
    ble_init();
    vTaskDelete(NULL);
}


int main(void)
{
    static StackType_t ble_init_stack[1024];
    static StaticTask_t ble_init_task_h;

    bflb_platform_init(0);
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, 17);

    vPortDefineHeapRegions(xHeapRegions);

    MSG("[OS] ble_init_task.....\r\n");
    xTaskCreateStatic(ble_init_task, (char*)"ble_init", sizeof(ble_init_stack)/4, NULL, 15, ble_init_stack, &ble_init_task_h);

    vTaskStartScheduler();
    while(1)
    {
    }
}
