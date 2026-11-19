/**
 * @file display_types.h
 * @brief Common types shared by every layer of the display framework.
 *
 * This header has no hardware dependency and can be used in host builds.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_TYPES_H
#define DISPLAY_TYPES_H

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DISPLAY Display framework
 * @brief Layered display framework: Bus / IO / Panel / Scanout / Display.
 * @{
 */

/**
 * @defgroup DISPLAY_TYPES Common types
 * @{
 */

/**
 * @brief Pixel layout in memory.
 *
 * Zero is RGB565, the default of an endpoint that does not set `.format` (a command-only endpoint does not
 * need one). Frames of a scanout always set it, and a mismatch is caught by the stride and plane checks.
 *
 * The number of bits sent on the wire is selected separately, see @ref display_wire_t.
 */
typedef enum {
    DISPLAY_FMT_RGB565 = 0, /**< 2 bytes/pixel, little-endian uint16 RRRRRGGGGGGBBBBB */
    DISPLAY_FMT_RGB888,     /**< 3 bytes/pixel, packed, memory order B, G, R */
    DISPLAY_FMT_XRGB8888,   /**< 4 bytes/pixel, little-endian uint32 0xXXRRGGBB, X is ignored */
    DISPLAY_FMT_ARGB8888,   /**< Scanout only: same layout as XRGB8888, A is the non-premultiplied alpha */
    DISPLAY_FMT_YUYV,       /**< 2 bytes/pixel, Y0 U Y1 V, width must be even */
    DISPLAY_FMT_NV12,       /**< Scanout only: Y plane 1 byte/pixel + interleaved UV plane (U first), 4:2:0 */
    DISPLAY_FMT_UYVY,       /**< Scanout only: 2 bytes/pixel, U Y0 V Y1, width must be even */
    DISPLAY_FMT_AYUV8888,   /**< Scanout only: 4 bytes/pixel, little-endian 0xAAYYUUVV, A is not premultiplied */
    DISPLAY_FMT_COUNT       /**< Number of formats */
} display_format_t;

/**
 * @brief Bit of a format in a format mask.
 *
 * @param fmt a @ref display_format_t value
 */
#define DISPLAY_FMT_BIT(fmt) (1u << (fmt))

/**
 * @brief Get the number of bytes per pixel of a format.
 *
 * @param [in] format pixel format
 * @return bytes per pixel (NV12 counts the Y plane only), 0 for an invalid format
 */
static inline size_t display_format_bytes(display_format_t format)
{
    static const uint8_t bytes[DISPLAY_FMT_COUNT] = { 2, 3, 4, 4, 2, 1, 2, 4 };

    return (unsigned)format < DISPLAY_FMT_COUNT ? bytes[format] : 0;
}

/**
 * @brief Pixel width on the wire, i.e. the interface pixel class (COLMOD) of the panel.
 *
 * RGB666 is sent exactly like RGB888 (3 bytes per pixel) and counts as 24 bit.
 */
typedef enum {
    DISPLAY_WIRE_AUTO = 0, /**< Zero, the default: derived from the memory format, RGB565 -> 16 bit, others -> 24 bit */
    DISPLAY_WIRE_16,       /**< 16 bit/pixel (RGB565) */
    DISPLAY_WIRE_24        /**< 24 bit/pixel (RGB888, RGB666) */
} display_wire_t;

/**
 * @brief Resolve @ref DISPLAY_WIRE_AUTO for a memory format.
 *
 * @param [in] format memory pixel format
 * @param [in] wire   configured wire width
 * @return the effective wire width, never @ref DISPLAY_WIRE_AUTO
 */
static inline display_wire_t display_wire_resolve(display_format_t format, display_wire_t wire)
{
    if (wire != DISPLAY_WIRE_AUTO) {
        return wire;
    }
    return format == DISPLAY_FMT_RGB565 ? DISPLAY_WIRE_16 : DISPLAY_WIRE_24;
}

