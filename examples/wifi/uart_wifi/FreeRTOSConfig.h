/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/
#include "stdio.h"


static inline unsigned long long getCycleCounter() {
#if __riscv_xlen == 32
  register unsigned int cycle, cycleh, cycleh_tmp;
  do {
    asm volatile ("rdcycleh %0" : "=r"(cycleh));
    asm volatile ("rdcycle %0" : "=r"(cycle));
    asm volatile ("rdcycleh %0" : "=r"(cycleh_tmp));
  } while(cycleh != cycleh_tmp);
  return ((unsigned long long)cycleh << 32) | cycle;
#elif __riscv_xlen == 64
  register unsigned long long cycle;
  asm volatile ("rdcycle %0" : "=r"(cycle));
  return cycle;
#endif
}

static inline void resetCycleCounter() {
  asm volatile ("csrw mcycle, x0");
} 

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() resetCycleCounter()
#define portGET_RUN_TIME_COUNTER_VALUE() getCycleCounter()

#define configCPU_CORE_CLOCK_HZ (320 * 1000 * 1000)
#define portHAS_STAT_TRAP_TIME 1
#define configGENERATE_RUN_TIME_STATS 1
#define INCLUDE_xTaskGetIdleTaskHandle 1
#define configRUN_TIME_COUNTER_TYPE uint64_t


#ifdef BFLB_undef
#define configMTIME_BASE_ADDRESS    (0x02000000UL + 0xBFF8UL)
#define configMTIMECMP_BASE_ADDRESS (0x02000000UL + 0x4000UL)
#else
#define configMTIME_BASE_ADDRESS    (0xE0000000UL + 0xBFF8UL)
#define configMTIMECMP_BASE_ADDRESS (0xE0000000UL + 0x4000UL)
#endif

#define configSUPPORT_STATIC_ALLOCATION         1
#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCPU_CLOCK_HZ                      ((uint32_t)(1 * 1000 * 1000))
#define configTICK_RATE_HZ                      ((TickType_t)1000)
#define configMAX_PRIORITIES                    (32)
#define configMINIMAL_STACK_SIZE                ((unsigned short)128) /* Only needs to be this high as some demo tasks also use this constant.  In production only the idle task would use this. */
#define configTOTAL_HEAP_SIZE                   ((size_t)100 * 1024)
#define configMAX_TASK_NAME_LEN                 (16)
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 0
#define configUSE_MUTEXES                       1
#define configQUEUE_REGISTRY_SIZE               8
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_MALLOC_FAILED_HOOK            1
#define configUSE_APPLICATION_TASK_TAG          1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_POSIX_ERRNO                   1

#ifndef configUSE_TICKLESS_IDLE
#define configUSE_TICKLESS_IDLE                 0
#endif

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES           0
#define configMAX_CO_ROUTINE_PRIORITIES (2)

/* Software timer definitions. */
#define configUSE_TIMERS             1
#define configTIMER_TASK_PRIORITY    (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH     4
#define configTIMER_TASK_STACK_DEPTH (1024)

/* Task priorities.  Allow these to be overridden. */
#ifndef uartPRIMARY_PRIORITY
#define uartPRIMARY_PRIORITY (configMAX_PRIORITIES - 3)
#endif

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet         1
#define INCLUDE_uxTaskPriorityGet        1
#define INCLUDE_vTaskDelete              1
#define INCLUDE_vTaskCleanUpResources    1
#define INCLUDE_vTaskSuspend             1
#define INCLUDE_vTaskDelayUntil          1
#define INCLUDE_vTaskDelay               1
#define INCLUDE_eTaskGetState            1
#define INCLUDE_xTimerPendFunctionCall   1
#define INCLUDE_xTaskAbortDelay          1
#define INCLUDE_xTaskGetHandle           1
#define INCLUDE_xSemaphoreGetMutexHolder 1

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
void vApplicationMallocFailedHook(void);
void vAssertCalled(void);
#define configASSERT(x)                        \
    if ((x) == 0) {                            \
        printf("file [%s]\r\n", __FILE__);     \
        printf("func [%s]\r\n", __FUNCTION__); \
        printf("line [%d]\r\n", __LINE__);     \
        printf("%s\r\n", (const char *)(#x));  \
        vAssertCalled();                       \
    }

#if (configUSE_TICKLESS_IDLE != 0)
void vApplicationSleep(uint32_t xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP(xExpectedIdleTime) vApplicationSleep(xExpectedIdleTime)

#ifndef configEXPECTED_IDLE_TIME_BEFORE_SLEEP
extern uint32_t expected_idle_before_sleep(void);
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP expected_idle_before_sleep()
#endif

#ifdef TICKLESS_DEBUG
#include <stdint.h>
#include "portmacro.h"
extern void tickless_debug_who_wake_me(const char *name, TickType_t ticks);
#define configPRE_SUPPRESS_TICKS_AND_SLEEP_PROCESSING( x ) do { \
  TCB_t *next_wake_tcb = (TCB_t *)((uint32_t)pxDelayedTaskList->xListEnd.pxNext - 4); \
  tickless_debug_who_wake_me(next_wake_tcb->pcTaskName, next_wake_tcb->xStateListItem.xItemValue); \
} while(0);
#endif
#endif

// #define portUSING_MPU_WRAPPERS

/* Enable TLS */
#define config_ENABLE_OS_TLS_SWITCH
#ifdef config_ENABLE_OS_TLS_SWITCH
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS     1
#define configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS 1
#endif

#endif /* FREERTOS_CONFIG_H */
