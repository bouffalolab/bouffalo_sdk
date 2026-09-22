#ifndef WIFI_MGMR_COEX_INTERNAL_H
#define WIFI_MGMR_COEX_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>

#include "coexm.h"
#include "mac_types.h"
#include "wifi_mgmr_coex.h"

/* Static platform binding. Prepare/status return wifi_mgmr_coex_error values;
 * SPDT callbacks obey the nonblocking MACSW hardware contract. */
struct wifi_mgmr_coex_rf_ops {
    /* Returns a bit per wifi_mgmr_coex_rf_mode, without writing hardware. */
    int (*board_modes_get)(const struct wifi_mgmr_coex_board_config *board,
                           uint32_t *modes);
    int (*prepare)(const struct wifi_mgmr_coex_board_config *board,
                   enum wifi_mgmr_coex_rf_mode mode);
    int (*status_get)(void);
    struct coexm_board_ops spdt;
};

const struct wifi_mgmr_coex_rf_ops *platform_coex_rf_ops_get(void);

enum wifi_mgmr_coex_spdt_debug_mode {
    WIFI_MGMR_COEX_SPDT_DEBUG_2G,
    WIFI_MGMR_COEX_SPDT_DEBUG_BT,
    WIFI_MGMR_COEX_SPDT_DEBUG_PTA,
};
int platform_coex_spdt_debug_apply(enum wifi_mgmr_coex_spdt_debug_mode mode);
int wifi_mgmr_coex_spdt_debug_apply(enum wifi_mgmr_coex_spdt_debug_mode mode);

struct wifi_mgmr_coex_radio_context {
    bool radio_ready;
    bool sta_active;
    bool ap_active;
    bool multi_channel;
    int8_t vif_idx;
    int8_t ps_vif_idx;
    bool ps_pta_capable;
    struct mac_chan_op channel;
};

struct wifi_mgmr_coex_resolve_input {
    struct wifi_mgmr_coex_board_config board;
    uint32_t hw_caps;
    enum coex_rf_path rf_path;
    bool radio_ready;
    bool multi_channel;
    bool ps_pta_capable;
    int8_t ps_vif_idx;
    uint8_t band;
    wifi_mgmr_coex_runtime_policy_t requested_policy;
};

struct wifi_mgmr_coex_effective_config {
    enum wifi_mgmr_coex_board_topology topology;
    enum coex_rf_path rf_path;
    wifi_mgmr_coex_runtime_policy_t runtime;
    uint8_t band;
};

struct wifi_mgmr_coex_radio_key {
    uint8_t band;
    uint8_t type;
    uint16_t prim20_freq;
    uint16_t center1_freq;
    uint16_t center2_freq;
};

struct wifi_mgmr_coex_activation_record {
    bool active;
    bool ps_pta_started;
    int8_t owner_vif_idx;
    int8_t ps_vif_idx;
    wifi_mgmr_coex_runtime_policy_t requested_policy;
    struct wifi_mgmr_coex_radio_key radio;
    struct wifi_mgmr_coex_effective_config effective;
};

struct wifi_mgmr_coex_activation_ops {
    int (*resolve)(wifi_mgmr_coex_runtime_policy_t policy,
                   struct wifi_mgmr_coex_effective_config *effective,
                   struct wifi_mgmr_coex_radio_context *radio,
                   void *context);
    int (*activate)(const struct wifi_mgmr_coex_effective_config *effective,
                    const struct wifi_mgmr_coex_radio_context *radio,
                    void *context);
    int (*deactivate)(
        const struct wifi_mgmr_coex_activation_record *activation,
        void *context);
    int (*replace)(
        const struct wifi_mgmr_coex_activation_record *activation,
        const struct wifi_mgmr_coex_effective_config *effective,
        const struct wifi_mgmr_coex_radio_context *radio,
        void *context);
    int (*duty_set)(uint8_t active_ms, void *context);
    uint32_t (*duty_get)(void *context);
    bool (*ps_pta_running)(void *context);
    bool (*hardware_faulted)(void *context);
    void *context;
};

