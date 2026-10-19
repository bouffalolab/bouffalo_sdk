/**
 * @file st7701s_dpi.h
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

#ifndef _ST7701S_DPI_H_
#define _ST7701S_DPI_H_

#include "../lcd_conf.h"

/* Do not modify the following information */

#if defined LCD_DPI_ST7701S

#if (ST7701S_DPI_PIXEL_FORMAT == 1)
#define ST7701S_DPI_COLOR_DEPTH 16
typedef uint16_t st7701s_dpi_color_t;
#elif (ST7701S_DPI_PIXEL_FORMAT == 2)
#define ST7701S_DPI_COLOR_DEPTH 32
typedef uint32_t st7701s_dpi_color_t;
#else
#error "ST7701S pixel format select error"
#endif

/* Gc9503v needs to be initialized using the DBI(typeC) or SPI interface */
typedef struct {
    uint8_t cmd; /* 0xFF : delay(databytes)ms */
    const char *data;
    uint8_t databytes; /* Num of data in data; or delay time */
} st7701s_dpi_init_cmd_t;

int st7701s_dpi_init(st7701s_dpi_color_t *screen_buffer);
int st7701s_dpi_screen_switch(st7701s_dpi_color_t *screen_buffer);
st7701s_dpi_color_t *st7701s_dpi_get_screen_using(void);
int st7701s_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void));

#endif
#endif
