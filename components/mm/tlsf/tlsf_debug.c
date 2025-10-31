/****************************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "tlsf.h"
#include "mem.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define MAX_FRAGMENTATION_BINS 32

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef struct {
    size_t size_range_min;
    size_t size_range_max;
    uint32_t count;
    size_t total_size;
} fragmentation_bin_t;

typedef struct {
    fragmentation_bin_t bins[MAX_FRAGMENTATION_BINS];
    uint32_t bin_count;
    size_t total_free_size;
    uint32_t total_free_blocks;
    uint32_t total_used_blocks;
    size_t largest_free_block;
    size_t smallest_free_block;
    float fragmentation_index;  /* 0.0 = no fragmentation, 1.0 = maximum fragmentation */
    float utilization_efficiency; /* How efficiently the free space can be used */
} fragmentation_analysis_t;

typedef struct {
    size_t total_used_size;
    size_t total_free_size;
    size_t largest_free_block;
    size_t smallest_free_block;
    uint32_t free_block_count;
    uint32_t used_block_count;
    uint32_t total_block_count;
} memory_stats_t;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* Walker function to collect statistics and print only used blocks */
static void used_blocks_walker(void *ptr, size_t size, int used, void *user)
{
    memory_stats_t *stats = (memory_stats_t *)user;
    
    /* Update statistics for all blocks */
    stats->total_block_count++;
    
    if (used) {
        /* Print only used block information */
        printf("%-12p %-8s %-12zu %-6u %-6u %-6u\n",
               ptr,
               "USED",
               size,
               0,  /* FL index not relevant for used blocks */
               0,  /* SL index not relevant for used blocks */
               stats->used_block_count);
        
        stats->total_used_size += size;
        stats->used_block_count++;
    } else {
        /* Update free block statistics but don't print */
        stats->total_free_size += size;
        stats->free_block_count++;
        
        if (stats->largest_free_block == 0 || size > stats->largest_free_block) {
            stats->largest_free_block = size;
        }
        if (stats->smallest_free_block == 0 || size < stats->smallest_free_block) {
            stats->smallest_free_block = size;
        }
    }
}

/* Walker function to print only free blocks directly */
static void free_blocks_walker(void *ptr, size_t size, int used, void *user)
{
    if (!used) {
        /* Calculate FL and SL indices */
        uint32_t fl_index = 0, sl_index = 0;
        if (size >= 128) {
            fl_index = 31 - __builtin_clz(size);
            sl_index = (size >> (fl_index - 5)) ^ (1 << 5);
            fl_index -= 6;
        } else {
            fl_index = 0;
            sl_index = size / 4;
        }
        
        /* Print free block information directly */
        printf("%-12p %-12zu %-6u %-6u\n",
               ptr,
               size,
               fl_index,
               sl_index);
    }
}

/* Walker function for fragmentation analysis */
static void fragmentation_walker(void *ptr, size_t size, int used, void *user)
{
    fragmentation_analysis_t *analysis = (fragmentation_analysis_t *)user;
    
    if (!used) { /* Only analyze free blocks */
        analysis->total_free_size += size;
        analysis->total_free_blocks++;
        
        if (analysis->largest_free_block == 0 || size > analysis->largest_free_block) {
            analysis->largest_free_block = size;
        }
        if (analysis->smallest_free_block == 0 || size < analysis->smallest_free_block) {
            analysis->smallest_free_block = size;
        }
        
        /* Find appropriate bin for this block size */
        int bin_index = -1;
        for (uint32_t i = 0; i < analysis->bin_count; i++) {
            if (size >= analysis->bins[i].size_range_min && 
                size <= analysis->bins[i].size_range_max) {
                bin_index = i;
                break;
            }
        }
        
        /* If no bin found, create a new one */
        if (bin_index == -1 && analysis->bin_count < MAX_FRAGMENTATION_BINS) {
            bin_index = analysis->bin_count++;
            analysis->bins[bin_index].size_range_min = size;
            analysis->bins[bin_index].size_range_max = size;
            analysis->bins[bin_index].count = 0;
            analysis->bins[bin_index].total_size = 0;
        }
        
        if (bin_index >= 0) {
            analysis->bins[bin_index].count++;
            analysis->bins[bin_index].total_size += size;
            
            /* Expand range if necessary */
            if (size < analysis->bins[bin_index].size_range_min) {
                analysis->bins[bin_index].size_range_min = size;
            }
            if (size > analysis->bins[bin_index].size_range_max) {
                analysis->bins[bin_index].size_range_max = size;
            }
        }
    } else {
        analysis->total_used_blocks++;
    }
}

