#ifndef NH_USB_BACKEND_H
#define NH_USB_BACKEND_H

#include "nh_profile.h"
#include <stdbool.h>

typedef int (*nh_usb_backend_acm_rx_cb_t)(void *arg, uint8_t *data_buff, uint32_t data_size);

int nh_usb_backend_init(void);
nethub_route_result_t nh_usb_backend_output(nethub_frame_t *frame);
int nh_usb_backend_acm_send(const uint8_t *data_buff, uint32_t data_size);
int nh_usb_backend_acm_recv_register(nh_usb_backend_acm_rx_cb_t recv_cb, void *cb_arg);
bool nh_usb_backend_is_idle(void);
int nh_usb_backend_lowpower_prepare(void);
int nh_usb_backend_lowpower_resume(void);

#endif
