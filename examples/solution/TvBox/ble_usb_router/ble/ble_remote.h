#ifndef BLE_REMOTE_H
#define BLE_REMOTE_H

#include "gatt.h"

/* Write and Write Without Response share the existing Notify value UUID. */
ssize_t ble_remote_write_rx(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                            const void *buf, uint16_t len, uint16_t offset, uint8_t flags);

#endif