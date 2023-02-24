/**
 * @file gc9503v_dpi.c
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

#include "../lcd.h"

#if defined(LCD_DPI_GC9503V)

#include "gc9503v_dpi.h"
#include "bl_mipi_dpi_pec.h"

#include "bflb_mtimer.h"
#include "bflb_gpio.h"

/* mipi dpi (RGB) paramant */
static lcd_mipi_dpi_init_t dpi_para = {
    .width = GC9503V_DPI_W,  /* LCD Active Width */
    .height = GC9503V_DPI_H, /* LCD Active Height */
                             /* Total Width = HSW + HBP + Active_Width + HFP */
    .hsw = 10,               /* LCD HSW (Hsync Pulse Width) */
    .hbp = 10,               /* LCD HBP (Hsync Back Porch) */
    .hfp = 10,               /* LCD HFP (Hsync Front Porch) */
                             /* Total Height = VSW + VBP + Active_Height + VFP */
    .vsw = 3,                /* LCD VSW (Vsync Pulse Width) */
    .vbp = 12,               /* LCD VBP (Vsync Back Porch) */
    .vfp = 14,               /* LCD VFP (Vsync Front Porch) */

    .frame_rate = 60, /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */

#if (GC9503V_DPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_MIPI_DPI_PIXEL_FORMAT_RGB565,
#endif

    .frame_buff = NULL,
};

#if (GC9503V_DPI_INIT_INTERFACE == 1)

static struct bflb_device_s *gc9503v_gpio;

/* gpio spi init */
static void gc9503v_soft_spi_init(void)
{
    /* gpio24 spi clk init */
    /* gpio25 spi mosi init */
    /* gpio12 spi cs init */
    gc9503v_gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gc9503v_gpio, GC9503V_DPI_SPI_CS_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gc9503v_gpio, GC9503V_DPI_SPI_CLK_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gc9503v_gpio, GC9503V_DPI_SPI_DAT_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    GC9503V_SPI_CS_HIGH;
    GC9503V_SPI_CLK_HIGH;
    GC9503V_SPI_MOSI_HIGH;
}

/* gpio spi send byte */
static ATTR_TCM_SECTION void spi_send_byte(uint8_t data)
{
    for (uint32_t i = 0; i < 8; i++) {
        GC9503V_SPI_CLK_LOW;
        if (data & 0x80) {
            GC9503V_SPI_MOSI_HIGH;
        } else {
            GC9503V_SPI_MOSI_LOW;
        }
        GC9503V_SPI_CLK_HIGH;
        data <<= 1;
        DUMMY_DELAY;
    }
}

/* spi write cmd */
static ATTR_TCM_SECTION int gc9503v_spi_write_cmd(uint8_t cmd)
{
    GC9503V_SPI_CLK_LOW;
    GC9503V_SPI_CS_LOW;
    GC9503V_SPI_MOSI_LOW;
    GC9503V_SPI_CLK_HIGH;
    spi_send_byte(cmd);
    GC9503V_SPI_CLK_LOW;
    GC9503V_SPI_CS_HIGH;
    GC9503V_SPI_MOSI_HIGH;
    return 0;
}

/* spi write data */
static ATTR_TCM_SECTION int gc9503v_spi_write_data_byte(uint8_t data)
{
    GC9503V_SPI_CLK_LOW;
    GC9503V_SPI_CS_LOW;
    GC9503V_SPI_MOSI_HIGH;
    GC9503V_SPI_CLK_HIGH;
    spi_send_byte(data);
    GC9503V_SPI_CLK_LOW;
    GC9503V_SPI_CS_HIGH;
    GC9503V_SPI_MOSI_HIGH;
    return 0;
}

#endif

