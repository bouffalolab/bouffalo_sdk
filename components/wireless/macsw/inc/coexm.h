/**
 ****************************************************************************************
 *
 * @file coexm.h
 *
 * @brief WiFi/BLE Coexistence Power Management module - Public API
 *
 * This module manages hardware PTA roles and optional PS-PTA time slicing for
 * WiFi/BLE coexistence.
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
#include "mac_types.h"

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

/** Result codes returned while applying a coexistence operation. */
enum coex_config_status {
    COEX_CONFIG_OK = 0,
    COEX_CONFIG_ERR_INVALID_PARAM = -1,
    COEX_CONFIG_ERR_UNSUPPORTED = -2,
    COEX_CONFIG_ERR_MODE_REQUIRED = -3,
    COEX_CONFIG_ERR_INVALID_COMBINATION = -4,
    COEX_CONFIG_ERR_HARDWARE_NOT_READY = -5,
    COEX_CONFIG_ERR_BUSY = -6,
    /** Backend changed hardware but could not complete or verify the write. */
    COEX_CONFIG_ERR_PARTIAL_WRITE = -7,
};

/** Coexistence capabilities implemented by the selected MACSW backend. */
enum coex_hw_capability {
    COEX_HW_CAP_HARDWARE_PTA       = (1u << 0),
    COEX_HW_CAP_PS_PTA             = (1u << 1),
    COEX_HW_CAP_WIFI_5G            = (1u << 2),
    COEX_HW_CAP_COMBO_PATH         = (1u << 3),
    COEX_HW_CAP_STANDALONE_PATH    = (1u << 4),
    COEX_HW_CAP_SPDT_CTRL          = (1u << 5),
    COEX_HW_CAP_CHANNEL_OVERLAY    = (1u << 6),
    COEX_HW_CAP_ADJUSTED_POWER     = (1u << 7),
    COEX_HW_CAP_HOST_TX_GATE       = (1u << 8),
};

/** Current BZ RF path reported by the selected MACSW backend. */
enum coex_rf_path {
    COEX_RF_PATH_COMBO = 0,
    COEX_RF_PATH_BZ_STANDALONE,
    COEX_RF_PATH_UNKNOWN,
};

/** Board RF/antenna topology resolved by WiFi6 before activation. */
enum coexm_hw_topology {
    COEXM_HW_TOPOLOGY_COMBO_SHARED_PATH = 0,
    COEXM_HW_TOPOLOGY_STANDALONE_DUAL_ANT,
    COEXM_HW_TOPOLOGY_STANDALONE_SINGLE_ANT_SPDT,
};

/** Immutable board/RF-path facts used by connection Protection. */
struct coexm_protect_profile {
    enum coexm_hw_topology topology;
    enum coex_rf_path rf_path;
};

/** Runtime policy carried by the firmware-internal hardware plan. */
enum coexm_hw_runtime {
    COEXM_HW_RUNTIME_HARDWARE_ONLY = 1,
    COEXM_HW_RUNTIME_PS_PTA_REQUIRED = 2,
};

/**
 * Complete coexistence hardware input resolved for one stable Wi-Fi channel.
 *
 * This is a WiFi6-to-MACSW firmware contract, not a customer-facing API.
 */
struct coexm_hw_plan {
    enum coexm_hw_topology topology;
    enum coex_rf_path rf_path;
    uint8_t band;
    uint8_t runtime;
    struct mac_chan_op channel;
};

enum coexm_spdt_mode {
    COEXM_SPDT_FIXED_BT = 0,
    COEXM_SPDT_DYNAMIC_PTA = 1,
};

/* Immutable startup operations bound to board wiring by the caller.
 * Callbacks receive only the mode and must not block or query WiFi6. */
struct coexm_board_ops {
    int (*spdt_apply)(uint8_t mode);
    int (*spdt_verify)(uint8_t mode);
};

int coexm_hw_board_register(const struct coexm_board_ops *ops);
/* Pure startup admission: no RF/MMIO access or board callback required. */
int coexm_hw_mode_validate(enum coexm_hw_topology topology, enum coex_rf_path path);
bool coexm_hw_auto_enabled(void);
bool coexm_hw_baseline_ready(void);
/* Startup-only board identity, without a fabricated connected channel.
 * Repeated calls must use the same topology. Does not enable PS-PTA. */
