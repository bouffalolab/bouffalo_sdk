/**
 * @file display_io.h
 * @brief IO endpoint (L2): the connection of one panel to one bus.
 *
 * An endpoint is "bus + software CS + how this panel is wired and used": the interface type, clock,
 * pixel format and width (@ref display_io_cfg_t). The settings that belong to the panel IC on that
 * interface (command encoding, QSPI protocol, default clock) are loaded by the panel driver with
 * display_io_set_link().
 *
 * Endpoints of every backend are defined with the same macro, DISPLAY_IO(). Whenever the bus switches
 * to an endpoint, the backend reconfigures the controller for it through LHAL, so several panels with
 * different interfaces, clocks and pixel formats can share one bus.
 *
 * Applications may call these functions directly, e.g. to read the panel ID, send vendor
 * commands, or push pixels without a panel driver.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_IO_H
#define DISPLAY_IO_H

#include "bus/display_bus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_IO IO endpoint
 * @{
 */

/** @brief Endpoint object. */
typedef struct display_io display_io_t;

/**
 * @brief Generic endpoint settings: how the panel is wired and used. Set by DISPLAY_IO().
 *
 * The settings that depend on the panel IC (command encoding, QSPI protocol, default clock) come from
 * the IC description through display_io_set_link().
 *
 * Fields left out of the static definition are zero, and zero is never an accident:
 * - `cs`: DISPLAY_PIN_NONE, the panel is always selected (it must then own the bus);
 * - `iface`: DISPLAY_IFACE_NONE, rejected: the interface has to be named;
 * - `freq_hz`: the default of the panel IC or of the interface;
 * - `format`: RGB565; `wire`: derived from the format;
 * - `dcache_dis`: the bus cleans the data cache of the pixels sent by DMA.
 */
typedef struct {
    display_pin_t cs;        /**< Software CS, selects this panel on a shared bus. DISPLAY_PIN_NONE: the panel
                                  is always selected (no CS, or CS driven by the controller); whether such an
                                  endpoint can share the bus is up to the backend */
    display_iface_t iface;   /**< Interface the panel is wired to; the bus must be able to drive it */
    uint32_t freq_hz;        /**< Clock; 0: from the panel IC, or @ref DISPLAY_IFACE_DEFAULT_HZ. Ignored by
                                  buses without a settable clock (software SPI) */
    display_format_t format; /**< Pixel format in memory, must be supported by the bus on this interface */
    display_wire_t wire;     /**< Pixel width on the wire (panel COLMOD); AUTO derives it from format */
    bool dcache_dis;         /**< 0 (default): the bus writes back the data cache of the pixels it sends by DMA
                                  before the transfer starts. 1: it does not, the caller manages the cache (for
                                  example it has already cleaned exactly the lines it wrote) */
} display_io_cfg_t;

/** @brief Message without a command phase (data only). */
#define DISPLAY_IO_NO_CMD      UINT32_MAX

/**
 * @name Message flags
 * @{
 */
#define DISPLAY_IO_MSG_COLOR   (1u << 0) /**< Pixel data: last message only, may use DMA */
#define DISPLAY_IO_MSG_READ    (1u << 1) /**< Read len bytes into rx after the command */
#define DISPLAY_IO_MSG_CS_KEEP (1u << 2) /**< Keep CS asserted and merge with the next message */
#define DISPLAY_IO_MSG_DSI_GENERIC \
    (1u << 3) /**< MIPI DSI: send this command as a generic write instead of a DCS write */
#define DISPLAY_IO_MSG_DSI_LONG \
    (1u << 4) /**< MIPI DSI: send this command as a long packet, even with 0 or 1 parameter */
/** @} */

/**
 * @brief One "command + data" transaction on the wire.
 */
typedef struct {
    uint32_t cmd;      /**< DCS command, or DISPLAY_IO_NO_CMD */
    const void *tx;    /**< Parameters or pixels to write */
    void *rx;          /**< Read buffer, DISPLAY_IO_MSG_READ only */
    uint32_t len;      /**< Number of bytes */
    uint16_t flags;    /**< DISPLAY_IO_MSG_* */
    uint16_t delay_ms; /**< Delay after this message; not allowed on an asynchronous pixel message */
} display_io_msg_t;

/**
 * @name Endpoint capability flags
 * @{
 */
#define DISPLAY_IO_CAP_DMA        (1u << 0) /**< Pixels are sent by DMA (synchronous and asynchronous calls); otherwise by CPU */
#define DISPLAY_IO_CAP_READ       (1u << 1) /**< Read is supported */
#define DISPLAY_IO_CAP_FIXED_WIRE (1u << 2) /**< The wire width follows the memory format (RGB565: 16 bit, others: 24 bit);
                                                 other combinations are rejected when the endpoint is attached */
