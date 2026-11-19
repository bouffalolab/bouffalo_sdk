/**
 * @file display_bus.c
 * @brief Bus core: lazy init, lock, endpoint switching, software CS, command encoding, DMA completion.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bus/display_bus_driver.h"

/*****************************************************************************
 * Attach / detach
 *****************************************************************************/

/**
 * @brief Serialize attach/detach transitions of one bus.
 *
 * The transfer semaphore cannot be used for the first attach because it does not exist until the
 * controller has been initialized.  Keep this state separate from the transfer lock and only hold
 * it while one task performs the lifecycle transition; the controller initialization itself runs
 * with interrupts enabled.
 */
static void bus_lifecycle_enter(display_bus_t *bus)
{
    for (;;) {
        uintptr_t flags = display_os_irq_save();

        if (!bus->lifecycle_busy) {
            bus->lifecycle_busy = true;
            display_os_irq_restore(flags);
            return;
        }
        display_os_irq_restore(flags);
        display_os_delay_ms(1);
    }
}

/** @brief Release the attach/detach lifecycle transition. */
static void bus_lifecycle_exit(display_bus_t *bus)
{
    uintptr_t flags = display_os_irq_save();

    bus->lifecycle_busy = false;
    display_os_irq_restore(flags);
}

/**
 * @brief Release the controller after the last endpoint left (or a failed first attach).
 */
static void bus_release_controller(display_bus_t *bus)
{
    if (bus->ops->deinit) {
        bus->ops->deinit(bus);
    }
    display_os_sem_deinit(&bus->lock);
}

/**
 * @brief The endpoint configuration names a real interface and format.
 */
static bool io_cfg_valid(const display_io_cfg_t *cfg)
{
    return cfg->iface > DISPLAY_IFACE_NONE && cfg->iface < DISPLAY_IFACE_COUNT &&
           (unsigned)cfg->format < DISPLAY_FMT_COUNT;
}

/**
 * @brief Check the endpoint against the capabilities the backend reported.
 */
static int io_caps_check(const display_io_t *io)
{
    if (io->caps.format_mask && !(io->caps.format_mask & DISPLAY_FMT_BIT(io->cfg.format))) {
        return -ENOTSUP;
    }
    if ((io->caps.flags & DISPLAY_IO_CAP_FIXED_WIRE) &&
        (io->cfg.format == DISPLAY_FMT_RGB565) != (io->wire == DISPLAY_WIRE_16)) {
        return -ENOTSUP;
    }
    return 0;
}

/**
 * @brief First endpoint of a bus: create the lock and initialize the controller.
 */
static int bus_acquire_controller(display_bus_t *bus)
{
    int ret = display_os_sem_init(&bus->lock);

    if (ret) {
        return ret;
    }
    ret = bus->ops->init ? bus->ops->init(bus) : 0;
    if (ret) {
        display_os_sem_deinit(&bus->lock);
        return ret;
    }
    bus->applied = NULL;
    bus->active = NULL;
    bus->done_cb = NULL;
    return 0;
}

int display_bus_attach(display_io_t *io)
{
    display_bus_t *bus = io->bus;
    bool first;
    int ret;

    if (!bus || !bus->ops || !bus->ops->attach || !bus->ops->write || !io_cfg_valid(&io->cfg)) {
        return -EINVAL;
    }

    bus_lifecycle_enter(bus);
    first = bus->users == 0;

    /* First endpoint: initialize the controller lazily. */
    if (first) {
        ret = bus_acquire_controller(bus);
        if (ret) {
            bus_lifecycle_exit(bus);
            return ret;
        }
    }

    /* Protect the CS setup and backend endpoint state from an in-flight transfer. */
    ret = display_os_sem_take(&bus->lock, CONFIG_DISPLAY_BUS_LOCK_TIMEOUT_MS);
    if (ret) {
        if (first) {
            bus_release_controller(bus);
        }
        bus_lifecycle_exit(bus);
        return ret;
    }

    io->link = NULL;
    io->wire = display_wire_resolve(io->cfg.format, io->cfg.wire);
    ret = bus->ops->attach(bus, io);
    if (!ret) {
        ret = io_caps_check(io);
    }
    if (ret) {
        display_os_sem_give(&bus->lock);
        if (first) {
            bus_release_controller(bus);
        }
        bus_lifecycle_exit(bus);
        return ret;
    }

    if (!bus->ops->write_dma) {
        io->caps.flags &= ~DISPLAY_IO_CAP_DMA;
    }
    display_pin_output(&io->cfg.cs, false); /* inactive level before the pin becomes an output */
    io->attached = true;
    bus->users++;
    display_os_sem_give(&bus->lock);
    bus_lifecycle_exit(bus);
    return 0;
}

