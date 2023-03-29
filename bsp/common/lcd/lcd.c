/**
 * @file lcd.c
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

#include "lcd.h"
#include "font.h"
#include "bflb_core.h"

#if defined(LCD_RESET_EN)
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#endif

uint8_t lcd_dir = 0;
uint16_t lcd_max_x = LCD_W - 1, lcd_max_y = LCD_H - 1;

/* MCU LCD Common interface */
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_SPI)

/**
 * @brief LCD init
 *
 * @return int
 */
int lcd_init(void)
{
    int res;

#if (defined(LCD_RESET_EN) && LCD_RESET_EN)
    struct bflb_device_s *gpio;

    /* gpio init */
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, LCD_RESET_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* lcd reset */
#if LCD_RESET_ACTIVE_LEVEL
    bflb_gpio_set(gpio, LCD_RESET_PIN);
#else
    bflb_gpio_reset(gpio, LCD_RESET_PIN);
#endif

    bflb_mtimer_delay_ms(LCD_RESET_HOLD_MS);

    /* lcd recovery */
#if LCD_RESET_ACTIVE_LEVEL
    bflb_gpio_reset(gpio, LCD_RESET_PIN);
#else
    bflb_gpio_set(gpio, LCD_RESET_PIN);
#endif

    bflb_mtimer_delay_ms(LCD_RESET_DELAY);
#endif

    res = _LCD_FUNC_DEFINE(init);

    return res;
}

static int lcd_async_callback_enable(bool enable)
{
    _LCD_FUNC_DEFINE(async_callback_enable, enable);
    return 0;
}

int lcd_async_callback_register(void (*callback)(void))
{
    _LCD_FUNC_DEFINE(async_callback_register, callback);
    return 0;
}

/**
 * @brief Set display direction and mir
 *
 * @param dir 0~3 : 0~270 Angle
 * @param mir_flag 0:normal  1:Horizontal Mirroring(if support)
 * @return int
 */
int lcd_set_dir(uint8_t dir, uint8_t mir_flag)
{
    dir %= 4;
    lcd_dir = dir;
    if (dir == 0 || dir == 2) {
        lcd_max_x = LCD_W - 1;
        lcd_max_y = LCD_H - 1;
    } else {
        lcd_max_x = LCD_H - 1;
        lcd_max_y = LCD_W - 1;
    }

    return _LCD_FUNC_DEFINE(set_dir, dir, mir_flag);
}

/**
 * @brief Draws a point at the specified position
 *
 * @param x  X coordinate
 * @param y  Y coordinate
 * @param color
 * @return int
 */
int lcd_draw_point(uint16_t x, uint16_t y, lcd_color_t color)
{
    _LCD_FUNC_DEFINE(draw_point, x, y, color);
    return 0;
}

/**
 * @brief Draw a monochrome rectangle (May be less efficient)
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param color
 * @return int
 */
int lcd_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color)
{
    _LCD_FUNC_DEFINE(draw_area, x1, y1, x2, y2, color);
    return 0;
}

/**
 * @brief Draw a picture in the designated area(blocking),Will wait for the drawing to finish
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param picture
 * @return int
 */
int lcd_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture)
{
    _LCD_FUNC_DEFINE(draw_picture_blocking, x1, y1, x2, y2, picture);
    return 0;
}

/**
 * @brief  Draw a picture in the designated area(nonblocking,if it supports),
 *  Must be calle lcd_draw_is_busy! and (lcd_draw_is_busyd()==1) before performing other drawing and changing  picture data!
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param picture
 * @return int
 */
int lcd_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture)
{
    _LCD_FUNC_DEFINE(draw_picture_nonblocking, x1, y1, x2, y2, picture);
    return 0;
}

/**
 * @brief Check if it is drawing, must call it After call lcd_draw_picture_nonblocking
 *
 * @return int 1:lcd Drawing,Prohibit other operations! 0:Drawing is over
 */
int lcd_draw_is_busy(void)
{
    return _LCD_FUNC_DEFINE(draw_is_busy);
}

/**
 * @brief clear lcd
 *
 * @param color
 * @return int
 */
