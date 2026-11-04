/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef FATFS_SPI_NAND_H
#define FATFS_SPI_NAND_H

#include "bflb_spi_nand.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Register an initialized NAND as DEV_FLASH (/flash). No formatting.
 * Do not replace a live drive; stop users and unmount before unregistering.
 * The caller owns the NAND handle and releases it after unregister succeeds.
 */
int fatfs_spi_nand_driver_register(bflb_spi_nand_t *nand);
int fatfs_spi_nand_driver_unregister(void);

#ifdef __cplusplus
}
#endif

#endif
