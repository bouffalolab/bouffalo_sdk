/**
 * @file display_bl618dg_dpu_hw.c
 * @brief BL618DG display processing unit, hardware level: output and the layers Base, OSD0 and OSD1.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_hw.h"
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_clock.h"
#include "bus/dsi/display_dsi.h"
#include "display_os.h"
#include "bflb_dpi.h"
#include "bflb_irq.h"
#include "bflb_osd.h"
#include "bflb_core.h"
#include "bl618dg_clock.h"

#define DPU_SYNC_MAX         255u /**< Sync pulse and front porch are 8-bit fields */
#define DPU_TOTAL_MAX        0xFFFFu
#define DPU_STOP_WAIT_MS     200u    /**< Upper bound of the wait for frame IRQs when stopping a layer */
#define DPU_PATTERN_MAX      0xFFFFu /**< With step 0 the custom pattern is a constant */
#define DPU_BASE_BLACK       0x0000u
#define DPU_OSD_ALPHA_OPAQUE 255u

static uint8_t dpu_dummy[BFLB_CACHE_LINE_SIZE] __attribute__((aligned(BFLB_CACHE_LINE_SIZE)));

static bool dpu_is_layer(int layer)
{
    return layer >= 0 && layer < DISPLAY_BL618DG_DPU_LAYER_COUNT;
}

static bool dpu_is_osd(int layer)
{
    return layer == DISPLAY_BL618DG_DPU_LAYER_OSD0 || layer == DISPLAY_BL618DG_DPU_LAYER_OSD1;
}

static struct bflb_device_s *dpu_osd_dev(const display_bl618dg_dpu_hw_t *hw, int layer)
{
    return layer == DISPLAY_BL618DG_DPU_LAYER_OSD1 ? hw->osd1 : hw->osd0;
}

/** @brief Frame interrupts seen by the OSD layer (Base counts with OSD0). */
static uint32_t dpu_seof(const display_bl618dg_dpu_hw_t *hw, int layer)
{
    return layer == DISPLAY_BL618DG_DPU_LAYER_OSD1 ? hw->seof1 : hw->seof0;
}

static void dpu_osd_submit(display_bl618dg_dpu_hw_t *hw, int index, const display_fb_t *fb,
                           const display_bl618dg_dpu_window_t *win, bool first);
static void dpu_osd_irq_off(display_bl618dg_dpu_hw_t *hw, int layer);

/** @brief Apply a deferred window/address together, never complete on this IRQ. */
static bool dpu_apply_window(display_bl618dg_dpu_hw_t *hw, int index)
{
    display_bl618dg_dpu_layer_t *layer = &hw->layer[index];

    if (!layer->win_pending || dpu_seof(hw, index) == layer->arm_cnt) {
        return false;
    }
    dpu_osd_submit(hw, index, &layer->pending, &layer->new_win,
                   layer->pending_op == DISPLAY_FRAME_OP_FIRST);
    bflb_osd_int_clear(dpu_osd_dev(hw, index));
    layer->cfg.window = layer->new_win;
    layer->win_pending = false;
    layer->arm_cnt = dpu_seof(hw, index);
    return true;
}

/** @brief Complete after the first corresponding IRQ following MMIO submission. */
static void dpu_layer_tick(display_bl618dg_dpu_hw_t *hw, int index)
{
    display_bl618dg_dpu_layer_t *layer = &hw->layer[index];
    display_event_type_t event = DISPLAY_EVT_VSYNC;
    uintptr_t flags = bflb_irq_save();
    display_bl618dg_dpu_cb_t cb;
    void *ctx;

    if (!dpu_apply_window(hw, index) && layer->pending_op != DISPLAY_FRAME_OP_NONE &&
        dpu_seof(hw, index) != layer->arm_cnt) {
        display_frame_op_t op = layer->pending_op;

        layer->current = layer->pending;
        layer->pending = (display_fb_t){ 0 };
        layer->pending_op = DISPLAY_FRAME_OP_NONE;
        layer->running = op != DISPLAY_FRAME_OP_LAST;
        event = op == DISPLAY_FRAME_OP_FIRST ? DISPLAY_EVT_FB_FIRST :
                op == DISPLAY_FRAME_OP_FLIP  ? DISPLAY_EVT_FB_RETIRED :
                                               DISPLAY_EVT_FB_LAST;
        if (index == DISPLAY_BL618DG_DPU_LAYER_OSD1 && op == DISPLAY_FRAME_OP_LAST) {
            dpu_osd_irq_off(hw, index);
        }
    }
    cb = layer->cb;
    ctx = layer->cb_ctx;
    bflb_irq_restore(flags);
    if (cb) {
        cb(event, ctx);
    }
}

/** @brief OSD0 frame interrupt: count the frame, report the Base and the OSD0 frames. */
static void dpu_osd0_isr(int irq, void *arg)
{
    display_bl618dg_dpu_hw_t *hw = arg;

    (void)irq;
    bflb_osd_int_clear(hw->osd0);
    hw->seof0++;
    dpu_layer_tick(hw, DISPLAY_BL618DG_DPU_LAYER_BASE);
    dpu_layer_tick(hw, DISPLAY_BL618DG_DPU_LAYER_OSD0);
}

