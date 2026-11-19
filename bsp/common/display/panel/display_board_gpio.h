/**
 * @file display_board_gpio.h
 * @brief GPIO implementation of the board callbacks: power, reset and backlight are plain GPIOs.
 *
 * @code
 * static display_board_gpio_t pwr_bc = { .power = DISPLAY_PIN_HIGH(22), .reset = DISPLAY_PIN_LOW(23),
 *                                        .backlight = DISPLAY_PIN_HIGH(17) };
 * ... .board = DISPLAY_BOARD_GPIO(&pwr_bc) ...
 * @endcode
 *
 * Pass the same object to panels sharing power/reset lines: only the first panel powered on resets
 * the line, and power is removed when the last panel is powered off.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_PANEL_BOARD_GPIO_H
#define DISPLAY_PANEL_BOARD_GPIO_H

#include "panel/display_board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_BOARD
 * @{
 */

/**
 * @brief GPIO board control, shareable between panels.
 */
typedef struct {
    display_pin_t power;     /**< "Active" = powered */
    display_pin_t reset;     /**< "Active" = in reset, usually DISPLAY_PIN_LOW(n) */
    display_pin_t backlight; /**< "Active" = backlight on; DISPLAY_PIN_NONE: no backlight (-ENOTSUP) */
    uint8_t users;           /**< Runtime: number of powered panels */
    bool inited;             /**< Runtime: pins configured */
} display_board_gpio_t;

/** @brief Callbacks of the GPIO implementation. */
extern const display_board_ops_t display_board_gpio_ops;

/** @brief Board control using a @ref display_board_gpio_t. */
#define DISPLAY_BOARD_GPIO(_b) { .ops = &display_board_gpio_ops, .ctx = (_b) }

/** @} */ /* DISPLAY_BOARD */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_PANEL_BOARD_GPIO_H */
