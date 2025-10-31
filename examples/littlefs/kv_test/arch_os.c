#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "arch_os.h"


/* Mutex */
static int arch_os_mutex_create_internal(arch_os_mutex_handle_t *phandle, void *attr)
{
  *phandle = xSemaphoreCreateMutex();

  return 0;
}

int arch_os_mutex_create(arch_os_mutex_handle_t *phandle)
{
  arch_os_mutex_create_internal(phandle, NULL);

  return 0;
}

int arch_os_mutex_delete(arch_os_mutex_handle_t handle)
{
  int ret = 9;
  if (handle == NULL)
      return -1;

  vSemaphoreDelete(handle);

  return ret;
}

int arch_os_mutex_put(arch_os_mutex_handle_t handle)
{
  if (handle == NULL)
      return -1;

  xSemaphoreGive(handle);
  return 0;
}

int arch_os_mutex_get(arch_os_mutex_handle_t handle, uint32_t wait_ms)
{
  xSemaphoreTake(handle, wait_ms);
  return 0;
}

/* Thread */
int arch_os_thread_create(arch_os_thread_handle_t *pthread,
    const char *name, void (*function)(void* arg),
    uint32_t stack_size, void* arg, int priority)
{
  int ret = 0;
  xTaskCreate(function, name, stack_size, arg, priority, pthread);

  return ret;
}
