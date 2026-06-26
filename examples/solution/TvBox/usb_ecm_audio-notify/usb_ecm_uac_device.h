#ifndef USB_ECM_UAC_DEVICE_H
#define USB_ECM_UAC_DEVICE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void usb_ecm_uac_device_start(void);
void usb_ecm_uac_device_stop(void);
int usb_ecm_uac_hid_send_report(const uint8_t report[8]);
int usb_ecm_uac_hid_send_key(uint8_t key_code);
void usb_ecm_uac_hid_get_status(uint8_t *ready, uint8_t *busy, uint8_t *suspend, uint8_t *configured);

#ifdef __cplusplus
}
#endif

#endif
