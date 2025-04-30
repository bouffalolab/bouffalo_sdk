/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBD_CDC_ECM_H
#define USBD_CDC_ECM_H

#include "usb_cdc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Ethernet Maximum Segment size, typically 1514 bytes */
#define CONFIG_CDC_ECM_ETH_MAX_SEGSZE 1514U
// #define CONFIG_USBDEV_CDC_ECM_USING_LWIP

/* Init cdc ecm interface driver */
struct usbd_interface *usbd_cdc_ecm_init_intf(struct usbd_interface *intf, const uint8_t int_ep, const uint8_t out_ep, const uint8_t in_ep);

/* Setup request command callback api */
void usbd_cdc_ecm_set_connect_speed(uint32_t speed[2]);
void usbd_cdc_ecm_send_connect_status(bool connected);

int usbd_cdc_ecm_send_packet(uint8_t *buf, uint32_t len);
int usbd_cdc_ecm_receive_packet(uint8_t *buf, uint32_t len);

void usbd_cdc_ecm_packet_send_done(uint32_t nbytes);
void usbd_cdc_ecm_packet_recv_done(uint32_t nbytes);

#ifdef __cplusplus
}
#endif

#endif /* USBD_CDC_ECM_H */
