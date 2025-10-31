#ifndef RTOS_H_
#define RTOS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdbool.h>
#include <stdio.h>

//#include "FreeRTOS.h"
//#include "semphr.h"
//#include "bflb_mtimer.h"

#define INTE_EN (1UL<<3)
#define GLOBAL_INT_DISABLE_LONG_TIME()                                                 \
    do {                                                                               \
        int irq_rest;                                                                  \
        uint32_t btime, etime;                                                         \
        __asm__ volatile ("csrrci %0, mstatus, %1" : "=r" (irq_rest) : "i" (INTE_EN)); \
        btime = 0; /* bflb_mtimer_get_time_ms(); */                                    \
                                                                                       \
        /* Increment the critical section nesting level */                             \
        /* CRITICAL_NESTING_INC(); */

#define GLOBAL_INT_RESTORE_LONG_TIME()                                                \
        /* Decrement the critical section nesting level */                            \
        /* CRITICAL_NESTING_DEC(); */                                                 \
        etime = 0; /*bflb_mtimer_get_time_ms();*/                                     \
        /* Restore mstatus to its previous value */                                   \
        __asm__ volatile ("csrw mstatus, %0" : /* no output */ : "r" (irq_rest));     \
        if (etime - btime > 5) {                                                      \
          printf("disable interrupt time %d ms too long. %s:%d\r\n",                  \
                  etime - btime, __func__, __LINE__);                                 \
        }                                                                             \
    } while (0);

#if MACSW_WFA
extern volatile int GLOBAL_INT_DISABLE_has_lock;
extern volatile uint32_t critical_nesting_level;
//extern volatile void *GLOBAL_INT_task_handle;
extern int32_t TrapNetCounter; /* Someone actually called 'GLOBAL_INT_DISABLE' in the interrupt, check TrapNetCounter only avoid this */
#define GLOBAL_INT_DISABLE()                                                        \
    do {                                                                            \
    int irq_rest = 0;                                                               \
    if(!TrapNetCounter) {                                                           \
        /* only debug if(!GLOBAL_INT_task_handle) GLOBAL_INT_task_handle = xTaskGetCurrentTaskHandle(); */ \
        /* only debug if(pxCurrentTCB != GLOBAL_INT_task_handle) while(1); */       \
        irq_rest = *(volatile uint32_t *) 0xe0800008;                               \
        if(irq_rest == 0) {                                                         \
            *(volatile uint32_t *) 0xe0800008 = 0x1f000000;                         \
            asm("nop\n nop\n nop\n fence":::"memory"); /* nop_function(); */        \
            GLOBAL_INT_DISABLE_has_lock = 1;                                        \
        }                                                                           \
        critical_nesting_level++;                                                   \
     }
#else
#define GLOBAL_INT_DISABLE() GLOBAL_INT_DISABLE_LONG_TIME()
#endif

#if MACSW_WFA
#define GLOBAL_INT_RESTORE()                                                        \
        if(!TrapNetCounter) {                                                       \
        critical_nesting_level--;                                                   \
        if (0 == critical_nesting_level) {                                          \
            if (irq_rest == 0) {                                                    \
              GLOBAL_INT_DISABLE_has_lock = 0;                                      \
              *(volatile uint32_t *) 0xe0800008 = irq_rest; }                       \
            }                                                                       \
        }                                                                           \
    } while(0);
#else
#define GLOBAL_INT_RESTORE() GLOBAL_INT_RESTORE_LONG_TIME()
#endif

/**
 ****************************************************************************************
 * @brief Create the RTOS WiFi task.
 *
 * Create the wifi task that will execute the wifi UMAC and LMAC layer.
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
void wifi_task_create(void);

/**
 ****************************************************************************************
 * @brief Request the RTOS to suspend the WiFi task.
 * This function will return only when the WiFi task is resumed by the function @ref
 * rtos_wifi_task_resume
 ****************************************************************************************
 */
void wifi_task_suspend(void);

/**
 ****************************************************************************************
 * @brief Request the RTOS to resume the WiFi task.
 * This function first checks if the task was indeed suspended and then proceed to the
 * resume. Note that currently this function is supposed to be called from interrupt.
 *
 * @param[in] isr Indicates if called from ISR
 ****************************************************************************************
 */
void wifi_task_resume(bool isr);

/**
 ****************************************************************************************
 * @brief Get the system running time in milliseconds.
 *
 * @param[in] isr Indicates if called from ISR
 ****************************************************************************************
 */
uint32_t wifi_sys_now_ms(bool isr);

#endif // RTOS_H_
