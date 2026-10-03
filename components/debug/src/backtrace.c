/**
 * @file backtrace.c
 * @brief DWARF CFI-based backtrace wrapper for FreeRTOS
 *
 * This module provides FreeRTOS integration and multi_bins descriptor parsing
 * on top of the low-level unwind_6byte implementation.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "backtrace.h"
#include "unwind_6byte.h"
#include "FreeRTOS.h"
#include "task.h"
#include "multi_bins.h"

/*============================================================================
 * Internal Functions
 *============================================================================*/

/**
 * Get CFI table base address from multi_bins descriptor
 * Returns NULL if not found
 */
static const uint8_t* get_cfi_table_base(void)
{
    return multi_bins_get_start("DWARFCFI");
}

/*============================================================================
 * Public API - Backtrace (wrapping unwind_6byte.c)
 *============================================================================*/

int backtrace_unwind(uint32_t pc, uint32_t sp,
                      uint32_t *addrs, int max)
{
    /* Get CFI table base address from multi_bins descriptor */
    const uint8_t *cfi_table_base = get_cfi_table_base();
    if (cfi_table_base == NULL) {
        return 0;  /* CFI table not found */
    }

    /* Call low-level backtrace from unwind_6byte.c */
    return backtrace(pc, sp, addrs, max, cfi_table_base);
}

static int backtrace_unwind_with_ra(uint32_t pc, uint32_t sp, uint32_t ra,
                                    uint32_t *addrs, int max)
{
    const uint8_t *cfi_table_base = get_cfi_table_base();
    if (cfi_table_base == NULL) {
        return 0;
    }

    return backtrace_with_ra(pc, sp, ra, addrs, max, cfi_table_base);
}

/*============================================================================
 * Get task context from TCB (for non-current tasks)
 *============================================================================*/

/**
 * Get task SP, PC, and x1/RA from TCB (for saved task contexts)
 *
 * For RISC-V FreeRTOS:
 * - TCB->pxTopOfStack points to the BOTTOM of saved context stack
 * - Stack layout at context switch (portContext.h):
 *   sp[0]: mepc (PC)
 *   sp[1]: mstatus
 *   sp[fpu_words + 2]: x1 (ra)
 *   FPU state occupies 33 words between mstatus and x1 when FS is dirty.
 *
 * To get actual SP:
 *   - No FPU: pxTopOfStack + 31 words
 *   - With FPU: pxTopOfStack + 31 + 33 words
 */
static bool get_task_context_from_tcb(TaskHandle_t handle, uint32_t *sp,
                                      uint32_t *pc, uint32_t *ra)
{
    if (!handle || !sp || !pc || !ra) {
        return false;
    }

    /* TCB structure: first member is pxTopOfStack */
    typedef struct {
        volatile StackType_t *pxTopOfStack;  /* Points to saved context */
    } tcb_t;

    tcb_t *tcb = (tcb_t *)handle;
    uint32_t stack_top = (uint32_t)(tcb->pxTopOfStack);

    /* Read mstatus to detect FPU status */
    /* mstatus is at stack_top + 1 (word offset) */
    uint32_t mstatus = *(uint32_t*)(stack_top + 4);

    /* Calculate SP based on FPU status */
    /* FS bits [14:13] in mstatus: 0x3 = dirty (FPU context saved) */
    int fpu_context_words = 0;
    if ((mstatus & 0x6000) == 0x6000) {
        fpu_context_words = 33;
    }

    /* Actual SP points to top of general register area */
    *sp = stack_top + (31 + fpu_context_words) * 4;

    /* Get PC from stack: mepc is saved at stack_top[0] */
    *pc = *(uint32_t*)stack_top;

    /* FPU context, when present, is between mepc/mstatus and x1. */
    *ra = *(uint32_t*)(stack_top + (fpu_context_words + 2) * 4);

    return true;
}

static void backtrace_task_from_isr_cb(TaskHandle_t handle, eTaskState state)
{
    (void)state;
    uint32_t addrs[16];
    uint32_t sp = 0;
    uint32_t pc = 0;
    uint32_t ra = 0;
    int count = 0;
    bool is_cur = (handle == xTaskGetCurrentTaskHandle());
    const char *name = pcTaskGetName(handle);

    if (name == NULL) {
        name = "<noname>";
    }

    if (!get_task_context_from_tcb(handle, &sp, &pc, &ra)) {
        printf("%s%s: [failed to get context]\r\n", name, is_cur ? " *" : "");
        return;
    }

    count = backtrace_unwind_with_ra(pc, sp, ra, addrs, 16);

    printf("%s%s: ", name, is_cur ? " *" : "");
    for (int i = 0; i < count; i++) {
        printf("0x%08x ", addrs[i]);
    }
    printf("\r\n");
}

/*============================================================================
 * FreeRTOS Integration
 *============================================================================*/

static inline uintptr_t get_sp(void)
{
    uintptr_t sp;
    __asm__ volatile("mv %0, sp" : "=r"(sp));
    return sp;
}

static inline uintptr_t get_pc(void)
{
    uintptr_t pc;
    __asm__ volatile("auipc %0, 0" : "=r"(pc));
    return pc;
}

