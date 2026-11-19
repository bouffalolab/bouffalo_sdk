/**
 * @file display_soft_dbi_c.h
 * @brief Software DBI Type C (3-wire / 4-wire serial) bus backend, bit-banged on GPIO.
 *
 * Synchronous only. Typical use: the command port of RGB panels; it can also drive small GRAM panels.
 * Pixel formats: RGB565 (high byte first), RGB888 and XRGB8888 (sent as R, G, B). Slow.
 *
 * Interfaces: DISPLAY_IFACE_DBI_C3 (3-wire, D/C is the 9th bit) always; DISPLAY_IFACE_DBI_C4 (4-wire) when
 * the bus has a DC pin. The pins belong to the bus; an endpoint only adds its CS and its interface.
 * The clock is fixed (SPI mode 0, as fast as the GPIO writes allow): freq_hz is ignored.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_SOFT_DBI_C_H
#define DISPLAY_SOFT_DBI_C_H

#include "bus/display_bus_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_BUS_SOFT_DBI_C Software DBI Type C bus backend
 * @{
 */

struct bflb_device_s;

/**
 * @brief Software DBI Type C bus configuration.
 */
typedef struct {
    display_pin_t sck;  /**< Clock */
    display_pin_t mosi; /**< Data out */
    display_pin_t miso; /**< Data in; DISPLAY_PIN_NONE: no read (3-wire bidirectional read is not supported) */
    display_pin_t dc;   /**< D/CX for the 4-wire interface, "active" means data, usually DISPLAY_PIN_HIGH(n);
                             DISPLAY_PIN_NONE: the bus supports 3-wire only */
} display_soft_dbi_c_bus_cfg_t;

/**
 * @brief Software DBI Type C bus object.
 */
typedef struct {
    display_bus_t base;               /**< Bus core, must be first */
    display_soft_dbi_c_bus_cfg_t cfg; /**< Configuration */
    struct bflb_device_s *gpio;       /**< Runtime: GPIO device */
    bool exclusive;                   /**< Runtime: an endpoint without CS owns the bus */
} display_soft_dbi_c_bus_t;

/** @brief Backend operations. */
extern const display_bus_ops_t display_soft_dbi_c_bus_ops;

/** @brief Static definition of a software DBI Type C bus. Endpoints use DISPLAY_IO(). */
#define DISPLAY_SOFT_DBI_C_BUS(...)                                                   \
    {                                                                                 \
        .base = DISPLAY_BUS_BASE(&display_soft_dbi_c_bus_ops), .cfg = { __VA_ARGS__ } \
    }

/** @} */ /* DISPLAY_BUS_SOFT_DBI_C */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_SOFT_DBI_C_H */
