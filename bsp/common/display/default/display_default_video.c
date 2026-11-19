/**
 * @file display_default_video.c
 * @brief Default video display (BL618DG), defined statically from Kconfig (default/Kconfig).
 *
 * The objects are chained DSI host -> endpoint -> panel -> display (or panel without endpoint for a standard RGB
 * panel), with the DPU scanout next to the panel. The default display owns no frame memory: the application gives
 * frames with display_flip() after display_init() and callback registration.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "default/display_default.h"
#include "default/display_default_board.h"
#include "default/display_default_gpio_ext.h"
#include "bflb_core.h"
#if defined(CONFIG_DISPLAY_DEFAULT_DSI)
#include "bus/dsi/display_dsi.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_RGB_CMD)
#include "bus/soft_dbi_c/display_soft_dbi_c.h"
#endif
#include "panel/video_standard/display_video_standard.h"
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_scanout.h"

/* The panel module selected in Kconfig (DEFAULT_MODULE) and the size of its frames: display_default_module.h */

#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_RGB888)
#define DEFAULT_FORMAT DISPLAY_FMT_RGB888
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_XRGB8888)
#define DEFAULT_FORMAT DISPLAY_FMT_XRGB8888
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_ARGB8888)
#define DEFAULT_FORMAT DISPLAY_FMT_ARGB8888
#else
#define DEFAULT_FORMAT DISPLAY_FMT_RGB565
#endif

#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_DCS)
/* RAM copy of the (const) module, so that the application can change it before display_init():
 * display_default_get_module() */
static display_dcs_module_t module_ram;
#endif

#if defined(CONFIG_DISPLAY_DEFAULT_DSI)
/* MIPI DSI: the host is the command port of the panel and carries its video */
#if defined(CONFIG_DISPLAY_DEFAULT_DSI_LANES_2130)
#define DEFAULT_LANE_ORDER DISPLAY_DSI_LANES_2130
#elif defined(CONFIG_DISPLAY_DEFAULT_DSI_LANES_1320)
#define DEFAULT_LANE_ORDER DISPLAY_DSI_LANES_1320
#elif defined(CONFIG_DISPLAY_DEFAULT_DSI_LANES_3120)
#define DEFAULT_LANE_ORDER DISPLAY_DSI_LANES_3120
#else
#define DEFAULT_LANE_ORDER DISPLAY_DSI_LANES_3210
#endif

_Static_assert(CONFIG_DISPLAY_DEFAULT_DSI_LANES != 3, "MIPI DSI has 1, 2 or 4 data lanes");

static display_dsi_host_t dsi =
    DISPLAY_DSI_HOST(.lanes_wired = CONFIG_DISPLAY_DEFAULT_DSI_LANES, .lane_order = DEFAULT_LANE_ORDER);
static display_dsi_io_t io = DISPLAY_DSI_IO(&dsi, 0, .iface = DISPLAY_IFACE_DSI_LP, .cs = DISPLAY_PIN_NONE);
static display_dcs_panel_t panel = DISPLAY_DCS_PANEL(&io.base, &module_ram, DEFAULT_PANEL_BOARD);
static display_bl618dg_dpu_scanout_t dpu = DISPLAY_BL618DG_DPU_SCANOUT(DEFAULT_FORMAT, .dsi = &dsi);
#else
/* RGB pins: the board muxes the pins of the layout */
#if defined(CONFIG_DISPLAY_DEFAULT_RGB_PINS_18_MODE1)
#define DEFAULT_PINS DISPLAY_BL618DG_DPU_PINS_18_MODE1
#elif defined(CONFIG_DISPLAY_DEFAULT_RGB_PINS_18_MODE2)
#define DEFAULT_PINS DISPLAY_BL618DG_DPU_PINS_18_MODE2
#elif defined(CONFIG_DISPLAY_DEFAULT_RGB_PINS_16_MODE1)
#define DEFAULT_PINS DISPLAY_BL618DG_DPU_PINS_16_MODE1
#elif defined(CONFIG_DISPLAY_DEFAULT_RGB_PINS_16_MODE2)
#define DEFAULT_PINS DISPLAY_BL618DG_DPU_PINS_16_MODE2
#elif defined(CONFIG_DISPLAY_DEFAULT_RGB_PINS_16_MODE3)
#define DEFAULT_PINS DISPLAY_BL618DG_DPU_PINS_16_MODE3
#else
#define DEFAULT_PINS DISPLAY_BL618DG_DPU_PINS_24
#endif

#if defined(CONFIG_DISPLAY_DEFAULT_RGB_CMD)
/* RGB panel with a driver IC: the command port is a software DBI Type C bus on GPIOs, commands only */
#if defined(CONFIG_DISPLAY_DEFAULT_RGB_CMD_NONE)
static display_dcs_panel_t panel = DISPLAY_DCS_PANEL_NO_IO(&module_ram, DEFAULT_PANEL_BOARD);
#else
#if defined(CONFIG_DISPLAY_DEFAULT_RGB_CMD_C4)
#define DEFAULT_CMD_IFACE DISPLAY_IFACE_DBI_C4
#define DEFAULT_CMD_DC    DISPLAY_PIN_HIGH(CONFIG_DISPLAY_DEFAULT_RGB_CMD_DC_PIN)
#else
#define DEFAULT_CMD_IFACE DISPLAY_IFACE_DBI_C3
#define DEFAULT_CMD_DC    DISPLAY_PIN_NONE
#endif
static display_soft_dbi_c_bus_t cmd_bus =
    DISPLAY_SOFT_DBI_C_BUS(.sck = DISPLAY_PIN_HIGH(CONFIG_DISPLAY_DEFAULT_RGB_CMD_SCK_PIN),
                           .mosi = DISPLAY_PIN_HIGH(CONFIG_DISPLAY_DEFAULT_RGB_CMD_MOSI_PIN), .miso = DISPLAY_PIN_NONE,
                           .dc = DEFAULT_CMD_DC);
static display_io_t cmd_io =
    DISPLAY_IO(&cmd_bus, .cs = DISPLAY_PIN_LOW(CONFIG_DISPLAY_DEFAULT_RGB_CMD_CS_PIN), .iface = DEFAULT_CMD_IFACE);
static display_dcs_panel_t panel = DISPLAY_DCS_PANEL(&cmd_io, &module_ram, DEFAULT_PANEL_BOARD);
#endif
#else
/* A standard panel has no command port */
static display_video_panel_t panel = DISPLAY_VIDEO_PANEL(DEFAULT_MODULE, DEFAULT_PANEL_BOARD);
#endif
static display_bl618dg_dpu_scanout_t dpu =
    DISPLAY_BL618DG_DPU_SCANOUT(DEFAULT_FORMAT, .pins = DEFAULT_PINS, .pins_init = board_display_dpu_gpio_init,
                                .pins_deinit = board_display_dpu_gpio_deinit);
#endif

/* Display: video panel, frames are switched with display_flip() */
static display_t display = DISPLAY_FB(&panel, &dpu);

/** @brief Copy the module to RAM on first use. */
static void default_module_init(void)
{
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_DCS)
    static bool copied;

    /* Not reentrant: call it from the application init, before display_init() */
    if (!copied) {
        module_ram = *DEFAULT_MODULE;
        copied = true;
    }
#endif
}

/** @brief See display_default.h. */
display_t *display_default_get(void)
{
    default_module_init();
    return &display;
}

#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_DCS)
/** @brief See display_default.h. */
display_dcs_module_t *display_default_get_module(void)
{
    default_module_init();
    return &module_ram;
}
#endif

