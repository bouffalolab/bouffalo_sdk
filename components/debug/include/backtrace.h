/**
 * @file backtrace.h
 * @brief DWARF CFI-based backtrace support for Bouffalo SDK
 *
 * This module provides stack unwinding using DWARF Call Frame Information (CFI)
 * compiled into the firmware. It uses a compact 6-byte entry format optimized for
 * embedded systems.
 *
 * Binary format (version 4.1, 6-byte entries with function size):
 *
 *   1. Non-standard RA table (at file beginning):
 *      uint16_t num_non_standard_ra_entries;
 *      struct { uint16_t entry_idx; int16_t ra_offset; } entries[];
 *      uint16_t terminator;  // 0xFFFF
 *
 *   2. Segment table:
 *      struct { uint16_t segment_base; uint16_t start_entry_idx; uint16_t end_entry_idx; } segments[];
 *      uint32_t terminator[3];  // 0xFFFFFFFF × 3
 *
 *   3. Entries (6 bytes each, no padding):
 *      struct {
 *          uint16_t offset_in_segment | flag;  // bits 15-1: offset, bit 0: flag
 *          uint16_t frame_size_words;          // frame_size / 4
 *          uint16_t func_size;                 // function size in bytes
 *      }
 *
 *   IMPORTANT: Leaf functions (frame_size=0) are NOT in the table.
 *   During backtrace, if PC is not found, it's treated as a leaf function.
 */

#ifndef __BACKTRACE_H__
#define __BACKTRACE_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Configuration */
#ifndef CONFIG_BACKTRACE_DEPTH
#define CONFIG_BACKTRACE_DEPTH  64  /**< Maximum backtrace depth */
#endif

/*============================================================================
 * Core API
 *============================================================================*/

/**
 * @brief Perform backtrace using DWARF CFI
 *
 * Unwinds the stack starting from the given PC and SP addresses,
 * using the DWARF CFI table for accurate frame information.
 * The CFI table is automatically located from multi_bins descriptor.
 *
 * @param pc      Program counter (current execution address)
 * @param sp      Stack pointer
 * @param addrs   [out] Array to store backtrace addresses
 * @param max     Maximum number of frames to capture
 * @return Number of frames captured (0 on error)
 */
int backtrace_unwind(uint32_t pc, uint32_t sp,
                      uint32_t *addrs, int max);

/*============================================================================
 * FreeRTOS Integration
 *============================================================================*/

/**
 * @brief Backtrace current task
 *
 * Captures and prints the call stack of the currently running task.
 */
void backtrace_now(void);

/**
 * @brief Backtrace all tasks
 *
 * Captures and prints the call stack of all tasks in the system.
 * The currently running task is marked with '*'.
 */
void backtrace_tasks_all(void);

/**
 * @brief Backtrace all tasks from ISR/exception context
 *
 * ISR-safe variant for exception handlers. It avoids scheduler APIs that may
 * suspend/resume tasks and iterates task lists via ISR foreach helper.
 */
void backtrace_tasks_all_isr(void);

/**
 * @brief Backtrace specified task
 *
 * @param handle  Task handle
 * @param addrs   [out] Array to store backtrace addresses
 * @param max     Maximum number of frames to capture
 * @return Number of frames captured (0 on failure)
 */
int backtrace_task(void *handle, uint32_t *addrs, int max);

/**
 * @brief Backtrace task by name
 *
 * @param name    Task name
 * @param addrs   [out] Array to store backtrace addresses
 * @param max     Maximum number of frames to capture
 * @return Number of frames captured (0 if task not found)
 */
int backtrace_task_by_name(const char *name, uint32_t *addrs, int max);

/*============================================================================
 * Legacy API Compatibility (dwarf_cfi)
 *============================================================================*/

/* Legacy aliases for backward compatibility */
#define dwarf_cfi_backtrace(pc, sp, addrs, max)  backtrace_unwind(pc, sp, addrs, max)
#define dwarf_cfi_backtrace_now()      backtrace_now()
#define dwarf_cfi_backtrace_task()     backtrace_task()
#define dwarf_cfi_backtrace_tasks_all() backtrace_tasks_all()
#define dwarf_cfi_backtrace_task_by_name backtrace_task_by_name
#define dwarf_cfi_lookup_function(addr) NULL  /* Not supported in new implementation */

/*============================================================================
 * Shell Commands
 *============================================================================*/

#ifdef CONFIG_SHELL
/**
 * @brief Shell command: bt [task] [depth]
 *
 * Perform backtrace using DWARF CFI information.
 *
 * Usage:
 *   bt                  - Backtrace current task
 *   bt all              - Backtrace all tasks
 *   bt <name>           - Backtrace named task
 *   bt <name> <N>       - Backtrace with max depth N
 *
 * @param argc  Argument count
 * @param argv  Argument vector
 * @return 0 on success
 */
int cmd_backtrace(int argc, char **argv);

/* Legacy alias */
#define cmd_dwarf_cfi_backtrace cmd_backtrace
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BACKTRACE_H__ */
