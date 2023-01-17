#ifndef __ARCH_OS_H__
#define __ARCH_OS_H__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "arch_time.h"

#ifndef SCHED_NATIVE_PRIORITY_DEFAULT
#define SCHED_NATIVE_PRIORITY_DEFAULT (4)
#endif

#define ARCH_OS_PRIORITY_DEFAULT        SCHED_NATIVE_PRIORITY_DEFAULT
#define ARCH_OS_NATIVE_PRIORITY_DEFAULT SCHED_NATIVE_PRIORITY_DEFAULT

#define ARCH_OS_WAIT_FOREVER            (0xFFFFFFFF)
#define ARCH_OS_NO_WAIT                 (0)

typedef SemaphoreHandle_t arch_os_mutex_handle_t;
typedef TaskHandle_t arch_os_thread_handle_t;

#define ARCH_OS_FUNCTION_RETURN(v) ((void *)(intptr_t)v)

arch_os_thread_handle_t arch_os_thread_current(void);
int arch_os_thread_create(arch_os_thread_handle_t *pthread, const char *name,
                          void (*function)(void *), uint32_t stack_size, void *arg, int priority);
int arch_os_thread_delete(arch_os_thread_handle_t thread);
void arch_os_thread_finish(void);

void arch_os_enter_critical(void);
void arch_os_exit_critical(void);

int arch_os_mutex_create(arch_os_mutex_handle_t *phandle);
int arch_os_mutex_delete(arch_os_mutex_handle_t handle);
int arch_os_mutex_put(arch_os_mutex_handle_t handle);
int arch_os_mutex_get(arch_os_mutex_handle_t handle, uint32_t wait_ms);

#endif /* __ARCH_OS_H__ */
