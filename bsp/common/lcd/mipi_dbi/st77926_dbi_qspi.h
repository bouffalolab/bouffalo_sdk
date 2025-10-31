/**
 * @file st77926_dpi.h
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

#ifndef _ST77926_DBI_H_
#define _ST77926_DBI_H_

#include "../lcd_conf.h"

#if defined LCD_DBI_ST77926

/* Do not modify the following */

/* cmd:0x38, cmd:1-wire(1-byte), addr:4wire(3-byte), data:4wire */
#define ST77926_QSPI_CMD          (0x38)
#define ST77926_QSPI_ADDR(cmd)    ((cmd << 8) & 0x0000FF00)
#define ST77926_QSPI_ADDR_BYTE    (3)

/* ST77926-datasheet: 
    XS [9:0] & “XE [9:0] – XS [9:0] + 1” always must be set as a multiple of 4 */
#define ST77926_DBI_PIXEL_ALIGN (4)

#if (ST77926_DBI_PIXEL_FORMAT == 1)
#define ST77926_DBI_COLOR_DEPTH 16
typedef uint16_t st77926_dbi_color_t;
#elif (ST77926_DBI_PIXEL_FORMAT == 2)
#define ST77926_DBI_COLOR_DEPTH 32
typedef uint32_t st77926_dbi_color_t;
#endif

typedef struct {
    uint8_t cmd; /* 0xFF : delay(databytes)ms */
    const char *data;
    uint8_t databytes; /* Num of data in data; or delay time */
} st77926_dbi_init_cmd_t;

int st77926_dbi_init();
void st77926_dbi_async_callback_enable(bool enable);
void st77926_dbi_async_callback_register(void (*callback)(void));
int st77926_dbi_set_dir(uint8_t dir, uint8_t mir_flag);
void st77926_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void st77926_dbi_draw_point(uint16_t x, uint16_t y, st77926_dbi_color_t color);
void st77926_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st77926_dbi_color_t color);
void st77926_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st77926_dbi_color_t *picture);
void st77926_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st77926_dbi_color_t *picture);
int st77926_dbi_draw_is_busy(void);

#endif
#endif
