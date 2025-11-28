/**
 * @file mm_config.h
 * @brief Configuration file for the Bouffalo SDK memory management framework
 * @version 1.0
 * @date 2025
 *
 * This file defines the configuration options and compile-time parameters
 * for the memory management framework.
 *
 * Copyright (c) 2025 Bouffalo Lab
 */

#ifndef __MM_CONFIG_H
#define __MM_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/** @brief Basic configuration parameters */
/* ======================================================================== */

/**
 * @brief Number of supported memory allocators
 *
 * Defines the maximum number of allocators supported by the system,
 * including TLSF, FreeRTOS heap5, etc.
 */
#ifndef CONFIG_MM_ALLOCATOR_COUNT
#define CONFIG_MM_ALLOCATOR_COUNT 4
#endif

/**
 * @brief Number of supported memory heaps
 *
 * Defines the maximum number of memory heaps supported by the system,
 * corresponding to different physical memory regions.
 */
#ifndef CONFIG_MM_HEAP_COUNT
#define CONFIG_MM_HEAP_COUNT 8
#endif

/**
 * @brief Maximum length of allocator names
 *
 * Defines the maximum length of allocator name strings (including null terminator).
 */
#ifndef CONFIG_MM_ALLOCATOR_NAME_MAX
#define CONFIG_MM_ALLOCATOR_NAME_MAX 16
#endif

/**
 * @brief Maximum length of heap names
 *
 * Defines the maximum length of heap name strings (including null terminator).
 */
#ifndef CONFIG_MM_HEAP_NAME_MAX
#define CONFIG_MM_HEAP_NAME_MAX 16
#endif

/* ======================================================================== */
/** @brief Feature switches */
/* ======================================================================== */

/**
 * @brief Enable statistics
 *
 * Enables collection of memory allocation/deallocation statistics,
 * increasing a small amount of runtime overhead.
 * If statistics are not needed, disable to reduce overhead.
 */
#ifndef CONFIG_MM_ENABLE_STATISTICS
#define CONFIG_MM_ENABLE_STATISTICS 1
#endif

/**
 * @brief Enable memory leak detection
 *
 * Enables tracking of memory allocations and deallocations,
 * detecting potential memory leaks.
 */
#ifndef CONFIG_MM_ENABLE_LEAK_DETECTION
#define CONFIG_MM_ENABLE_LEAK_DETECTION 0
#endif

/* ======================================================================== */
/** @brief Allocator selection configuration */
/* ======================================================================== */

/**
 * @brief Enable TLSF allocator
 *
 * TLSF (Two-Level Segregate Fit) efficient memory allocator.
 */
#ifndef CONFIG_MM_TLSF_EN
#define CONFIG_MM_TLSF_EN 1
#endif

/**
 * @brief Enable FreeRTOS heap5 allocator
 *
 * Compatible with FreeRTOS heap5 memory allocator.
 */
#ifndef CONFIG_MM_ENABLE_FREERTOS_HEAP5
#define CONFIG_MM_ENABLE_FREERTOS_HEAP5 0
#endif

/* ======================================================================== */
/** @brief Internal macros */
/* ======================================================================== */

/** @brief Memory alignment helper macros */
#ifndef ALIGN_UP
#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MM_CONFIG_H */