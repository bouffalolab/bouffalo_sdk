/**
 * @file ili9488_dpi.c
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

#if defined(LCD_DPI_ILI9488)

#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "ili9488_dpi.h"

#if (ILI9488_DPI_INIT_INTERFACE == 2)

#if defined(BL808)
#include "bl808_dbi.h"
#elif defined(BL616)
#include "bl616_dbi.h"
#endif

#elif (ILI9488_DPI_INIT_INTERFACE == 1)

#include "bflb_spi.h"

// #define ILI9488_SPI_INDEX SPI0_INDEX
#define ILI9488_SPI_ID   0
#define ILI9488_SPI_NAME "spi0"

#endif

/* mipi dpi (RGB) paramant */
static lcd_mipi_dpi_init_t dpi_para = {
    .width = ILI9488_DPI_W,  /* LCD Active Width */
    .height = ILI9488_DPI_H, /* LCD Active Height */
                             /* Total Width = HSW + HBP + Active_Width + HFP */
    .hsw = 4,                /* LCD HSW (Hsync Pulse Width) */
    .hbp = 4,                /* LCD HBP (Hsync Back Porch) */
    .hfp = 12,               /* LCD HFP (Hsync Front Porch) */
                             /* Total Height = VSW + VBP + Active_Height + VFP */
    .vsw = 6,                /* LCD VSW (Vsync Pulse Width) */
    .vbp = 6,                /* LCD VBP (Vsync Back Porch) */
    .vfp = 18,               /* LCD VFP (Vsync Front Porch) */

    .frame_rate = 60, /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */

#if (ILI9488_DPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_MIPI_DPI_PIXEL_FORMAT_RGB565,
#endif

    .frame_buff = NULL,
};

#if (ILI9488_DPI_INIT_INTERFACE == 2)

/* dbi config */
static DBI_CFG_Type dbiCfg = {
    .mode = DBI_TYPE_C_4_WIRE,               /* DBI type B or C select */
    .pixelFormat = DBI_PIXEL_RGB888,         /* DBI pixel format */
    .fifoFormat = DBI_FIFO_888_NBGR,         /* DBI fifo format */
    .continueEn = ENABLE,                    /* Enable:CS will stay asserted between each consecutive pixel, disable:CS will de-assert between each pixel */
    .dummyEn = DISABLE,                      /* Enable:dummy cycle will be inserted between command phase adn data phase, disable:no dummy cycle */
    .dummyCnt = 0,                           /* Dummy cycle count,effective only in type C(fixed to 1 in type B) */
    .clkPhase = DBI_SCL_CLK_PHASE_0,         /* DBI clock phase */
    .clkPolarity = DBI_SCL_CLK_POLARITY_LOW, /* DBI clock polarity */
    .period.startLen = 9,
    .period.dataPhase0Len = 9,
    .period.dataPhase1Len = 9,
    .period.intervalLen = 9
};

#elif (ILI9488_DPI_INIT_INTERFACE == 1)

static struct bflb_device_s *ili9488_gpio;
static struct bflb_device_s *ili9488_spi;

/* spi write cmd */
static int ili9488_spi_write_cmd(uint8_t cmd)
{
    ILI9488_SPI_DC_LOW;
    ILI9488_SPI_CS_LOW;
    bflb_spi_poll_send(ili9488_spi, cmd);
    ILI9488_SPI_CS_HIGH;
    ILI9488_SPI_DC_HIGH;
    return 0;
}

/* spi write data */
static int ili9488_spi_write_data_byte(uint8_t data)
{
    ILI9488_SPI_CS_LOW;
    bflb_spi_poll_send(ili9488_spi, data);
    ILI9488_SPI_CS_HIGH;
    return 0;
}

#endif

static const ili9488_dpi_init_cmd_t ili9488_dpi_mode_init_cmds[] = {
    { 0x01, NULL, 0 },  /* software reset */
    { 0xFF, NULL, 10 }, /* delay 10ms */

    { 0x11, NULL, 0 },   /* Sleep Out */
    { 0xFF, NULL, 120 }, /* delay 120ms */

    { 0xE0, "\x00\x07\x0F\x0D\x1B\x0A\x3C\x78\x4A\x07\x0E\x09\x1B\x1E\x0F", 15 }, /* PGAMCTRL (Positive Gamma Control) */
    { 0xE1, "\x00\x22\x24\x26\x12\x07\x36\x47\x47\x06\x0A\x07\x30\x37\x0F", 15 }, /* NGAMCTRL (Negative Gamma Control) */

    { 0xC0, "\x10\x10", 2 }, /* Power Control 1 */
    { 0xC1, "\x41", 1 },     /* Power Control 2 */

    { 0xC5, "\x00\x20\xd0", 3 }, /* VCOM Control */
    { 0x36, "\x08", 1 },         /* Memory Access Control */

    { 0x3A, "\x50", 1 }, /* Interface Pixel 16bits/pixel*/

    { 0xB0, "\x00", 1 }, /* Interface Mode Control */
    { 0xB1, "\xB0", 1 }, /* Frame rate 70Hz */
    { 0xB4, "\x02", 1 }, /* Display Inversion Control */
    // { 0xB5, "\x08\x08\x06\x12", 4},/* Display Inversion Control */
    { 0xB6, "\x30\x22\x3B", 3 }, /* Display Function Control, DE Mode */
    { 0xBE, "\x00\x04", 1 },
    { 0xE9, "\x00", 1 }, /* Set Image Function */

    { 0xF7, "\xA9\x51\x2C\x82", 4 }, /* Adjust Control 3 */

    { 0x29, NULL, 0 }, /* Display On */
    { 0xFF, NULL, 10 },
};

