/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BFLB_SPI_NAND_LHAL_H
#define BFLB_SPI_NAND_LHAL_H

#include "bflb_spi.h"
#include "bflb_spi_nand.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BFLB_SPI_NAND_LHAL_PIN_NONE 0xFFU

typedef struct {
    struct bflb_device_s *spi;
    struct bflb_device_s *gpio;
    uint8_t cs_pin;
    uint8_t wp_pin;
    uint8_t rb_pin;
    bool cs_active_low;
} bflb_spi_nand_lhal_t;

int bflb_spi_nand_lhal_init(bflb_spi_nand_lhal_t *port, const char *spi_name, uint8_t cs_pin,
                            const struct bflb_spi_config_s *spi_config, bool cs_active_low, bflb_spi_nand_bus_t *bus);
int bflb_spi_nand_lhal_init_ex(bflb_spi_nand_lhal_t *port, const char *spi_name, uint8_t cs_pin, uint8_t wp_pin,
                               uint8_t rb_pin, const struct bflb_spi_config_s *spi_config, bool cs_active_low,
                               bflb_spi_nand_bus_t *bus);

#ifdef __cplusplus
}
#endif

#endif