int coexm_hw_default_apply(enum coexm_hw_topology topology);
int coexm_hw_radio_update(const struct mac_chan_op *channel);
/* Notify actual home before lifecycle cleanup; NULL means confirmed no home. */
void coexm_hw_radio_notify(const struct mac_chan_op *channel);
/* Context lookup failed, not equivalent to no home. Neither API writes MMIO. */
void coexm_hw_radio_invalidate(void);
bool coexm_hw_radio_sync_lost(void);
bool coexm_hw_radio_matches(const struct mac_chan_op *channel);
int coexm_hw_plan_get(struct coexm_hw_plan *plan);

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
 * @param params Reserved initialization parameters; may be NULL.
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
 *          Initialized scheduling is paused even on failure; hardware fault
 *          and the last applied role remain unchanged on a pre-existing fault.
 * @return 0 on success, a negative value if PM is uninitialized or hardware
 *         restoration failed.
 */
int pm_coex_pause(void);

/**
 * @brief Resume the power management functionality.
 * @details Allows pm_coex_sleep and pm_coex_wakeup calls to function normally.
 * @return 0 on success, -1 if PM is not initialized or role apply failed.
 */
int pm_coex_resume(void);

/** Return true when the PS-PTA runtime is enabled for the current activation. */
bool coex_ps_pta_is_enabled(void);

/** Return true when the PS-PTA coordinator is currently running. */
bool coex_ps_pta_is_running(void);

/**
 * @brief Return capabilities of the CMake-selected MACSW backend.
 *
 * This query is read-only and does not inspect board-level antenna assembly.
 */
uint32_t coexm_hw_capabilities_get(void);

/**
 * @brief Read the current BZ RF path.
 * @param[out] path Current RF path, including an explicit unknown state.
 * @return 0 on success, -1 if @p path is NULL or hardware state is unknown.
 *
 * This query never changes the PHYRF path or coexistence registers.
 */
int coexm_rf_path_get(enum coex_rf_path *path);

/** Apply a resolved hardware plan using the selected chip backend. */
int coexm_hw_plan_apply(const struct coexm_hw_plan *plan);

/** Release the plan after applying the selected backend's inactive baseline. */
int coexm_hw_plan_restore(const struct coexm_hw_plan *plan);

/** Atomically replace an active plan after a same-band channel change. */
int coexm_hw_plan_replace(const struct coexm_hw_plan *old_plan,
                          const struct coexm_hw_plan *new_plan);

/** Temporarily apply a plan-specific scan recipe for the requested bands. */
int coexm_hw_scan_override_begin(uint8_t scan_band_mask);
/* Physical scan terminal only (including failure before scan starts).
 * Ends override ownership even on error; does not clear a hardware fault.
 * Caller serializes this with release of its acquired SCAN Protection owner. */
int coexm_hw_scan_override_end(bool joining);

enum coexm_hw_replay_reason {
    COEXM_HW_REPLAY_MAC_RESET = 0,
    COEXM_HW_REPLAY_MAC_REINIT,
    COEXM_HW_REPLAY_PDS_RESUME,
};

/** Replay the committed plan after MAC/PHY hardware state is reinitialized. */
int coexm_hw_replay(enum coexm_hw_replay_reason reason);

/** True when hardware could not be restored to the committed software fact. */
bool coexm_hw_is_faulted(void);

/** Read-only MACSW initialization fact, not proof that a recipe is applied. */
bool coexm_hw_is_ready(void);

/* Read-only hardware ownership check; callers must serialize any mutation. */
bool coexm_hw_blocks_reconfiguration(void);

/*
 * PS-PTA coordinator entrypoints
 ****************************************************************************************
 */

/**
 * @brief Enable PS-PTA for the current explicit activation.
 * @return 0 on success; -1 if runtime role apply failed.
 */
int coex_ps_pta_enable(void);

/**
 * @brief Disable PS-PTA for the current explicit activation.
 * @details Always disables local runtime and releases slice timer/TX gating,
 *          including on hardware failure. Does not clear hardware fault.
 * @return 0 on success; a negative value on hardware fault or restore failure.
 */
int coex_ps_pta_disable(void);

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
 * These hooks control the explicitly enabled PS_PTA runtime for the current
 * connection. A real disconnect invokes disable; reconnect does not restore it.
 */
void coex_coord_on_enable(void);
void coex_coord_on_disable(void);
void coex_coord_on_runtime_pause(void);
void coex_coord_on_runtime_resume(void);

/*
 * pm_coex_sleep failure bitmask for the PS-PTA coordinator
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
 * @brief Pause PS-PTA runtime behaviors
 * @details Pauses runtime behaviors (PTA switching, TBTT slice, host TX gating)
 *          while preserving the current-connection enable state.
 *
 * This is typically triggered when Wi-Fi PS is turned OFF.
 * Preserves the enable state, including DISABLED on late callbacks. Local
 * timer/TX-gate cleanup is unconditional, even if role restoration fails.
 *
 * @note Coex protection is controlled separately and remains disabled by
 *       default in the explicit-activation design.
 * @return 0 on success; a negative value on hardware fault or restore failure.
 */
