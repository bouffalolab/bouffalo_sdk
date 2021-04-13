/**
 * @file hal_camera.c
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
#include "hal_camera.h"
#include "bl702_i2c.h"
#include "bl702_gpio.h"
#include "bl702_glb.h"
#include "bl702_psram.h"
#include "bl702_l1c.h"
#include "bl702_timer.h"

/** @addtogroup  Image_Sensor_Driver
 *  @{
 */

/** @addtogroup  IMAGE_SENSOR
 *  @{
 */

/** @defgroup  IMAGE_SENSOR_Private_Macros
 *  @{
 */
#define BFLB_EXTPSRAM_CLK_GPIO GLB_GPIO_PIN_27
#define BFLB_EXTPSRAM_CS_GPIO GLB_GPIO_PIN_17
#define BFLB_EXTPSRAM_DATA0_GPIO GLB_GPIO_PIN_28
#define BFLB_EXTPSRAM_DATA1_GPIO GLB_GPIO_PIN_24
#define BFLB_EXTPSRAM_DATA2_GPIO GLB_GPIO_PIN_23
#define BFLB_EXTPSRAM_DATA3_GPIO GLB_GPIO_PIN_26
#if (IMAGE_SENSOR_USE == IMAGE_SENSOR_BF2013)
#define I2C_CAMERA_ADDR 0x6E
#define BF2013_ID_MSB 0xFC
#define BF2013_ID_LSB 0xFD
#elif (IMAGE_SENSOR_USE == IMAGE_SENSOR_GC0308)
#define I2C_CAMERA_ADDR 0x21
#define GC0308_ID 0x00
#endif

void ClkOutInit(void)
{
    uint8_t CLKPins[] = {GLB_GPIO_PIN_9};
    GLB_Set_I2S_CLK(ENABLE, GLB_I2S_OUT_REF_CLK_NONE);
    PDS_Set_Audio_PLL_Freq(AUDIO_PLL_24576000_HZ);
    GLB_GPIO_Func_Init(0, CLKPins, sizeof(CLKPins) / sizeof(CLKPins[0]));
    GLB_Set_Chip_Out_1_CLK_Sel(GLB_CHIP_CLK_OUT_I2S_REF_CLK);
}

CAM_CFG_Type cameraCfg =
    {
        .swMode = CAM_SW_MODE_AUTO,
        .swIntCnt = 0,
        .frameMode = CAM_INTERLEAVE_MODE,
#if (YUV_USE == 1)
        .yuvMode = CAM_YUV422,
#else
        .yuvMode = CAM_YUV400_ODD,
#endif
        .linePol = CAM_LINE_ACTIVE_POLARITY_HIGH,
        .framePol = CAM_FRAME_ACTIVE_POLARITY_HIGH,
        .camSensorMode = CAM_SENSOR_MODE_V_AND_H,
        .burstType = CAM_BURST_TYPE_INCR16,
        .waitCount = 0x40,
        .memStart0 = MJPEG_READ_ADDR,
        .memSize0 = CAMERA_BUFFER_SIZE_WHEN_MJPEG,
        .frameSize0 = CAMERA_FRAME_SIZE,
        .memStart1 = 0,
        .memSize1 = 0,
        .frameSize1 = 0,
};

MJPEG_CFG_Type mjpegCfg =
    {
        .burst = MJPEG_BURST_INCR16,
        .quality = 10,
#if (YUV_USE == 1)
        .yuv = MJPEG_YUV422_INTERLEAVE,
#else
        .yuv = MJPEG_YUV400,
#endif
        .waitCount = 0x400,
        .bufferMjpeg = MJPEG_WRITE_ADDR,
        .sizeMjpeg = MJPEG_WRITE_SIZE,
        .bufferCamYY = MJPEG_READ_ADDR,
        .sizeCamYY = MJPEG_READ_SIZE,
        .bufferCamUV = 0,
        .sizeCamUV = 0,
        .resolutionX = CAMERA_RESOLUTION_X,
        .resolutionY = CAMERA_RESOLUTION_Y,
        .bitOrderEnable = ENABLE,
        .evenOrderEnable = ENABLE,
        .swapModeEnable = DISABLE,
        .overStopEnable = ENABLE,
        .reflectDmy = DISABLE,
        .verticalDmy = DISABLE,
        .horizationalDmy = DISABLE,
};
GLB_GPIO_Cfg_Type gpio_cfg0 = {
    .gpioPin = GLB_GPIO_PIN_22,
    .gpioFun = 11,
    .gpioMode = GPIO_MODE_OUTPUT,
    .pullType = GPIO_PULL_NONE,
    .drive = 0,
    .smtCtrl = 1};
TIMER_CFG_Type timerCh0Cfg = {
    TIMER_CH0,                /* timer channel 0 */
    TIMER_CLKSRC_FCLK,        /* timer clock source:bus clock */
    TIMER_PRELOAD_TRIG_COMP2, /* reaload on comaparator 2  */
    TIMER_COUNT_PRELOAD,      /* preload when match occur */
    0,                        /* clock division value */
    230000,                   /* match value 0  */
    260000,                   /* match value 1 */
    14400000,                   /* match value 2 */
    0,                   /* preload value */
};
void Test_H(void)
{
    GLB_GPIO_Write(gpio_cfg0.gpioPin, 1);
}
void Test_L(void)
{
    GLB_GPIO_Write(gpio_cfg0.gpioPin, 0);
}
BL_Err_Type Timer_Init(void)
{
    // TIMER_IntMask(timerCh0Cfg.timerCh, TIMER_INT_ALL, MASK);
    // TIMER_Disable(timerCh0Cfg.timerCh);
    // TIMER_Init(&timerCh0Cfg);
    // TIMER_ClearIntStatus(timerCh0Cfg.timerCh, TIMER_COMP_ID_0);
    // TIMER_ClearIntStatus(timerCh0Cfg.timerCh, TIMER_COMP_ID_1);
    // TIMER_ClearIntStatus(timerCh0Cfg.timerCh, TIMER_COMP_ID_2);
    // TIMER_IntMask(timerCh0Cfg.timerCh, TIMER_INT_COMP_0, UNMASK);
    // Timer_Int_Callback_Install(timerCh0Cfg.timerCh, TIMER_INT_COMP_0, &Timer_Match0_Cbf);
    // NVIC_ClearPendingIRQ((IRQn_Type)(timerCh0Cfg.timerCh + TIMER_CH0_IRQn));
    // NVIC_EnableIRQ((IRQn_Type)(timerCh0Cfg.timerCh + TIMER_CH0_IRQn));
    // TIMER_Enable(timerCh0Cfg.timerCh);

    GLB_GPIO_Write(gpio_cfg0.gpioPin, 0);
    GLB_GPIO_Init(&gpio_cfg0);
    GLB_GPIO_OUTPUT_Enable(gpio_cfg0.gpioPin);

    return SUCCESS;
}

