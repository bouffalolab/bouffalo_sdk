/**
 * @file main.c
 * @brief LED matrix solution
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

#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"
#include "bflb_led_matrix.h"
#include "shell.h"

static struct bflb_device_s *uart0;

typedef enum {
    DISPLAY_MODE_IDLE = 0,
    DISPLAY_MODE_TIMER,
    DISPLAY_MODE_PALINDROME,
    DISPLAY_MODE_SCAN,
    DISPLAY_MODE_RAIN,
    DISPLAY_MODE_EXPAND,
} display_mode_t;

static volatile display_mode_t display_mode = DISPLAY_MODE_IDLE;

static uint64_t timer_start_time = 0;
static uint64_t timer_last_update = 0;
static uint32_t timer_base_seconds = 0;

static uint64_t palindrome_last_update = 0;
static uint8_t palindrome_step = 0;
static uint8_t palindrome_brightness = 100;

static uint64_t animation_last_update = 0;
static uint16_t animation_step = 0;
static uint8_t animation_brightness = 100;

static int animation_start(display_mode_t mode, int argc, char **argv, const char *name)
{
    uint32_t brightness = animation_brightness;

    if (argc > 2) {
        printf("Usage: %s [brightness]\r\n", name);
        printf("  brightness: Optional value, 0-100\r\n");
        return 1;
    }

    if (argc == 2) {
        brightness = atoi(argv[1]);
        if (brightness > 100) {
            printf("Error: brightness must be 0-100\r\n");
            return 1;
        }
    }

    animation_brightness = (uint8_t)brightness;
    animation_step = 0;
    animation_last_update = 0;
    display_mode = mode;
    printf("%s animation started\r\n", name);
    return 0;
}

static void animation_show_frame(const uint8_t *pixels)
{
    led_matrix_set_rectangle(0, 0, LED_MATRIX_ROWS, LED_MATRIX_COLS, pixels);
    led_matrix_update();
}

static void display_timer_value(uint32_t seconds)
{
    uint8_t uni = seconds % 10;
    uint8_t dec = (seconds / 10) % 10;

    led_matrix_clear();
    led_matrix_set_number(1, 2, dec);
    led_matrix_set_number(1, 6, uni);
    led_matrix_update();
}

int timer_start(int argc, char **argv)
{
    uint32_t start_seconds = 0;

    if (argc > 2) {
        printf("Usage: timer_start [seconds]\r\n");
        printf("  seconds: Optional start value, 0-99\r\n");
        return 1;
    }

    if (argc == 2) {
        start_seconds = atoi(argv[1]);
        if (start_seconds > 99) {
            printf("Error: seconds must be 0-99\r\n");
            return 1;
        }
    }

    display_mode = DISPLAY_MODE_TIMER;
    timer_base_seconds = start_seconds;
    timer_start_time = bflb_mtimer_get_time_ms();
    timer_last_update = 0;
    display_timer_value(timer_base_seconds);

    printf("Timer display started\r\n");
    return 0;
}

int display_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    display_mode = DISPLAY_MODE_IDLE;
    led_matrix_clear();
    led_matrix_update();
    printf("Display stopped and cleared\r\n");

    return 0;
}

void timer_process(void)
{
    uint64_t now = bflb_mtimer_get_time_ms();

    if (now - timer_last_update < 1000) {
        return;
    }

    timer_last_update = now;
    uint32_t seconds = timer_base_seconds + (uint32_t)((now - timer_start_time) / 1000);

    if (seconds > 99) {
        timer_start_time = now;
        timer_base_seconds = 0;
        seconds = 0;
    }

    display_timer_value(seconds);
    printf("Time: %02u\r\n", seconds % 100);
}

int number_show(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: number <value>\r\n");
        printf("  value: Number to display, 0-99\r\n");
        return 1;
    }

    uint32_t number = atoi(argv[1]);
    if (number > 99) {
        printf("Error: number must be 0-99\r\n");
        return 1;
    }

    display_mode = DISPLAY_MODE_IDLE;
    led_matrix_clear();
    if (number < 10) {
        led_matrix_set_number(1, 4, (uint8_t)number);
    } else {
        led_matrix_set_number(1, 2, (uint8_t)(number / 10));
        led_matrix_set_number(1, 6, (uint8_t)(number % 10));
    }
    led_matrix_update();

    printf("Number %u displayed\r\n", number);
    return 0;
}

int brightness_pixel(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usage: pixel <row> <col> <brightness>\r\n");
        printf("  row: 0-%d\r\n", LED_MATRIX_ROWS - 1);
        printf("  col: 0-%d\r\n", LED_MATRIX_COLS - 1);
        printf("  brightness: 0-100\r\n");
        return 1;
    }

    uint32_t row = atoi(argv[1]);
    uint32_t col = atoi(argv[2]);
    uint32_t brightness = atoi(argv[3]);

    if (row >= LED_MATRIX_ROWS) {
        printf("Error: row must be 0-%d\r\n", LED_MATRIX_ROWS - 1);
        return 1;
    }
    if (col >= LED_MATRIX_COLS) {
        printf("Error: col must be 0-%d\r\n", LED_MATRIX_COLS - 1);
        return 1;
    }
    if (brightness > 100) {
        printf("Error: brightness must be 0-100\r\n");
        return 1;
    }

    display_mode = DISPLAY_MODE_IDLE;
    led_matrix_set_pixel((uint8_t)row, (uint8_t)col, (uint8_t)brightness);
    led_matrix_update();
    printf("Pixel (%u,%u) brightness set to %u%%\r\n", row, col, brightness);
    return 0;
}

int brightness_all(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: all <brightness>\r\n");
        printf("  brightness: 0-100\r\n");
        return 1;
    }

    uint32_t brightness = atoi(argv[1]);
    if (brightness > 100) {
        printf("Error: brightness must be 0-100\r\n");
        return 1;
    }

    display_mode = DISPLAY_MODE_IDLE;
    uint8_t pixels[LED_MATRIX_ROWS * LED_MATRIX_COLS];
    for (uint32_t i = 0; i < sizeof(pixels); i++) {
        pixels[i] = (uint8_t)brightness;
    }
    led_matrix_set_rectangle(0, 0, LED_MATRIX_ROWS, LED_MATRIX_COLS, pixels);
    led_matrix_update();

    printf("All pixels set to %u%%\r\n", brightness);
    return 0;
}

int palindrome_start(int argc, char **argv)
{
    uint32_t brightness = palindrome_brightness;

    if (argc > 2) {
        printf("Usage: palindrome_start [brightness]\r\n");
        printf("  brightness: Optional value, 0-100\r\n");
        return 1;
    }

    if (argc == 2) {
        brightness = atoi(argv[1]);
        if (brightness > 100) {
            printf("Error: brightness must be 0-100\r\n");
            return 1;
        }
    }

    palindrome_brightness = (uint8_t)brightness;
    palindrome_step = 0;
    palindrome_last_update = 0;
    display_mode = DISPLAY_MODE_PALINDROME;

    printf("Palindrome display started\r\n");
    return 0;
}

void palindrome_process(void)
{
    uint64_t now = bflb_mtimer_get_time_ms();

    if (now - palindrome_last_update < 250) {
        return;
    }

    uint8_t pixels[LED_MATRIX_ROWS * LED_MATRIX_COLS] = { 0 };

    palindrome_last_update = now;
    for (uint8_t row = 0; row < LED_MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < (LED_MATRIX_COLS + 1) / 2; col++) {
            uint8_t mirror_col = LED_MATRIX_COLS - 1 - col;
            uint8_t value = ((row + col + palindrome_step) % 3 == 0) ? palindrome_brightness : 0;

            pixels[row * LED_MATRIX_COLS + col] = value;
            if (mirror_col != col) {
                pixels[row * LED_MATRIX_COLS + mirror_col] = value;
            }
        }
    }

    animation_show_frame(pixels);
    palindrome_step++;
}

int scan_start(int argc, char **argv)
{
    return animation_start(DISPLAY_MODE_SCAN, argc, argv, "scan_start");
}

void scan_process(void)
{
    uint64_t now = bflb_mtimer_get_time_ms();
    uint8_t pixels[LED_MATRIX_ROWS * LED_MATRIX_COLS] = { 0 };
    uint8_t position;
    uint8_t period = LED_MATRIX_COLS * 2 - 2;

    if (now - animation_last_update < 70) {
        return;
    }

    animation_last_update = now;
    position = animation_step % period;
    if (position >= LED_MATRIX_COLS) {
        position = period - position;
    }

    for (uint8_t row = 0; row < LED_MATRIX_ROWS; row++) {
        for (uint8_t distance = 0; distance < 3; distance++) {
            if (position >= distance) {
                pixels[row * LED_MATRIX_COLS + position - distance] = animation_brightness / (distance + 1);
            }
        }
    }

    animation_show_frame(pixels);
    animation_step++;
}

int rain_start(int argc, char **argv)
{
    return animation_start(DISPLAY_MODE_RAIN, argc, argv, "rain_start");
}

void rain_process(void)
{
    uint64_t now = bflb_mtimer_get_time_ms();
    uint8_t pixels[LED_MATRIX_ROWS * LED_MATRIX_COLS] = { 0 };

    if (now - animation_last_update < 110) {
        return;
    }

    animation_last_update = now;
    for (uint8_t col = 0; col < LED_MATRIX_COLS; col++) {
        uint8_t head = (animation_step + col * 5) % (LED_MATRIX_ROWS + 4);

        for (uint8_t tail = 0; tail < 4; tail++) {
            if (head >= tail && head - tail < LED_MATRIX_ROWS) {
                pixels[(head - tail) * LED_MATRIX_COLS + col] = animation_brightness / (tail + 1);
            }
        }
    }

    animation_show_frame(pixels);
    animation_step++;
}

int expand_start(int argc, char **argv)
{
    return animation_start(DISPLAY_MODE_EXPAND, argc, argv, "expand_start");
}

void expand_process(void)
{
    uint64_t now = bflb_mtimer_get_time_ms();
    uint8_t pixels[LED_MATRIX_ROWS * LED_MATRIX_COLS] = { 0 };
    uint8_t max_radius = (LED_MATRIX_COLS - 1) / 2;
    uint8_t cycle = max_radius * 2;
    uint8_t radius = animation_step % cycle;

    if (now - animation_last_update < 120) {
        return;
    }

    animation_last_update = now;
    if (radius > max_radius) {
        radius = cycle - radius;
    }

    for (uint8_t row = 0; row < LED_MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < LED_MATRIX_COLS; col++) {
            uint8_t row_distance =
                row < LED_MATRIX_ROWS / 2 ? LED_MATRIX_ROWS / 2 - 1 - row : row - LED_MATRIX_ROWS / 2;
            uint8_t col_distance =
                col < LED_MATRIX_COLS / 2 ? LED_MATRIX_COLS / 2 - 1 - col : col - LED_MATRIX_COLS / 2;
            uint8_t distance = row_distance > col_distance ? row_distance : col_distance;

            if (distance <= radius) {
                pixels[row * LED_MATRIX_COLS + col] = animation_brightness;
            }
        }
    }

    animation_show_frame(pixels);
    animation_step++;
}

int pwm_gradient_show(int argc, char **argv)
{
    (void)argv;

    if (argc != 1) {
        printf("Usage: pwm_gradient\r\n");
        return 1;
    }

    display_mode = DISPLAY_MODE_IDLE;
    led_matrix_clear();

    for (uint8_t row = 0; row < LED_MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < LED_MATRIX_COLS; col++) {
            uint16_t pwm = row * LED_MATRIX_COLS + col;
            if (pwm > 100) {
                pwm = 100;
            }
            led_matrix_set_pixel(row, col, (uint8_t)pwm);
        }
    }
    led_matrix_update();

    printf("PWM gradient displayed\r\n");
    return 0;
}

int main(void)
{
    int ch;

    board_init();
    uart0 = bflb_device_get_by_name("uart0");

    printf("=== LED Matrix Shell Control ===\r\n");
    printf("Matrix size: %dx%d\r\n", LED_MATRIX_ROWS, LED_MATRIX_COLS);
    printf(
        "Commands: timer_start, stop, number, pixel, all, pwm_gradient, palindrome_start, scan_start, rain_start, expand_start\r\n");
    printf("Type 'help' for command details\r\n\r\n");

    int ret = led_matrix_init(board_led_matrix_gpio_config_get());
    if (ret < 0) {
        printf("LED matrix initialization failed: %d\r\n", ret);
        return ret;
    }
    led_matrix_on();
    led_matrix_clear();

    shell_init();

    while (1) {
        while ((ch = bflb_uart_getchar(uart0)) != -1) {
            shell_handler(ch);
        }

        switch (display_mode) {
            case DISPLAY_MODE_TIMER:
                timer_process();
                break;
            case DISPLAY_MODE_PALINDROME:
                palindrome_process();
                break;
            case DISPLAY_MODE_SCAN:
                scan_process();
                break;
            case DISPLAY_MODE_RAIN:
                rain_process();
                break;
            case DISPLAY_MODE_EXPAND:
                expand_process();
                break;
            case DISPLAY_MODE_IDLE:
            default:
                break;
        }
    }

    return 0;
}

/* 
timer_start [seconds]      // Start timer display, default 0, range 0-99
stop                       // Stop the active display or animation and clear

number <0-99>              // Display a one- or two-digit number
pixel <row> <col> <0-100>  // Set one pixel brightness
all <0-100>                // Set all pixels to one brightness

pwm_gradient               // Display a PWM brightness gradient
palindrome_start [0-100]   // Start a horizontally symmetric animation
scan_start [0-100]         // Start a left-right scanning animation
rain_start [0-100]         // Start a falling rain animation
expand_start [0-100]       // Start a center expand-contract animation
 */
SHELL_CMD_EXPORT_ALIAS(timer_start, timer_start, Start timer display.);
SHELL_CMD_EXPORT_ALIAS(display_stop, stop, Stop the active display and clear the matrix.);
SHELL_CMD_EXPORT_ALIAS(number_show, number, Display number 0 - 99.);
SHELL_CMD_EXPORT_ALIAS(brightness_pixel, pixel, Set single pixel brightness.);
SHELL_CMD_EXPORT_ALIAS(brightness_all, all, Set all pixels brightness.);
SHELL_CMD_EXPORT_ALIAS(pwm_gradient_show, pwm_gradient, Display PWM brightness gradient.);
SHELL_CMD_EXPORT_ALIAS(palindrome_start, palindrome_start, Start palindrome display.);
SHELL_CMD_EXPORT_ALIAS(scan_start, scan_start, Start horizontal scan animation.);
SHELL_CMD_EXPORT_ALIAS(rain_start, rain_start, Start falling rain animation.);
SHELL_CMD_EXPORT_ALIAS(expand_start, expand_start, Start center expand and contract animation.);