int display_bus_detach(display_io_t *io)
{
    display_bus_t *bus = io->bus;
    int ret;

    bus_lifecycle_enter(bus);

    /* Wait for the in-flight transfer */
    ret = display_os_sem_take(&bus->lock, CONFIG_DISPLAY_BUS_LOCK_TIMEOUT_MS);
    if (ret) {
        bus_lifecycle_exit(bus);
        return ret;
    }
    if (bus->applied == io) {
        bus->applied = NULL;
    }
    io->attached = false;
    io->link = NULL;
    io->video = NULL;
    bus->users--;
    display_os_sem_give(&bus->lock);

    if (bus->users == 0) {
        bus_release_controller(bus);
    }
    bus_lifecycle_exit(bus);
    return 0;
}

/*****************************************************************************
 * Transfer
 *****************************************************************************/

/**
 * @brief End a transfer: statistics, endpoint, then release the bus.
 *        Called by the task (CPU transfer) or by the backend ISR (DMA transfer).
 */
static void bus_finish(display_bus_t *bus, int result, bool dma)
{
    if (result) {
        bus->applied = NULL; /* controller state unknown: reconfigure next time */
        bus->stats.errors++;
    } else {
        bus->stats.xfers++;
        if (dma) {
            bus->stats.dma_xfers++;
        }
    }
    bus->active = NULL;
    display_os_sem_give(&bus->lock); /* for a synchronous DMA call this also wakes the caller */
}

/**
 * @brief Begin a transfer: take the bus for an endpoint and switch the controller to it if needed.
 *        bus_finish() ends it.
 */
static int bus_begin(display_bus_t *bus, display_io_t *io)
{
    int ret;

    ret = display_os_sem_take(&bus->lock, CONFIG_DISPLAY_BUS_LOCK_TIMEOUT_MS);
    if (ret) {
        return ret;
    }
    bus->active = io;
    if (bus->applied != io) {
        ret = bus->ops->apply ? bus->ops->apply(bus, io) : 0;
        if (ret) {
            bus_finish(bus, ret, false);
            return ret;
        }
        bus->applied = io;
        bus->stats.applies++;
    }
    return 0;
}

/**
 * @brief The command of the message travels as two bytes: the endpoint uses 16-bit indexed addressing.
 */
static bool cmd16_applies(const display_io_t *io, const display_io_msg_t *msg)
{
    return display_io_get_cmd_mode(io) == DISPLAY_CMD_16_INDEXED && msg->cmd != DISPLAY_IO_NO_CMD;
}

/**
 * @brief Send one message with 16-bit indexed addressing (NT35510 on 8080).
 *
 * Parameters: [cmd] [index | 0x00, value] for each parameter.
 * No parameter or pixels: [cmd] [0x00 | data].
 */
static int write_cmd16(display_bus_t *bus, display_io_t *io, const display_io_msg_t *msg)
{
    const display_io_msg_t hi = { .cmd = msg->cmd };
    const uint8_t *param = msg->tx;
    int ret;

    if ((msg->flags & DISPLAY_IO_MSG_COLOR) || !msg->len) {
        display_io_msg_t lo = *msg;

        lo.cmd = 0;
        ret = bus->ops->write(bus, io, &hi);
        return ret ? ret : bus->ops->write(bus, io, &lo);
    }
    for (uint32_t i = 0; i < msg->len; i++) {
        uint8_t value[2] = { 0, param[i] };
        display_io_msg_t lo = { .cmd = i, .tx = value, .len = 2 };

        ret = bus->ops->write(bus, io, &hi);
        if (!ret) {
            ret = bus->ops->write(bus, io, &lo);
        }
        if (ret) {
            return ret;
        }
    }
    return 0;
}

/**
 * @brief Send one message with the command encoding of the endpoint.
 */
static int bus_write_msg(display_bus_t *bus, display_io_t *io, const display_io_msg_t *msg)
{
    if (cmd16_applies(io, msg)) {
        return write_cmd16(bus, io, msg);
    }
    return bus->ops->write(bus, io, msg);
}

/**
 * @brief Start the last message by DMA. CS is asserted and the bus is held by the caller.
 *
 * @param cb callback of an asynchronous call, NULL for a synchronous call
 */
static int start_dma(display_bus_t *bus, display_io_t *io, const display_io_msg_t *msg, display_done_cb_t cb, void *ctx)
{
    display_io_msg_t last = *msg;

    if (cmd16_applies(io, &last)) {
        /* High address byte now; the low byte (0) travels with the DMA message */
        const display_io_msg_t hi = { .cmd = last.cmd };
        int ret = bus->ops->write(bus, io, &hi);

        if (ret) {
            return ret;
        }
        last.cmd = 0;
    }
    /* Set before starting: the completion interrupt may fire before write_dma() returns.
     * The bus is held and no transfer is in flight, so the ISR cannot read these meanwhile. */
    bus->done_cb = cb;
    bus->done_ctx = ctx;
    bus->done_buf = last.tx;
    if (!io->cfg.dcache_dis) {
        display_os_dcache_clean(last.tx, last.len); /* the DMA reads memory, not the cache */
    }
    return bus->ops->write_dma(bus, io, &last);
}

