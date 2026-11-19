/**
 * @file display_dbi.h
 * @brief DBI peripheral bus backend (BL616 / BL616CL / BL618DG). One peripheral can drive several panels,
 *        even on different interface types.
 *
 * Interfaces (MIPI DBI naming): DISPLAY_IFACE_DBI_B8 (Type B, 8080 x8), DISPLAY_IFACE_DBI_C3 (Type C
 * 3-wire), DISPLAY_IFACE_DBI_C4 (Type C 4-wire) and, where DBI_QSPI_SUPPORT is set, DISPLAY_IFACE_QSPI
 * (vendor extension).
 *
 * The bus declares the interfaces it supports in its configuration (@ref display_dbi_iface_t). Each one
 * names two functions provided by the board, which mux the pins for that interface and release them.
 * The framework does not know the pins. The functions run when the peripheral switches to an endpoint of
 * another interface: the old interface's func_deinit(), then the new one's func_init(), then the
 * controller is reprogrammed. Pins may overlap between interfaces, except the software CS pins, which
 * belong to the framework and must not be touched. A single-interface bus may leave both functions NULL
 * when the board has muxed the pins already.
 *
 * Pixel formats (input = memory format, output = wire width, converted by the DBI hardware):
 * - input:  RGB565, RGB888, XRGB8888 (and YUYV where DBI_YUYV_SUPPORT is set);
 * - output: 16 bit wire -> RGB565, 24 bit wire -> RGB888 (RGB666 panels take it as well).
 *
 * Clock polarity/phase of the serial interfaces is private to this backend: CONFIG_DISPLAY_DBI_CLK_MODE.
 * Pixels go by DMA, which completes on the DBI TC interrupt (the last pixel has left the pins).
 *
 * QSPI: a transfer is [opcode, 1 wire] [24-bit address] [data]. Opcode and address come from the QSPI
 * protocol of the panel IC (@ref display_link_t): opcode in bits 31..24 of the code, address template in
 * bits 23..0, the DCS command is put into address bits 15..8. Parameters and pixels have separate codes
 * and wire layouts, switched with bflb_dbi_qspi_set_wire() between messages.
 *
 * CS:
 * - several panels: software CS (`.cs = DISPLAY_PIN_LOW(n)`), the DBI CS pin is not muxed to DBI;
 * - one panel: the CS pin may be muxed to DBI and driven by hardware (`.cs = DISPLAY_PIN_NONE`); that
 *   endpoint then owns the bus, no other endpoint can join.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_DBI_H
#define DISPLAY_DBI_H

#include "bus/display_bus_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_BUS_DBI DBI peripheral bus backend
 * @{
 */

struct bflb_device_s;
struct bflb_dma_channel_lli_pool_s;

/**
 * @brief One interface the DBI bus is set up for, with the board functions that mux its pins.
 */
typedef struct {
    display_iface_t iface;     /**< Interface; DISPLAY_IFACE_NONE ends the list */
    void (*func_init)(void);   /**< Board: set the pins to the function of this interface. Optional */
    void (*func_deinit)(void); /**< Board: release the pins. Optional */
} display_dbi_iface_t;

/**
 * @brief DBI bus configuration.
 */
typedef struct {
    const char *dev;                         /**< Controller device name, NULL: "dbi" */
    const char *dma;                         /**< DMA channel, e.g. "dma0_ch3"; NULL: CPU only */
    struct bflb_dma_channel_lli_pool_s *lli; /**< DMA LLI pool, about 16 KB per node */
    uint16_t lli_count;                      /**< Number of LLI nodes, sized for the largest transfer */
    const display_dbi_iface_t *ifaces;       /**< Interfaces of this run, see DISPLAY_DBI_IFACES() */
} display_dbi_bus_cfg_t;

/**
 * @brief DBI bus object.
 */
typedef struct {
    display_bus_t base;        /**< Bus core, must be first */
    display_dbi_bus_cfg_t cfg; /**< Configuration */
    struct bflb_device_s *dbi; /**< Runtime: controller */
    struct bflb_device_s *dma; /**< Runtime: DMA channel, NULL without DMA */
    uint8_t fifo_threshold;    /**< Runtime: TX FIFO threshold matching the DMA burst */
    uint8_t iface;             /**< Runtime: interface the pins and the controller are set for (display_iface_t) */
    uint8_t qspi_lines;        /**< Runtime: QSPI wire layout programmed now, 0xFF: unknown */
    bool exclusive;            /**< Runtime: an endpoint without CS owns the bus */
} display_dbi_bus_t;

/** @brief Backend operations. */
extern const display_bus_ops_t display_dbi_bus_ops;

/**
 * @brief List of the interfaces of a DBI bus, with the board pin functions of each.
 *
 * @code
 * .ifaces = DISPLAY_DBI_IFACES({ DISPLAY_IFACE_DBI_B8, board_dbi_b8_init, board_dbi_b8_deinit },
 *                              { DISPLAY_IFACE_DBI_C4, board_dbi_c4_init, board_dbi_c4_deinit })
 * @endcode
 *
 * Without board functions, write only the interface, as a designated initializer (positional ones
 * with missing fields trigger -Wmissing-field-initializers): `{ .iface = DISPLAY_IFACE_DBI_B8 }`.
 */
#define DISPLAY_DBI_IFACES(...) ((const display_dbi_iface_t[]){ __VA_ARGS__, { DISPLAY_IFACE_NONE, NULL, NULL } })

/**
 * @brief Static definition of a DBI bus.
 *
 * @code
 * static struct bflb_dma_channel_lli_pool_s dbi_lli[20];
 * static display_dbi_bus_t dbi0 = DISPLAY_DBI_BUS(.dma = "dma0_ch3", .lli = dbi_lli, .lli_count = 20,
 *                                                 .ifaces = DISPLAY_DBI_IFACES({ .iface = DISPLAY_IFACE_DBI_B8 }));
 * static display_io_t io = DISPLAY_IO(&dbi0, .cs = DISPLAY_PIN_LOW(5), .iface = DISPLAY_IFACE_DBI_B8,
 *                                     .format = DISPLAY_FMT_RGB565);
 * @endcode
 */
#define DISPLAY_DBI_BUS(...)                                                   \
    {                                                                          \
        .base = DISPLAY_BUS_BASE(&display_dbi_bus_ops), .cfg = { __VA_ARGS__ } \
    }

/** @} */ /* DISPLAY_BUS_DBI */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_DBI_H */
