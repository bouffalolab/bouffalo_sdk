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
 *
 * @return 0 on success. On failure returns a non-zero bitmask (see
 *         PM_COEX_SLEEP_FAIL_*).
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
 * @brief Coex coordinator "gate" (master enable).
 *
 * Persistent enable gate used by coex coordinator to decide whether coex-related
 * behaviors may be attached (regardless of runtime pause state).
 *
 * @return true if coexistence mode is enabled, false otherwise.
 */
bool coex_coord_is_enabled(void);

bool coex_coord_is_active(void);

/**
 * @brief Backward-compatible alias of @ref coex_coord_is_enabled.
 *
 * @deprecated Prefer @ref coex_coord_is_enabled for coordinator semantics.
 */
bool ps_is_coex_mode(void);

/*
 * COEX coordinator - Stage 1 entrypoints
 ****************************************************************************************
 */

/// TXQ pending query callback (implemented by fhost, registered at runtime)
typedef bool (*coex_txq_has_pending_cb_t)(uint8_t vif_idx);

/**
 * @brief Register a callback for querying whether Host TXQ has pending packets.
 *
 * This avoids tight linkage between macsw (coex coordinator) and fhost.
 * The callback SHALL be fast and non-blocking.
 */
void coex_coord_register_txq_has_pending_cb(coex_txq_has_pending_cb_t cb);

/**
 * @brief TBTT hook (Wi-Fi RX anchor) for coex coordinator.
 * @param tbtt_time   TBTT timestamp (us, MAC timer).
 * @param vif_index   MAC VIF index associated with this TBTT.
 */
void coex_coord_on_tbtt(uint32_t tbtt_time, uint8_t vif_index);

/**
 * @brief Called by platform right before Wi-Fi task blocks.
 *
 * @return true if `pm_coex_sleep()` commit succeeded and caller MUST pair with
 *         `coex_coord_on_wifi_wake(true)`.
 */
bool coex_coord_on_wifi_suspend_enter(void);

/**
 * @brief Called by platform right after Wi-Fi task is woken up.
 * @param slept_committed  Return value from @ref coex_coord_on_wifi_suspend_enter.
 */
void coex_coord_on_wifi_wake(bool slept_committed);

/**
 * @brief Coex coordinator runtime hooks (called by PS layer).
 *
 * These hooks reflect the existing coex runtime gating semantics:
 * - enable/disable: persistent master switch
 * - runtime pause/resume: disconnect/reconnect gating while preserving enable flag
 */
void coex_coord_on_enable(void);
void coex_coord_on_disable(void);
void coex_coord_on_runtime_pause(void);
void coex_coord_on_runtime_resume(void);

/*
 * pm_coex_sleep failure bitmask (Stage 1)
 ****************************************************************************************
 */

/// pm_coex_sleep(): guard / not ready (not init, runtime paused, wrong state)
#define PM_COEX_SLEEP_FAIL_GUARD         (1u << 0)
/// PS is OFF
#define PM_COEX_SLEEP_FAIL_PS_OFF        (1u << 1)
/// prevent_sleep is active (global or per-VIF)
#define PM_COEX_SLEEP_FAIL_PREVENT_SLEEP (1u << 2)
/// TX path is not sleepable (TX inflight / pck_cnt etc)
#define PM_COEX_SLEEP_FAIL_TX_INFLIGHT   (1u << 3)
/// HW timer / MAC state blocks sleep
#define PM_COEX_SLEEP_FAIL_HW_TIMER      (1u << 4)
/// KE msg queue not empty / state not allowing sleep
#define PM_COEX_SLEEP_FAIL_KE_MSG        (1u << 5)
/// CPU / events block sleep
#define PM_COEX_SLEEP_FAIL_CPU           (1u << 6)
/// pm_coex_sleep_ctl(true) failed
#define PM_COEX_SLEEP_FAIL_SLEEP_CTL     (1u << 7)

/**
 * @brief Pause coexistence runtime behaviors
 * @details Pauses runtime behaviors (PTA switching, TBTT slice, host TX gating)
 *          while preserving the persistent "coex enabled" configuration.
 *
 * This is typically triggered when Wi-Fi PS is turned OFF (e.g. disconnect).
 * Does nothing if coex is disabled.
 *
 * @note Coex protect module enable follows the persistent coex feature enable
 *       and stays enabled while coex is enabled (regardless of runtime pause).
 */
void ps_coex_runtime_pause(void);

/**
 * @brief Resume coexistence runtime behaviors
 * @details Resumes runtime behaviors after Wi-Fi PS is turned ON.
 *          Does nothing if coex is disabled.
 * @note Wi-Fi PS re-enable must be handled separately by caller.
 * @note Coex protect module stays enabled while coex is enabled; protection
 *       ref-count state is preserved across runtime pause/resume.
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
    COEX_PROT_CONNECT,       ///< WiFi connect protection (AUTH/ASSOC)
    COEX_PROT_DHCP,          ///< DHCP protection
    COEX_PROT_DISCONNECT,    ///< Disconnect protection (deauth TX)
    COEX_PROT_KEY_MGMT,      ///< Key management protection (ASSOCIATED->AUTHORIZED)
    COEX_PROT_MAX
} coex_protect_type_t;

/// Timeout for 4-way handshake wait (milliseconds)
#define PM_COEX_HANDSHAKE_WAIT_TIMEOUT_MS  10000

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
 * @brief Get current protection bitmask (read-only)
 * @return Active protection bitmask (bit per type)
 */
uint32_t coex_protect_get_mask(void);

/**
 * @brief Dump coex protection state for debugging
 */
void coex_protect_dump(void);

/*
 * COEX STATUS QUERY API (Read-Only)
 ****************************************************************************************
 */

/**
 * @brief Coex status snapshot for read-only query
 *
 * This structure captures a point-in-time snapshot of coex state.
 * All fields are read-only and do not modify any internal state.
 */
struct pm_coex_status {
    /* PS layer coex gate */
    uint8_t ps_coex_state;          ///< ps_env.coex_state (PS_COEX_DISABLED/ENABLED/RUNNING)

    /* pm_coex state machine */
    uint8_t pm_state;               ///< g_pm_coex_ctx.state (PM_COEX_STATE_*)
    uint8_t pta_current_role;       ///< g_pm_coex_ctx.pta_current_role (PTA_ROLE_*)
    bool wifi_active_window;        ///< g_pm_coex_ctx.wifi_active_window
    bool wifi_connecting;           ///< g_pm_coex_ctx.wifi_connecting

    /* WiFi duty cycle config */
    uint32_t wifi_duty_ms;          ///< WiFi active time in ms (configured via wifi_sta_coex_duty_set)

    /* coex protect */
    bool protect_enabled;           ///< coex_protect module enabled
    bool protect_active;            ///< any protection currently active
    uint32_t protect_mask;          ///< active protection bitmask
};

/**
 * @brief Get coex status snapshot (read-only)
 *
 * @param[out] out  Pointer to status structure to fill
 * @return 0 on success, -1 if out is NULL or module not initialized
 *
 * @note This function is safe to call from any context.
 *       Uses GLOBAL_INT_DISABLE() internally for atomic read.
 * @note This function does NOT modify any internal state.
 */
int pm_coex_get_status(struct pm_coex_status *out);


#ifdef __cplusplus
}
#endif

/// @}

#endif /* __MACSW_COEXM_PUBLIC_H__ */
