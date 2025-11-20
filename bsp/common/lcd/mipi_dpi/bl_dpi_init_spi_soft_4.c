/**
 * @file bl_dpi_init_spi_soft_4.c
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

#if (defined(LCD_DPI_INIT_INTERFACE_TYPE) && (LCD_DPI_INIT_INTERFACE_TYPE == 2))

#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bl_dpi_init_spi_soft_4.h"

#define SPI4_CS_PIN  LCD_DPI_INIT_SPI_SOFT_4_PIN_CS
#define SPI4_CLK_PIN LCD_DPI_INIT_SPI_SOFT_4_PIN_CLK
#define SPI4_DAT_PIN LCD_DPI_INIT_SPI_SOFT_4_PIN_DAT
#define SPI4_DC_PIN  LCD_DPI_INIT_SPI_SOFT_4_PIN_DC

#ifndef LCD_DPI_INIT_SPI_SOFT_4_DELAY
#define SPI4_DELAY 50
#else
#define SPI4_DELAY LCD_DPI_INIT_SPI_SOFT_4_DELAY
#endif

static struct bflb_device_s *spi4_gpio = NULL;

static void spi_sig_io_out(uint8_t sig_pin, bool output)
{
    if (output) {
        bflb_gpio_set(spi4_gpio, sig_pin);
    } else {
        bflb_gpio_reset(spi4_gpio, sig_pin);
    }
}

static void spi_sig_delay(void)
{
    for (int i = 0; i < SPI4_DELAY; i++) {
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
    }
}

int lcd_dpi_init_spi_soft_4_init(void)
{
    spi4_gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(spi4_gpio, SPI4_CS_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(spi4_gpio, SPI4_CLK_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(spi4_gpio, SPI4_DAT_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(spi4_gpio, SPI4_DC_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    spi_sig_io_out(SPI4_CS_PIN, 1);
    spi_sig_io_out(SPI4_CLK_PIN, 1);
    spi_sig_io_out(SPI4_DAT_PIN, 1);
    spi_sig_io_out(SPI4_DC_PIN, 1);

    spi_sig_delay();

    return 0;
}

int lcd_dpi_init_spi_soft_4_transmit_cmd_para(uint8_t cmd, uint8_t *para, size_t para_num)
{
    uint8_t byte;

    spi_sig_io_out(SPI4_CS_PIN, 0);

    spi_sig_delay();

    /* DC=0 for command */ 
    spi_sig_io_out(SPI4_DC_PIN, 0);

    spi_sig_delay();

    byte = cmd;
    for (uint8_t j = 0; j < 8; j++) {
        spi_sig_io_out(SPI4_CLK_PIN, 0);
        spi_sig_io_out(SPI4_DAT_PIN, (bool)(byte & 0x80));
        spi_sig_delay();
        spi_sig_io_out(SPI4_CLK_PIN, 1);
        spi_sig_delay();
        byte <<= 1;
    }

    spi_sig_delay();

    /* DC=1 for data */
    if (para && para_num > 0) {
        spi_sig_io_out(SPI4_DC_PIN, 1);

        spi_sig_delay();

        for (size_t i = 0; i < para_num; i++) {
            byte = para[i];
            for (uint8_t j = 0; j < 8; j++) {
                spi_sig_io_out(SPI4_CLK_PIN, 0);
                spi_sig_io_out(SPI4_DAT_PIN, (bool)(byte & 0x80));
                spi_sig_delay();
                spi_sig_io_out(SPI4_CLK_PIN, 1);
                spi_sig_delay();
                byte <<= 1;
            }
        }
    }

    spi_sig_delay();

    spi_sig_io_out(SPI4_CS_PIN, 1);
    spi_sig_io_out(SPI4_CLK_PIN, 1);
    spi_sig_io_out(SPI4_DAT_PIN, 1);

    return 0;
}

#endif