/** @brief OSD1 frame interrupt, as OSD0 does for its frame. */
static void dpu_osd1_isr(int irq, void *arg)
{
    display_bl618dg_dpu_hw_t *hw = arg;

    (void)irq;
    bflb_osd_int_clear(hw->osd1);
    hw->seof1++;
    dpu_layer_tick(hw, DISPLAY_BL618DG_DPU_LAYER_OSD1);
}

int display_bl618dg_dpu_hw_open(display_bl618dg_dpu_hw_t *hw)
{
    if (!hw) {
        return -EINVAL;
    }
    hw->dpi = bflb_device_get_by_name("dpi");
    hw->osd0 = bflb_device_get_by_name("osd0");
    hw->osd1 = bflb_device_get_by_name("osd1");
    if (!hw->dpi || !hw->osd0 || !hw->osd1) {
        return -ENODEV;
    }
    return 0;
}

/** @brief Check the timing against the 8-bit and 16-bit fields of the DPI. */
static int dpu_check_mode(const display_bl618dg_dpu_hw_t *hw, const display_video_mode_t *mode)
{
    uint32_t htotal = (uint32_t)mode->width + mode->hsw + mode->hbp + mode->hfp;
    uint32_t vtotal = (uint32_t)mode->height + mode->vsw + mode->vbp + mode->vfp;

    if (mode->link != (hw->cfg.dsi ? DISPLAY_VLINK_DSI : DISPLAY_VLINK_DPI)) {
        return -ENOTSUP;
    }
    if (mode->hsw > DPU_SYNC_MAX || mode->hfp > DPU_SYNC_MAX || mode->vsw > DPU_SYNC_MAX || mode->vfp > DPU_SYNC_MAX ||
        htotal > DPU_TOTAL_MAX || vtotal > DPU_TOTAL_MAX || !mode->clock_hz || !mode->width || !mode->height) {
        return -EINVAL;
    }
    return 0;
}

/** @brief Bytes of a frame of the layer. */
static size_t dpu_frame_bytes(const display_bl618dg_dpu_layer_cfg_t *cfg)
{
    return (size_t)cfg->window.width * cfg->window.height * display_format_bytes(cfg->format);
}

uint32_t display_bl618dg_dpu_hw_layer_formats(int layer)
{
    switch (layer) {
        case DISPLAY_BL618DG_DPU_LAYER_BASE: /* the DPI reads RGB and the YUV formats of a camera */
            return DISPLAY_FMT_BIT(DISPLAY_FMT_RGB565) | DISPLAY_FMT_BIT(DISPLAY_FMT_RGB888) |
                   DISPLAY_FMT_BIT(DISPLAY_FMT_XRGB8888) | DISPLAY_FMT_BIT(DISPLAY_FMT_YUYV) |
                   DISPLAY_FMT_BIT(DISPLAY_FMT_UYVY) | DISPLAY_FMT_BIT(DISPLAY_FMT_NV12);
        case DISPLAY_BL618DG_DPU_LAYER_OSD0: /* the OSD blends RGB and AYUV frames; no YUYV / NV12 layout */
        case DISPLAY_BL618DG_DPU_LAYER_OSD1:
            return DISPLAY_FMT_BIT(DISPLAY_FMT_RGB565) | DISPLAY_FMT_BIT(DISPLAY_FMT_XRGB8888) |
                   DISPLAY_FMT_BIT(DISPLAY_FMT_ARGB8888) | DISPLAY_FMT_BIT(DISPLAY_FMT_AYUV8888);
        default:
            return 0;
    }
}

/** @brief Whether the layer can show frames of the format. */
static bool dpu_layer_accepts(int layer, display_format_t format)
{
    return (unsigned)format < DISPLAY_FMT_COUNT &&
           (display_bl618dg_dpu_hw_layer_formats(layer) & DISPLAY_FMT_BIT(format));
}

/** @brief The DPI data format of a Base frame buffer (the format is accepted by the Base layer already). */
static int dpu_base_data_format(display_format_t format)
{
    switch (format) {
        case DISPLAY_FMT_RGB565:
            return DPI_DATA_FORMAT_RGB565;
        case DISPLAY_FMT_RGB888:
            return DPI_DATA_FORMAT_RGB888;
        case DISPLAY_FMT_XRGB8888:
            return DPI_DATA_FORMAT_NRGB8888;
        case DISPLAY_FMT_YUYV:
            return DPI_DATA_FORMAT_YUYV;
        case DISPLAY_FMT_UYVY:
            return DPI_DATA_FORMAT_UYVY;
        case DISPLAY_FMT_NV12:
            return DPI_DATA_FORMAT_Y_UV_PLANAR;
        default:
            return -1;
    }
}