static const uint8_t sensorRegList[][2] =
    {
#if (IMAGE_SENSOR_USE == IMAGE_SENSOR_BF2013)
        {0x12, 0x80},
        {0x67, 0x00},
        {0x68, 0x00},
        //{0xb9, 0x80},//use Test pattern
        //{0x69,0x20},
        {0x3a, 0x02},
        {0x09, 0x01},
        {0x15, 0x02},
        {0x12, 0x00},
        {0x1e, 0x00},
        {0x13, 0x00},
        {0x01, 0x14},
        {0x02, 0x21},
        {0x8c, 0x02},
        {0x8d, 0x64},
        {0x87, 0x18},
        {0x13, 0x07},
        //{0x11,0x80},//pclk=mclk
        {0x11, 0x30}, //pclk=mclk/8
        {0x2b, 0x20},
        {0x92, 0x40},
        {0x9d, 0x99},
        {0x06, 0xe0},
        {0x29, 0x54},
        {0xeb, 0x30},
        {0xbb, 0x20},
        {0xf5, 0x21},
        {0xe1, 0x3c},
        {0x16, 0x01},
        {0xe0, 0x0b},
        {0x2f, 0xf6},
        {0x33, 0x20},
        {0x34, 0x08},
        {0x35, 0x50},
        {0x65, 0x4a},
        {0x66, 0x50},
        {0x36, 0x05},
        {0x37, 0xf6},
        {0x38, 0x46},
        {0x9b, 0xf6},
        {0x9c, 0x46},
        {0xbc, 0x01},
        {0xbd, 0xf6},
        {0xbe, 0x46},
        {0x82, 0x14},
        {0x83, 0x23},
        {0x9a, 0x23},
        {0x70, 0x6f},
        {0x72, 0x3f},
        {0x73, 0x3f},
        {0x74, 0x27},
        {0x77, 0x90},
        {0x79, 0x48},
        {0x7a, 0x1e},
        {0x7b, 0x30},
        {0x84, 0x1a},
        {0x85, 0x20},
        {0x89, 0x02},
        {0x8a, 0x64},
        {0x86, 0x30},
        {0x96, 0xa6},
        {0x97, 0x0c},
        {0x98, 0x18},
        {0x80, 0x55},
        {0x24, 0x70},
        {0x25, 0x80},
        {0x94, 0x0a},
        {0x1f, 0x20},
        {0x22, 0x20},
        {0x26, 0x20},
        {0x56, 0x40},
        {0x61, 0xd3},
        {0x79, 0x48},
        {0x3b, 0x60},
        {0x3c, 0x20},
        {0x39, 0x80},
        {0x3f, 0xb0},
        {0x39, 0x80},
        {0x40, 0x58},
        {0x41, 0x54},
        {0x42, 0x4e},
        {0x43, 0x44},
        {0x44, 0x3e},
        {0x45, 0x39},
        {0x46, 0x35},
        {0x47, 0x31},
        {0x48, 0x2e},
        {0x49, 0x2b},
        {0x4b, 0x29},
        {0x4c, 0x27},
        {0x4e, 0x23},
        {0x4f, 0x20},
        {0x50, 0x1e},
        {0x51, 0x05},
        {0x52, 0x10},
        {0x53, 0x0b},
        {0x54, 0x15},
        {0x57, 0x87},
        {0x58, 0x72},
        {0x59, 0x5f},
        {0x5a, 0x7e},
        {0x5b, 0x1f},
        {0x5c, 0x0e},
        {0x5d, 0x95},
        {0x60, 0x28},
        {0xb0, 0xe0},
        {0xb1, 0xc0},
        {0xb2, 0xb0},
        {0xb3, 0x88},
        {0x6a, 0x01},
        {0x23, 0x66},
        {0xa0, 0x03},
        {0xa1, 0x31},
        {0xa2, 0x0b},
        {0xa3, 0x26},
        {0xa4, 0x05},
        {0xa5, 0x25},
        {0xa6, 0x06},
        {0xa7, 0x80},
        {0xa8, 0x80},
        {0xa9, 0x20},
        {0xaa, 0x20},
        {0xab, 0x20},
        {0xac, 0x3c},
        {0xad, 0xf0},
        {0xc8, 0x18},
        {0xc9, 0x20},
        {0xca, 0x17},
        {0xcb, 0x1f},
        {0xaf, 0x00},
        {0xc5, 0x18},
        {0xc6, 0x00},
        {0xc7, 0x20},
        {0xae, 0x80},
        {0xcc, 0x40},
        {0xcd, 0x58},
        {0xee, 0x4c},
        {0x8e, 0x07},
        {0x8f, 0x79},

#elif (IMAGE_SENSOR_USE == IMAGE_SENSOR_GC0308)
        {0xfe, 0x80},
        {0xfe, 0x00}, //set page0
        {0xd2, 0x10}, //close AEC
        {0x22, 0x55}, //close AWB
        {0x5a, 0x56},
        {0x5b, 0x40},
        {0x5c, 0x4a},
        {0x22, 0x57}, //Open AWB
#if 0
    //config for 39 FPS
    {0x01 , 0x6a},//HB 106
    //{0x02 , 0x70},//VB 112
    {0x02 , 0x0c},//change VB from 112 to 12
    {0x0f , 0x00},
    {0xe2 , 0x00},//anti-flicker step [11:8]
    {0xe3 , 0x96},//anti-flicker step [7:0]
    {0xe4 , 0x01},//exp level 1  50.00fps or 20ms
    {0xe5 , 0x2c},
    {0xe6 , 0x03},//exp level 2  16.67fps
    {0xe7 , 0x84},
    {0xe8 , 0x04},//exp level 3  12.5fps
    {0xe9 , 0xb0},
    {0xea , 0x09},//exp level 4  6.00fps
    {0xeb , 0xc4},
#elif 0
        //config for 33.33 FPS
        {0x01, 0x6a}, //HB 106
        {0x02, 0x52}, //VB 82
        {0x0f, 0x00},
        {0xe2, 0x00}, //anti-flicker step [11:8]
        {0xe3, 0x96}, //anti-flicker step [7:0]
        {0xe4, 0x01}, //exp level 1  33.33fps or 30ms
        {0xe5, 0x2c},
        {0xe6, 0x03}, //exp level 2  16.67fps
        {0xe7, 0x84},
        {0xe8, 0x04}, //exp level 3  12.5fps
        {0xe9, 0xb0},
        {0xea, 0x09}, //exp level 4  6.00fps
        {0xeb, 0xc4},
#else
        //config for 25 FPS
        {0x01, 0x6a}, //HB 106
        //{0x02 , 0x70},//VB 112
        {0x02, 0xe8}, //change VB from 112 to 232
        {0x0f, 0x00},
        {0xe2, 0x00}, //anti-flicker step [11:8]
        {0xe3, 0x96}, //anti-flicker step [7:0]
        {0xe4, 0x02}, //exp level 1  25.00fps or 40ms
        {0xe5, 0x58},
        {0xe6, 0x03}, //exp level 2  16.67fps
        {0xe7, 0x84},
        {0xe8, 0x04}, //exp level 3  12.5fps
        {0xe9, 0xb0},
        {0xea, 0x09}, //exp level 4  6.00fps
        {0xeb, 0xc4},
#endif
        {0xec, 0x00}, //select max exposure level 1 - highest fps
        {0x05, 0x00},
        {0x06, 0x00},
        {0x07, 0x00},
        {0x08, 0x00},
        {0x09, 0x01},
        {0x0a, 0xe8},
        //{0x0a , 0x70},//change win_height from 488 to 368
        {0x0b, 0x02},
        {0x0c, 0x88}, //win_width 648
        {0x0d, 0x02},
        {0x0e, 0x02},
        {0x10, 0x22},
        {0x11, 0xfd},
        {0x12, 0x2a},
        {0x13, 0x00},
        //{0x14 , 0x10},
        {0x14, 0x13}, //enable mirror & flip
        //-------------H_V_Switch(4)---------------//
        /*
    1://normal
    {0x14 , 0x10},
    2://IMAGE_H_MIRROR
    {0x14 , 0x11},
    3://IMAGE_V_MIRROR
    {0x14 , 0x12},
    4://IMAGE_HV_MIRROR
    {0x14 , 0x13},*/
        {0x15, 0x0a},
        {0x16, 0x05},
        {0x17, 0x01},
        {0x18, 0x44},
        {0x19, 0x44},
        {0x1a, 0x1e},
        {0x1b, 0x00},
        {0x1c, 0xc1},
        {0x1d, 0x08},
        {0x1e, 0x60},
        {0x1f, 0x17},
        {0x20, 0xff},
        {0x21, 0xf8},
        {0x22, 0x57},
        {0x24, 0xa0}, //Cb Y Cr Y
        {0x25, 0x0f},
        //output sync_mode
        //{0x26 , 0x02},//0x03  20101016 zhj
        {0x26, 0x03}, //sync mode, high high
        {0x2f, 0x01},
        {0x30, 0xf7},
        {0x31, 0x50},
        {0x32, 0x00},
        {0x39, 0x04},
        {0x3a, 0x18},
        {0x3b, 0x20},
        {0x3c, 0x00},
        {0x3d, 0x00},
        {0x3e, 0x00},
        {0x3f, 0x00},
        {0x50, 0x10},
        {0x53, 0x82},
        {0x54, 0x80},
        {0x55, 0x80},
        {0x56, 0x82},
        {0x8b, 0x40},
        {0x8c, 0x40},
        {0x8d, 0x40},
        {0x8e, 0x2e},
        {0x8f, 0x2e},
        {0x90, 0x2e},
        {0x91, 0x3c},
        {0x92, 0x50},
        {0x5d, 0x12},
        {0x5e, 0x1a},
        {0x5f, 0x24},
        {0x60, 0x07},
        {0x61, 0x15},
        {0x62, 0x08},
        {0x64, 0x03},
        {0x66, 0xe8},
        {0x67, 0x86},
        {0x68, 0xa2},
        {0x69, 0x18},
        {0x6a, 0x0f},
        {0x6b, 0x00},
        {0x6c, 0x5f},
        {0x6d, 0x8f},
        {0x6e, 0x55},
        {0x6f, 0x38},
        {0x70, 0x15},
        {0x71, 0x33},
        {0x72, 0xdc},
        {0x73, 0x80},
        {0x74, 0x02},
        {0x75, 0x3f},
        {0x76, 0x02},
        {0x77, 0x36},
        {0x78, 0x88},
        {0x79, 0x81},
        {0x7a, 0x81},
        {0x7b, 0x22},
        {0x7c, 0xff},
        {0x93, 0x48},
        {0x94, 0x00},
        {0x95, 0x05},
        {0x96, 0xe8},
        {0x97, 0x40},
        {0x98, 0xf0},
        {0xb1, 0x38},
        {0xb2, 0x38},
        {0xbd, 0x38},
        {0xbe, 0x36},
#if 1
        {0xd0, 0xc9},
        {0xd1, 0x10},
        {0xd3, 0x80},
        {0xd5, 0xf2},
        {0xd6, 0x16},
#else
        //default AEC setting
        {0xd0, 0xca},
        {0xd1, 0xa1},
        {0xd3, 0xa0},
        {0xd5, 0xf2},
        {0xd6, 0x18},
#endif
        {0xdb, 0x92},
        {0xdc, 0xa5},
        {0xdf, 0x23},
        {0xd9, 0x00},
        {0xda, 0x00},
        {0xe0, 0x09},
        {0xed, 0x04},
#if 0
    //default max dgain
    {0xee , 0xa0},
    {0xef , 0x40},
#else
        //increased max dgain
        {0xee, 0xf0},
        {0xef, 0x60},
#endif
        {0x80, 0x03},
        {0x80, 0x03},
        {0x9F, 0x10},
        {0xA0, 0x20},
        {0xA1, 0x38},
        {0xA2, 0x4E},
        {0xA3, 0x63},
        {0xA4, 0x76},
        {0xA5, 0x87},
        {0xA6, 0xA2},
        {0xA7, 0xB8},
        {0xA8, 0xCA},
        {0xA9, 0xD8},
        {0xAA, 0xE3},
        {0xAB, 0xEB},
        {0xAC, 0xF0},
        {0xAD, 0xF8},
        {0xAE, 0xFD},
        {0xAF, 0xFF},
        /*
    GC0308_GAMMA_Select,
    1://smallest gamma curve
    {0x9F , 0x0B},
    {0xA0 , 0x16},
    {0xA1 , 0x29},
    {0xA2 , 0x3C},
    {0xA3 , 0x4F},
    {0xA4 , 0x5F},
    {0xA5 , 0x6F},
    {0xA6 , 0x8A},
    {0xA7 , 0x9F},
    {0xA8 , 0xB4}, 
    {0xA9 , 0xC6},
    {0xAA , 0xD3},
    {0xAB , 0xDD},
    {0xAC , 0xE5},
    {0xAD , 0xF1},
    {0xAE , 0xFA},
    {0xAF , 0xFF},
    2:
    {0x9F , 0x0E},
    {0xA0 , 0x1C},
    {0xA1 , 0x34},
    {0xA2 , 0x48},
    {0xA3 , 0x5A},
    {0xA4 , 0x6B},
    {0xA5 , 0x7B},
    {0xA6 , 0x95},
    {0xA7 , 0xAB},
    {0xA8 , 0xBF},
    {0xA9 , 0xCE},
    {0xAA , 0xD9},
    {0xAB , 0xE4},
    {0xAC , 0xEC},
    {0xAD , 0xF7},
    {0xAE , 0xFD},
    {0xAF , 0xFF},
    3:
    {0x9F , 0x10},
    {0xA0 , 0x20},
    {0xA1 , 0x38},
    {0xA2 , 0x4E},
    {0xA3 , 0x63},
    {0xA4 , 0x76},
    {0xA5 , 0x87},
    {0xA6 , 0xA2},
    {0xA7 , 0xB8},
    {0xA8 , 0xCA},
    {0xA9 , 0xD8},
    {0xAA , 0xE3},
    {0xAB , 0xEB},
    {0xAC , 0xF0},
    {0xAD , 0xF8},
    {0xAE , 0xFD},
    {0xAF , 0xFF},
    4:
    {0x9F , 0x14},
    {0xA0 , 0x28},
    {0xA1 , 0x44},
    {0xA2 , 0x5D},
    {0xA3 , 0x72},
    {0xA4 , 0x86},
    {0xA5 , 0x95},
    {0xA6 , 0xB1},
    {0xA7 , 0xC6},
    {0xA8 , 0xD5},
    {0xA9 , 0xE1},
    {0xAA , 0xEA},
    {0xAB , 0xF1},
    {0xAC , 0xF5},
    {0xAD , 0xFB},
    {0xAE , 0xFE},
    {0xAF , 0xFF},
    5://largest gamma curve
    {0x9F , 0x15},
    {0xA0 , 0x2A},
    {0xA1 , 0x4A},
    {0xA2 , 0x67},
    {0xA3 , 0x79},
    {0xA4 , 0x8C},
    {0xA5 , 0x9A},
    {0xA6 , 0xB3},
    {0xA7 , 0xC5},
    {0xA8 , 0xD5},
    {0xA9 , 0xDF},
    {0xAA , 0xE8},
    {0xAB , 0xEE},
    {0xAC , 0xF3},
    {0xAD , 0xFA},
    {0xAE , 0xFD},
    {0xAF , 0xFF},
    */
        //-----------GAMMA Select End--------------//
        {0xc0, 0x00},
        {0xc1, 0x10},
        {0xc2, 0x1C},
        {0xc3, 0x30},
        {0xc4, 0x43},
        {0xc5, 0x54},
        {0xc6, 0x65},
        {0xc7, 0x75},
        {0xc8, 0x93},
        {0xc9, 0xB0},
        {0xca, 0xCB},
        {0xcb, 0xE6},
        {0xcc, 0xFF},
        {0xf0, 0x02},
        {0xf1, 0x01},
        {0xf2, 0x01},
        {0xf3, 0x30},
        {0xf9, 0x9f},
        {0xfa, 0x78},
        //{0xfa , 0x58},//Change measure window Y1 from 480 to 352
        //-------------------------------------------------
        {0xfe, 0x01}, // set page1
        {0x00, 0xf5},
        {0x02, 0x1a},
        {0x0a, 0xa0},
        {0x0b, 0x60},
        {0x0c, 0x08},
        {0x0e, 0x4c},
        {0x0f, 0x39},
        {0x11, 0x3f},
        {0x12, 0x72},
        {0x13, 0x13},
        {0x14, 0x42},
        {0x15, 0x43},
        {0x16, 0xc2},
        {0x17, 0xa8},
        {0x18, 0x18},
        {0x19, 0x40},
        {0x1a, 0xd0},
        {0x1b, 0xf5},
        {0x70, 0x40},
        {0x71, 0x58},
        {0x72, 0x30},
        {0x73, 0x48},
        {0x74, 0x20},
        {0x75, 0x60},
        {0x77, 0x20},
        {0x78, 0x32},
        {0x30, 0x03},
        {0x31, 0x40},
        {0x32, 0xe0},
        {0x33, 0xe0},
        {0x34, 0xe0},
        {0x35, 0xb0},
        {0x36, 0xc0},
        {0x37, 0xc0},
        {0x38, 0x04},
        {0x39, 0x09},
        {0x3a, 0x12},
        {0x3b, 0x1C},
        {0x3c, 0x28},
        {0x3d, 0x31},
        {0x3e, 0x44},
        {0x3f, 0x57},
        {0x40, 0x6C},
        {0x41, 0x81},
        {0x42, 0x94},
        {0x43, 0xA7},
        {0x44, 0xB8},
        {0x45, 0xD6},
        {0x46, 0xEE},
        {0x47, 0x0d},
        {0xfe, 0x00}, //set page0
        //-----------Update the registers 2010/07/06-------------//
        //Registers of Page0
        {0xfe, 0x00}, //set page0
        {0x10, 0x26},
        {0x11, 0x0d}, //fd,modified by mormo 2010/07/06
        {0x1a, 0x2a}, //1e,modified by mormo 2010/07/06
        {0x1c, 0x49}, //c1,modified by mormo 2010/07/06
        {0x1d, 0x9a}, //08,modified by mormo 2010/07/06
        {0x1e, 0x61}, //60,modified by mormo 2010/07/06
        {0x3a, 0x20},
        {0x50, 0x14}, //10,modified by mormo 2010/07/06
        {0x53, 0x80},
        {0x56, 0x80},
        {0x8b, 0x20}, //LSC
        {0x8c, 0x20},
        {0x8d, 0x20},
        {0x8e, 0x14},
        {0x8f, 0x10},
        {0x90, 0x14},
        {0x94, 0x02},
        {0x95, 0x07},
        {0x96, 0xe0},
        {0xb1, 0x40}, //YCPT
        {0xb2, 0x40},
        {0xb3, 0x40},
        {0xb6, 0xe0},
        //{0xd0 , 0xcb},//AECT    c9,modifed by mormo 2010/07/06
        //{0xd3 , 0x48},//80,modified by mormor 2010/07/06
        {0xf2, 0x02},
        //{0xf7 , 0x12},
        //{0xf8 , 0x0a},
        //Registers
        {0xfe, 0x01}, // set page1
        {0x02, 0x20},
        {0x04, 0x10},
        {0x05, 0x08},
        {0x06, 0x20},
        {0x08, 0x0a},
        {0x0e, 0x44},
        {0x0f, 0x32},
        {0x10, 0x41},
        {0x11, 0x37},
        {0x12, 0x22},
        {0x13, 0x19},
        {0x14, 0x44},
        {0x15, 0x44},
        {0x19, 0x50},
        {0x1a, 0xd8},
        {0x32, 0x10},
        {0x35, 0x00},
        {0x36, 0x80},
        {0x37, 0x00},
        //-----------Update the registers end---------//
        {0xfe, 0x00}, //set page0
        {0xd2, 0x90},
#endif
};

