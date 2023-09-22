/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef MSP_KERNEL_H
#define MSP_KERNEL_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MSP_WAIT_FOREVER 0xffffffffu
#define MSP_NO_WAIT 0x0
#ifndef MSP_DEFAULT_APP_PRI
#define MSP_DEFAULT_APP_PRI 15
#endif

#define MSP_EVENT_AND              0x02u
#define MSP_EVENT_AND_CLEAR        0x03u
#define MSP_EVENT_OR               0x00u
#define MSP_EVENT_OR_CLEAR         0x01u

#define MSP_FLAGS_AND_MASK         0x2u
#define MSP_FLAGS_CLEAR_MASK       0x1u

typedef void * msp_task_t;
typedef void * msp_mutex_t;
typedef void * msp_sem_t;
typedef void * msp_queue_t;
typedef void * msp_timer_t;
typedef void * msp_event_t;

typedef int32_t msp_status_t; /**< MSP返回值状态类型 */

msp_task_t msp_task_self();

const char *msp_task_get_name(msp_task_t *task);

    /**
     * Create a task.
     *
     * @param[in]  name       task name.
     * @param[in]  fn         function to run.
     * @param[in]  arg        argument of the function.
     * @param[in]  stacksize  stack-size in bytes.
     *
     * @return  0: success.
     */
    int msp_task_new(const char *name, void (*fn)(void *), void *arg, int stack_size);

    /**
     * Create a task.
     *
     * @param[in]  task        handle.
     * @param[in]  name        task name.
     * @param[in]  fn          task function.
     * @param[in]  arg         argument of the function..
     * @param[in]  stack_buf   stack-buf: if stack_buf==NULL, provided by
     * kernel.
     * @param[in]  stack_size  stack-size in bytes.
     * @param[in]  prio        priority value, the max is
     * RHINO_CONFIG_USER_PRI_MAX(default 60).
     *
     * @return  0: success.
     */
    int msp_task_new_ext(msp_task_t *task, const char *name, void (*fn)(void *),
                         void *arg, int stack_size, int prio);

    void msp_task_delete(msp_task_t *task);

    /**
     * Exit a task.
     *
     * @param[in]  code  not used now.
     */
    void msp_task_exit(int code);

    /**
     * Alloc a mutex.
     *
     * @param[in]  mutex  pointer of mutex object, mutex object must be alloced,
     *                    hdl pointer in msp_mutex_t will refer a kernel obj
     * internally.
     *
     * @return  0: success.
     */
    int msp_mutex_new(msp_mutex_t *mutex);

    /**
     * Free a mutex.
     *
     * @param[in]  mutex  mutex object, mem refered by hdl pointer in
     * msp_mutex_t will be freed internally.
     */
    void msp_mutex_free(msp_mutex_t *mutex);

    /**
     * Lock a mutex.
     *
     * @param[in]  mutex    mutex object, it contains kernel obj pointer which
     * msp_mutex_new alloced.
     * @param[in]  timeout  waiting until timeout in milliseconds.
     *
     * @return  0: success.
     */
    int msp_mutex_lock(msp_mutex_t *mutex, unsigned int timeout);

    /**
     * Unlock a mutex.
     *
     * @param[in]  mutex  mutex object, it contains kernel obj pointer which
     * oc_mutex_new alloced.
     *
     * @return  0: success.
     */
    int msp_mutex_unlock(msp_mutex_t *mutex);

    /**
     * This function will check if mutex is valid.
     *
     * @param[in]  mutex  pointer to the mutex.
     *
     * @return  0: invalid, 1: valid.
     */
    int msp_mutex_is_valid(msp_mutex_t *mutex);

    /**
     * Alloc a semaphore.
     *
     * @param[out]  sem    pointer of semaphore object, semaphore object must be
     * alloced, hdl pointer in msp_sem_t will refer a kernel obj internally.
     * @param[in]   count  initial semaphore counter.
     *
     * @return  0:success.
     */
    int msp_sem_new(msp_sem_t *sem, int count);

    /**
     * Destroy a semaphore.
     *
     * @param[in]  sem  pointer of semaphore object, mem refered by hdl pointer
     *                  in msp_sem_t will be freed internally.
     */
    void msp_sem_free(msp_sem_t *sem);

    /**
     * Acquire a semaphore.
     *
     * @param[in]  sem      semaphore object, it contains kernel obj pointer
     * which msp_sem_new alloced.
     * @param[in]  timeout  waiting until timeout in milliseconds.
     *
     * @return  0: success.
     */
    int msp_sem_wait(msp_sem_t *sem, unsigned int timeout);

    /**
     * Release a semaphore.
     *
     * @param[in]  sem  semaphore object, it contains kernel obj pointer which
     * msp_sem_new alloced.
     */
    void msp_sem_signal(msp_sem_t *sem);

    /**
     * This function will check if semaphore is valid.
     *
     * @param[in]  sem  pointer to the semaphore.
     *
     * @return  0: invalid, 1: valid.
     */
    int msp_sem_is_valid(msp_sem_t *sem);

    /**
     * This function will create a queue.
     *
     * @param[in]  queue    pointer to the queue(the space is provided by user).
     * @param[in]  size     the bytes of the buf.
     * @param[in]  max_msg  the max size of the msg.
     * @param[in]  options    reserved.
     * @return  0: success.
     */
    msp_status_t msp_queue_create(msp_queue_t *queue, size_t size, size_t max_msg, uint32_t options);

    /**
     * This function will create a queue.
     *
     * @param[in]  queue    pointer to the queue(the space is provided by user).
     * @param[in]  buf      buf of the queue(provided by user).
     * @param[in]  size     the bytes of the buf.
     * @param[in]  max_msg  the max size of the msg.
     *
     * @return  0: success.
     */

    int msp_queue_new(msp_queue_t *queue, void *buf, unsigned int size,
                      int max_msg);

    /**
     * This function will delete a queue.
     *
     * @param[in]  queue  pointer to the queue.
     */
    void msp_queue_free(msp_queue_t *queue);

    /**
     * This function will send a msg to the front of a queue.
     *
     * @param[in]  queue  pointer to the queue.
     * @param[in]  msg    msg to send.
     * @param[in]  size   size of the msg.
     *
     * @return  0: success.
     */
    int msp_queue_send(msp_queue_t *queue, void *msg, unsigned int size);

    /**
     * This function will receive msg from a queue.
     *
     * @param[in]   queue  pointer to the queue.
     * @param[in]   ms     ms to wait before receive.
     * @param[out]  msg    buf to save msg.
     * @param[out]  size   size of the msg.
     *
     * @return  0: success.
     */
    int msp_queue_recv(msp_queue_t *queue, unsigned int ms, void *msg,
                       unsigned int *size);

    /**
     * This function will check if queue is valid.
     *
     * @param[in]  queue  pointer to the queue.
     *
     * @return  0: invalid, 1: valid.
     */
    int msp_queue_is_valid(msp_queue_t *queue);

    /**
     * Get number of queued messages in a message queue.
     *
     * @param[in]  queue  message queue handle to operate.
     *
     * @return  number of queued messages.negative indicates error code.
     */
    int msp_queue_get_count(msp_queue_t *queue);

    /**
     * This function will return buf ptr if queue is inited.
     *
     * @param[in]  queue  pointer to the queue.
     *
     * @return  NULL: error.
     */
    void *msp_queue_buf_ptr(msp_queue_t *queue);

    /**
     * This function will create a timer and run auto.
     *
     * @param[in]  timer   pointer to the timer.
     * @param[in]  fn      callbak of the timer.
     * @param[in]  arg     the argument of the callback.
     * @param[in]  ms      ms of the normal timer triger.
     * @param[in]  repeat  repeat or not when the timer is created.
     *
     * @return  0: success.
     */
    int msp_timer_new(msp_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      int ms, int repeat);

    /**
     * This function will create a timer.
     *
     * @param[in]  timer   pointer to the timer.
     * @param[in]  fn      callbak of the timer.
     * @param[in]  arg     the argument of the callback.
     * @param[in]  ms      ms of the normal timer triger.
     * @param[in]  repeat  repeat or not when the timer is created.
     * @param[in]  auto_run  run auto or not when the timer is created.
     *
     * @return  0: success.
     */
    int msp_timer_new_ext(msp_timer_t *timer, void (*fn)(void *, void *),
                          void *arg, int ms, int repeat,
                          unsigned char auto_run);

    /**
     * This function will delete a timer.
     *
     * @param[in]  timer  pointer to a timer.
     */
    void msp_timer_free(msp_timer_t *timer);

    /**
     * This function will start a timer.
     *
     * @param[in]  timer  pointer to the timer.
     *
     * @return  0: success.
     */
    int msp_timer_start(msp_timer_t *timer);

    /**
     * This function will stop a timer.
     *
     * @param[in]  timer  pointer to the timer.
     *
     * @return  0: success.
     */
    int msp_timer_stop(msp_timer_t *timer);

    /**
     * This function will change attributes of a timer.
     *
     * @param[in]  timer  pointer to the timer.
     * @param[in]  ms     ms of the timer triger.
     *
     * @return  0: success.
     */
    int msp_timer_change(msp_timer_t *timer, int ms);

    /**
     * Realloc memory.
     *
     * @param[in]  mem   current memory address point.
     * @param[in]  size  new size of the mem to remalloc.
     *
     * @return  NULL: error.
     */
    void *msp_realloc(void *mem, size_t size);

    /**
     * Alloc memory.
     *
     * @param[in]  size  size of the mem to malloc.
     *
     * @return  NULL: error.
     */
    void *msp_malloc(unsigned int size);

    /**
     * Alloc memory and clear to zero.
     *
     * @param[in]  nitems  number of items to malloc.
     * @param[in]  size    size of one item to malloc.
     *
     * @return  NULL: error.
     */
    void *msp_calloc(unsigned int nitems, unsigned int size);

    /**
     * Alloc memory and clear to zero.
     *
     * @param[in]  size  size of the mem to malloc.
     *
     * @return  NULL: error.
     */
    void *msp_zalloc(unsigned int size);

    /**
     * Realloc memory.
     *
     * @param[in]  mem   current memory address point.
     * @param[in]  size  new size of the mem to remalloc.
     *
     * @return  NULL: error.
     */
    void *msp_realloc(void *mem, size_t size);

    void *msp_malloc_check(size_t size);
    void *msp_realloc_check(void *mem, size_t size);
    void *msp_zalloc_check(size_t size);
    void *msp_calloc_check(size_t size, size_t num);

    /**
     * Free memory.
     *
     * @param[in]  ptr  address point of the mem.
     */
    void msp_free(void *mem);

    /**
     * Free memory and set NULL.
     *
     * @param[in]  *ptr  address point of the mem.
     */
    void msp_freep(char **ptr);

    /**
     * Get current time in mini seconds.
     *
     * @return  elapsed time in mini seconds from system starting.
     */
    long long msp_now_ms(void);

    /**
     * @brief Retrieves the timer string. Under RTOS w/o RTC, this fucntion will
     * return the UTC time string that consider boot-up as 01-01 00:00:00.000.
     *    Under Linuxhost and compile option "vall=posix", this function will
     * get the local time which considering time zone.
     *
     * @param [buf] give buffer to save timer string
     * @param [len] the length of buffer, recommand 19, right a terminating
     * null-character is appended in last
     * @return the string of timer. Format is MM-dd hh:mm:ss.ms. e.g. 01-01
     * 00:00:10.000.
     *
     * @see None.
     * @note None.
     */
    char *msp_now_time_str(char *buffer, const int len);

    /**
     * Msleep.
     *
     * @param[in]  ms  sleep time in milliseconds.
     */
    void msp_msleep(int ms);

    /**
     * This function will create an event with an initialization flag set.
     * This function should not be called from interrupt context.
     *
     * @param[in]  event    event object pointer.
     * @param[in]  flags    initialization flag set(provided by caller).
     *
     * @return  0: success.
     */

    int msp_event_new(msp_event_t *event, unsigned int flags);

    /**
     * This function will free an event.
     * This function shoud not be called from interrupt context.
     *
     * @param[in]  event    memory refered by hdl pointer in event will be freed.
     *
     * @return  N/A.
     */

    void msp_event_free(msp_event_t *event);

    /**
     * This function will try to get flag set from given event, if the request flag
     * set is satisfied, it will return immediately, if the request flag set is not
     * satisfied with timeout(RHINO_WAIT_FOREVER,0xFFFFFFFF), the caller task will be
     * pended on event until the flag is satisfied, if the request flag is not
     * satisfied with timeout(RHINO_NO_WAIT, 0x0), it will also return immediately.
     * Note, this function should not be called from interrupt context because it has
     * possible to lead context switch and an interrupt has no TCB to save context.
     *
     * @param[in]  event        event object pointer.
     * @param[in]  flags        request flag set.
     * @param[in]  opt          operation type, such as AND,OR,AND_CLEAR,OR_CLEAR.
     * @param[out] actl_flags   the internal flags value hold by event.
     * @param[in]  flags        request flag set.
     * @param[in]  timeout      max wait time in millisecond.
     *
     * @return  0: success.
     */

    int msp_event_get(msp_event_t *event, unsigned int flags, unsigned char opt,
                      unsigned int *actl_flags, unsigned int timeout);

    /**
    * This function will set flag set to given event, and it will check if any task
    * which is pending on the event should be waken up.
    *
    * @param[in]  event    event object pointer.
    * @param[in]  flags    flag set to be set into event.
    * @param[in]  opt      operation type, such as AND,OR.
    *
    * @return  0: success.
    */

    int msp_event_set(msp_event_t *event, unsigned int flags, unsigned char opt);

    /**
     * This function will check if event is valid.
     *
     * @param[in]  sem  pointer to the semaphore.
     *
     * @return  0: invalid, 1: valid.
     */
    int msp_event_is_valid(msp_event_t *event);

    // Suspend the scheduler.
    void msp_kernel_sched_suspend(void);

    // Resume the scheduler.
    void msp_kernel_sched_resume(void);


    void msp_task_wdt_attach(void (*will)(void *), void *args);
    void msp_task_wdt_detach();
    void msp_task_wdt_feed(int time);
    int msp_irq_context(void);
    int msp_is_sched_disable(void);
    int msp_is_irq_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* MSP_KERNEL_H */
