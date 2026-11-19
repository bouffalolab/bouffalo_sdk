#ifndef APP_USB_COMPOSITE_H
#define APP_USB_COMPOSITE_H

#include <stdint.h>

int app_usb_composite_configure(void);
int app_usb_composite_start(void);

/*
 * Video DATA ACM. This interface is application-specific and stays out of the
 * NetHub backend: the app owns the endpoint and drives OUT transfers directly.
 * out_done_cb fires (ISR context) when an armed OUT read completes; event_cb fires on USB
 * (re)configuration so the reader can (re)start its framing.
 */
typedef void (*app_usb_video_out_done_cb_t)(void *arg, uint32_t len);
typedef void (*app_usb_video_event_cb_t)(void *arg, int configured);

void app_usb_video_register_callbacks(app_usb_video_out_done_cb_t out_done_cb, app_usb_video_event_cb_t event_cb,
                                      void *cb_arg);
int app_usb_video_start_out_read(uint8_t *data, uint32_t len);

#endif /* APP_USB_COMPOSITE_H */