/**
 * @brief Panel rotation, clockwise.
 */
typedef enum {
    DISPLAY_ROTATION_0 = 0, /**< Native orientation */
    DISPLAY_ROTATION_90,    /**< 90 degrees */
    DISPLAY_ROTATION_180,   /**< 180 degrees */
    DISPLAY_ROTATION_270    /**< 270 degrees */
} display_rotation_t;

/**
 * @brief Rectangle in the current rotation.
 */
typedef struct {
    uint16_t x; /**< Left */
    uint16_t y; /**< Top */
    uint16_t w; /**< Width */
    uint16_t h; /**< Height */
} display_area_t;

/**
 * @brief A frame buffer for a scanout engine.
 *
 * @ref data is the identity of the frame. For NV12, the UV plane belongs to the same frame and is handed
 * over together with it.
 */
typedef struct {
    void *data;              /**< Frame (Y plane for NV12) */
    uint32_t stride;         /**< Bytes per line: the line length (frames are tightly packed), or 0 for the same */
    display_format_t format; /**< Pixel format */
    void *uv;                /**< NV12 only: UV plane */
    uint32_t uv_stride;      /**< NV12 only: bytes per UV line, same rule as stride */
} display_fb_t;

/**
 * @brief Description of a control GPIO (CS, DC, reset, power, ...).
 *
 * Use the initializer macros, e.g. `.cs = DISPLAY_PIN_LOW(20)`.
 * A negative pin number is the same as @ref DISPLAY_PIN_NONE, which lets Kconfig use -1 for "not wired".
 */
typedef struct {
    uint8_t num;   /**< GPIO number */
    uint8_t flags; /**< DISPLAY_PIN_USED / DISPLAY_PIN_ACTIVE_LOW */
} display_pin_t;

/**
 * @name Pin flags
 * @{
 */
#define DISPLAY_PIN_USED       0x01 /**< The pin is wired */
#define DISPLAY_PIN_ACTIVE_LOW 0x02 /**< The pin is active low */
/** @} */

/**
 * @brief Pin that is active high.
 *
 * @param n GPIO number, negative when not wired
 */
#define DISPLAY_PIN_HIGH(n)    { .num = (uint8_t)((n) < 0 ? 0 : (n)), .flags = (n) < 0 ? 0 : DISPLAY_PIN_USED }
/**
 * @brief Pin that is active low.
 *
 * @param n GPIO number, negative when not wired
 */
#define DISPLAY_PIN_LOW(n) \
    { .num = (uint8_t)((n) < 0 ? 0 : (n)), .flags = (n) < 0 ? 0 : (DISPLAY_PIN_USED | DISPLAY_PIN_ACTIVE_LOW) }
/** @brief Pin that is not wired. */
#define DISPLAY_PIN_NONE { .num = 0, .flags = 0 }

/**
 * @brief Check whether a pin is wired.
 *
 * @param [in] pin pin description
 * @return true if the pin is wired
 */
static inline bool display_pin_used(const display_pin_t *pin)
{
    return (pin->flags & DISPLAY_PIN_USED) != 0;
}

/**
 * @defgroup DISPLAY_IFACE Command interfaces
 *
 * The framework describes a connection with a small set of standard interface types (MIPI DBI
 * naming) instead of backend specific modes:
 * - a bus backend declares which interfaces it can drive and what it can do on each of them;
 * - a panel IC declares which interfaces it supports and its standard settings on each of them
 *   (@ref display_link_t);
 * - an endpoint names the interface its panel is wired to.
 *
 * Whether a bus can drive an interface is checked when the endpoint is attached, whether the panel
 * IC supports it when the panel is initialized. The same interface has the same settings on every
 * backend: a hardware SPI and the DBI peripheral both implement DISPLAY_IFACE_DBI_C4, the SPI just
 * supports fewer pixel formats.
 *
 * @{
 */

/**
 * @brief Interface type of a command/pixel connection.
 *
 * "DBI" in the names is the MIPI DBI interface type; the DBI peripheral of the chip is a backend
 * that implements several of them.
 */
