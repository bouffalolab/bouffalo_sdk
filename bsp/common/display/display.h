/**
 * @file display.h
 * @brief Display (L4): one physical screen. Applications and GUI ports normally use only this header.
 *
 * A display is a panel, plus a scanout for video panels:
 * @code
 * display_t lcd = DISPLAY_GRAM(&panel);           // GRAM panel, pixels with display_draw[_async]()
 * display_t tv  = DISPLAY_FB(&panel, &scanout);   // video panel, pixels with display_flip()
 * @endcode
 *
 * Lifecycle: display_init() brings the screen to a usable state, display_deinit() switches it off:
 * - display_init():   check the wiring, initialize buses and scanout, power on, reset, send the
 *                     initialization sequence, display on; configure video output without any frame reads
 * - pixels:           GRAM panel: display_draw[_async](); video panel: display_flip()
 * - display_set_backlight(): the backlight is off after init; switch it on when the first picture is shown
 * - display_deinit(): backlight off, synchronously stop/release video (DSI HS first), display off, power off
 *
 * Concurrency: a display's lifecycle calls (init / deinit) and rotation changes must be serialized by
 * the caller. Draws of displays that share a bus are arbitrated by the bus; panels that share a power /
 * reset group must be initialized and deinitialized by one task at a time.
 * Once a panel or scanout belongs to a display, drive its lifecycle only through the display.
 * For special needs, get the lower layers with display_get_panel() / display_get_scanout().
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include "panel/display_panel.h"
#include "scanout/display_scanout.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_CORE Display
 * @{
 */

/** @brief Display handle type, see @ref display. */
typedef struct display display_t;

/**
 * @brief Display event callback. Called from an ISR (or from the calling task when the bus has no
 *        DMA). Video callbacks may submit the next frame; never wait for a frame or start blocking IO here.
 *
 * @param [in] display display
 * @param [in] event   event
 * @param [in] ctx     user context
 */
typedef void (*display_event_cb_t)(display_t *display, const display_event_t *event, void *ctx);

/**
 * @brief Display kind, given by the composition.
 */
typedef enum {
    DISPLAY_KIND_GRAM = 0, /**< Panel with GRAM: draw areas */
    DISPLAY_KIND_FB        /**< Video panel + scanout: flip frame buffers */
} display_kind_t;

/**
 * @brief Display state.
 */
typedef enum {
    DISPLAY_STATE_UNINIT = 0, /**< Not initialized */
    DISPLAY_STATE_READY,      /**< Powered and initialized; video layer may have no frame */
    DISPLAY_STATE_ERROR       /**< The scanout could not be stopped; frames still owned by the hardware */
} display_state_t;

/**
 * @brief Display information.
 */
typedef struct {
    display_kind_t kind;            /**< Kind */
    display_state_t state;          /**< State */
    uint16_t width;                 /**< Width in the current rotation */
    uint16_t height;                /**< Height in the current rotation */
    display_format_t format;        /**< GRAM: memory pixel format of the endpoint; FB: format of the frames */
    display_wire_t wire;            /**< GRAM: wire width of the endpoint */
    display_rotation_t rotation;    /**< Rotation */
    uint8_t x_align;                /**< Draw area alignment in x, pixels */
    uint8_t y_align;                /**< Draw area alignment in y, pixels */
    bool dma;                       /**< GRAM: pixels are sent by DMA (draws do not occupy the CPU) */
    uint32_t max_draw_bytes;        /**< GRAM: maximum bytes of one draw */
    uint8_t buf_align;              /**< GRAM: required alignment in bytes of the pixel buffer address; a buffer
                                         that does not meet it is rejected with -EINVAL */
    display_scanout_caps_t scanout; /**< FB: frame buffer requirements */
} display_info_t;

/**
 * @brief Display object: static wiring and runtime state.
 */
struct display {
    display_panel_t *panel;     /**< Panel (static, required) */
    display_scanout_t *scanout; /**< Scanout (static), NULL for GRAM panels */
    display_state_t state;      /**< State */
    display_event_cb_t cb;      /**< Event callback */
    void *cb_ctx;               /**< Callback context */
};

/**
 * @brief Static definition of a GRAM display.
 *
 * @param _panel pointer to a concrete panel object (its `base` member is used)
 */
#define DISPLAY_GRAM(_panel)         { .panel = &(_panel)->base }

/**
 * @brief Static definition of a video display.
 *
 * @param _panel   pointer to a concrete video panel object (its `base` member is used)
 * @param _scanout pointer to a concrete scanout object (its `base` member is used)
 */
#define DISPLAY_FB(_panel, _scanout) { .panel = &(_panel)->base, .scanout = &(_scanout)->base }

/**
 * @brief Bring a display to a usable state: claim the panel and scanout, initialize them (and their
 *        buses), power on, reset, send the initialization sequence and switch the display on. A video
 *        display configures output without a frame. Register a callback after READY, then explicitly
 *        display_flip() the first frame. Backlight stays off. Calling again while READY is a no-op.
 *
 * On error roll back only after output stops. Failed cleanup retains resources/owner in ERROR for deinit retry.
 *
 * @param [in,out] display display
 * @retval 0 success, or already initialized
 * @retval -EBUSY the panel or the scanout belongs to another display, or is not idle
 * @retval -EINVAL invalid argument or composition (video panel without scanout, or the opposite)
 * @retval <0 see display_panel_init(), display_panel_prepare(), display_panel_enable(), display_scanout_init()
 *         and display_scanout_config()
 */
