/**
 * @file display_panel_dcs.h
 * @brief Generic MIPI DCS panel driver, covering most SPI / DBI / QSPI / DSI panels.
 *
 * The driver is data driven, with two levels of data:
 * - IC description (@ref display_dcs_ic_t), one per driver IC: the interfaces it supports with its
 *   standard settings on each (@ref display_link_t), default initialization sequence (and the ones that
 *   differ per interface), reset timing;
 * - module description (@ref display_dcs_module_t), one per panel: size, rotation, offsets,
 *   inversion, the video links, the interfaces the module is strapped for, and optionally a replacement
 *   or an extension of the initialization sequence (e.g. the gamma of a specific module).
 *
 * At init the driver looks up the interface of the endpoint in the IC links (and the module's mask) and
 * loads the matching link into the endpoint; an interface the IC or module does not support is rejected
 * with -ENOTSUP.
 *
 * An IC that offers a command port next to a video link (e.g. an RGB panel with a 3-wire SPI port, or
 * a panel that has both an RGB and a MIPI DSI input) is one IC description. The interface of the
 * endpoint also selects the video link of the module: DSI_LP the DSI link, every other interface the
 * parallel / serial RGB link.
 *
 * Every supported IC provides its IC description and a default module in panel/<ic>/.
 * Users write their own module descriptions in their project, see panel/template/.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_PANEL_DCS_H
#define DISPLAY_PANEL_DCS_H

#include "panel/display_panel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL
 * @{
 */

/**
 * @defgroup DISPLAY_PANEL_DCS Generic DCS panel driver
 * @{
 */

/**
 * @brief One command of an initialization sequence.
 */
typedef struct {
    uint8_t cmd;         /**< Command */
    uint8_t len;         /**< Number of parameters */
    uint16_t delay_ms;   /**< Delay after the command */
    const uint8_t *data; /**< Parameters */
    uint16_t io_flags;   /**< MIPI DSI packet type: DISPLAY_IO_MSG_DSI_* (other interfaces ignore it) */
} display_dcs_cmd_t;

/**
 * @brief Command with parameters.
 *
 * @code
 * DISPLAY_DCS(0xB2, 0, 0x0C, 0x0C, 0x00, 0x33, 0x33)  // 0xB2 with 5 parameters, no delay
 * @endcode
 */
#define DISPLAY_DCS(_cmd, _delay, ...)                 \
    { .cmd = (_cmd),                                   \
      .len = sizeof((const uint8_t[]){ __VA_ARGS__ }), \
      .delay_ms = (_delay),                            \
      .data = ((const uint8_t[]){ __VA_ARGS__ }) }

/**
 * @brief Command with parameters, sent as a MIPI DSI generic long write (other interfaces send it as usual).
 */
#define DISPLAY_DCS_GENERIC_LONG(_cmd, _delay, ...)    \
    { .cmd = (_cmd),                                   \
      .len = sizeof((const uint8_t[]){ __VA_ARGS__ }), \
      .delay_ms = (_delay),                            \
      .data = ((const uint8_t[]){ __VA_ARGS__ }),      \
      .io_flags = DISPLAY_IO_MSG_DSI_GENERIC | DISPLAY_IO_MSG_DSI_LONG }

/**
 * @brief Command without parameters, e.g. `DISPLAY_DCS0(0x11, 120)` (sleep out, then 120 ms).
 */
#define DISPLAY_DCS0(_cmd, _delay) { .cmd = (_cmd), .len = 0, .delay_ms = (_delay), .data = NULL }

/** @brief Number of commands of a sequence array. */
#define DISPLAY_DCS_COUNT(_t)      ((uint16_t)(sizeof(_t) / sizeof((_t)[0])))

/**
 * @brief Initialization sequence that replaces the default one of an IC on some interfaces.
 */
typedef struct {
    uint8_t iface_mask;            /**< Interfaces it applies to, DISPLAY_IFACE_BIT() */
    const display_dcs_cmd_t *init; /**< Sequence */
    uint16_t init_count;           /**< Number of commands */
} display_dcs_iface_init_t;