typedef enum {
    DISPLAY_IFACE_NONE = 0, /**< Not set */
    DISPLAY_IFACE_DBI_B8,   /**< MIPI DBI Type B: 8080 parallel, 8 data lines */
    DISPLAY_IFACE_DBI_C3,   /**< MIPI DBI Type C, 3-wire: SCL + SDA, D/C is the 9th bit */
    DISPLAY_IFACE_DBI_C4,   /**< MIPI DBI Type C, 4-wire: SCL + SDA + D/CX */
    DISPLAY_IFACE_QSPI,     /**< Vendor QSPI: 1-wire opcode, address and data on 1 or 4 wires */
    DISPLAY_IFACE_DSI_LP, /**< MIPI DSI low-power (LP) command packets; the same host carries the video of the panel */
    DISPLAY_IFACE_COUNT   /**< Number of interface types */
} display_iface_t;

/** @brief Bit of an interface in an interface mask. */
#define DISPLAY_IFACE_BIT(iface)        (1u << (iface))

/** @brief Default clock of an interface when neither the endpoint nor the panel IC sets one. */
#define DISPLAY_IFACE_DEFAULT_HZ(iface) ((iface) == DISPLAY_IFACE_DBI_B8 ? 27000000u : 40000000u)

/**
 * @brief Command encoding of a panel IC on an interface.
 *
 * It depends on the interface, not on the panel IC alone (NT35510 uses 16-bit addresses on 8080 but
 * standard DCS on MIPI DSI). Panel drivers always send standard DCS; the bus core expands it.
 */
typedef enum {
    DISPLAY_CMD_8 = 0,     /**< Standard DCS, 8-bit command */
    DISPLAY_CMD_16_INDEXED /**< 16-bit address: high byte = command, low byte = parameter index,
                                each parameter is sent as 0x00, value */
} display_cmd_mode_t;

/**
 * @brief Wires of the QSPI address and data phases. The opcode phase always uses 1 wire.
 */
typedef enum {
    DISPLAY_QSPI_LINES_1_1 = 0, /**< address 1 wire, data 1 wire */
    DISPLAY_QSPI_LINES_1_4,     /**< address 1 wire, data 4 wires */
    DISPLAY_QSPI_LINES_4_4      /**< address 4 wires, data 4 wires */
} display_qspi_lines_t;

/**
 * @brief One QSPI transfer layout: header code and wires.
 *
 * Header sent on the wire: opcode = code[31:24]; 24-bit address = code[23:0] | (DCS command << 8).
 * Example: code 0x38000000 and command 0x2C give opcode 0x38, address 0x002C00.
 */
typedef struct {
    uint32_t code;              /**< Opcode and address template */
    display_qspi_lines_t lines; /**< Wires of the address and data phases */
} display_qspi_layout_t;

/**
 * @brief QSPI protocol of a panel IC: parameters and pixels may need different layouts
 *        (e.g. parameters 0x02 on 1 wire, pixels 0x32 on 4 wires).
 */
typedef struct {
    display_qspi_layout_t cmd;   /**< Commands with parameters */
    display_qspi_layout_t color; /**< Pixel data */
} display_qspi_proto_t;

/**
 * @brief QSPI protocol initializer, e.g.
 *        `DISPLAY_QSPI_PROTO(0x38000000, DISPLAY_QSPI_LINES_4_4, 0x38000000, DISPLAY_QSPI_LINES_4_4)` (ST77926).
 */
#define DISPLAY_QSPI_PROTO(_cmd_code, _cmd_lines, _color_code, _color_lines) \
    {                                                                        \
        .cmd = { .code = (_cmd_code), .lines = (_cmd_lines) },               \
        .color = { .code = (_color_code), .lines = (_color_lines) }          \
    }

/**
 * @name MIPI DSI packet flags
 * @{
 */
