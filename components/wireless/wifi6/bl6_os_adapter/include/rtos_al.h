/**
 * @file rtos_al.h
 * @brief
 *
 * Copyright (c) 2023 Bouffalolab team
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

#ifndef RTOS_AL_H_
#define RTOS_AL_H_

/**
 ****************************************************************************************
 * @defgroup FHOST_RTOS_AL FHOST_RTOS_AL
 * @ingroup FHOST_AL
 * @brief RTOS interface
 *
 * The Fully Hosted firmware requires an implementation of the functions described here.
 *
 * A file rtos_def.h must also be provided and include definition of the following types:
 * - @b rtos_task_handle: Defined a pointer to the structure used to represent a TASK
 * - @b rtos_queue: Defined a pointer to the structure used to represent a queue
 * - @b rtos_semaphore: Defined a pointer to the structure used to represent a semaphore
 * - @b rtos_mutex: Defined a pointer to the structure used to represent a mutex
 * - @b rtos_prio: Priority of a task.
 * - @b rtos_task_fct: Function prototype for task entry point
 * Those types are never dereferenced so they don't need to be fully defined (except rtos_prio).
 *
 * The following MACROs must also be defined:
 * - @b RTOS_TASK_FCT(name): Used to declare the task entry point function 'name'
 * - @b RTOS_TASK_PRIORITY(prio): Convert FHOST priority (int) in RTOS priority (rtos_prio).
 *                                Higher value means higher priority for the FHOST module.
 * - @b RTOS_TASK_NULL: Define a NULL task handle
 *
 * @{
 ****************************************************************************************
 */
#include "rtos_def.h"
#include "rtos_trace.h"

/**
 * RTOS task identifier
 */
enum rtos_task_id {
    IDLE_TASK = 0,
    WIFI_TASK,
    CONTROL_TASK,
    RX_TASK,
    TX_TASK,
    SUPPLICANT_TASK,
    IP_TASK,
    APPLICATION_TASK,
    TG_SEND_TASK,
    PING_SEND_TASK,
    IPERF_TASK,
    SMARTCONF_TASK,
    DOORBELL_TASK,
    WPA_CONNECTED_TASK,
    MAX_TASK,
    UNDEF_TASK = 255,
};

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Get the current RTOS time, in ms.
 *
 * @param[in] isr  Indicate if this is called from ISR.
 *
 * @return The current RTOS time (in ms)
 ****************************************************************************************
 */
uint32_t rtos_now(bool isr);

/**
 ****************************************************************************************
 * @brief Allocate memory.
 *
 * @param[in] size Size, in bytes, to allocate.
 *
 * @return Address of allocated memory on success and NULL if error occurred.
 ****************************************************************************************
 */
void *rtos_malloc(uint32_t size);

/**
 ****************************************************************************************
 * @brief Allocate memory and initialize it to 0.
 *
 * @param[in] nb_elt  Number of element to allocate.
 * @param[in] size    Size, in bytes, of each element allocate.
 *
 * @return Address of allocated and initialized memory on success and NULL if error occurred.
 ****************************************************************************************
 */
void *rtos_calloc(uint32_t nb_elt, uint32_t size);

/**
 ****************************************************************************************
 * @brief Free memory.
 *
 * @param[in] ptr Memory buffer to free. MUST have been allocated with @ref rtos_malloc
 ****************************************************************************************
 */
void rtos_free(void *ptr);

/**
 ****************************************************************************************
 * @brief Get HEAP Memory information. (For debug purpose only)
 *
 * @param[out] total_size    Updated with HEAP memory size.
 * @param[out] free_size     Updated with the currently available memory.
 * @param[out] min_free_size Updated with the lowest level of free memory reached.
 ****************************************************************************************
 */
void rtos_heap_info(int *total_size, int *free_size, int *min_free_size);

/**
 ****************************************************************************************
 * @brief Create a RTOS task.
 *
 * @param[in] func Pointer to the task function
 * @param[in] name Name of the task
 * @param[in] task_id ID of the task
 * @param[in] stack_depth Required stack depth for the task
 * @param[in] params Pointer to private parameters of the task function, if any
 * @param[in] prio Priority of the task
 * @param[out] task_handle Handle of the task, that might be used in subsequent RTOS
 *                         function calls
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int rtos_task_create(rtos_task_fct func,
                     const char * const name,
                     enum rtos_task_id task_id,
                     const uint16_t stack_depth,
                     void * const params,
                     rtos_prio prio,
                     rtos_task_handle * const task_handle);
/**
 ****************************************************************************************
 * @brief Delete a RTOS task.
 *
 * @param[in] task_handle Handle of the task to delete.
 ****************************************************************************************
 */
void rtos_task_delete(rtos_task_handle task_handle);

/**
 ****************************************************************************************
 * @brief RTOS task suspends itself for a specific duration.
 *
 * @param[in] duration Duration in ms.
 ****************************************************************************************
 */
