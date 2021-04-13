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

int hid_custom_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("Standard request:"
                 "bRequest 0x%02x, bmRequestType 0x%02x, len %d",
                 setup->bRequest, setup->bmRequestType, *len);

    if (REQTYPE_GET_DIR(setup->bmRequestType) == USB_REQUEST_DEVICE_TO_HOST &&
        setup->bRequest == USB_REQUEST_GET_DESCRIPTOR)
    {
        uint8_t value = (uint8_t)(setup->wValue >> 8);
        //uint8_t iface_num = (uint8_t)setup->wIndex;

        switch (value)
        {
        case USB_DESCRIPTOR_TYPE_HID:

            USBD_LOG_DBG("Return HID Descriptor");

            break;
        case USB_DESCRIPTOR_TYPE_HID_REPORT:
            USBD_LOG_DBG("Return Report Descriptor");

            break;
        case USB_DESCRIPTOR_TYPE_HID_PHYSICAL:
            USBD_LOG_DBG("Return PHYSICAL Descriptor");

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
    USBD_LOG_DBG("Class request:"
                 "bRequest 0x%02x, bmRequestType 0x%02x len %d",
                 setup->bRequest, setup->bmRequestType, *len);

    switch (setup->bRequest)
    {
    case HID_REQUEST_GET_IDLE:

        break;
    case HID_REQUEST_GET_REPORT:

        break;
    case HID_REQUEST_GET_PROTOCOL:

        break;
    case HID_REQUEST_SET_IDLE:

        break;
    case HID_REQUEST_SET_REPORT:

        break;
    case HID_REQUEST_SET_PROTOCOL:

        break;
    default:
        USBD_LOG_ERR("Unhandled request 0x%02x", setup->bRequest);
        break;
    }

    return 0;
}
