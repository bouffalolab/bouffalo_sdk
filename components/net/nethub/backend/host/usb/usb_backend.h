#ifndef NH_USB_BACKEND_H
#define NH_USB_BACKEND_H

#include "nh_profile.h"

int nh_usb_backend_init(void);
nethub_route_result_t nh_usb_backend_output(nethub_frame_t *frame);
int nh_usb_backend_ctrl_upld_send(uint8_t *data_buff, uint32_t data_size);
int nh_usb_backend_ctrl_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg);

#endif