int lcd_clear(lcd_color_t color)
{
    lcd_draw_area(0, 0, lcd_max_x, lcd_max_y, color);
    return 0;
}

/**
 * @brief
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param color
 * @return int
 */
int lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color)
{
    int xVariation, yVariation, temp;
    int absX, absY, i;
    xVariation = x2 - x1;
    yVariation = y2 - y1;
    absX = ABS(xVariation);
    absY = ABS(yVariation);

    if (absX > absY) {
        for (i = 0; i < absX + 1; i++) {
            temp = yVariation * 100 / absX * i / 100;

            if (xVariation > 0) {
                lcd_draw_point(x1 + i, y1 + temp, color);
            } else {
                lcd_draw_point(x1 - i, y1 + temp, color);
            }
        }
    } else {
        for (i = 0; i < absY + 1; i++) {
            temp = xVariation * 100 / absY * i / 100;

            if (yVariation > 0) {
                lcd_draw_point(x1 + temp, y1 + i, color);
            } else {
                lcd_draw_point(x1 + temp, y1 - i, color);
            }
        }
    }
    return 0;
}

/**
 * @brief
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param color
 * @return int
 */
int lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x2, y1, x2, y2, color);
    lcd_draw_line(x2, y2, x1, y2, color);
    lcd_draw_line(x1, y2, x1, y1, color);
    return 0;
}

/**
 * @brief draw a circle
 *
 * @param x coordinate
 * @param y coordinate
 * @param r
 * @param color
 * @return int
 */
int lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r, lcd_color_t color)
{
    int a = 0, b;
    int di;
    b = r;
    di = 3 - (r << 1);

    while (a <= b) {
        lcd_draw_point(x - b, y - a, color);
        lcd_draw_point(x + b, y - a, color);
        lcd_draw_point(x - a, y + b, color);
        lcd_draw_point(x - b, y - a, color);
        lcd_draw_point(x - a, y - b, color);
        lcd_draw_point(x + b, y + a, color);
        lcd_draw_point(x + a, y - b, color);
        lcd_draw_point(x + a, y + b, color);
        lcd_draw_point(x - b, y + a, color);
        a++;

        if (di < 0) {
            di += 4 * a + 6;
        } else {
            di += 10 + 4 * (a - b);
            b--;
        }

        lcd_draw_point(x + a, y + b, color);
    }
    return 0;
}

#if FONT_ASCII_16X8

/**
 * @brief Draw font(16*8) ,Use double buffer to speed up drawing
 *
 * @param x start coordinate
 * @param y start coordinate
 * @param color font color
 * @param bk_color Background color
 * @param str The string to be displayed
 * @param num  number of characters displayed
 * @return int
 */
int lcd_draw_str_ascii16(uint16_t x, uint16_t y, lcd_color_t color, lcd_color_t bk_color, uint8_t *str, uint8_t num)
{
    static lcd_color_t draw_buff[2][16 * 8];
    uint16_t buff_color_num;
    uint8_t buff_using_num = 0;
    uint8_t ch, temp;
    uint16_t x0 = x;

    lcd_async_callback_enable(false);

    for (uint16_t i = 0; i < num && str[i]; i++) {
        if (str[i] < 128) {
            if (x > lcd_max_x - 8) {
                x = x0;
                y += 16;
            }
            if (x > lcd_max_x - 8 || y > lcd_max_y - 16)
                break;

            ch = str[i];

            if (ch >= ' ') {
                ch = ch - ' ';
            } else if (ch == '\n') {
                x = x0;
                y += 16;
                continue;
            } else {
                continue;
            }

            buff_color_num = 0;
            for (uint8_t j = 0; j < 16; j++) {
                temp = font_ascii_16x8[ch * 16 + j];
                for (uint8_t k = 0; k < 8; k++) {
                    if (temp & (0x80 >> k))
                        draw_buff[buff_using_num][buff_color_num++] = color;
                    else
                        draw_buff[buff_using_num][buff_color_num++] = bk_color;
                }
            }

            while (lcd_draw_is_busy()) {
            };

            lcd_draw_picture_nonblocking(x, y, x + 7, y + 15, draw_buff[buff_using_num]);
            buff_using_num = !buff_using_num;
            x += 8;

        } else {
            continue;
        }
    }
    while (lcd_draw_is_busy()) {
    };

    lcd_async_callback_enable(true);
    return 0;
}
#endif