static int dpu_fb_check(const display_bl618dg_dpu_layer_cfg_t *cfg, const display_fb_t *fb)
{
    const uint32_t row = (uint32_t)(cfg->window.width * display_format_bytes(cfg->format));
    const bool nv12 = cfg->format == DISPLAY_FMT_NV12;

    if (!fb->data || fb->format != cfg->format || (uintptr_t)fb->data % BFLB_CACHE_LINE_SIZE ||
        (fb->stride && fb->stride != row)) {
        return -EINVAL;
    }
    if (nv12) {
        if (!fb->uv || (uintptr_t)fb->uv % BFLB_CACHE_LINE_SIZE || (fb->uv_stride && fb->uv_stride != row)) {
            return -EINVAL;
        }
    } else if (fb->uv || fb->uv_stride) {
        return -EINVAL;
    }
    return 0;
}

static void dpu_clean(const display_bl618dg_dpu_layer_cfg_t *cfg, const display_fb_t *fb)
{
    if (!cfg->dcache_dis) {
        size_t bytes = dpu_frame_bytes(cfg);

        display_os_dcache_clean(fb->data, bytes);
        if (fb->format == DISPLAY_FMT_NV12) {
            display_os_dcache_clean(fb->uv, bytes / 2);
        }
    }
}

/** @brief Always initialize pattern black, then program the global BT.601 range. */
static void dpu_base_init(display_bl618dg_dpu_hw_t *hw, const display_video_mode_t *mode)
{
    const struct bflb_dpi_config_s cfg = {
        .width = mode->width,
        .height = mode->height,
        .hsw = (uint8_t)mode->hsw,
        .hbp = mode->hbp,
        .hfp = (uint8_t)mode->hfp,
        .vsw = (uint8_t)mode->vsw,
        .vbp = mode->vbp,
        .vfp = (uint8_t)mode->vfp,
        .interface = hw->cfg.dsi ? DPI_INTERFACE_24_PIN : (uint8_t)hw->cfg.pins, /* DSI takes the full 24 bit */
        .input_sel = DPI_INPUT_SEL_TEST_PATTERN_WITH_OSD,
        .test_pattern = DPI_TEST_PATTERN_NULL,
        .data_format = DPI_DATA_FORMAT_TEST_PATTERN,
    };

    bflb_dpi_init(hw->dpi, &cfg);
    if (mode->link == DISPLAY_VLINK_DPI) {
        const uint8_t edge = mode->pclk_invert ? DPI_PCLK_SAMPLE_EDGE_RISING : DPI_PCLK_SAMPLE_EDGE_FALLING;

        /* Program both values so an inverted previous run cannot leak into a default run.
         * HSYNC/VSYNC/DE inversion is not supported by this backend and is ignored. */
        bflb_dpi_set_pclk_sample_edge(hw->dpi, edge);
    }
    /* The converters keep the range of the last run: set it every time */
    bflb_dpi_set_yuv_range(hw->dpi, hw->cfg.yuv_range == DISPLAY_BL618DG_DPU_YUV_LIMITED ? DPI_YUV_RANGE_LIMITED :
                                                                                           DPI_YUV_RANGE_FULL);
    bflb_dpi_feature_control(hw->dpi, DPI_CMD_SET_BURST, DPI_BURST_INCR8);
    bflb_dpi_set_test_pattern_custom(hw->dpi, DPU_PATTERN_MAX, DPU_BASE_BLACK, 0);
}

/**
 * @brief An unused OSD needs one transparent pixel enabled then disabled before
 * its SEOF can run. MMIO only; the caller cleans the static zero dummy first.
 */
static void dpu_osd_idle(display_bl618dg_dpu_hw_t *hw, int index)
{
    struct bflb_device_s *dev = dpu_osd_dev(hw, index);
    struct bflb_osd_blend_config_s cfg = {
        .blend_format = OSD_BLEND_FORMAT_RGB565,
        .order_a = 3,
        .order_rv = 2,
        .order_gy = 1,
        .order_bu = 0,
        .coor = { .start_x = 0, .start_y = 0, .end_x = 1, .end_y = 1 },
        .layer_buffer_addr = (uint32_t)(uintptr_t)dpu_dummy,
    };

    bflb_osd_blend_disable(dev);
    bflb_osd_blend_init(dev, &cfg);
    bflb_osd_blend_set_global_a(dev, true, 0);
    bflb_osd_blend_enable(dev);
    bflb_osd_blend_disable(dev);
}

static void dpu_osd_irq_on(display_bl618dg_dpu_hw_t *hw, int layer)
{
    struct bflb_device_s *dev = dpu_osd_dev(hw, layer);

    bflb_osd_int_clear(dev);
    bflb_osd_int_mask(dev, false);
    bflb_irq_attach(dev->irq_num, layer == DISPLAY_BL618DG_DPU_LAYER_OSD1 ? dpu_osd1_isr : dpu_osd0_isr, hw);
    bflb_irq_enable(dev->irq_num);
}

static void dpu_osd_irq_off(display_bl618dg_dpu_hw_t *hw, int layer)
{
    struct bflb_device_s *dev = dpu_osd_dev(hw, layer);

    bflb_osd_int_mask(dev, true);
    bflb_irq_disable(dev->irq_num);
    bflb_irq_detach(dev->irq_num);
}

