/**
 * @file cdc_acm_template.c
 * @brief CherryUSB host serial throughput test.
 */

#include "bflb_mtimer.h"

#include "usbh_core.h"
#include "usbh_serial.h"

/** @name Serial throughput test configuration
 * @{ */
#define USBH_SERIAL_IN_BUFF_SIZE  (4 * 1024)
#define USBH_SERIAL_OUT_BUFF_SIZE (2 * 1024)

#define USBH_SERIAL_TEST_TIME     (2 * 1000) // 2s
/** @} */

/** @brief Lifecycle states for the single-instance serial throughput test. */
enum usbh_serial_state {
    USBH_SERIAL_STOPPED = 0, /**< No worker owns a serial instance. */
    USBH_SERIAL_STARTING,    /**< The worker is opening and configuring the serial device. */
    USBH_SERIAL_RUNNING,     /**< The worker may access the serial device. */
    USBH_SERIAL_STOPPING,    /**< Stop requested; the worker is closing the serial device. */
};

/** @brief Shared state for the serial worker and disconnect callback. */
struct usbh_serial_context {
    struct usbh_serial *serial;            /**< Serial instance owned by the worker. */
    volatile enum usbh_serial_state state; /**< Lifecycle state shared by the worker and stop caller. */
    usb_osal_mutex_t state_mutex;          /**< Serializes ownership checks and coordinated state updates. */
    usb_osal_sem_t worker_sem;             /**< Wakes the worker after transfer completion or stop. */
    usb_osal_sem_t stopped_sem;            /**< Signals completion of worker teardown. */
    volatile bool stop_waiting;            /**< A disconnect caller is waiting for teardown. */
    volatile bool tx_busy;                 /**< A serial bulk-OUT URB is outstanding. */
    volatile bool rx_busy;                 /**< A serial bulk-IN URB is outstanding. */
    volatile uint32_t tx_size;             /**< Total successfully transmitted bytes. */
    volatile uint32_t rx_size;             /**< Total successfully received bytes. */
};

static struct usbh_serial_context g_serial_ctx = {
    .state = USBH_SERIAL_STOPPED,
};

/** @brief DMA-accessible asynchronous serial receive buffer. */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t serial_read_buffer[USBH_SERIAL_IN_BUFF_SIZE];
/** @brief DMA-accessible serial transmit buffer. */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t serial_write_buffer[USBH_SERIAL_OUT_BUFF_SIZE];

/** @brief Lazily create synchronization objects that remain valid across reconnects. */
static int usbh_serial_context_init(void)
{
    if (g_serial_ctx.state_mutex == NULL) {
        g_serial_ctx.state_mutex = usb_osal_mutex_create();
    }
    if (g_serial_ctx.worker_sem == NULL) {
        g_serial_ctx.worker_sem = usb_osal_sem_create(0);
    }
    if (g_serial_ctx.stopped_sem == NULL) {
        g_serial_ctx.stopped_sem = usb_osal_sem_create(0);
    }

    if (g_serial_ctx.state_mutex == NULL ||
        g_serial_ctx.worker_sem == NULL ||
        g_serial_ctx.stopped_sem == NULL) {
        USB_LOG_ERR("USBH serial synchronization object allocation failed\r\n");
        return -USB_ERR_NOMEM;
    }

    return 0;
}

/**
 * @brief Complete an asynchronous serial bulk-OUT transfer.
 * @param[in,out] arg Serial context supplied with the URB.
 * @param[in] nbytes Number of bytes transferred, or a negative error code.
 * @note This callback runs in USB transfer-completion context.
 */
static void usbh_serial_out_callback(void *arg, int nbytes)
{
    struct usbh_serial_context *ctx = (struct usbh_serial_context *)arg;

    if (nbytes < 0) {
        if (nbytes != -USB_ERR_SHUTDOWN && ctx->state == USBH_SERIAL_RUNNING) {
            USB_LOG_ERR("serial bulk out error,ret:%d\r\n", nbytes);
        }
    } else {
        ctx->tx_size += nbytes;
    }

    ctx->tx_busy = false;
    usb_osal_sem_give(ctx->worker_sem);
}

/**
 * @brief Complete an asynchronous serial bulk-IN transfer.
 * @param[in,out] arg Serial context supplied with the URB.
 * @param[in] nbytes Number of bytes received, or a negative error code.
 * @note This callback runs in USB transfer-completion context.
 */
static void usbh_serial_in_callback(void *arg, int nbytes)
{
    struct usbh_serial_context *ctx = (struct usbh_serial_context *)arg;

    if (nbytes < 0) {
        if (nbytes != -USB_ERR_SHUTDOWN && ctx->state == USBH_SERIAL_RUNNING) {
            USB_LOG_ERR("serial bulk in error,ret:%d\r\n", nbytes);
        }
    } else {
        ctx->rx_size += nbytes;
    }

    ctx->rx_busy = false;
    usb_osal_sem_give(ctx->worker_sem);
}

/**
 * @brief Run the timed bidirectional serial throughput test.
 * @note The serial context is supplied through the USB OSAL thread argument
 *       macro. The thread deletes itself when the test ends.
 */
