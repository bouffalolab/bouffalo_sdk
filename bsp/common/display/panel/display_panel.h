/**
 * @file display_panel.h
 * @brief Panel (L3): semantics of the panel driver IC (init, display on/off, rotation, GRAM window)
 *        plus board power/reset.
 *
 * Panel drivers only send standard DCS (command + parameters) through an IO endpoint; they do not
 * know the bus type or the command encoding. Applications may use a panel without a Display object
 * (factory test, boot logo, ...).
 *
 * Lifecycle: OFF --prepare--> PREPARED --enable--> ENABLED, and back with disable/unprepare.
 * Repeated calls return 0.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_PANEL_H
#define DISPLAY_PANEL_H

#include "bus/display_io.h"
#include "panel/display_board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_PANEL Panel
 * @{
 */

/** @brief Panel object, see @ref display_panel. */
typedef struct display_panel display_panel_t;

/**
 * @brief Panel state.
 */
typedef enum {
    DISPLAY_PANEL_OFF = 0,  /**< Not powered */
    DISPLAY_PANEL_PREPARED, /**< Powered and initialized, display off; GRAM can be written */
    DISPLAY_PANEL_ENABLED   /**< Display on */
} display_panel_state_t;

/**
 * @brief Panel information.
 */
typedef struct {
    uint16_t width;                         /**< Visible width in the current rotation */
    uint16_t height;                        /**< Visible height in the current rotation */
    display_format_t format;                /**< GRAM panels: memory format of the endpoint */
    display_wire_t wire;                    /**< GRAM panels: wire width of the endpoint */
    display_rotation_t rotation;            /**< Current rotation */
    uint8_t rotation_mask;                  /**< Supported rotations, 1u << DISPLAY_ROTATION_* */
    uint8_t x_align;                        /**< Required x/width alignment of draw areas, pixels */
    uint8_t y_align;                        /**< Required y/height alignment of draw areas, pixels */
    bool gram;                              /**< true: supports draw; false: pixels come from a scanout */
    const display_video_mode_t *video_mode; /**< Video panels: timing */
    uint16_t video_start_delay_ms;          /**< Video panels: wait between the last command and the first pixel */
} display_panel_info_t;

/**
 * @brief Operations implemented by a panel driver.
 *
 * Everything except init is optional. State checks, argument validation and board power are done by
 * the panel core, so a driver can assume valid arguments and a matching state. Every operation
 * returns 0 on success or a negative errno value.
 */
typedef struct {
    /** @brief Validate the driver data and fill info. The IO endpoint is already attached (if any). */
    int (*init)(display_panel_t *panel);
    /** @brief Reset and initialization sequence; power is already on. */
    int (*prepare)(display_panel_t *panel);
    /** @brief Display on. */
    int (*enable)(display_panel_t *panel);
    /** @brief Display off. */
    int (*disable)(display_panel_t *panel);
    /** @brief Sleep in; the core removes power afterwards. */
    int (*unprepare)(display_panel_t *panel);
    /** @brief Update width/height/rotation in info; send the rotation command unless the state is OFF. */
    int (*set_rotation)(display_panel_t *panel, display_rotation_t rotation);
    /** @brief GRAM panels only. A NULL `cb` means synchronous. Arguments are already validated. */
    int (*draw)(display_panel_t *panel, const display_area_t *area, const void *pixels, size_t len,
                display_done_cb_t cb, void *ctx);
} display_panel_ops_t;

/**
 * @brief Panel object. Drivers embed it as the first member `.base`.
 *
 * ops, io and board are wired statically; the other fields are runtime state.
 */
struct display_panel {
    const display_panel_ops_t *ops; /**< Driver operations (static) */
    display_io_t *io;               /**< Command endpoint (static); NULL for RGB panels without command port */
    display_board_t board;          /**< Board power/reset (static) */
    display_panel_info_t info;      /**< Information */
    display_panel_state_t state;    /**< State */
    bool initialized;               /**< Initialized */
    bool claimed;                   /**< Managed by a Display; a panel belongs to at most one Display */
};

/**
 * @brief Initialize a panel: put the board pins at their inactive level, attach the IO endpoint
 *        (initializing the bus on demand) and validate the driver data. Sends nothing to the panel.
 *
 * @param [in] panel panel
 * @retval 0 success
 * @retval -EINVAL invalid driver data
 * @retval -ENOTSUP the endpoint format/wire width is not supported by the panel
 * @retval <0 see display_io_init()
 */
int display_panel_init(display_panel_t *panel);

/**
 * @brief Power off (if needed) and detach the IO endpoint.
 *
 * @param [in] panel panel
 * @retval 0 success
 * @retval -EINVAL @p panel is NULL
 * @retval <0 see display_io_deinit(); the panel stays initialized so that deinit can be retried
 */
int display_panel_deinit(display_panel_t *panel);

/**
 * @brief Get panel information.
 *
 * @param [in]  panel initialized panel
 * @param [out] info  information
 * @retval 0 success
 * @retval -EINVAL not initialized
 */
