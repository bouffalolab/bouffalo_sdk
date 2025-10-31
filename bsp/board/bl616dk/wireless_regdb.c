#include "wireless_config.h"
#include <stdint.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
// Example IEEE80211 power limits
// -----------------------------------------------------------------------------
// FCC power limits
static const struct reg_power_limits_t ieee80211_fcc_cpl_rules[] = {
    // chan,  dsss, cck,  g,    n20,  n40,  ax20, ax40
    /* Example rules */
    /*
    DEFINE_WLAN_CHANNEL_POWER(1,   20,   20,   15,   14,   23,   14,   23),
    DEFINE_WLAN_CHANNEL_POWER(2,   20,   20,   16,   16,   23,   16,   23),
    DEFINE_WLAN_CHANNEL_POWER(3,   20,   20,   18,   17,   12,   17,   13),
    DEFINE_WLAN_CHANNEL_POWER(4,   20,   20,   18,   18,   13,   18,   14),
    DEFINE_WLAN_CHANNEL_POWER(5,   20,   20,   19,   18,   15,   18,   15),
    DEFINE_WLAN_CHANNEL_POWER(6,   20,   20,   20,   20,   15,   19,   15),
    DEFINE_WLAN_CHANNEL_POWER(7,   20,   20,   19,   19,   15,   18,   15),
    DEFINE_WLAN_CHANNEL_POWER(8,   20,   20,   19,   18,   15,   18,   14),
    DEFINE_WLAN_CHANNEL_POWER(9,   20,   20,   18,   18,   14,   17,   14),
    DEFINE_WLAN_CHANNEL_POWER(10,  20,   20,   17,   17,   13,   16,   13),
    DEFINE_WLAN_CHANNEL_POWER(11,  20,   20,   15,   15,   12,   14,   11),
    DEFINE_WLAN_CHANNEL_POWER(12,  14,   14,   12,   12,   23,   12,   23),
    DEFINE_WLAN_CHANNEL_POWER(13,  9,    9,    10,   10,   23,   10,   23),
    */
};

// ETSI power limits
static const struct reg_power_limits_t ieee80211_etsi_cpl_rules[] = {
    // chan,  dsss, cck,  g,    n20,  n40,  ax20, ax40
    /* Example rules */
    /*
    DEFINE_WLAN_CHANNEL_POWER(1,   14,   14,   15,   15,   23,   15,   23),
    DEFINE_WLAN_CHANNEL_POWER(2,   14,   14,   15,   15,   23,   15,   23),
    DEFINE_WLAN_CHANNEL_POWER(3,   14,   14,   15,   15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(4,   14,   14,   15,   15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(5,   14,   14,   15,   15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(6,   14,   14,   15,   15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(7,   13,   13,   15,   14,   14,   14,   15),
    DEFINE_WLAN_CHANNEL_POWER(8,   13,   13,   15,   14,   14,   14,   15),
    DEFINE_WLAN_CHANNEL_POWER(9,   13,   13,   15,   14,   14,   14,   14),
    DEFINE_WLAN_CHANNEL_POWER(10,  13,   13,   15,   14,   14,   14,   15),
    DEFINE_WLAN_CHANNEL_POWER(11,  14,   14,   15,   14,   14,   14,   14),
    DEFINE_WLAN_CHANNEL_POWER(12,  14,   14,   15,   15,   23,   14,   23),
    DEFINE_WLAN_CHANNEL_POWER(13,  13,   13,   14,   14,   23,   14,   23),
    */
};

