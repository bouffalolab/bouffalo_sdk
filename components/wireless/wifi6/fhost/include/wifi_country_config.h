#ifndef __WIFI_COUNTRY_CONFIG_H_
#define __WIFI_COUNTRY_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

/* 5G Channel definitions using enum for type safety */
enum wifi_5g_channel_bit_t {
    /* UNII-1 band: 5.15-5.25 GHz */
    WIFI_CHANNEL_36 = 0,
    WIFI_CHANNEL_40,
    WIFI_CHANNEL_44,
    WIFI_CHANNEL_48,

    /* UNII-2A band: 5.25-5.35 GHz */
    WIFI_CHANNEL_52,
    WIFI_CHANNEL_56,
    WIFI_CHANNEL_60,
    WIFI_CHANNEL_64,

    /* UNII-2C band: 5.47-5.725 GHz */
    WIFI_CHANNEL_100,
    WIFI_CHANNEL_104,
    WIFI_CHANNEL_108,
    WIFI_CHANNEL_112,
    WIFI_CHANNEL_116,
    WIFI_CHANNEL_120,
    WIFI_CHANNEL_124,
    WIFI_CHANNEL_128,
    WIFI_CHANNEL_132,
    WIFI_CHANNEL_136,
    WIFI_CHANNEL_140,
    WIFI_CHANNEL_144,

    /* UNII-3 band: 5.725-5.850 GHz */
    WIFI_CHANNEL_149,
    WIFI_CHANNEL_153,
    WIFI_CHANNEL_157,
    WIFI_CHANNEL_161,
    WIFI_CHANNEL_165,

    /* Extended band (optional) */
    WIFI_CHANNEL_169,
    WIFI_CHANNEL_173,
    WIFI_CHANNEL_177,

    WIFI_CHANNEL_5G_MAX = WIFI_CHANNEL_177  /* Maximum channel index */
};

/* Bitmap configuration - based on enum index */
#define WIFI_5G_CHANNEL_COUNT      (WIFI_CHANNEL_5G_MAX + 1)  /* Total 28 channel positions */
#define WIFI_5G_BITMAP_SIZE        ((WIFI_5G_CHANNEL_COUNT + 7) / 8)  /* 4 bytes */

/* Bitmap operation macros - using enum index */
#define WIFI_5G_BITMAP_SET(bmp, chan_enum)   ((bmp)[(chan_enum) / 8] |= (1 << ((chan_enum) % 8)))
#define WIFI_5G_BITMAP_CLR(bmp, chan_enum)   ((bmp)[(chan_enum) / 8] &= ~(1 << ((chan_enum) % 8)))
#define WIFI_5G_BITMAP_GET(bmp, chan_enum)   (((bmp)[(chan_enum) / 8] >> ((chan_enum) % 8)) & 1)

/* Channel number to enum conversion function */
static inline enum wifi_5g_channel_bit_t wifi_5g_chan_to_enum(uint8_t channel) {
    switch (channel) {
        case 36:  return WIFI_CHANNEL_36;
        case 40:  return WIFI_CHANNEL_40;
        case 44:  return WIFI_CHANNEL_44;
        case 48:  return WIFI_CHANNEL_48;
        case 52:  return WIFI_CHANNEL_52;
        case 56:  return WIFI_CHANNEL_56;
        case 60:  return WIFI_CHANNEL_60;
        case 64:  return WIFI_CHANNEL_64;
        case 100: return WIFI_CHANNEL_100;
        case 104: return WIFI_CHANNEL_104;
        case 108: return WIFI_CHANNEL_108;
        case 112: return WIFI_CHANNEL_112;
        case 116: return WIFI_CHANNEL_116;
        case 120: return WIFI_CHANNEL_120;
        case 124: return WIFI_CHANNEL_124;
        case 128: return WIFI_CHANNEL_128;
        case 132: return WIFI_CHANNEL_132;
        case 136: return WIFI_CHANNEL_136;
        case 140: return WIFI_CHANNEL_140;
        case 144: return WIFI_CHANNEL_144;
        case 149: return WIFI_CHANNEL_149;
        case 153: return WIFI_CHANNEL_153;
        case 157: return WIFI_CHANNEL_157;
        case 161: return WIFI_CHANNEL_161;
        case 165: return WIFI_CHANNEL_165;
        case 169: return WIFI_CHANNEL_169;
        case 173: return WIFI_CHANNEL_173;
        case 177: return WIFI_CHANNEL_177;
        default: return WIFI_CHANNEL_5G_MAX;  /* Invalid channel */
    }
}

/* Enum to channel number conversion function */
static inline uint8_t wifi_5g_enum_to_chan(enum wifi_5g_channel_bit_t chan_enum) {
    const uint8_t channel_map[] = {
        36, 40, 44, 48, 52, 56, 60, 64,  /* UNII-1/2A */
        100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144,  /* UNII-2C */
        149, 153, 157, 161, 165,  /* UNII-3 */
        169, 173, 177  /* Extended band */
    };

    if (chan_enum <= WIFI_CHANNEL_177) {
        return channel_map[chan_enum];
    }
    return 0;  /* Invalid channel */
}

/* Country code configuration structure definition */
struct ieee80211_dot_d {
    char *code;
    uint8_t channel24G_num;
    uint8_t channel24G_start;
    uint8_t channel5G_num;
    uint8_t channel5G_bitmap[WIFI_5G_BITMAP_SIZE];  /* 4-byte bitmap, supports 28 5G channels */
};

/* Country configuration related functions */

/* Configure 5G channels for specified country */
void wifi_country_config_channels(const char* country_code);

/* Initialize channel configuration for all countries */
void wifi_country_init_all_configs(void);

