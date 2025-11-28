#ifndef _RTOS_AL_
#define _RTOS_AL_

#include <stdbool.h>
#include <stdint.h>

/// RTOS semaphore
typedef void *rtos_semaphore;

/// RTOS mutex
typedef void *rtos_mutex;

uint32_t rtos_al_ms2tick(int ms);
uint32_t rtos_now(bool isr);

void rtos_lock(void);
void rtos_unlock(void);

int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count,
                          int init_count);
void rtos_semaphore_delete(rtos_semaphore semaphore);
int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout);
int rtos_semaphore_signal(rtos_semaphore semaphore, bool isr);

int rtos_mutex_create(rtos_mutex *mutex);
void rtos_mutex_delete(rtos_mutex mutex);
void rtos_mutex_lock(rtos_mutex mutex);
void rtos_mutex_unlock(rtos_mutex mutex);

void rtos_timeouts_init(void);
void rtos_timeouts_start(unsigned int delay);
#endif
