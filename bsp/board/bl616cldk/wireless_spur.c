#include "wireless_config.h"
#include <string.h>

// Empty spur mitigation rules since no clock spurs are present
static const struct spur_rule_t default_spur_rules[] = {

    // Example
    // Channel 2422MHz Spur 2424MHz
    // rf_rx_notch1_en_setf(1);
    // rf_rx_notch1_alpha_setf(2);
    // rf_rx_notch1_fnrm_setf(102);
    // {
    //     .channel = 3,
    //     .cfg20 = 0x90660000,
    //     .cfg40 = 0x0,
    // },
    // Channel 2427MHz Spur 2424MHz
    // rf_rx_notch1_en_setf(1);
    // rf_rx_notch1_alpha_setf(2);
    // rf_rx_notch1_fnrm_setf(1895);
    // {
    //     .channel = 4,
    //     .cfg20 = 0x97670000,
    //     .cfg40 = 0x0,
    // }
};

// Default spur mitigation database with empty rules
static const struct spur_mitigation_db_t wireless_spur_db = {
    .rules = default_spur_rules,
    .num_rules = sizeof(default_spur_rules) / sizeof(default_spur_rules[0]),
};

void wireless_spur_init(void) {
    // Initialize the spur mitigation database
    // This function is be called by driverduring the initialization phase
    wireless_config_set_spur_db(&wireless_spur_db);
}