/* RGB LCD Common interface */
#elif (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)

/**
 * @brief lcd_dpi_init
 *
 * @param screen_buffer First screen memory
 * @return int
 */
int lcd_init(lcd_color_t *screen_buffer)
{
    return _LCD_FUNC_DEFINE(init, screen_buffer);
}

/**
 * @brief Switch the screen, If it is in single-screen mode, there is no effect
 *
 * @param screen_id screen id
 * @return int
 */
int lcd_screen_switch(lcd_color_t *screen_buffer)
{
    return _LCD_FUNC_DEFINE(screen_switch, screen_buffer);
}

/**
 * @brief Gets the id of the screen in use
 *
 * @return int  screen id
 */
lcd_color_t *lcd_get_screen_using(void)
{
    return _LCD_FUNC_DEFINE(get_screen_using);
}

int lcd_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    return _LCD_FUNC_DEFINE(frame_callback_register, callback_type, callback);
}

int lcd_draw_point(lcd_color_t *screen_buffer, uint16_t x, uint16_t y, lcd_color_t color)
{
    if (screen_buffer == NULL) {
        return -1;
    } else if (x >= LCD_W || y >= LCD_H) {
        return -2;
    }

    screen_buffer[x * LCD_W + y] = color;
    return 0;
}

int lcd_draw_area(lcd_color_t *screen_buffer, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color)
{
    if (screen_buffer == NULL) {
        return -1;
    } else if (x1 >= LCD_W || y1 >= LCD_H) {
        return -2;
    }

    screen_buffer += y1 * LCD_W;

    x2 = (x2 < LCD_W) ? x2 : (LCD_W - 1);
    y2 = (y2 < LCD_H) ? y2 : (LCD_H - 1);

    for (uint16_t i = y1; i <= y2; i++) {
        for (uint16_t j = x1; j <= x2; j++) {
            screen_buffer[j] = color;
        }
        screen_buffer += LCD_W;
    }
    return 0;
}

int lcd_clear(lcd_color_t *screen_buffer, lcd_color_t color)
{
    if (screen_buffer == NULL) {
        return -1;
    }

    return lcd_draw_area(screen_buffer, 0, 0, LCD_W, LCD_H, color);
}

int lcd_draw_picture(lcd_color_t *screen_buffer, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture)
{
    if (screen_buffer == NULL) {
        return -1;
    } else if (x1 >= LCD_W || y1 >= LCD_H || x2 >= LCD_W || y2 >= LCD_H) {
        return -2;
    }

    screen_buffer += y1 * LCD_W;

    for (uint16_t i = y1; i <= y2; i++) {
        for (uint16_t j = x1; j <= x2; j++) {
            screen_buffer[j] = *picture;
            picture++;
        }
        screen_buffer += LCD_W;
    }
    return 0;
}

int lcd_draw_line(lcd_color_t *screen_buffer, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color)
{
    if (screen_buffer == NULL) {
        return -1;
    } else if (x1 >= LCD_W || y1 >= LCD_H || x2 >= LCD_W || y2 >= LCD_H) {
        return -2;
    }

    if (x1 == x2) {
        if (y1 > y2) {
            uint16_t a = y1;
            y1 = y2;
            y2 = a;
        }
        screen_buffer += y1 * LCD_W;
        for (; y1 <= y2; y1++) {
            screen_buffer[x1] = color;
            screen_buffer += LCD_W;
        }
    } else if (y1 == y2) {
        if (x1 > x2) {
            uint16_t b = x1;
            x1 = x2;
            x2 = b;
        }
        screen_buffer += y1 * LCD_W;
        for (; x1 <= x2; x1++) {
            screen_buffer[x1] = color;
        }
    } else {
        int xVariation, yVariation, temp;
        int absX, absY, i;
        xVariation = x2 - x1;
        yVariation = y2 - y1;
        absX = ABS(xVariation);
        absY = ABS(yVariation);

        if (absX > absY) {
            for (i = 0; i < absX + 1; i++) {
                temp = yVariation * 100 / absX * i / 100;

                if (xVariation > 0) {
                    screen_buffer[(x1 + i) * LCD_W + (y1 + temp)] = color;
                } else {
                    screen_buffer[(x1 - i) * LCD_W + (y1 + temp)] = color;
                }
            }
        } else {
            for (i = 0; i < absY + 1; i++) {
                temp = xVariation * 100 / absY * i / 100;

                if (yVariation > 0) {
                    screen_buffer[(x1 + i) * LCD_W + (y1 + temp)] = color;
                } else {
                    screen_buffer[(x1 + i) * LCD_W + (y1 - temp)] = color;
                }
            }
        }
    }
    return 0;
}