SPI_Psram_Cfg_Type apMemory1604 = {
    .readIdCmd = 0x9F,
    .readIdDmyClk = 0,
    .burstToggleCmd = 0xC0,
    .resetEnableCmd = 0x66,
    .resetCmd = 0x99,
    .enterQuadModeCmd = 0x35,
    .exitQuadModeCmd = 0xF5,
    .readRegCmd = 0xB5,
    .readRegDmyClk = 1,
    .writeRegCmd = 0xB1,
    .readCmd = 0x03,
    .readDmyClk = 0,
    .fReadCmd = 0x0B,
    .fReadDmyClk = 1,
    .fReadQuadCmd = 0xEB,
    .fReadQuadDmyClk = 3,
    .writeCmd = 0x02,
    .quadWriteCmd = 0x38,
    .pageSize = 512,
    .ctrlMode = PSRAM_SPI_CTRL_MODE,
    .driveStrength = PSRAM_DRIVE_STRENGTH_50_OHMS,
    .burstLength = PSRAM_BURST_LENGTH_512_BYTES,
};

SF_Ctrl_Cmds_Cfg cmdsCfg = {
    .cmdsEn = ENABLE,
    .burstToggleEn = ENABLE,
    .wrapModeEn = DISABLE,
    .wrapLen = SF_CTRL_WRAP_LEN_512,
};

