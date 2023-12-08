/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <aos/kernel.h>
#include <msp/kernel.h>

#define ms2tick                     pdMS_TO_TICKS
#define bzero(stack, stack_size)    memset(stack, 0, stack_size)

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
int msp_task_new(const char *name, void (*fn)(void *), void *arg, int stack_size)
{
    return aos_task_new(name, fn, arg, stack_size);
}

int msp_task_new_ext(msp_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    return aos_task_new_ext(task, name, fn, arg, stack_size, prio);
}

void msp_task_delete(msp_task_t *task)
{
    aos_task_delete(task);
}

void msp_task_exit(int code)
{
    aos_task_exit(code);
}

void msp_task_wdt_attach(void (*will)(void *), void *args)
{
    aos_task_wdt_attach(will, args);
}

void msp_task_wdt_detach()
{
    aos_task_wdt_detach();
}

void msp_task_wdt_feed(int time)
{
    aos_task_wdt_feed(time);
}

int msp_mutex_new(msp_mutex_t *mutex)
{
    return aos_mutex_new(mutex);
}

void msp_mutex_free(msp_mutex_t *mutex)
{
    aos_mutex_free(mutex);
}

int msp_mutex_lock(msp_mutex_t *mutex, unsigned int ms)
{
    return aos_mutex_lock(mutex, ms);
}

int msp_mutex_unlock(msp_mutex_t *mutex)
{
    return aos_mutex_unlock(mutex);
}

int msp_mutex_is_valid(msp_mutex_t *mutex)
{
    return aos_mutex_is_valid(mutex);
}

int msp_sem_new(msp_sem_t *sem, int count)
{
    return aos_sem_new(sem, count);
}

void msp_sem_free(msp_sem_t *sem)
{
    aos_sem_free(sem);
}

int msp_sem_wait(msp_sem_t *sem, unsigned int ms)
{
    return aos_sem_wait(sem, ms);
}

void msp_sem_signal(msp_sem_t *sem)
{
    aos_sem_signal(sem);
}

int msp_sem_is_valid(msp_sem_t *sem)
{
    return aos_sem_is_valid(sem);
}

int msp_queue_new(msp_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    return aos_queue_new(queue, buf, size, max_msg);
}

msp_status_t msp_queue_create(msp_queue_t *queue, size_t size, size_t max_msg, uint32_t options)
{
    return (msp_status_t )aos_queue_create(queue, size, max_msg, options);
}

void msp_queue_free(msp_queue_t *queue)
{
    aos_queue_free(queue);
}

int msp_queue_send(msp_queue_t *queue, void *msg, unsigned int size)
{
    return aos_queue_send(queue, msg, size);
}

int msp_queue_recv(msp_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{
    return aos_queue_recv(queue, ms, msg, size);
}

int msp_queue_is_valid(msp_queue_t *queue)
{
    return aos_queue_is_valid(queue);
}

int msp_queue_get_count(msp_queue_t *queue)
{
    return aos_queue_get_count(queue);
}

void *msp_queue_buf_ptr(msp_queue_t *queue)
{
    return aos_queue_buf_ptr(queue);
}

int msp_timer_new(msp_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    return msp_timer_new_ext(timer,fn,arg,ms,repeat,1);
}

int msp_timer_new_ext(msp_timer_t *timer, void (*fn)(void *, void *),
                      void *arg, int ms, int repeat, unsigned char auto_run)
{
    return aos_timer_new_ext(timer, fn, arg, ms, repeat, auto_run);
}

void msp_timer_free(msp_timer_t *timer)
{
    aos_timer_free(timer);
}

int msp_timer_start(msp_timer_t *timer)
{
    return aos_timer_start(timer);
}

int msp_timer_stop(msp_timer_t *timer)
{
    return aos_timer_stop(timer);
}

int msp_timer_change(msp_timer_t *timer, int ms)
{
    return aos_timer_change(timer, ms);
}

int msp_timer_is_valid(msp_timer_t *timer)
{
    return aos_timer_is_valid(timer);
}

int msp_timer_change_once(msp_timer_t *timer, int ms)
{
    return aos_timer_change_once(timer, ms);
}

// Suspend the scheduler.
// return time in ticks, for how long the system can sleep or power-down.
void msp_kernel_sched_suspend(void)
{
    aos_kernel_sched_suspend();
}

// Resume the scheduler.
// param[in]     sleep_ticks   time in ticks for how long the system was in sleep or power-down mode.
void msp_kernel_sched_resume()
{
    aos_kernel_sched_resume();
}

void *msp_zalloc(unsigned int size)
{
    return aos_zalloc(size);
}

void *msp_malloc(unsigned int size)
{
    return aos_malloc(size);
}

void msp_free(void *mem)
{
    aos_free(mem);
}

void msp_freep(char **ptr)
{
    aos_freep(ptr);
}

void msp_msleep(int ms)
{
    aos_msleep(ms);
}

long long msp_now_ms(void)
{
    return aos_now_ms();
}

int msp_event_new(msp_event_t *event, unsigned int flags)
{
    return aos_event_new(event, flags);
}

void msp_event_free(msp_event_t *event)
{
    aos_event_free(event);
}

int msp_event_get
(
    msp_event_t *event,
    unsigned int flags,
    unsigned char opt,
    unsigned int *actl_flags,
    unsigned int timeout
)
{
    return aos_event_get(event, flags, opt, actl_flags, timeout);
}

int msp_event_set(msp_event_t *event, unsigned int flags, unsigned char opt)
{
    return aos_event_set(event, flags, opt);
}

int msp_event_is_valid(msp_event_t *event)
{
    return aos_event_is_valid(event);
}

void *msp_zalloc_check(size_t size)
{
    return aos_zalloc_check(size);
}

void *msp_malloc_check(size_t size)
{
    return aos_malloc_check(size);
}

void *msp_calloc_check(size_t size, size_t num)
{
    return aos_calloc_check(size, num);
}


void *msp_realloc(void *mem, size_t size)
{
    return aos_realloc(mem, size);
}

void *msp_realloc_check(void *ptr, size_t size)
{
    return aos_realloc_check(ptr, size);
}

const char *msp_task_get_name(msp_task_t *task)
{
    return aos_task_get_name(task);
}

msp_task_t msp_task_self(void)
{
    static msp_task_t task;
    task = aos_task_self();
    return task;
}

int msp_irq_context(void)
{
    return aos_irq_context();
}

int msp_is_sched_disable(void)
{
    return aos_is_sched_disable();
}

int msp_is_irq_disable(void)
{
    return aos_is_irq_disable();
}