int ili9488_dpi_init(ili9488_dpi_color_t *screen_buffer)
{
/* Ili9488 needs to be initialized using the DBI(typeC) or SPI interface */
#if (ILI9488_DPI_INIT_INTERFACE == 2)
    DBI_Init(&dbiCfg);

    for (uint16_t i = 0; i < (sizeof(ili9488_dpi_mode_init_cmds) / sizeof(ili9488_dpi_init_cmd_t)); i++) {
        if (ili9488_dpi_mode_init_cmds[i].cmd == 0xFF) {
            bflb_mtimer_delay_ms(ili9488_dpi_mode_init_cmds[i].databytes);
        } else {
            DBI_SetPhaseState(ENABLE, DISABLE);
            DBI_SendCmdWithNormalData(ili9488_dpi_mode_init_cmds[i].cmd, 0, NULL);
            DBI_SetPhaseState(DISABLE, ENABLE);
            for (uint8_t j = 0; j < ili9488_dpi_mode_init_cmds[i].databytes; j++) {
                DBI_SendCmdWithNormalData(0xFF, 1, (uint8_t *)&ili9488_dpi_mode_init_cmds[i].data[j]);
            }
        }
    }
    DBI_SetPhaseState(DISABLE, DISABLE);

#elif (ILI9488_DPI_INIT_INTERFACE == 1)

    /* spi */
    struct bflb_spi_config_s spi_cfg = {
        .freq = 8 * 1000 * 1000,
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE3,
        .data_width = SPI_DATA_WIDTH_8BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    /* CS and DC pin init */
    ili9488_gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(ili9488_gpio, ILI9488_DPI_SPI_CS_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(ili9488_gpio, ILI9488_DPI_SPI_DC_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    ILI9488_SPI_CS_HIGH;
    ILI9488_SPI_DC_HIGH;

    /* spi init */
    ili9488_spi = bflb_device_get_by_name(ILI9488_SPI_NAME);
    bflb_spi_init(ili9488_spi, &spi_cfg);

    for (uint16_t i = 0; i < (sizeof(ili9488_dpi_mode_init_cmds) / sizeof(ili9488_dpi_init_cmd_t)); i++) {
        if ((ili9488_dpi_mode_init_cmds[i].cmd == 0xFF) && (ili9488_dpi_mode_init_cmds[i].data == NULL) && (ili9488_dpi_mode_init_cmds[i].databytes)) {
            bflb_mtimer_delay_ms(ili9488_dpi_mode_init_cmds[i].databytes);
        } else {
            /* send register address */
            ili9488_spi_write_cmd(ili9488_dpi_mode_init_cmds[i].cmd);

            /* send register data */
            for (uint8_t j = 0; j < (ili9488_dpi_mode_init_cmds[i].databytes & 0x7F); j++) {
                ili9488_spi_write_data_byte(ili9488_dpi_mode_init_cmds[i].data[j]);
            }
        }
    }

#endif

    if (screen_buffer == NULL) {
        return -1;
    }
    /* mipi dpi init */
    dpi_para.frame_buff = (void *)screen_buffer;
    return lcd_mipi_dpi_init(&dpi_para);
}

int ili9488_dpi_screen_switch(ili9488_dpi_color_t *screen_buffer)
{
    return lcd_mipi_dpi_screen_switch((void *)screen_buffer);
}

ili9488_dpi_color_t *ili9488_dpi_get_screen_using(void)
{
    return (ili9488_dpi_color_t *)lcd_mipi_dpi_get_screen_using();
}

int ili9488_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == FRAME_INT_TYPE_CYCLE) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE, callback);
    } else if (callback_type == FRAME_INT_TYPE_SWAP) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP, callback);
    }

    return 0;
}

#endif