int display_panel_get_info(display_panel_t *panel, display_panel_info_t *info);

/**
 * @brief Get the command endpoint of a panel.
 *
 * @param [in] panel panel
 * @return endpoint, or NULL
 */
display_io_t *display_panel_get_io(display_panel_t *panel);

/**
 * @brief Power on, reset and initialize. The display stays off. OFF -> PREPARED.
 *
 * @param [in] panel initialized panel
 * @retval 0 success
 * @retval -EINVAL panel not initialized
 * @retval <0 error from the board or the bus; power is removed again
 */
int display_panel_prepare(display_panel_t *panel);

/**
 * @brief Display on. PREPARED -> ENABLED.
 *
 * @param [in] panel prepared panel
 * @retval 0 success
 * @retval -EINVAL panel not initialized
 * @retval -EPIPE not prepared
 * @retval <0 error from the panel driver or the bus
 */
int display_panel_enable(display_panel_t *panel);

/**
 * @brief Display off. ENABLED -> PREPARED.
 *
 * @note The panel is treated as disabled even when the driver reports an error.
 *
 * @param [in] panel panel
 * @retval 0 success
 * @retval -EINVAL panel not initialized
 * @retval <0 error from the panel driver or the bus
 */
int display_panel_disable(display_panel_t *panel);

/**
 * @brief Display off, sleep in and power off. -> OFF.
 *
 * @note Power is removed and the state becomes OFF even when the driver reports an error; the first
 *       error is returned.
 *
 * @param [in] panel panel
 * @retval 0 success
 * @retval -EINVAL panel not initialized
 * @retval <0 error from the panel driver or the bus
 */
int display_panel_unprepare(display_panel_t *panel);

/**
 * @brief Set the rotation. Allowed in any state; takes effect at prepare when OFF.
 *
 * Wait for every in-flight draw before changing the rotation.
 *
 * @param [in] panel    initialized panel
 * @param [in] rotation rotation
 * @retval 0 success
 * @retval -EINVAL panel not initialized or invalid rotation
 * @retval -ENOTSUP rotation not supported by the module
 * @retval <0 error from the panel driver or the bus
 */
int display_panel_set_rotation(display_panel_t *panel, display_rotation_t rotation);

/**
 * @brief Draw an area into GRAM, synchronously.
 *
 * @param [in] panel  prepared or enabled GRAM panel
 * @param [in] area   area in the current rotation, aligned to x_align/y_align
 * @param [in] pixels contiguous pixels in the endpoint format
 * @param [in] len    bytes, must equal w * h * bytes per pixel
 * @retval 0 success
 * @retval -EINVAL invalid argument, area or length
 * @retval -ENOTSUP not a GRAM panel
 * @retval -EPIPE panel off
 * @retval <0 see display_io_xfer()
 */
int display_panel_draw(display_panel_t *panel, const display_area_t *area, const void *pixels, size_t len);

/**
 * @brief Draw an area into GRAM, asynchronously. @p cb is called exactly once when 0 is returned.
 *
 * @note @p cb may run in interrupt or DMA completion context; keep it short and do not block.
 *
 * @param [in] panel  prepared or enabled GRAM panel
 * @param [in] area   area in the current rotation, aligned to x_align/y_align
 * @param [in] pixels contiguous pixels in the endpoint format, valid until @p cb
 * @param [in] len    bytes, must equal w * h * bytes per pixel
 * @param [in] cb     completion callback
 * @param [in] ctx    callback context
 * @retval 0 success; @p cb will be called
 * @retval -EINVAL @p cb is NULL, or see display_panel_draw()
 * @retval <0 see display_panel_draw(); @p cb is not called
 */
int display_panel_draw_async(display_panel_t *panel, const display_area_t *area, const void *pixels, size_t len,
                             display_done_cb_t cb, void *ctx);

/**
 * @brief Set the backlight level of the board: 0 is off, @ref DISPLAY_BACKLIGHT_MAX is full brightness.
 *
 * @param [in] panel initialized panel
 * @param [in] level 0 .. DISPLAY_BACKLIGHT_MAX
 * @retval 0 success
 * @retval -EINVAL panel not initialized, or @p level is out of range
 * @retval -ENOTSUP the board has no backlight
 * @retval <0 error from the board callback
 */
int display_panel_set_backlight(display_panel_t *panel, uint16_t level);

/**
 * @brief Generate a reset pulse on the board reset line, then wait. Without a reset line only the wait remains:
 *        the panel needs that time after power-up as well.
 *
 * @param [in] panel   panel
 * @param [in] low_ms  time in reset, ms (ignored without a reset line)
 * @param [in] wait_ms time after reset or power-up, ms
 * @retval 0 success
 * @retval -EINVAL @p panel is NULL
 * @retval <0 error from the board callback
 */
int display_panel_hw_reset(display_panel_t *panel, uint16_t low_ms, uint16_t wait_ms);

/** @} */ /* DISPLAY_PANEL */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_PANEL_H */