/** @} */

/**
 * @brief Endpoint capabilities, filled by the bus backend when the endpoint is attached.
 */
typedef struct {
    uint32_t flags;           /**< DISPLAY_IO_CAP_* */
    uint32_t format_mask;     /**< Memory formats the bus can send; 0: commands only */
    uint32_t max_param_bytes; /**< Maximum parameter length of one message */
    uint32_t max_color_bytes; /**< Maximum pixel length of one message */
    uint8_t color_align;      /**< Required pixel buffer address alignment in bytes, 0/1: none. This is about the
                                   address only: the DBI reads 32-bit words, but the logical length need not be a
                                   multiple of 4, the controller counts the valid pixels and each transfer starts
                                   with a clean FIFO */
} display_io_caps_t;

/**
 * @brief Endpoint information for users.
 */
typedef struct {
    display_io_caps_t caps;      /**< Capabilities */
    display_format_t format;     /**< Memory pixel format */
    display_wire_t wire;         /**< Effective wire width */
    display_iface_t iface;       /**< Interface */
    display_cmd_mode_t cmd_mode; /**< Command encoding */
} display_io_info_t;

/**
 * @brief Endpoint object.
 *
 * @ref bus and @ref cfg are set statically (DISPLAY_IO); the other fields are runtime state.
 */
struct display_io {
    display_bus_t *bus;                /**< Bus this endpoint belongs to (static) */
    display_io_cfg_t cfg;              /**< Settings (static) */
    const display_link_t *link;        /**< Settings of the panel IC on this interface; NULL: defaults */
    const display_video_mode_t *video; /**< Video link of the panel, for a bus that carries video too (MIPI DSI); NULL: none */
    display_io_caps_t caps;            /**< Capabilities */
    display_wire_t wire;               /**< Effective wire width (AUTO resolved) */
    bool attached;                     /**< Attached to the bus */
};

/**
 * @brief Static definition of an endpoint, the same for every bus backend.
 *
 * @code
 * static display_io_t io = DISPLAY_IO(&dbi0, .cs = DISPLAY_PIN_LOW(5), .iface = DISPLAY_IFACE_DBI_B8,
 *                                     .format = DISPLAY_FMT_RGB565, .freq_hz = 27000000);
 * @endcode
 */
#define DISPLAY_IO(_bus, ...)                        \
    {                                                \
        .bus = &(_bus)->base, .cfg = { __VA_ARGS__ } \
    }

/**
 * @brief Attach an endpoint to its bus.
 *
 * The bus controller is initialized when its first endpoint is attached and released when the last
 * one is detached. Usually called by display_panel_init(); call it yourself when using IO directly.
 * Configures the CS pin as output at its inactive level. Calling it again is a no-op.
 *
 * @param [in] io endpoint
 * @retval 0 success
 * @retval -EINVAL invalid configuration
 * @retval -ENOTSUP the bus cannot drive the interface, or send the configured format or wire width
 * @retval -EBUSY the endpoint cannot share the bus (backend rule, e.g. an endpoint without CS)
 * @retval -ENODEV controller or DMA device not found
 */
int display_io_init(display_io_t *io);

/**
 * @brief Detach an endpoint from its bus, after the in-flight transfer has finished.
 *
 * @param [in] io endpoint
 * @retval 0 success
 * @retval -ETIMEDOUT the bus stayed busy
 */
int display_io_deinit(display_io_t *io);

/**
 * @brief Load the settings of the panel IC on the endpoint's interface. Normally done by the panel
 *        driver right after display_io_init(); call it yourself when using IO directly with a
 *        panel that needs a non-default command encoding or QSPI protocol.
 *
 * The link must stay valid while the endpoint is attached. The controller is reconfigured on the next
 * transfer.
 *
 * @param [in] io   attached endpoint
 * @param [in] link settings, @ref display_link_t::iface must be the interface of the endpoint
 * @retval 0 success
 * @retval -EINVAL not attached, the link is for another interface, or a QSPI link without protocol (opcode 0)
 * @retval -EPERM called from an ISR
 * @retval -ETIMEDOUT the bus stayed busy
 */
int display_io_set_link(display_io_t *io, const display_link_t *link);

/**
 * @brief Tell the bus which video link the panel behind the endpoint receives. Normally done by the panel
 *        driver before the first command; a bus that also carries video (MIPI DSI) needs it to set up the
 *        link before the panel can be initialized.
 *
 * The video link must stay valid while the endpoint is attached. The controller is reconfigured on the next
 * transfer.
 *
 * @param [in] io    attached endpoint
 * @param [in] video video link of the panel, NULL to clear
 * @retval 0 success
 * @retval -EINVAL not attached
 * @retval -EPERM called from an ISR
 * @retval -ETIMEDOUT the bus stayed busy
 */