static void usbh_serial_thread(CONFIG_USB_OSAL_THREAD_SET_ARGV)
{
    struct usbh_serial_context *ctx = (struct usbh_serial_context *)CONFIG_USB_OSAL_THREAD_GET_ARGV;
    struct usbh_serial *serial_instance = ctx->serial;
    struct usbh_serial *serial = NULL;
    char devname[CONFIG_USBHOST_DEV_NAMELEN];
    bool stop_waiting;
    bool test_started = false;
    int ret;
    uint32_t line_state;
    uint32_t start_ms = 0;
    uint32_t elapsed_ms;

    USB_LOG_RAW("USBH serial speed test start\r\n");

    usb_osal_msleep(10);

    if (ctx->state != USBH_SERIAL_STARTING) {
        goto exit;
    }

    if (serial_instance->cdc_minor >= 0) {
        snprintf(devname, sizeof(devname), "/dev/ttyACM%d", serial_instance->cdc_minor);
    } else {
        snprintf(devname, sizeof(devname), "/dev/ttyUSB%d", serial_instance->minor);
    }

    serial = usbh_serial_open(devname, USBH_SERIAL_O_RDWR | USBH_SERIAL_O_NONBLOCK);
    if (serial == NULL) {
        if (ctx->state == USBH_SERIAL_STARTING) {
            USB_LOG_ERR("open serial device %s failed\r\n", devname);
        }
        goto exit;
    }

    if (serial != serial_instance || ctx->state != USBH_SERIAL_STARTING) {
        if (serial != serial_instance) {
            USB_LOG_ERR("opened an unexpected serial instance: %s\r\n", devname);
        }
        goto exit;
    }

#if 0
    struct usbh_serial_termios termios = { 0 };
    termios.baudrate = 2000000;
    termios.stopbits = USBH_SERIAL_STOPBITS_1;
    termios.parity = USBH_SERIAL_PARITY_NONE;
    termios.databits = USBH_SERIAL_DATABITS_8;
    termios.rtscts = false;
    termios.rx_timeout = 0;
    ret = usbh_serial_control(serial, USBH_SERIAL_CMD_SET_ATTR, &termios);
    if (ret < 0) {
        USB_LOG_ERR("set serial attributes error,ret:%d\r\n", ret);
        goto exit;
    }

#error "The CDC asynchronous mode cannot use the USBH_SERIAL_CMD_SET_ATTR command."
#endif

    /* Keep DTR asserted for the device-side loopback test and deassert RTS. */
    line_state = USBH_SERIAL_TIOCM_DTR;
    ret = usbh_serial_control(serial, USBH_SERIAL_CMD_TIOCMSET, &line_state);
    if (ret < 0) {
        USB_LOG_ERR("set serial DTR/RTS error,ret:%d\r\n", ret);
        goto exit;
    }

    usb_osal_mutex_take(ctx->state_mutex);
    if (ctx->state == USBH_SERIAL_STARTING) {
        ctx->state = USBH_SERIAL_RUNNING;
    }
    usb_osal_mutex_give(ctx->state_mutex);
    if (ctx->state != USBH_SERIAL_RUNNING) {
        goto exit;
    }

    usb_osal_msleep(10);

    USB_LOG_RAW("USBH serial cdc speed test start\r\n");

    memset(serial_write_buffer, 0xA5, sizeof(serial_write_buffer));
    start_ms = bflb_mtimer_get_time_ms();
    test_started = true;

    while (ctx->state == USBH_SERIAL_RUNNING) {
        if (ctx->tx_busy == false) {
            ctx->tx_busy = true;
            ret = usbh_serial_cdc_write_async(serial, serial_write_buffer, sizeof(serial_write_buffer),
                                              usbh_serial_out_callback, ctx);
            if (ret < 0) {
                ctx->tx_busy = false;
                if (ret != -USB_ERR_SHUTDOWN && ctx->state == USBH_SERIAL_RUNNING) {
                    USB_LOG_ERR("serial bulk out submit error,ret:%d\r\n", ret);
                }
                goto exit;
            }
        }

        if (ctx->rx_busy == false) {
            ctx->rx_busy = true;
            ret = usbh_serial_cdc_read_async(serial, serial_read_buffer, sizeof(serial_read_buffer),
                                             usbh_serial_in_callback, ctx);
            if (ret < 0) {
                ctx->rx_busy = false;
                if (ret != -USB_ERR_SHUTDOWN && ctx->state == USBH_SERIAL_RUNNING) {
                    USB_LOG_ERR("serial bulk in submit error,ret:%d\r\n", ret);
                }
                goto exit;
            }
        }

        /* stop after a fixed time */
        if (bflb_mtimer_get_time_ms() - start_ms > USBH_SERIAL_TEST_TIME) {
            break;
        }

        /* TX and RX remain independently outstanding while the worker sleeps. */
        usb_osal_sem_take(ctx->worker_sem, 10);
    }

    if (ctx->state == USBH_SERIAL_RUNNING && serial->hport && serial->hport->connected) {
        line_state = 0;
        ret = usbh_serial_control(serial, USBH_SERIAL_CMD_TIOCMSET, &line_state);
        if (ret < 0) {
            USB_LOG_ERR("clear serial DTR/RTS error,ret:%d\r\n", ret);
        }

        usbh_serial_close(serial);
    }

    if (test_started) {
        elapsed_ms = bflb_mtimer_get_time_ms() - start_ms;
        USB_LOG_RAW("USBH serial test time: %u\r\n", (unsigned int)elapsed_ms);
        USB_LOG_RAW("USBH serial tx size: %u\r\n", (unsigned int)ctx->tx_size);
        USB_LOG_RAW("USBH serial rx size: %u\r\n", (unsigned int)ctx->rx_size);
        if (elapsed_ms > 0) {
            USB_LOG_RAW("USBH serial tx speed: %llu KB/S\r\n", (uint64_t)ctx->tx_size * 1000 / 1024 / elapsed_ms);
            USB_LOG_RAW("USBH serial rx speed: %llu KB/S\r\n", (uint64_t)ctx->rx_size * 1000 / 1024 / elapsed_ms);
            USB_LOG_RAW("USBH serial total speed: %llu KB/S\r\n", (uint64_t)(ctx->tx_size + ctx->rx_size) * 1000 / 1024 / elapsed_ms);
        }
    }

exit:
    g_serial_ctx.state = USBH_SERIAL_STOPPING;

    if (serial) {
        usbh_kill_urb(&serial->bulkout_urb);
        usbh_kill_urb(&serial->bulkin_urb);
    }

    usb_osal_mutex_take(ctx->state_mutex);
    ctx->serial = NULL;
    ctx->tx_busy = false;
    ctx->rx_busy = false;
    stop_waiting = ctx->stop_waiting;
    if (stop_waiting == false) {
        ctx->state = USBH_SERIAL_STOPPED;
    }
    usb_osal_mutex_give(ctx->state_mutex);

    USB_LOG_WRN("USBH serial speed test end\r\n");
    if (stop_waiting) {
        usb_osal_sem_give(ctx->stopped_sem);
    }
    usb_osal_thread_delete(NULL);
}