/** @brief Resolve a full-screen shorthand and check hardware format geometry. */
static int dpu_window_resolve(const display_bl618dg_dpu_hw_t *hw, int index,
                              const display_bl618dg_dpu_window_t *in, display_bl618dg_dpu_window_t *out)
{
    *out = *in;
    if (!out->width && !out->height) {
        out->x = 0;
        out->y = 0;
        out->width = hw->width;
        out->height = hw->height;
    }
    if (!out->width || !out->height || (uint32_t)out->x + out->width > hw->width ||
        (uint32_t)out->y + out->height > hw->height) {
        return -EINVAL;
    }
    if (index == DISPLAY_BL618DG_DPU_LAYER_BASE &&
        (out->x || out->y || out->width != hw->width || out->height != hw->height)) {
        return -EINVAL;
    }
    return 0;
}

static int dpu_cfg_check(const display_bl618dg_dpu_hw_t *hw, int index,
                         const display_bl618dg_dpu_layer_cfg_t *in, display_bl618dg_dpu_layer_cfg_t *out)
{
    int ret;

    if (!dpu_layer_accepts(index, in->format)) {
        return -ENOTSUP;
    }
    *out = *in;
    ret = dpu_window_resolve(hw, index, &in->window, &out->window);
    if (ret) {
        return ret;
    }
    if ((in->format == DISPLAY_FMT_YUYV || in->format == DISPLAY_FMT_UYVY || in->format == DISPLAY_FMT_NV12) &&
        (out->window.width & 1u)) {
        return -EINVAL;
    }
    if ((in->format == DISPLAY_FMT_NV12 && (out->window.height & 1u)) ||
        (uint64_t)out->window.width * out->window.height * display_format_bytes(in->format) > SIZE_MAX) {
        return -EINVAL;
    }
    return 0;
}

/** @brief Whether the frames of the format have an alpha of their own. */
static bool dpu_format_has_alpha(display_format_t format)
{
    return format == DISPLAY_FMT_ARGB8888 || format == DISPLAY_FMT_AYUV8888;
}

static void dpu_osd_apply_alpha(struct bflb_device_s *dev, const display_bl618dg_dpu_layer_t *layer)
{
    bflb_osd_blend_set_global_a(dev, layer->alpha_mode == DISPLAY_BL618DG_DPU_ALPHA_CONSTANT, layer->alpha_value);
}

/** @brief MMIO only: task critical section, or frame ISR for deferred windows. */
static void dpu_osd_submit(display_bl618dg_dpu_hw_t *hw, int index, const display_fb_t *fb,
                           const display_bl618dg_dpu_window_t *win, bool first)
{
    display_bl618dg_dpu_layer_t *layer = &hw->layer[index];
    struct bflb_device_s *dev = dpu_osd_dev(hw, index);
    /* ARGB8888 is 0xAARRGGBB, AYUV8888 is 0xAAYYUUVV: the bytes of R / V, G / Y, B / U change places */
    const bool ayuv = layer->cfg.format == DISPLAY_FMT_AYUV8888;
    struct bflb_osd_coordinate_s coor = { .start_x = win->x,
                                          .start_y = win->y,
                                          .end_x = (uint16_t)(win->x + win->width),
                                          .end_y = (uint16_t)(win->y + win->height) };

    if (!first) {
        if (layer->win_pending) {
            bflb_osd_blend_set_coordinate(dev, &coor);
        }
        bflb_osd_blend_set_layer_buffer(dev, (uint32_t)(uintptr_t)fb->data);
        return;
    }
    struct bflb_osd_blend_config_s cfg = {
        .blend_format = layer->cfg.format == DISPLAY_FMT_RGB565 ? OSD_BLEND_FORMAT_RGB565 :
                        ayuv                                    ? OSD_BLEND_FORMAT_AYUV8888 :
                                                                  OSD_BLEND_FORMAT_ARGB8888,
        .order_a = 3,
        .order_rv = ayuv ? 0 : 2,
        .order_gy = ayuv ? 2 : 1,
        .order_bu = ayuv ? 1 : 0,
        .coor = coor,
        .layer_buffer_addr = (uint32_t)(uintptr_t)fb->data,
    };

    bflb_osd_blend_disable(dev);
    bflb_osd_blend_init(dev, &cfg);
    dpu_osd_apply_alpha(dev, layer);
    bflb_osd_blend_enable(dev);
}

/**
 * @brief Bring up the output: the DSI host starts HS video (the panel is initialized already, and the host has set the
 *        display clock), or the display clock and the RGB pins are set up. Pixels flow from the DPI enable on.
 */
static int dpu_output_start(display_bl618dg_dpu_hw_t *hw, const display_video_mode_t *mode)
{
    display_bl618dg_dpu_clk_t clk;

    if (hw->cfg.dsi) {
        return display_dsi_hs_start(hw->cfg.dsi);
    }
    if (display_bl618dg_dpu_clock_select(Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK), mode->clock_hz, &clk)) {
        return -ENOTSUP;
    }
    GLB_Set_Display_CLK(ENABLE, (uint8_t)clk.src, clk.div);
    if (hw->cfg.pins_init) {
        hw->cfg.pins_init();
    }
    return 0;
}

