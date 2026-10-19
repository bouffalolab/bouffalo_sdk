#include <FreeRTOS.h>

void async_rtos_lock(void) {
  vTaskEnterCritical();
}

void async_rtos_unlock(void) {
  vTaskExitCritical();
}