/**
 * @brief Start a serial throughput-test thread.
 * @param[in] serial Serial host-class instance to exercise.
 */
void usbh_serial_run(struct usbh_serial *serial)
{
    usb_osal_thread_t thread;

    if (usbh_serial_context_init() < 0) {
        return;
    }

    usb_osal_mutex_take(g_serial_ctx.state_mutex);
    if (g_serial_ctx.state != USBH_SERIAL_STOPPED) {
        usb_osal_mutex_give(g_serial_ctx.state_mutex);
        USB_LOG_ERR("USBH serial test already running\r\n");
        return;
    }

    usb_osal_sem_reset(g_serial_ctx.worker_sem);
    usb_osal_sem_reset(g_serial_ctx.stopped_sem);
    g_serial_ctx.serial = serial;
    g_serial_ctx.stop_waiting = false;
    g_serial_ctx.tx_busy = false;
    g_serial_ctx.rx_busy = false;
    g_serial_ctx.tx_size = 0;
    g_serial_ctx.rx_size = 0;
    g_serial_ctx.state = USBH_SERIAL_STARTING;
    usb_osal_mutex_give(g_serial_ctx.state_mutex);

    thread = usb_osal_thread_create("usbh_serial", 2048, CONFIG_USBHOST_PSC_PRIO + 1, usbh_serial_thread, &g_serial_ctx);
    if (thread == NULL) {
        usb_osal_mutex_take(g_serial_ctx.state_mutex);
        g_serial_ctx.serial = NULL;
        g_serial_ctx.state = USBH_SERIAL_STOPPED;
        usb_osal_mutex_give(g_serial_ctx.state_mutex);
        USB_LOG_ERR("create USBH serial test thread failed\r\n");
    }
}

/**
 * @brief Handle a request to stop the serial test.
 * @param[in] serial Serial host-class instance being disconnected.
 * @note This function blocks until the worker no longer references the instance.
 */
void usbh_serial_stop(struct usbh_serial *serial)
{
    if (g_serial_ctx.state_mutex == NULL) {
        return;
    }

    usb_osal_mutex_take(g_serial_ctx.state_mutex);
    if (g_serial_ctx.state == USBH_SERIAL_STOPPED ||
        g_serial_ctx.serial != serial) {
        usb_osal_mutex_give(g_serial_ctx.state_mutex);
        return;
    }
    g_serial_ctx.stop_waiting = true;
    g_serial_ctx.state = USBH_SERIAL_STOPPING;
    usb_osal_mutex_give(g_serial_ctx.state_mutex);

    usb_osal_sem_give(g_serial_ctx.worker_sem);
    usb_osal_sem_take(g_serial_ctx.stopped_sem, USB_OSAL_WAITING_FOREVER);

    g_serial_ctx.stop_waiting = false;
    g_serial_ctx.state = USBH_SERIAL_STOPPED;
}