static const gc9503v_dpi_init_cmd_t gc9503v_dpi_mode_init_cmds[] = {
    { 0x10, NULL, 0 }, /* enter sleep mode */
    { 0xF0, "\x55\xAA\x52\x08\x00", 5 },
    { 0xF6, "\x5A\x87", 2 },
    { 0xC1, "\x3F", 1 },
    { 0xC2, "\x0E", 1 },
    { 0xC6, "\xF8", 1 },
    { 0xC9, "\x10", 1 },
    { 0xCD, "\x25", 1 },
    { 0xF8, "\x8A", 1 },
    { 0xAC, "\x45", 1 },
    { 0xA0, "\xDD", 1 },
    { 0xA7, "\x47", 1 },
    { 0xFA, "\x00\x00\x00\x04", 4 },
    { 0xA3, "\xEE", 1 },
    { 0xFD, "\x28\x28\x00", 3 },
    { 0x71, "\x48", 1 },
    { 0x72, "\x48", 1 },
    { 0x73, "\x00\x44", 2 },
    { 0x97, "\xEE", 1 },
    { 0x83, "\x93", 1 },
    { 0x9A, "\x72", 1 },
    { 0x9B, "\x5A", 1 },
    { 0x82, "\x2C\x2C", 2 },
    { 0xB1, "\x10", 1 },
    { 0x6D, "\x00\x1F\x19\x1A\x10\x0E\x0C\x0A\x02\x07\x1E\x1E\x1E\x1E\x1E\x1E\x1E\x1E\x1E\x1E\x1E\x1E\x08\x01\x09\x0B\x0D\x0F\x1A\x19\x1F\x00", 32 },
    { 0x64, "\x38\x05\x01\xDB\x03\x03\x38\x04\x01\xDC\x03\x03\x7A\x7A\x7A\x7A", 16 },
    { 0x65, "\x38\x03\x01\xDD\x03\x03\x38\x02\x01\xDE\x03\x03\x7A\x7A\x7A\x7A", 16 },
    { 0x66, "\x38\x01\x01\xDF\x03\x03\x38\x00\x01\xE0\x03\x03\x7A\x7A\x7A\x7A", 16 },
    { 0x67, "\x30\x01\x01\xE1\x03\x03\x30\x02\x01\xE2\x03\x03\x7A\x7A\x7A\x7A", 16 },
    { 0x68, "\x00\x08\x15\x08\x15\x7A\x7A\x08\x15\x08\x15\x7A\x7A", 13 },
    { 0x60, "\x38\x08\x7A\x7A\x38\x09\x7A\x7A", 8 },
    { 0x63, "\x31\xE4\x7A\x7A\x31\xE5\x7A\x7A", 8 },
    { 0x6B, "\x07", 1 },
    { 0x7A, "\x08\x13", 2 },
    { 0x7B, "\x08\x13", 2 },
    { 0xD1, "\x00\x00\x00\x04\x00\x12\x00\x18\x00\x21\x00\x2A\x00\x35\x00\x47\x00\x56\x00\x90\x00\xE5\x01\x68\x01\xD5\x01\xD7\x02\x36\x02\xA6\x02\xEE\x03\x48\x03\xA0\x03\xBA\x03\xC5\x03\xD0\x03\xE0\x03\xEA\x03\xFA\x03\xFF", 52 },
    { 0xD2, "\x00\x00\x00\x04\x00\x12\x00\x18\x00\x21\x00\x2A\x00\x35\x00\x47\x00\x56\x00\x90\x00\xE5\x01\x68\x01\xD5\x01\xD7\x02\x36\x02\xA6\x02\xEE\x03\x48\x03\xA0\x03\xBA\x03\xC5\x03\xD0\x03\xE0\x03\xEA\x03\xFA\x03\xFF", 52 },
    { 0xD3, "\x00\x00\x00\x04\x00\x12\x00\x18\x00\x21\x00\x2A\x00\x35\x00\x47\x00\x56\x00\x90\x00\xE5\x01\x68\x01\xD5\x01\xD7\x02\x36\x02\xA6\x02\xEE\x03\x48\x03\xA0\x03\xBA\x03\xC5\x03\xD0\x03\xE0\x03\xEA\x03\xFA\x03\xFF", 52 },
    { 0xD4, "\x00\x00\x00\x04\x00\x12\x00\x18\x00\x21\x00\x2A\x00\x35\x00\x47\x00\x56\x00\x90\x00\xE5\x01\x68\x01\xD5\x01\xD7\x02\x36\x02\xA6\x02\xEE\x03\x48\x03\xA0\x03\xBA\x03\xC5\x03\xD0\x03\xE0\x03\xEA\x03\xFA\x03\xFF", 52 },
    { 0xD5, "\x00\x00\x00\x04\x00\x12\x00\x18\x00\x21\x00\x2A\x00\x35\x00\x47\x00\x56\x00\x90\x00\xE5\x01\x68\x01\xD5\x01\xD7\x02\x36\x02\xA6\x02\xEE\x03\x48\x03\xA0\x03\xBA\x03\xC5\x03\xD0\x03\xE0\x03\xEA\x03\xFA\x03\xFF", 52 },
    { 0xD6, "\x00\x00\x00\x04\x00\x12\x00\x18\x00\x21\x00\x2A\x00\x35\x00\x47\x00\x56\x00\x90\x00\xE5\x01\x68\x01\xD5\x01\xD7\x02\x36\x02\xA6\x02\xEE\x03\x48\x03\xA0\x03\xBA\x03\xC5\x03\xD0\x03\xE0\x03\xEA\x03\xFA\x03\xFF", 52 },
    { 0x3A, "\x66", 1 }, /* pixel format set 16bits:0x55;18bits:0x66 */
    { 0x11, NULL, 0 },   /* sleep out */
    { 0xFF, NULL, 200 }, /* delay 200ms */
    { 0x29, NULL, 0 },   /* display on */
    { 0xFF, NULL, 30 },  /* delay 30ms */
};

