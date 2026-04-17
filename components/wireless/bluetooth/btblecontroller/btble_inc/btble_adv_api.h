#ifndef BTBLE_ADV_API_H_
#define BTBLE_ADV_API_H_

#include <stdbool.h>
#include <stdint.h>

/**************************************************************************************************
 * BLE Beacon Only API
 **************************************************************************************************
 * These APIs bypass the HCI layer and directly control the advertising functionality.
 * Only available when CONFIG_BLE_BEACON_ONLY is enabled.
 **************************************************************************************************/

/**
 * Advertising parameters
 */
struct btble_adv_params {
    uint16_t adv_interval_min;     ///< Advertising interval min (0.625ms units)
    uint16_t adv_interval_max;     ///< Advertising interval max (0.625ms units)
    uint8_t  adv_type;             ///< Advertising type
    uint8_t  own_addr_type;        ///< Own address type
    uint8_t  peer_addr_type;       ///< Peer address type (for directed advertising)
    uint8_t  peer_addr[6];         ///< Peer address (for directed advertising)
    uint8_t  adv_channel_map;      ///< Channel map
    uint8_t  adv_filter_policy;    ///< Filter policy
};

/* Advertising type definitions */
#define BTBLE_ADV_TYPE_CONNECTABLE_UNDIRECTED       0x00  ///< ADV_IND
#define BTBLE_ADV_TYPE_CONNECTABLE_DIRECTED         0x01  ///< ADV_DIRECT_IND
#define BTBLE_ADV_TYPE_SCANNABLE_UNDIRECTED         0x02  ///< ADV_SCAN_IND
#define BTBLE_ADV_TYPE_NON_CONNECTABLE_UNDIRECTED   0x03  ///< ADV_NONCONN_IND (Beacon)

/* Address type definitions */
#define BTBLE_ADDR_TYPE_PUBLIC                      0x00
#define BTBLE_ADDR_TYPE_RANDOM                      0x01
#define BTBLE_ADDR_TYPE_RPA_OR_RANDOM               0x02
#define BTBLE_ADDR_TYPE_RPA_OR_PUBLIC               0x03

/* Channel map definitions */
#define BTBLE_ADV_CHANNEL_37                        0x01
#define BTBLE_ADV_CHANNEL_38                        0x02
#define BTBLE_ADV_CHANNEL_39                        0x04
#define BTBLE_ADV_CHANNEL_ALL                       0x07

/* Filter policy definitions */
#define BTBLE_ADV_FILTER_ALLOW_ALL                  0x00
#define BTBLE_ADV_FILTER_WHITE_LIST_SCAN            0x01
#define BTBLE_ADV_FILTER_WHITE_LIST_CONN            0x02
#define BTBLE_ADV_FILTER_WHITE_LIST_ALL             0x03

/* Advertising interval range (in 0.625ms units) */
#define BTBLE_ADV_INTERVAL_MIN                      0x0020  /* 20ms */
#define BTBLE_ADV_INTERVAL_MAX                      0x4000  /* 10.24s */

/* Advertising data max length */
#define BTBLE_ADV_DATA_MAX_LEN                      31
#define BTBLE_ADV_SCAN_RSP_DATA_MAX_LEN             31

/**
 * @brief Initialize BLE advertising subsystem
 *
 * Must be called after btble_controller_init() and before btble_adv_set_parameter().
 * Sends HCI_Reset and Set_Event_Mask to the controller.
 *
 * @return 0 on success, negative error code on failure
 */
int btble_adv_init(void);

/**
 * @brief Deinitialize BLE advertising subsystem
 *
 * Stops advertising if active, releases internal resources.
 * Must be called before btble_controller_deinit().
 */
void btble_adv_deinit(void);

/**
 * @brief Set advertising parameters
 *
 * btble_adv_init() must be called before this function.
 *
 * @param params Pointer to advertising parameters
 * @return 0 on success, negative error code on failure
 */
int btble_adv_set_parameter(struct btble_adv_params *params);

/**
 * @brief Set advertising data
 * @param data Pointer to advertising data (max 31 bytes)
 * @param len Length of advertising data (0-31)
 * @return 0 on success, negative error code on failure
 */
int btble_adv_set_data(uint8_t *data, uint8_t len);

/**
 * @brief Set scan response data
 * @param data Pointer to scan response data (max 31 bytes)
 * @param len Length of scan response data (0-31)
 * @return 0 on success, negative error code on failure
 */
int btble_adv_set_scan_rsp_data(uint8_t *data, uint8_t len);

/**
 * @brief Start advertising
 * @return 0 on success, negative error code on failure
 */
int btble_adv_start(void);

/**
 * @brief Stop advertising
 * @return 0 on success, negative error code on failure
 */
int btble_adv_stop(void);

/**
 * @brief Update advertising data while advertising is active
 * @param data Pointer to new advertising data (max 31 bytes)
 * @param len Length of advertising data (0-31)
 * @return 0 on success, negative error code on failure
 */
int btble_adv_update_data(uint8_t *data, uint8_t len);

/**
 * @brief Update scan response data while advertising is active
 * @param data Pointer to new scan response data (max 31 bytes)
 * @param len Length of scan response data (0-31)
 * @return 0 on success, negative error code on failure
 */
int btble_adv_update_scan_rsp_data(uint8_t *data, uint8_t len);

/**
 * @brief Check if advertising is currently active
 * @return 1 if advertising, 0 if not advertising
 */
int btble_adv_is_active(void);

/**
 * @brief Set random address
 * @param random_addr Pointer to 6-byte random address
 * @return 0 on success, negative error code on failure
 */
int btble_adv_set_random_addr(uint8_t *random_addr);

/**
 * @brief Get the controller public BD address (MAC)
 *
 * Returns the 6-byte public BD address loaded from efuse during
 * btble_controller_init(). Byte order: addr[0]=LSB, addr[5]=MSB.
 * Must be called after btble_controller_init().
 *
 * @param addr Output buffer, at least 6 bytes
 * @return 0 on success, -1 if addr is NULL
 */
int btble_adv_get_public_addr(uint8_t *addr);

#endif /* BTBLE_ADV_API_H_ */