/**
 * @brief Driver IC description.
 *
 * The default sequence must not contain 0x29/0x28 (sent by enable/disable). For GRAM panels it must
 * not contain 0x36/0x3A either (sent from the rotation and the endpoint wire width). For video panels
 * 0x3A selects the RGB interface width and must be in the sequence.
 */
typedef struct {
    const char *name;              /**< Name */
    uint16_t ram_width;            /**< GRAM width, used to check offsets */
    uint16_t ram_height;           /**< GRAM height */
    uint8_t x_align;               /**< Window alignment in x (e.g. 2 or 4 for some QSPI panels) */
    uint8_t y_align;               /**< Window alignment in y */
    const display_link_t *links;   /**< Interfaces the IC supports, with its settings on each */
    uint8_t link_count;            /**< Number of links */
    uint16_t reset_low_ms;         /**< Hardware reset pulse */
    uint16_t reset_wait_ms;        /**< Wait after hardware reset */
    const display_dcs_cmd_t *init; /**< Default initialization sequence */
    uint16_t init_count;           /**< Number of commands */
    const display_dcs_iface_init_t
        *iface_init;          /**< Sequences used instead of the default one on some interfaces; may be NULL */
    uint8_t iface_init_count; /**< Number of iface_init entries */
    const display_dcs_cmd_t *invert_seq; /**< IC specific commands sent before INVON (inversion mode); may be NULL */
    uint16_t invert_seq_count;           /**< Number of commands of invert_seq */
    uint8_t flags;                       /**< DISPLAY_DCS_IC_* */
} display_dcs_ic_t;

/**
 * @name IC flags
 * @{
 */
/**
 * The driver does not send DISPON when the display is initialized: the init sequence leaves the display
 * off and the application turns it on with its own command after the first frame (e.g. an AMOLED panel). Not
 * for MIPI DSI video panels: LP commands are refused while the video runs.
 */
#define DISPLAY_DCS_IC_NO_AUTO_DISPLAY_ON (1u << 0)
/**
 * The IC does not take the display on/off and sleep commands (0x29, 0x28, 0x10): the driver sends none of them, and
 * the display is on as soon as it receives video.
 */
#define DISPLAY_DCS_IC_NO_DISPLAY_CMDS    (1u << 1)
/** @} */

/**
 * @brief Panel module description: one glass on top of a driver IC.
 *
 * Index [r] of the per-rotation arrays is a display_rotation_t value.
 */
typedef struct {
    const char *name;              /**< Name */
    const display_dcs_ic_t *ic;    /**< Driver IC */
    uint16_t width;                /**< Visible width at 0 degrees */
    uint16_t height;               /**< Visible height at 0 degrees */
    uint8_t rotation_mask;         /**< Supported rotations; 0 means 0 degrees only */
    uint8_t madctl[4];             /**< MADCTL per rotation (including RGB/BGR and mirroring) */
    uint16_t gap_x[4];             /**< GRAM column offset per rotation */
    uint16_t gap_y[4];             /**< GRAM row offset per rotation */
    uint8_t iface_mask;            /**< Interfaces the module is strapped for (DISPLAY_IFACE_BIT); 0: all of the IC */
    bool invert;                   /**< Send INVON */
    const display_dcs_cmd_t *init; /**< NULL: IC default sequence; otherwise replaces it */
    uint16_t init_count;           /**< Number of commands of init */
    const display_dcs_cmd_t *init_extra; /**< Sent after the sequence, e.g. module gamma or voltages */
    uint16_t init_extra_count;           /**< Number of commands of init_extra */
    const display_dcs_cmd_t
        *init_user; /**< Sent last in prepare(): commands of the application (for the default display set through
                      display_default_get_module()) */
    uint16_t init_user_count;          /**< Number of commands of init_user */
    const display_video_mode_t *video; /**< Video links of the panel; NULL: GRAM panel */
    uint8_t video_count;               /**< Number of video links */
    uint16_t video_start_delay_ms;     /**< Video panels: wait between the last command and the first pixel */
} display_dcs_module_t;

