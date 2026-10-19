/**
 * @file heap5.c
 * @brief Instance-based heap5 allocator core
 *
 * Adapted from FreeRTOS heap_5.c. This version removes the global allocator
 * state and exposes an instance API for the mm framework.
 */

#include "heap5.h"
#include <stdint.h>
#include <string.h>

#ifndef CONFIG_MM_HEAP5_REGION_MAX
#define CONFIG_MM_HEAP5_REGION_MAX 4
#endif

#define heap5BITS_PER_BYTE             ((size_t)8)
#define heap5SIZE_MAX                  (~((size_t)0))
#define heap5BYTE_ALIGNMENT            (sizeof(uintptr_t))
#define heap5BYTE_ALIGNMENT_MASK       (heap5BYTE_ALIGNMENT - 1U)
#define heap5BLOCK_ALLOCATED_BITMASK   (((size_t)1) << ((sizeof(size_t) * heap5BITS_PER_BYTE) - 1U))
#define heap5BLOCK_SIZE(block)         ((block)->xBlockSize & ~heap5BLOCK_ALLOCATED_BITMASK)
#define heap5BLOCK_IS_ALLOCATED(block) (((block)->xBlockSize & heap5BLOCK_ALLOCATED_BITMASK) != 0U)
#define heap5ALLOCATE_BLOCK(block)     ((block)->xBlockSize |= heap5BLOCK_ALLOCATED_BITMASK)
#define heap5FREE_BLOCK(block)         ((block)->xBlockSize &= ~heap5BLOCK_ALLOCATED_BITMASK)
#define heap5SIZE_IS_VALID(size)       (((size) & heap5BLOCK_ALLOCATED_BITMASK) == 0U)

typedef struct heap5_block_link {
    struct heap5_block_link *pxNextFreeBlock;
    size_t xBlockSize;
} heap5_block_link_t;

typedef struct heap5_region_desc {
    heap5_block_link_t *first_block;
    heap5_block_link_t *end_block;
} heap5_region_desc_t;

struct heap5_control {
    heap5_block_link_t xStart;
    heap5_block_link_t *pxEnd;
    size_t xFreeBytesRemaining;
    size_t xMinimumEverFreeBytesRemaining;
    size_t xTotalHeapSize;
    size_t xNumberOfSuccessfulAllocations;
    size_t xNumberOfSuccessfulFrees;
    uint32_t region_count;
    heap5_region_desc_t regions[CONFIG_MM_HEAP5_REGION_MAX];
};

static const size_t xHeapStructSize =
    (sizeof(heap5_block_link_t) + (heap5BYTE_ALIGNMENT - 1U)) & ~(heap5BYTE_ALIGNMENT_MASK);

#define heap5MINIMUM_BLOCK_SIZE ((size_t)(xHeapStructSize << 1U))

static int heap5_add_will_overflow(size_t a, size_t b)
{
    return a > (heap5SIZE_MAX - b);
}

static void heap5_insert_block_into_free_list(heap5_t heap, heap5_block_link_t *block_to_insert)
{
    heap5_block_link_t *iterator;
    uint8_t *puc;

    for (iterator = &heap->xStart;
         iterator->pxNextFreeBlock < block_to_insert;
         iterator = iterator->pxNextFreeBlock) {
    }

    puc = (uint8_t *)iterator;
    if ((puc + heap5BLOCK_SIZE(iterator)) == (uint8_t *)block_to_insert) {
        iterator->xBlockSize += heap5BLOCK_SIZE(block_to_insert);
        block_to_insert = iterator;
    }

    puc = (uint8_t *)block_to_insert;
    if ((puc + heap5BLOCK_SIZE(block_to_insert)) == (uint8_t *)iterator->pxNextFreeBlock) {
        if (iterator->pxNextFreeBlock != heap->pxEnd) {
            block_to_insert->xBlockSize += heap5BLOCK_SIZE(iterator->pxNextFreeBlock);
            block_to_insert->pxNextFreeBlock = iterator->pxNextFreeBlock->pxNextFreeBlock;
        } else {
            block_to_insert->pxNextFreeBlock = heap->pxEnd;
        }
    } else {
        block_to_insert->pxNextFreeBlock = iterator->pxNextFreeBlock;
    }

    if (iterator != block_to_insert) {
        iterator->pxNextFreeBlock = block_to_insert;
    }
}

