/**
 * @file display_board_gpio.c
 * @brief GPIO implementation of the board callbacks.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/display_board_gpio.h"
#include "display_os.h"

/** @brief Put power off, reset asserted and the backlight off, once per shared object. */
static int board_gpio_init(void *ctx)
{
    display_board_gpio_t *b = ctx;

    if (!b->inited) {
        display_pin_output(&b->power, false);
        display_pin_output(&b->reset, true); /* held in reset until the first power-on */
        display_pin_output(&b->backlight, false);
        b->inited = true;
    }
    return 0;
}

/** @brief Reference-counted power switch: first user powers on, last user powers off. */
static int board_gpio_power(void *ctx, bool on)
{
    display_board_gpio_t *b = ctx;

    if (on) {
        if (b->users++ == 0) {
            display_pin_set(&b->power, true);
        }
    } else if (b->users && --b->users == 0) {
        /* Hold the IC in reset before power is cut */
        display_pin_set(&b->reset, true);
        display_pin_set(&b->power, false);
    }
    return 0;
}

/** @brief Drive the shared reset line; ignored while another panel is running. */
static int board_gpio_reset(void *ctx, bool asserted)
{
    display_board_gpio_t *b = ctx;

    /* Shared reset line: only the first powered panel resets, otherwise a running panel is reset */
    if (b->users <= 1) {
        display_pin_set(&b->reset, asserted);
    }
    return 0;
}

/** @brief Switch the backlight GPIO: on for any level above 0. */
static int board_gpio_backlight(void *ctx, uint16_t level)
{
    display_board_gpio_t *b = ctx;

    if (!display_pin_used(&b->backlight)) {
        return -ENOTSUP;
    }
    display_pin_set(&b->backlight, level > 0);
    return 0;
}

/** @brief Callbacks of the GPIO implementation. */
const display_board_ops_t display_board_gpio_ops = {
    .init = board_gpio_init,
    .power = board_gpio_power,
    .reset = board_gpio_reset,
    .backlight = board_gpio_backlight,
};