static bool dpu_phy_equal(const display_dsi_phy_t *a, const display_dsi_phy_t *b)
{
    if (!a || !b) {
        return a == b;
    }
    return a->clk_zero == b->clk_zero && a->clk_trail == b->clk_trail && a->clk_exit == b->clk_exit &&
           a->data_zero == b->data_zero && a->data_trail == b->data_trail && a->data_exit == b->data_exit &&
           a->data_prepare == b->data_prepare && a->ta_go == b->ta_go && a->ta_get == b->ta_get &&
           a->req_ready == b->req_ready && a->lpx == b->lpx && a->wakeup == b->wakeup;
}

static bool dpu_mode_equal(const display_video_mode_t *a, const display_video_mode_t *b)
{
    return a->link == b->link && a->width == b->width && a->height == b->height && a->hsw == b->hsw &&
           a->hbp == b->hbp && a->hfp == b->hfp && a->vsw == b->vsw && a->vbp == b->vbp && a->vfp == b->vfp &&
           a->clock_hz == b->clock_hz && (a->link != DISPLAY_VLINK_DPI || a->pclk_invert == b->pclk_invert) &&
           a->dsi.lanes == b->dsi.lanes && a->dsi.lane_mbps == b->dsi.lane_mbps &&
           a->dsi.format == b->dsi.format && dpu_phy_equal(a->dsi.phy, b->dsi.phy);
}

/** @brief Nothing runs: reset the state of the layers and the frame counters for a new run. */
static void dpu_state_reset(display_bl618dg_dpu_hw_t *hw)
{
    for (int i = 0; i < DISPLAY_BL618DG_DPU_LAYER_COUNT; i++) {
        display_bl618dg_dpu_layer_t *layer = &hw->layer[i];

        layer->running = false;
        layer->current = (display_fb_t){ 0 };
        layer->pending = (display_fb_t){ 0 };
        layer->pending_op = DISPLAY_FRAME_OP_NONE;
        layer->win_pending = false;
        layer->arm_cnt = 0;
    }
    hw->seof0 = 0;
    hw->seof1 = 0;
    hw->irq_down = false;
}

int display_bl618dg_dpu_hw_start(display_bl618dg_dpu_hw_t *hw, const display_video_mode_t *mode)
{
    uintptr_t flags;
    int ret;

    if (!hw || !mode || !hw->dpi || !hw->osd0 || !hw->osd1) {
        return -EINVAL;
    }
    if (hw->stopping) {
        return -EBUSY;
    }
    if (hw->running) {
        return dpu_mode_equal(&hw->mode, mode) ? 0 : -EBUSY;
    }
    ret = dpu_check_mode(hw, mode);
    if (ret) {
        return ret;
    }
    ret = dpu_output_start(hw, mode);
    if (ret) {
        return ret;
    }

    hw->width = mode->width;
    hw->height = mode->height;
    hw->mode = *mode;
    if (mode->dsi.phy) {
        hw->phy = *mode->dsi.phy;
        hw->mode.dsi.phy = &hw->phy;
    }
    dpu_state_reset(hw);
    for (int i = 0; i < DISPLAY_BL618DG_DPU_LAYER_COUNT; i++) {
        hw->layer[i].configured = false;
    }
    dpu_base_init(hw, mode);
    bflb_osd_blend_disable(hw->osd1);
    display_os_dcache_clean(dpu_dummy, sizeof(dpu_dummy));

    flags = bflb_irq_save();
    dpu_osd_idle(hw, DISPLAY_BL618DG_DPU_LAYER_OSD0);
    hw->running = true;
    dpu_osd_irq_on(hw, DISPLAY_BL618DG_DPU_LAYER_OSD0);
    if (!hw->cfg.dsi) {
        bflb_dpi_enable(hw->dpi);
    }
    bflb_irq_restore(flags);
    return 0;
}

static int dpu_access(display_bl618dg_dpu_hw_t *hw, int index, bool trusted)
{
    if (!hw || !dpu_is_layer(index)) {
        return -EINVAL;
    }
    if (hw->layer[index].claimed && !trusted) {
        return -EBUSY;
    }
    return 0;
}

/** @brief Caller holds the short task/ISR state lock, including trusted claim/release. */
static void dpu_callback_set(display_bl618dg_dpu_layer_t *layer, display_bl618dg_dpu_cb_t cb, void *ctx)
{
    layer->cb_ctx = cb ? ctx : NULL;
    layer->cb = cb;
}

int display_bl618dg_dpu_hw_layer_set_callback(display_bl618dg_dpu_hw_t *hw, int layer,
                                              display_bl618dg_dpu_cb_t cb, void *ctx)
{
    uintptr_t flags;
    int ret = dpu_access(hw, layer, false);

    if (ret) {
        return ret;
    }
    flags = bflb_irq_save();
    dpu_callback_set(&hw->layer[layer], cb, ctx);
    bflb_irq_restore(flags);
    return 0;
}

