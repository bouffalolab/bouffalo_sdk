/*
 * Amazon FreeRTOS POSIX V1.1.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file FreeRTOS_POSIX_pthread_cond.c
 * @brief Implementation of condition variable functions in pthread.h
 */

/* C standard library includes. */
#include <limits.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/utils.h"

#include "atomic_rtos.h"

/**
 * @brief Initialize a statically initialized cond.
 *
 * PTHREAD_COND_INITIALIZER sets a flag for a cond to be initialized later.
 * This function performs the initialization.
 *
 * libstdc++'s std::condition_variable constructor does not use
 * pthread_cond_init(): it only stores the toolchain's PTHREAD_COND_INITIALIZER
 * (all ones) in the 32-bit condition handle and leaves the remaining storage
 * untouched.  Treat every state other than pdTRUE as uninitialized and
 * normalize the waiter-list length so that a std::condition_variable can be
 * used without an explicit pthread_cond_init() call.
 *
 * @param[in] pxCond The cond to initialize.
 *
 * @retval pdTRUE - The cond is initialized.
 * @retval pdFALSE - The cond could not be initialized.
 */
static bool prvInitializeStaticCond(pthread_cond_internal_t *pxCond);

/*-----------------------------------------------------------*/

static bool prvInitializeStaticCond(pthread_cond_internal_t *pxCond)
{
    int i = 0;

    /* Check if the condition variable needs to be initialized.  A condition
     * variable initialized by PTHREAD_COND_INITIALIZER is flagged with
     * pdFALSE, while libstdc++ stores all ones in the handle. */
    if (pxCond->xIsInitialized != pdTRUE) {
        /* Cond initialization must be in a critical section to prevent two threads
         * from initializing it at the same time. */
        vTaskSuspendAll();

        /* Check again that the cond is still uninitialized, i.e. it wasn't
         * initialized while this function was waiting to enter the critical
         * section. */
        if (pxCond->xIsInitialized != pdTRUE) {
            /* Always use the compile-time waiter-list size: PTHREAD_COND_INITIALIZER
             * sets it to this value, while libstdc++ leaves it uninitialized. */
            pxCond->xTasksWaiting = pvPortMalloc(sizeof(TaskHandle_t) * posixconfigPTHREAD_COND_MAX_WAITERS);
            if (pxCond->xTasksWaiting != NULL) {
                /* Initialize Task List to NULL */
                for (i = 0; i < posixconfigPTHREAD_COND_MAX_WAITERS; i++) {
                    pxCond->xTasksWaiting[i] = NULL;
                }

                pxCond->tasksLenth = posixconfigPTHREAD_COND_MAX_WAITERS;
                pxCond->xIsInitialized = pdTRUE;
            }
        }

        /* Exit the critical section. */
        xTaskResumeAll();
    }

    return pxCond->xIsInitialized == pdTRUE;
}

/*-----------------------------------------------------------*/

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    int i = 0;
    pthread_cond_internal_t *pxCond = (pthread_cond_internal_t *)(cond);

    /* If the cond is uninitialized, perform initialization. */
    if (prvInitializeStaticCond(pxCond) == pdFALSE) {
        return ENOMEM;
    }

    /* Enter critical section to protect task list access and
     * prevent this task from being switched out while notifying
     * all blocked tasks */
    taskENTER_CRITICAL();

    for (i = 0; i < pxCond->tasksLenth; i++) {
        if (pxCond->xTasksWaiting[i] != NULL) {
            xTaskNotify(pxCond->xTasksWaiting[i], 0, eNoAction);
            pxCond->xTasksWaiting[i] = NULL;
        }
    }

    /* Exit the critical section. */
    taskEXIT_CRITICAL();

    return 0;
}

/*-----------------------------------------------------------*/

int pthread_cond_destroy(pthread_cond_t *cond)
{
    pthread_cond_internal_t *pxCond = (pthread_cond_internal_t *)(cond);

    /* Only an initialized cond owns the waiter list.  A std::condition_variable
     * that was never waited on or notified has no valid internal storage, so
     * leave it untouched instead of freeing an uninitialized pointer. */
    if (pxCond->xIsInitialized == pdTRUE) {
        /* Free all resources in use by the cond. */
        vPortFree(pxCond->xTasksWaiting);
        pxCond->xTasksWaiting = NULL;
        pxCond->tasksLenth = posixconfigPTHREAD_COND_MAX_WAITERS;
        pxCond->xIsInitialized = pdFALSE;
    }

    return 0;
}

