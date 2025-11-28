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

    allocator->get_usage_info(heap, &usage);

    LOG_I("\033[32mHeap id:%u (%s, %s)\033[0m, Usage Info:\r\n",
          heap_id, heap->name, allocator->name);

    LOG_I("%10s  %10s  %10s  %13s  %11s  %11s\r\n",
          "Total_Size", "Free_Size", "Used_Size", "Max_Free_Size", "Free_Nodes", "Alloc_Nodes");
    LOG_I("%10u  %10u  %10u  %13u  %11u  %11u\r\n",
          usage.total_size,
          usage.free_size,
          usage.used_size,
          usage.max_free_size,
          (uint32_t)usage.free_node,
          (uint32_t)usage.alloc_node);

#if (CONFIG_MM_ENABLE_STATISTICS)
    LOG_I("%10s  %10s  %10s  %13s  %11s\r\n",
          "Start_Addr", "End_Addr", "Total_Size", "Alloc_Count", "Free_Count");
    LOG_I("0x%08X  0x%08X  %10u  %13u  %11u\r\n",
          (uint32_t)(uintptr_t)heap->start_addr,
          (uint32_t)(uintptr_t)heap->end_addr,
          (uint32_t)heap->total_size,
          heap->stats.alloc_count,
          heap->stats.free_count);

    if ((uint32_t)heap->total_size != usage.total_size) {
        LOG_E("Heap size mismatch, Memory may be corruption\r\n");
    }
#endif
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
#if (CONFIG_MM_ENABLE_STATISTICS)
        LOG_I("\033[32mGlobal Statistics\033[0m: \r\n");
        LOG_I("  Total malloc successes: %u\r\n", g_mem_manager.stats.total_malloc_successes);
        LOG_I("  Total free successes  : %u\r\n", g_mem_manager.stats.total_free_successes);
        LOG_I("  Total malloc failures : %u\r\n\r\n", g_mem_manager.stats.total_malloc_failures);
#endif
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

/**
 * @brief Return remaining OCRAM heap size for legacy compatibility.
 *
 * Historically `kfree_size` reported the free bytes of the OCRAM heap.
 * This implementation locates the OCRAM heap (MM_HEAP_OCRAM_0) and queries
 * its allocator for current usage statistics. If the manager is not
 * initialized, the heap is inactive, or the allocator cannot provide usage
 * information, the function returns 0.
 *
 * @return Remaining free bytes in the OCRAM heap, or 0 on failure.
 */
size_t kfree_size(void)
{
    mem_manager_t *manager = &g_mem_manager;
    mm_heap_t *heap;
    const mm_allocator_t *allocator;
    struct mm_usage_info usage;

    if (!manager->initialized) {
        return 0;
    }

    if (MM_HEAP_OCRAM_0 >= CONFIG_MM_HEAP_COUNT) {
        return 0;
    }

    heap = manager->heaps[MM_HEAP_OCRAM_0];
    if (!heap || !heap->is_active) {
        return 0;
    }

    allocator = manager->allocators[heap->allocator_id];
    if (!allocator || !allocator->get_usage_info) {
        return 0;
    }

    allocator->get_usage_info(heap, &usage);

    return (size_t)usage.free_size;
}

#if defined(CONFIG_SHELL)
#include <shell.h>

SHELL_CMD_EXPORT_ALIAS(cmd_mm_info, mm_info, show memory usage);

#endif