#define DISPLAY_DSI_PKT_GENERIC (1u << 0) /**< Commands go as generic write packets instead of DCS write packets */
#define DISPLAY_DSI_PKT_LONG    (1u << 1) /**< Every command goes as a long packet, even with 0 or 1 parameter */
/** @} */

/**
 * @brief Standard settings of a panel IC on one interface. Provided by the IC description.
 *
 * Pixels on the wire come in two classes, as in MIPI DBI: 16 bit (RGB565) and 24 bit (RGB888).
 * RGB666 is sent exactly like RGB888 (3 bytes per pixel), so it is the 24-bit class: put the COLMOD
 * of the widest RGB888/RGB666 mode of the IC in @ref colmod.
 */
typedef struct {
    display_iface_t iface;       /**< Interface these settings are for */
    uint32_t default_hz;         /**< Clock when the endpoint sets none; 0: @ref DISPLAY_IFACE_DEFAULT_HZ */
    display_cmd_mode_t cmd_mode; /**< Command encoding */
    uint8_t colmod[2];           /**< COLMOD for 16 / 24 bit on the wire; 0: not supported on this interface */
    display_qspi_proto_t qspi;   /**< QSPI only */
    uint8_t dsi_flags;           /**< MIPI DSI only: DISPLAY_DSI_PKT_*; 0: DCS packets, sized by the parameter count */
} display_link_t;

/** @} */ /* DISPLAY_IFACE */

/**
 * @brief Kind of the link that carries pixels to a video panel.
 *
 * The link decides what one clock moves, so it also decides how the timing fields are counted, see
 * @ref display_video_mode_t.
 */
typedef enum {
    DISPLAY_VLINK_NONE = 0, /**< Not set */
    DISPLAY_VLINK_DPI,      /**< Parallel RGB, one pixel per clock */
    DISPLAY_VLINK_SRGB,     /**< Serial RGB, one sub-pixel per clock (RGB888: three clocks per pixel) */
    DISPLAY_VLINK_DSI       /**< MIPI DSI video mode */
} display_vlink_t;

/**
 * @brief Bit of a link in a link mask.
 *
 * @param link a @ref display_vlink_t value
 */
#define DISPLAY_VLINK_BIT(link) (1u << (link))

/**
 * @brief Pixel format on a MIPI DSI link, i.e. the DSI data type (zero is not valid).
 */
typedef enum {
    DISPLAY_DSI_FMT_RGB565 = 1, /**< 16 bit/pixel */
    DISPLAY_DSI_FMT_RGB666,     /**< 18 bit/pixel, packed */
    DISPLAY_DSI_FMT_RGB888      /**< 24 bit/pixel */
} display_dsi_format_t;

/**
 * @brief MIPI D-PHY timing override, as register values of the DSI host.
 *
 * Optional per module: a panel that cannot work with the default timing of the host sets it, all other
 * panels leave @ref display_dsi_link_t::phy at NULL.
 */
typedef struct {
    uint8_t clk_zero;     /**< Clock lane HS-ZERO */
    uint8_t clk_trail;    /**< Clock lane HS-TRAIL */
    uint8_t clk_exit;     /**< Clock lane HS-EXIT */
    uint8_t data_zero;    /**< Data lane HS-ZERO */
    uint8_t data_trail;   /**< Data lane HS-TRAIL */
    uint8_t data_exit;    /**< Data lane HS-EXIT */
    uint8_t data_prepare; /**< Data lane HS-PREPARE */
    uint8_t ta_go;        /**< Turnaround GO */
    uint8_t ta_get;       /**< Turnaround GET */
    uint8_t req_ready;    /**< Request to ready */
    uint8_t lpx;          /**< Low-power transmission */
    uint32_t wakeup;      /**< Wake-up, in clock cycles */
} display_dsi_phy_t;

/**
 * @brief MIPI DSI parameters of a video link.
 */