int display_init(display_t *display);

/**
 * @brief Switch a display off and release it: backlight off, stop scanning (video panel), display off,
 *        sleep in, power off.
 *
 * @param [in,out] display display
 * @retval 0 success, or not initialized
 * @retval -EINVAL invalid argument
 * @retval <0 the scanout could not be stopped (the display stays in the ERROR state) or the panel could
 *         not be released; the display is kept so that the call can be repeated
 */
int display_deinit(display_t *display);

/**
 * @brief Set the event callback of an initialized display. Video: task or ISR.
 *
 * For a GRAM display the bus lock is held while the callback changes, so it never changes during a
 * transfer of the display; that GRAM path requires task context.
 *
 * @param [in,out] display initialized display
 * @param [in]     cb      callback, NULL to remove
 * @param [in]     ctx     callback context
 * @retval 0 success
 * @retval -EINVAL display not initialized
 * @retval -EPERM GRAM bus acquisition called from an ISR
 * @retval -ETIMEDOUT bus busy
 */
int display_set_callback(display_t *display, display_event_cb_t cb, void *ctx);

/**
 * @brief Set the backlight level of the board: 0 is off, @ref DISPLAY_BACKLIGHT_MAX is full brightness.
 *        The backlight is off after display_init(). Task context only.
 *
 * @param [in] display initialized display
 * @param [in] level   0 .. DISPLAY_BACKLIGHT_MAX; a GPIO backlight is on for any level above 0
 * @retval 0 success
 * @retval -EINVAL display not initialized, or @p level is out of range
 * @retval -ENOTSUP the board has no backlight
 * @retval <0 error from the board callback
 */
int display_set_backlight(display_t *display, uint16_t level);

/**
 * @brief Get display information.
 *
 * @param [in]  display initialized display
 * @param [out] info    information
 * @retval 0 success
 * @retval -EINVAL not initialized, or @p info is NULL
 */
int display_get_info(display_t *display, display_info_t *info);

/**
 * @brief Draw an area and wait until it is sent. GRAM displays.
 *
 * Uses DMA when the bus has it (the calling task sleeps meanwhile); no DRAW_DONE event is emitted.
 *
 * @param [in] display display
 * @param [in] area    area in the current rotation
 * @param [in] pixels  contiguous pixels in the endpoint format
 * @param [in] len     bytes, w * h * bytes per pixel
 * @retval 0 success
 * @retval -EINVAL display not initialized, or invalid argument
 * @retval -ENOTSUP not a GRAM display
 * @retval <0 see display_panel_draw()
 */
int display_draw(display_t *display, const display_area_t *area, const void *pixels, size_t len);

/**
 * @brief Draw an area asynchronously; @ref DISPLAY_EVT_DRAW_DONE carries @p pixels when it can be reused.
 *
 * @param [in] display display
 * @param [in] area    area in the current rotation
 * @param [in] pixels  contiguous pixels, valid until DRAW_DONE
 * @param [in] len     bytes, w * h * bytes per pixel
 * @retval 0 success, the request is queued
 * @retval -EINVAL display not initialized, or invalid argument
 * @retval -ENOTSUP not a GRAM display
 * @retval <0 see display_panel_draw_async()
 */
int display_draw_async(display_t *display, const display_area_t *area, const void *pixels, size_t len);

/**
 * @brief Task/ISR video submission: non-NULL enables/replaces, NULL disables the layer.
 * FIRST confirms the first frame; RETIRED releases the previous frame; LAST releases the final frame.
 * NULL while disabled is an event-free no-op. One owner manages the layer and at most one operation is pending.
 * Completion callbacks may submit again. Cache clean/backend preparation can increase ISR latency.
 *
 * @param [in,out] display display
 * @param [in]     fb      frame copied, or NULL; accepted memory stays hardware-owned until RETIRED/LAST/deinit
 * @retval 0 success
 * @retval -EINVAL display not initialized, or a non-NULL frame has invalid data/format
 * @retval -ENOTSUP not a video display
 * @retval -EPIPE the display is in the ERROR state
 * @retval <0 see display_scanout_flip()
 */
int display_flip(display_t *display, const display_fb_t *fb);

/**
 * @brief Set the rotation. Wait for every in-flight draw first.
 *
 * @param [in,out] display  initialized display
 * @param [in]     rotation rotation
 * @retval 0 success
 * @retval -EINVAL display not initialized
 * @retval <0 see display_panel_set_rotation()
 */
int display_set_rotation(display_t *display, display_rotation_t rotation);

/**
 * @brief Get the panel of a display, for direct use.
 *
 * @param [in] display display
 * @return the panel, or NULL if @p display is NULL
 */
display_panel_t *display_get_panel(display_t *display);

/**
 * @brief Get the scanout of a display, for direct use.
 *
 * @param [in] display display
 * @return the scanout, or NULL for a GRAM display or if @p display is NULL
 */
display_scanout_t *display_get_scanout(display_t *display);

/** @} */ /* DISPLAY_CORE */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H */