size_t heap5_control_size(void)
{
    return (sizeof(struct heap5_control) + (heap5BYTE_ALIGNMENT - 1U)) & ~(heap5BYTE_ALIGNMENT_MASK);
}

void heap5_init(heap5_t heap)
{
    if (heap) {
        memset(heap, 0, sizeof(*heap));
    }
}

int heap5_add_region(heap5_t heap, void *start_addr, size_t size_in_bytes)
{
    heap5_block_link_t *first_free_block;
    heap5_block_link_t *previous_end;
    heap5_block_link_t *end_marker;
    uintptr_t aligned_start;
    uintptr_t address;
    size_t total_region_size;

    if (!heap || !start_addr || size_in_bytes == 0U) {
        return -1;
    }

    if (heap->region_count >= CONFIG_MM_HEAP5_REGION_MAX) {
        return -1;
    }

    total_region_size = size_in_bytes;
    address = (uintptr_t)start_addr;

    if ((address & heap5BYTE_ALIGNMENT_MASK) != 0U) {
        aligned_start = (address + heap5BYTE_ALIGNMENT - 1U) & ~(uintptr_t)heap5BYTE_ALIGNMENT_MASK;
        if (aligned_start <= address || (aligned_start - address) >= total_region_size) {
            return -1;
        }
        total_region_size -= (size_t)(aligned_start - address);
        address = aligned_start;
    }

    if (total_region_size <= xHeapStructSize) {
        return -1;
    }

    if (heap->region_count == 0U) {
        heap->xStart.pxNextFreeBlock = (heap5_block_link_t *)address;
        heap->xStart.xBlockSize = 0U;
        previous_end = NULL;
    } else {
        previous_end = heap->pxEnd;
        if (address <= (uintptr_t)previous_end) {
            return -1;
        }
    }

    if (heap5_add_will_overflow(address, total_region_size)) {
        return -1;
    }

    aligned_start = address + total_region_size;
    aligned_start -= xHeapStructSize;
    aligned_start &= ~(uintptr_t)heap5BYTE_ALIGNMENT_MASK;
    if (aligned_start <= address) {
        return -1;
    }

    end_marker = (heap5_block_link_t *)aligned_start;
    end_marker->xBlockSize = 0U;
    end_marker->pxNextFreeBlock = NULL;

    first_free_block = (heap5_block_link_t *)address;
    first_free_block->xBlockSize = (size_t)(aligned_start - address);
    first_free_block->pxNextFreeBlock = end_marker;
    if (first_free_block->xBlockSize < heap5MINIMUM_BLOCK_SIZE) {
        return -1;
    }

    if (previous_end != NULL) {
        previous_end->pxNextFreeBlock = first_free_block;
    }

    heap->pxEnd = end_marker;
    heap->regions[heap->region_count].first_block = first_free_block;
    heap->regions[heap->region_count].end_block = end_marker;
    heap->region_count++;

    heap->xFreeBytesRemaining += first_free_block->xBlockSize;
    heap->xTotalHeapSize += first_free_block->xBlockSize;
    if (heap->region_count == 1U) {
        heap->xMinimumEverFreeBytesRemaining = heap->xFreeBytesRemaining;
    }

    return 0;
}

