/**
 * @file display_bl618dg_dpu_hw.h
 * @brief One physical BL618DG DPU, independent Base/OSD0/OSD1 layers.
 *
 * One owner serializes each layer's lifecycle/configuration. Layer operations
 * may run in task or ISR; cache cleaning preserves the caller's IRQ state and
 * only shared state and short MMIO sequences mask IRQs. Frames are tightly
 * packed and cache-line aligned. FIRST/RETIRED/LAST
 * contain no address and replace VSYNC at that layer's boundary, not once per
 * physical frame globally. Base and OSD0 share OSD0 SEOF; OSD1 uses its own SEOF
 * and may stop its IRQ after LAST.
 *
 * Include bus/dsi/display_dsi.h when defining the optional DSI host.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_BL618DG_DPU_HW_H
#define DISPLAY_BL618DG_DPU_HW_H

#include "display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_BL618DG_DPU_HW BL618DG DPU, hardware level
 * @{
 */

struct bflb_device_s;
struct display_dsi_host;

/** @name Layers, bottom to top */
/** @{ */
#define DISPLAY_BL618DG_DPU_LAYER_BASE  0 /**< Base: the whole screen, a constant color or a frame buffer */
#define DISPLAY_BL618DG_DPU_LAYER_OSD0  1 /**< OSD0: a window, the screen layer of a display */
#define DISPLAY_BL618DG_DPU_LAYER_OSD1  2 /**< OSD1: a window over OSD0 */
#define DISPLAY_BL618DG_DPU_LAYER_COUNT 3 /**< Number of layers */
/** @} */

/**
 * @brief Range of the YUV data (BT.601): of the Base frames, and of the conversion that the RGB frames and the OSD
 *        layers go through.
 */
typedef enum {
    DISPLAY_BL618DG_DPU_YUV_FULL = 0, /**< Y, U and V use 0..255 */
    DISPLAY_BL618DG_DPU_YUV_LIMITED   /**< Y uses 16..235, U and V 16..240: what most cameras give */
} display_bl618dg_dpu_yuv_range_t;

/**
 * @brief RGB data pin layout on the board (ignored for DSI output).
 */
typedef enum {
    DISPLAY_BL618DG_DPU_PINS_24 = 0,   /**< D0..D23 */
    DISPLAY_BL618DG_DPU_PINS_18_MODE1, /**< D0..D17 */
    DISPLAY_BL618DG_DPU_PINS_18_MODE2, /**< D0..D5, D8..D13, D16..D21 */
    DISPLAY_BL618DG_DPU_PINS_16_MODE1, /**< D0..D15 */
    DISPLAY_BL618DG_DPU_PINS_16_MODE2, /**< D0..D4, D8..D13, D16..D20 */
    DISPLAY_BL618DG_DPU_PINS_16_MODE3  /**< D1..D5, D8..D13, D17..D21 */
} display_bl618dg_dpu_pins_t;

/** @brief How a layer blends over what is below it. */
typedef enum {
    DISPLAY_BL618DG_DPU_ALPHA_CONSTANT, /**< One alpha value for the whole layer (255: opaque) */
    DISPLAY_BL618DG_DPU_ALPHA_PIXEL     /**< Alpha of every pixel, not premultiplied (ARGB8888 and AYUV8888 frames only) */
} display_bl618dg_dpu_alpha_t;

/**
 * @brief DPU configuration.
 */
typedef struct {
    display_bl618dg_dpu_pins_t pins;           /**< RGB pin layout (RGB output only) */
    void (*pins_init)(void);                   /**< Board function that muxes the RGB pins, matching @ref pins; may be NULL */
    void (*pins_deinit)(void);                 /**< Board function that releases the RGB pins; may be NULL */
    struct display_dsi_host *dsi;             /**< NULL: RGB pins; otherwise output through this DSI host */
    display_bl618dg_dpu_yuv_range_t yuv_range; /**< Range of YUV frames (YUYV, UYVY, NV12, AYUV8888); full by default */
} display_bl618dg_dpu_cfg_t;

/**
 * @brief Window of an OSD layer, in pixels of the screen.
 */
typedef struct {
    uint16_t x;      /**< Left */
    uint16_t y;      /**< Top */
    uint16_t width;  /**< Width, not 0 */
    uint16_t height; /**< Height, not 0 */
} display_bl618dg_dpu_window_t;

/**
 * @brief Configuration without a frame address; only while disabled and no pending op.
 */
