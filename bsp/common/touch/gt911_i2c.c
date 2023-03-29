/**
 * @file gt911_i2c.c
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

#include "touch.h"

#ifdef TOUCH_I2C_GT911
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_i2c.h"
#include "gt911_i2c.h"

#define BL_RDWD_FRM_BYTEP(p) ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | (p[0]))

static struct bflb_device_s *touch_gt911_i2c = NULL;

static void gt911_i2c_gpio_init(void)
{
    struct bflb_device_s *gt911_i2c_gpio = NULL;
    gt911_i2c_gpio = bflb_device_get_by_name("gpio");

    /* I2C0_SCL */
    bflb_gpio_init(gt911_i2c_gpio, TOUCH_I2C_SCL_PIN /* GPIO_PIN_6 */, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2C0_SDA */
    bflb_gpio_init(gt911_i2c_gpio, TOUCH_I2C_SDA_PIN /* GPIO_PIN_9 */, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
}

static int gt911_i2c_peripheral_init(void)
{
    touch_gt911_i2c = bflb_device_get_by_name("i2c0");

    if (touch_gt911_i2c) {
        // printf("gt911 i2c gpio init\r\n");
        /* init i2c gpio */
        gt911_i2c_gpio_init();
        /* init i2c 200k */
        bflb_i2c_init(touch_gt911_i2c, 200000);
    } else {
        printf("i2c device get fail\r\n");
        return -1;
    }
    return 0;
}

static int gt911_i2c_read_byte(uint16_t register_addr, uint8_t *data_buf, uint16_t len)
{
    static struct bflb_i2c_msg_s msg[2];

    msg[0].addr = GT911_I2C_SLAVE_ADDR;
    msg[0].flags = I2C_M_NOSTOP;
    msg[0].buffer = (uint8_t *)(&register_addr);
    msg[0].length = 2;

    msg[1].addr = GT911_I2C_SLAVE_ADDR;
    msg[1].flags = I2C_M_READ;
    msg[1].buffer = data_buf;
    msg[1].length = len;
    bflb_i2c_transfer(touch_gt911_i2c, msg, 2);
    return 0;
}

static int gt911_i2c_write_byte(uint16_t register_addr, uint8_t *data_buf, uint16_t len)
{
    static struct bflb_i2c_msg_s msg[2];

    msg[0].addr = GT911_I2C_SLAVE_ADDR;
    msg[0].flags = I2C_M_NOSTOP;
    msg[0].buffer = (uint8_t *)(&register_addr);
    msg[0].length = 2;

    msg[1].addr = GT911_I2C_SLAVE_ADDR;
    msg[1].flags = 0;
    msg[1].buffer = data_buf;
    msg[1].length = len;

    bflb_i2c_transfer(touch_gt911_i2c, msg, 2);
    return 0;
}

int gt911_i2c_get_gesture_id()
{
    uint8_t data_buf = 0;

    if (gt911_i2c_read_byte(GT911_READ_XY_REG, &data_buf, 1)) {
        return -1;
    }

    return data_buf;
}

int gt911_i2c_init(touch_coord_t *max_value)
{
    uint8_t data_buf[5] = { 0 };
    uint32_t product_id = 0;
    uint16_t x_max = 0, y_max = 0;

    gt911_i2c_peripheral_init();

    if (gt911_i2c_read_byte(GT911_PRODUCT_ID_REG, data_buf, 4)) {
        printf("touch i2c read error\r\n");
        return -1;
    }
    product_id = BL_RDWD_FRM_BYTEP(data_buf);
    printf("Touch Product ID: 0x%08x\r\n", product_id);

    if (product_id == GT911_PRODUCT_ID) {
#if 0
        data_buf[0] = 0;
        data_buf[1] = (uint8_t)(max_value->coord_x << 8);
        data_buf[2] = (uint8_t)(max_value->coord_x);
        data_buf[3] = (uint8_t)(max_value->coord_y << 8);
        data_buf[4] = (uint8_t)(max_value->coord_y);

        if (gt911_i2c_write_byte(GT911_CONFIG_REG, data_buf, 5)) {
            return -1;
        }

        if (gt911_i2c_write_byte(GT911_CONFIG_REG + (0x0005 << 8), (void *)g_gt911_cfg_params + 5, (sizeof(g_gt911_cfg_params) / sizeof(g_gt911_cfg_params[0]) - 5))) {
            return -1;
        }
#else
        if (gt911_i2c_write_byte(GT911_CONFIG_REG, (void *)g_gt911_cfg_params, (sizeof(g_gt911_cfg_params) / sizeof(g_gt911_cfg_params[0])))) {
            return -1;
        }
#endif

    } else {
        printf("Touch Product ID read fail!\r\n");
        return -1;
    }

    if (gt911_i2c_read_byte(GT911_FIRMWARE_VERSION_REG, data_buf, 2)) {
        return -1;
    }
    printf("Touch Firmware Version: 0x%04x\r\n", data_buf);

    if (gt911_i2c_read_byte(GT911_X_RESOLUTION, data_buf, 4)) {
        return -1;
    }
    x_max = (((uint16_t)data_buf[1] << 8) | data_buf[0]);
    y_max = (((uint16_t)data_buf[3] << 8) | data_buf[2]);

    printf("Touch Resolution %dx%d \r\n", x_max, y_max);

    return 0;
}

int gt911_i2c_read(uint8_t *point_num, touch_coord_t *touch_coord, uint8_t max_num)
{
    uint8_t data_buf[8 * GT911_I2C_MAX_POINT] = { 0 };
    uint8_t read_num;

    *point_num = 0;

    if (point_num == NULL || touch_coord == NULL || max_num == 0) {
        return -1;
    }

    if (gt911_i2c_read_byte(GT911_READ_XY_REG, data_buf, 1)) {
        return -1;
    }

    /* no touch */
    if (data_buf[0] == 0) {
        return 0;
    }

    if (data_buf[0] > GT911_I2C_MAX_POINT) {
        data_buf[0] = GT911_I2C_MAX_POINT;
    }

    read_num = (data_buf[0] > max_num) ? max_num : data_buf[0];

    /* read gt911 reg */
    gt911_i2c_read_byte(GT911_READ_XY_REG, data_buf, (8 * read_num));

    /* get point coordinates */
    for (uint8_t i = 0; i < read_num; i++) {
        uint8_t *p_data = &data_buf[i * 8];
        touch_coord[i].coord_x = (uint16_t)p_data[3] << 8 | p_data[2];
        touch_coord[i].coord_y = (uint16_t)p_data[5] << 8 | p_data[4];
    }

    *point_num = read_num;

    data_buf[0] = 0;
    gt911_i2c_write_byte(GT911_READ_XY_REG, data_buf, 1);

    return 0;
}

#endif