int coex_ps_pta_runtime_pause(void);

/**
 * @brief Resume PS-PTA runtime behaviors
 * @details Resumes runtime behaviors after Wi-Fi PS is turned ON.
 *          Does nothing if PS-PTA is disabled.
 * @note Wi-Fi PS re-enable must be handled separately by caller.
 * @note This function does not enable or resume coex protection.
 * @return 0 on success; -1 if the PTA role transition failed.
 */
int coex_ps_pta_runtime_resume(void);

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

/** Result of a protection acquire request. */
typedef enum {
    COEX_PROTECT_ACQUIRE_ERROR = -1,
    COEX_PROTECT_ACQUIRED = 0,
    COEX_PROTECT_DISABLED,
    COEX_PROTECT_SKIPPED,
} coex_protect_acquire_result_t;

/** Backend decision for one Protection event. */
enum coexm_protect_action {
    COEXM_PROTECT_ACTION_SKIP = 0,
    COEXM_PROTECT_ACTION_WIFI_PRIORITY,
    COEXM_PROTECT_ACTION_WIFI_PRIORITY_SPDT,
    COEXM_PROTECT_ACTION_UNSUPPORTED,
};

#define COEX_PROTECT_SCAN_BAND_2G4 (1u << PHY_BAND_2G4)
#define COEX_PROTECT_SCAN_BAND_5G  (1u << PHY_BAND_5G)
#define COEX_PROTECT_SCAN_BAND_ALL \
    (COEX_PROTECT_SCAN_BAND_2G4 | COEX_PROTECT_SCAN_BAND_5G)

/// Timeout for 4-way handshake wait (milliseconds)
#define PM_COEX_HANDSHAKE_WAIT_TIMEOUT_MS  10000

/**
 * @brief Enable/disable coex protection module
 * @param enable true to enable, false to disable
 * @param profile Immutable board topology/RF-path profile when enabling;
 *        ignored when disabling.
 * @return 0 on success; -1 if active hardware priority could not be released
 */
int coex_protect_set_enabled(
    bool enable, const struct coexm_protect_profile *profile);

/**
 * @brief Check if coex protection module is enabled
 * @return true if enabled, false otherwise
 */
bool coex_protect_is_enabled(void);

/**
 * @brief Acquire RF protection for a specific operation type
 * @param type Protection type (COEX_PROT_SCAN, COEX_PROT_CONNECT, etc.)
 * @param band Wi-Fi band value, e.g. PHY_BAND_2G4 or PHY_BAND_5G.
 * @return One of @ref coex_protect_acquire_result_t. Disabled and skipped
 *         results are intentional no-ops and never imply a hardware fallback.
 * @note A SCAN request through this compatibility API represents one band.
 *       Mixed-band scans must use @ref coex_protect_scan_acquire.
 */
coex_protect_acquire_result_t coex_protect_acquire(
    coex_protect_type_t type, uint8_t band);

/**
 * @brief Acquire scan protection using all bands in the scan request.
 * @param band_mask OR of COEX_PROTECT_SCAN_BAND_* values.
 * @return One of @ref coex_protect_acquire_result_t.
 */
coex_protect_acquire_result_t coex_protect_scan_acquire(
    uint8_t band_mask);

/**
 * @brief Release RF protection for a specific operation type
 * @param type Protection type to release
 * @param band Wi-Fi band value, e.g. PHY_BAND_2G4 or PHY_BAND_5G.
 * @return 0 if released or already clear; -1 on invalid input or if the
 *         final hardware priority transition failed. On transition failure,
 *         the reference and mask remain active.
 */
int coex_protect_release(coex_protect_type_t type, uint8_t band);

/**
 * @brief Consume this physical scan's acquired reference at SCANU terminal.
 * @param held Receipt set only for COEX_PROTECT_ACQUIRED; consumed on cleanup.
 * @return 0 on normal cleanup/no receipt; -1 on error. A latched hardware fault
 *         still returns -1 after logical cleanup, without writing a PTA role.
 * @note Serialize with scan override end. Never releases other owners or
 *       changes their watchdog. This does not relax the general release API.
 */
int coex_protect_scan_complete(bool *held);

/**
 * @brief Release all RF protections (global emergency reset)
 * @return 0 on success; -1 if active hardware priority could not be released.
 *         On failure, the current references and mask remain active.
 * @note Reserved for module disable, global reset/replay, or fatal recovery.
 *       Normal operation owners must release only their own protection types.
 */
