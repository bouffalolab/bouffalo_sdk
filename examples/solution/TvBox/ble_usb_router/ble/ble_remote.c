#include "ble_common.h"
#include "ble_remote.h"

/* Phone and sender clients write the same channel frames through this UUID. */
ssize_t ble_remote_write_rx(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                            const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    (void)conn;
    (void)attr;

    /* Channel frames already provide fragmentation; ATT long writes do not. */
    if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
        return BT_GATT_ERR(BT_ATT_ERR_NOT_SUPPORTED);
    }
    if (offset != 0) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    if (!buf || len < BLE_CHAN_FRAME_HDR_LEN || len > BLE_CHAN_ATT_PAYLOAD_MAX) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    ble_common_receive_frame((const uint8_t *)buf, len);
    return len;
}