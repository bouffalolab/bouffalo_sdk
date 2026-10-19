/**
 * @file heap5.h
 * @brief Instance-based heap5 allocator core for the mm framework
 *
 * Adapted from FreeRTOS heap_5.c to support per-heap instances inside the
 * Bouffalo SDK memory manager.
 */

#ifndef __MM_HEAP5_H
#define __MM_HEAP5_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct heap5_control *heap5_t;

struct heap5_stats {
    size_t total_size;
    size_t free_size;
    size_t min_free_size;
    size_t largest_free_block;
    size_t smallest_free_block;
    size_t alloc_block_count;
    size_t free_block_count;
    size_t alloc_count;
    size_t free_count;
};

size_t heap5_control_size(void);
void heap5_init(heap5_t heap);
int heap5_add_region(heap5_t heap, void *start_addr, size_t size_in_bytes);
void *heap5_malloc(heap5_t heap, size_t size);
void heap5_free(heap5_t heap, void *ptr);
size_t heap5_get_free_size(heap5_t heap);
size_t heap5_get_total_size(heap5_t heap);
size_t heap5_get_min_free_size(heap5_t heap);
void heap5_get_stats(heap5_t heap, struct heap5_stats *stats);

#ifdef __cplusplus
}
#endif

#endif /* __MM_HEAP5_H */