typedef struct {
    uint8_t lanes;                /**< Number of data lanes (1, 2 or 4) */
    uint16_t lane_mbps;           /**< Bit rate of one data lane, Mbit/s */
    display_dsi_format_t format;  /**< Data type on the link */
    const display_dsi_phy_t *phy; /**< D-PHY timing override, NULL: default of the host */
} display_dsi_link_t;

/**
 * @brief Video link of an RGB or MIPI DSI video panel: link kind, timing and clock. Provided by the panel module.
 *
 * The values come from the data sheet of the panel and are fixed per module: the framework neither plans
 * them nor checks that they agree, the backend only maps them to its hardware. Signal invert flags
 * reverse backend defaults on DPI/SRGB links. False keeps the default; unsupported flags are ignored.
 *
 * Horizontal values count link clocks (pixels on DPI and DSI, sub-pixel clocks on SRGB), vertical values
 * count lines. @ref width and @ref height count pixels.
 */
typedef struct {
    display_vlink_t link;   /**< Kind of the link */
    uint16_t width;         /**< Active width, pixels */
    uint16_t height;        /**< Active height, lines */
    uint16_t hsw;           /**< HSYNC pulse width */
    uint16_t hbp;           /**< Horizontal back porch */
    uint16_t hfp;           /**< Horizontal front porch */
    uint16_t vsw;           /**< VSYNC pulse width */
    uint16_t vbp;           /**< Vertical back porch */
    uint16_t vfp;           /**< Vertical front porch */
    uint32_t clock_hz;      /**< Link clock: pixel clock (DPI, DSI) or sub-pixel clock (SRGB) */
    display_dsi_link_t dsi; /**< DSI only */
    bool pclk_invert;       /**< DPI/SRGB: reverse the default pixel clock edge, if supported */
    bool hsync_invert;      /**< DPI/SRGB: reverse the default HSYNC polarity, if supported */
    bool vsync_invert;      /**< DPI/SRGB: reverse the default VSYNC polarity, if supported */
    bool de_invert;         /**< DPI/SRGB: reverse the default DE polarity, if supported */
} display_video_mode_t;

/**
 * @brief Display event type.
 */
typedef enum {
    DISPLAY_EVT_DRAW_DONE = 0, /**< GRAM: the pixel buffer can be reused */
    DISPLAY_EVT_FB_RETIRED,    /**< Scanout: the previous frame left the scan-out and can be rewritten */
    DISPLAY_EVT_VSYNC,         /**< Scanout: a frame boundary without a completed operation */
    DISPLAY_EVT_FB_FIRST,      /**< Scanout: the first frame is active; no previous frame is released */
    DISPLAY_EVT_FB_LAST        /**< Scanout: disabled; the last frame can be reused */
} display_event_type_t;

/** @brief One asynchronous frame operation; NONE is distinct from a pending NULL (disable). */
typedef enum {
    DISPLAY_FRAME_OP_NONE = 0,
    DISPLAY_FRAME_OP_FIRST,
    DISPLAY_FRAME_OP_FLIP,
    DISPLAY_FRAME_OP_LAST
} display_frame_op_t;

/**
 * @brief Display event.
 */
typedef struct {
    display_event_type_t type; /**< Event type */
    int result;                /**< DRAW_DONE: 0 or a negative errno; FIRST / RETIRED / LAST / VSYNC: always 0 */
    const void *buf;           /**< DRAW_DONE: reusable buffer; FIRST / RETIRED / LAST / VSYNC: always NULL */
} display_event_t;

/**
 * @brief Completion callback of one asynchronous request (IO and Panel layers).
 *
 * Called from an ISR, or from the calling task when the bus has no asynchronous support.
 * Only notify from here; do not start another transfer.
 *
 * @param [in] result 0 or a negative errno
 * @param [in] buf    the pixel buffer of the request, which can be reused now
 * @param [in] ctx    user context of the request
 */
typedef void (*display_done_cb_t)(int result, const void *buf, void *ctx);

/** @} */ /* DISPLAY_TYPES */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_TYPES_H */
