/**
 * @file hid_template.c
 * @brief CherryUSB host HID interrupt-IN report demonstration.
 */

#include "usbh_core.h"
#include "usbh_hid.h"

#define USBH_HID_BUFFER_SIZE 128U

/** @brief Lifecycle states for the single-buffer HID report demo. */
enum usbh_hid_state {
    USBH_HID_STOPPED = 0,
    USBH_HID_STARTING,
    USBH_HID_RUNNING,
    USBH_HID_STOPPING,
};

/** @brief Shared state for the HID worker and transfer callback. */
struct usbh_hid_context {
    struct usbh_hid *hid_class;
    volatile enum usbh_hid_state state;
    usb_osal_mutex_t state_mutex;
    usb_osal_sem_t worker_sem;
    usb_osal_sem_t stopped_sem;
    volatile bool stop_waiting;
    volatile bool in_busy;
    volatile int in_nbytes;
};

static struct usbh_hid_context g_hid_ctx = {
    .state = USBH_HID_STOPPED,
};

/** @brief DMA-accessible buffer for incoming HID reports. */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_buffer[USBH_HID_BUFFER_SIZE];

/** @brief Lazily create synchronization objects that remain callback-valid. */
static void usbh_hid_context_init(void)
{
    if (g_hid_ctx.state_mutex == NULL) {
        g_hid_ctx.state_mutex = usb_osal_mutex_create();
        g_hid_ctx.worker_sem = usb_osal_sem_create(0);
        g_hid_ctx.stopped_sem = usb_osal_sem_create(0);
    }
}

/**
 * @brief Record the result of a HID interrupt-IN transfer.
 * @param[in,out] arg HID context supplied with the URB.
 * @param[in] nbytes Number of bytes received, or a negative error code.
 * @note This callback records completion state and wakes the HID worker.
 */
void usbh_hid_in_callback(void *arg, int nbytes)
{
    struct usbh_hid_context *ctx = (struct usbh_hid_context *)arg;

    ctx->in_nbytes = nbytes;
    ctx->in_busy = false;
    usb_osal_sem_give(ctx->worker_sem);
}

/**
 * @brief Continuously receive and print HID interrupt reports.
 * @note The HID class instance is supplied through the USB OSAL thread
 *       argument macro. The loop runs until stop is requested or a transfer
 *       error occurs.
 */
static void usbh_hid_thread(CONFIG_USB_OSAL_THREAD_SET_ARGV)
{
    struct usbh_hid_context *ctx = (struct usbh_hid_context *)CONFIG_USB_OSAL_THREAD_GET_ARGV;
    struct usbh_hid *hid_class = ctx->hid_class;
    bool stop_waiting;
    int ret;
    uint16_t max_packet_size;
    uint32_t input_cnt = 0;

    if (ctx->state != USBH_HID_STARTING) {
        goto exit;
    }

    if (hid_class->hport == NULL || hid_class->intin == NULL) {
        USB_LOG_ERR("USBH HID interrupt-IN endpoint is unavailable\r\n");
        goto exit;
    }

    max_packet_size = USB_GET_MAXPACKETSIZE(hid_class->intin->wMaxPacketSize);
    if (max_packet_size == 0 || max_packet_size > sizeof(hid_buffer)) {
        USB_LOG_ERR("USBH HID packet size %u exceeds buffer size %u\r\n",
                    max_packet_size, (unsigned int)sizeof(hid_buffer));
        goto exit;
    }

    if (hid_class->intin->bInterval == 0 ||
        (hid_class->hport->speed == USB_SPEED_HIGH && hid_class->intin->bInterval > 16)) {
        USB_LOG_ERR("USBH HID invalid interval:%u\r\n", hid_class->intin->bInterval);
        goto exit;
    }
    USB_LOG_RAW("USBH HID test start, bInterval:%u\r\n", hid_class->intin->bInterval);

    usb_osal_mutex_take(ctx->state_mutex);
    if (ctx->state == USBH_HID_STARTING) {
        ctx->state = USBH_HID_RUNNING;
    }
    usb_osal_mutex_give(ctx->state_mutex);

    while (ctx->state == USBH_HID_RUNNING) {
        if (ctx->in_busy == false) {
            if (ctx->in_nbytes < 0) {
                USB_LOG_ERR("hid in error,ret:%d\r\n", ctx->in_nbytes);
                break;
            } else if (ctx->in_nbytes > 0) {
                USB_LOG_RAW("HID input[%d] %dByte: ", input_cnt, ctx->in_nbytes);
                for (int i = 0; i < ctx->in_nbytes; i++) {
                    USB_LOG_RAW("0x%02x ", hid_buffer[i]);
                }
                USB_LOG_RAW("\r\n");
            }
            input_cnt++;
            ctx->in_nbytes = 0;

            ctx->in_busy = true;
            usbh_int_urb_fill(&hid_class->intin_urb, hid_class->hport, hid_class->intin,
                              hid_buffer, max_packet_size, 0, usbh_hid_in_callback, ctx);
            ret = usbh_submit_urb(&hid_class->intin_urb);
            if (ret < 0) {
                ctx->in_busy = false;
                USB_LOG_ERR("hid submit urb error,ret:%d\r\n", ret);
                break;
            }
        }

        usb_osal_sem_take(ctx->worker_sem, 10);
    }

exit:
    g_hid_ctx.state = USBH_HID_STOPPING;

    /* No new URBs are submitted after the worker leaves the loop. Cancel the
     * remaining URB; the class disconnect may already have killed it. */
    usbh_kill_urb(&hid_class->intin_urb);

    usb_osal_mutex_take(ctx->state_mutex);
    ctx->hid_class = NULL;
    ctx->in_busy = false;
    stop_waiting = ctx->stop_waiting;
    if (stop_waiting == false) {
        ctx->state = USBH_HID_STOPPED;
    }
    usb_osal_mutex_give(ctx->state_mutex);

    USB_LOG_RAW("USBH HID test end\r\n");
    if (stop_waiting) {
        usb_osal_sem_give(ctx->stopped_sem);
    }
    usb_osal_thread_delete(NULL);
}