// SRRC power limits
static const struct reg_power_limits_t ieee80211_srrc_cpl_rules[] = {
    // chan,  dsss, cck,  g,    n20,  n40,  ax20, ax40
    /* Example rules */
    /*
    DEFINE_WLAN_CHANNEL_POWER(1,   15,   15,   15,  15,   23,   15,   23),
    DEFINE_WLAN_CHANNEL_POWER(2,   15,   15,   15,  15,   23,   15,   23),
    DEFINE_WLAN_CHANNEL_POWER(3,   15,   15,   15,  15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(4,   15,   15,   15,  15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(5,   15,   15,   15,  15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(6,   15,   15,   15,  15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(7,   15,   15,   15,  15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(8,   15,   15,   15,  15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(9,   15,   15,   15,  15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(10,  15,   15,   15,  15,   15,   15,   15),
    DEFINE_WLAN_CHANNEL_POWER(11,  14,   14,   15,  15,   15,   14,   15),
    DEFINE_WLAN_CHANNEL_POWER(12,  12,   12,   12,  12,   23,   13,   23),
    DEFINE_WLAN_CHANNEL_POWER(13,  10,   10,   10,  10,   23,   10,   23),
    */
};

// NCC power limits
static const struct reg_power_limits_t ieee80211_ncc_cpl_rules[] = {
    // chan,  dsss, cck,  g,    n20,  n40,  ax20, ax40
    /*
	DEFINE_WLAN_CHANNEL_POWER(1,   20,   20,   15,   14,   23,   14,   23),
    DEFINE_WLAN_CHANNEL_POWER(2,   20,   20,   16,   16,   23,   16,   23),
    DEFINE_WLAN_CHANNEL_POWER(3,   20,   20,   18,   17,   12,   17,   13),
    DEFINE_WLAN_CHANNEL_POWER(4,   20,   20,   18,   18,   13,   18,   14),
    DEFINE_WLAN_CHANNEL_POWER(5,   20,   20,   19,   18,   15,   18,   15),
    DEFINE_WLAN_CHANNEL_POWER(6,   20,   20,   20,   20,   15,   19,   15),
    DEFINE_WLAN_CHANNEL_POWER(7,   20,   20,   19,   19,   15,   18,   15),
    DEFINE_WLAN_CHANNEL_POWER(8,   20,   20,   19,   18,   15,   18,   14),
    DEFINE_WLAN_CHANNEL_POWER(9,   20,   20,   18,   18,   14,   17,   14),
    DEFINE_WLAN_CHANNEL_POWER(10,  20,   20,   17,   17,   13,   16,   13),
    DEFINE_WLAN_CHANNEL_POWER(11,  20,   20,   15,   15,   12,   14,   11),
    DEFINE_WLAN_CHANNEL_POWER(12,  14,   14,   12,   12,   23,   12,   23),
    DEFINE_WLAN_CHANNEL_POWER(13,  9,    9,    10,   10,   23,   10,   23),
    */
};

// MIC power limits
static const struct reg_power_limits_t ieee80211_mic_cpl_rules[] = {
    // chan,  dsss, cck,  g,    n20,  n40,  ax20, ax40
    /*
    DEFINE_WLAN_CHANNEL_POWER(1,   16,   16,   18,   17,   23,   17,   23),
    DEFINE_WLAN_CHANNEL_POWER(2,   16,   16,   18,   17,   23,   17,   23),
    DEFINE_WLAN_CHANNEL_POWER(3,   16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(4,   16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(5,   16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(6,   16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(7,   16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(8,   16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(9,   16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(10,  16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(11,  16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(12,  16,   16,   18,   17,   17,   17,   17),
    DEFINE_WLAN_CHANNEL_POWER(13,  17,   17,   18,   17,   23,   17,   23),
    DEFINE_WLAN_CHANNEL_POWER(14,  16,   16,   23,   23,   23,   23,   23),
    */
};

