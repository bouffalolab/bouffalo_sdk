/**
 * @file display_bus_driver.h
 * @brief Bus core (L1): one object per physical controller (SPI0, DBI0, software SPI, DSI host, ...).
 *
 * The logic common to every bus is implemented once here:
 * - lazy initialization when the first endpoint is attached, release after the last one;
 * - the bus lock;
 * - reconfiguring the controller when the bus switches to another endpoint;
 * - software CS;
 * - expanding the command encoding (16-bit addresses);
 * - sending pixels by DMA whenever the backend can, for synchronous and asynchronous calls;
 * - finishing DMA transfers in one way, in the backend ISR: CS -> bus lock -> optional callback;
 * - statistics.
 *
 * One binary semaphore per bus means "the bus is in use". A synchronous DMA call takes it a second
 * time after starting the DMA: whenever it gets it back, its own transfer is finished, because nobody
 * can give the semaphore before that transfer completes. It then gives it back and returns.
 *
 * The core knows nothing about hardware CS: an endpoint whose CS is DISPLAY_PIN_NONE is simply never
 * selected by software. Whether such an endpoint may share the bus is decided by the backend when the
 * endpoint is attached (see display_bus_claim()).
 *
 * A backend declares the interfaces it can drive, checks and applies an endpoint's settings through
 * LHAL, and puts one message on the wire; it never touches the lock or CS.
 *
 * This header defines the bus backend contract and complete base object for static allocation.
 * Concrete backend headers include it. Applications using an existing bus need only display_bus.h;
 * applications sending commands or pixels through an endpoint use display_io.h.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_BUS_DRIVER_H
#define DISPLAY_BUS_DRIVER_H

#include "bus/display_bus.h"
#include "bus/display_io.h"
#include "display_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_BUS
 * @{
 */

/**
 * @brief Operations implemented by a bus backend.
 */
typedef struct {
    /**
     * @brief Called when the first endpoint is attached: look up devices, initialize controller and DMA.
     *        Optional.
     *
     * @param [in] bus bus
     * @retval 0 success
     * @retval -EINVAL invalid bus configuration
     * @retval -ENODEV controller or DMA device not found
     */
    int (*init)(display_bus_t *bus);
    /**
     * @brief Called when the last endpoint is detached, or after a failed first attach. Optional.
     *
     * @param [in] bus bus
     */
    void (*deinit)(display_bus_t *bus);
    /**
     * @brief Validate an endpoint configuration and fill io->caps: the interface must be one the bus is
     *        set up for, and the backend decides whether the endpoint can share the bus. Must not touch
     *        hardware.
     *
     * @param [in]     bus bus
     * @param [in,out] io  endpoint, io->caps is filled
     * @retval 0 success
     * @retval -ENOTSUP the interface or format is not supported
     * @retval -EBUSY the endpoint cannot share the bus
     */
    int (*attach)(display_bus_t *bus, display_io_t *io);
    /**
     * @brief Switch the controller to an endpoint: interface (and the board pin functions of it), clock,
     *        pixel format, ... through LHAL. Optional.
     *
     * @param [in] bus bus, its lock is held
     * @param [in] io  endpoint to switch to
     * @retval 0 success
     * @retval -ENOTSUP the endpoint settings cannot be applied
     */
    int (*apply)(display_bus_t *bus, display_io_t *io);
    /**
     * @brief Send one message synchronously; the wire is idle on return.
     *        The bus lock is held and CS is asserted by the core.
     *
     * @param [in]     bus bus
     * @param [in]     io  endpoint
     * @param [in,out] msg message (rx is written for a read)
     * @retval 0 success
     * @retval -ENOTSUP the message kind is not supported
     * @retval -E2BIG message too long
     * @retval -EIO hardware error
     */
    int (*write)(display_bus_t *bus, display_io_t *io, const display_io_msg_t *msg);
    /**
     * @brief Start a pixel message by DMA and return. When the last bit has left the pin, the
     *        backend calls display_bus_complete() from its ISR, exactly once per started transfer.
     *        An error return means nothing was started. Optional; without it pixels are sent by write().
     *
     * @param [in] bus bus, its lock is held
     * @param [in] io  endpoint
     * @param [in] msg pixel message; the buffer stays valid until display_bus_complete()
     * @retval 0 started
     * @retval -ENOTSUP the message cannot be sent by DMA
     * @retval -E2BIG message does not fit the DMA descriptors
     */
    int (*write_dma)(display_bus_t *bus, display_io_t *io, const display_io_msg_t *msg);
} display_bus_ops_t;

/**
 * @brief Bus object. Backend buses embed it as the first member `.base`.
 *
 * Only @ref ops is set statically (DISPLAY_BUS_BASE); the other fields are runtime state.
 */
struct display_bus {
    const display_bus_ops_t *ops; /**< Backend operations */
    display_os_sem_t lock;        /**< Bus lock */
    display_io_t *active;         /**< Endpoint of the current transaction */
    display_io_t *applied;        /**< Endpoint the controller is configured for; NULL: unknown */
    uint8_t users;                /**< Number of attached endpoints */
    bool lifecycle_busy;          /**< Attach/detach lifecycle transition in progress */
    display_done_cb_t done_cb;    /**< Asynchronous call in flight: its callback; NULL for a synchronous call */
    void *done_ctx;               /**< Callback context */
    const void *done_buf;         /**< Pixel buffer of the asynchronous call */
    display_bus_stats_t stats;    /**< Statistics */
};

/** @brief Static initializer of the bus base. */
#define DISPLAY_BUS_BASE(_ops) { .ops = (_ops) }

/**
 * @brief Backend helper for attach: decide whether an endpoint may join the bus.
 *
 * An endpoint without software CS (no CS wired, or CS driven by the controller) is always selected,
 * so it cannot share the bus with any other endpoint, in either order.
 *
 * @param [in]     bus       bus
 * @param [in,out] exclusive backend flag "an endpoint without CS owns the bus"; cleared by the backend
 *                           in its deinit, which runs when the last endpoint leaves
 * @param [in]     cfg       configuration of the endpoint being attached
 * @retval 0 the endpoint may join (and @p exclusive is updated)
 * @retval -EBUSY the endpoint cannot share the bus
 */
static inline int display_bus_claim(const display_bus_t *bus, bool *exclusive, const display_io_cfg_t *cfg)
{
    bool no_cs = !display_pin_used(&cfg->cs);

    if (*exclusive || (no_cs && bus->users)) {
        return -EBUSY;
    }
    *exclusive = no_cs;
    return 0;
}

/**
 * @brief Report that the DMA pixel message finished. Backend ISR only, exactly once per started transfer.
 *
 * The core releases CS and the bus lock (which also wakes a synchronous caller), then calls the callback
 * of an asynchronous call. Tasks cannot run before the ISR returns, so this order is invisible to them.
 *
 * @param [in] bus    bus
 * @param [in] result 0 or a negative errno
 */
void display_bus_complete(display_bus_t *bus, int result);

/**
 * @cond DISPLAY_INTERNAL
 * Internal: used by display_io.c, the display core and the bus backends.
 */
int display_bus_attach(display_io_t *io);
int display_bus_detach(display_io_t *io);
int display_bus_xfer(display_io_t *io, const display_io_msg_t *msgs, size_t count, display_done_cb_t cb, void *ctx);
int display_bus_hold(display_bus_t *bus);
void display_bus_unhold(display_bus_t *bus);
/** @endcond */

/** @} */ /* DISPLAY_BUS */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_BUS_DRIVER_H */
