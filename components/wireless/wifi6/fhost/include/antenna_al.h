/**
 * @file antenna_al.h
 * @brief Hardware Abstraction Layer for antenna control
 */

#ifndef __ANTENNA_HAL_H__
#define __ANTENNA_HAL_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ANT_CAPA_STATIC_DIV = 1,
    ANT_CAPA_DYNAMIC_DIV = 2,
} ant_capa_t;

/**
 * @brief Log levels for antenna HAL
 */
typedef enum {
    ANT_LOG_ERROR = 0,  ///< Error conditions
    ANT_LOG_WARN,       ///< Warning conditions
    ANT_LOG_INFO,       ///< Informational messages
    ANT_LOG_DEBUG,      ///< Debug messages
} ant_log_level_t;

/**
 * @brief Log categories for antenna HAL
 */
typedef enum {
    ANT_CAT_STATIC = 0,  ///< Static antenna diversity
    ANT_CAT_DYNAMIC,     ///< Dynamic antenna diversity
    ANT_CAT_HAL,         ///< HAL operations
} ant_log_category_t;

/**
 * @brief Maximum supported antennas
 */
#define ANTENNA_MAX_COUNT 4

/**
 * @brief Antenna selection
 */
typedef enum {
    ANTENNA_0 = 0,  ///< Primary antenna
    ANTENNA_1 = 1,  ///< Secondary antenna
    ANTENNA_2 = 2,  ///< Third antenna
    ANTENNA_3 = 3,  ///< Fourth antenna
    ANTENNA_MAX     ///< Maximum number of antennas
} antenna_id_t;

/**
 * @brief Selected antenna index for dynamic diversity
 */
typedef enum {
    ANT_SEL_PRIMARY = 0,    ///< Primary selected antenna
    ANT_SEL_SECONDARY = 1,  ///< Secondary selected antenna
} ant_sel_idx_t;

/**
 * @brief Selected antenna pair for dynamic diversity
 */
typedef struct {
    antenna_id_t primary;    ///< Primary antenna ID
    antenna_id_t secondary;  ///< Secondary antenna ID
    bool valid;             ///< Whether the selection is valid
} antenna_pair_t;

/**
 * @brief Antenna control operations and configuration
 */
typedef struct {
    /**
     * @brief Initialize antenna control hardware
     * @return 0 on success, negative value on error
     */
    int (*init)(void);

    /**
     * @brief Switch to specified antenna
     * @param antenna Antenna to switch to
     * @return 0 on success, negative value on error
     */
    int (*switch_antenna)(antenna_id_t antenna);

    /**
     * @brief Get number of supported antennas
     * @return Number of supported antennas (1-4)
     */
    uint8_t (*get_antenna_count)(void);

    /**
     * @brief Deinitialize antenna control hardware
     */
    void (*deinit)(void);

    /**
     * @brief Log function for antenna HAL
     * @param level Log level
     * @param category Log category  
     * @param fmt Format string
     * @param args Variable arguments
     */
    void (*log)(ant_log_level_t level, ant_log_category_t category, const char *fmt, va_list args);

    /**
     * @brief Antenna diversity configuration
     */
    bool dynamic_div_enabled;

    /**
     * @brief Antenna diversity configuration
     */
    bool static_div_enabled;
} antenna_hal_ops_t;

/**
 * @brief Initialize antenna HAL
 * @param ops Pointer to antenna control operations and configuration
 * @return 0 on success, negative value on error
 */
int antenna_hal_init(const antenna_hal_ops_t *ops);

/**
 * @brief Switch to specified antenna
 * @param antenna Antenna to switch to
 * @return 0 on success, negative value on error
 */
int antenna_hal_switch_antenna(antenna_id_t antenna);

/**
 * @brief Get current active antenna
 * @return Current active antenna
 */
antenna_id_t antenna_hal_get_current_antenna(void);

/**
 * @brief Get number of supported antennas
 * @return Number of supported antennas (1-4)
 */
uint8_t antenna_hal_get_antenna_count(void);

/**
 * @brief Deinitialize antenna HAL
 */
void antenna_hal_deinit(void);

/**
 * @brief Enable logging for a category
 * @param category Log category to enable
 */
void antenna_hal_log_enable(ant_log_category_t category);

/**
 * @brief Disable logging for a category
 * @param category Log category to disable
 */
void antenna_hal_log_disable(ant_log_category_t category);

/**
 * @brief Set minimum log level
 * @param level Minimum log level
 */
void antenna_hal_log_set_level(ant_log_level_t level);

/**
 * @brief Set selected antenna pair for dynamic diversity
 * @param primary Primary antenna ID
 * @param secondary Secondary antenna ID
 * @return 0 on success, negative value on error
 */
int antenna_hal_set_selected_pair(antenna_id_t primary, antenna_id_t secondary);

/**
 * @brief Get selected antenna pair for dynamic diversity
 * @return Selected antenna pair structure
 */
antenna_pair_t antenna_hal_get_selected_pair(void);

/**
 * @brief Clear selected antenna pair
 */
void antenna_hal_clear_selected_pair(void);

/**
 * @brief Switch between selected antennas for dynamic diversity
 * This API is designed for MM layer to switch between the two selected antennas.
 * It will automatically map ANT_SEL_PRIMARY/SECONDARY to the actual antenna IDs.
 * @param sel Which antenna in the selected pair to use
 * @return 0 on success, negative value on error
 */
int antenna_hal_switch_selected(ant_sel_idx_t sel);

/**
 * @brief Get current selected antenna index
 * Returns which antenna (primary/secondary) is currently active
 * @return Current selected antenna index
 */
ant_sel_idx_t antenna_hal_get_current_selected(void);

/**
 * @brief Antenna diversity capabilities and control
 */
typedef struct {
    struct {
        uint8_t supported: 1;  ///< Whether static diversity is supported (compile-time)
        uint8_t enabled: 1;    ///< Whether static diversity is enabled (runtime)
        uint8_t reserved: 6;   ///< Reserved for future use
    } static_div;

    struct {
        uint8_t supported: 1;  ///< Whether dynamic diversity is supported (compile-time)
        uint8_t enabled: 1;    ///< Whether dynamic diversity is enabled (runtime)
        uint8_t reserved: 6;   ///< Reserved for future use
    } dynamic_div;
} antenna_div_caps_t;

/**
 * @brief Get antenna diversity capabilities and control flags
 * @return Current antenna diversity capabilities and control
 */
bool antenna_hal_is_dynamic_div_enabled(void);

/**
 * @brief Get antenna diversity capabilities and control flags
 * @return Current antenna diversity capabilities and control
 */
bool antenna_hal_is_static_div_enabled(void);

/**
 * @brief Enable/disable static antenna diversity
 * @param enable true to enable, false to disable
 * @return 0 on success, negative value on error
 * @note Only works if static diversity is supported
 */
int antenna_hal_static_div_enable(bool enable);

/**
 * @brief Enable/disable dynamic antenna diversity
 * @param enable true to enable, false to disable
 * @return 0 on success, negative value on error
 * @note Only works if dynamic diversity is supported
 */
int antenna_hal_dynamic_div_enable(bool enable);

/**
 * @brief Log a message through the registered log function
 * @param level Log level
 * @param category Log category
 * @param fmt Format string
 * @param ... Variable arguments
 */
//void antenna_hal_log(ant_log_level_t level, ant_log_category_t category, const char *fmt, ...);
#define antenna_hal_log(...)

#ifdef __cplusplus
}
#endif

#endif /* __ANTENNA_HAL_H__ */ 
