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

int platform_get_mac(enum wl80211_vif_type vif, uint8_t mac[6]);
int platform_feature_index(void);
#define WL80211_FEATURE(feat) (wl80211_platform_feature[platform_feature_index()].feat)

#endif /* _WL80211_PLATFORM_H_ */
