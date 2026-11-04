/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bflb_spi_nand_lhal.h"

#include "bflb_gpio.h"

static int lhal_transfer(void *ctx, const void *tx, void *rx, size_t len)
{
    bflb_spi_nand_lhal_t *port = (bflb_spi_nand_lhal_t *)ctx;

    return bflb_spi_poll_exchange(port->spi, tx, rx, len);
}

static void lhal_chip_select(void *ctx, bool asserted)
{
    bflb_spi_nand_lhal_t *port = (bflb_spi_nand_lhal_t *)ctx;
    bool active = port->cs_active_low ? asserted : !asserted;

    if (active) {
        bflb_gpio_reset(port->gpio, port->cs_pin);
    } else {
        bflb_gpio_set(port->gpio, port->cs_pin);
    }
}

int bflb_spi_nand_lhal_init_ex(bflb_spi_nand_lhal_t *port, const char *spi_name, uint8_t cs_pin, uint8_t wp_pin,
                               uint8_t rb_pin, const struct bflb_spi_config_s *spi_config, bool cs_active_low,
                               bflb_spi_nand_bus_t *bus)
{
    struct bflb_spi_config_s default_config = {
        .freq = 8U * 1000U * 1000U,
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE0,
        .data_width = SPI_DATA_WIDTH_8BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    if (port == NULL || bus == NULL || spi_name == NULL) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    port->spi = bflb_device_get_by_name(spi_name);
    port->gpio = bflb_device_get_by_name("gpio");
    if (port->spi == NULL || port->gpio == NULL) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    port->cs_pin = cs_pin;
    port->wp_pin = wp_pin;
    port->rb_pin = rb_pin;
    port->cs_active_low = cs_active_low;
    const struct bflb_spi_config_s *config = spi_config != NULL ? spi_config : &default_config;

    if (config->role != SPI_ROLE_MASTER || config->data_width != SPI_DATA_WIDTH_8BIT ||
        config->bit_order != SPI_BIT_MSB || (config->mode != SPI_MODE0 && config->mode != SPI_MODE3)) {
        return BFLB_NAND_ERR_NOT_SUPPORTED;
    }

    bflb_spi_init(port->spi, config);
    bflb_spi_feature_control(port->spi, SPI_CMD_SET_CS_DISABLE, 1);
    bflb_gpio_init(port->gpio, cs_pin, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    lhal_chip_select(port, false);
    if (wp_pin != BFLB_SPI_NAND_LHAL_PIN_NONE) {
        bflb_gpio_init(port->gpio, wp_pin, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_set(port->gpio, wp_pin);
    }
    if (rb_pin != BFLB_SPI_NAND_LHAL_PIN_NONE) {
        bflb_gpio_init(port->gpio, rb_pin, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN);
    }

    bus->transfer = lhal_transfer;
    bus->chip_select = lhal_chip_select;
    bus->ctx = port;
    bus->lock = NULL;
    bus->unlock = NULL;
    return BFLB_NAND_OK;
}

int bflb_spi_nand_lhal_init(bflb_spi_nand_lhal_t *port, const char *spi_name, uint8_t cs_pin,
                            const struct bflb_spi_config_s *spi_config, bool cs_active_low, bflb_spi_nand_bus_t *bus)
{
    return bflb_spi_nand_lhal_init_ex(port, spi_name, cs_pin, BFLB_SPI_NAND_LHAL_PIN_NONE, BFLB_SPI_NAND_LHAL_PIN_NONE,
                                      spi_config, cs_active_low, bus);
}