/* Calculate fragmentation metrics */
static void calculate_fragmentation_metrics(fragmentation_analysis_t *analysis)
{
    if (analysis->total_free_blocks == 0 || analysis->total_free_size == 0) {
        analysis->fragmentation_index = 0.0f;
        analysis->utilization_efficiency = 0.0f;
        return;
    }
    
    /* Calculate fragmentation index using standard deviation approach */
    float mean_size = (float)analysis->total_free_size / analysis->total_free_blocks;
    float variance = 0.0f;
    
    for (uint32_t i = 0; i < analysis->bin_count; i++) {
        float bin_mean = (float)analysis->bins[i].total_size / analysis->bins[i].count;
        float diff = bin_mean - mean_size;
        variance += diff * diff * analysis->bins[i].count;
    }
    
    variance /= analysis->total_free_blocks;
    float std_dev = sqrtf(variance);
    
    /* Normalize fragmentation index (0.0 to 1.0) */
    analysis->fragmentation_index = std_dev / mean_size;
    if (analysis->fragmentation_index > 1.0f) {
        analysis->fragmentation_index = 1.0f;
    }
    
    /* Calculate utilization efficiency */
    /* This measures how much of the free space can be effectively used */
    size_t effective_size = 0;
    for (uint32_t i = 0; i < analysis->bin_count; i++) {
        /* Assume blocks larger than 64 bytes are more useful */
        if (analysis->bins[i].size_range_min >= 64) {
            effective_size += analysis->bins[i].total_size;
        }
    }
    
    analysis->utilization_efficiency = (float)effective_size / analysis->total_free_size;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**
 * @brief Dump only used memory blocks with detailed information
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_dump_used_blocks(struct mem_heap_s *heap)
{
    if (!heap || !heap->priv) {
        printf("Error: Invalid heap pointer\n");
        return -1;
    }
    
    memory_stats_t stats = {0};
    pool_t pool = tlsf_get_pool(heap->priv);
    
    if (!pool) {
        printf("Error: Failed to get pool from TLSF\n");
        return -1;
    }
   
    /* Print header for used block information */
    printf("=== Used Memory Blocks ===\n");
    printf("%-12s %-8s %-12s %-6s %-6s %-6s\n", 
           "Address", "Status", "Size(bytes)", "FL", "SL", "Index");
    printf("--------------------------------------------------------\n");
    
    /* Walk through all blocks and print only used blocks */
    tlsf_walk_pool(pool, used_blocks_walker, &stats);
    
    return 0;
}

/**
 * @brief Dump only free memory blocks with TLSF-specific information
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_dump_free_blocks(struct mem_heap_s *heap)
{
    if (!heap || !heap->priv) {
        printf("Error: Invalid heap pointer\n");
        return -1;
    }
    
    pool_t pool = tlsf_get_pool(heap->priv);
    
    if (!pool) {
        printf("Error: Failed to get pool from TLSF\n");
        return -1;
    }
    
    printf("\n=== TLSF Free Blocks Dump ===\n");
    printf("================================\n");
    printf("%-12s %-12s %-6s %-6s\n", 
           "Address", "Size(bytes)", "FL", "SL");
    printf("----------------------------------------\n");
    
    /* Walk through all blocks and print free block information directly */
    tlsf_walk_pool(pool, free_blocks_walker, NULL);
    printf("\n");
    
    return 0;
}

/**
 * @brief Dump TLSF control structure information
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_dump_control_info(struct mem_heap_s *heap)
{
    if (!heap || !heap->priv) {
        printf("Error: Invalid heap pointer\n");
        return -1;
    }
    
    /* Note: This function requires access to internal TLSF structures
     * which are not exposed in the public API. For a complete implementation,
     * we would need to modify the TLSF header to expose the control_t structure.
     */
    
    printf("\n=== TLSF Control Structure Info ===\n");
    printf("Note: Control structure details require internal TLSF access\n");
    printf("TLSF Size: %zu bytes\n", tlsf_size());
    printf("TLSF Align Size: %zu bytes\n", tlsf_align_size());
    printf("TLSF Block Size Min: %zu bytes\n", tlsf_block_size_min());
    printf("TLSF Block Size Max: %zu bytes\n", tlsf_block_size_max());
    printf("TLSF Pool Overhead: %zu bytes\n", tlsf_pool_overhead());
    printf("TLSF Alloc Overhead: %zu bytes\n", tlsf_alloc_overhead());
    printf("====================================\n\n");
    
    return 0;
}