void *heap5_malloc(heap5_t heap, size_t wanted_size)
{
    heap5_block_link_t *block;
    heap5_block_link_t *previous_block;
    heap5_block_link_t *new_block;
    void *ret = NULL;
    size_t additional_size;

    if (!heap || heap->pxEnd == NULL) {
        return NULL;
    }

    if (wanted_size > 0U) {
        if (heap5_add_will_overflow(wanted_size, xHeapStructSize)) {
            return NULL;
        }

        wanted_size += xHeapStructSize;
        if ((wanted_size & heap5BYTE_ALIGNMENT_MASK) != 0U) {
            additional_size = heap5BYTE_ALIGNMENT - (wanted_size & heap5BYTE_ALIGNMENT_MASK);
            if (heap5_add_will_overflow(wanted_size, additional_size)) {
                return NULL;
            }
            wanted_size += additional_size;
        }
    }

    if (!heap5SIZE_IS_VALID(wanted_size) ||
        wanted_size == 0U ||
        wanted_size > heap->xFreeBytesRemaining) {
        return NULL;
    }

    previous_block = &heap->xStart;
    block = heap->xStart.pxNextFreeBlock;
    while ((heap5BLOCK_SIZE(block) < wanted_size) && (block->pxNextFreeBlock != NULL)) {
        previous_block = block;
        block = block->pxNextFreeBlock;
    }

    if (block == heap->pxEnd) {
        return NULL;
    }

    ret = (void *)(((uint8_t *)block) + xHeapStructSize);
    previous_block->pxNextFreeBlock = block->pxNextFreeBlock;

    if ((heap5BLOCK_SIZE(block) - wanted_size) > heap5MINIMUM_BLOCK_SIZE) {
        new_block = (heap5_block_link_t *)(((uint8_t *)block) + wanted_size);
        new_block->xBlockSize = heap5BLOCK_SIZE(block) - wanted_size;
        new_block->pxNextFreeBlock = NULL;
        block->xBlockSize = wanted_size;
        heap5_insert_block_into_free_list(heap, new_block);
    }

    heap->xFreeBytesRemaining -= heap5BLOCK_SIZE(block);
    if (heap->xFreeBytesRemaining < heap->xMinimumEverFreeBytesRemaining) {
        heap->xMinimumEverFreeBytesRemaining = heap->xFreeBytesRemaining;
    }

    heap5ALLOCATE_BLOCK(block);
    block->pxNextFreeBlock = NULL;
    heap->xNumberOfSuccessfulAllocations++;

    return ret;
}

void heap5_free(heap5_t heap, void *ptr)
{
    uint8_t *puc;
    heap5_block_link_t *link;

    if (!heap || !ptr) {
        return;
    }

    puc = (uint8_t *)ptr;
    puc -= xHeapStructSize;
    link = (heap5_block_link_t *)puc;

    if (!heap5BLOCK_IS_ALLOCATED(link) || link->pxNextFreeBlock != NULL) {
        return;
    }

    heap5FREE_BLOCK(link);
    heap->xFreeBytesRemaining += heap5BLOCK_SIZE(link);
    heap5_insert_block_into_free_list(heap, link);
    heap->xNumberOfSuccessfulFrees++;
}

size_t heap5_get_free_size(heap5_t heap)
{
    return heap ? heap->xFreeBytesRemaining : 0U;
}

size_t heap5_get_total_size(heap5_t heap)
{
    return heap ? heap->xTotalHeapSize : 0U;
}

size_t heap5_get_min_free_size(heap5_t heap)
{
    return heap ? heap->xMinimumEverFreeBytesRemaining : 0U;
}

void heap5_get_stats(heap5_t heap, struct heap5_stats *stats)
{
    if (!heap || !stats) {
        return;
    }

    memset(stats, 0, sizeof(*stats));
    stats->smallest_free_block = heap5SIZE_MAX;
    stats->total_size = heap->xTotalHeapSize;
    stats->free_size = heap->xFreeBytesRemaining;
    stats->min_free_size = heap->xMinimumEverFreeBytesRemaining;
    stats->alloc_count = heap->xNumberOfSuccessfulAllocations;
    stats->free_count = heap->xNumberOfSuccessfulFrees;

    for (uint32_t i = 0; i < heap->region_count; i++) {
        heap5_block_link_t *block = heap->regions[i].first_block;
        heap5_block_link_t *end = heap->regions[i].end_block;

        while (block < end) {
            size_t block_size = heap5BLOCK_SIZE(block);

            if (block_size == 0U) {
                break;
            }

            if (heap5BLOCK_IS_ALLOCATED(block)) {
                stats->alloc_block_count++;
            } else {
                stats->free_block_count++;
                if (block_size > stats->largest_free_block) {
                    stats->largest_free_block = block_size;
                }
                if (block_size < stats->smallest_free_block) {
                    stats->smallest_free_block = block_size;
                }
            }

            block = (heap5_block_link_t *)(((uint8_t *)block) + block_size);
        }
    }

    if (stats->smallest_free_block == heap5SIZE_MAX) {
        stats->smallest_free_block = 0U;
    }
}