SF_Ctrl_Psram_Cfg sfCtrlPsramCfg = {
    .owner = SF_CTRL_OWNER_SAHB,
    .padSel = SF_CTRL_PAD_SEL_DUAL_CS_SF2,
    .bankSel = SF_CTRL_SEL_PSRAM,
    .psramRxClkInvertSrc = ENABLE,
    .psramRxClkInvertSel = ENABLE,
    .psramDelaySrc = ENABLE,
    .psramClkDelay = 0,
};

/*@} end of group IMAGE_SENSOR_Private_Variables */

/** @defgroup  IMAGE_SENSOR_Global_Variables
 *  @{
 */

/*@} end of group IMAGE_SENSOR_Global_Variables */

/** @defgroup  IMAGE_SENSOR_Private_Fun_Declaration
 *  @{
 */
static void CAM_GPIO_Init(void);
static void I2C_GPIO_Init(void);
static BL_Err_Type CAM_Write_Byte8(uint8_t data, uint8_t cmd);
static uint8_t CAM_Read_Byte8(uint8_t cmd);
static BL_Err_Type CAM_Read_ID(void);
static BL_Err_Type CAM_Reg_Config(void);

/*@} end of group IMAGE_SENSOR_Private_Fun_Declaration */

/** @defgroup  IMAGE_SENSOR_Private_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  CAMERA GPIO Initialization
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void CAM_GPIO_Init(void)
{
    GLB_GPIO_Cfg_Type cfg;

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_0;
    cfg.gpioFun = GPIO0_FUN_PIX_CLK;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_1;
    cfg.gpioFun = GPIO1_FUN_FRAME_VLD;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_2;
    cfg.gpioFun = GPIO2_FUN_LINE_VLD;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_3;
    cfg.gpioFun = GPIO3_FUN_PIX_DAT0;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_4;
    cfg.gpioFun = GPIO4_FUN_PIX_DAT1;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_5;
    cfg.gpioFun = GPIO5_FUN_PIX_DAT2;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_6;
    cfg.gpioFun = GPIO6_FUN_PIX_DAT3;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_12;
    cfg.gpioFun = GPIO12_FUN_PIX_DAT4;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_29;
    cfg.gpioFun = GPIO29_FUN_PIX_DAT5;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_30;
    cfg.gpioFun = GPIO30_FUN_PIX_DAT6;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_31;
    cfg.gpioFun = GPIO31_FUN_PIX_DAT7;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_10;
    cfg.gpioFun = GPIO10_FUN_CAM_REF_CLK;
    cfg.gpioMode = GPIO_MODE_OUTPUT;
    cfg.pullType = GPIO_PULL_NONE;
    //GLB_GPIO_Init(&cfg);
}

/****************************************************************************/ /**
 * @brief  Dump image sensor register while is set
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_Dump_Register(void)
{
    uint32_t i;

    for (i = 0; i < sizeof(sensorRegList) / sizeof(sensorRegList[0]); i++)
    {
        MSG("reg[%02x]: %02x\n", sensorRegList[i][0], CAM_Read_Byte8(sensorRegList[i][0]));
    }
}

/****************************************************************************/ /**
 * @brief  I2C GPIO Initialization
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void I2C_GPIO_Init(void)
{
    uint8_t i;
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[2];
    uint8_t gpiofuns[2];
    uint8_t gpioMode[2];

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioMode = GPIO_MODE_OUTPUT;

    gpiopins[0] = GLB_GPIO_PIN_16;
    gpiopins[1] = GLB_GPIO_PIN_11;
    gpiofuns[0] = GPIO16_FUN_I2C0_SCL;
    gpiofuns[1] = GPIO11_FUN_I2C0_SDA;
    gpioMode[0] = GPIO_MODE_AF;
    gpioMode[1] = GPIO_MODE_AF;

    for (i = 0; i < sizeof(gpiopins) / sizeof(gpiopins[0]); i++)
    {
        cfg.gpioPin = gpiopins[i];
        cfg.gpioFun = gpiofuns[i];
        cfg.gpioMode = gpioMode[i];
        GLB_GPIO_Init(&cfg);
    }
}

/****************************************************************************/ /**
 * @brief  PSRAM GPIO Initialization
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void ATTR_TCM_SECTION SF_Cfg_Init_Ext_Psram_Gpio(void)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[6];
    uint8_t i = 0;

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_FLASH_PSRAM;

    gpiopins[0] = BFLB_EXTPSRAM_CLK_GPIO;
    gpiopins[1] = BFLB_EXTPSRAM_CS_GPIO;
    gpiopins[2] = BFLB_EXTPSRAM_DATA0_GPIO;
    gpiopins[3] = BFLB_EXTPSRAM_DATA1_GPIO;
    gpiopins[4] = BFLB_EXTPSRAM_DATA2_GPIO;
    gpiopins[5] = BFLB_EXTPSRAM_DATA3_GPIO;

    for (i = 0; i < sizeof(gpiopins); i++)
    {
        cfg.gpioPin = gpiopins[i];
        if (i == 0 || i == 1)
        {
            /*flash clk and cs is output*/
            cfg.gpioMode = GPIO_MODE_OUTPUT;
        }
        else
        {
            /*data are bidir*/
            cfg.gpioMode = GPIO_MODE_AF;
        }
        GLB_GPIO_Init(&cfg);
    }
}