static void dpu_pattern(display_bl618dg_dpu_hw_t *hw)
{
    const display_bl618dg_dpu_layer_cfg_t *cfg = &hw->layer[DISPLAY_BL618DG_DPU_LAYER_BASE].cfg;

    bflb_dpi_set_test_pattern_custom(hw->dpi, DPU_PATTERN_MAX,
                                     (uint16_t)(((uint16_t)cfg->color_g << 8) | cfg->color_r), 0);
}

static int dpu_layer_config(display_bl618dg_dpu_hw_t *hw, int layer,
                            const display_bl618dg_dpu_layer_cfg_t *cfg, bool trusted)
{
    display_bl618dg_dpu_layer_cfg_t checked;
    display_bl618dg_dpu_layer_t *l;
    uintptr_t flags;
    int ret = dpu_access(hw, layer, trusted);

    if (ret) {
        return ret;
    }
    if (!cfg) {
        return -EINVAL;
    }
    if (!hw->running || hw->stopping) {
        return -EPIPE;
    }
    l = &hw->layer[layer];
    ret = dpu_cfg_check(hw, layer, cfg, &checked);
    if (ret) {
        return ret;
    }
    flags = bflb_irq_save();
    /* FIRST may finish between two task-side loads: test this pair atomically. */
    if (l->running || l->pending_op != DISPLAY_FRAME_OP_NONE) {
        bflb_irq_restore(flags);
        return -EBUSY;
    }
    l->cfg = checked;
    l->configured = true;
    l->alpha_mode = !cfg->alpha && dpu_format_has_alpha(cfg->format) ? DISPLAY_BL618DG_DPU_ALPHA_PIXEL :
                                                                       DISPLAY_BL618DG_DPU_ALPHA_CONSTANT;
    l->alpha_value = cfg->alpha                                       ? cfg->alpha :
                     l->alpha_mode == DISPLAY_BL618DG_DPU_ALPHA_PIXEL ? 0 :
                                                                        DPU_OSD_ALPHA_OPAQUE;
    if (layer == DISPLAY_BL618DG_DPU_LAYER_BASE) {
        dpu_pattern(hw);
    }
    bflb_irq_restore(flags);
    return 0;
}

int display_bl618dg_dpu_hw_layer_config(display_bl618dg_dpu_hw_t *hw, int layer,
                                        const display_bl618dg_dpu_layer_cfg_t *cfg)
{
    return dpu_layer_config(hw, layer, cfg, false);
}