/*-----------------------------------------------------------*/

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    int i = 0;
    int iStatus = 0;
    pthread_cond_internal_t *pxCond = (pthread_cond_internal_t *)cond;

    /* Silence warnings about unused parameters. */
    (void)attr;

    /* Configure default settings */
    pxCond->tasksLenth = posixconfigPTHREAD_COND_MAX_WAITERS;

    if (pxCond == NULL) {
        iStatus = ENOMEM;
    }

    if (iStatus == 0) {
        pxCond->xTasksWaiting = pvPortMalloc(sizeof(TaskHandle_t) * pxCond->tasksLenth);
        if (pxCond->xTasksWaiting != NULL) {
            /* Initialize Task List to NULL */
            for (i = 0; i < pxCond->tasksLenth; i++) {
                pxCond->xTasksWaiting[i] = NULL;
            }
            pxCond->xIsInitialized = pdTRUE;
        } else {
            iStatus = ENOMEM;
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int pthread_cond_signal(pthread_cond_t *cond)
{
    int i = 0;
    TaskHandle_t *xTaskToNotify = NULL;
    pthread_cond_internal_t *pxCond = (pthread_cond_internal_t *)(cond);

    /* If the cond is uninitialized, perform initialization. */
    if (prvInitializeStaticCond(pxCond) == pdFALSE) {
        return ENOMEM;
    }

    /* Enter critical section to protect task list access and
     * prevent this task from being switched out while notifying
     * the blocked task */
    taskENTER_CRITICAL();

    for (i = 0; i < pxCond->tasksLenth; i++) {
        if (pxCond->xTasksWaiting[i] != NULL) {
            if (xTaskToNotify == NULL) {
                xTaskToNotify = &pxCond->xTasksWaiting[i];
                continue;
            }

            /* POSIX Specification states that the scheduling policy shall determine the order
             * in which threads are unblocked. Since signal only unblocks one task, we need to make
             * sure that the thread that is unblocked is of the highest priority that is waiting.
             * Note the specification does not mention the order a thread is unblocked of the same
             * priority.
             *
             * An alternative would be to unblock all threads of the same priority as "spurious"
             * wakeups are allowed, thus allowing the threads to block on the mutex and condition
             * check. */
            if (uxTaskPriorityGet(*xTaskToNotify) < uxTaskPriorityGet(pxCond->xTasksWaiting[i])) {
                xTaskToNotify = &pxCond->xTasksWaiting[i];
            }
        }
    }

    if (xTaskToNotify != NULL) {
        xTaskNotify(*xTaskToNotify, 0, eNoAction);
        *xTaskToNotify = NULL;
    }

    /* Exit the critical section. */
    taskEXIT_CRITICAL();

    return 0;
}

/*-----------------------------------------------------------*/

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
    int i = 0;
    int iStatus = 0;
    bool iSet = pdFALSE;
    pthread_cond_internal_t *pxCond = (pthread_cond_internal_t *)(cond);
    TickType_t xDelay = portMAX_DELAY;

    /* If the cond is uninitialized, perform initialization. */
    if (prvInitializeStaticCond(pxCond) == pdFALSE) {
        return ENOMEM;
    }

    /* Convert abstime to a delay in TickType_t if provided. */
    if (abstime != NULL) {
        struct timespec xCurrentTime = { 0 };

        /* Get current time */
        if (clock_gettime(CLOCK_REALTIME, &xCurrentTime) != 0) {
            iStatus = EINVAL;
        } else {
            iStatus = UTILS_AbsoluteTimespecToDeltaTicks(abstime, &xCurrentTime, &xDelay);
        }
    }

    if (iStatus == 0) {
        /* Enter critical section to protect task list access and
         * prevent this task from being switched out while adding
         * itself to the notify list */
        taskENTER_CRITICAL();

        for (i = 0; i < pxCond->tasksLenth; i++) {
            if (pxCond->xTasksWaiting[i] == NULL) {
                pxCond->xTasksWaiting[i] = xTaskGetCurrentTaskHandle();
                iSet = pdTRUE;
                break;
            }
        }

        /* Exit the critical section. */
        taskEXIT_CRITICAL();

        /* Verify the task was added to the list */
        if (iSet == pdFALSE) {
            /* Note: ENOMEM is not part of the return value list for
             * pthread_cond_timedwait, but the specification only
             * states that EINTR can NOT be returned */
            iStatus = ENOMEM;
        }

        if (iStatus == 0) {
            iStatus = pthread_mutex_unlock(mutex);
        }

        /* Wait on the condition variable. */
        if (iStatus == 0) {
            if (xTaskNotifyWait(0, 0, NULL, xDelay) == pdPASS) {
                /* When successful, relock mutex. */
                iStatus = pthread_mutex_lock(mutex);
            } else {
                /* Timeout. Relock mutex and decrement number of waiting threads. */
                iStatus = ETIMEDOUT;
                (void)pthread_mutex_lock(mutex);
            }

            /* Enter critical section to protect task list access and
             * remove the task from the waiting list */
            taskENTER_CRITICAL();

            for (i = 0; i < pxCond->tasksLenth; i++) {
                if (pxCond->xTasksWaiting[i] == xTaskGetCurrentTaskHandle()) {
                    pxCond->xTasksWaiting[i] = NULL;
                    break;
                }
            }

            /* Exit the critical section. */
            taskEXIT_CRITICAL();
        }
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    return pthread_cond_timedwait(cond, mutex, NULL);
}

/*-----------------------------------------------------------*/
