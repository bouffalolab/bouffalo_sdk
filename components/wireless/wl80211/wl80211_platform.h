/**
 ****************************************************************************************
 *
 * @file wl80211_platform.h
 *
 * @brief Platform abstraction layer header for wl80211 WiFi driver.
 *        Provides platform-independent RTOS primitives and synchronization mechanisms.
 *
 ****************************************************************************************
 */

#ifndef _WL80211_PLATFORM_H_
#define _WL80211_PLATFORM_H_

#include <stdbool.h>
#include <stdint.h>
#include "wl80211.h"

#define WG_FLAG_UPDATE(f, v) \
    rtos_lock();             \
    wl80211_glb.f = v;       \
    rtos_unlock();

extern const struct platform_feature {
    uint32_t he_mcs  : 2;
    uint32_t band_5g : 1;
    uint32_t ht40    : 1;
} wl80211_platform_feature[];

/**
 * @brief RTOS semaphore handle type
 */
typedef void *rtos_semaphore;

/**
 * @brief RTOS mutex handle type
 */
typedef void *rtos_mutex;

/*
 ****************************************************************************************
 * TIME AND TIMING FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Convert milliseconds to RTOS ticks
 *
 * @param[in] ms Time in milliseconds to convert
 * @return Number of equivalent RTOS ticks
 */
uint32_t rtos_ms2tick(int ms);

/**
 * @brief Get current system time in milliseconds
 *
 * @param[in] isr Whether called from interrupt context
 * @return Current time in milliseconds
 */
uint32_t rtos_now(bool isr);

/*
 ****************************************************************************************
 * CRITICAL SECTION FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Enter critical section to protect shared resources
 */
void rtos_lock(void);

/**
 * @brief Exit critical section after protecting shared resources
 */
void rtos_unlock(void);

/*
 ****************************************************************************************
 * SEMAPHORE FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Create a semaphore
 *
 * @param[out] semaphore Pointer to semaphore handle
 * @param[in] max_count Maximum count for counting semaphores
 * @param[in] init_count Initial count value
 * @return 0 on success, -1 on failure
 */
int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count, int init_count);

/**
 * @brief Delete a semaphore
 *
 * @param[in] semaphore Semaphore handle to delete
 */
void rtos_semaphore_delete(rtos_semaphore semaphore);

/**
 * @brief Wait for a semaphore with timeout
 *
 * @param[in] semaphore Semaphore handle
 * @param[in] timeout Timeout in milliseconds (-1 for infinite wait)
 * @return 0 on success, -1 on timeout
 */
int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout);

/**
 * @brief Signal/release a semaphore
 *
 * @param[in] semaphore Semaphore handle
 * @param[in] isr Whether called from interrupt context
 * @return 0 on success, -1 on failure
 */
int rtos_semaphore_signal(rtos_semaphore semaphore, bool isr);

/*
 ****************************************************************************************
 * MUTEX FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Create a mutex
 *
 * @param[out] mutex Pointer to mutex handle
 * @return 0 on success, -1 on failure
 */
int rtos_mutex_create(rtos_mutex *mutex);

/**
 * @brief Delete a mutex
 *
 * @param[in] mutex Mutex handle to delete
 */
void rtos_mutex_delete(rtos_mutex mutex);

/**
 * @brief Lock a mutex (blocking)
 *
 * @param[in] mutex Mutex handle to lock
 */
void rtos_mutex_lock(rtos_mutex mutex);

/**
 * @brief Unlock a mutex
 *
 * @param[in] mutex Mutex handle to unlock
 */
void rtos_mutex_unlock(rtos_mutex mutex);

/*
 ****************************************************************************************
 * TIMER MANAGEMENT TYPES
 ****************************************************************************************
 */

/**
 * @brief RTOS timer handle type
 */
typedef void *rtos_timer_handle;

/**
 * @brief RTOS timer callback function type
 */
typedef void (*rtos_timer_cb)(rtos_timer_handle timer, void *arg);

/*
 ****************************************************************************************
 * TIMEOUT MANAGEMENT FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Initialize the timeout system
 */
void rtos_timeouts_init(void);

/**
 * @brief Start timeout processing with specified delay
 *
 * @param[in] delay Initial delay in milliseconds
 */
void rtos_timeouts_start(unsigned int delay);

/**
 ****************************************************************************************
 * @brief Get MAC Address
 *
 * @param[out] mac Buffer of hold mac address.
 * @return 0 on success and OTHERS if error occurred.
 ****************************************************************************************
 */

int platform_get_mac(uint8_t vif_type, uint8_t mac[6]); // ref @ enum wl80211_vif_type
int platform_feature_index(void);
#define WL80211_FEATURE(feat) (wl80211_platform_feature[platform_feature_index()].feat)

/*
 ****************************************************************************************
 * WRAM MEMORY ALLOCATION FUNCTIONS
 ****************************************************************************************
 */

/* The total memory size limit used by WL80211 */
#ifndef CONFIG_WL80211_WRAM_MEM_SIZE_LIMIT
#define CONFIG_WL80211_WRAM_MEM_SIZE_LIMIT (32 * 1024)
#endif
/* The total memory count limit used by WL80211 */
#ifndef CONFIG_WL80211_WRAM_MEM_CNT_LIMIT
#define CONFIG_WL80211_WRAM_MEM_CNT_LIMIT (0)
#endif

/**
 * Allocate memory in WRAM region (WiFi RAM)
 *
 * This function allocates memory from the WiFi RAM (WRAM) region.
 * It also tracks the total allocated memory size and count, and enforces limits if configured.
 *
 * @param[in] size Size of memory to allocate in bytes
 * @return Pointer to allocated memory, or NULL on failure
 *
 * @note Memory allocated by this function MUST be freed using wl80211_platform_free_wram()
 */
void *wl80211_platform_malloc_wram(size_t size);

/**
 * @brief Free memory allocated from WRAM region
 *
 * Frees memory that was allocated by wl80211_platform_malloc_wram().
 *
 * @param[in] ptr Pointer to memory to free
 */
void wl80211_platform_free_wram(void *ptr);

#if CONFIG_WL80211_WRAM_MEM_SIZE_LIMIT
/** Get current WRAM memory size usage
 *
 * @return Current allocated memory size in bytes
 */
size_t wl80211_platform_get_wram_mem_size_usage(void);
#endif

#if CONFIG_WL80211_WRAM_MEM_CNT_LIMIT
/** Get current WRAM memory count usage
 *
 * @return Current allocated memory count
 */
size_t wl80211_platform_get_wram_mem_cnt_usage(void);
#endif

/** * @brief Allocate memory in WRAM region without limit
 *
 * This function allocates memory from the WiFi RAM (WRAM) region without enforcing any limits.
 * It is intended for use cases where the caller needs to bypass the standard memory usage tracking.
 * For example, when sending the Wi-Fi management frames
 *
 * @param[in] size Size of memory to allocate in bytes
 * @return Pointer to allocated memory, or NULL on failure
 *
 * @note Memory allocated by this function MUST be freed using wl80211_platform_free_wram_nolimit()
 */
void *wl80211_platform_malloc_wram_nolimit(size_t size);

/** * @brief Free memory allocated from WRAM region without limit
 *
 * Frees memory that was allocated by wl80211_platform_malloc_wram_nolimit().
 *
 * @param[in] ptr Pointer to memory to free
 */
void wl80211_platform_free_wram_nolimit(void *ptr);

#endif /* _WL80211_PLATFORM_H_ */
