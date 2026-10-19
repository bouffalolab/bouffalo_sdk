/**
  ******************************************************************************
  * @file    at_port_usb.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <at_main.h>

#include "netbus_usbd_cdc_acm.h"

static netbus_usb_cdc_t *g_usb_cdc = NULL;

int at_port_init(void)
{
    extern netbus_usb_cdc_t *netbus_usb_cdc_init(void);
    if (!g_usb_cdc) {
        g_usb_cdc = netbus_usb_cdc_init();
    }

    return 0;
}

int at_port_deinit(void)
{
    netbus_usb_cdcacm_deinit(g_usb_cdc);
    return 0;
}

int at_port_read_data(uint8_t *data, int len)
{
    int nBytes = 0;

    nBytes = netbus_usb_cdcacm_receive(g_usb_cdc, data, len, portMAX_DELAY);

#ifdef CONFIG_AT_PORT_USB_DUMP
    if (nBytes>0) {
        printf("---> dnld[%d] :", nBytes);
        for (int i = 0; i < nBytes; i++) {
            printf(" %02X", data[i]);
        }
        printf("\r\n");
    }
#endif
    if (nBytes <= 0) {
        return 0;
    }

    return nBytes;
}

#define AT_PORT_WRITE_TIMEOUT      (10000)// 10s
int at_port_write_data(uint8_t *data, int len)
{
    int msg_len;

#ifdef CONFIG_AT_PORT_USB_DUMP
    if (len) {
        printf("<--- upld[%d] : ", len);
        for (int i = 0; i < len; i++) {
            printf(" %02X", data[i]);
        }
        printf("\r\n");
    }
#endif
    msg_len = netbus_usb_cdcacm_send(g_usb_cdc, data, len, AT_PORT_WRITE_TIMEOUT);
    if (msg_len > 0) {
        return msg_len;
    }

    return 0;
}

int at_port_para_set(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control)
{
    return 0;
}

int at_port_para_get(int *baudrate, uint8_t *databits, uint8_t *stopbits, uint8_t *parity, uint8_t *flow_control)
{
    return 0;
}

__attribute__((weak)) int at_port_netmode_get()
{
    return 0;
}

int at_port_netmode_set(int mode)
{
    return 0;
}

static const at_device_ops usbdev_ops = {
    .init_device = at_port_init,
    .deinit_device = at_port_deinit,
    .read_data = at_port_read_data,
    .write_data = at_port_write_data,
    .read_zero_copy = NULL,
    .read_buffer_release = NULL,
    .schedule_work = NULL,
};

const at_device_ops *at_devops_get(void)
{
    return &usbdev_ops;
}
