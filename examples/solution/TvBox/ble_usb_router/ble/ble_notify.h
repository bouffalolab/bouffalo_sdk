#ifndef BLE_NOTIFY_H
#define BLE_NOTIFY_H

#include <stddef.h>
#include <stdint.h>
#include "conn.h"
#include "gatt.h"

/* Internal lifecycle hooks called by the common connection manager. */
int ble_notify_init(void);
void ble_notify_connected(void);
void ble_notify_disconnected(void);
int ble_notify_send_frame(const uint8_t *data, size_t len, uint32_t timeout_ms);

#endif