/****************************************************************************/ /**
 * @brief  I2C CAMERA Write 8 bits
 *
 * @param  data: Reg Value
 * @param  cmd: Reg Addr
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type CAM_Write_Byte8(uint8_t data, uint8_t cmd)
{
    uint8_t temp = data;
    I2C_Transfer_Cfg tranCfg;

    tranCfg.slaveAddr = I2C_CAMERA_ADDR;
    tranCfg.stopEveryByte = DISABLE;
    tranCfg.subAddrSize = 1;
    tranCfg.subAddr = cmd;
    tranCfg.dataSize = 1;
    tranCfg.data = &temp;
    return I2C_MasterSendBlocking(I2C0_ID, &tranCfg);
}

/****************************************************************************/ /**
 * @brief  I2C CAMERA Read 8 bits
 *
 * @param  cmd: Reg Addr
 *
 * @return Reg Value
 *
*******************************************************************************/
static uint8_t CAM_Read_Byte8(uint8_t cmd)
{
    uint8_t temp = 0;
    I2C_Transfer_Cfg tranCfg;

    tranCfg.slaveAddr = I2C_CAMERA_ADDR;
    tranCfg.stopEveryByte = DISABLE;
    tranCfg.subAddrSize = 1;
    tranCfg.subAddr = cmd;
    tranCfg.dataSize = 1;
    tranCfg.data = &temp;
    I2C_MasterReceiveBlocking(I2C0_ID, &tranCfg);

    return temp;
}