int lcd_draw_rectangle(lcd_color_t *screen_buffer, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color)
{
    lcd_draw_line(screen_buffer, x1, y1, x2, y1, color);
    lcd_draw_line(screen_buffer, x2, y1, x2, y2, color);
    lcd_draw_line(screen_buffer, x2, y2, x1, y2, color);
    lcd_draw_line(screen_buffer, x1, y2, x1, y1, color);
    return 0;
}

int lcd_draw_circle(lcd_color_t *screen_buffer, uint16_t x, uint16_t y, uint16_t r, lcd_color_t color)
{
    int a = 0, b;
    int di;
    b = r;
    di = 1 - r;

    while (a <= b) {
        screen_buffer[(x - b) * LCD_W + (y - a)] = color;
        screen_buffer[(x + b) * LCD_W + (y - a)] = color;
        screen_buffer[(x - a) * LCD_W + (y + b)] = color;
        screen_buffer[(x - b) * LCD_W + (y - a)] = color;
        screen_buffer[(x - a) * LCD_W + (y - b)] = color;
        screen_buffer[(x + b) * LCD_W + (y + a)] = color;
        screen_buffer[(x + a) * LCD_W + (y - b)] = color;
        screen_buffer[(x + a) * LCD_W + (y + b)] = color;
        screen_buffer[(x - b) * LCD_W + (y + a)] = color;
        a++;

        if (di < 0) {
            di += (a << 1) + 3;
        } else {
            di += ((a - b) << 1) + 5;
            b--;
        }
    }
    return 0;
}

#if FONT_ASCII_16X8

/**
 * @brief Draw font(16*8)
 *
 * @param x start coordinate
 * @param y start coordinate
 * @param color font color
 * @param bk_color Background color
 * @param str The string to be displayed
 * @param num  number of characters displayed
 * @return int
 */
int lcd_draw_str_ascii16(lcd_color_t *screen_buffer, uint16_t x, uint16_t y, lcd_color_t color, lcd_color_t bk_color, uint8_t *str, uint8_t num)
{
    lcd_color_t draw_buff[16 * 8];
    uint16_t buff_color_num;
    uint8_t ch, temp;
    uint16_t x0 = x;

    for (uint16_t i = 0; i < num && str[i]; i++) {
        if (str[i] < 128) {
            // if (x > LCD_W - 8) {
            //     x = x0;
            //     y += 16;
            // }
            if (x > LCD_W - 8 || y > LCD_H - 16)
                break;

            ch = str[i];

            if (ch >= ' ') {
                ch = ch - ' ';
            } else if (ch == '\n') {
                x = x0;
                y += 16;
                continue;
            } else {
                continue;
            }

            buff_color_num = 0;
            for (uint8_t j = 0; j < 16; j++) {
                temp = font_ascii_16x8[ch * 16 + j];
                for (uint8_t k = 0; k < 8; k++) {
                    if (temp & (0x80 >> k))
                        draw_buff[buff_color_num++] = color;
                    else
                        draw_buff[buff_color_num++] = bk_color;
                }
            }

            lcd_draw_picture(screen_buffer, x, y, x + 7, y + 15, draw_buff);
            x += 8;

        } else {
            continue;
        }
    }

    return 0;
}
#endif

#endif