// KCC power limits
static const struct reg_power_limits_t ieee80211_kcc_cpl_rules[] = {
    // chan,  dsss, cck,  g,    n20,  n40,  ax20, ax40
    /*
    DEFINE_WLAN_CHANNEL_POWER(1,   16,   16,   18,   17,   23,   17,   23),
    DEFINE_WLAN_CHANNEL_POWER(2,   16,   16,   18,   17,   23,   17,   23),
    DEFINE_WLAN_CHANNEL_POWER(3,   16,   16,   18,   17,   16,   17,   16),
    DEFINE_WLAN_CHANNEL_POWER(4,   16,   16,   18,   17,   16,   17,   16),
    DEFINE_WLAN_CHANNEL_POWER(5,   16,   16,   18,   17,   16,   17,   16),
    DEFINE_WLAN_CHANNEL_POWER(6,   16,   16,   18,   17,   16,   17,   16),
    DEFINE_WLAN_CHANNEL_POWER(7,   16,   16,   18,   17,   16,   16,   16),
    DEFINE_WLAN_CHANNEL_POWER(8,   16,   16,   18,   17,   16,   16,   16),
    DEFINE_WLAN_CHANNEL_POWER(9,   16,   16,   18,   17,   16,   16,   16),
    DEFINE_WLAN_CHANNEL_POWER(10,  16,   16,   18,   17,   16,   16,   16),
    DEFINE_WLAN_CHANNEL_POWER(11,  16,   16,   18,   17,   16,   16,   16),
    DEFINE_WLAN_CHANNEL_POWER(12,  16,   16,   18,   17,   23,   16,   23),
    DEFINE_WLAN_CHANNEL_POWER(13,  16,   16,   18,   16,   23,   16,   23),
    */
};

// ROW (Rest of World) power limits
static const struct reg_power_limits_t ieee80211_row_cpl_rules[] = {
    // chan,  dsss, cck,  g,    n20,  n40,  ax20, ax40
    /*
    DEFINE_WLAN_CHANNEL_POWER(1,   20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(2,   20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(3,   20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(4,   20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(5,   20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(6,   20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(7,   20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(8,   20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(9,   20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(10,  20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(11,  20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(12,  20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(13,  20,   20,   18,   17,   18,   19,   19),
    DEFINE_WLAN_CHANNEL_POWER(14,  20,   20,   18,   17,   18,   19,   19),
    */
};

// Max (Max Power) power limits
static const struct reg_power_limits_t ieee80211_max_cpl_rules[] = {
    // chan,  dsss, cck,  g,    n20,  n40,  ax20, ax40
    /*
    DEFINE_WLAN_CHANNEL_POWER(1,   23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(2,   23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(3,   23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(4,   23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(5,   23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(6,   23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(7,   23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(8,   23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(9,   23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(10,  23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(11,  23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(12,  23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(13,  23,   23,   23,   23,   23,   23,   23),
    DEFINE_WLAN_CHANNEL_POWER(14,  23,   23,   23,   23,   23,   23,   23),
    */
};

// -----------------------------------------------------------------------------
// Example IEEE802154/BLE/BT power limits
// -----------------------------------------------------------------------------
/**
 * @brief Example rules for IEEE802154 under FCC regulations.
 *
 * The user sets freq_range_start/freq_range_end so that each rule
 * applies to a certain band, with max_power indicating the maximum allowed
 * power (dBm) for that band.
 */
static const int8_t nb_fcc_max_power = 20;
static const struct reg_power_limits_nb_t nb_fcc_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2468-1, 18),
    DEFINE_NARROWBAND_CHANNEL_POWER(2468, 2484-1, 11),
    */
};

static const struct reg_power_limits_nb_t nb_154_fcc_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2476-1, 19),
    DEFINE_NARROWBAND_CHANNEL_POWER(2476, 2485-1, 9),
    */
};

static const int8_t nb_srrc_max_power = 20;
static const struct reg_power_limits_nb_t nb_srrc_cpl_rules[] = {
    /* Example rules for SRRC */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2485-1, 5),
    */
};


static const struct reg_power_limits_nb_t nb_154_srrc_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2470-1, 6),
    DEFINE_NARROWBAND_CHANNEL_POWER(2470, 2485-1, 6),
    */
};

static const int8_t nb_etsi_max_power = 20;
static const struct reg_power_limits_nb_t nb_etsi_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2485-1, 5),
    */
};

static const struct reg_power_limits_nb_t nb_154_etsi_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2470-1, 6),
    DEFINE_NARROWBAND_CHANNEL_POWER(2470, 2485-1, 6),
    */
};

static const int8_t nb_ncc_max_power = 20;
static const struct reg_power_limits_nb_t nb_ncc_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2468-1, 18),
    DEFINE_NARROWBAND_CHANNEL_POWER(2468, 2484-1, 11),
    */ 
};

