/**
 ****************************************************************************************
 *
 * @file coexm.h
 *
 * @brief WiFi/BLE Coexistence Power Management module - Public API
 *
 * This module manages PTA (Packet Traffic Arbiter) switching for WiFi/BLE coexistence.
 * It coordinates RF access between WiFi and BLE through time-division multiplexing.
 *
 * This is the PUBLIC header for external modules. MACSW internal modules should
 * include both this file and coexm_int.h for internal APIs.
 *
 * Copyright (C) Bouffalo Lab 2024
 *
 ****************************************************************************************
 */

#ifndef __MACSW_COEXM_PUBLIC_H__
#define __MACSW_COEXM_PUBLIC_H__

/**
 ****************************************************************************************
 * @defgroup COEXM WiFi/BLE Coexistence Manager
 * @ingroup MACSW
 * @brief WiFi/BLE Coexistence power management module
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * DEFINITIONS
 ****************************************************************************************
 */

/**
 * @brief PTA (Packet Traffic Arbiter) role definitions
 *
 * Defines the different operating modes for the PTA hardware which controls
 * RF access arbitration between WiFi, BLE, Thread, and other protocols.
 */
enum pta_role {
    PTA_ROLE_BT = 0,                    ///< BLE exclusive mode (BLE has priority)
    PTA_ROLE_WIFI,                      ///< WiFi exclusive mode (WiFi has priority)
    PTA_ROLE_WIFI_AND_BT_DEFAULT,       ///< Compete mode (WiFi and BLE use default priority)
    PTA_ROLE_THREAD,                    ///< Thread exclusive mode
    PTA_ROLE_PTI,                       ///< PTI (Packet Traffic Information) mode
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/**
 * @brief Initialization parameters for PM COEX module.
 */
typedef struct {
    bool pta_force_enable;              ///< Force enable PTA on init
} pm_coex_init_params_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 * @brief Initialize the PM COEX module.
 * @param params Initialization parameters (must not be NULL).
 * @return 0 on success, -1 on failure.
 */
int pm_coex_init(const pm_coex_init_params_t *params);

/**
 * @brief De-initialize the PM COEX module.
 * @note Restores the PTA state to its original configuration.
 * @return 0 on success, -1 on failure.
 */
int pm_coex_deinit(void);

/**
 * @brief Enter sleep mode.
 * @note Performs a pre-check to ensure the system can sleep safely.
 * @return 0 on success, -1 on failure.
 */
int pm_coex_sleep(void);

/**
 * @brief Wake the system from sleep.
 * @return 0 on success, -1 on failure.
 */
int pm_coex_wakeup(void);

/**
 * @brief Pause the power management functionality.
 * @details When paused, pm_coex_sleep and pm_coex_wakeup calls will be ignored.
 * @return 0 on success, -1 if PM not initialized.
 */
int pm_coex_pause(void);

/**
 * @brief Resume the power management functionality.
 * @details Allows pm_coex_sleep and pm_coex_wakeup calls to function normally.
 * @return 0 on success, -1 if PM not initialized.
 */
int pm_coex_resume(void);

/**
 * @brief Check if currently in WiFi/BLE coexistence mode
 * @return true if coexistence mode is enabled, false otherwise
 */
bool ps_is_coex_mode(void);

/**
 * @brief Pause coexistence runtime behaviors on Wi-Fi disconnect
 * @details Pauses runtime behaviors (PTA switching, flow control, protect)
 *          while preserving the persistent coex_mode_enabled flag.
 *          Does nothing if coex mode is not enabled.
 */
void ps_coex_runtime_pause(void);

/**
 * @brief Resume coexistence runtime behaviors on Wi-Fi reconnect (Got IP)
 * @details Resumes runtime behaviors that were paused on disconnect.
 *          Does nothing if coex mode is not enabled.
 * @note Wi-Fi PS re-enable must be handled separately by caller.
 */
void ps_coex_runtime_resume(void);

/**
 * @brief Check if currently in WiFi active window (WiFi's time slice).
 * @details Returns true if WiFi is in its allocated time slice and can use PTA_ROLE_WIFI.
 *          Returns false if outside WiFi's time slice (e.g., BLE's time slice), where WiFi
 *          should compete using PTA_ROLE_WIFI_AND_BT_DEFAULT instead.
 * @return true if in WiFi active window, false otherwise.
 */
bool pm_coex_is_wifi_active_window(void);

/*
 * COEX PROTECTION API
 ****************************************************************************************
 */

/**
 * @brief Coex protection types for RF access control
 *
 * Different WiFi operations that require exclusive RF access during coexistence.
 */
typedef enum {
    COEX_PROT_SCAN = 0,      ///< WiFi scan protection
    COEX_PROT_CONNECT,       ///< WiFi connect/handshake protection
    COEX_PROT_DHCP,          ///< DHCP protection
    COEX_PROT_DISCONNECT,    ///< Disconnect protection (deauth TX)
    COEX_PROT_MAX
} coex_protect_type_t;

/// Timeout for 4-way handshake wait (milliseconds)
#define PM_COEX_HANDSHAKE_WAIT_TIMEOUT_MS  5000

/**
 * @brief Enable/disable coex protection module
 * @param enable true to enable, false to disable
 */
void coex_protect_set_enabled(bool enable);

/**
 * @brief Check if coex protection module is enabled
 * @return true if enabled, false otherwise
 */
bool coex_protect_is_enabled(void);

/**
 * @brief Acquire RF protection for a specific operation type
 * @param type Protection type (COEX_PROT_SCAN, COEX_PROT_CONNECT, etc.)
 * @return 0 on success, -1 on error
 */
int coex_protect_acquire(coex_protect_type_t type);

/**
 * @brief Release RF protection for a specific operation type
 * @param type Protection type to release
 * @return 0 on success, -1 on error
 */
int coex_protect_release(coex_protect_type_t type);

/**
 * @brief Release all RF protections (emergency reset)
 */
void coex_protect_release_all(void);

/**
 * @brief Check if any RF protection is currently active
 * @return true if protection is active, false otherwise
 */
bool coex_protect_is_active(void);

/**
 * @brief Dump coex protection state for debugging
 */
void coex_protect_dump(void);


#ifdef __cplusplus
}
#endif

/// @}

#endif /* __MACSW_COEXM_PUBLIC_H__ */