static int dpu_layer_flip(display_bl618dg_dpu_hw_t *hw, int layer, const display_fb_t *fb,
                          const display_bl618dg_dpu_window_t *win, bool trusted, bool stopping)
{
    display_bl618dg_dpu_layer_t *l;
    display_bl618dg_dpu_layer_cfg_t cfg;
    const void *current;
    bool running;
    display_frame_op_t op;
    uintptr_t flags;
    int ret = dpu_access(hw, layer, trusted);

    if (ret) {
        return ret;
    }
    if (win && (!dpu_is_osd(layer) || !fb)) {
        return -EINVAL;
    }
    l = &hw->layer[layer];
    if (hw->stopping && !stopping) {
        return -EPIPE;
    }
    flags = bflb_irq_save();
    if (l->pending_op != DISPLAY_FRAME_OP_NONE) {
        bflb_irq_restore(flags);
        return -EBUSY;
    }
    running = l->running;
    current = l->current.data;
    cfg = l->cfg;
    bflb_irq_restore(flags);
    if (!fb && !running) {
        return 0;
    }
    if (!hw->running || hw->irq_down || !l->configured) {
        return -EPIPE;
    }
    if (win) {
        cfg.window = *win;
        ret = dpu_cfg_check(hw, layer, &cfg, &cfg);
        if (ret) {
            return ret;
        }
    }
    if (fb) {
        ret = dpu_fb_check(&cfg, fb);
        if (ret || fb->data == current) {
            return ret ? ret : -EINVAL;
        }
        dpu_clean(&cfg, fb); /* never mask IRQs for cache maintenance */
    }
    if (win && !running && layer == DISPLAY_BL618DG_DPU_LAYER_OSD1) {
        display_os_dcache_clean(dpu_dummy, sizeof(dpu_dummy));
    }

    flags = bflb_irq_save();
    if (l->pending_op != DISPLAY_FRAME_OP_NONE || l->running != running || l->current.data != current) {
        bflb_irq_restore(flags);
        return -EBUSY; /* a callback submitted/completed another frame during preparation */
    }
    op = !fb ? DISPLAY_FRAME_OP_LAST : running ? DISPLAY_FRAME_OP_FLIP :
                                                 DISPLAY_FRAME_OP_FIRST;
    l->pending = fb ? *fb : (display_fb_t){ 0 };
    l->pending_op = op;
    l->win_pending = win != NULL;
    if (win) {
        l->new_win = cfg.window;
    }
    if (layer == DISPLAY_BL618DG_DPU_LAYER_BASE) {
        if (!fb) {
            dpu_pattern(hw);
            bflb_dpi_set_input(hw->dpi, DPI_INPUT_SEL_TEST_PATTERN_WITH_OSD, DPI_DATA_FORMAT_TEST_PATTERN, 0, 0);
        } else if (op == DISPLAY_FRAME_OP_FIRST) {
            if (fb->format == DISPLAY_FMT_NV12) {
                bflb_dpi_feature_control(hw->dpi, DPI_CMD_SET_YUV420_UV_VALID_LINE, DPI_YUV420_UV_VALID_LINE_EVEN);
            }
            bflb_dpi_set_input(hw->dpi, DPI_INPUT_SEL_FRAMEBUFFER_WITH_OSD,
                               (uint8_t)dpu_base_data_format(fb->format), (uint32_t)(uintptr_t)fb->data,
                               (uint32_t)(uintptr_t)fb->uv);
        } else if (fb->format == DISPLAY_FMT_NV12) {
            bflb_dpi_framebuffer_planar_switch(hw->dpi, (uint32_t)(uintptr_t)fb->data, (uint32_t)(uintptr_t)fb->uv);
        } else {
            bflb_dpi_framebuffer_switch(hw->dpi, (uint32_t)(uintptr_t)fb->data);
        }
    } else if (!fb) {
        bflb_osd_blend_disable(dpu_osd_dev(hw, layer));
    } else if (!win) {
        dpu_osd_submit(hw, layer, fb, &cfg.window, op == DISPLAY_FRAME_OP_FIRST);
    } else if (op == DISPLAY_FRAME_OP_FIRST && layer == DISPLAY_BL618DG_DPU_LAYER_OSD1) {
        /* No user frame or new window is visible yet; bootstrap SEOF for the
         * deferred ISR submission even if OSD1 has never run before. */
        dpu_osd_idle(hw, layer);
    }
    if (layer == DISPLAY_BL618DG_DPU_LAYER_OSD1 && op == DISPLAY_FRAME_OP_FIRST) {
        dpu_osd_irq_on(hw, layer);
    } else {
        /* Drop a boundary that preceded MMIO; it cannot confirm this operation.
         * Base and OSD0 share this clear: any other pending layer may wait one
         * additional boundary, but is never released prematurely. */
        bflb_osd_int_clear(dpu_osd_dev(hw, layer));
    }
    l->arm_cnt = dpu_seof(hw, layer);
    bflb_irq_restore(flags);
    return 0; /* ISR may already have completed: do not touch state here */
}

int display_bl618dg_dpu_hw_layer_flip(display_bl618dg_dpu_hw_t *hw, int layer, const display_fb_t *fb,
                                      const display_bl618dg_dpu_window_t *win)
{
    return dpu_layer_flip(hw, layer, fb, win, false, false);
}

int display_bl618dg_dpu_osd0_claim(display_bl618dg_dpu_hw_t *hw, display_bl618dg_dpu_cb_t cb, void *ctx)
{
    display_bl618dg_dpu_layer_t *l;
    uintptr_t flags;
    int ret = dpu_access(hw, DISPLAY_BL618DG_DPU_LAYER_OSD0, false);

    if (ret) {
        return ret;
    }
    l = &hw->layer[DISPLAY_BL618DG_DPU_LAYER_OSD0];
    flags = bflb_irq_save();
    if (hw->stopping || l->claimed || l->running || l->pending_op != DISPLAY_FRAME_OP_NONE) {
        ret = -EBUSY;
    } else {
        l->claimed = true;
        dpu_callback_set(l, cb, ctx);
    }
    bflb_irq_restore(flags);
    return ret;
}

int display_bl618dg_dpu_osd0_release(display_bl618dg_dpu_hw_t *hw)
{
    uintptr_t flags;
    int ret = dpu_access(hw, DISPLAY_BL618DG_DPU_LAYER_OSD0, true);

    if (ret) {
        return ret;
    }
    flags = bflb_irq_save();
    if (hw->running || hw->stopping) {
        ret = -EBUSY;
    } else {
        dpu_callback_set(&hw->layer[DISPLAY_BL618DG_DPU_LAYER_OSD0], NULL, NULL);
        hw->layer[DISPLAY_BL618DG_DPU_LAYER_OSD0].claimed = false;
    }
    bflb_irq_restore(flags);
    return ret;
}

int display_bl618dg_dpu_osd0_config(display_bl618dg_dpu_hw_t *hw, const display_bl618dg_dpu_layer_cfg_t *cfg)
{
    if (!hw || !hw->layer[DISPLAY_BL618DG_DPU_LAYER_OSD0].claimed) {
        return -EPERM;
    }
    return dpu_layer_config(hw, DISPLAY_BL618DG_DPU_LAYER_OSD0, cfg, true);
}