static const struct reg_power_limits_nb_t nb_154_ncc_cpl_rules[] = {
    /* Example rules */
    /*
	DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2476-1, 19),
    DEFINE_NARROWBAND_CHANNEL_POWER(2476, 2485-1, 9),
    */
};

static const int8_t nb_mic_max_power = 20;
static const struct reg_power_limits_nb_t nb_mic_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2485-1, 8),
    */
};

static const struct reg_power_limits_nb_t nb_154_mic_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2469-1, 8),
    DEFINE_NARROWBAND_CHANNEL_POWER(2470, 2485-1, 8),
    */
};

static const int8_t nb_kcc_max_power = 20;
static const struct reg_power_limits_nb_t nb_kcc_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2485-1, 9),
    */
};

static const struct reg_power_limits_nb_t nb_154_kcc_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2469-1, 10),
    DEFINE_NARROWBAND_CHANNEL_POWER(2470, 2485-1, 10),
    */
};

static const int8_t nb_row_max_power = 20;
static const struct reg_power_limits_nb_t nb_row_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2485-1, 13),
    */
};

static const struct reg_power_limits_nb_t nb_154_row_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2485-1, 13),
    */ 
};

static const int8_t nb_max_max_power = 20;
static const struct reg_power_limits_nb_t nb_max_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2485-1, 20),
    */
};

static const struct reg_power_limits_nb_t nb_154_max_cpl_rules[] = {
    /* Example rules */
    /*
    DEFINE_NARROWBAND_CHANNEL_POWER(2400, 2485-1, 20),
    */
};

