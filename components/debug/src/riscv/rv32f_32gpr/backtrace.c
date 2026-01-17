#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "backtrace.h"
#include "FreeRTOS.h"
#include "task.h"

/* Code segment boundary symbols, defined by linker script */
extern char __text_code_start__[];
extern char __text_code_end__[];

#define BT_FUNC_LIMIT       0x2000

/*
 * Basic context (no FPU):
 *   [0]  mepc
 *   [1]  mstatus
 *   [2]  ra (x1)
 *   [3-29] x5-x31 (27 GPRs)
 *   [30] xCriticalNesting
 *   Total: 31 words
 *
 * FPU context (configENABLE_FPU=1 and FS=Dirty):
 *   [0]  mepc
 *   [1]  mstatus
 *   [2-34] f0-f31, fcsr (33 FPU regs)
 *   [35] ra (x1)
 *   [36-62] x5-x31 (27 GPRs)
 *   [63] xCriticalNesting
 *   Total: 31 + 33 = 64 words
 */
#define CTX_BASE_SIZE           31      /**< Basic context size (words) */
#define CTX_MEPC_OFFSET         0       /**< mepc offset in context */
#define CTX_MSTATUS_OFFSET      1       /**< mstatus offset in context */
#define CTX_RA_OFFSET_NO_FPU    2       /**< ra offset when no FPU */

#define CTX_FPU_SIZE            33      /**< FPU context size (f0-f31 + fcsr) */
#define CTX_RA_OFFSET_FPU       (CTX_RA_OFFSET_NO_FPU + CTX_FPU_SIZE) /**< ra offset with FPU */

#define MSTATUS_FS_MASK         0x6000  /**< FS field mask in mstatus */
#define MSTATUS_FS_DIRTY        0x6000  /**< FS=Dirty indicates FPU was used */

#ifndef CONFIG_BACKTRACE_DEPTH
#define CONFIG_BACKTRACE_DEPTH  64      /**< Default maximum backtrace depth */
#endif

/*============================================================================
 * Helper Functions
 *============================================================================*/

static inline bool is_code_addr(uintptr_t addr)
{
    return addr >= (uintptr_t)__text_code_start__ &&
           addr < (uintptr_t)__text_code_end__;
}

static inline __attribute__((always_inline)) uintptr_t get_sp(void)
{
    uintptr_t sp;
    __asm__ volatile("mv %0, sp" : "=r"(sp));
    return sp;
}

static inline __attribute__((always_inline)) uintptr_t get_pc(void)
{
    uintptr_t pc;
    __asm__ volatile("auipc %0, 0" : "=r"(pc));
    return pc;
}

