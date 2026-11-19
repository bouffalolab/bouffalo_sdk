/**
 * @file display_scanout.c
 * @brief Scanout core: frame validation, pending/current ownership, events.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "scanout/display_scanout_driver.h"
#include "display_os.h"

/**
 * @brief Deliver an event to the listener, if any. The listener is read atomically, called outside the
 *        critical section.
 */
static void scanout_emit(display_scanout_t *scanout, display_event_type_t type)
{
    display_event_t event = { .type = type, .result = 0, .buf = NULL };
    display_scanout_cb_t cb;
    void *ctx;
    uintptr_t flags;

    flags = display_os_irq_save();
    cb = scanout->cb;
    ctx = scanout->cb_ctx;
    display_os_irq_restore(flags);
    if (cb) {
        cb(scanout, &event, ctx);
    }
}

/**
 * @brief Check a frame against the backend requirements and the video mode.
 *
 * @retval 0 the frame is acceptable
 * @retval -EINVAL missing data / UV plane, stride other than the line length, or misaligned address / size
 * @retval -ENOTSUP format not supported
 */
static int scanout_fb_check(const display_scanout_t *scanout, const display_fb_t *fb, const display_video_mode_t *mode)
{
    const display_scanout_caps_t *caps = &scanout->caps;
    const uint32_t bpp = (uint32_t)display_format_bytes(fb->format);
    const uint32_t row = mode->width * bpp; /* bytes of one visible line */
    const bool nv12 = fb->format == DISPLAY_FMT_NV12;

    if (!fb->data || !bpp || fb->format != scanout->cfg.format) {
        return -EINVAL;
    }
    if (!(caps->format_mask & DISPLAY_FMT_BIT(fb->format))) {
        return -ENOTSUP;
    }
    /* Frames are tightly packed: the stride is the line length, and 0 stands for it */
    if ((fb->stride && fb->stride != row) || (nv12 && (!fb->uv || (fb->uv_stride && fb->uv_stride != row))) ||
        (!nv12 && (fb->uv || fb->uv_stride))) {
        return -EINVAL;
    }
    if (caps->addr_align > 1 &&
        ((uintptr_t)fb->data % caps->addr_align || (nv12 && (uintptr_t)fb->uv % caps->addr_align))) {
        return -EINVAL;
    }
    if (caps->size_align > 1 && (((uint64_t)row * mode->height) % caps->size_align ||
                                 (nv12 && ((uint64_t)row * mode->height / 2) % caps->size_align))) {
        return -EINVAL;
    }
    return 0;
}

/**
 * @brief Write back the data cache of a frame, unless the caller does it (dcache_dis).
 */
static void scanout_clean_frame(const display_scanout_t *scanout, const display_fb_t *fb,
                                const display_video_mode_t *mode)
{
    size_t plane0; /* the whole frame, or the Y plane of NV12 */

    if (scanout->cfg.dcache_dis) {
        return;
    }
    plane0 = (size_t)mode->width * mode->height * display_format_bytes(fb->format);
    display_os_dcache_clean(fb->data, plane0);
    if (fb->format == DISPLAY_FMT_NV12) {
        display_os_dcache_clean(fb->uv, plane0 / 2); /* interleaved UV, half the samples of Y */
    }
}

int display_scanout_init(display_scanout_t *scanout)
{
    int ret;

    if (!scanout || !scanout->ops || !scanout->ops->config || !scanout->ops->flip || !scanout->ops->deinit) {
        return -EINVAL;
    }
    if (scanout->initialized) {
        return scanout->state == DISPLAY_SCANOUT_ERROR ? -EPIPE : 0;
    }
    ret = scanout->ops->init ? scanout->ops->init(scanout) : 0;
    if (ret) {
        return ret;
    }
    scanout->state = DISPLAY_SCANOUT_STOPPED;
    scanout->current = (display_fb_t){ 0 };
    scanout->pending = (display_fb_t){ 0 };
    scanout->pending_op = DISPLAY_FRAME_OP_NONE;
    scanout->configured = false;
    scanout->initialized = true;
    return 0;
}