struct wifi_mgmr_coex_activation_control {
    struct wifi_mgmr_coex_activation_record record;
    const struct wifi_mgmr_coex_activation_ops *ops;
    uint8_t scan_depth;
};

void wifi_mgmr_coex_activation_control_init(
    struct wifi_mgmr_coex_activation_control *control,
    const struct wifi_mgmr_coex_activation_ops *ops);
int wifi_mgmr_coex_activation_start(
    struct wifi_mgmr_coex_activation_control *control,
    wifi_mgmr_coex_runtime_policy_t policy);
int wifi_mgmr_coex_activation_stop(
    struct wifi_mgmr_coex_activation_control *control);
int wifi_mgmr_coex_activation_status_get(
    const struct wifi_mgmr_coex_activation_control *control,
    struct wifi_mgmr_coex_status *status);
int wifi_mgmr_coex_activation_duty_set(
    struct wifi_mgmr_coex_activation_control *control,
    uint8_t active_ms);
int wifi_mgmr_coex_activation_scan_begin(
    struct wifi_mgmr_coex_activation_control *control, int vif_idx);
int wifi_mgmr_coex_activation_scan_end(
    struct wifi_mgmr_coex_activation_control *control, int vif_idx);
int wifi_mgmr_coex_activation_radio_activated(
    struct wifi_mgmr_coex_activation_control *control, int vif_idx);
int wifi_mgmr_coex_activation_channel_changed(
    struct wifi_mgmr_coex_activation_control *control, int vif_idx);
int wifi_mgmr_coex_activation_radio_invalidated(
    struct wifi_mgmr_coex_activation_control *control, int vif_idx);

int wifi_mgmr_coex_resolve(
    const struct wifi_mgmr_coex_resolve_input *input,
    struct wifi_mgmr_coex_effective_config *effective);

int wifi_mgmr_coex_resolve_current(
    wifi_mgmr_coex_runtime_policy_t policy,
    struct wifi_mgmr_coex_effective_config *effective);

int wifi_mgmr_coex_snapshot_resolve(
    wifi_mgmr_coex_runtime_policy_t policy,
    struct wifi_mgmr_coex_effective_config *effective,
    struct wifi_mgmr_coex_radio_context *radio);

int wifi_mgmr_coex_control_init(void);

/* Product mutations. These functions run only in the fhost control task. */
int wifi_mgmr_coex_control_start(wifi_mgmr_coex_runtime_policy_t policy);
/* Called after MAC reset/start, before the first radio operation. */
int wifi_mgmr_coex_hardware_init(void);
int wifi_mgmr_coex_control_stop(void);
int wifi_mgmr_coex_control_status_get(struct wifi_mgmr_coex_status *status);
int wifi_mgmr_coex_control_duty_set(uint8_t active_ms);

/* Wi-Fi lifecycle facts emitted by the fhost control task. */
int wifi_mgmr_coex_lifecycle_scan_begin(int vif_idx);
int wifi_mgmr_coex_lifecycle_scan_end(int vif_idx);
int wifi_mgmr_coex_lifecycle_radio_activated(int vif_idx);
int wifi_mgmr_coex_lifecycle_channel_changed(int vif_idx);
int wifi_mgmr_coex_lifecycle_radio_invalidated(int vif_idx);
int wifi_mgmr_coex_lifecycle_connection_failed(int vif_idx);

/* fhost control-task hardware executors used by the lifecycle controller. */
int fhost_cntrl_coex_activate(
    const struct wifi_mgmr_coex_effective_config *effective,
    const struct wifi_mgmr_coex_radio_context *radio);
int fhost_cntrl_coex_deactivate(
    const struct wifi_mgmr_coex_activation_record *activation);
int fhost_cntrl_coex_replace(
    const struct wifi_mgmr_coex_activation_record *activation,
    const struct wifi_mgmr_coex_effective_config *effective,
    const struct wifi_mgmr_coex_radio_context *radio);
int fhost_cntrl_coex_duty_set(uint8_t active_ms);

int wifi_mgmr_coex_radio_context_get(
    struct wifi_mgmr_coex_radio_context *context);

#endif /* WIFI_MGMR_COEX_INTERNAL_H */
