/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BFLB_NAND_INTERNAL_H
#define BFLB_NAND_INTERNAL_H

#include "bflb_spi_nand.h"
#include "bflb_nand_os.h"
#include "dhara/map.h"

struct nand_chip {
    uint8_t manufacturer;
    uint16_t device;
    uint8_t page_shift;
    uint8_t block_shift;
    uint16_t blocks;
    uint16_t spare_bytes;
    uint8_t configuration_mask;
    uint8_t configuration_value;
    uint8_t protection_mask;
    uint8_t ecc_error_mask;
    uint8_t ecc_error_value;
    uint8_t bad_marker_pages;
    uint8_t bad_marker_bytes;
    uint32_t read_timeout_us;
    uint32_t program_timeout_us;
    uint32_t erase_timeout_us;
};

struct nand_phy {
    bflb_spi_nand_bus_t bus;
    const struct nand_chip *chip;
    int io_error; /* Transport/timeout failures must not retire healthy media. */
};

struct nand_ftl {
    struct dhara_nand geometry;
    struct dhara_map map;
    struct nand_phy phy;
    uint8_t *journal_buffer;
    uint8_t *copy_buffer;
    uint8_t *bad_blocks;
};

extern const struct nand_chip nand_w25n01kv;
extern const struct nand_chip nand_f35sqa512m;

int nand_bus_command(struct nand_phy *phy, uint8_t opcode, uint32_t address, unsigned address_bytes,
                     unsigned dummy_bytes, const void *tx, void *rx, size_t length);
int nand_phy_open(struct nand_phy *phy);
int nand_phy_read(struct nand_phy *phy, uint32_t page, uint16_t column, void *data, size_t length, bool check_ecc);
int nand_phy_program(struct nand_phy *phy, uint32_t page, const uint8_t *data);
int nand_phy_erase(struct nand_phy *phy, uint32_t block);
int nand_phy_retire(struct nand_phy *phy, uint32_t block);
int nand_ftl_open(struct nand_ftl *ftl, const bflb_spi_nand_bus_t *bus, uint8_t ratio);
void nand_ftl_close(struct nand_ftl *ftl);
int nand_ftl_read(struct nand_ftl *ftl, uint32_t page, uint8_t *data);
int nand_ftl_write(struct nand_ftl *ftl, uint32_t page, const uint8_t *data);
int nand_ftl_sync(struct nand_ftl *ftl);

#endif