int display_scanout_deinit(display_scanout_t *scanout)
{
    uintptr_t flags;
    int ret;

    if (!scanout) {
        return -EINVAL;
    }
    if (!scanout->initialized) {
        return 0;
    }
    ret = scanout->ops->deinit(scanout);
    flags = display_os_irq_save();
    if (ret) {
        scanout->state = DISPLAY_SCANOUT_ERROR;
        display_os_irq_restore(flags);
        return ret;
    }
    scanout->state = DISPLAY_SCANOUT_STOPPED;
    scanout->current = (display_fb_t){ 0 };
    scanout->pending = (display_fb_t){ 0 };
    scanout->pending_op = DISPLAY_FRAME_OP_NONE;
    scanout->mode = (display_video_mode_t){ 0 };
    scanout->configured = false;
    scanout->initialized = false;
    display_os_irq_restore(flags);
    return 0;
}

int display_scanout_get_caps(display_scanout_t *scanout, display_scanout_caps_t *caps)
{
    if (!scanout || !scanout->initialized || !caps) {
        return -EINVAL;
    }
    *caps = scanout->caps;
    return 0;
}

int display_scanout_get_status(display_scanout_t *scanout, display_scanout_status_t *status)
{
    uintptr_t flags;

    if (!scanout || !scanout->initialized || !status) {
        return -EINVAL;
    }
    flags = display_os_irq_save();
    status->state = scanout->state;
    status->current = scanout->current.data;
    status->pending = scanout->pending.data;
    status->pending_op = scanout->pending_op;
    display_os_irq_restore(flags);
    return 0;
}

int display_scanout_set_callback(display_scanout_t *scanout, display_scanout_cb_t cb, void *ctx)
{
    uintptr_t flags;
    int ret = 0;

    if (!scanout || !scanout->ops) {
        return -EINVAL;
    }
    flags = display_os_irq_save();
    if (cb && scanout->cb) {
        ret = -EBUSY;
    } else {
        scanout->cb = cb;
        scanout->cb_ctx = cb ? ctx : NULL;
    }
    display_os_irq_restore(flags);
    return ret;
}

/**
 * @brief The mode names a real video link and a visible area that is not empty.
 */
static bool scanout_mode_valid(const display_video_mode_t *mode)
{
    return mode->link != DISPLAY_VLINK_NONE && (unsigned)mode->link <= DISPLAY_VLINK_DSI && mode->width && mode->height;
}

int display_scanout_config(display_scanout_t *scanout, const display_video_mode_t *mode,
                           const display_scanout_cfg_t *cfg)
{
    display_scanout_status_t status;
    int ret;

    if (!scanout || !scanout->initialized || !mode || !cfg || !scanout_mode_valid(mode) ||
        !display_format_bytes(cfg->format)) {
        return -EINVAL;
    }
    ret = display_scanout_get_status(scanout, &status);
    if (ret) {
        return ret;
    }
    if (status.state != DISPLAY_SCANOUT_STOPPED || status.pending_op != DISPLAY_FRAME_OP_NONE) {
        return -EBUSY;
    }
    /* The owner serializes configuration with frame submissions. */
    if (!(scanout->caps.vlink_mask & DISPLAY_VLINK_BIT(mode->link)) ||
        !(scanout->caps.format_mask & DISPLAY_FMT_BIT(cfg->format))) {
        return -ENOTSUP;
    }
    if (cfg->format == DISPLAY_FMT_NV12 && ((mode->width | mode->height) & 1u)) {
        return -EINVAL;
    }
    if ((cfg->format == DISPLAY_FMT_YUYV || cfg->format == DISPLAY_FMT_UYVY) && (mode->width & 1u)) {
        return -EINVAL;
    }
    if ((uint64_t)mode->width * mode->height * display_format_bytes(cfg->format) > SIZE_MAX) {
        return -EINVAL;
    }
    ret = scanout->ops->config(scanout, mode, cfg);
    if (!ret) {
        scanout->mode = *mode;
        scanout->cfg = *cfg;
        scanout->configured = true;
    }
    return ret;
}

