/**
 * @file usb_dc.c
 * @brief 
 * 
 * Copyright (c) 2021 Bouffalolab team
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 * 
 */

#include "hal_usb.h"
#include "stdbool.h"
#include "usbd_core.h"

struct device *usb;

static void usb_dc_event_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    switch (state)
    {
    case USB_DC_EVENT_ERROR:
        usbd_event_notify_handler(USB_EVENT_ERROR,NULL);
        break;
    case USB_DC_EVENT_RESET:
        usbd_event_notify_handler(USB_EVENT_RESET,NULL);
        break;
    case USB_DC_EVENT_SOF:
        usbd_event_notify_handler(USB_EVENT_SOF,NULL);
        break;
    case USB_DC_EVENT_SETUP_NOTIFY:
        usbd_event_notify_handler(USB_EVENT_SETUP_NOTIFY,NULL);
        break;
    case USB_DC_EVENT_EP0_IN_NOTIFY:
        usbd_event_notify_handler(USB_EVENT_EP0_IN_NOTIFY,NULL);
        break;
    case USB_DC_EVENT_EP0_OUT_NOTIFY:
        usbd_event_notify_handler(USB_EVENT_EP0_OUT_NOTIFY,NULL);
        break;
    case USB_DC_EVENT_EP_IN_NOTIFY:
        usbd_event_notify_handler(USB_EVENT_EP_IN_NOTIFY,args);
        break;
    case USB_DC_EVENT_EP_OUT_NOTIFY:
        usbd_event_notify_handler(USB_EVENT_EP_OUT_NOTIFY,args);
        break; 
    default:
        break;
    }
}
struct device* usb_dc_init(void)
{
    usb_dc_register(USB_INDEX, "usb", DEVICE_OFLAG_RDWR);
    usb = device_find("usb");
    device_set_callback(usb,usb_dc_event_callback);
    device_open(usb, 0);
    return usb;
}

int usbd_set_address(const uint8_t addr)
{
    return device_control(usb,DEVICE_CTRL_USB_DC_SET_ADDR,(void*)(uint32_t)addr);
}

int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg)
{
    return usb_dc_ep_open(usb,(const struct usb_dc_ep_cfg*)ep_cfg);
}
int usbd_ep_close(const uint8_t ep)
{
    device_control(usb,DEVICE_CTRL_USB_DC_SET_NACK,(void*)(uint32_t)ep);
    return 0;
}
int usbd_ep_set_stall(const uint8_t ep)
{
    return device_control(usb,DEVICE_CTRL_USB_DC_SET_STALL,(void*)(uint32_t)ep);
}
int usbd_ep_clear_stall(const uint8_t ep)
{
    return device_control(usb,DEVICE_CTRL_USB_DC_CLR_STALL,(void*)(uint32_t)ep);
}
int usbd_ep_is_stalled(const uint8_t ep, uint8_t *stalled)
{
    return usb_dc_ep_is_stalled(usb,ep,stalled);
}

int usbd_ep_write(const uint8_t ep, const uint8_t *data, uint32_t data_len, uint32_t *ret_bytes)
{
    return usb_dc_ep_write(usb,ep,data,data_len,ret_bytes);
}

int usbd_ep_read(const uint8_t ep, uint8_t *data, uint32_t max_data_len,uint32_t *read_bytes)
{
    return usb_dc_ep_read(usb,ep,data,max_data_len,read_bytes);
}