/****************************************************************************/ /**
 * @brief  CAMERA Read ID
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type CAM_Read_ID(void)
{
    uint8_t buf[2] = {0};
#if (IMAGE_SENSOR_USE == IMAGE_SENSOR_BF2013)
    buf[0] = CAM_Read_Byte8(BF2013_ID_MSB);
    bflb_platform_delay_ms(10);
    buf[1] = CAM_Read_Byte8(BF2013_ID_LSB);

    if (buf[0] == 0x37 && buf[1] == 0x03)
    {
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
#elif (IMAGE_SENSOR_USE == IMAGE_SENSOR_GC0308)
    buf[0] = CAM_Read_Byte8(GC0308_ID);

    if (buf[0] == 0x9b)
    {
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
#endif
}

/****************************************************************************/ /**
 * @brief  CAMERA Reg Config
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type CAM_Reg_Config(void)
{
    int i;

    for (i = 0; i < sizeof(sensorRegList) / sizeof(sensorRegList[0]); i++)
    {
        if (CAM_Write_Byte8(sensorRegList[i][1], sensorRegList[i][0]) != SUCCESS)
        {
            return ERROR;
        }
        //if(i == 0){
        bflb_platform_delay_ms(1);
        //}
    }
    return SUCCESS;
}

/*@} end of group IMAGE_SENSOR_Private_Functions */