int display_scanout_flip(display_scanout_t *scanout, const display_fb_t *fb)
{
    display_scanout_status_t status;
    display_frame_op_t op;
    uintptr_t flags;
    int ret;

    if (!scanout || !scanout->initialized) {
        return -EINVAL;
    }
    /* FIRST may change STOPPED/current and clear pending in one IRQ. Never mix those instants. */
    ret = display_scanout_get_status(scanout, &status);
    if (ret) {
        return ret;
    }
    if (status.state == DISPLAY_SCANOUT_ERROR) {
        return -EPIPE;
    }
    if (status.pending_op != DISPLAY_FRAME_OP_NONE) {
        return -EBUSY;
    }
    if (!fb && status.state == DISPLAY_SCANOUT_STOPPED) {
        return 0; /* no completion for an already disabled layer */
    }
    if (!scanout->configured) {
        return -EPIPE;
    }
    if (!fb) {
        op = DISPLAY_FRAME_OP_LAST;
    } else {
        op = status.state == DISPLAY_SCANOUT_STOPPED ? DISPLAY_FRAME_OP_FIRST : DISPLAY_FRAME_OP_FLIP;
    }
    /* Cache maintenance does not extend the driver's IRQ critical section. */
    if (fb) {
        ret = scanout_fb_check(scanout, fb, &scanout->mode);
        if (ret) {
            return ret;
        }
        if (op == DISPLAY_FRAME_OP_FLIP && !(scanout->caps.flags & DISPLAY_SCANOUT_CAP_FLIP)) {
            return -ENOTSUP;
        }
        if (fb->data == status.current) {
            return -EINVAL;
        }
        scanout_clean_frame(scanout, fb, &scanout->mode);
    }
    /* Publish before submission: the backend reports only its own committed ops. */
    flags = display_os_irq_save();
    if (scanout->pending_op != DISPLAY_FRAME_OP_NONE || scanout->state != status.state ||
        scanout->current.data != status.current) {
        display_os_irq_restore(flags);
        return -EBUSY; /* an ISR submitted/completed another frame while this one was prepared */
    }
    scanout->pending = fb ? *fb : (display_fb_t){ 0 };
    scanout->pending_op = op;
    display_os_irq_restore(flags);

    ret = scanout->ops->flip(scanout, fb); /* expensive preparation stays outside the critical section */
    if (ret) {
        flags = display_os_irq_save();
        scanout->pending = (display_fb_t){ 0 };
        scanout->pending_op = DISPLAY_FRAME_OP_NONE;
        display_os_irq_restore(flags);
    }
    return ret; /* success must not overwrite an ISR that already completed */
}

void display_scanout_on_event(display_scanout_t *scanout, display_event_type_t type)
{
    uintptr_t flags;

    if (!scanout) {
        return;
    }
    flags = display_os_irq_save();
    if (!scanout->initialized || scanout->state == DISPLAY_SCANOUT_ERROR ||
        (type != DISPLAY_EVT_VSYNC &&
         !((type == DISPLAY_EVT_FB_FIRST && scanout->pending_op == DISPLAY_FRAME_OP_FIRST) ||
           (type == DISPLAY_EVT_FB_RETIRED && scanout->pending_op == DISPLAY_FRAME_OP_FLIP) ||
           (type == DISPLAY_EVT_FB_LAST && scanout->pending_op == DISPLAY_FRAME_OP_LAST)))) {
        display_os_irq_restore(flags);
        return;
    }
    if (type != DISPLAY_EVT_VSYNC) {
        scanout->current = scanout->pending;
        scanout->state = type == DISPLAY_EVT_FB_LAST ? DISPLAY_SCANOUT_STOPPED : DISPLAY_SCANOUT_RUNNING;
        scanout->pending = (display_fb_t){ 0 };
        scanout->pending_op = DISPLAY_FRAME_OP_NONE;
    }
    display_os_irq_restore(flags);
    scanout_emit(scanout, type);
}