void rtos_task_suspend(int duration);

/**
 ****************************************************************************************
 * @brief Initialize notification for a FHOST task.
 *
 * If notification are natively supported by the target RTOS, then this function will
 * probably do nothing. If this is not the case this function allows the RTOS_AL
 * implementation to initialize its own notification system for the task (e.g. allocating
 * a binary semaphore for the task).
 *
 * To ensure the maximum compatibility, this function must be called before
 * @ref rtos_task_wait_notification or @ref rtos_task_notify can be used on a task.
 *
 * @param[in] task  Task handle
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
  */
int rtos_task_init_notification(rtos_task_handle task);

/**
 ****************************************************************************************
 * @brief Task suspend itself until it is notified (or timeout expires)
 *
 * The task will be resumed when another task call @ref rtos_task_notify. It another task
 * already call @ref rtos_task_notify then the function will return immediately.
 * On return it clears all pending notification.
 * @ref rtos_task_init_notification must be called first before calling this function.
 *
 * @param[in] timeout Maximum duration to wait, in ms, if no notification is pending.
 *                    0 means do not wait and -1 means wait indefinitely.
 *
 * @return The number of pending notification (0 if timeout was reached)
 ****************************************************************************************
 */
int rtos_task_wait_notification(int timeout);

/**
 ****************************************************************************************
 * @brief Send notification to a task
 *
 * If the task is suspended, after calling @ref rtos_task_wait_notification, it will
 * resume it. Otherwise the notification will be pending for the task.
 * If @ref rtos_task_init_notification has not been called for the target task before the
 * behavior is undefined.
 *
 * @param[in] task Task to notify.
 * @param[in] isr  Indicate if this is called from ISR.
 *
 ****************************************************************************************
 */
void rtos_task_notify(rtos_task_handle task, bool isr);

/**
 ****************************************************************************************
 * @brief Create a RTOS message queue.
 *
 * @param[in]  elt_size Size, in bytes, of one queue element
 * @param[in]  nb_elt   Number of element to allocate for the queue
 * @param[out] queue    Update with queue handle on success
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int rtos_queue_create(int elt_size, int nb_elt, rtos_queue *queue);

/**
 ****************************************************************************************
 * @brief Delete a queue previously created by @ref rtos_queue_create.
 * This function does not verify if the queue is empty or not before deleting it.
 *
 * @param[in]  queue   Queue handle
 ****************************************************************************************
 */
void rtos_queue_delete(rtos_queue queue);

/**
 ****************************************************************************************
 * @brief Check if a RTOS message queue is empty or not.
 * This function can be called both from an ISR and a task.
 *
 * @param[in]  queue   Queue handle
 *
 * @return true if queue is empty, false otherwise.
 ****************************************************************************************
 */
bool rtos_queue_is_empty(rtos_queue queue);

/**
 ****************************************************************************************
 * @brief Check if a RTOS message queue is full or not.
 * This function can be called both from an ISR and a task.
 *
 * @param[in]  queue   Queue handle
 *
 * @return true if queue is full, false otherwise.
 ****************************************************************************************
 */
bool rtos_queue_is_full(rtos_queue queue);

/**
 ****************************************************************************************
 * @brief Get the number of messages pending a queue.
 * This function can be called both from an ISR and a task.
 *
 * @param[in]  queue   Queue handle
 *
 * @return The number of messages pending in the queue.
 ****************************************************************************************
 */
int rtos_queue_cnt(rtos_queue queue);

/**
 ****************************************************************************************
 * @brief Write a message at the end of a RTOS message queue.
 *
 * @param[in]  queue   Queue handle
 * @param[in]  msg     Message to copy in the queue. (It is assume that buffer is of the
 *                     size specified in @ref rtos_queue_create)
 * @param[in]  timeout Maximum duration to wait, in ms, if queue is full. 0 means do not
 *                     wait and -1 means wait indefinitely.
 * @param[in]  isr     Indicate if this is called from ISR. If set, @p timeout parameter
 *                     is ignored.
 *
 * @return 0 on success and != 0 if error occurred (i.e queue was full and maximum
 * duration has been reached).
 ****************************************************************************************
 */
int rtos_queue_write(rtos_queue queue, void *msg, int timeout, bool isr);

/**
 ****************************************************************************************
 * @brief Read a message from a RTOS message queue.
 *
 * @param[in]  queue   Queue handle
 * @param[in]  msg     Buffer to copy into. (It is assume that buffer is of the
 *                     size specified in @ref rtos_queue_create)
 * @param[in]  timeout Maximum duration to wait, in ms, if queue is empty. 0 means do not
 *                     wait and -1 means wait indefinitely.
 * @param[in]  isr     Indicate if this is called from ISR. If set, @p timeout parameter
 *                     is ignored.
 *
 * @return 0 on success and != 0 if error occurred (i.e queue was empty and maximum
 * duration has been reached).
 ****************************************************************************************
 */
