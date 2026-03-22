/**
 * @file mm_debug.c
 * @brief Debug helpers for the Bouffalo SDK memory management framework
 * @version 1.0
 * @date 2025
 *
 * Provides shell command helpers and legacy compatibility utilities such as
 * OCRAM free size queries.
 *
 * Copyright (c) 2025 Bouffalo Lab
 */

#include "mm.h"
#include <stdlib.h>

#define DBG_TAG "MM"
#ifdef CONFIG_MM_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_MM_LOG_LEVEL
#endif
#include "log.h"

/**
 * @brief Dump memory heap information
 *
 * Logs detailed information about the specified heap, including usage statistics
 * and allocator details.
 *
 * @param heap_id ID of the heap to dump information for
 */
static void mm_dump_info(uint32_t heap_id)
{
    mm_heap_t *heap;
    const mm_allocator_t *allocator;
    struct mm_usage_info usage;
    size_t free_size;
    uintptr_t irq_flags;

    heap = g_mem_manager.heaps[heap_id];
    if (!heap || !heap->is_active) {
        LOG_I("Heap %u is not active or does not exist.\r\n", heap_id);
        return;
    }

    allocator = g_mem_manager.allocators[heap->allocator_id];
    if (!allocator || !allocator->get_usage_info) {
        LOG_I("Allocator for heap %u does not support usage info.\r\n", heap_id);
        return;
    }

    irq_flags = mm_lock_save();
    allocator->get_usage_info(heap, &usage);
    free_size = allocator->get_free_size(heap);
    mm_unlock_restore(irq_flags);

    /* clang-format off */
    LOG_I("\033[32mHeap id:%u (%s, %s)\033[0m, Usage Info:\r\n",
          heap_id, heap->name, allocator->name);

    LOG_I("%10s  %10s  %10s  %12s  %11s  %13s\r\n",
          "Total_Size", "Free_Size", "Used_Size", "Alloc_Nodes", "Free_Nodes", "Max_Free_Size");
    LOG_I("%10u  %10u  %10u  %12u  %11u  %13u\r\n",
          usage.total_size,
          usage.free_size,
          usage.used_size,
          (uint32_t)usage.alloc_node,
          (uint32_t)usage.free_node,
          usage.max_free_size);

#if (CONFIG_MM_ENABLE_STATISTICS)
    LOG_I("%10s  %10s  %10s  %12s  %11s\r\n",
          "Start_Addr", "End_Addr", "Total_Size", "Alloc_Count", "Free_Count");
    LOG_I("0x%08X  0x%08X  %10u  %12u  %11u\r\n",
          (uint32_t)(uintptr_t)heap->start_addr,
          (uint32_t)(uintptr_t)heap->end_addr,
          (uint32_t)heap->total_size,
          heap->stats.alloc_count,
          heap->stats.free_count);
    /* clang-format on */

    if ((uint32_t)heap->total_size != usage.total_size) {
        LOG_E("Heap size mismatch, Memory may be corruption\r\n");
    }
#endif

    if (free_size != usage.free_size) {
        LOG_E("Heap free size mismatch: %u vs %u, Memory may be corruption\r\n",
              (uint32_t)free_size, (uint32_t)usage.free_size);
    }

    LOG_RI("\r\n");
}

/**
 * @brief Print heap usage information via shell command.
 *
 * With no arguments, dumps all active heaps (and global stats if enabled).
 * When a heap ID is supplied, only the specified heap is reported.
 *
 * @param argc Argument count from shell invocation.
 * @param argv Argument vector containing optional heap ID.
 * @return 0 on success, negative value on usage error.
 */
int cmd_mm_info(int argc, char **argv)
{
    if (argc == 1) {
        LOG_I("\033[32mGlobal Statistics\033[0m: \r\n");
        LOG_I("  Total free bytes      : %u\r\n", (uint32_t)kfree_size(0));
#if CONFIG_MM_ENABLE_MIN_FREE_TRACKING
        LOG_I("  Total min free bytes  : %u\r\n", (uint32_t)kmin_free_size());
#endif
#if (CONFIG_MM_ENABLE_STATISTICS)
        LOG_I("  Total malloc successes: %u\r\n", g_mem_manager.stats.total_malloc_successes);
        LOG_I("  Total free successes  : %u\r\n", g_mem_manager.stats.total_free_successes);
        LOG_I("  Total malloc failures : %u\r\n", g_mem_manager.stats.total_malloc_failures);
#endif
        LOG_I("\r\n");

        /* dump all heaps info */
        for (uint32_t i = 0; i < CONFIG_MM_HEAP_COUNT; i++) {
            mm_heap_t *heap = g_mem_manager.heaps[i];
            if (!heap || !heap->is_active) {
                continue;
            }
            mm_dump_info(i);
        }

    } else if (argc == 2) {
        /* dump specified heap info */
        uint32_t heap_id = atoi(argv[1]);
        if (heap_id >= CONFIG_MM_HEAP_COUNT) {
            LOG_E("Invalid heap ID: %u\r\n", heap_id);
            return -1;
        }

        mm_dump_info(heap_id);
    } else {
        LOG_E("Usage: mminfo [heap_id]\r\n");
        return -1;
    }

    return 0;
}

#if defined(CONFIG_SHELL)
#include <shell.h>

SHELL_CMD_EXPORT_ALIAS(cmd_mm_info, mm_info, show memory usage);

#endif