/* Check if country code is supported */
bool wifi_country_is_supported(const char* country_code);

/* Get country's 5G channel configuration information
 * @param country_code: Country code
 * @param channel_count: Output parameter, returns channel count
 * @param channel_list: Output parameter, returns channel list (optional)
 * @return 0 for success, -1 for failure
 */
int wifi_country_get_5g_info(const char* country_code, uint8_t *channel_count, uint8_t channel_list[]);

/* Validate if channel is in specified country's configuration */
bool wifi_country_validate_channel(const char* country_code, uint8_t channel);

/* Get regulatory domain statistics */
void wifi_country_get_stats(int *total_countries, int *supported_reg_domains);

/* Country list declaration */
extern struct ieee80211_dot_d country_list[];

/* Country validation and query related functions - now support both built-in and custom countries */
int wifi_mgmr_check_country_code(const char* country_code);
int wifi_mgmr_get_channel_nums(const char *country_code, uint8_t *c24G_cnt, uint8_t *c5G_cnt);
int wifi_mgmr_get_channel_start(const char *country_code, uint8_t *chan_start);
int wifi_mgmr_get_channel_list(const char *country_code, uint8_t **c5G_list);

/* Country configuration verification print function */
void wifi_mgmr_print_country_config_verification(void);

/* ====================================================================
 * Custom Country Code API
 * ====================================================================
 * These APIs allow users to add their own country codes with custom
 * channel configurations for regulatory compliance or testing purposes.
 */

/* Custom country configuration structure */
typedef struct {
    char country_code[3];              /* Country code (max 3 chars + null terminator) */
    uint8_t channel24G_num;            /* Number of 2.4G channels (1-14) */
    uint8_t channel24G_start;          /* Starting 2.4G channel number (1-14) */
    uint8_t channel5G_bitmap[WIFI_5G_BITMAP_SIZE]; /* 5G channel bitmap */
    bool is_active;                    /* Whether this custom country is active */
} wifi_custom_country_config_t;

/* Maximum number of custom countries that can be added */
#define WIFI_MAX_CUSTOM_COUNTRIES  5

/* Custom country management functions */

/**
 * @brief Add a custom country with specific channel configuration
 *
 * @param country_code 2-letter ISO country code (e.g., "XX")
 * @param channel24G_num Number of 2.4G channels (1-14)
 * @param channel24G_start Starting 2.4G channel (1-14)
 * @param channel5G_list Array of 5G channel numbers (can be NULL for no 5G)
 * @param channel5G_count Number of channels in channel5G_list
 * @return int 0 on success, negative error code on failure:
 *         -1: Invalid parameters
 *         -2: Country code already exists
 *         -3: Maximum custom countries reached
 */
int wifi_country_add_custom(const char* country_code,
                           uint8_t channel24G_num,
                           uint8_t channel24G_start,
                           const uint8_t* channel5G_list,
                           uint8_t channel5G_count);

/**
 * @brief Remove a custom country configuration
 *
 * @param country_code Country code to remove
 * @return int 0 on success, negative error code on failure:
 *         -1: Country code not found
 *         -2: Cannot remove built-in country
 */
int wifi_country_remove_custom(const char* country_code);

/**
 * @brief Update channel configuration for an existing custom country
 *
 * @param country_code Country code to update
 * @param channel5G_list Array of 5G channel numbers (can be NULL for no 5G)
 * @param channel5G_count Number of channels in channel5G_list
 * @return int 0 on success, negative error code on failure:
 *         -1: Country code not found
 *         -2: Cannot modify built-in country
 */
int wifi_country_update_custom_channels(const char* country_code,
                                       const uint8_t* channel5G_list,
                                       uint8_t channel5G_count);

/**
 * @brief Get information about a custom country
 *
 * @param country_code Country code to query
 * @param config Output buffer for custom country config
 * @return int 0 on success, negative error code on failure:
 *         -1: Country code not found
 *         -2: Not a custom country
 */
int wifi_country_get_custom_info(const char* country_code,
                                wifi_custom_country_config_t* config);

/**
 * @brief Check if a country code is custom defined
 *
 * @param country_code Country code to check
 * @return true if country is custom defined, false otherwise
 */
bool wifi_country_is_custom(const char* country_code);

/**
 * @brief Get list of all custom country codes
 *
 * @param country_codes Output array for country codes (each 3 chars + null)
 * @param max_countries Maximum number of countries to return
 * @return int Number of custom countries found, or negative error code
 */
int wifi_country_get_custom_list(char country_codes[][4], int max_countries);

/**
 * @brief Clear all custom country configurations
 *
 * @return int 0 on success
 */
int wifi_country_clear_all_custom(void);

/**
 * @brief Validate custom country parameters
 *
 * @param country_code Country code to validate
 * @param channel24G_num Number of 2.4G channels
 * @param channel24G_start Starting 2.4G channel
 * @param channel5G_list Array of 5G channel numbers
 * @param channel5G_count Number of 5G channels
 * @return int 0 if valid, negative error code if invalid
 */
int wifi_country_validate_custom_params(const char* country_code,
                                       uint8_t channel24G_num,
                                       uint8_t channel24G_start,
                                       const uint8_t* channel5G_list,
                                       uint8_t channel5G_count);

/**
 * @brief Create a custom country by copying from an existing regulatory domain
 *
 * @param new_country_code New country code to create
 * @param template_reg_domain Regulatory domain to copy from ("FCC", "ETSI", etc.)
 * @return int 0 on success, negative error code on failure
 */
int wifi_country_create_from_domain(const char* new_country_code,
                                   const char* template_reg_domain);

#endif /* __WIFI_COUNTRY_CONFIG_H_ */