int display_bl618dg_dpu_osd0_flip(display_bl618dg_dpu_hw_t *hw, const display_fb_t *fb)
{
    if (!hw || !hw->layer[DISPLAY_BL618DG_DPU_LAYER_OSD0].claimed) {
        return -EPERM;
    }
    return dpu_layer_flip(hw, DISPLAY_BL618DG_DPU_LAYER_OSD0, fb, NULL, true, false);
}

int display_bl618dg_dpu_hw_layer_get_info(display_bl618dg_dpu_hw_t *hw, int layer,
                                          display_bl618dg_dpu_layer_info_t *info)
{
    display_bl618dg_dpu_layer_t *l;
    uintptr_t flags;

    if (!hw || !info || !dpu_is_layer(layer)) {
        return -EINVAL;
    }
    l = &hw->layer[layer];
    flags = bflb_irq_save();
    info->running = l->running;
    info->format = l->cfg.format;
    info->window = layer == DISPLAY_BL618DG_DPU_LAYER_BASE ?
                       (display_bl618dg_dpu_window_t){ 0, 0, hw->width, hw->height } :
                       l->cfg.window;
    info->current = l->current.data;
    info->pending = l->pending.data;
    info->pending_op = l->pending_op;
    info->uv_current = l->current.uv;
    info->uv_pending = l->pending.uv;
    bflb_irq_restore(flags);
    return 0;
}

/** @brief Poll shared state briefly; all delays run with IRQs enabled. */
static int dpu_wait_pending(display_bl618dg_dpu_hw_t *hw, int index)
{
    for (uint32_t waited = 0;; waited++) {
        uintptr_t flags = bflb_irq_save();
        bool done = hw->layer[index].pending_op == DISPLAY_FRAME_OP_NONE;

        bflb_irq_restore(flags);
        if (done) {
            return 0;
        }
        if (waited >= DPU_STOP_WAIT_MS) {
            return -ETIMEDOUT;
        }
        display_os_delay_ms(1);
    }
}

int display_bl618dg_dpu_hw_stop(display_bl618dg_dpu_hw_t *hw)
{
    uintptr_t flags;
    int ret = 0;

    if (display_os_in_isr()) {
        return -EPERM;
    }
    if (!hw) {
        return -EINVAL;
    }
    if (!hw->running) {
        return 0;
    }
    flags = bflb_irq_save();
    hw->stopping = true;
    bflb_irq_restore(flags);
    if (!hw->irq_down) {
        for (int i = DISPLAY_BL618DG_DPU_LAYER_OSD1; i >= DISPLAY_BL618DG_DPU_LAYER_BASE; i--) {
            ret = dpu_wait_pending(hw, i);
            if (ret) {
                return ret;
            }
            ret = dpu_layer_flip(hw, i, NULL, NULL, true, true);
            if (ret) {
                return ret;
            }
            ret = dpu_wait_pending(hw, i);
            if (ret) {
                return ret;
            }
        }
        flags = bflb_irq_save();
        dpu_osd_irq_off(hw, DISPLAY_BL618DG_DPU_LAYER_OSD0);
        hw->irq_down = true;
        bflb_irq_restore(flags);
    }

    if (hw->cfg.dsi) {
        ret = display_dsi_hs_stop(hw->cfg.dsi);
    } else {
        bflb_dpi_disable(hw->dpi);
        if (hw->cfg.pins_deinit) {
            hw->cfg.pins_deinit();
        }
    }
    if (!ret) {
        flags = bflb_irq_save();
        dpu_state_reset(hw);
        hw->running = false;
        hw->stopping = false;
        bflb_irq_restore(flags);
    }
    return ret;
}

int display_bl618dg_dpu_hw_layer_set_alpha(display_bl618dg_dpu_hw_t *hw, int layer, display_bl618dg_dpu_alpha_t mode,
                                           uint8_t alpha)
{
    display_bl618dg_dpu_layer_t *l;
    uintptr_t flags;
    int ret = dpu_access(hw, layer, false);

    if (ret) {
        return ret;
    }
    if (!dpu_is_osd(layer) ||
        (mode != DISPLAY_BL618DG_DPU_ALPHA_PIXEL && mode != DISPLAY_BL618DG_DPU_ALPHA_CONSTANT)) {
        return -EINVAL;
    }
    l = &hw->layer[layer];
    if (!hw->running || hw->stopping) {
        return -EPIPE;
    }
    if (mode == DISPLAY_BL618DG_DPU_ALPHA_PIXEL && !dpu_format_has_alpha(l->cfg.format)) {
        return -ENOTSUP;
    }
    flags = bflb_irq_save();
    if (l->pending_op != DISPLAY_FRAME_OP_NONE || !l->running) {
        ret = l->pending_op != DISPLAY_FRAME_OP_NONE ? -EBUSY : -EPIPE;
        bflb_irq_restore(flags);
        return ret;
    }
    l->alpha_mode = mode;
    l->alpha_value = mode == DISPLAY_BL618DG_DPU_ALPHA_PIXEL ? 0 : alpha;
    dpu_osd_apply_alpha(dpu_osd_dev(hw, layer), l);
    bflb_irq_restore(flags);
    return 0;
}
