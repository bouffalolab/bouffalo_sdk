/**
 * @file hal_camera.h
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
#ifndef __IMAGE_SENSOR_H__
#define __IMAGE_SENSOR_H__

#include "bl702_common.h"
#include "bl702_cam.h"
#include "bl702_mjpeg.h"

#define IMAGE_SENSOR_BF2013 0
#define IMAGE_SENSOR_GC0308 1
#define IMAGE_SENSOR_USE IMAGE_SENSOR_GC0308


#define ENDADDR                        (0x42030000)
#define CAMERA_RESOLUTION_X            640
#define CAMERA_RESOLUTION_Y            480
#define CAMERA_FRAME_SIZE              (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2)
#define MJPEG_READ_ADDR                0x42020800//0x42022000
#define MJPEG_READ_SIZE                2
#define YUV_USE                        1
#if(YUV_USE == 1)
#define CAMERA_BUFFER_SIZE_WHEN_MJPEG  (CAMERA_RESOLUTION_X * 2 * 8 * MJPEG_READ_SIZE)
#else
#define CAMERA_BUFFER_SIZE_WHEN_MJPEG  (CAMERA_RESOLUTION_X * 8 * MJPEG_READ_SIZE)
#endif
#define MJPEG_WRITE_ADDR               (MJPEG_READ_ADDR+CAMERA_BUFFER_SIZE_WHEN_MJPEG)
#define MJPEG_WRITE_SIZE               (ENDADDR - MJPEG_READ_ADDR -CAMERA_BUFFER_SIZE_WHEN_MJPEG)

extern CAM_CFG_Type cameraCfg;
extern MJPEG_CFG_Type mjpegCfg;

void Image_Sensor_PSRAM_Init(void);
BL_Err_Type Image_Sensor_Init(BL_Fun_Type mjpegEn, CAM_CFG_Type *camCfg, MJPEG_CFG_Type *mjpegCfg);
void Image_Sensor_Dump_Register(void);
BL_Err_Type Image_Sensor_CAM_Get_Interleave(uint8_t **pic, uint32_t *len);
BL_Err_Type Image_Sensor_CAM_Get_Planar(CAM_YUV_Mode_Type yuv, uint8_t **picYY, uint32_t *lenYY,
                                        uint8_t **picUV, uint32_t *lenUV);
uint8_t Image_Sensor_CAM_Frame_Count0(void);
uint8_t Image_Sensor_CAM_Frame_Count1(void);
void Image_Sensor_CAM_Release_Interleave(void);
void Image_Sensor_CAM_Release_Planar(void);
void Image_Sensor_CAM_Open(void);
void Image_Sensor_CAM_Close(void);
void Image_Sensor_CAM_Deinit(void);
BL_Err_Type Image_Sensor_MJPEG_Get(uint8_t **pic, uint32_t *len, uint8_t *q);
uint8_t Image_Sensor_MJPEG_Frame_Count(void);
void Image_Sensor_MJPEG_Release(void);
void Image_Sensor_MJPEG_Open(void);
void Image_Sensor_MJPEG_Close(void);
void Image_Sensor_MJPEG_Deinit(void);

void ClkOutInit(void);
BL_Err_Type Timer_Init(void);
void Test_H(void);
void Test_L(void);
void Timer_Match0_Cbf(void);

#endif

// #ifndef __HAL_CAM__H__
// #define __HAL_CAM__H__

// #include "drv_device.h"
// #include "bl702_config.h"

// #define DEVICE_CTRL_CONFIG_CAM_START        0x10
// #define DEVICE_CTRL_CONFIG_CAM_STOP         0x11

// typedef enum
// {
//     CAMERA_PLANAR_MODE,
//     CAMERA_INTERLEAVE_MODE,
// } camera_frame_mode_t;

// typedef enum
// {
//     CAMERA_YUV422,
//     CAMERA_YUV420_EVEN,
//     CAMERA_YUV420_ODD,
//     CAMERA_YUV400_EVEN,
//     CAMERA_YUV400_ODD,
// } camera_yuv_mode_t;

// typedef struct
// {
//     camera_frame_mode_t frame_mode;
//     camera_yuv_mode_t yuv_mode;
//     BL_Fun_Type mjpeg_enable;
//     uint8_t mjpegQ;
// } camera_pic_format_t;

// typedef enum
// {
//     CAMERA_LINE_ACTIVE_POLARITY_LOW,  /*!< CAMERA line active polarity low */
//     CAMERA_LINE_ACTIVE_POLARITY_HIGH, /*!< CAMERA line active polarity high */
// } camera_line_active_polarity;

// typedef enum
// {
//     CAMERA_FRAME_ACTIVE_POLARITY_LOW,  /*!< CAMERA frame active polarity low */
//     CAMERA_FRAME_ACTIVE_POLARITY_HIGH, /*!< CAMERA frame active polarity high */
// } camera_frame_active_polarity;

// typedef struct 
// {
//     uint8_t mclk;
//     uint8_t pclk;
//     uint8_t vsync;
//     uint8_t hsync;
//     uint8_t data0;
//     uint8_t data1;
//     uint8_t data2;
//     uint8_t data3;
//     uint8_t data4;
//     uint8_t data5;
//     uint8_t data6;
//     uint8_t data7;
//     BL_Fun_Type mclk_enable;
// } camera_pin_t;

// typedef struct
// {
//     uint8_t *base1;
//     uint8_t *addr1;
//     uint32_t length1;
//     uint8_t *base2;
//     uint8_t *addr2;
//     uint32_t length2;
// } camera_picture_t;

// typedef struct
// {
//     struct device parent;
//     uint8_t id;
//     camera_pin_t pin;
//     uint32_t pic_reso_x;
//     uint32_t pic_reso_y;
//     uint8_t *buffer;
//     uint32_t length;
//     camera_pic_format_t pic_format;
//     camera_line_active_polarity line_pol;
//     camera_frame_active_polarity frame_pol;
//     uint8_t mjpeg_quality;
//     camera_picture_t picture;
// } camera_device_t;

// void camera_register(camera_device_t *device, const char *name, uint16_t flag);
// int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

// extern camera_device_t camera_device;

// #endif