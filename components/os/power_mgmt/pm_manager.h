#ifndef PM_MANAGER_H
#define PM_MANAGER_H

#include <stdint.h>

/**
 * @brief Maximum number of sleep check callbacks that can be registered
 */
#ifndef PM_SLEEP_CHECK_MAX_CALLBACKS
#define PM_SLEEP_CHECK_MAX_CALLBACKS 8
#endif

extern int enable_multicast_broadcast;

/**
 * @brief Sleep check callback function type
 * @return 0 if sleep is allowed, non-zero to prevent sleep
 */
typedef int (*pm_sleep_check_cb_t)(void);

/**
 * @brief Sleep check callback entry structure
 */
typedef struct {
    const char *name;           /**< Callback name for identification and logging */
    pm_sleep_check_cb_t cb;     /**< Callback function pointer */
    uint8_t priority;           /**< Priority (lower value = higher priority, executed first) */
    uint8_t enabled;            /**< Whether this entry is enabled/registered */
} pm_sleep_check_entry_t;

int pm_sys_init(void);
int pm_mem_pool_alloc(void);
int pm_mem_pool_free(void);
uint32_t pm_manager_handle_get(void);
int pm_pbufc_check(void);
int pm_enter_lp_perparation(void);
int pm_exit_lp_perparation(void);
int pm_alloc_mem_reset(void);
void set_dtim_config(int dtim);
int pm_enable_tickless(void);
int pm_disable_tickless(void);

/**
 * @brief Register a sleep check callback
 * 
 * @param name Callback name for identification and logging (must remain valid)
 * @param cb Callback function pointer
 * @param priority Priority value (lower = higher priority, executed first)
 * @return 0 on success, -1 if registry is full, -2 if name already exists
 */
int pm_sleep_check_register(const char *name, pm_sleep_check_cb_t cb, uint8_t priority);

/**
 * @brief Unregister a sleep check callback by name
 * 
 * @param name Callback name to unregister
 * @return 0 on success, -1 if not found
 */
int pm_sleep_check_unregister(const char *name);

/**
 * @brief Dispatch all registered sleep check callbacks
 * 
 * Executes all registered callbacks in priority order (lower priority value first).
 * Logs the name and return code of any callback that returns non-zero.
 * 
 * @return 0 if all callbacks allow sleep, non-zero from first callback that prevents sleep
 */
int pm_sleep_check_dispatch(void);

#endif // PM_MANAGER_H