int display_io_set_video(display_io_t *io, const display_video_mode_t *video);

/**
 * @brief Get the clock of an endpoint before the backend limits it to what the controller can do:
 *        the endpoint setting, else the panel IC setting, else the interface default.
 *
 * @param [in] io endpoint
 * @return clock in Hz
 */
uint32_t display_io_get_freq(const display_io_t *io);

/**
 * @brief Get the command encoding of an endpoint (standard DCS without a link).
 *
 * @param [in] io endpoint
 * @return command encoding
 */
display_cmd_mode_t display_io_get_cmd_mode(const display_io_t *io);

/**
 * @brief Get endpoint information.
 *
 * @param [in]  io   attached endpoint
 * @param [out] info information
 * @retval 0 success
 * @retval -EINVAL not attached
 */
int display_io_get_info(display_io_t *io, display_io_info_t *info);

/**
 * @brief Execute messages atomically on the bus. Task context only.
 *
 * No other endpoint can use the bus between the messages. Each message is one CS transaction
 * unless DISPLAY_IO_MSG_CS_KEEP is set. When the last message carries pixels and the endpoint has
 * DISPLAY_IO_CAP_DMA, the pixels are sent by DMA and the calling task sleeps until the transfer is
 * done, leaving the CPU to other tasks. An error reported by the completion interrupt is counted in
 * the bus statistics but not returned (completion interrupts are considered reliable).
 *
 * @param [in] io    attached endpoint
 * @param [in] msgs  messages
 * @param [in] count number of messages
 * @retval 0 success, the wire is idle and the buffers can be reused
 * @retval -EINVAL invalid message
 * @retval -ENOTSUP read or pixels not supported by this endpoint
 * @retval -E2BIG message too long
 * @retval -EPIPE endpoint not attached
 * @retval -EPERM called from an ISR
 * @retval -ETIMEDOUT bus busy
 * @retval -EIO hardware error
 */
int display_io_xfer(display_io_t *io, const display_io_msg_t *msgs, size_t count);

/**
 * @brief Execute messages whose last one is an asynchronous pixel message.
 *
 * All messages before the last one are sent before returning, so @p msgs may live on the stack.
 * The pixel buffer must stay valid until @p cb. Returning 0 means @p cb is called exactly once;
 * a negative return means it is never called. Without DISPLAY_IO_CAP_DMA the request is completed
 * synchronously and @p cb is called from the calling task before returning.
 *
 * @param [in] io    attached endpoint
 * @param [in] msgs  messages, the last one with DISPLAY_IO_MSG_COLOR
 * @param [in] count number of messages
 * @param [in] cb    completion callback, must not be NULL
 * @param [in] ctx   callback context
 * @retval -EINVAL @p cb is NULL, or the last message is not an immediate pixel message
 * @return otherwise the same as display_io_xfer()
 */
int display_io_xfer_async(display_io_t *io, const display_io_msg_t *msgs, size_t count, display_done_cb_t cb,
                          void *ctx);

/**
 * @brief Write a command with parameters.
 *
 * @param [in] io    attached endpoint
 * @param [in] cmd   command
 * @param [in] param parameters, may be NULL when @p len is 0
 * @param [in] len   number of parameter bytes
 * @return same as display_io_xfer()
 */
int display_io_write(display_io_t *io, uint32_t cmd, const void *param, size_t len);

/**
 * @brief Write a command and read the response.
 *
 * @param [in]  io  attached endpoint with DISPLAY_IO_CAP_READ
 * @param [in]  cmd command
 * @param [out] buf response
 * @param [in]  len number of bytes to read
 * @return same as display_io_xfer()
 */
int display_io_read(display_io_t *io, uint32_t cmd, void *buf, size_t len);

/**
 * @brief Write a command followed by pixels, synchronously.
 *
 * @param [in] io     attached endpoint
 * @param [in] cmd    command, usually 0x2C
 * @param [in] pixels pixels in the endpoint format
 * @param [in] len    number of bytes, a multiple of the pixel size
 * @return same as display_io_xfer()
 */
int display_io_write_color(display_io_t *io, uint32_t cmd, const void *pixels, size_t len);

/**
 * @brief Write a command followed by pixels, asynchronously.
 *
 * @param [in] io     attached endpoint
 * @param [in] cmd    command, usually 0x2C
 * @param [in] pixels pixels in the endpoint format, valid until @p cb
 * @param [in] len    number of bytes, a multiple of the pixel size
 * @param [in] cb     completion callback
 * @param [in] ctx    callback context
 * @return same as display_io_xfer_async()
 */
int display_io_write_color_async(display_io_t *io, uint32_t cmd, const void *pixels, size_t len, display_done_cb_t cb,
                                 void *ctx);

/** @} */ /* DISPLAY_IO */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_IO_H */
