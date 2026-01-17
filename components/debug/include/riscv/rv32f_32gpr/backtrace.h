#ifndef __BACKTRACE_H__
#define __BACKTRACE_H__

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print the stack backtrace of the current task
 *
 * Get the call stack at the current position and print it to the console for debugging and exception analysis.
 * Output format: "task_name: *0xaddr1 0xaddr2 ..."
 *
 * @code
 * void my_function(void) {
 *     backtrace_now();  // Print the call chain up to this point
 * }
 * @endcode
 */
void backtrace_now(void);

/**
 * @brief Print stack backtraces for all tasks
 *
 * Traverse all FreeRTOS tasks in the system and print the call stack for each task.
 * The currently running task is marked with '*', and tasks that cannot obtain a backtrace display '-'.
 *
 * Example output:
 * @code
 * ========== Backtrace ==========
 * shell_task  : *0xa002dca8 0xa002dd98
 * IDLE        : 0xa00589dc 0xa0092e18
 * wifi fw     : -
 * ===============================
 * @endcode
 *
 * @note Executed in a critical section, interrupts are briefly disabled
 */
void backtrace_tasks_all(void);

/**
 * @brief Get the stack backtrace for a specified task
 *
 * @param handle FreeRTOS task handle
 * @param bt     [out] Array to store backtrace addresses
 * @param max    Maximum capacity of the array
 * @return Actual backtrace depth obtained, 0 on failure
 *
 * @code
 * TaskHandle_t task;
 * uintptr_t bt[16];
 * int depth = backtrace_task(task, bt, 16);
 * for (int i = 0; i < depth; i++) {
 *     printf("[%d] 0x%lx\r\n", i, bt[i]);
 * }
 * @endcode
 */
int backtrace_task(TaskHandle_t handle, uintptr_t *bt, int max);

/**
 * @brief Get stack backtrace by task name
 *
 * @param name Task name string (must match the name specified in xTaskCreate)
 * @param bt   [out] Array to store backtrace addresses
 * @param max  Maximum capacity of the array
 * @return Actual backtrace depth obtained, 0 if the task does not exist
 *
 * @code
 * uintptr_t bt[16];
 * int depth = backtrace_task_by_name("shell_task", bt, 16);
 * @endcode
 */
int backtrace_task_by_name(const char *name, uintptr_t *bt, int max);

#ifdef __cplusplus
}
#endif

#endif /* __BACKTRACE_H__ */