void backtrace_now(void)
{
    uint32_t addrs[CONFIG_BACKTRACE_DEPTH];
    uintptr_t sp = get_sp();
    uintptr_t pc = get_pc();
    char *task_name;

    if(taskSCHEDULER_NOT_STARTED == xTaskGetSchedulerState()) {
        task_name = "null";
    } else {
        /* Get current task name */
        TaskHandle_t cur = xTaskGetCurrentTaskHandle();
        task_name = pcTaskGetName(cur);
    }

    int count = backtrace_unwind(pc, sp, addrs, CONFIG_BACKTRACE_DEPTH);

    /* Print in same format as bt all */
    printf("%s: ", task_name);
    for (int i = 0; i < count; i++) {
        printf("0x%08x ", addrs[i]);
    }
    printf("\r\n");
}

void backtrace_tasks_all(void)
{
    if(taskSCHEDULER_NOT_STARTED ==  xTaskGetSchedulerState()) {
        return;
    }
    /* Use static array to avoid malloc in interrupt context */
    static TaskStatus_t tasks[32];  /* Support up to 32 tasks */
    UBaseType_t num = uxTaskGetNumberOfTasks();

    if (num > 32) {
        printf("[BACKTRACE] Too many tasks (%d), max supported is 32\r\n", num);
        num = 32;
    }

    uintptr_t cur_sp = get_sp();
    uintptr_t cur_pc = get_pc();

    taskENTER_CRITICAL();

    TaskHandle_t cur = xTaskGetCurrentTaskHandle();
    uxTaskGetSystemState(tasks, num, NULL);

    printf("\r\n========== Backtrace All Tasks ==========\r\n");

    for (UBaseType_t i = 0; i < num; i++) {
        uint32_t addrs[16];  /* Limited depth for task list */
        uint32_t sp, pc, ra = 0;

        bool is_cur = (tasks[i].xHandle == cur);

        if (is_cur) {
            /* Current task: use actual SP and PC */
            sp = cur_sp;
            pc = cur_pc;
        } else {
            /* Non-current task: extract from TCB */
            if (!get_task_context_from_tcb(tasks[i].xHandle, &sp, &pc, &ra)) {
                printf("%s: [failed to get context]\r\n", tasks[i].pcTaskName);
                continue;
            }
        }

        int count = is_cur ? backtrace_unwind(pc, sp, addrs, 16) :
                             backtrace_unwind_with_ra(pc, sp, ra, addrs, 16);
        printf("%s%s: ", tasks[i].pcTaskName, is_cur ? " *" : "");

        for (int j = 0; j < count; j++) {
            printf("0x%08x ", addrs[j]);
        }
        printf("\r\n");
    }

    printf("==========================================\r\n\r\n");

    taskEXIT_CRITICAL();
}

void backtrace_tasks_all_isr(void)
{
    if(taskSCHEDULER_NOT_STARTED ==  xTaskGetSchedulerState()) {
        return;
    }
    printf("\r\n========== Backtrace All Tasks (ISR) ==========\r\n");
    vTaskHandleForeachFromISR(backtrace_task_from_isr_cb);
    printf("================================================\r\n\r\n");
}

int backtrace_task(void *handle, uint32_t *addrs, int max)
{
    if (!handle || !addrs || max <= 0) {
        return 0;
    }

    uint32_t sp, pc, ra = 0;

    /* Check if this is the current task */
    TaskHandle_t cur = xTaskGetCurrentTaskHandle();
    bool is_cur = (handle == cur);

    if (is_cur) {
        /* Current task: use actual SP and PC */
        sp = get_sp();
        pc = get_pc();
    } else {
        /* Non-current task: extract from TCB */
        if (!get_task_context_from_tcb(handle, &sp, &pc, &ra)) {
            return 0;
        }
    }

    taskENTER_CRITICAL();
    int count = is_cur ? backtrace_unwind(pc, sp, addrs, max) :
                         backtrace_unwind_with_ra(pc, sp, ra, addrs, max);
    taskEXIT_CRITICAL();

    return count;
}

int backtrace_task_by_name(const char *name, uint32_t *addrs, int max)
{
    TaskHandle_t h = name ? xTaskGetHandle(name) : NULL;
    return h ? backtrace_task(h, addrs, max) : 0;
}

/*============================================================================
 * Shell Commands
 *============================================================================*/

#ifdef CONFIG_SHELL
#include <shell.h>
#include <stdlib.h>

int cmd_backtrace(int argc, char **argv)
{
    char name[configMAX_TASK_NAME_LEN + 1] = {0};

    /* Parse arguments */
    if (argc == 2) {
        strncpy(name, argv[1], configMAX_TASK_NAME_LEN);
        name[configMAX_TASK_NAME_LEN] = '\0';
    }

    /* Execute backtrace */
    if (!name[0] || strcmp(name, "all") == 0) {
        /* Display all tasks or current task */
        if (name[0] == 0) {
            backtrace_now();
        } else {
            backtrace_tasks_all();
        }
    } else {
        /* Backtrace specific task - use backtrace_task_by_name */
        static uint32_t addrs[64];
        int depth = backtrace_task_by_name(name, addrs, 64);
        if (depth) {
            printf("%s: ", name);
            for (int i = 0; i < depth; i++) {
                printf("0x%08x ", addrs[i]);
            }
            printf("\r\n");
        } else {
            printf("Task '%s' not found\r\n", name);
        }
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_backtrace, backtrace, DWARF CFI backtrace [task|all]);
#endif
