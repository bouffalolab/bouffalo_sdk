/**
 * @file display_default.h
 * @brief The default display: one screen defined from Kconfig (CONFIG_DISPLAY_DEFAULT).
 *
 * For the common case of a single screen without special needs. The display is defined statically in
 * display_default_gram.c or display_default_video.c from the options of default/Kconfig; the application, or a port
 * such as the LVGL display port, gets it with display_default_get() and drives it like any other display:
 * display_init() -> first frame with display_draw() -> display_set_backlight(display, DISPLAY_BACKLIGHT_MAX).
 * A video display (CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO) is scanned from frame buffers that the application owns: it
 * calls display_init(), registers its callback, then submits the first and later frames with display_flip().
 * Initialization reads no frame. display_flip(display, NULL) disables the layer and LAST returns the final frame.
 *
 * Only build code that uses it when CONFIG_DISPLAY_DEFAULT is set; projects with several screens, or with
 * special needs, define their displays in C and use their own port.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_DEFAULT_H
#define DISPLAY_DEFAULT_H

#include "display.h"
#include "panel/dcs/display_panel_dcs.h"
#include "default/display_default_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_CORE
 * @{
 */

/**
 * @brief Get the default display.
 *
 * @note The display is not initialized; call display_init() first.
 *
 * @return The default display, never NULL.
 */
display_t *display_default_get(void);

/** Bytes per pixel of the default display, from its Kconfig pixel format. */
#if defined(CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO)
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_XRGB8888) || defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_ARGB8888)
#define DISPLAY_DEFAULT_BPP 4
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_RGB888)
#define DISPLAY_DEFAULT_BPP 3
#else
#define DISPLAY_DEFAULT_BPP 2
#endif
#elif defined(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM)
#if defined(CONFIG_DISPLAY_DEFAULT_FORMAT_XRGB8888)
#define DISPLAY_DEFAULT_BPP 4
#elif defined(CONFIG_DISPLAY_DEFAULT_FORMAT_RGB888)
#define DISPLAY_DEFAULT_BPP 3
#else
#define DISPLAY_DEFAULT_BPP 2
#endif
#endif

#if defined(CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO)
/** @name Frames of the default video display, from the Kconfig options (for code that provides more frames) */
/** @{ */
/* DISPLAY_DEFAULT_VIDEO_WIDTH and DISPLAY_DEFAULT_VIDEO_HEIGHT: display_default_module.h */
#define DISPLAY_DEFAULT_VIDEO_BPP DISPLAY_DEFAULT_BPP /**< Bytes per pixel; retained for existing users */
/** Bytes of a frame, tightly packed */
#define DISPLAY_DEFAULT_VIDEO_FRAME_BYTES \
    (DISPLAY_DEFAULT_VIDEO_WIDTH * DISPLAY_DEFAULT_VIDEO_HEIGHT * DISPLAY_DEFAULT_VIDEO_BPP)
/** @} */

#endif

#if defined(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM) || defined(CONFIG_DISPLAY_DEFAULT_VIDEO_DCS)
/**
 * @brief Get the RAM copy of the panel module of the default display, to change it before display_init().
 *
 * The default display uses a copy of the module selected in Kconfig. Change what the board needs in it, for
 * example the orientation and color order of a GRAM panel (`madctl[]`), the inversion (`invert`), or the
 * commands of the module: `init_extra` replaces the extra commands of the module, `init_user` adds commands that
 * are sent last. A table of commands must stay valid as long as the display is used (a `static` array).
 *
 * @code
 * display_dcs_module_t *module = display_default_get_module();
 * module->madctl[0] = 0x48;
 * module->invert = true;
 * @endcode
 *
 * Only before display_init(): the driver reads the module when the display is initialized. Not available for
 * a standard RGB panel, which has no command port and no module of this kind.
 *
 * @note Call it from the application init, from one task: the copy is made on the first call of
 *       display_default_get() or of this function.
 *
 * @return The module, never NULL.
 */
display_dcs_module_t *display_default_get_module(void);
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_DEFAULT_H */