/**
 * @brief Perform TLSF integrity check and dump results
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_check_and_dump(struct mem_heap_s *heap)
{
    if (!heap || !heap->priv) {
        printf("Error: Invalid heap pointer\n");
        return -1;
    }
    
    printf("\n=== TLSF Integrity Check ===\n");
    
    int tlsf_status = tlsf_check(heap->priv);
    if (tlsf_status == 0) {
        printf("TLSF integrity check: PASSED\n");
    } else {
        printf("TLSF integrity check: FAILED (status: %d)\n", tlsf_status);
    }
    
    pool_t pool = tlsf_get_pool(heap->priv);
    if (pool) {
        int pool_status = tlsf_check_pool(pool);
        if (pool_status == 0) {
            printf("Pool integrity check: PASSED\n");
        } else {
            printf("Pool integrity check: FAILED (status: %d)\n", pool_status);
        }
    }
    
    return tlsf_status;
}

/**
 * @brief Perform detailed fragmentation analysis
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_analyze_fragmentation(struct mem_heap_s *heap)
{
    if (!heap || !heap->priv) {
        printf("Error: Invalid heap pointer\n");
        return -1;
    }
    
    fragmentation_analysis_t analysis = {0};
    pool_t pool = tlsf_get_pool(heap->priv);
    
    if (!pool) {
        printf("Error: Failed to get pool from TLSF\n");
        return -1;
    }
    
    printf("Heap Base: %p\n", heap->heapstart);
    printf("Heap Size: %zu bytes\n", heap->heapsize);
    
    /* Walk through all blocks and analyze fragmentation */
    tlsf_walk_pool(pool, fragmentation_walker, &analysis);
    
    /* Calculate fragmentation metrics */
    calculate_fragmentation_metrics(&analysis);
    
    /* Print fragmentation summary */
    printf("Total Free Size: %zu bytes\n", analysis.total_free_size);
    //printf("Minimum Free Size: %zu bytes\n", heap->min_free_bytes);
    printf("Total Free Blocks: %u\n", analysis.total_free_blocks);
    printf("Total Used Blocks: %u\n", analysis.total_used_blocks);
    printf("Largest Free Block: %zu bytes\n", analysis.largest_free_block);
    printf("Smallest Free Block: %zu bytes\n", analysis.smallest_free_block);
    printf("Average Free Block Size: %.2f bytes\n", 
           (float)analysis.total_free_size / analysis.total_free_blocks);
    printf("Fragmentation Index: %.3f (0.0=no frag, 1.0=max frag)\n", 
           analysis.fragmentation_index);
    printf("Utilization Efficiency: %.3f (0.0=low, 1.0=high)\n", 
           analysis.utilization_efficiency);
    
    ///* Print size distribution */
    //printf("=== Free Block Size Distribution ===\n");
    //printf("%-12s %-12s %-8s %-12s %-8s\n", 
    //       "Min Size", "Max Size", "Count", "Total Size", "Percentage");
    //printf("--------------------------------------------------------\n");
    //
    //for (uint32_t i = 0; i < analysis.bin_count; i++) {
    //    float percentage = (float)analysis.bins[i].total_size * 100.0f / analysis.total_free_size;
    //    printf("%-12zu %-12zu %-8u %-12zu %-8.2f%%\n",
    //           analysis.bins[i].size_range_min,
    //           analysis.bins[i].size_range_max,
    //           analysis.bins[i].count,
    //           analysis.bins[i].total_size,
    //           percentage);
    //}
   
    return 0;
}

/**
 * @brief Comprehensive memory analysis and dump
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_comprehensive_dump(struct mem_heap_s *heap)
{
    if (!heap || !heap->priv) {
        printf("Error: Invalid heap pointer\n");
        return -1;
    }
    
    printf("\n");
    printf("************************************************************\n");
    printf("                TLSF MEMORY ANALYSIS REPORT                \n");
    printf("************************************************************\n");
    
    /* Perform integrity check first */
    //tlsf_check_and_dump(heap);
    
    /* Dump control structure info */
    //tlsf_dump_control_info(heap);
    
    /* Dump all used blocks */
    tlsf_dump_used_blocks(heap);
    
    /* Dump free blocks specifically */
    tlsf_dump_free_blocks(heap);

    tlsf_analyze_fragmentation(heap);
    
    return 0;
}

