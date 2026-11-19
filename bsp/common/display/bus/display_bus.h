/**
 * @file display_bus.h
 * @brief Shared display bus: borrow a controller and query its statistics.
 *
 * Each physical controller has one bus. IO endpoints attach to it through
 * display_io_init(); the first endpoint initializes the controller and the last
 * one releases it. Transfers on different endpoints of one bus are serialized.
 *
 * Applications may borrow an initialized bus for a non-display device through
 * display_bus_acquire() / display_bus_release(). To define a bus and its IO
 * endpoints, include the concrete backend header, e.g. bus/spi/display_spi.h.
 * Backend implementation types and helpers are in bus/display_bus_driver.h.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_BUS_H
#define DISPLAY_BUS_H

#include "display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_BUS Bus core
 * @{
 */

/** @brief Bus object, shared by endpoints on one physical controller. */
typedef struct display_bus display_bus_t;

/**
 * @brief Bus statistics.
 */
typedef struct {
    uint32_t xfers;     /**< Completed transfers (display_io_xfer / _async) */
    uint32_t dma_xfers; /**< Of which the pixels went by DMA (synchronous or asynchronous call) */
    uint32_t applies;   /**< Controller reconfigurations (endpoint switches) */
    uint32_t errors;    /**< Failed transfers */
    uint32_t acquires;  /**< display_bus_acquire() by non-display devices */
} display_bus_stats_t;

/**
 * @brief Borrow the bus for a non-display device (touch, flash, ...) on the same controller.
 *
 * The next display transfer reconfigures the controller after the bus is released.
 *
 * @param [in] bus        bus with at least one attached endpoint
 * @param [in] timeout_ms upper bound of waiting
 * @retval 0 success
 * @retval -EINVAL the bus is not initialized
 * @retval -EPERM called from an ISR
 * @retval -ETIMEDOUT bus busy
 */
int display_bus_acquire(display_bus_t *bus, uint32_t timeout_ms);

/**
 * @brief Return a bus borrowed with display_bus_acquire().
 *
 * @param [in] bus bus
 */
void display_bus_release(display_bus_t *bus);

/**
 * @brief Get the bus statistics.
 *
 * @param [in]  bus   bus
 * @param [out] stats statistics
 * @retval 0 success
 * @retval -EINVAL invalid argument
 */
int display_bus_get_stats(display_bus_t *bus, display_bus_stats_t *stats);

/** @} */ /* DISPLAY_BUS */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_BUS_H */
