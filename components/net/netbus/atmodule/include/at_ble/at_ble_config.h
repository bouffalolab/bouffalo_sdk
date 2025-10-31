/**
  ******************************************************************************
  * @file    at_ble_config.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_BLE_CONFIG_H
#define AT_BLE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_CONN_MAX_NUM 2

#define BLE_SRV_MAX_NUM 3
#define BLE_CHAR_MAX_NUM 5

#define BLE_GATTC_SRV_MAX_NUM 8
#define BLE_GATTC_CHAR_MAX_NUM 16

#define BLE_GATT_SRV_PRIMARY 0x01
#define BLE_GATT_SRV_SECONDARY 0x00

#define BLE_GATT_CHAR_PROP_READ 0x02
#define BLE_GATT_CHAR_PROP_WRITE_WITHOUT_RESP 0x04
#define BLE_GATT_CHAR_PROP_WRITE 0x08
#define BLE_GATT_CHAR_PROP_NOTIFY 0x10
#define BLE_GATT_CHAR_PROP_INDICATE 0x20


#define BLE_GATT_PERM_READ 0x01
#define BLE_GATT_PERM_WRITE 0x02

#define AT_CONFIG_KEY_BLE_NAME         		"BLENAME"

typedef enum {
    BLE_DISABLE = 0,
    BLE_CLIENT,
    BLE_SERVER,
} ble_work_role;

typedef struct {
    uint8_t addr[6];
} ble_addr;

typedef struct {
    uint16_t adv_int_min;
    uint16_t adv_int_max;
    uint8_t adv_type;
    uint8_t own_addr_type;
    uint8_t channel_map;
    uint8_t adv_filter_policy;
    uint8_t peer_addr_type;
    uint8_t peer_addr[6];
    uint8_t primary_PHY;
    uint8_t secondary_PHY;
} ble_adv_param;

typedef struct {
    uint8_t scan_type;
    uint8_t own_addr_type;
    uint8_t filter_policy;
    uint16_t scan_interval;
    uint16_t scan_window;
} ble_scan_param;

typedef struct {
    uint8_t len;
    uint8_t data[31];
} ble_adv_data;

typedef struct {
    uint8_t len;
    uint8_t data[31];
} ble_scan_rsp_data;

typedef struct {
    ble_work_role work_role;
    char ble_name[29+1];
    ble_adv_param adv_param;
    ble_scan_param scan_param;
    ble_adv_data adv_data;
    ble_scan_rsp_data scan_rsp_data;
    int ble_sec_param;
}ble_config;

extern ble_config *at_ble_config;

int at_ble_config_init(void);

int at_ble_config_save(const char *key);

int at_ble_config_default(void);

#ifdef __cplusplus
}
#endif

#endif/* AT_BLE_CONFIG_H */

