#ifndef USB_ECM_BLE_H
#define USB_ECM_BLE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    USB_ECM_BLE_CHANNEL_VOICE = 1,
    USB_ECM_BLE_CHANNEL_KEY = 2,
    USB_ECM_BLE_CHANNEL_USER = 3,
    USB_ECM_BLE_CHANNEL_MAX = USB_ECM_BLE_CHANNEL_USER,
} usb_ecm_ble_channel_t;

/* Per-channel received-frame callback. Register the same handler on every
 * channel to receive all traffic, or different handlers per channel. */
typedef void (*usb_ecm_ble_rx_cb_t)(usb_ecm_ble_channel_t channel,
                                    const uint8_t *data,
                                    size_t len);

int usb_ecm_ble_stack_init(void);
void usb_ecm_ble_set_channel_data_callback(usb_ecm_ble_channel_t channel, usb_ecm_ble_rx_cb_t cb);
int usb_ecm_ble_send_channel(usb_ecm_ble_channel_t channel, const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
