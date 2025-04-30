/*
 * Copyright (c) 2023, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_cdc_ecm.h"

#define CDC_ECM_OUT_EP_IDX 0
#define CDC_ECM_IN_EP_IDX  1
#define CDC_ECM_INT_EP_IDX 2

/* Describe EndPoints configuration */
static struct usbd_endpoint cdc_ecm_ep_data[3];

#ifdef CONFIG_USB_HS
#define CDC_ECM_MAX_PACKET_SIZE 512
#else
#define CDC_ECM_MAX_PACKET_SIZE 64
#endif

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_cdc_ecm_notify_buf[16];

static volatile uint8_t g_cmd_intf = 0;
static volatile uint8_t g_int_ep_busy = 0;

static uint32_t g_connect_speed_table[2] = { CDC_ECM_CONNECT_SPEED_UPSTREAM,
                                             CDC_ECM_CONNECT_SPEED_DOWNSTREAM };

void usbd_cdc_ecm_send_notify(uint8_t notifycode, uint8_t value, uint32_t *speed)
{
    struct cdc_eth_notification *notify = (struct cdc_eth_notification *)g_cdc_ecm_notify_buf;
    uint8_t bytes2send = 0;

    notify->bmRequestType = CDC_ECM_BMREQUEST_TYPE_ECM;
    notify->bNotificationType = notifycode;

    switch (notifycode) {
        case CDC_ECM_NOTIFY_CODE_NETWORK_CONNECTION:
            notify->wValue = value;
            notify->wIndex = g_cmd_intf;
            notify->wLength = 0U;

            for (uint8_t i = 0U; i < 8U; i++) {
                notify->data[i] = 0U;
            }
            bytes2send = 8U;
            break;
        case CDC_ECM_NOTIFY_CODE_RESPONSE_AVAILABLE:
            notify->wValue = 0U;
            notify->wIndex = g_cmd_intf;
            notify->wLength = 0U;
            for (uint8_t i = 0U; i < 8U; i++) {
                notify->data[i] = 0U;
            }
            bytes2send = 8U;
            break;
        case CDC_ECM_NOTIFY_CODE_CONNECTION_SPEED_CHANGE:
            notify->wValue = 0U;
            notify->wIndex = g_cmd_intf;
            notify->wLength = 0x0008U;
            bytes2send = 16U;

            memcpy(notify->data, speed, 8);
            break;

        default:
            break;
    }

    if (bytes2send) {
        g_int_ep_busy = 1;
        usbd_ep_start_write(cdc_ecm_ep_data[CDC_ECM_INT_EP_IDX].ep_addr, g_cdc_ecm_notify_buf, bytes2send);
    }
}

static int cdc_ecm_class_interface_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USB_LOG_DBG("CDC ECM Class request: "
                "bRequest 0x%02x\r\n",
                setup->bRequest);

    g_cmd_intf = LO_BYTE(setup->wIndex);

    switch (setup->bRequest) {
        case CDC_REQUEST_SET_ETHERNET_PACKET_FILTER:
            /* bit0 Promiscuous 
             * bit1 ALL Multicast
             * bit2 Directed
             * bit3 Broadcast
             * bit4 Multicast
            */
            break;
        default:
            USB_LOG_WRN("Unhandled CDC ECM Class bRequest 0x%02x\r\n", setup->bRequest);
            return -1;
    }

    return 0;
}

void cdc_ecm_notify_handler(uint8_t event, void *arg)
{
    switch (event) {
        case USBD_EVENT_RESET:
            g_int_ep_busy = 0;
            break;
        case USBD_EVENT_CONFIGURED:
            break;

        default:
            break;
    }
}

void cdc_ecm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    usbd_cdc_ecm_packet_recv_done(nbytes);
}

void cdc_ecm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    if ((nbytes % CDC_ECM_MAX_PACKET_SIZE) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(ep, NULL, 0);
    } else {
        // g_cdc_ecm_tx_data_length = 0;
        usbd_cdc_ecm_packet_send_done(nbytes);
    }
}

void cdc_ecm_int_in(uint8_t ep, uint32_t nbytes)
{
    g_int_ep_busy = 0;
}

void usbd_cdc_ecm_send_connect_status(bool connected)
{
    while (g_int_ep_busy) {
    }

    if (connected) {
        usbd_cdc_ecm_send_notify(CDC_ECM_NOTIFY_CODE_NETWORK_CONNECTION, CDC_ECM_NET_CONNECTED, NULL);
    } else {
        usbd_cdc_ecm_send_notify(CDC_ECM_NOTIFY_CODE_NETWORK_CONNECTION, CDC_ECM_NET_DISCONNECTED, NULL);
    }
}

int usbd_cdc_ecm_send_packet(uint8_t *buf, uint32_t len)
{
    USB_LOG_DBG("txlen:%d\r\n", len);
    return usbd_ep_start_write(cdc_ecm_ep_data[CDC_ECM_IN_EP_IDX].ep_addr, buf, len);
}

int usbd_cdc_ecm_receive_packet(uint8_t *buf, uint32_t len)
{
    return usbd_ep_start_read(cdc_ecm_ep_data[CDC_ECM_OUT_EP_IDX].ep_addr, buf, len);
}

struct usbd_interface *usbd_cdc_ecm_init_intf(struct usbd_interface *intf, const uint8_t int_ep, const uint8_t out_ep, const uint8_t in_ep)
{
    intf->class_interface_handler = cdc_ecm_class_interface_request_handler;
    intf->class_endpoint_handler = NULL;
    intf->vendor_handler = NULL;
    intf->notify_handler = cdc_ecm_notify_handler;

    cdc_ecm_ep_data[CDC_ECM_OUT_EP_IDX].ep_addr = out_ep;
    cdc_ecm_ep_data[CDC_ECM_OUT_EP_IDX].ep_cb = cdc_ecm_bulk_out;
    cdc_ecm_ep_data[CDC_ECM_IN_EP_IDX].ep_addr = in_ep;
    cdc_ecm_ep_data[CDC_ECM_IN_EP_IDX].ep_cb = cdc_ecm_bulk_in;
    cdc_ecm_ep_data[CDC_ECM_INT_EP_IDX].ep_addr = int_ep;
    cdc_ecm_ep_data[CDC_ECM_INT_EP_IDX].ep_cb = cdc_ecm_int_in;

    usbd_add_endpoint(&cdc_ecm_ep_data[CDC_ECM_OUT_EP_IDX]);
    usbd_add_endpoint(&cdc_ecm_ep_data[CDC_ECM_IN_EP_IDX]);
    usbd_add_endpoint(&cdc_ecm_ep_data[CDC_ECM_INT_EP_IDX]);

    return intf;
}

void usbd_cdc_ecm_set_connect_speed(uint32_t speed[2])
{
    memcpy(g_connect_speed_table, speed, 8);

    while (g_int_ep_busy) {
    }
    usbd_cdc_ecm_send_notify(CDC_ECM_NOTIFY_CODE_CONNECTION_SPEED_CHANGE, NULL, g_connect_speed_table);
}

__WEAK void usbd_cdc_ecm_packet_recv_done(uint32_t nbytes)
{
}

__WEAK void usbd_cdc_ecm_packet_send_done(uint32_t nbytes)
{
}