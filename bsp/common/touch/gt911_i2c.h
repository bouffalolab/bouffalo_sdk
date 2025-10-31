/**
 * @file gt911_i2c.h
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
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
#ifndef _GT911_I2C_H
#define _GT911_I2C_H

#include "bflb_core.h"
#include "touch.h"

#define GT911_CUSTOM_CFG           (1)
#define GT911_I2C_SLAVE_ADDR       (0xBA >> 1) // #define GT911_ADDR (0xBA)   //(0x28)   /* slave addr */
#define GT911_READ_XY_REG          (0x4E81)    //(0x814E) /* Coordinate register */
#define GT911_CLEARBUF_REG         (0x4E81)    //(0x814E) /* Clear the coordinate register*/
#define GT911_POINT1_REG           (0x4F81)    //(0x814f) /* point 1 reg */
#define GT911_POINT2_REG           (0x5781)    //(0x8157) /* point 2 reg */
#define GT911_POINT3_REG           (0x5F81)    //(0x815f) /* point 3 reg */
#define GT911_POINT4_REG           (0x6781)    //(0x8167) /* point 4 reg */
#define GT911_POINT5_REG           (0x6F81)    //(0x816f) /* point 5 reg */
#define GT911_CONFIG_REG           (0x4780)    //(0x8047) /* Configuration parameter register */
#define GT911_COMMAND_REG          (0x4080)    //(0x8040) /* Command */
#define GT911_PRODUCT_ID_REG       (0x4081)    //(0x8140) /* Chip ID */
#define GT911_VENDOR_ID_REG        (0x4A81)    //(0x814A) /* Current module option information */
#define GT911_CONFIG_VERSION_REG   (0x4780)    //(0x8047) /* Configuration file version number */
#define GT911_CONFIG_CHECKSUM_REG  (0xFF80)    //(0x80FF) /* Configuration file check code */
#define GT911_FIRMWARE_VERSION_REG (0x4481)    //(0x8144) /* Firmware version number */
#define GT911_X_RESOLUTION         (0x4681)    //(0x8146) /* X-coordinate resolution */
#define GT911_Y_RESOLUTION         (0x4881)    //(0x8148) /* Y-coordinate resolution */

#define GT911_PRODUCT_ID           (0x00313139)

/* Maximum border values of the touchscreen pad that the chip can handle */
#if GT911_CUSTOM_CFG
#define GT911_MAX_WIDTH   ((uint16_t)320)
#define GT911_MAX_HEIGHT  ((uint16_t)480)
#define GT911_INT_TRIGGER (0)
#else
#define GT911_MAX_WIDTH   ((uint16_t)4096)
#define GT911_MAX_HEIGHT  ((uint16_t)4096)
#define GT911_INT_TRIGGER (1)
#endif

/* Max detectable simultaneous touch points */
#define GT911_I2C_MAX_POINT 5

/* GT911 cfg data */
static const uint8_t g_gt911_cfg_params[] = {
    /*KDC CFG*/
    0x00, 0x40, 0x01, 0xE0, 0x01, 0x0A, 0x05, 0x00,
    0x01, 0x08, 0x28, 0x05, 0x50, 0x32, 0x03, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x86, 0x26, 0x07, 0x17, 0x15,
    0x31, 0x0D, 0x00, 0x00, 0x01, 0xBA, 0x03, 0x1D,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x64, 0x32,
    0x00, 0x00, 0x00, 0x0F, 0x4B, 0x94, 0xC5, 0x02,
    0x07, 0x00, 0x00, 0x04, 0x9B, 0x11, 0x00, 0x72,
    0x18, 0x00, 0x57, 0x21, 0x00, 0x42, 0x2E, 0x00,
    0x35, 0x40, 0x00, 0x35, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x12, 0x10, 0x0E, 0x0C, 0x0A, 0x08, 0x06, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x21,
    0x20, 0x1F, 0x1E, 0x1D, 0x00, 0x02, 0x04, 0x06,
    0x08, 0x0A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xDC, 0x01 /* update flag */
};

typedef struct
{
    uint8_t touch_point_flag;
    uint8_t touch_key_sts;

    uint16_t x0;
    uint16_t y0;
    uint16_t p0;

    uint16_t x1;
    uint16_t y1;
    uint16_t p1;

    uint16_t x2;
    uint16_t y2;
    uint16_t p2;

    uint16_t x3;
    uint16_t y3;
    uint16_t p3;

    uint16_t x4;
    uint16_t y4;
    uint16_t p4;
} gt911_point_t;

int gt911_i2c_init(touch_coord_t *max_value);
int gt911_i2c_get_gesture_id();
int gt911_i2c_read(uint8_t *point_num, touch_coord_t *touch_coord, uint8_t max_num);

#endif /* __gt911_H */