/** @brief Field initializer: IC specific commands that go with INVON. */
#define DISPLAY_DCS_INVERT_SEQ(_t) .invert_seq = (_t), .invert_seq_count = DISPLAY_DCS_COUNT(_t)
/** @brief Field initializer: replace the IC default sequence. */
#define DISPLAY_DCS_INIT(_t)       .init = (_t), .init_count = DISPLAY_DCS_COUNT(_t)
/** @brief Field initializer: sequences of an IC that differ per interface. */
#define DISPLAY_DCS_IFACE_INIT(_t) .iface_init = (_t), .iface_init_count = DISPLAY_DCS_COUNT(_t)
/** @brief Field initializer: the video links of a module (array of @ref display_video_mode_t). */
#define DISPLAY_DCS_VIDEO(_t)      .video = (_t), .video_count = DISPLAY_DCS_COUNT(_t)
/** @brief Field initializer: the interfaces of an IC. */
#define DISPLAY_DCS_LINKS(_t)      .links = (_t), .link_count = DISPLAY_DCS_COUNT(_t)
/** @brief Field initializer: commands sent after the sequence. */
#define DISPLAY_DCS_INIT_EXTRA(_t) .init_extra = (_t), .init_extra_count = DISPLAY_DCS_COUNT(_t)

/**
 * @brief Display inversion choice of display_dcs_module_adjust().
 */
typedef enum {
    DISPLAY_DCS_INVERT_KEEP = 0, /**< Keep the setting of the module */
    DISPLAY_DCS_INVERT_OFF,      /**< No inversion */
    DISPLAY_DCS_INVERT_ON        /**< Inversion (INVON, plus the IC specific commands) */
} display_dcs_invert_t;

/**
 * @brief Make a RAM copy of a module with the common orientation and color settings changed.
 *
 * MADCTL and INVON are standard DCS, so this works for every module. The copy is meant to be passed to
 * DISPLAY_DCS_PANEL() in place of @p src, which must outlive it.
 *
 * - @p swap_rgb_bgr toggles the BGR bit (0x08) of every rotation;
 * - @p mirror mirrors the displayed image horizontally at every rotation: it toggles MX (0x40), or MY (0x80)
 *   when the rotation uses MV;
 * - @p invert sets or keeps the INVON setting.
 *
 * Only the rotations enabled by the module (and 0 degrees) are changed.
 *
 * @param [out] dst          copy
 * @param [in]  src          module
 * @param [in]  swap_rgb_bgr toggle RGB/BGR order
 * @param [in]  mirror       mirror horizontally
 * @param [in]  invert       inversion choice
 */
void display_dcs_module_adjust(display_dcs_module_t *dst, const display_dcs_module_t *src, bool swap_rgb_bgr,
                               bool mirror, display_dcs_invert_t invert);

/**
 * @brief Panel object of the generic DCS driver.
 */
typedef struct {
    display_panel_t base;               /**< Panel core, must be first */
    const display_dcs_module_t *module; /**< Module description */
} display_dcs_panel_t;

/** @brief Driver operations of the generic DCS panel. */
extern const display_panel_ops_t display_dcs_panel_ops;

/**
 * @brief Static definition of a DCS panel.
 *
 * @code
 * static display_dcs_panel_t panel = DISPLAY_DCS_PANEL(&io, &display_module_st7796_320x480, DISPLAY_BOARD_NONE);
 * @endcode
 */
#define DISPLAY_DCS_PANEL(_io, _module, _board) \
    { .base = { .ops = &display_dcs_panel_ops, .io = (_io), .board = _board }, .module = (_module) }

/** @brief Static definition of an RGB panel without command port (no IO endpoint). */
#define DISPLAY_DCS_PANEL_NO_IO(_module, _board) \
    { .base = { .ops = &display_dcs_panel_ops, .io = NULL, .board = _board }, .module = (_module) }

/** @} */ /* DISPLAY_PANEL_DCS */
/** @} */ /* DISPLAY_PANEL */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_PANEL_DCS_H */
