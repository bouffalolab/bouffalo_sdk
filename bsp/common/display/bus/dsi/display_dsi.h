/**
 * @file display_dsi.h
 * @brief MIPI DSI host bus backend (BL618DG).
 *
 * - Command side: the host itself is a bus; @ref display_dsi_io_t is the endpoint of a panel
 *   (LP short/long packets). The panel driver hands the video link of the module to the endpoint
 *   (display_io_set_video()), and the first command brings the link up: PLL, clocks, controller and D-PHY.
 * - Video side: pixels come from the DPU scanout and DSI is only its output PHY, see the `.dsi` field
 *   of the DPU scanout configuration. There is no separate DSI scanout; the scanout starts and stops the
 *   HS mode through display_dsi_hs_start() / display_dsi_hs_stop().
 *
 * Hardware constraints:
 * - LP commands are rejected with -EBUSY while HS video runs (an LP transfer silently stops HS);
 * - no read, no command-mode panels (writing GRAM over DSI); the endpoint carries no pixels;
 * - the controller cannot be reset on its own: after an LP error the host enters a fault state and every
 *   operation returns -EIO.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_DSI_H
#define DISPLAY_DSI_H

#include "bus/display_bus_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_BUS_DSI MIPI DSI host backend
 * @{
 */

struct bflb_device_s;

/**
 * @brief Lane order of the board layout, i.e. which controller lane drives which pin pair.
 */
typedef enum {
    DISPLAY_DSI_LANES_3210 = 0, /**< Lane 3, 2, 1, 0 */
    DISPLAY_DSI_LANES_2130,     /**< Lane 2, 1, 3, 0 */
    DISPLAY_DSI_LANES_1320,     /**< Lane 1, 3, 2, 0 */
    DISPLAY_DSI_LANES_3120      /**< Lane 3, 1, 2, 0 */
} display_dsi_lane_order_t;

/**
 * @brief DSI host configuration (board wiring).
 */
typedef struct {
    const char *dev;                     /**< Device name, NULL: "dsi" */
    uint8_t lanes_wired;                 /**< Data lanes wired on the board: 1, 2 or 4 */
    display_dsi_lane_order_t lane_order; /**< Lane swap according to the board layout */
    uint16_t lane_rate_max_mbps;         /**< Board limit of the lane rate, Mbit/s; 0: only the PLL steps limit it */
} display_dsi_host_cfg_t;

/**
 * @brief DSI host object.
 */
typedef struct display_dsi_host {
    display_bus_t base;                /**< Bus core, must be first */
    display_dsi_host_cfg_t cfg;        /**< Configuration */
    struct bflb_device_s *dsi;         /**< Runtime: controller */
    const display_video_mode_t *video; /**< Runtime: video link the controller is set up for; NULL: not set up */
    bool exclusive;                    /**< Runtime: an endpoint without CS owns the bus */
    bool hs_running;                   /**< Runtime: HS video is on; changed with the bus lock held */
    int fault;                         /**< Runtime: sticky fault (negative errno) */
} display_dsi_host_t;

/**
 * @brief DSI endpoint object: the generic endpoint (`.iface = DISPLAY_IFACE_DSI_LP`, `.cs = DISPLAY_PIN_NONE`)
 *        plus the virtual channel.
 */
typedef struct {
    display_io_t base;       /**< Endpoint core, must be first */
    uint8_t virtual_channel; /**< Virtual channel */
} display_dsi_io_t;

/** @brief Backend operations. */
extern const display_bus_ops_t display_dsi_bus_ops;

/** @brief Static definition of a DSI host. */
#define DISPLAY_DSI_HOST(...)                                                  \
    {                                                                          \
        .base = DISPLAY_BUS_BASE(&display_dsi_bus_ops), .cfg = { __VA_ARGS__ } \
    }

/** @brief Static definition of a DSI endpoint: virtual channel, then the generic settings of DISPLAY_IO(). */
#define DISPLAY_DSI_IO(_host, _vc, ...) { .base = DISPLAY_IO(_host, __VA_ARGS__), .virtual_channel = (_vc) }

/**
 * @brief Start the HS video. The controller must be set up: the panel driver has handed the video link to the
 *        endpoint and sent its first command. Called by the scanout after the panel is initialized.
 *
 * @param [in] host DSI host
 * @retval 0 success, or HS is already running
 * @retval -EINVAL @p host is NULL, or the link is not set up
 * @retval -ETIMEDOUT the bus stayed busy
 * @retval -EIO the host is in the fault state
 */
int display_dsi_hs_start(display_dsi_host_t *host);

/**
 * @brief Stop the HS video, so that LP commands work again. The scanout stops fetching pixels first.
 *
 * @param [in] host DSI host
 * @retval 0 success, or HS is not running
 * @retval -EINVAL @p host is NULL
 * @retval -ETIMEDOUT the bus stayed busy
 * @retval <0 the controller did not leave HS; HS counts as running
 */
int display_dsi_hs_stop(display_dsi_host_t *host);

/** @} */ /* DISPLAY_BUS_DSI */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_DSI_H */
