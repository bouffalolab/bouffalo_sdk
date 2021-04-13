/**
 * @file usbd_video.c
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
#include "usbd_video.h"

#define WIDTH                                         (unsigned int)320
#define HEIGHT                                        (unsigned int)240
#define VIDEO_PACKET_SIZE                             (unsigned int)(768+2)

#define CAM_FPS                                       20
#define INTERVAL                                      (unsigned long)(10000000/CAM_FPS)
#define MIN_BIT_RATE                                  (unsigned long)(WIDTH*HEIGHT*16*CAM_FPS)//16 bit
#define MAX_BIT_RATE                                  (unsigned long)(WIDTH*HEIGHT*16*CAM_FPS)
#define MAX_FRAME_SIZE                                (unsigned long)(WIDTH * HEIGHT *2)

struct video_probe_and_commit_controls probe =
{
    .hintUnion.bmHint = 0,
    .hintUnion1.bmHint = 0,
    .bFormatIndex = 1,
    .bFrameIndex = 1,
    .dwFrameInterval = INTERVAL,
    .wKeyFrameRate = 0,
    .wPFrameRate = 0,
    .wCompQuality = 0,
    .wCompWindowSize = 0,
    .wDelay = 0,
    .dwMaxVideoFrameSize = MAX_FRAME_SIZE,
    .dwMaxPayloadTransferSize = 0,
    .dwClockFrequency = 0,
    .bmFramingInfo = 0,
    .bPreferedVersion = 0,
    .bMinVersion = 0,
    .bMaxVersion = 0,
};
struct video_probe_and_commit_controls commit =
{
    .hintUnion.bmHint = 0,
    .hintUnion1.bmHint = 0,
    .bFormatIndex = 1,
    .bFrameIndex = 1,
    .dwFrameInterval = INTERVAL,
    .wKeyFrameRate = 0,
    .wPFrameRate = 0,
    .wCompQuality = 0,
    .wCompWindowSize = 0,
    .wDelay = 0,
    .dwMaxVideoFrameSize = MAX_FRAME_SIZE,
    .dwMaxPayloadTransferSize = 0,
    .dwClockFrequency = 0,
    .bmFramingInfo = 0,
    .bPreferedVersion = 0,
    .bMinVersion = 0,
    .bMaxVersion = 0,
};

int video_class_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("Class request:"
                 "bRequest 0x%02x, bmRequestType 0x%02x len %d",
                 setup->bRequest, setup->bmRequestType, *len);

    switch (setup->bRequest)
    {
        case VIDEO_REQUEST_SET_CUR:
            if (setup->wValue == 256)
                memcpy((uint8_t *)&probe,*data,setup->wLength);
            else if (setup->wValue == 512)
                memcpy((uint8_t *)&commit,*data,setup->wLength);
            break;
        case VIDEO_REQUEST_GET_CUR:
            if (setup->wValue == 256)
                *data = (uint8_t *)&probe;
            else if (setup->wValue == 512)
                *data = (uint8_t *)&commit;
            break;
        case VIDEO_REQUEST_GET_MIN:
            if (setup->wValue == 256)
                *data = (uint8_t *)&probe;
            else if (setup->wValue == 512)
                *data = (uint8_t *)&commit;
            break;
        case VIDEO_REQUEST_GET_MAX:
            if (setup->wValue == 256)
                *data = (uint8_t *)&probe;
            else if (setup->wValue == 512)
                *data = (uint8_t *)&commit;
            break;
        case VIDEO_REQUEST_GET_RES:

            break;
        case VIDEO_REQUEST_GET_LEN:

            break;
        case VIDEO_REQUEST_GET_INFO:

            break;
        case VIDEO_REQUEST_GET_DEF:
            if (setup->wLength == 256)
                *data = (uint8_t *)&probe;
            else if (setup->wLength == 512)
                *data = (uint8_t *)&commit;
            break;
        default:
            USBD_LOG_ERR("Unhandled request 0x%02x", setup->bRequest);
            break;
    }
    return 0;
}
uint8_t play_status = 0;
void video_notify_handler(uint8_t event, void* arg)
{
	switch (event)
	{
		case USB_EVENT_RESET:
			//usbd_cdc_acm_reset();
			break;
		case USB_EVENT_SET_INTERFACE:
			if(((uint8_t*)arg)[3])
            {
                play_status =1;
                MSG("Y\r\n");
            }
            else
            {
                play_status=0;
                MSG("N\r\n");
            }
			break;		
		default:
			break;
	}	
}

__weak void usbd_video_isoch_out(uint8_t ep)
{

}
__weak void usbd_video_isoch_in(uint8_t ep)
{

}



static usbd_class_t video_class;

usbd_interface_t video_stream_intf =
{
	.class_handler = video_class_request_handler,
	.vendor_handler = NULL,
	.notify_handler = video_notify_handler,
};

static usbd_endpoint_t video_in_ep = 
{
	.ep_cb = usbd_video_isoch_in,
	.ep_addr = 0x81
};

void usbd_video_class_init(const char *name,uint8_t in_ep)
{
    video_class.name = name;

	usbd_class_register(&video_class);
	usbd_class_add_interface(&video_class,&video_stream_intf);
    video_stream_intf.intf_num = 1;
    video_in_ep.ep_addr = in_ep;
 	usbd_interface_add_endpoint(&video_stream_intf,&video_in_ep);

}