/**
 * @brief Wait for the DMA transfer of a synchronous call: getting the bus lock back means it is done (see
 *        display_bus_driver.h).
 */
static void bus_wait_dma_done(display_bus_t *bus)
{
    display_os_sem_take(&bus->lock, DISPLAY_OS_WAIT_FOREVER);
    display_os_sem_give(&bus->lock);
}

int display_bus_xfer(display_io_t *io, const display_io_msg_t *msgs, size_t count, display_done_cb_t cb, void *ctx)
{
    display_bus_t *bus = io->bus;
    const display_io_msg_t *last = &msgs[count - 1];
    /* Pixels go by DMA whenever the endpoint can, for synchronous and asynchronous calls alike */
    bool dma = (io->caps.flags & DISPLAY_IO_CAP_DMA) && (last->flags & DISPLAY_IO_MSG_COLOR) && !last->delay_ms;
    size_t cpu_count = dma ? count - 1 : count;
    bool selected = false;
    int ret;

    ret = bus_begin(bus, io);
    if (ret) {
        return ret;
    }

    /* Messages sent by the CPU. CS stays asserted across messages flagged CS_KEEP; the last message
     * of the request always releases it. */
    for (size_t i = 0; i < cpu_count; i++) {
        if (!selected) {
            display_pin_set(&io->cfg.cs, true);
            selected = true;
        }
        ret = bus_write_msg(bus, io, &msgs[i]);
        if (ret || !(msgs[i].flags & DISPLAY_IO_MSG_CS_KEEP) || i == count - 1) {
            display_pin_set(&io->cfg.cs, false);
            selected = false;
        }
        if (ret) {
            break;
        }
        if (msgs[i].delay_ms) {
            display_os_delay_ms(msgs[i].delay_ms);
        }
    }

    /* Pixel message by DMA: CS is released by display_bus_complete() or on a start error */
    if (!ret && dma) {
        if (!selected) {
            display_pin_set(&io->cfg.cs, true);
        }
        ret = start_dma(bus, io, last, cb, ctx);
        if (!ret) {
            if (!cb) {
                bus_wait_dma_done(bus);
            }
            return 0; /* an asynchronous call is completed by display_bus_complete() */
        }
        display_pin_set(&io->cfg.cs, false);
    }
    bus_finish(bus, ret, false);

    /* Asynchronous call on a bus without DMA: same contract, called back from the calling task */
    if (!ret && cb) {
        cb(0, last->tx, ctx);
    }
    return ret;
}

void display_bus_complete(display_bus_t *bus, int result)
{
    display_done_cb_t cb = bus->done_cb;
    void *ctx = bus->done_ctx;
    const void *buf = bus->done_buf;

    display_pin_set(&bus->active->cfg.cs, false);
    bus_finish(bus, result, true);
    if (cb) {
        cb(result, buf, ctx);
    }
}

/*****************************************************************************
 * Sharing with non-display devices, statistics
 *****************************************************************************/

/**
 * @brief Take the bus lock of an initialized bus, from a task.
 */
static int bus_take(display_bus_t *bus, uint32_t timeout_ms)
{
    if (!bus || !bus->users) {
        return -EINVAL;
    }
    if (display_os_in_isr()) {
        return -EPERM;
    }
    return display_os_sem_take(&bus->lock, timeout_ms);
}

int display_bus_acquire(display_bus_t *bus, uint32_t timeout_ms)
{
    int ret = bus_take(bus, timeout_ms);

    if (!ret) {
        bus->applied = NULL; /* the other device changes the controller settings */
        bus->stats.acquires++;
    }
    return ret;
}

void display_bus_release(display_bus_t *bus)
{
    if (bus && bus->users) {
        display_os_sem_give(&bus->lock);
    }
}

/**
 * @brief Take the bus lock without touching the controller: no transfer on this bus is in flight
 *        while it is held. Used to change state read by completion callbacks.
 *
 * @retval 0 success
 * @retval -EINVAL the bus is not initialized
 * @retval -EPERM called from an ISR
 * @retval -ETIMEDOUT bus busy
 */
int display_bus_hold(display_bus_t *bus)
{
    return bus_take(bus, CONFIG_DISPLAY_BUS_LOCK_TIMEOUT_MS);
}

/**
 * @brief Release a lock taken with display_bus_hold().
 */
void display_bus_unhold(display_bus_t *bus)
{
    display_os_sem_give(&bus->lock);
}

int display_bus_get_stats(display_bus_t *bus, display_bus_stats_t *stats)
{
    uintptr_t flags;

    if (!bus || !stats) {
        return -EINVAL;
    }
    flags = display_os_irq_save();
    *stats = bus->stats;
    display_os_irq_restore(flags);
    return 0;
}
