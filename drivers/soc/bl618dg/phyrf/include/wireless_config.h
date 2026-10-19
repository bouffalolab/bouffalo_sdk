#ifndef WIRELESS_CONFIG_H
#define WIRELESS_CONFIG_H

#include <stdint.h>
#include "wl_api.h"

#define WIRELESS_ERR_INVALID_PARAM 0x01

#define WLAN_2G_POWER_CHANNELS            14
#define WLAN_5G_20M_POWER_CHANNELS        28
#define WLAN_5G_40M_POWER_CHANNELS        14
#define WLAN_5G_80M_POWER_CHANNELS        7
#define WLAN_5G_POWER_CHANNELS            (WLAN_5G_20M_POWER_CHANNELS + WLAN_5G_40M_POWER_CHANNELS + WLAN_5G_80M_POWER_CHANNELS)

static inline uint8_t wlan_channel_to_power_index(uint8_t channel)
{
    static const uint8_t wlan_5g_channels[] = {
        // 20M channels
        36, 40, 44, 48, 52, 56, 60, 64,
        100, 104, 108, 112, 116, 120, 124, 128,
        132, 136, 140, 144, 149, 153, 157, 161,
        165, 169, 173, 177,
        // 40M channels
        38, 46, 54, 62, 102, 110, 118, 126,
        134, 142, 151, 159, 167, 175,
        // 80M channels
        42, 58, 106, 122, 138, 155, 171,
    };

    if (channel >= 1 && channel <= WLAN_2G_POWER_CHANNELS) {
        return channel - 1;
    }

    for (uint8_t i = 0; i < sizeof(wlan_5g_channels) / sizeof(wlan_5g_channels[0]); i++) {
        if (channel == wlan_5g_channels[i]) {
            return WLAN_2G_POWER_CHANNELS + i;
        }
    }

    return NUM_WLAN_CHANNELS;
}

// Helper macros to define channel power limits
#define DEFINE_WLAN_CHANNEL_POWER(pv_chan, pv_dsss, pv_cck, pv_g, pv_n20, pv_ac20, pv_ax20, pv_n40, pv_ac40, pv_ax40, pv_ac80, pv_ax80) \
    { \
        .channel = pv_chan, \
        .limits = { \
            .b_dsss = pv_dsss, \
            .b_cck = pv_cck, \
            .g = pv_g, \
            .n20 = pv_n20, \
            .ac20 = pv_ac20, \
            .ax20 = pv_ax20, \
            .n40 = pv_n40, \
            .ac40 = pv_ac40, \
            .ax40 = pv_ax40, \
            .ac80 = pv_ac80, \
            .ax80 = pv_ax80, \
        } \
    }

#define DEFINE_NARROWBAND_CHANNEL_POWER(pv_freq_range_start, pv_freq_range_end, pv_max_power) \
    { \
        .freq_range_start = pv_freq_range_start, \
        .freq_range_end = pv_freq_range_end, \
        .max_power = pv_max_power, \
    }

// Regulatory domain identifiers
typedef enum {
    REG_DOMAIN_FCC = 0,
    REG_DOMAIN_ETSI,
    REG_DOMAIN_SRRC,
    REG_DOMAIN_NCC,
    REG_DOMAIN_MIC,
    REG_DOMAIN_KCC,
    REG_DOMAIN_ROW,  // Rest of World
    REG_DOMAIN_MAX
} reg_domain_t;

// Country code type (ISO 3166-1 alpha-2)
typedef struct {
    char code[2];  // Two-letter country code
} country_code_t;

// Country to regulatory domain mapping entry
struct country_reg_map_t {
    char country_code[2];  // ISO 3166-1 alpha-2 country code
    reg_domain_t domain;   // Regulatory domain for this country
    const char *name;      // Country name (optional, for debugging)
    uint16_t country_phone_code; // Country Phone Code
};

// Power limits per channel and mode
struct reg_power_limits_t {
    uint8_t channel;
    struct {
        int8_t b_dsss;     // Power limit for 802.11b DSSS (dBm)
        int8_t b_cck;      // Power limit for 802.11b CCK
        int8_t g;          // Power limit for 802.11g
        int8_t n20;        // Power limit for 802.11n 20MHz
        int8_t ac20;       // Power limit for 802.11ac 20MHz
        int8_t ax20;       // Power limit for 802.11ax 20MHz
        int8_t n40;        // Power limit for 802.11n 40MHz
        int8_t ac40;       // Power limit for 802.11ac 40MHz
        int8_t ax40;       // Power limit for 802.11ax 40MHz
        int8_t ac80;       // Power limit for 802.11ac 80MHz
        int8_t ax80;       // Power limit for 802.11ax 80MHz
    } limits;
};