int rtos_queue_read(rtos_queue queue, void *msg, int timeout, bool isr);

/**
 ****************************************************************************************
 * @brief Creates and returns a new semaphore.
 *
 * @param[out] semaphore Semaphore handle returned by the function
 * @param[in]  max_count The maximum count value that can be reached by the semaphore.
 *             When the semaphore reaches this value it can no longer be 'given'.
 * @param[in]  init_count The count value assigned to the semaphore when it is created.
 *
 * @return 0 on success and != 0 otherwise.
 ****************************************************************************************
 */
int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count, int init_count);

/**
 ****************************************************************************************
 * @brief Return a semaphore count.
 *
 * @param[in]  semaphore Semaphore handle
 *
 * @return Semaphore count.
 ****************************************************************************************
 */
int rtos_semaphore_get_count(rtos_semaphore semaphore);

/**
 ****************************************************************************************
 * @brief Delete a semaphore previously created by @ref rtos_semaphore_create.
 *
 * @param[in]  semaphore Semaphore handle
 ****************************************************************************************
 */
void rtos_semaphore_delete(rtos_semaphore semaphore);

/**
 ****************************************************************************************
 * @brief Wait for a semaphore to be available.
 *
 * @param[in]  semaphore Semaphore handle
 * @param[in]  timeout   Maximum duration to wait, in ms. 0 means do not wait and -1 means
 *                       wait indefinitely.
 *
 * @return 0 on success and != 0 if timeout occurred.
 ****************************************************************************************
 */
int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout);

/**
 ****************************************************************************************
 * @brief Signal the semaphore the handle of which is passed as parameter.
 *
 * @param[in]  semaphore Semaphore handle
 * @param[in]  isr       Indicate if this is called from ISR
 *
 * @return 0 on success and != 0 otherwise.
 ****************************************************************************************
 */
int rtos_semaphore_signal(rtos_semaphore semaphore, bool isr);

/**
 ****************************************************************************************
 * @brief Creates and returns a new mutex.
 *
 * @param[out] mutex Mutex handle returned by the function
 *
 * @return 0 on success and != 0 otherwise.
 ****************************************************************************************
 */
int rtos_mutex_create(rtos_mutex *mutex);

/**
 ****************************************************************************************
 * @brief Delete a mutex previously created by @ref rtos_mutex_create.
 *
 * @param[in]  mutex Mutex handle
 ****************************************************************************************
 */
void rtos_mutex_delete(rtos_mutex mutex);

/**
 ****************************************************************************************
 * @brief Lock a mutex.
 *
 * @param[in]  mutex Mutex handle
 ****************************************************************************************
 */
void rtos_mutex_lock(rtos_mutex mutex);

/**
 ****************************************************************************************
 * @brief Unlock a mutex.
 *
 * @param[in]  mutex Mutex handle
 ****************************************************************************************
 */
void rtos_mutex_unlock(rtos_mutex mutex);

/**
 ****************************************************************************************
 * @brief Get the handle of the task that holds the mutx specified by the function parameter.
 *
 * @param[in]  mutex Mutex handle
 ****************************************************************************************
 */
rtos_task_handle rtos_mutex_get_holder(rtos_mutex mutex);

/**
 ****************************************************************************************
 * @brief Enter a critical section.
 * This function returns the previous protection level that is then used in the
 * @ref rtos_unprotect function call in order to put back the correct protection level
 * when exiting the critical section. This allows nesting the critical sections.
 *
 * @return  The previous protection level
 ****************************************************************************************
 */
uint32_t rtos_protect(void);

/**
 ****************************************************************************************
 * @brief Exit a critical section.
 * This function restores the previous protection level.
 *
 * @param[in]  protect The protection level to restore.
 ****************************************************************************************
 */
void rtos_unprotect(uint32_t protect);

/**
 ****************************************************************************************
 * @brief Launch the RTOS scheduler.
 * This function is supposed not to return as RTOS will switch the context to the highest
 * priority task inside this function.
 *
 ****************************************************************************************
 */
void rtos_start_scheduler(void);

/**
 ****************************************************************************************
 * @brief Init RTOS
 *
 * Initialize RTOS layers before start.
 *
 * @return 0 on success and != 0 if error occurred
 ****************************************************************************************
 */
int rtos_init(void);

/**
 ****************************************************************************************
 * @brief Change the priority of a task
 * This function cannot be called from an ISR.
 *
 * @param[in] handle Task handle
 * @param[in] priority New priority to set to the task
 *
 ****************************************************************************************
 */
void rtos_priority_set(rtos_task_handle handle, rtos_prio priority);

/**
 ****************************************************************************************
 * @brief Return RTOS task handle
 *
 * @return current task handle
 ****************************************************************************************
 */
rtos_task_handle rtos_get_task_handle();

#endif // RTOS_H_

/**
 * @}
 */
