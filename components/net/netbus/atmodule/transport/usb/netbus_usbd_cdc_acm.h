#ifndef __NETBUS_USB_CDCECM_H__
#define __NETBUS_USB_CDCECM_H__

#include <stdio.h>
#include <stdint.h>
#include <utils_list.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stream_buffer.h>
#include <event_groups.h>
#include <stdint.h>

#define CDC_ACM_IN_EP      0x83
#define CDC_ACM_OUT_EP     0x04
#define CDC_ACM_INT_EP     0x86

#define CDC_MAX_MPS 512

typedef void (*netbus_usb_cdc_func_t)(void *arg, int event);

typedef struct netbus_usb_cdc {
    StreamBufferHandle_t rxbuf;
    SemaphoreHandle_t w_mutex;
    EventGroupHandle_t event;

    netbus_usb_cdc_func_t cb;
    void *arg;
} netbus_usb_cdc_t;

int netbus_usb_cdcacm_init(netbus_usb_cdc_t *pctx, uint32_t txbuf_size, uint32_t rxbuf_size);

int netbus_usb_cdcacm_callback_set(netbus_usb_cdc_t *pctx, netbus_usb_cdc_func_t pfunc, void *arg);

int netbus_usb_cdcacm_send(netbus_usb_cdc_t *pctx, const uint8_t *p_data, uint32_t len, uint32_t timeout);
int netbus_usb_cdcacm_receive(netbus_usb_cdc_t *pctx, uint8_t *p_buffer, uint32_t buf_len, uint32_t timeout);
int netbus_usb_cdcacm_deinit(netbus_usb_cdc_t *ctx);

#endif



