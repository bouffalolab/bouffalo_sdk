/**
 * @file display_spi.h
 * @brief Hardware SPI bus backend, BL616 / BL616CL / BL618DG.
 *
 * Implements the interface DISPLAY_IFACE_DBI_C4 (MIPI DBI Type C 4-wire: SCK + MOSI + GPIO DC), the same
 * interface the DBI peripheral offers, with fewer pixel formats. No other interface.
 *
 * Pixel formats (variable FIFO width):
 * - RGB565:   16-bit frames, high byte first, 16 bit on the wire;
 * - XRGB8888: 24-bit frames, R, G, B of every 32-bit word, 18/24 bit on the wire;
 * - packed RGB888 is not supported (3-byte pixels cannot be fetched as 32-bit words).
 *
 * When the bus switches to an endpoint, the backend calls SPI_CMD_SET_FREQ; during a transfer it switches
 * SPI_CMD_SET_DATA_WIDTH (8 bit for commands, 16/24 bit for pixels). The SPI mode is private to this
 * backend: CONFIG_DISPLAY_SPI_MODE. Board code muxes SCK/MOSI; the controller SS pin is not muxed, CS is a
 * GPIO driven by the bus core. DC is shared by all panels of the bus. The board functions in the bus
 * configuration mux and release the SCK / MOSI pins.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_SPI_H
#define DISPLAY_SPI_H

#include "bus/display_bus_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_BUS_SPI SPI bus backend
 * @{
 */

struct bflb_device_s;
struct bflb_dma_channel_lli_pool_s;

/**
 * @brief SPI bus configuration.
 */
typedef struct {
    const char *dev;                         /**< Controller device name, NULL: "spi0" */
    const char *dma;                         /**< DMA channel, e.g. "dma0_ch0"; NULL: synchronous only */
    struct bflb_dma_channel_lli_pool_s *lli; /**< DMA LLI pool, sized for the largest transfer */
    uint16_t lli_count;                      /**< Number of LLI nodes */
    display_pin_t dc;                        /**< Required; "active" means data phase, usually DISPLAY_PIN_HIGH(n) */
    void (*func_init)(void);                 /**< Board: mux the SCK / MOSI pins, when the first endpoint is attached.
                                                  Optional (board_display_spi_gpio_init) */
    void (*func_deinit)(void);               /**< Board: release the pins, when the last endpoint leaves. Optional */
} display_spi_bus_cfg_t;

/**
 * @brief SPI bus object.
 */
typedef struct {
    display_bus_t base;        /**< Bus core, must be first */
    display_spi_bus_cfg_t cfg; /**< Configuration */
    struct bflb_device_s *spi; /**< Runtime: controller */
    struct bflb_device_s *dma; /**< Runtime: DMA channel, NULL without DMA */
    uint32_t tx_fifo_addr;     /**< Runtime: TX FIFO address for DMA */
    uint8_t dma_width;         /**< Runtime: DMA data width currently programmed, 0xFF: none */
    bool exclusive;            /**< Runtime: an endpoint without CS owns the bus */
} display_spi_bus_t;

/** @brief Backend operations. */
extern const display_bus_ops_t display_spi_bus_ops;

/**
 * @brief Static definition of an SPI bus, see @ref display_spi_bus_cfg_t. Endpoints use DISPLAY_IO()
 *        with `.iface = DISPLAY_IFACE_DBI_C4`.
 */
#define DISPLAY_SPI_BUS(...)                                                   \
    {                                                                          \
        .base = DISPLAY_BUS_BASE(&display_spi_bus_ops), .cfg = { __VA_ARGS__ } \
    }

/** @} */ /* DISPLAY_BUS_SPI */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_SPI_H */
