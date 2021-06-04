/**
 * @file usbd_hid.c
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
#include "usbd_core.h"
#include "usbd_hid.h"

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

struct usbd_hid_cfg_private
{
   const uint8_t* hid_descriptor;
   const uint8_t* hid_report_descriptor;
   uint32_t hid_report_descriptor_len;
   uint32_t protocol;
   uint32_t idle_state;
   uint8_t hid_state;
} usbd_hid_cfg;

static void usbd_hid_reset(void)
{
    usbd_hid_cfg.idle_state = 0;
    usbd_hid_cfg.protocol = 2;
    usbd_hid_cfg.hid_state = HID_STATE_IDLE;
}

int hid_custom_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("Standard request:"
                 "bmRequestType 0x%02x, bRequest 0x%02x, len %d\r\n",
                 setup->bmRequestType, setup->bRequest, *len);

    if (REQTYPE_GET_DIR(setup->bmRequestType) == USB_REQUEST_DEVICE_TO_HOST &&
        setup->bRequest == USB_REQUEST_GET_DESCRIPTOR)
    {
        uint8_t value = (uint8_t)(setup->wValue >> 8);
        //uint8_t iface_num = (uint8_t)setup->wIndex;

        switch (value)
        {
        case HID_DESCRIPTOR_TYPE_HID:
            USBD_LOG("get HID Descriptor\r\n");
            *data = (uint8_t*)usbd_hid_cfg.hid_descriptor;
            *len = usbd_hid_cfg.hid_descriptor[0];
            break;
        case HID_DESCRIPTOR_TYPE_HID_REPORT:
            USBD_LOG("get Report Descriptor\r\n");
            *data = (uint8_t*)usbd_hid_cfg.hid_report_descriptor;
            *len = usbd_hid_cfg.hid_report_descriptor_len;
            break;
        case HID_DESCRIPTOR_TYPE_HID_PHYSICAL:
            USBD_LOG_DBG("get PHYSICAL Descriptor\r\n");

            break;            
        default:
            return -2;
        }

        return 0;
    }

    return -1;
}

int hid_class_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG("Class request:"
                 "bmRequestType 0x%02x bRequest 0x%02x,  len %d\r\n",
                 setup->bmRequestType,setup->bRequest, *len);

    switch (setup->bRequest)
    {
    case HID_REQUEST_GET_IDLE:
        *data = (uint8_t*)&usbd_hid_cfg.idle_state;
        *len = 1; 
        break;
    case HID_REQUEST_GET_PROTOCOL:
        *data = (uint8_t*)&usbd_hid_cfg.protocol;
        *len = 1; 
        break;
    case HID_REQUEST_SET_IDLE:
        usbd_hid_cfg.idle_state = setup->wValueH;
        break;
    case HID_REQUEST_SET_PROTOCOL:
        usbd_hid_cfg.protocol = setup->wValueL;
        break;
    default:
        USBD_LOG_ERR("Unhandled request 0x%02x\r\n", setup->bRequest);
        break;
    }

    return 0;
}

static void hid_notify_handler(uint8_t event, void* arg)
{
	switch (event)
	{
		case USB_EVENT_RESET:
			usbd_hid_reset();
			break;
		default:
			break;
	}	
}

void usbd_hid_reset_state(void)
{
    usbd_hid_cfg.hid_state = HID_STATE_IDLE;
}

void usbd_hid_send_report(uint8_t ep, uint8_t* data, uint8_t len)
{
    if(usbd_hid_cfg.hid_state == HID_STATE_IDLE)
    {
        usbd_hid_cfg.hid_state = HID_STATE_BUSY;
        usbd_ep_write(ep,data,len,NULL);
    }
}

void usbd_hid_descriptor_register(const uint8_t* desc)
{
    usbd_hid_cfg.hid_descriptor = desc;
}

void usbd_hid_report_descriptor_register(const uint8_t* desc, uint32_t desc_len)
{
    usbd_hid_cfg.hid_report_descriptor = desc;
    usbd_hid_cfg.hid_report_descriptor_len = desc_len;
}

void usbd_hid_add_interface(usbd_class_t *class, usbd_interface_t *intf)
{
	static usbd_class_t *last_class = NULL;

	if(last_class != class)
	{
		last_class = class;		
		usbd_class_register(class);
	}

	intf->class_handler = hid_class_request_handler;
	intf->custom_handler = hid_custom_request_handler;
	intf->vendor_handler = NULL;
	intf->notify_handler = hid_notify_handler;
	usbd_class_add_interface(class,intf);
}