// Power limits for IEEE802154/BT/BLE narrowband signal
struct reg_power_limits_nb_t {
    uint16_t freq_range_start;
    uint16_t freq_range_end;
    int8_t max_power;
};

// Regulatory domain database entry
struct reg_domain_info_t {
    reg_domain_t domain;
    const char *name;
    const char *description;
    int8_t max_power;
    uint8_t num_cpl_rules;
    const struct reg_power_limits_t *cpl_rules;
    int8_t nb_max_power;
    uint8_t nb_num_cpl_rules;
    const struct reg_power_limits_nb_t *nb_cpl_rules;
    uint8_t nb_154_num_cpl_rules;
    const struct reg_power_limits_nb_t *nb_154_cpl_rules;
};

// Regulatory database
struct reg_db_t {
    // Antenna gain
    uint8_t antenna_gain_dbi;

    // Regulatory domain section
    const struct reg_domain_info_t *reg_domains;
    uint8_t num_reg_domains;
    
    // Country mapping section
    const struct country_reg_map_t *country_reg_map;
    uint8_t num_country_maps;
};

// Spur mitigation rule for a specific channel
struct spur_rule_t {
    uint8_t channel;      // WiFi channel number
    uint32_t cfg20;        // Configuration for 20MHz mode
    uint32_t cfg40;        // Configuration for 40MHz mode
    uint32_t cfg80;        // Configuration for 80MHz mode
};

// Spur mitigation database
struct spur_mitigation_db_t {
    const struct spur_rule_t *rules;
    uint8_t num_rules;
};

// APIs to update country code
void wireless_config_update_country(const char *country_code);

// APIs to configure the databases
void wireless_config_set_spur_db(const struct spur_mitigation_db_t *spur_db);
void wireless_config_set_reg_db(const struct reg_db_t *reg_db);

// Weak functions that can be overridden by board-specific implementations
void wireless_regdb_init(void);
void wireless_spur_init(void);

// Debug log levels
#define WL_CFG_LOG_NONE      0  // No debug output
#define WL_CFG_LOG_ERROR     1  // Errors only
#define WL_CFG_LOG_WARNING   2  // Warnings and errors
#define WL_CFG_LOG_INFO      3  // General info, warnings and errors
#define WL_CFG_LOG_DEBUG     4  // Detailed debug info
#define WL_CFG_LOG_VERBOSE   5  // Very detailed debug info

// Configure the desired log level here
#ifndef WL_CFG_LOG_LEVEL
#define WL_CFG_LOG_LEVEL     WL_CFG_LOG_INFO  // Default to INFO level
#endif

#if 1
#define WL_CFG_LOG(level, fmt, ...) do {} while(0)
#else
// Debug log macros
#define WL_CFG_LOG(level, fmt, ...) \
    do { \
        if (level <= WL_CFG_LOG_LEVEL) { \
            printf("[WL_CFG][%s] " fmt "\r\n", \
                   level == WL_CFG_LOG_ERROR ? "ERROR" : \
                   level == WL_CFG_LOG_WARNING ? "WARN" : \
                   level == WL_CFG_LOG_INFO ? "INFO" : \
                   level == WL_CFG_LOG_DEBUG ? "DEBUG" : "VERB", \
                   ##__VA_ARGS__); \
        } \
    } while(0)
#endif

#define WL_CFG_ERROR(fmt, ...)   WL_CFG_LOG(WL_CFG_LOG_ERROR, fmt, ##__VA_ARGS__)
#define WL_CFG_WARNING(fmt, ...) WL_CFG_LOG(WL_CFG_LOG_WARNING, fmt, ##__VA_ARGS__)
#define WL_CFG_INFO(fmt, ...)    WL_CFG_LOG(WL_CFG_LOG_INFO, fmt, ##__VA_ARGS__)
#define WL_CFG_DEBUG(fmt, ...)   WL_CFG_LOG(WL_CFG_LOG_DEBUG, fmt, ##__VA_ARGS__)
#define WL_CFG_VERBOSE(fmt, ...) WL_CFG_LOG(WL_CFG_LOG_VERBOSE, fmt, ##__VA_ARGS__)

#endif // WIRELESS_CONFIG_H 
