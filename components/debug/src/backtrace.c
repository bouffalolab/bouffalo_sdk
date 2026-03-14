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

/*============================================================================
 * Multi_bins Descriptor Parsing
 *============================================================================*/

/* Multi_bins descriptor format (16 bytes) */
typedef struct {
    char name[8];         /* Descriptor name */
    uint32_t start_addr;  /* Flash offset relative to firmware start */
    uint32_t end_addr;    /* End offset */
} __attribute__((packed)) multi_bin_desc_t;

/* Linker symbols for multi_bins array */
extern const uint8_t __multi_bins__[];

/*============================================================================
 * Internal Functions
 *============================================================================*/

/**
 * Get CFI table base address from multi_bins descriptor
 * Returns NULL if not found
 */
static const uint8_t* get_cfi_table_base(void)
{
    const uint8_t *array_ptr = __multi_bins__;
    const multi_bin_desc_t *desc;

    /* Iterate through descriptors until terminator (16 bytes each) */
    for (int i = 0; i < 16; i++) {
        desc = (const multi_bin_desc_t*)(array_ptr + i * 16);

        /* Check for terminator */
        if (desc->start_addr == 0xFFFFFFFF) {
            break;
        }

        /* Check if this is DWARFCFI descriptor (8-byte name) */
        if (memcmp(desc->name, "DWARFCFI", 8) == 0) {
            /* Convert firmware-relative offset to XIP mapped address */
            uint32_t cfi_flash_addr = desc->start_addr;
#ifdef BL616
            uint32_t cfi_xip_addr = (cfi_flash_addr - 0x1000) + 0xA0000000;
#elif defined(BL618DG)
            uint32_t cfi_xip_addr = (cfi_flash_addr - 0x1000) + 0x80000000;
#endif
            return (const uint8_t*)cfi_xip_addr;
        }
    }

    return NULL;  /* Not found */
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

/*============================================================================
 * Get task context from TCB (for non-current tasks)
 *============================================================================*/

/**
 * Get task SP and PC from TCB (for non-current tasks)
 *
 * For RISC-V FreeRTOS:
 * - TCB->pxTopOfStack points to the BOTTOM of saved context stack
 * - Stack layout at context switch (portContext.h):
 *   sp[0]: mepc (PC)
 *   sp[1]: mstatus
 *   sp[2]: x1 (ra)
 *   ...
 *   sp[29]: x31 (t6)
 *   sp[30]: xPS
 *   sp[31]: xCriticalNesting
 *   [+ FPU context: sp[32..64] if mstatus.FS=0x3 (dirty)]
 *
 * To get actual SP:
 *   - No FPU: pxTopOfStack + 31 words
 *   - With FPU: pxTopOfStack + 31 + 33 words
 */
static bool get_task_context_from_tcb(TaskHandle_t handle, uint32_t *sp, uint32_t *pc)
{
    if (!handle || !sp || !pc) {
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
    int stack_offset_words = 31;  /* Default: no FPU */
    if ((mstatus & 0x6000) == 0x6000) {
        stack_offset_words = 31 + 33;  /* FPU context: 33 words */
    }

    /* Actual SP points to top of general register area */
    *sp = stack_top + stack_offset_words * 4;

    /* Get PC from stack: mepc is saved at stack_top[0] */
    *pc = *(uint32_t*)stack_top;

    return true;
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

    /* Get current task name */
    TaskHandle_t cur = xTaskGetCurrentTaskHandle();
    const char *task_name = pcTaskGetName(cur);

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
        uint32_t sp, pc;

        bool is_cur = (tasks[i].xHandle == cur);

        if (is_cur) {
            /* Current task: use actual SP and PC */
            sp = cur_sp;
            pc = cur_pc;
        } else {
            /* Non-current task: extract from TCB */
            if (!get_task_context_from_tcb(tasks[i].xHandle, &sp, &pc)) {
                printf("%s: [failed to get context]\r\n", tasks[i].pcTaskName);
                continue;
            }
        }

        int count = backtrace_unwind(pc, sp, addrs, 16);
        printf("%s%s: ", tasks[i].pcTaskName, is_cur ? " *" : "");

        for (int j = 0; j < count; j++) {
            printf("0x%08x ", addrs[j]);
        }
        printf("\r\n");
    }

    printf("==========================================\r\n\r\n");

    taskEXIT_CRITICAL();
}

int backtrace_task(void *handle, uint32_t *addrs, int max)
{
    if (!handle || !addrs || max <= 0) {
        return 0;
    }

    uint32_t sp, pc;

    /* Check if this is the current task */
    TaskHandle_t cur = xTaskGetCurrentTaskHandle();
    bool is_cur = (handle == cur);

    if (is_cur) {
        /* Current task: use actual SP and PC */
        sp = get_sp();
        pc = get_pc();
    } else {
        /* Non-current task: extract from TCB */
        if (!get_task_context_from_tcb(handle, &sp, &pc)) {
            return 0;
        }
    }

    taskENTER_CRITICAL();
    int count = backtrace_unwind(pc, sp, addrs, max);
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
