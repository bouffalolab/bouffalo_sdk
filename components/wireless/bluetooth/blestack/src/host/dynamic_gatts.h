#ifndef _BLE_DYNAMIC_GATTS_H_
#define _BLE_DYNAMIC_GATTS_H_

#include <bluetooth.h>
#include <conn.h>
#include <gatt.h>
#include "bt_uuid.h"
typedef void (*dynamic_gatt_wr_callbck_func_t)(const struct bt_gatt_attr* attr,u8_t *data, int * length);
typedef void (*dynamic_gatt_rd_callbck_func_t)(const struct bt_gatt_attr* attr ,u8_t *data, u16_t length);
typedef void (*dynamic_gatt_noti_callbck_func_t)(const struct bt_gatt_attr* attr ,u8_t data);

#define GATT_SERVICE_PRIMARY		0x00
#define GATT_SERVICE_SECONDARY		0x01

struct gatt_add_descriptor {
	uint16_t desc_id;
	uint8_t permissions;
	const struct bt_uuid *uuid;
};

struct gatt_add_characteristic {
	uint16_t char_id;
	uint8_t properties;
	uint8_t permissions;
	const struct bt_uuid *uuid;
};

void ble_dynamic_gatt_server_init(void);
void ble_dynamic_gatt_server_deinit(void);
int ble_dynamic_unregister_service(void);
void ble_dynamic_gatt_cb_register(dynamic_gatt_rd_callbck_func_t rb,dynamic_gatt_wr_callbck_func_t wb,dynamic_gatt_noti_callbck_func_t nb);
int  ble_dynamic_gatt_add_service(struct bt_uuid * uuid,unsigned char uuid_type, unsigned char service_type);
int  ble_dynamic_gatt_add_characteristic(struct gatt_add_characteristic *ch);
int  ble_dynamic_gatt_add_descriptor(struct gatt_add_descriptor *cmd);
void ble_dynamic_gatt_start_server(void);
struct bt_gatt_attr* ble_dynamic_gatt_get_attr(struct bt_uuid *uuid);
#endif
