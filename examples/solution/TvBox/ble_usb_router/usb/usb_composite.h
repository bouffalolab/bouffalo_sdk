#ifndef USB_COMPOSITE_H
#define USB_COMPOSITE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void usb_composite_start(void);
void usb_composite_stop(void);
int usb_composite_hid_send_report(const uint8_t report[8]);

#ifdef __cplusplus
}
#endif

#endif
