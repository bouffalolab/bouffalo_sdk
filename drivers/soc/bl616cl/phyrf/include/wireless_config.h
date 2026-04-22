#ifndef WIRELESS_CONFIG_H
#define WIRELESS_CONFIG_H

#include <stdint.h>

#define WIRELESS_ERR_INVALID_PARAM 0x01

// Helper macro to define channel power limits
#define DEFINE_WLAN_CHANNEL_POWER(pv_chan, pv_dsss, pv_cck, pv_g, pv_n20, pv_n40, pv_ax20, pv_ax40) \
    { \
        .channel = pv_chan, \
        .limits = { \
            .b_dsss = pv_dsss, \
            .b_cck = pv_cck, \
            .g = pv_g, \
            .n20 = pv_n20, \
            .n40 = pv_n40, \
            .ax20 = pv_ax20, \
            .ax40 = pv_ax40, \
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
        int8_t n40;        // Power limit for 802.11n 40MHz
        int8_t ax20;       // Power limit for 802.11ax 20MHz
        int8_t ax40;       // Power limit for 802.11ax 40MHz
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