int ATTR_TCM_SECTION gc9503v_dpi_init(gc9503v_dpi_color_t *screen_buffer)
{
#if (GC9503V_DPI_INIT_INTERFACE == 1)
    /* spi init */
    gc9503v_soft_spi_init();

    for (uint16_t i = 0; i < (sizeof(gc9503v_dpi_mode_init_cmds) / sizeof(gc9503v_dpi_init_cmd_t)); i++) {
        if ((gc9503v_dpi_mode_init_cmds[i].cmd == 0xFF) && (gc9503v_dpi_mode_init_cmds[i].data == NULL) && (gc9503v_dpi_mode_init_cmds[i].databytes)) {
            bflb_mtimer_delay_ms(gc9503v_dpi_mode_init_cmds[i].databytes);
        } else {
            /* send register address */
            gc9503v_spi_write_cmd(gc9503v_dpi_mode_init_cmds[i].cmd);
            bflb_mtimer_delay_us(22);
            /* send register data */
            for (uint8_t j = 0; j < (gc9503v_dpi_mode_init_cmds[i].databytes); j++) {
                gc9503v_spi_write_data_byte(gc9503v_dpi_mode_init_cmds[i].data[j]);
                bflb_mtimer_delay_us(22);
            }
        }
    }

#endif

    /* mipi dpi init */
    if (screen_buffer == NULL) {
        return -1;
    }
    dpi_para.frame_buff = (void *)screen_buffer;
    return lcd_mipi_dpi_init(&dpi_para);
}

int gc9503v_dpi_screen_switch(gc9503v_dpi_color_t *screen_buffer)
{
    return lcd_mipi_dpi_screen_switch((void *)screen_buffer);
}

gc9503v_dpi_color_t *gc9503v_dpi_get_screen_using(void)
{
    return (gc9503v_dpi_color_t *)lcd_mipi_dpi_get_screen_using();
}

int gc9503v_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == FRAME_INT_TYPE_CYCLE) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE, callback);
    } else if (callback_type == FRAME_INT_TYPE_SWAP) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP, callback);
    }

    return 0;
}

#endif