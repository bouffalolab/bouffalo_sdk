/**
  ******************************************************************************
  * @file    at_ble_main.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_BLE_MAIN_H
#define AT_BLE_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*ble_scan_callback_t)(uint8_t addr_type, uint8_t *addr, int8_t rssi, uint8_t evtype, uint8_t *data, int len);

int at_ble_start(void);

int at_ble_stop(void);

int at_ble_init(int role);

int at_ble_set_public_addr(uint8_t *addr);

int at_ble_get_public_addr(uint8_t *addr);

int at_ble_set_random_addr(uint8_t *addr);

int at_ble_set_tx_power(int power);

int at_ble_get_tx_power(void);

int at_ble_scan_start(ble_scan_callback_t cb);

int at_ble_scan_stop(void);

int at_ble_adv_start(void);

int at_ble_adv_stop(void);

int at_ble_is_valid_conn_idx(int idx);

int at_ble_is_connected(int idx);

int at_ble_conn(int idx, uint8_t *addr, int addr_type, int timeout);

int at_ble_conn_get_addr(int idx, uint8_t *addr);

int at_ble_conn_get_param(int idx, int *min_interval, int *max_interval, int *cur_interval, int *latency, int *timeout);

int at_ble_conn_update_param(int idx, int min_interval, int max_interval, int latency, int timeout);

int at_ble_disconn(int idx);

int at_ble_conn_update_datalen(int idx, int data_len, int time);

int at_ble_conn_get_mtu(int idx, int *mtu_size);

int at_ble_conn_update_mtu(int idx);

int at_ble_gatts_service_get(int srv_idx, uint8_t *srv_uuid, uint8_t *srv_type,uint8_t *uuid_type);

int at_ble_gatts_service_set(int srv_idx, uint8_t *srv_uuid, uint8_t srv_type,uint8_t uuid_type);

int at_ble_gatts_service_del(int srv_idx);

int at_ble_gatts_service_register(int enable);

int at_ble_gatts_service_char_get(int srv_idx, int char_idx, uint8_t *char_uuid, uint32_t *char_prop,uint32_t *char_perm,uint8_t *uuidtype);

int at_ble_gatts_service_char_set(int srv_idx, int char_idx, uint8_t *char_uuid, uint32_t char_prop,uint32_t char_perm, uint8_t uuid_type);

int at_ble_gatts_service_notify(int srv_idx, int char_idx, void * buffer, int length);

int at_ble_gatts_service_indicate(int srv_idx, int char_idx, void * buffer, int length);

int at_ble_gatts_service_read(int srv_idx, int char_idx, void * buffer, int length);

int at_ble_gattc_service_discover(int idx, int timeout);

int at_ble_gattc_service_char_discover(int idx, int srv_idx, int timeout);

int at_ble_gattc_service_write(int idx, int srv_idx, int char_idx, void * buffer, int length);

int at_ble_gattc_service_read(int idx, int srv_idx, int char_idx, int timeout);

int at_ble_subscribe(int idx, int ccc_handle, int value_handle,int value);

int at_ble_unsubscribe(int idx, int value_handle);

int at_ble_sec_paramter_setup(int sec);

int at_ble_sec_auth_cancel(int idx);

int at_ble_sec_auth_passkey_confirm(int idx);

int at_ble_sec_auth_pairing_confirm(int idx);

int at_ble_sec_auth_passkey(int idx, int passkey);

void at_ble_get_ltk_list(void);

int at_ble_get_unpair(char *addr, int type);

int at_ble_sec_start_security(int idx, int level);

#if defined(CONFIG_BT_BAS_SERVER)
int at_ble_register_bas(void);
int at_ble_unregister_bas(void);
int at_ble_get_battery_level(void);
int at_ble_set_battery_level(int idx, int lvl);
#endif

#if defined (CONFIG_BT_IAS_SERVER)
int at_ble_register_ias(void);
int at_ble_unregister_ias(void);
#endif

#if defined (CONFIG_BT_DIS_SERVER)

int at_ble_register_dis(int at_vid_src, int at_vid, int at_pid, int at_pnp_ver);
int at_ble_unregister_dis(void);
int at_ble_dis_set(char* dis_name, char* dis_value, int dis_value_len);

#endif

#ifdef __cplusplus
}
#endif

#endif/* AT_BLE_MAIN_H */

