/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BFLB_SPI_NAND_H
#define BFLB_SPI_NAND_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BFLB_SPI_NAND_SECTOR_SIZE 512U

typedef enum {
    BFLB_NAND_OK = 0,
    BFLB_NAND_ERR_INVALID_ARG = -1,
    BFLB_NAND_ERR_NO_MEM = -2,
    BFLB_NAND_ERR_IO = -3,
    BFLB_NAND_ERR_TIMEOUT = -4,
    BFLB_NAND_ERR_NOT_SUPPORTED = -5,
    BFLB_NAND_ERR_INVALID_STATE = -6,
    BFLB_NAND_ERR_ECC = -7,
    BFLB_NAND_ERR_BAD_BLOCK = -8,
    BFLB_NAND_ERR_FULL = -9,
    BFLB_NAND_ERR_RECOVERY = -10,
    BFLB_NAND_ERR_INVALID_SIZE = -11,
    BFLB_NAND_ERR_INVALID_RESPONSE = -12
} bflb_spi_nand_error_t;

typedef struct {
    /* Return zero on success. CS stays asserted across header/data exchanges. */
    int (*transfer)(void *ctx, const void *tx, void *rx, size_t length);
    void (*chip_select)(void *ctx, bool asserted);
    /* Optional pair; serialize this SPI controller with other bus clients. */
    void (*lock)(void *ctx);
    void (*unlock)(void *ctx);
    void *ctx;
} bflb_spi_nand_bus_t;

typedef struct bflb_spi_nand bflb_spi_nand_t;

typedef struct {
    uint8_t manufacturer_id;
    uint16_t device_id;
    uint32_t page_size;
    uint32_t block_size;
    uint32_t physical_blocks;
    uint32_t sector_count; /* Usable 512-byte logical sectors. */
} bflb_spi_nand_info_t;

/* Probe a supported SPI-NAND for SIO, Dhara mapping and 512-byte sectors.
 * gc_factor=0 selects 45; keep the same value when reopening existing media.
 * Init never formats a filesystem. Lifecycle calls require exclusive access.
 * With FreeRTOS, call after scheduler start. Bare metal: single caller, no ISR I/O.
 */
int bflb_spi_nand_init(const bflb_spi_nand_bus_t *bus, uint8_t gc_factor, bflb_spi_nand_t **out);

/* Sync before freeing; if sync fails retain the live handle for retry. */
int bflb_spi_nand_deinit(bflb_spi_nand_t *nand);
int bflb_spi_nand_get_info(bflb_spi_nand_t *nand, bflb_spi_nand_info_t *info);
int bflb_spi_nand_read(bflb_spi_nand_t *nand, void *buffer, uint32_t sector, uint32_t count);
int bflb_spi_nand_write(bflb_spi_nand_t *nand, const void *buffer, uint32_t sector, uint32_t count);
int bflb_spi_nand_sync(bflb_spi_nand_t *nand);

#ifdef __cplusplus
}
#endif

#endif