int coex_protect_release_all(void);

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
    /* PS layer PS-PTA gate */
    uint8_t ps_pta_state;          ///< ps_env.ps_pta_state (PS_PTA_DISABLED/ENABLED/RUNNING)

    /* pm_coex state machine */
    uint8_t pm_state;               ///< g_pm_coex_ctx.state (PM_COEX_STATE_*)
    uint8_t pta_current_role;       ///< g_pm_coex_ctx.pta_current_role (PTA_ROLE_*)
    bool wifi_active_window;        ///< g_pm_coex_ctx.wifi_active_window
    bool wifi_connecting;           ///< g_pm_coex_ctx.wifi_connecting

    /* WiFi duty cycle config */
    uint32_t wifi_duty_ms;          ///< WiFi active time in ms (configured via wifi_coex_duty_set)

    /* coex protect */
    bool protect_enabled;           ///< coex_protect module enabled
    bool protect_active;            ///< any protection currently active
    uint32_t protect_mask;          ///< active protection bitmask

    /** BT-path coexistence configuration snapshot */
    bool bt_path_spdt_ctrl_enabled;     ///< User enabled BT path SPDT control.
    bool bt_path_adj_tx_power_enabled;  ///< BT path TX power adjustment configured.
    bool bt_path_channel_overlay_enabled; ///< BT path channel overlay detection configured.
    bool bt_path_channel_bw_40;             ///< Connected channel uses 40 MHz.

    uint8_t bt_path_channel_connected;     ///< 2.4 GHz center channel, 0 means invalid.
    uint8_t bt_path_channel_overlay_margin; ///< Channel overlay detection margin.
    uint32_t bt_path_adj_tx_power_reg;  ///< RF power control register snapshot for TX power adjustment.
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

/* Print coex register snapshot (current vs default captured at pm_coex_init). */
void pm_coex_dump_registers(void);

/**
 * @brief Error codes returned by BT-path coexistence configuration APIs.
 *
 * Runtime BT-path reconfiguration is rejected while a Hardware Plan is active,
 * PS-PTA is enabled, or connection protection is enabled. Stop coexistence and
 * disable protection first.
 */
typedef enum {
    COEXM_CONFIG_BT_PATH_COEX_OK = 0,                                             ///< BT path coexistence configured.
    COEXM_CONFIG_BT_PATH_COEX_ERR_DEDICATED_ANTENNA_ON_2G_PATH = -1,              ///< Dedicated BT path antenna requested while BT path is on 2G path.
    COEXM_CONFIG_BT_PATH_COEX_ERR_BLE_TX_POWER_RANGE = -6,                        ///< BLE TX power value is out of range.
    COEXM_CONFIG_BT_PATH_COEX_ERR_IEEE802154_TX_POWER_RANGE = -7,                 ///< IEEE 802.15.4 TX power value is out of range.
    COEXM_CONFIG_BT_PATH_COEX_ERR_BT_TX_POWER_RANGE = -8,                         ///< BT TX power value is out of range.
    COEXM_CONFIG_BT_PATH_COEX_ERR_WIFI_TX_POWER_RANGE = -9,                       ///< Wi-Fi TX power value is out of range.
    COEXM_CONFIG_BT_PATH_COEX_ERR_CHANNEL_MARGIN_RANGE = -10,                    ///< Channel margin is out of range.
    COEXM_CONFIG_BT_PATH_COEX_ERR_SPDT_CTRL_ON_2G_PATH = -11,                    ///< SPDT control is only valid when BT uses BT path.
    COEXM_CONFIG_BT_PATH_COEX_ERR_CHANNEL_OVERLAY_REQUIRED = -12,                 ///< TX power adjustment requires channel overlay enabled first.
    COEXM_CONFIG_BT_PATH_COEX_ERR_BT_PATH_REQUIRED = -13,                         ///< Requested mode requires BT to use BT path.
    COEXM_CONFIG_BT_PATH_COEX_ERR_PS_PTA_CONFLICT = -14,                          ///< PTA-CHAN/PTA-CHAN-PWR cannot run with PS-PTA.
    COEXM_CONFIG_BT_PATH_COEX_ERR_INVALID_POWER_TYPE = -15,                       ///< Invalid adjusted TX power type.
    COEXM_CONFIG_BT_PATH_COEX_ERR_ADJ_TX_PWR_ENABLED = -16,
    COEXM_CONFIG_BT_PATH_COEX_ERR_SPDT_ENABLED = -17,
    COEXM_CONFIG_BT_PATH_COEX_ERR_PROTECTION_CONFLICT = -18,
    COEXM_CONFIG_BT_PATH_COEX_ERR_ACTIVE_PLAN_CONFLICT = -19,                    ///< An active Hardware Plan owns the coexistence registers.
    COEXM_CONFIG_BT_PATH_COEX_ERR_NOT_READY = -20,                               ///< MACSW coexistence hardware is not initialized.
} coexm_config_bt_path_coex_err_t;