/**
 * @brief Start the HID report thread.
 * @param[in] hid_class HID host class instance to monitor.
 */
void usbh_hid_run(struct usbh_hid *hid_class)
{
    usbh_hid_context_init();

    usb_osal_mutex_take(g_hid_ctx.state_mutex);
    if (g_hid_ctx.state != USBH_HID_STOPPED) {
        usb_osal_mutex_give(g_hid_ctx.state_mutex);
        USB_LOG_ERR("USBH HID test already running\r\n");
        return;
    }

    usb_osal_sem_reset(g_hid_ctx.worker_sem);
    usb_osal_sem_reset(g_hid_ctx.stopped_sem);
    g_hid_ctx.hid_class = hid_class;
    g_hid_ctx.stop_waiting = false;
    g_hid_ctx.in_busy = false;
    g_hid_ctx.in_nbytes = 0;
    g_hid_ctx.state = USBH_HID_STARTING;
    usb_osal_mutex_give(g_hid_ctx.state_mutex);

    usb_osal_thread_create("usbh_hid", 2048, CONFIG_USBHOST_PSC_PRIO - 1, usbh_hid_thread, &g_hid_ctx);
}

/**
 * @brief Handle a request to stop the HID report thread.
 * @param[in] hid_class HID host class instance being disconnected.
 * @note This function blocks until the report thread no longer references the class.
 */
void usbh_hid_stop(struct usbh_hid *hid_class)
{
    if (g_hid_ctx.state_mutex == NULL) {
        return;
    }

    usb_osal_mutex_take(g_hid_ctx.state_mutex);
    if (g_hid_ctx.state == USBH_HID_STOPPED ||
        g_hid_ctx.hid_class != hid_class) {
        usb_osal_mutex_give(g_hid_ctx.state_mutex);
        return;
    }
    g_hid_ctx.stop_waiting = true;
    g_hid_ctx.state = USBH_HID_STOPPING;
    usb_osal_mutex_give(g_hid_ctx.state_mutex);

    usb_osal_sem_give(g_hid_ctx.worker_sem);
    usb_osal_sem_take(g_hid_ctx.stopped_sem, USB_OSAL_WAITING_FOREVER);

    usb_osal_mutex_take(g_hid_ctx.state_mutex);
    g_hid_ctx.stop_waiting = false;
    g_hid_ctx.state = USBH_HID_STOPPED;
    usb_osal_mutex_give(g_hid_ctx.state_mutex);
}