// Define regulatory domain database
static const struct reg_domain_info_t reg_domains[] = {
    {
        .domain = REG_DOMAIN_FCC,
        .name = "FCC",
        .description = "United States/FCC & Canada/IC",
        .max_power = 30,
        .cpl_rules = ieee80211_fcc_cpl_rules,
        .num_cpl_rules = sizeof(ieee80211_fcc_cpl_rules) / sizeof(ieee80211_fcc_cpl_rules[0]),
        .nb_max_power = nb_fcc_max_power,
        .nb_cpl_rules = nb_fcc_cpl_rules,
        .nb_num_cpl_rules = sizeof(nb_fcc_cpl_rules) / sizeof(nb_fcc_cpl_rules[0]),
        .nb_154_cpl_rules = nb_154_fcc_cpl_rules,
        .nb_154_num_cpl_rules = sizeof(nb_154_fcc_cpl_rules) / sizeof(nb_154_fcc_cpl_rules[0]),
    },
    {
        .domain = REG_DOMAIN_ETSI,
        .name = "ETSI",
        .max_power = 20,
        .cpl_rules = ieee80211_etsi_cpl_rules,
        .num_cpl_rules = sizeof(ieee80211_etsi_cpl_rules) / sizeof(ieee80211_etsi_cpl_rules[0]),
        .nb_max_power = nb_etsi_max_power,
        .nb_cpl_rules = nb_etsi_cpl_rules,
        .nb_num_cpl_rules = sizeof(nb_etsi_cpl_rules) / sizeof(nb_etsi_cpl_rules[0]),
        .nb_154_cpl_rules = nb_154_etsi_cpl_rules,
        .nb_154_num_cpl_rules = sizeof(nb_154_etsi_cpl_rules) / sizeof(nb_154_etsi_cpl_rules[0]),
    },
    {
        .domain = REG_DOMAIN_SRRC,
        .name = "SRRC",
        .max_power = 27,
        .cpl_rules = ieee80211_srrc_cpl_rules,
        .num_cpl_rules = sizeof(ieee80211_srrc_cpl_rules) / sizeof(ieee80211_srrc_cpl_rules[0]),
        .nb_max_power = nb_srrc_max_power,
        .nb_cpl_rules = nb_srrc_cpl_rules,
        .nb_num_cpl_rules = sizeof(nb_srrc_cpl_rules) / sizeof(nb_srrc_cpl_rules[0]),
        .nb_154_cpl_rules = nb_154_srrc_cpl_rules,
        .nb_154_num_cpl_rules = sizeof(nb_154_srrc_cpl_rules) / sizeof(nb_154_srrc_cpl_rules[0]),
    },
    {
        .domain = REG_DOMAIN_NCC,
        .name = "NCC",
        .max_power = 27,
        .cpl_rules = ieee80211_ncc_cpl_rules,
        .num_cpl_rules = sizeof(ieee80211_ncc_cpl_rules) / sizeof(ieee80211_ncc_cpl_rules[0]),
        .nb_max_power = nb_ncc_max_power,
        .nb_cpl_rules = nb_ncc_cpl_rules,
        .nb_num_cpl_rules = sizeof(nb_ncc_cpl_rules) / sizeof(nb_ncc_cpl_rules[0]),
        .nb_154_cpl_rules = nb_154_ncc_cpl_rules,
        .nb_154_num_cpl_rules = sizeof(nb_154_ncc_cpl_rules) / sizeof(nb_154_ncc_cpl_rules[0]),
    },
    {
        .domain = REG_DOMAIN_MIC,
        .name = "MIC",
        .max_power = 30,
        .cpl_rules = ieee80211_mic_cpl_rules,
        .num_cpl_rules = sizeof(ieee80211_mic_cpl_rules) / sizeof(ieee80211_mic_cpl_rules[0]),
        .nb_max_power = nb_mic_max_power,
        .nb_cpl_rules = nb_mic_cpl_rules,
        .nb_num_cpl_rules = sizeof(nb_mic_cpl_rules) / sizeof(nb_mic_cpl_rules[0]),
        .nb_154_cpl_rules = nb_154_mic_cpl_rules,
        .nb_154_num_cpl_rules = sizeof(nb_154_mic_cpl_rules) / sizeof(nb_154_mic_cpl_rules[0]),
    },
    {
        .domain = REG_DOMAIN_KCC,
        .name = "KCC",
        .max_power = 30,
        .cpl_rules = ieee80211_kcc_cpl_rules,
        .num_cpl_rules = sizeof(ieee80211_kcc_cpl_rules) / sizeof(ieee80211_kcc_cpl_rules[0]),
        .nb_max_power = nb_kcc_max_power,
        .nb_cpl_rules = nb_kcc_cpl_rules,
        .nb_num_cpl_rules = sizeof(nb_kcc_cpl_rules) / sizeof(nb_kcc_cpl_rules[0]),
        .nb_154_cpl_rules = nb_154_kcc_cpl_rules,
        .nb_154_num_cpl_rules = sizeof(nb_154_kcc_cpl_rules) / sizeof(nb_154_kcc_cpl_rules[0]),
    },
    {
        .domain = REG_DOMAIN_ROW,
        .name = "ROW",
        .description = "Rest of World",
        .max_power = 20,
        .cpl_rules = ieee80211_row_cpl_rules,
        .num_cpl_rules = sizeof(ieee80211_row_cpl_rules) / sizeof(ieee80211_row_cpl_rules[0]),
        .nb_max_power = nb_row_max_power,
        .nb_cpl_rules = nb_row_cpl_rules,
        .nb_num_cpl_rules = sizeof(nb_row_cpl_rules) / sizeof(nb_row_cpl_rules[0]),
        .nb_154_cpl_rules = nb_154_row_cpl_rules,
        .nb_154_num_cpl_rules = sizeof(nb_154_row_cpl_rules) / sizeof(nb_154_row_cpl_rules[0]),
    },
    {
        .domain = REG_DOMAIN_MAX,
        .name = "MAX",
        .description = "Max Power",
        .max_power = 20,
        .cpl_rules = ieee80211_max_cpl_rules,
        .num_cpl_rules = sizeof(ieee80211_max_cpl_rules) / sizeof(ieee80211_max_cpl_rules[0]),
        .nb_max_power = nb_max_max_power,
        .nb_cpl_rules = nb_max_cpl_rules,
        .nb_num_cpl_rules = sizeof(nb_max_cpl_rules) / sizeof(nb_max_cpl_rules[0]),
        .nb_154_cpl_rules = nb_154_max_cpl_rules,
        .nb_154_num_cpl_rules = sizeof(nb_154_max_cpl_rules) / sizeof(nb_154_max_cpl_rules[0]),
    },
};

