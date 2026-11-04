#ifndef BLE_COMMON_H
#define BLE_COMMON_H

#include <stddef.h>
#include <stdint.h>

#include "conn.h"
#include "gatt.h"

#ifndef USB_ECM_BLE_ENABLE_WRITE_RX
#define USB_ECM_BLE_ENABLE_WRITE_RX 1
#endif

#ifndef USB_ECM_BLE_ENABLE_WRITE_TX
#define USB_ECM_BLE_ENABLE_WRITE_TX 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BLE_COMMON_CHANNEL_VOICE = 1,
    BLE_COMMON_CHANNEL_KEY = 2,
    BLE_COMMON_CHANNEL_USER = 3,
    BLE_COMMON_CHANNEL_MAX = BLE_COMMON_CHANNEL_USER,
} ble_common_channel_t;

/* Per-channel received-frame callback. Register the same handler on every
 * channel to receive all traffic, or different handlers per channel. */
typedef void (*ble_common_rx_cb_t)(ble_common_channel_t channel,
                                    const uint8_t *data,
                                    size_t len);

/* Call serially from a startup task; BL616 controller init runs synchronously.
 * Return does not indicate controller health or asynchronous BLE host readiness. */
int ble_common_stack_init(void);
#if USB_ECM_BLE_ENABLE_WRITE_RX
void ble_common_set_channel_data_callback(ble_common_channel_t channel, ble_common_rx_cb_t cb);
void ble_common_receive_frame(const uint8_t *data, uint16_t len);
#endif
#if USB_ECM_BLE_ENABLE_WRITE_TX
int ble_common_send_channel(ble_common_channel_t channel, const uint8_t *data, size_t len);
#endif

/* Cross-module interfaces shared by the BLE ingress/egress modules. */

/* Shared wire format; neither ingress module owns channel/reassembly state. */
#define BLE_CHAN_FRAME_HDR_LEN 12
#define BLE_CHAN_ATT_PAYLOAD_MAX 497

extern struct bt_uuid_128 ble_common_service_uuid;
extern struct bt_uuid_128 ble_common_notify_uuid;
#define BT_UUID_BLE_COMMON_SVC (&ble_common_service_uuid.uuid)
#define BT_UUID_BLE_COMMON_NOT (&ble_common_notify_uuid.uuid)

/* Borrowed live connection, matching the original single-link lifetime model. */
struct bt_conn *ble_common_connection(void);

#ifdef __cplusplus
}
#endif

#endif