static inline uint32_t read_u32_unaligned(uintptr_t addr)
{
    uint8_t *p = (uint8_t *)addr;
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/*============================================================================
 * Instruction Parsing Functions
 *============================================================================*/

static int get_framesize_16(uint16_t inst)
{
    uint32_t imm;

    /* c.addi sp, -imm: opcode 0x1101, 6-bit immediate */
    if ((inst & 0xFF83) == 0x1101) {
        imm = (inst >> 2) & 0x1F;
        imm |= ((inst >> 12) & 0x1) << 5;
        if (imm & 0x20) {
            imm = (~imm & 0x3F) + 1;  /* Sign extend and negate */
        }
        return imm >> 2;  /* Convert to word count */
    }

    /* c.addi16sp -imm: opcode 0x7101, immediate requires special decoding */
    if ((inst & 0xFF83) == 0x7101) {
        imm = (inst >> 3) & 0x3;
        imm = (imm << 1) | ((inst >> 5) & 0x1);
        imm = (imm << 1) | ((inst >> 2) & 0x1);
        imm = (imm << 1) | ((inst >> 6) & 0x1);
        imm |= ((inst >> 12) & 0x1) << 5;
        if (imm & 0x20) {
            imm = ((~imm & 0x3F) + 1) << 4;
        } else {
            imm <<= 4;
        }
        return imm >> 2;
    }

    return -1;
}

static int get_framesize_32(uint32_t inst)
{
    /* Check if it's an addi instruction: opcode=0x13, funct3=0 */
    if ((inst & 0x707F) != 0x0013) {
        return -1;
    }

    uint32_t rd = (inst >> 7) & 0x1F;
    uint32_t rs1 = (inst >> 15) & 0x1F;

    /* Must be addi sp, sp, imm */
    if (rd != 2 || rs1 != 2) {
        return -1;
    }

    /* Extract 12-bit signed immediate */
    int32_t imm = (int32_t)(inst & 0xFFF00000) >> 20;

    /* Stack frame allocation should be negative (sp decreases) */
    if (imm >= 0) {
        return -1;
    }

    return (-imm) >> 2;
}

static int get_ra_offset_16(uint16_t inst)
{
    /* c.sw rs2, offset(sp): opcode pattern 0xC006 */
    if ((inst & 0xE07F) == 0xC006) {
        uint32_t imm = (inst >> 7) & 0x3;
        imm = (imm << 4) | ((inst >> 9) & 0xF);
        return imm;
    }
    return -1;
}

static int get_ra_offset_32(uint32_t inst)
{
    /* sw instruction: opcode=0x23, funct3=2 */
    if ((inst & 0x707F) == 0x2023) {
        uint32_t rs1 = (inst >> 15) & 0x1F;
        uint32_t rs2 = (inst >> 20) & 0x1F;
        /* Must be sw ra, offset(sp) */
        if (rs1 == 2 && rs2 == 1) {
            uint32_t imm = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
            return imm >> 2;
        }
    }
    return -1;
}

static int get_call_inst_size(uintptr_t lr)
{
    if (!is_code_addr(lr) || lr < 4) {
        return 0;
    }

    uint32_t inst = read_u32_unaligned(lr - 4);
    /* RISC-V: lower 2 bits 0b11 indicate 32-bit instruction */
    return ((inst & 0x3) == 0x3) ? 4 : 2;
}

/*============================================================================
 * Core Backtrace Algorithm
 *============================================================================*/

/**
 * @brief Unwind one stack frame
 *
 * Algorithm steps:
 * 1. Search upward from current PC for function entry (stack frame allocation instruction)
 * 2. Search between function entry and current PC for ra save instruction
 * 3. Read saved return address from stack
 * 4. Update SP and PC for next level backtrace
 *
 * @param pSP    [in/out] Stack pointer
 * @param pPC    [in/out] Program counter
 * @param bt     [out] Backtrace address array
 * @param depth  [in/out] Current backtrace depth
 * @param max    Maximum backtrace depth
 * @return 0 success, non-zero for failure or end
 *
 * @note Use O0 optimization to prevent compiler optimizations from affecting stack frame analysis
 */
__attribute__((optimize("O0")))
static int backtrace_from_stack(uintptr_t *pSP, uintptr_t *pPC,
                                uintptr_t *bt, int *depth, int max)
{
    uintptr_t *sp = (uintptr_t *)*pSP;
    uintptr_t pc = *pPC;
    uintptr_t code_addr = 0;
    int framesize = -1;
    int ra_offset = -1;

    if (!is_code_addr(pc)) {
        return -1;
    }

    /* Search upward for function entry (stack frame allocation instruction) */
    for (int i = 0; i < BT_FUNC_LIMIT; i += 2) {
        code_addr = (pc & ~0x1) - i;
        if (!is_code_addr(code_addr)) {
            break;
        }

        /* Try 16-bit compressed instruction */
        uint16_t inst16 = *(uint16_t *)code_addr;
        framesize = get_framesize_16(inst16);
        if (framesize > 0) {
            break;
        }

        /* Try 32-bit instruction */
        if (is_code_addr(code_addr + 3)) {
            uint32_t inst32 = read_u32_unaligned(code_addr);
            framesize = get_framesize_32(inst32);
            if (framesize > 0) {
                break;
            }
        }
    }

    if (framesize <= 0) {
        return -1;
    }

    /* Search for ra save instruction */
    for (int i = 0; code_addr + i < pc; i += 2) {
        uint16_t inst16 = *(uint16_t *)(code_addr + i);
        ra_offset = get_ra_offset_16(inst16);
        if (ra_offset >= 0) {
            break;
        }

        if (code_addr + i + 4 <= pc) {
            uint32_t inst32 = read_u32_unaligned(code_addr + i);
            ra_offset = get_ra_offset_32(inst32);
            if (ra_offset >= 0) {
                break;
            }
        }
    }

    if (ra_offset < 0) {
        return -1;
    }

    /* Read return address from stack */
    uintptr_t lr = sp[ra_offset];

    if (lr == 0 || !is_code_addr(lr)) {
        return -1;
    }

    /* Calculate call site address and record */
    int inst_size = get_call_inst_size(lr);
    uintptr_t call_addr = lr - inst_size;

    if (*depth < max && is_code_addr(call_addr)) {
        bt[(*depth)++] = call_addr;
    }

    /* Update SP/PC for next level */
    *pSP = (uintptr_t)(sp + framesize);
    *pPC = call_addr;

    return (inst_size == 0) ? 1 : 0;
}

/**
 * @brief Perform complete stack backtrace
 *
 * @param sp          Starting stack pointer
 * @param pc          Starting program counter
 * @param bt         [out] Backtrace address array
 * @param max         Maximum backtrace depth
 * @param is_current  Whether it's the current task
 * @return Actual backtrace depth
 */
static int do_backtrace(uintptr_t sp, uintptr_t pc, uintptr_t *bt,
                        int max, bool is_current)
{
    int depth = 0;

    /* Non-current task: record current PC first */
    if (!is_current && depth < max && is_code_addr(pc)) {
        bt[depth++] = pc;
    }

    /* Unwind frame by frame */
    while (depth < max) {
        if (backtrace_from_stack(&sp, &pc, bt, &depth, max) != 0) {
            break;
        }
    }

    return depth;
}

/**
 * @brief Get task's SP and PC
 *
 * Extract context information from FreeRTOS TCB
 *
 * @param handle  Task handle
 * @param cur     Current task handle
 * @param cur_sp  Current task SP (use if it's the current task)
 * @param cur_pc  Current task PC (use if it's the current task)
 * @param sp      [out] Task SP
 * @param pc      [out] Task PC
 * @return true if it's the current task
 */
static bool get_task_sp_pc(TaskHandle_t handle, TaskHandle_t cur,
                           uintptr_t cur_sp, uintptr_t cur_pc,
                           uintptr_t *sp, uintptr_t *pc)
{
    bool is_cur = (handle == cur);

    if (is_cur) {
        *sp = cur_sp;
        *pc = cur_pc;
    } else {
        /* TCB first field is stack top pointer, pointing to saved context */
        uintptr_t *ctx = *(uintptr_t **)handle;
        uintptr_t mepc = ctx[CTX_MEPC_OFFSET];
        uintptr_t mstatus = ctx[CTX_MSTATUS_OFFSET];

        int ra_offset = CTX_RA_OFFSET_NO_FPU;
        int ctx_size = CTX_BASE_SIZE;

#if (configENABLE_FPU == 1)
        /* Check if FPU context was saved */
        if ((mstatus & MSTATUS_FS_MASK) == MSTATUS_FS_DIRTY) {
            ra_offset = CTX_RA_OFFSET_FPU;
            ctx_size = CTX_BASE_SIZE + CTX_FPU_SIZE;
        }
#endif
        uintptr_t ra = ctx[ra_offset];

        /* Calculate task's actual stack top (after context) */
        *sp = (uintptr_t)(ctx + ctx_size);
        /* Prefer mepc, use ra if mepc is invalid */
        *pc = is_code_addr(mepc) ? mepc : ra;
    }

    return is_cur;
}

/**
 * @brief Print backtrace results
 *
 * @param name    Task name
 * @param bt      Backtrace address array
 * @param depth   Backtrace depth
 * @param is_cur  Whether it's the current task (display * mark)
 */
static void print_bt(const char *name, uintptr_t *bt, int depth, bool is_cur)
{
    printf("%-12s: %s", name, is_cur ? "*" : "");
    for (int i = 0; i < depth; i++) {
        printf("0x%lx ", (unsigned long)bt[i]);
    }
    printf("%s\r\n", depth ? "" : "-");
}

/*============================================================================
 * Public API
 *============================================================================*/

/**
 * @brief Print stack backtrace of the current task
 *
 * Get the call stack at current position and print it for debugging and exception analysis
 *
 * @code
 * void my_function(void) {
 *     backtrace_now();  // Print the call chain up to this point
 * }
 * @endcode
 */
void backtrace_now(void)
{
    uintptr_t bt[CONFIG_BACKTRACE_DEPTH];
    uintptr_t sp = get_sp();
    uintptr_t pc = get_pc();

    int depth = do_backtrace(sp, pc, bt, CONFIG_BACKTRACE_DEPTH, true);
    print_bt(pcTaskGetName(NULL), bt, depth, true);
}

/**
 * @brief Print stack backtrace for all tasks
 *
 * Traverse all tasks in the system and print each task's call stack
 * The currently running task is marked with *
 */
__attribute__((optimize("O0")))
void backtrace_tasks_all(void)
{
    UBaseType_t num = uxTaskGetNumberOfTasks();
    TaskStatus_t *tasks = pvPortMalloc(num * sizeof(TaskStatus_t));
    if (!tasks) {
        printf("bt: malloc failed\r\n");
        return;
    }

    uintptr_t cur_sp = get_sp();
    uintptr_t cur_pc = get_pc();

    taskENTER_CRITICAL();

    TaskHandle_t cur = xTaskGetCurrentTaskHandle();
    num = uxTaskGetSystemState(tasks, num, NULL);

    printf("========== Backtrace ==========\r\n");

    for (UBaseType_t i = 0; i < num; i++) {
        uintptr_t bt[CONFIG_BACKTRACE_DEPTH] = {0};
        uintptr_t sp, pc;

        bool is_cur = get_task_sp_pc(tasks[i].xHandle, cur,
                                     cur_sp, cur_pc, &sp, &pc);
        int depth = do_backtrace(sp, pc, bt, CONFIG_BACKTRACE_DEPTH, is_cur);
        print_bt(tasks[i].pcTaskName, bt, depth, is_cur);
    }

    printf("===============================\r\n");

    taskEXIT_CRITICAL();
    vPortFree(tasks);
}

/**
 * @brief Get stack backtrace for the specified task
 *
 * @param handle Task handle
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
int backtrace_task(TaskHandle_t handle, uintptr_t *bt, int max)
{
    if (!handle || !bt || max <= 0) {
        return 0;
    }

    uintptr_t cur_sp = get_sp();
    uintptr_t cur_pc = get_pc();

    taskENTER_CRITICAL();

    TaskHandle_t cur = xTaskGetCurrentTaskHandle();
    uintptr_t sp, pc;
    bool is_cur = get_task_sp_pc(handle, cur, cur_sp, cur_pc, &sp, &pc);
    int depth = do_backtrace(sp, pc, bt, max, is_cur);

    taskEXIT_CRITICAL();

    return depth;
}

/**
 * @brief Get stack backtrace by task name
 *
 * @param name Task name string
 * @param bt   [out] Array to store backtrace addresses
 * @param max  Maximum capacity of the array
 * @return Actual backtrace depth obtained, 0 if task doesn't exist
 */
int backtrace_task_by_name(const char *name, uintptr_t *bt, int max)
{
    TaskHandle_t h = name ? xTaskGetHandle(name) : NULL;
    return h ? backtrace_task(h, bt, max) : 0;
}

/*============================================================================
 * Shell Command Interface
 *============================================================================*/

#ifdef CONFIG_SHELL
#include <shell.h>
#include <stdlib.h>

/** Shell command argument structure */
typedef struct {
    char name[configMAX_TASK_NAME_LEN + 1];  /**< Task name */
    int max_depth;                            /**< Maximum backtrace depth */
} bt_cmd_arg_t;

/**
 * @brief bt_task command execution task
 *
 * Execute backtrace in an independent task to avoid blocking shell
 *
 * @param arg bt_cmd_arg_t pointer
 */
static void bt_cmd_task(void *arg)
{
    bt_cmd_arg_t *bt_arg = (bt_cmd_arg_t *)arg;

    if (!bt_arg->name[0]) {
        /* No task name: display all tasks */
        backtrace_tasks_all();
        goto out;
    }

    uintptr_t *bt = pvPortMalloc(bt_arg->max_depth * sizeof(uintptr_t));
    if (!bt) {
        printf("bt: malloc failed\r\n");
        goto out;
    }

    int depth = backtrace_task_by_name(bt_arg->name, bt, bt_arg->max_depth);
    if (depth) {
        print_bt(bt_arg->name, bt, depth, false);
    } else {
        printf("Task '%s' not found\r\n", bt_arg->name);
    }

    vPortFree(bt);

out:
    vPortFree(bt_arg);
    vTaskDelete(NULL);
}

static bool is_number(const char *str)
{
    if (!str || !*str) return false;
    char *endptr;
    strtol(str, &endptr, 10);
    return *endptr == '\0';
}

/**
 * @brief bt_task Shell command processing function
 *
 * Command format:
 *   bt_task                  - Display all tasks
 *   bt_task "task"           - Specify task only
 *   bt_task "task" [depth]   - Specify task and depth
 */
int cmd_backtrace(int argc, char **argv)
{
    bt_cmd_arg_t *bt_arg = pvPortMalloc(sizeof(bt_cmd_arg_t));
    if (!bt_arg) {
        return -1;
    }

    bt_arg->name[0] = '\0';
    bt_arg->max_depth = CONFIG_BACKTRACE_DEPTH;

    if (argc == 2) {
        if (is_number(argv[1])) {
            bt_arg->max_depth = atoi(argv[1]);
        } else {
            strncpy(bt_arg->name, argv[1], configMAX_TASK_NAME_LEN);
            bt_arg->name[configMAX_TASK_NAME_LEN] = '\0';
        }
    } else if (argc == 3) {
        strncpy(bt_arg->name, argv[1], configMAX_TASK_NAME_LEN);
        bt_arg->name[configMAX_TASK_NAME_LEN] = '\0';
        if (is_number(argv[2])) {
            bt_arg->max_depth = atoi(argv[2]);
        }
    }

    xTaskCreate(bt_cmd_task, "bt_task", 1024, bt_arg, 10, NULL);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_backtrace, bt_task, backtrace ["task"] [depth]);
#endif