// Country code to regulatory domain mapping table
static const struct country_reg_map_t country_reg_map[] = {
    // North America
    {"US", REG_DOMAIN_FCC,  "",  1},
    {"CA", REG_DOMAIN_FCC,  "",  1},
    
    // Asia Pacific
    {"CN", REG_DOMAIN_SRRC, "",  86},
    {"TW", REG_DOMAIN_NCC,  "",  886},
    {"JP", REG_DOMAIN_MIC,  "",  81},
    {"KR", REG_DOMAIN_KCC,  "",  82},
    {"TH", REG_DOMAIN_FCC,  "",  66},
    {"IN", REG_DOMAIN_FCC,  "",  91},
    {"AU", REG_DOMAIN_FCC,  "",  61},
    {"NZ", REG_DOMAIN_FCC,  "",  64},
    
    // Europe
    {"GB", REG_DOMAIN_ETSI, "",  44},
    {"CH", REG_DOMAIN_ETSI, "",  41},

    // European Union (ETSI)
    {"EU", REG_DOMAIN_ETSI, "",  388},
    {"AT", REG_DOMAIN_ETSI, "",  43},
    {"BE", REG_DOMAIN_ETSI, "",  32},
    {"BG", REG_DOMAIN_ETSI, "",  359},
    {"HR", REG_DOMAIN_ETSI, "",  385},
    {"CY", REG_DOMAIN_ETSI, "",  357},
    {"CZ", REG_DOMAIN_ETSI, "",  420},
    {"DK", REG_DOMAIN_ETSI, "",  45},
    {"EE", REG_DOMAIN_ETSI, "",  372},
    {"FI", REG_DOMAIN_ETSI, "",  358},
    {"FR", REG_DOMAIN_ETSI, "",  33},
    {"DE", REG_DOMAIN_ETSI, "",  49},
    {"GR", REG_DOMAIN_ETSI, "",  30},
    {"HU", REG_DOMAIN_ETSI, "",  36},
    {"IE", REG_DOMAIN_ETSI, "",  353},
    {"IT", REG_DOMAIN_ETSI, "",  39},
    {"LV", REG_DOMAIN_ETSI, "",  371},
    {"LT", REG_DOMAIN_ETSI, "",  370},
    {"LU", REG_DOMAIN_ETSI, "",  352},
    {"MT", REG_DOMAIN_ETSI, "",  356},
    {"NL", REG_DOMAIN_ETSI, "",  31},
    {"PL", REG_DOMAIN_ETSI, "",  48},
    {"PT", REG_DOMAIN_ETSI, "",  351},
    {"RO", REG_DOMAIN_ETSI, "",  40},
    {"SK", REG_DOMAIN_ETSI, "",  421},
    {"SI", REG_DOMAIN_ETSI, "",  386},
    {"ES", REG_DOMAIN_ETSI, "",  34},
    {"SE", REG_DOMAIN_ETSI, "",  46},
    
    // South America
    {"BR", REG_DOMAIN_FCC,  "",  55},

    // Special Values
    {"00", REG_DOMAIN_ROW,  "",  800},
    {"WW", REG_DOMAIN_ROW,  "",  800},
};

// Default regulatory database
static const struct reg_db_t wireless_reg_db = {
    .antenna_gain_dbi = 0,
    .reg_domains = reg_domains,
    .num_reg_domains = sizeof(reg_domains) / sizeof(reg_domains[0]),
    .country_reg_map = country_reg_map,
    .num_country_maps = sizeof(country_reg_map) / sizeof(country_reg_map[0])
};

void wireless_regdb_init(void) {
    // Initialize the regulatory database
    // This function is be called by driver during the initialization phase
    wireless_config_set_reg_db(&wireless_reg_db);
}
