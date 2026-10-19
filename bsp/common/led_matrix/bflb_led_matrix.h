/**
 * @file bflb_led_matrix.h
 * @brief LED matrix driver
 *
 * Copyright (c) 2026 Bouffalolab team
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

#ifndef _BFLB_LED_MATRIX_H_
#define _BFLB_LED_MATRIX_H_

#include "bflb_core.h"

/* LED_MATRIX_CONFIG led matrix configuration definition */
#define LED_MATRIX_ROWS                            8
#define LED_MATRIX_COLS                            12
#define LED_MATRIX_BRIGHTNESS_LEVELS               100 /* 1% precision brightness control */
#define LED_MATRIX_REFRESH_RATE                    240 /* Matrix refresh rate in Hz (target 240Hz) */
#define LED_MATRIX_BLANKING_PERCENT                4   /* Four extra blanking slots per row */
#define LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING (LED_MATRIX_BRIGHTNESS_LEVELS + LED_MATRIX_BLANKING_PERCENT)
#define LED_MATRIX_COMMON_CATHODE                  0 /* 0=row active low/column active low */
#define LED_MATRIX_NUMBER_HEIGHT                   5
#define LED_MATRIX_NUMBER_WIDTH                    3

/** @brief Board wiring. The configuration must remain valid while the driver is in use. */
typedef struct {
    uint16_t row_gpios[LED_MATRIX_ROWS];
    uint16_t col_gpios[LED_MATRIX_COLS];
} led_matrix_gpio_config_t;

#define GLB_GPIO_OUTPUT_LOW  0x20000ae4
#define GLB_GPIO_OUTPUT_HIGH 0x20000ae8

/* LED_MATRIX_DMA led matrix dma configuration definition */
#define LED_MATRIX_DMA_CH    0 /* DMA0 channel 0 for both scan backends */

/* BL616CL TIMER0 timing calculations. SPI timing uses the runtime peripheral clock. */
#if defined(BL616CL)
#define LED_MATRIX_TIMER_CLOCK_HZ     40000000UL /* TIMER0 XTAL clock, clock_div = 0 */
#define LED_MATRIX_TIMER_CLOCK_DIV    0U
#define LED_MATRIX_DMA_WORDS_PER_STEP 2U /* GPIO LOW and HIGH, one request per 32-bit word */
/* Round to the nearest TIMER tick instead of truncating to whole microseconds. */
#define LED_MATRIX_TIMER_COMP_TICKS                                                                              \
    ((LED_MATRIX_TIMER_CLOCK_HZ + (LED_MATRIX_REFRESH_RATE * LED_MATRIX_ROWS *                                   \
                                   LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING * LED_MATRIX_DMA_WORDS_PER_STEP) / \
                                      2U) /                                                                      \
     (LED_MATRIX_REFRESH_RATE * LED_MATRIX_ROWS * LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING *                   \
      LED_MATRIX_DMA_WORDS_PER_STEP))
#define LED_MATRIX_ROW_TIME_US                                                  \
    ((uint32_t)(((LED_MATRIX_TIMER_COMP_TICKS * LED_MATRIX_DMA_WORDS_PER_STEP * \
                  LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING * 1000000ULL) +    \
                 LED_MATRIX_TIMER_CLOCK_HZ / 2U) /                              \
                LED_MATRIX_TIMER_CLOCK_HZ))
#define LED_MATRIX_ONE_PERCENT_US                                                             \
    ((uint32_t)(((LED_MATRIX_TIMER_COMP_TICKS * LED_MATRIX_DMA_WORDS_PER_STEP * 1000000ULL) + \
                 LED_MATRIX_TIMER_CLOCK_HZ / 2U) /                                            \
                LED_MATRIX_TIMER_CLOCK_HZ))

#endif

/**
 * @brief LED matrix pixel structure
 */
typedef struct {
    uint8_t brightness; /* 0-100, 1% precision */
} led_matrix_pixel_t;

/**
 * @brief LED matrix buffer structure
 */
typedef struct {
    led_matrix_pixel_t pixels[LED_MATRIX_ROWS][LED_MATRIX_COLS];
    uint8_t is_enabled;
} led_matrix_buffer_t;

/**
 * @brief Initialize the LED matrix hardware
 *
 * @param config Board-specific row and column GPIO configuration
 *
 * @retval 0 on success, -EINVAL for invalid pins, -EBUSY if scanning is active
 *
 * @note This function initializes the LED matrix peripheral, configures the GPIO pins,
 *       sets up DMA channels and prepares the scanning mechanism.
 *       Must be called before using any other LED matrix functions.
 */
int led_matrix_init(const led_matrix_gpio_config_t *config);

/**
 * @brief Turn on the LED matrix
 *
 * @param None
 *
 * @retval None
 *
 * @note This function enables the LED matrix scanning and starts the DMA-based
 *       multiplexing operation.
 */
void led_matrix_on(void);

/**
 * @brief Turn off the LED matrix
 *
 * @param None
 *
 * @retval None
 *
 * @note This function disables the LED matrix scanning and turns off all LEDs.
 *       The display data is preserved.
 */
void led_matrix_off(void);

/**
 * @brief Set the brightness of a specific LED pixel
 *
 * @param row Row index
 * @param col Column index
 * @param brightness Brightness level (0-100, where 100 = full brightness)
 *
 * @retval None
 */
void led_matrix_set_pixel(uint8_t row, uint8_t col, uint8_t brightness);

/**
 * @brief Get the brightness of a specific LED pixel
 *
 * @param row Row index
 * @param col Column index
 *
 * @retval Current brightness level (0-100)
 */
uint8_t led_matrix_get_pixel(uint8_t row, uint8_t col);

/**
 * @brief Clear all LEDs (set brightness to 0)
 *
 * @param None
 *
 * @retval None
 */
void led_matrix_clear(void);

/**
 * @brief Update the LED matrix display buffer
 *
 * @param None
 *
 * @retval None
 *
 * @note Drawing functions only modify the pixel buffer. Call this once after
 *       completing a frame to refresh the DMA scanning buffer.
 */
void led_matrix_update(void);

/**
 * @brief Set entire row brightness values
 *
 * @param row Row index
 * @param brightness Array of LED_MATRIX_COLS brightness values (0-100 each)
 *
 * @retval None
 */
void led_matrix_set_row(uint8_t row, const uint8_t *brightness);

/**
 * @brief Set entire column brightness values
 *
 * @param col Column index
 * @param brightness Array of 8 brightness values (0-100 each)
 *
 * @retval None
 */
void led_matrix_set_column(uint8_t col, const uint8_t *brightness);

/**
 * @brief Set the brightness of a rectangular LED pixel area
 *
 * @param row Start row index
 * @param col Start column index
 * @param height Rectangle height in pixels
 * @param width Rectangle width in pixels
 * @param brightness Row-major brightness array, height * width values (0-100 each)
 *
 * @retval None
 */
void led_matrix_set_rectangle(uint8_t row, uint8_t col, uint8_t height, uint8_t width, const uint8_t *brightness);

/**
 * @brief Display a 3x5 number at the specified position
 *
 * @param row Start row index
 * @param col Start column index
 * @param num Number to display (0-9)
 *
 * @retval None
 */
void led_matrix_set_number(uint8_t row, uint8_t col, uint8_t num);

#endif
