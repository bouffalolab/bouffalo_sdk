/**
 * @file display_default_board.h
 * @brief Board part of the default display, shared by the two screen types: panel power, reset and backlight.
 *
 * They are optional GPIOs, or implemented by the project (CONFIG_DISPLAY_DEFAULT_BOARD_CUSTOM) for example on
 * an I/O expander or a PWM. The header defines static objects: include it in exactly one source file.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_DEFAULT_BOARD_H
#define DISPLAY_DEFAULT_BOARD_H

#include "panel/display_board_gpio.h"

#if defined(CONFIG_DISPLAY_DEFAULT_BOARD_CUSTOM)
extern const display_board_ops_t display_default_board_ops;

#define DEFAULT_PANEL_BOARD DISPLAY_BOARD_CUSTOM(&display_default_board_ops, NULL)
#else
static display_board_gpio_t default_board = {
#if defined(CONFIG_DISPLAY_DEFAULT_POWER_USED)
    .power = DISPLAY_PIN_HIGH(CONFIG_DISPLAY_DEFAULT_POWER_PIN),
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_RESET_USED)
    .reset = DISPLAY_PIN_LOW(CONFIG_DISPLAY_DEFAULT_RESET_PIN),
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_BACKLIGHT_GPIO)
#if defined(CONFIG_DISPLAY_DEFAULT_BACKLIGHT_ACTIVE_HIGH)
    .backlight = DISPLAY_PIN_HIGH(CONFIG_DISPLAY_DEFAULT_BACKLIGHT_PIN),
#else
    .backlight = DISPLAY_PIN_LOW(CONFIG_DISPLAY_DEFAULT_BACKLIGHT_PIN),
#endif
#endif
};

#define DEFAULT_PANEL_BOARD DISPLAY_BOARD_GPIO(&default_board)
#endif

#endif /* DISPLAY_DEFAULT_BOARD_H */
