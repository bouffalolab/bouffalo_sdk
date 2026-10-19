#ifndef NETHUB_USB_DEVICE_H
#define NETHUB_USB_DEVICE_H

#include "nethub_defs.h"

#include <stdint.h>

typedef struct {
    int (*start_out_read)(uint8_t *data, uint32_t len);
    int (*start_in_write)(uint8_t *data, uint32_t len);
    int (*write_interrupt_in)(uint8_t *data, uint32_t len);
} nethub_usb_device_cdc_ops_t;

typedef struct {
    int (*init)(void);
    int (*deinit)(void);
} nethub_usb_device_ops_t;

typedef struct {
    void (*in_done_cb)(uint32_t len);
    void (*out_done_cb)(uint32_t len);
    void (*event_cb)(uint8_t event);
} nethub_usb_device_cdc_cbs_t;

/*
 * NetHub owns only the ECM (network) and CMD ACM (control) channels. The
 * application owns descriptors, strings, interface/endpoint registration,
 * usbd_desc_register(), usbd_initialize(), endpoint I/O, and the device
 * lifecycle callbacks; it wires each channel's CDC I/O callbacks in via
 * these init calls and drives any extra application-specific interfaces
 * itself.
 */
int nethub_usb_device_init(const nethub_usb_device_ops_t *ops);
int nethub_usb_device_cdc_ecm_init(const nethub_usb_device_cdc_ops_t *ops, nethub_usb_device_cdc_cbs_t *cbs,
                                   uint32_t link_speed_bps);
int nethub_usb_device_cdc_acm_cmd_init(const nethub_usb_device_cdc_ops_t *ops, nethub_usb_device_cdc_cbs_t *cbs);
const char *nethub_usb_device_ecm_mac_string(void);

#endif /* NETHUB_USB_DEVICE_H */