/**
 * @brief Enable or disable SPDT control for BT-path coexistence.
 *
 * @param enable true to route BT path through the SPDT shared with Wi-Fi 2G
 *               antenna, false to disable SPDT control for dedicated BT path
 *               antenna mode.
 *
 * @note These coexm BT APIs configure BT path coexistence hardware, not the BT
 *       protocol stack.
 * @note Call this function after PHY/RF has routed BT/IEEE 802.15.4 to BT path.
 * @note This API controls the COEXM/PTA SPDT coexistence bit. It does not
 *       configure the RF switch GPIO mux or RF/FEM path-selection registers.
 * @note The application must configure the switch-control GPIO mux function to
 *       25. After BT wins RF access internally, the mux-25 GPIO polarity is
 *       index-based: even GPIOs such as IO0 drive high, and odd GPIOs such as
 *       IO1 drive low.
 *
 * @return COEXM_CONFIG_BT_PATH_COEX_OK on success; otherwise one of
 *         coexm_config_bt_path_coex_err_t error codes.
 */
coexm_config_bt_path_coex_err_t coexm_bt_set_spdt_ctrl(bool enable);

/**
 * @brief Configure channel overlay detection when BT path uses a dedicated
 *        antenna.
 *
 * @param enable true to enable channel overlay, false to disable it. When
 *               false, margin is ignored.
 * @param margin Channel overlay detection margin, valid range is 0..63.
 *
 * @note Call this function after PHY/RF has routed BT/IEEE 802.15.4 to BT path.
 * @note Existing TX power adjustment remains enabled on top of channel overlay.
 * @note Disable TX power adjustment before disabling channel overlay. If TX
 *       power adjustment is still enabled, the function returns
 *       COEXM_CONFIG_BT_PATH_COEX_ERR_ADJ_TX_PWR_ENABLED without changing
 *       hardware or software state.
 *
 * @return COEXM_CONFIG_BT_PATH_COEX_OK on success; otherwise one of
 *         coexm_config_bt_path_coex_err_t error codes.
 */
coexm_config_bt_path_coex_err_t coexm_bt_set_channel_overlay(
    bool enable, int margin);

/** Select the RF_PWR_CTRL_1 field updated by coexm_bt_set_adj_tx_power(). */
typedef enum {
    COEXM_BT_ADJ_TX_POWER_BLE = 0,
    COEXM_BT_ADJ_TX_POWER_IEEE802154,
    COEXM_BT_ADJ_TX_POWER_BT,
    COEXM_BT_ADJ_TX_POWER_WIFI,
} coexm_bt_adj_tx_power_type_t;

/**
 * @brief Configure one radio's TX power adjustment.
 *
 * @param enable true to enable adjustment, false to disable it. When false,
 *               type and pwr are ignored.
 * @param type Radio whose adjusted TX power should be changed.
 * @param pwr TX power adjustment, in 0.25 dBm units.
 *
 * @note Only the selected radio field is changed; the other register fields
 *       retain their current values.
 * @note Channel overlay must be enabled before configuring TX power adjustment.
 * @note The implementation validates the value and returns the corresponding
 *       range error; callers do not need to duplicate the range checks.
 * @note Enabling requires BT path routing and cannot be used with PS-PTA.
 *
 * @return COEXM_CONFIG_BT_PATH_COEX_OK on success; otherwise one of
 *         coexm_config_bt_path_coex_err_t error codes.
 */
coexm_config_bt_path_coex_err_t coexm_bt_set_adj_tx_power(
    bool enable, coexm_bt_adj_tx_power_type_t type, int8_t pwr);

/**
 * @brief Check whether BT is routed through the BT path.
 *
 * @return true if BT uses the BT path; false 2G path.
 */
bool coexm_bt_is_bt_path(void);

/**
 * @brief Check whether configuration allows for PS-PTA.
 *
 * @param band Current Wi-Fi PHY band.
 * @return true if the current band/path/configuration allows PS-PTA.
 */
bool coexm_bt_is_ps_pta_allowed(uint8_t band);

#ifdef __cplusplus
}
#endif

/// @}

#endif /* __MACSW_COEXM_PUBLIC_H__ */