typedef struct {
    display_format_t format;
    display_bl618dg_dpu_window_t window; /**< 0 x 0: full screen; Base must be full screen */
    uint8_t alpha;                       /**< 0: the alpha of the pixels for ARGB8888 and AYUV8888 frames, opaque for the other formats;
                     *   1..255: one constant alpha for the whole layer (255: opaque) */
    bool dcache_dis;                     /**< The caller manages the data cache of the frames; by default the DPU writes the cache of
                     *   a frame back before each non-NULL flip (both planes for NV12) */
    uint8_t color_r;                     /**< Disabled Base pattern: R low byte, G high byte, B zero */
    uint8_t color_g;
} display_bl618dg_dpu_layer_cfg_t;

/**
 * @brief What a layer shows, as returned by display_bl618dg_dpu_hw_layer_get_info().
 */
typedef struct {
    bool running;                        /**< The layer shows frames */
    display_format_t format;             /**< Format of its frames */
    display_bl618dg_dpu_window_t window; /**< Latest programmed window (full screen for Base); may still be pending */
    const void *current;                 /**< Frame shown (Y plane for NV12) */
    const void *pending;                 /**< Pending frame (Y plane for NV12); NULL for LAST or no operation */
    display_frame_op_t pending_op;       /**< Distinguishes pending LAST from no operation */
    const void *uv_current;
    const void *uv_pending;
} display_bl618dg_dpu_layer_info_t;

/** @brief ISR listener; may submit the next frame, but never block waiting for a boundary. */
typedef void (*display_bl618dg_dpu_cb_t)(display_event_type_t type, void *ctx);

/**
 * @brief Runtime state of a layer.
 */
typedef struct {
    display_bl618dg_dpu_layer_cfg_t cfg;
    bool configured;
    bool running; /**< Confirmed FIRST until confirmed LAST */
    bool claimed; /**< Adapter owns OSD0, including while disabled */
    display_bl618dg_dpu_cb_t cb;
    void *cb_ctx;
    display_fb_t current;
    display_fb_t pending;
    volatile display_frame_op_t pending_op;
    display_bl618dg_dpu_window_t new_win;
    bool win_pending;
    uint32_t arm_cnt;
    display_bl618dg_dpu_alpha_t alpha_mode;
    uint8_t alpha_value;
} display_bl618dg_dpu_layer_t;

/**
 * @brief DPU hardware object.
 */
typedef struct {
    display_bl618dg_dpu_cfg_t cfg; /**< Configuration (static) */
    struct bflb_device_s *dpi;     /**< Runtime: DPI device */
    struct bflb_device_s *osd0;    /**< Runtime: OSD0 device */
    struct bflb_device_s *osd1;    /**< Runtime: OSD1 device */
    uint16_t width;                /**< Runtime: size of the screen in pixels */
    uint16_t height;               /**< Runtime: size of the screen in pixels */
    display_video_mode_t mode;
    display_dsi_phy_t phy;                                              /**< Snapshot for field-wise timing comparison */
    display_bl618dg_dpu_layer_t layer[DISPLAY_BL618DG_DPU_LAYER_COUNT]; /**< Runtime: the layers */
    volatile uint32_t seof0;                                            /**< Runtime: OSD0 frame interrupts seen */
    volatile uint32_t seof1;                                            /**< Runtime: OSD1 frame interrupts seen */
    bool running;                                                       /**< Output lifecycle, independent of layer state */
    bool stopping;                                                      /**< Failed stop: no new config/flip/alpha, allow stop retry */
    bool irq_down;                                                      /**< Runtime: the layers and the interrupts are off, only the output is left */
} display_bl618dg_dpu_hw_t;

/**
 * @brief Look up the devices.
 *
 * @param [in,out] hw DPU
 * @retval 0 success
 * @retval -EINVAL @p hw is NULL
 * @retval -ENODEV the DPI or an OSD device does not exist
 */
int display_bl618dg_dpu_hw_open(display_bl618dg_dpu_hw_t *hw);

/** @brief Start black pattern (no AXI), OSDs disabled. Running output accepts
 * only a field-wise identical mode. Layer config requires output already running;
 * a fresh start requires config before submitting frames. */
int display_bl618dg_dpu_hw_start(display_bl618dg_dpu_hw_t *hw, const display_video_mode_t *mode);

