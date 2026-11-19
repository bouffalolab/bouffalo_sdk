/**
 * @file display_board.h
 * @brief Board control of a panel: power, reset and backlight.
 *
 * Pins and sequencing belong to the board, which provides the callbacks; a plain GPIO implementation
 * is in display_board_gpio.h.
 *
 * The backlight is optional and has a level from 0 (off) to DISPLAY_BACKLIGHT_MAX; a GPIO backlight
 * only distinguishes 0 from not 0, a PWM or I/O expander backlight is implemented by your own callback.
 * It is off after the display is initialized: switch it on once the first picture is shown.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_PANEL_BOARD_H
#define DISPLAY_PANEL_BOARD_H

#include "display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_BOARD Board control
 * @{
 */

/** @brief Highest backlight level (full brightness); 0 is off. */
#define DISPLAY_BACKLIGHT_MAX 1000u

/**
 * @brief Board callbacks. Every callback is optional.
 *
 * Every callback returns 0 on success or a negative errno value. When several panels share power or
 * reset lines, the implementation must count users. Callbacks are called from task context only.
 */
typedef struct {
    /** @brief Put the pins at their inactive level. May be called more than once. */
    int (*init)(void *ctx);
    /** @brief Switch the panel power. */
    int (*power)(void *ctx, bool on);
    /** @brief Drive the reset line; the pulse width is decided by the panel driver. */
    int (*reset)(void *ctx, bool asserted);
    /**
     * @brief Set the backlight level, 0 (off) to @ref DISPLAY_BACKLIGHT_MAX. Return -ENOTSUP when the board
     *        has no backlight.
     */
    int (*backlight)(void *ctx, uint16_t level);
} display_board_ops_t;

/**
 * @brief Board control of one panel.
 */
typedef struct {
    const display_board_ops_t *ops; /**< Callbacks, NULL: no board control */
    void *ctx;                      /**< Callback context */
} display_board_t;

/** @brief No board control. */
#define DISPLAY_BOARD_NONE               { .ops = NULL, .ctx = NULL }

/**
 * @brief Board control implemented by your own callbacks, e.g. power and reset behind an I/O expander,
 *        or a PWM backlight.
 *
 * @code
 * static const display_board_ops_t my_ops = { .init = my_init, .power = my_power, .reset = my_reset,
 *                                             .backlight = my_backlight };
 * static display_dcs_panel_t panel = DISPLAY_DCS_PANEL(&io, &module, DISPLAY_BOARD_CUSTOM(&my_ops, &my_ctx));
 * @endcode
 */
#define DISPLAY_BOARD_CUSTOM(_ops, _ctx) { .ops = (_ops), .ctx = (_ctx) }

/** @} */ /* DISPLAY_BOARD */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_PANEL_BOARD_H */