/** @defgroup  IMAGE_SENSOR_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  PSRAM Config Initialization
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_PSRAM_Init(void)
{
    uint8_t psramId[8] = {0};

    GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_72M, 1);
    SF_Cfg_Init_Ext_Psram_Gpio();

    Psram_Init(&apMemory1604, &cmdsCfg, &sfCtrlPsramCfg);
    Psram_ReadId(&apMemory1604, psramId);
    MSG("PSRAM ID: %02X %02X %02X %02X %02X %02X %02X %02X.\r\n",
        psramId[0], psramId[1], psramId[2], psramId[3], psramId[4], psramId[5], psramId[6], psramId[7]);

    Psram_Cache_Write_Set(&apMemory1604, SF_CTRL_QIO_MODE, DISABLE, ENABLE, ENABLE);
    L1C_Cache_Enable_Set(0x0f);
}

/****************************************************************************/ /**
 * @brief  Image sensor initialization
 *
 * @param  mjpegEn: Enable or disable mjpeg
 * @param  camCfg: CAM configuration structure pointer
 * @param  mjpegCfg: MJPEG configuration structure pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type Image_Sensor_Init(BL_Fun_Type mjpegEn, CAM_CFG_Type *camCfg, MJPEG_CFG_Type *mjpegCfg)
{
    /* Gpio init */
    CAM_GPIO_Init();
    I2C_GPIO_Init();

    /* Set clock */
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_I2C);
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_CAM);
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_MJPEG);
    GLB_Set_I2C_CLK(1, 9);
    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_DLL96M, 3);
    GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_CAM);

    if (CAM_Read_ID() != SUCCESS)
    {
        return ERROR;
    }
    if (CAM_Reg_Config() != SUCCESS)
    {
        return ERROR;
    }
    CAM_Disable();
    MJPEG_Disable();

    if (mjpegEn)
    {
        CAM_Init(camCfg);
        MJPEG_Init(mjpegCfg);
        MJPEG_Enable();
    }
    else
    {
        CAM_Init(camCfg);
    }

    CAM_Enable();

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get one camera frame in interleave mode
 *
 * @param  pic: Pointer of picture start address
 * @param  len: Length of picture
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type Image_Sensor_CAM_Get_Interleave(uint8_t **pic, uint32_t *len)
{
    CAM_Interleave_Frame_Info info;
    BL702_MemSet(&info, 0, sizeof(info));

    CAM_Interleave_Get_Frame_Info(&info);

    if (info.validFrames == 0)
    {
        return ERROR;
    }

    *pic = (uint8_t *)(info.curFrameAddr);
    *len = info.curFrameBytes;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get one camera frame in planar mode
 *
 * @param  yuv: YUV mode
 * @param  picYY: Pointer of picture YY data start address(YUYV)
 * @param  lenYY: Length of picture YY data
 * @param  picUV: Pointer of picture UV data start address(YUYV)
 * @param  lenUV: Length of picture UV data
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type Image_Sensor_CAM_Get_Planar(CAM_YUV_Mode_Type yuv, uint8_t **picYY, uint32_t *lenYY, uint8_t **picUV, uint32_t *lenUV)
{
    CAM_Planar_Frame_Info info;
    BL702_MemSet(&info, 0, sizeof(info));

    CAM_Planar_Get_Frame_Info(&info);

    if (yuv == CAM_YUV400_EVEN || yuv == CAM_YUV400_ODD)
    {
        if (info.validFrames0 == 0 && info.validFrames1 == 0)
        {
            return ERROR;
        }
    }
    else
    {
        if (info.validFrames0 == 0 || info.validFrames1 == 0)
        {
            return ERROR;
        }
    }

    *picYY = (uint8_t *)(info.curFrameAddr0);
    *lenYY = info.curFrameBytes0;
    *picUV = (uint8_t *)(info.curFrameAddr1);
    *lenUV = info.curFrameBytes1;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get available count0 of camera frames
 *
 * @param  None
 *
 * @return Frames count
 *
*******************************************************************************/
uint8_t Image_Sensor_CAM_Frame_Count0(void)
{
    return CAM_Get_Frame_Count_0();
}

/****************************************************************************/ /**
 * @brief  Get available count1 of camera frames
 *
 * @param  None
 *
 * @return Frames count
 *
*******************************************************************************/
uint8_t Image_Sensor_CAM_Frame_Count1(void)
{
    return CAM_Get_Frame_Count_1();
}

/****************************************************************************/ /**
 * @brief  Pop one camera frame in interleave mode
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_CAM_Release_Interleave(void)
{
    CAM_Interleave_Pop_Frame();
}

/****************************************************************************/ /**
 * @brief  Pop one camera frame in planar mode
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_CAM_Release_Planar(void)
{
    CAM_Planar_Pop_Frame();
}

/****************************************************************************/ /**
 * @brief  Open camera function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_CAM_Open(void)
{
    CAM_Enable();
}

/****************************************************************************/ /**
 * @brief  Close camera function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_CAM_Close(void)
{
    CAM_Disable();
}

/****************************************************************************/ /**
 * @brief  Deinit camera module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_CAM_Deinit(void)
{
    CAM_Deinit();
}

/****************************************************************************/ /**
 * @brief  Get one mjpeg frame
 *
 * @param  pic: Pointer of picture start address
 * @param  len: Length of picture
 * @param  q: Quantization
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type Image_Sensor_MJPEG_Get(uint8_t **pic, uint32_t *len, uint8_t *q)
{
    MJPEG_Frame_Info info;
    BL702_MemSet(&info, 0, sizeof(info));

    MJPEG_Get_Frame_Info(&info);

    if (info.validFrames == 0)
    {
        return ERROR;
    }

    *pic = (uint8_t *)(info.curFrameAddr);
    *len = info.curFrameBytes;
    *q = info.curFrameQ;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get available count of mjpeg frames
 *
 * @param  None
 *
 * @return Frames count
 *
*******************************************************************************/
uint8_t Image_Sensor_MJPEG_Frame_Count(void)
{
    return MJPEG_Get_Frame_Count();
}

/****************************************************************************/ /**
 * @brief  Pop one mjpeg frame
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_MJPEG_Release(void)
{
    MJPEG_Pop_Frame();
}

/****************************************************************************/ /**
 * @brief  Open mjpeg function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_MJPEG_Open(void)
{
    MJPEG_Enable();
}

/****************************************************************************/ /**
 * @brief  Close mjpeg function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_MJPEG_Close(void)
{
    MJPEG_Disable();
}

/****************************************************************************/ /**
 * @brief  Deinit mjpeg module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Image_Sensor_MJPEG_Deinit(void)
{
    MJPEG_Deinit();
}



// #include "hal_camera.h"
// #include "bl702_cam.h"
// #include "bl702_mjpeg.h"
// #include "bl702_i2c_gpio_sim.h"
// #include "bl702_glb.h"
// #include "bl702_config.h"

// static void camera_gpio_init(camera_device_t *dev);
// /**
//  * @brief 
//  * 
//  * @param dev 
//  * @param oflag 
//  * @return int 
//  */
// int camera_open(struct device *dev, uint16_t oflag)
// {
//     camera_device_t *camera_device = (camera_device_t *)dev;
//     CAM_CFG_Type camCfg;
//     uint32_t size;

//     GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_CAM);
//     GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_MJPEG);
//     camera_gpio_init(camera_device);
//     GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_DLL96M, 3);
//     GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_CAM);
//     CAM_Disable();
//     MJPEG_Disable();
//     camCfg.swIntCnt = 1;
//     camCfg.frameMode = camera_device->pic_format.frame_mode;
//     camCfg.yuvMode = camera_device->pic_format.yuv_mode;
//     camCfg.linePol = camera_device->line_pol;
//     camCfg.framePol = camera_device->frame_pol;
//     camCfg.camSensorMode = CAM_SENSOR_MODE_V_OR_H;
//     camCfg.burstType = CAM_BURST_TYPE_INCR8;
//     camCfg.waitCount = 0x40;
//     size = camera_device->pic_reso_x * camera_device->pic_reso_y;
//     if (camera_device->pic_format.mjpeg_enable == ENABLE)
//     {
//         MJPEG_CFG_Type mjpegCfg;
//         camCfg.swMode = CAM_SW_MODE_AUTO;
//         camCfg.memStart0 = (uint32_t)camera_device->buffer;
//         camCfg.memSize0 = camera_device->pic_reso_x * 2 * 8 * 3;
//         camCfg.frameSize0 = size * 2;
//         camCfg.memStart1 = 0;
//         camCfg.memSize1 = 0;
//         camCfg.frameSize1 = 0;
//         mjpegCfg.burst = MJPEG_BURST_INCR16;
//         mjpegCfg.quality = camera_device->pic_format.mjpegQ;
//         mjpegCfg.yuv = MJPEG_YUV422_INTERLEAVE;
//         mjpegCfg.waitCount = 0x400;
//         mjpegCfg.bufferMjpeg = camCfg.memStart0 + camCfg.memSize0;
//         mjpegCfg.sizeMjpeg = camera_device->length - camCfg.memSize0;
//         mjpegCfg.bufferCamYY = (uint32_t)camera_device->buffer;
//         mjpegCfg.sizeCamYY = 3;
//         mjpegCfg.bufferCamUV = 0;
//         mjpegCfg.sizeCamUV = 0;
//         mjpegCfg.resolutionX = camera_device->pic_reso_x;
//         mjpegCfg.resolutionY = camera_device->pic_reso_y;
//         mjpegCfg.bitOrderEnable = ENABLE,
//         mjpegCfg.evenOrderEnable = ENABLE,
//         mjpegCfg.swapModeEnable = DISABLE,
//         mjpegCfg.overStopEnable = ENABLE,
//         mjpegCfg.reflectDmy = DISABLE,
//         mjpegCfg.verticalDmy = DISABLE,
//         mjpegCfg.horizationalDmy = DISABLE,
//         camera_device->picture.base1 = (uint8_t*)mjpegCfg.bufferMjpeg;
//         camera_device->picture.base2 = (uint8_t *)0;
//         CAM_Init(&camCfg);
//         MJPEG_Init(&mjpegCfg);
//         MJPEG_Enable();
//     }
//     else
//     {
//         camCfg.swMode = CAM_SW_MODE_MANUAL;
//         if (camera_device->pic_format.frame_mode == CAMERA_INTERLEAVE_MODE)
//         {
//             camCfg.memStart0 = (uint32_t)camera_device->buffer;
//             camCfg.memSize0 = camera_device->length;
//             camCfg.memStart1 = 0;
//             camCfg.memSize1 = 0;
//             camCfg.frameSize1 = 0;
//             switch (camera_device->pic_format.yuv_mode)
//             {
//             case CAMERA_YUV422:
//                 camCfg.frameSize0 = size * 2;
//                 break;
//             case CAMERA_YUV420_EVEN:
//                 camCfg.frameSize0 = size * 2 * 3 / 4;
//                 break;
//             case CAMERA_YUV420_ODD:
//                 camCfg.frameSize0 = size * 2 * 3 / 4;
//                 break;
//             case CAMERA_YUV400_EVEN:
//                 camCfg.frameSize0 = size;
//                 break;
//             case CAMERA_YUV400_ODD:
//                 camCfg.frameSize0 = size;
//                 break;
//             default:
//                 break;
//             }
//             camera_device->picture.base1 = (uint8_t *)camCfg.memStart0;
//             camera_device->picture.base2 = (uint8_t *)0;
//         }
//         else
//         {
//             camCfg.memStart0 = (uint32_t)camera_device->buffer;
//             camCfg.memSize0 = camera_device->length / 2;
//             camCfg.memStart1 = (uint32_t)camera_device->buffer + camera_device->length / 2;
//             camCfg.memSize1 = camera_device->length / 2;
//             camCfg.frameSize1 = size;
//             switch (camera_device->pic_format.yuv_mode)
//             {
//             case CAMERA_YUV422:
//                 camCfg.frameSize0 = size;
//                 break;
//             case CAMERA_YUV420_EVEN:
//                 camCfg.frameSize0 = size / 2;
//                 break;
//             case CAMERA_YUV420_ODD:
//                 camCfg.frameSize0 = size / 2;
//                 break;
//             case CAMERA_YUV400_EVEN:
//                 camCfg.frameSize0 = 0;
//                 break;
//             case CAMERA_YUV400_ODD:
//                 camCfg.frameSize0 = 0;
//                 break;
//             default:
//                 break;
//             }
//             camera_device->picture.base1 = (uint8_t *)camCfg.memStart0;
//             camera_device->picture.base2 = (uint8_t *)camCfg.memStart1;
//         }
//         CAM_Init(&camCfg);
//     }
//     CAM_Enable();

//     return 0;
// }

// int camera_close(struct device *dev)
// {
//     CAM_Disable();

//     return 0;
// }

// int camera_control(struct device *dev, int cmd, void *args)
// {
//     //i2c_device_t *i2c_device = (i2c_device_t *)dev;

//     switch (cmd)
//     {
//     case DEVICE_CTRL_SET_INT /* constant-expression */:

//         break;
//     case DEVICE_CTRL_CLR_INT /* constant-expression */:
//         /* code */
//         /* Enable UART interrupt*/

//         break;
//     case DEVICE_CTRL_GET_INT /* constant-expression */:
//         /* code */
//         break;
//     case DEVICE_CTRL_CONFIG /* constant-expression */:
//         if (args == (void *)DEVICE_CTRL_CONFIG_CAM_START)
//         {

//         }
//         else if (args == (void *)DEVICE_CTRL_CONFIG_CAM_STOP)
//         {

//         }
//         break;
//     case 4 /* constant-expression */:
//         /* code */
//         break;
//     case 5 /* constant-expression */:
//         /* code */
//         break;
//     default:
//         break;
//     }

//     return 0;
// }

// int camera_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
// {
//     camera_device_t *camera_device = (camera_device_t *)dev;
//     camera_picture_t *pic = buffer;

//     if(camera_device->pic_format.mjpeg_enable == ENABLE)
//     {
//         MJPEG_Frame_Info info =
//             {
//                 .validFrames = 0,
//                 .curFrameAddr = 0,
//                 .curFrameBytes = 0,
//                 .curFrameQ = 0,
//                 .status = 0,
//             };
//         MJPEG_Get_Frame_Info(&info);
//         if (info.validFrames == 0)
//         {
//             return ERROR;
//         }
//         pic->addr1 = (uint8_t *)(info.curFrameAddr);
//         pic->length1 = info.curFrameBytes;
//         return SUCCESS;
//     }
//     else
//     {
//         if (camera_device->pic_format.frame_mode == CAMERA_INTERLEAVE_MODE)
//         {
//             CAM_Interleave_Frame_Info info =
//                 {
//                     .validFrames = 0,
//                     .curFrameAddr = 0,
//                     .curFrameBytes = 0,
//                     .status = 0,
//                 };
//             CAM_Interleave_Get_Frame_Info(&info);
//             if (info.validFrames == 0)
//             {
//                 return ERROR;
//             }
//             pic->addr1 = (uint8_t *)(info.curFrameAddr);
//             pic->length1 = info.curFrameBytes;
//         }
//         else
//         {
//             CAM_Planar_Frame_Info info =
//                 {
//                     .validFrames0 = 0,
//                     .validFrames1 = 0,
//                     .curFrameAddr0 = 0,
//                     .curFrameAddr1 = 0,
//                     .curFrameBytes0 = 0,
//                     .curFrameBytes1 = 0,
//                     .status = 0,
//                 };
//             CAM_Planar_Get_Frame_Info(&info);
//             if (camera_device->pic_format.yuv_mode == CAMERA_YUV400_EVEN || camera_device->pic_format.yuv_mode == CAMERA_YUV400_ODD)
//             {
//                 if (info.validFrames0 == 0 && info.validFrames1 == 0)
//                 {
//                     return ERROR;
//                 }
//             }
//             else
//             {
//                 if (info.validFrames0 == 0 || info.validFrames1 == 0)
//                 {
//                     return ERROR;
//                 }
//             }
//             pic->addr1 = (uint8_t *)(info.curFrameAddr0);
//             pic->length1 = info.curFrameBytes0;
//             pic->addr2 = (uint8_t *)(info.curFrameAddr1);
//             pic->length2 = info.curFrameBytes1;
//             return SUCCESS;
//         }
//     }
//     return ERROR;
// }

// #ifdef USING_DEVICE_OPS
// struct device_drv_ops cam_driver =
//     {
//         cam_open,
//         cam_close,
//         cam_control,
//         cam_write,
//         cam_read
//     };
// #endif

// void camera_register(camera_device_t *device, const char *name, uint16_t flag)
// {
//     struct device *dev;

//     dev = &(device->parent);
// #ifdef USING_DEVICE_OPS
//     dev->api = &cam_driver;
// #else
//     dev->open = camera_open;
//     dev->close = camera_close;
//     dev->control = camera_control;
//     dev->write = NULL;
//     dev->read = camera_read;
// #endif

//     dev->status = DEVICE_UNREGISTER;
//     dev->type = DEVICE_CLASS_CAMERA;
//     dev->handle = NULL;

//     device_register(dev, name, flag);
// }

// static void camera_gpio_init(camera_device_t *dev)
// {
//     GLB_GPIO_Cfg_Type cfg;

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.pclk;
//     cfg.gpioFun = GPIO0_FUN_PIX_CLK;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.vsync;
//     cfg.gpioFun = GPIO1_FUN_FRAME_VLD;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.hsync;
//     cfg.gpioFun = GPIO2_FUN_LINE_VLD;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.data0;
//     cfg.gpioFun = GPIO3_FUN_PIX_DAT0;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.data1;
//     cfg.gpioFun = GPIO4_FUN_PIX_DAT1;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.data2;
//     cfg.gpioFun = GPIO5_FUN_PIX_DAT2;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.data3;
//     cfg.gpioFun = GPIO6_FUN_PIX_DAT3;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.data4;
//     cfg.gpioFun = GPIO12_FUN_PIX_DAT4;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.data5;
//     cfg.gpioFun = GPIO29_FUN_PIX_DAT5;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.data6;
//     cfg.gpioFun = GPIO30_FUN_PIX_DAT6;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     cfg.drive = 0;
//     cfg.smtCtrl = 1;
//     cfg.gpioPin = dev->pin.data7;
//     cfg.gpioFun = GPIO31_FUN_PIX_DAT7;
//     cfg.gpioMode = GPIO_MODE_INPUT;
//     cfg.pullType = GPIO_PULL_NONE;
//     GLB_GPIO_Init(&cfg);

//     if(ENABLE == dev->pin.mclk_enable)
//     {
//         cfg.drive = 1;
//         cfg.smtCtrl = 1;
//         cfg.gpioPin = dev->pin.mclk;
//         cfg.gpioFun = GPIO10_FUN_CAM_REF_CLK;
//         cfg.gpioMode = GPIO_MODE_OUTPUT;
//         cfg.pullType = GPIO_PULL_NONE;
//         GLB_GPIO_Init(&cfg);
//     }
    
// }