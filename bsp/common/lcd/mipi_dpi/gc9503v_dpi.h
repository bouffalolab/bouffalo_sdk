/**
 * @file gc9503v_dpi.h
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

#ifndef _GC9503V_DPI_H_
#define _GC9503V_DPI_H_

#include "../lcd_conf.h"

/* Do not modify the following information */

#if defined LCD_DPI_GC9503V

#if (GC9503V_DPI_INIT_INTERFACE == 1)

/* Software spi 9-bit mode PIN */
#define GC9503V_SPI_CS_HIGH bflb_gpio_set(gc9503v_gpio, GC9503V_DPI_SPI_CS_PIN)
#define GC9503V_SPI_CS_LOW  bflb_gpio_reset(gc9503v_gpio, GC9503V_DPI_SPI_CS_PIN)

#define GC9503V_SPI_CLK_HIGH bflb_gpio_set(gc9503v_gpio, GC9503V_DPI_SPI_CLK_PIN)
#define GC9503V_SPI_CLK_LOW  bflb_gpio_reset(gc9503v_gpio, GC9503V_DPI_SPI_CLK_PIN)

#define GC9503V_SPI_MOSI_HIGH bflb_gpio_set(gc9503v_gpio, GC9503V_DPI_SPI_DAT_PIN)
#define GC9503V_SPI_MOSI_LOW  bflb_gpio_reset(gc9503v_gpio, GC9503V_DPI_SPI_DAT_PIN)

#define DUMMY_DELAY      \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop"); \
    asm volatile("nop");

#endif

#if (GC9503V_DPI_PIXEL_FORMAT == 1)
#define GC9503V_DPI_COLOR_DEPTH 16
typedef uint16_t gc9503v_dpi_color_t;
#elif (GC9503V_DPI_PIXEL_FORMAT == 2)
#define GC9503V_DPI_COLOR_DEPTH 32
typedef uint32_t gc9503v_dpi_color_t;
#else
#error "GC9503V pixel format select error"
#endif

/* Gc9503v needs to be initialized using the DBI(typeC) or SPI interface */
typedef struct {
    uint8_t cmd; /* 0xFF : delay(databytes)ms */
    const char *data;
    uint8_t databytes; /* Num of data in data; or delay time */
} gc9503v_dpi_init_cmd_t;

int gc9503v_dpi_init(gc9503v_dpi_color_t *screen_buffer);
int gc9503v_dpi_screen_switch(gc9503v_dpi_color_t *screen_buffer);
gc9503v_dpi_color_t *gc9503v_dpi_get_screen_using(void);
int gc9503v_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void));

#endif
#endif
