/**
 * @file ft6x36_i2c.c
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

#include "touch.h"

#ifdef TOUCH_I2C_FT6X36
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_i2c.h"
#include "ft6x36_i2c.h"

static struct bflb_device_s *touch_ft6x36_i2c = NULL;

static void ft6x36_i2c_gpio_init(void)
{
    struct bflb_device_s *ft63x6_i2c_gpio = NULL;
    ft63x6_i2c_gpio = bflb_device_get_by_name("gpio");

    /* I2C0_SCL */
    bflb_gpio_init(ft63x6_i2c_gpio, TOUCH_I2C_SCL_PIN, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2C0_SDA */
    bflb_gpio_init(ft63x6_i2c_gpio, TOUCH_I2C_SDA_PIN, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
}

static int ft6x36_i2c_peripheral_init(void)
{
    touch_ft6x36_i2c = bflb_device_get_by_name("i2c0");

    if (touch_ft6x36_i2c) {
        // printf("ft6x36 i2c gpio init\r\n");
        /* init i2c gpio */
        ft6x36_i2c_gpio_init();
        /* init i2c 200k */
        bflb_i2c_init(touch_ft6x36_i2c, 200000);
    } else {
        printf("i2c device get fail\r\n");
        return -1;
    }

    return 0;
}

static int ft6x36_i2c_read_byte(uint8_t register_addr, uint8_t *data_buf, uint16_t len)
{
    static struct bflb_i2c_msg_s msg[2];

    msg[0].addr = FT6236_I2C_SLAVE_ADDR;
    msg[0].flags = I2C_M_NOSTOP;
    msg[0].buffer = &register_addr;
    msg[0].length = 1;

    msg[1].flags = I2C_M_READ;
    msg[1].buffer = data_buf;
    msg[1].length = len;
    bflb_i2c_transfer(touch_ft6x36_i2c, msg, 2);

    return 0;
}

int ft6x36_get_gesture_id()
{
    uint8_t data_buf = FT6X36_GEST_ID_NO_GESTURE;

    if (ft6x36_i2c_read_byte(FT6X36_GEST_ID_REG, &data_buf, 1)) {
        return -1;
    }

    return data_buf;
}

int ft6x36_i2c_init(touch_coord_t *max_value)
{
    uint8_t data_buf;
    printf("ft6x36 i2c init\r\n");

    ft6x36_i2c_peripheral_init();

    if (ft6x36_i2c_read_byte(FT6X36_PANEL_ID_REG, &data_buf, 1)) {
        return -1;
    }
    printf("Touch Device ID: 0x%02x\r\n", data_buf);

    if (ft6x36_i2c_read_byte(FT6X36_CHIPSELECT_REG, &data_buf, 1)) {
        return -1;
    }
    printf("Touch Chip ID: 0x%02x\r\n", data_buf);

    if (ft6x36_i2c_read_byte(FT6X36_DEV_MODE_REG, &data_buf, 1)) {
        return -1;
    }
    printf("Touch Device mode: 0x%02x\r\n", data_buf);

    if (ft6x36_i2c_read_byte(FT6X36_FIRMWARE_ID_REG, &data_buf, 1)) {
        return -1;
    }
    printf("Touch Firmware ID: 0x%02x\r\n", data_buf);

    if (ft6x36_i2c_read_byte(FT6X36_RELEASECODE_REG, &data_buf, 1)) {
        return -1;
    }
    printf("Touch Release code: 0x%02x\r\n", data_buf);

    return 0;
}

int ft6x36_i2c_read(uint8_t *point_num, touch_coord_t *touch_coord, uint8_t max_num)
{
    uint8_t data_buf[5];
    *point_num = 0;

    if (point_num == NULL || touch_coord == NULL || max_num == 0) {
        return -1;
    }

    if (ft6x36_i2c_read_byte(FT6X36_TD_STAT_REG, &data_buf[0], 1)) {
        return -1;
    }
    data_buf[0] &= FT6X36_TD_STAT_MASK;

    /* no touch or err */
    if (data_buf[0] == 0 || data_buf[0] > 2) {
        if (data_buf[0] > 2) {
            return -2;
        } else {
            return 0;
        }
    }

    /* Get the first point */
    ft6x36_i2c_read_byte(FT6X36_P1_XH_REG, &data_buf[1], 4);
    touch_coord[0].coord_x = (data_buf[1] & FT6X36_MSB_MASK) << 8 | (data_buf[2] & FT6X36_LSB_MASK);
    touch_coord[0].coord_y = (data_buf[3] & FT6X36_MSB_MASK) << 8 | (data_buf[4] & FT6X36_LSB_MASK);
    *point_num = 1;

    /* Get the second point */
    if (data_buf[0] > 1 && max_num > 1) {
        ft6x36_i2c_read_byte(FT6X36_P2_XH_REG, &data_buf[1], 4);
        touch_coord[1].coord_x = (data_buf[1] & FT6X36_MSB_MASK) << 8 | (data_buf[2] & FT6X36_LSB_MASK);
        touch_coord[1].coord_y = (data_buf[3] & FT6X36_MSB_MASK) << 8 | (data_buf[4] & FT6X36_LSB_MASK);
        *point_num = 2;
    }
    return 0;
}

#endif
