/**
 * @file main.c
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
#include "mcu_lcd.h"
#include "hal_dma.h"
#include "hal_cam.h"
#include "hal_mjpeg.h"
#include "bsp_sf_psram.h"
#include "bsp_image_sensor.h"
#include "image_proc.h"
#include "tensorflow/lite/micro/examples/person_detection/main_functions.h"

#define CAMERA_RESOLUTION_X (640)
#define CAMERA_RESOLUTION_Y (480)
#define CAMERA_FRAME_SIZE   (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y)
#define CAMERA_WRITE_ADDR   (0x26000000)
#define CAMERA_BUFFER_SIZE  (0x96000)

#define PERSON_THRESHOLD (-20)

static mjpeg_device_t mjpeg_cfg;
static cam_device_t camera_cfg = {
    .software_mode = CAM_MANUAL_MODE,
    .frame_mode = CAM_FRAME_INTERLEAVE_MODE,
    .yuv_format = CAM_YUV_FORMAT_YUV400_ODD,
    .cam_write_ram_addr = CAMERA_WRITE_ADDR,
    .cam_write_ram_size = CAMERA_BUFFER_SIZE,
    .cam_frame_size = CAMERA_FRAME_SIZE,

    .cam_write_ram_addr1 = 0,
    .cam_write_ram_size1 = 0,
    .cam_frame_size1 = 0,
};

/**
 * @brief Convert YUV400 to grayscale image
 *
 * @param y_400 YUV400 image ptr
 * @param gray gray image ptr
 * @param len image size
 */
void Y_to_rgb565_gray(uint8_t *y_400, uint16_t *gray, uint32_t len)
{
    int8_t tmp = 0;

    for (uint32_t i = 0; i < len; i++) {
        if ((uint8_t)(y_400[i]) >= 0xaf) {
            tmp = (uint8_t)(y_400[i] >> 3);
        } else {
            tmp = (uint8_t)((y_400[i] + 0x30) >> 3);
        }
        gray[i] = tmp;
        gray[i] = gray[i] << 5;
        gray[i] += tmp;
        gray[i] = gray[i] << 6;
        gray[i] += tmp;
    }
}

void y400_to_rgb565_gray(uint8_t *y_400, uint16_t *gray, uint32_t y400_x, uint32_t y400_y, uint32_t ratio)
{
    uint8_t tmp = 0;
    uint32_t g = 0;
    for (uint32_t i = 0; i < y400_y; i += ratio) {
        for (uint32_t j = 0; j < y400_x; j += ratio) {
            tmp = (*y_400) >> 3;
            gray[g] = tmp;
            gray[g] = gray[g] << 5;
            gray[g] += tmp;
            gray[g] = gray[g] << 6;
            gray[g] += tmp;
            g++;
            y_400 += ratio;
        }
        y_400 += y400_x * (ratio - 1);
    }
}

int main(void)
{
    int score = 0;
    uint8_t *picture;
    uint8_t *picture_lcd;
    uint32_t pic_offset;
    uint32_t length;
    uint16_t *gray_565 = (uint16_t *)(0x26098000);

    bflb_platform_init(0);

    bsp_sf_psram_init(1);

    MSG("Setup LCD\r\n");
    if (lcd_init()) {
        MSG("lcd err \r\n");
    }
    // lcd_set_dir(0, 0);
    lcd_clear(0x0);
    lcd_draw_str_ascii16(0, 0, 0xFFFF, 0x0000, (uint8_t *)"Bouffalo LAB\nVWW DEMO @ BL706", 30);

    // TFLite setup
    MSG("Setup TFLite\r\n");

    setup();

    if (SUCCESS != image_sensor_init(DISABLE, &camera_cfg, &mjpeg_cfg)) {
        MSG("Init error!\n");
        BL_CASE_FAIL;
        while (1) {
        }
    }
    struct device *cam0 = device_find("camera0");
    device_control(cam0, DEVICE_CTRL_RESUME, NULL);

    while (1) {
        while (SUCCESS != cam_get_one_frame_interleave(&picture, &length)) {
        }
        device_control(cam0, DEVICE_CTRL_SUSPEND, NULL);
        // UART_SendData(0, (uint8_t *)(picture), length);

        //AI data preprocess
        int8_t *scaled_img = image_proc(picture, length, CAMERA_RESOLUTION_X, CAMERA_RESOLUTION_Y, (int *)&pic_offset);
        picture_lcd = (uint8_t *)(0x26000000 + pic_offset);

        loop(scaled_img, &score);

        // UART_SendData(0, (uint8_t *)(0x2604b9c4), 160 * 160);
        Y_to_rgb565_gray((uint8_t *)(picture_lcd), gray_565, (160 * 160));

        while (lcd_draw_is_busy()) {
        };
        printf("person score %d\r\n\r\n", score);

        lcd_set_dir(0, 0);
        if (score > PERSON_THRESHOLD) {
            lcd_draw_str_ascii16(68, 48, 0xFFFF, 0x07e0, (uint8_t *)"   PERSON   ", 13);
        } else {
            lcd_draw_str_ascii16(68, 48, 0xFFFF, 0xf800, (uint8_t *)"  NOPERSON  ", 13);
        }

        lcd_set_dir(2, 0);
        lcd_draw_picture_nonblocking(40, 80, (160 + 40 - 1), (160 + 80 - 1), gray_565);

        device_control(cam0, DEVICE_CTRL_CAM_FRAME_DROP, NULL);
        device_control(cam0, DEVICE_CTRL_RESUME, NULL);
    }

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