/** @brief Drain pending ops, submit LAST, await its IRQ, then stop output.
 * Failure preserves unconfirmed frames/resources for retry. Success preserves
 * cfg/callback/claim until adapter release. No fixed frame delay. Task context:
 * this call waits for future IRQs and must not run in a frame callback. */
int display_bl618dg_dpu_hw_stop(display_bl618dg_dpu_hw_t *hw);

int display_bl618dg_dpu_hw_layer_set_callback(display_bl618dg_dpu_hw_t *hw, int layer,
                                              display_bl618dg_dpu_cb_t cb, void *ctx);

/** @brief Requires output running; only disabled/no pending. Public config,
 * flip, alpha and callback calls reject an adapter-claimed OSD0 with -EBUSY.
 * Disabled Base color changes are applied immediately without enabling AXI. */
int display_bl618dg_dpu_hw_layer_config(display_bl618dg_dpu_hw_t *hw, int layer,
                                        const display_bl618dg_dpu_layer_cfg_t *cfg);

/** @brief NULL disables, already disabled NULL is a no-op. Non-NULL enables or
 * replaces. win is only for OSD + fb: ISR writes window/address, next IRQ completes
 * (two subsequent boundaries total). Task or ISR, including a completion callback;
 * frame cache cleaning may be lengthy. Successful completion may precede this call's return. */
int display_bl618dg_dpu_hw_layer_flip(display_bl618dg_dpu_hw_t *hw, int layer, const display_fb_t *fb,
                                      const display_bl618dg_dpu_window_t *win);

/**
 * @brief Set how an OSD layer blends over what is below it, while it runs.
 *
 * @param [in] hw    running DPU
 * @param [in] layer DISPLAY_BL618DG_DPU_LAYER_OSD0 or _OSD1
 * @param [in] mode  constant alpha, or the alpha of the pixels (ARGB8888 and AYUV8888 frames only)
 * @param [in] alpha constant alpha, 0 (transparent) to 255 (opaque); ignored for the alpha of the pixels
 * @retval 0 success
 * @retval -EINVAL invalid layer or mode
 * @retval -ENOTSUP the alpha of the pixels needs ARGB8888 or AYUV8888 frames
 * @retval -EPIPE the layer does not run
 */
int display_bl618dg_dpu_hw_layer_set_alpha(display_bl618dg_dpu_hw_t *hw, int layer, display_bl618dg_dpu_alpha_t mode,
                                           uint8_t alpha);

/**
 * @brief Pixel formats a layer can show (a mask of DISPLAY_FMT_BIT()): Base RGB565, RGB888, XRGB8888, YUYV, UYVY, NV12;
 *        OSD0 and OSD1 RGB565, XRGB8888, ARGB8888, AYUV8888. A format outside the mask is refused with `-ENOTSUP`
 *        when the layer is configured.
 *
 * @param [in] layer DISPLAY_BL618DG_DPU_LAYER_*
 * @return the mask, 0 for an invalid layer
 */
uint32_t display_bl618dg_dpu_hw_layer_formats(int layer);

/**
 * @brief Get what a layer shows (a snapshot taken between two frame interrupts).
 *
 * @param [in]  hw    DPU
 * @param [in]  layer DISPLAY_BL618DG_DPU_LAYER_*
 * @param [out] info  layer state
 * @retval 0 success
 * @retval -EINVAL invalid argument
 */
int display_bl618dg_dpu_hw_layer_get_info(display_bl618dg_dpu_hw_t *hw, int layer,
                                          display_bl618dg_dpu_layer_info_t *info);

/** @name Scanout adapter access
 * The adapter claims OSD0 so ordinary layer mutations cannot bypass its frame
 * bookkeeping. These entry points share the same layer implementation; they
 * are for the owning adapter, not application access to a claimed layer.
 * @{ */
int display_bl618dg_dpu_osd0_claim(display_bl618dg_dpu_hw_t *hw, display_bl618dg_dpu_cb_t cb, void *ctx);
int display_bl618dg_dpu_osd0_release(display_bl618dg_dpu_hw_t *hw);
int display_bl618dg_dpu_osd0_config(display_bl618dg_dpu_hw_t *hw, const display_bl618dg_dpu_layer_cfg_t *cfg);
int display_bl618dg_dpu_osd0_flip(display_bl618dg_dpu_hw_t *hw, const display_fb_t *fb);
/** @} */

/** @} */ /* DISPLAY_BL618DG_DPU_HW */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_BL618DG_DPU_HW_H */
