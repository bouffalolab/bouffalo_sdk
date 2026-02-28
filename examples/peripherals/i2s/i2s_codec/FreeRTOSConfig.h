#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#if defined(BL602) || defined(BL702) || defined(BL702L)
#define configMTIME_BASE_ADDRESS    (0x02000000UL + 0xBFF8UL)
#define configMTIMECMP_BASE_ADDRESS (0x02000000UL + 0x4000UL)
#else
#if __riscv_xlen == 64
#define configMTIME_BASE_ADDRESS    (0)
#define configMTIMECMP_BASE_ADDRESS ((0xE4000000UL) + 0x4000UL)
#else
#define configMTIME_BASE_ADDRESS    ((0xE0000000UL) + 0xBFF8UL)
#define configMTIMECMP_BASE_ADDRESS ((0xE0000000UL) + 0x4000UL)
#endif
#endif

#define configSUPPORT_STATIC_ALLOCATION             1
#define configUSE_PREEMPTION                        1
#define configUSE_IDLE_HOOK                         0
#define configUSE_TICK_HOOK                         0
#define configCPU_CLOCK_HZ                          ((uint32_t)(1 * 1000 * 1000))
#define configTICK_RATE_HZ                          ((TickType_t)1000)
#define configMAX_PRIORITIES                        (7)
#define configMINIMAL_STACK_SIZE                    ((unsigned short)128)
#define configTOTAL_HEAP_SIZE                       ((size_t)24 * 1024)
#define configMAX_TASK_NAME_LEN                     (16)
#define configUSE_TRACE_FACILITY                    1
#define configUSE_STATS_FORMATTING_FUNCTIONS        1
#define configUSE_16_BIT_TICKS                      0
#define configIDLE_SHOULD_YIELD                     0
#define configUSE_MUTEXES                           1
#define configQUEUE_REGISTRY_SIZE                   8
#define configCHECK_FOR_STACK_OVERFLOW              2
#define configUSE_RECURSIVE_MUTEXES                 1
#define configUSE_MALLOC_FAILED_HOOK                1
#define configUSE_APPLICATION_TASK_TAG              1
#define configUSE_COUNTING_SEMAPHORES               1
#define configGENERATE_RUN_TIME_STATS               1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION     1
#define configUSE_TICKLESS_IDLE                     0
#define configUSE_POSIX_ERRNO                       1
#define configRUN_TIME_COUNTER_TYPE                 uint64_t

#define configUSE_CO_ROUTINES                       0
#define configMAX_CO_ROUTINE_PRIORITIES             (2)

#define configUSE_TIMERS                            1
#define configTIMER_TASK_PRIORITY                   (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                    4
#define configTIMER_TASK_STACK_DEPTH                (configMINIMAL_STACK_SIZE)
#define configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS 0
#ifndef uartPRIMARY_PRIORITY
#define uartPRIMARY_PRIORITY (configMAX_PRIORITIES - 3)
#endif

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

void vApplicationMallocFailedHook(void);
void vAssertCalled(void);

#include <stdio.h>
#include <stdint.h>

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
#endif

extern uint64_t bflb_mtimer_get_time_us(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() ((void)0)
#define portGET_RUN_TIME_COUNTER_VALUE()         bflb_mtimer_get_time_us()

#endif /* FREERTOS_CONFIG_H */
