/**
 * @file hal_cam.h
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

#ifndef __HAL_CAM_H__
#define __HAL_CAM_H__

#include "drv_device.h"
#include "bl702_cam.h"
#include "bl702_config.h"

typedef enum
{
    CAM_AUTO_MODE,
    CAM_MANUAL_MODE,
} cam_software_mode_t;

typedef enum
{
    CAM_FRAME_PLANAR_MODE,
    CAM_FRAME_INTERLEAVE_MODE,
} cam_frame_mode_t;

typedef enum
{
    CAM_YUV_FORMAT_YUV422,
    CAM_YUV_FORMAT_YUV420_EVEN,
    CAM_YUV_FORMAT_YUV420_ODD,
    CAM_YUV_FORMAT_YUV400_EVEN,
    CAM_YUV_FORMAT_YUV400_ODD,
} cam_yuv_format_t;

typedef struct cam_device
{
    struct device parent;
    cam_software_mode_t software_mode;
    cam_frame_mode_t    frame_mode;
    cam_yuv_format_t    yuv_format;
    uint32_t            cam_write_ram_addr;
    uint32_t            cam_write_ram_size;
    uint32_t            cam_frame_size;

    // planar mode need use:
    uint32_t            cam_write_ram_addr1;          
    uint32_t            cam_write_ram_size1;
    uint32_t            cam_frame_size1;
} cam_device_t;

void cam_init(cam_device_t *cam_cfg);
void cam_start(void);
void cam_stop(void);
uint8_t cam_get_one_frame_interleave(uint8_t **pic, uint32_t *len);
uint8_t cam_get_one_frame_planar(CAM_YUV_Mode_Type yuv, uint8_t **picYY, uint32_t *lenYY, uint8_t **picUV, uint32_t *lenUV);
void cam_drop_one_frame_interleave(void);
void cam_drop_one_frame_planar(void);
void cam_hsync_crop(uint16_t start, uint16_t end);
void cam_vsync_crop(uint16_t start, uint16_t end);

#endif
