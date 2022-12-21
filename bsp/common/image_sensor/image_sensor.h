/**
 * @file image_sensor.h
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
#ifndef __IMAGE_SENSOR_H__
#define __IMAGE_SENSOR_H__

#include "bflb_core.h"

/** @defgroup IMAGE_SENSOR_FORMAT image sensor output format definition
  * @{
  */
#define IMAGE_SENSOR_FORMAT_YUV422_YUYV   0
#define IMAGE_SENSOR_FORMAT_YUV422_YVYU   1
#define IMAGE_SENSOR_FORMAT_YUV422_UYVY   2
#define IMAGE_SENSOR_FORMAT_YUV422_VYUY   3
#define IMAGE_SENSOR_FORMAT_GRAY          4
#define IMAGE_SENSOR_FORMAT_RGB565        5
#define IMAGE_SENSOR_FORMAT_BGR565        6
#define IMAGE_SENSOR_FORMAT_RGB888        7
#define IMAGE_SENSOR_FORMAT_BGR888        8
#define IMAGE_SENSOR_FORMAT_BAYER_BGRG    9
#define IMAGE_SENSOR_FORMAT_BAYER_RGBG    10
#define IMAGE_SENSOR_FORMAT_BAYER_GBGR    11
#define IMAGE_SENSOR_FORMAT_BAYER_GRGB    12
/**
  * @}
  */

/** @defgroup IMAGE_SENSOR_INFO image sensor information copy size definition
  * @{
  */
#define IMAGE_SENSOR_INFO_COPY_SIZE 12
/**
  * @}
  */

/**
 * @brief Image sensor command structure
 *
 * @param address  Address of command
 * @param paramete Paramete of command
 */
struct image_sensor_command_s
{
    uint16_t address;
    uint8_t paramete;
};

/**
 * @brief Image sensor configuration structure
 *
 * @param output_format  Output format use @IMAGE_SENSOR_FORMAT
 * @param resolution_x   Resolution X
 * @param resolution_y   Resolution Y
 * @param h_blank        Hsync blank
 * @param pixel_clock    Pixel clock
 * @param name           Name of sensor
 * @param slave_addr     Slave address of sensor
 * @param id_size        Size of sensor ID
 * @param reg_size       Size of sensor register
 * @param id_addr        ID address of sensor, size: (id_size * reg_size) bytes
 * @param id_value       ID value of sensor, size: id_size bytes
 * @param init_list_len  Length of init list
 * @param init_list      Pointer of init list
 */
struct image_sensor_config_s
{
    uint8_t output_format;
    uint16_t resolution_x;
    uint16_t resolution_y;
    uint16_t h_blank;
    uint32_t pixel_clock;
    char name[9];
    uint8_t slave_addr;
    uint8_t id_size;
    uint8_t reg_size;
    uint32_t id_addr;
    uint32_t id_value;
    uint32_t init_list_len;
    struct image_sensor_command_s *init_list;
};

void image_sensor_read(struct bflb_device_s *i2c, uint32_t sensor_index, struct image_sensor_command_s *read_list, uint32_t list_len);
void image_sensor_write(struct bflb_device_s *i2c, uint32_t sensor_index, struct image_sensor_command_s *read_list, uint32_t list_len);
uint32_t image_sensor_scan(struct bflb_device_s *i2c, struct image_sensor_config_s **config);

#endif /* __IMAGE_SENSOR_H__ */
