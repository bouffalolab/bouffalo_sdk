/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 * Implements the contracts in Dhara's nand.h.
 */
#include <stddef.h>
#include <string.h>
#include "bflb_nand_internal.h"

static struct nand_ftl *owner(const struct dhara_nand *geometry)
{
    return (struct nand_ftl *)((char *)geometry - offsetof(struct nand_ftl, geometry));
}

static int callback_result(int result, dhara_error_t *error)
{
    if (!result) {
        return 0;
    }

    dhara_error_t reason = DHARA_E_CORRUPT_MAP;

    if (result == BFLB_NAND_ERR_BAD_BLOCK) {
        reason = DHARA_E_BAD_BLOCK;
    }
    if (result == BFLB_NAND_ERR_ECC) {
        reason = DHARA_E_ECC;
    }

    dhara_set_error(error, reason);
    return -1;
}

int dhara_nand_is_bad(const struct dhara_nand *geometry, dhara_block_t block)
{
    struct nand_ftl *f = owner(geometry);

    if (block >= geometry->num_blocks) {
        return 1;
    }
    if (f->bad_blocks[block / 8] & (1U << (block % 8))) {
        return 1;
    }

    uint32_t page = block << geometry->log2_ppb;
    uint8_t markers[2];
    const struct nand_chip *chip = f->phy.chip;

    /* Factory markers use the first spare bytes on one or two pages,
     * according to the device. Main-array byte zero is normal user data.
     */
    for (uint8_t i = 0; i < chip->bad_marker_pages; i++) {
        int result =
            nand_phy_read(&f->phy, page + i, 1U << geometry->log2_page_size, markers, chip->bad_marker_bytes, false);

        if (result) {
            /* The sticky I/O error is checked by the caller. */
            return 1;
        }

        for (uint8_t j = 0; j < chip->bad_marker_bytes; j++) {
            if (markers[j] != 0xff) {
                f->bad_blocks[block / 8] |= 1U << (block % 8);
                return 1;
            }
        }
    }

    return 0;
}

void dhara_nand_mark_bad(const struct dhara_nand *geometry, dhara_block_t block)
{
    struct nand_ftl *f = owner(geometry);

    if (block >= geometry->num_blocks) {
        return;
    }

    f->bad_blocks[block / 8] |= 1U << (block % 8);
    (void)nand_phy_retire(&f->phy, block);
}

int dhara_nand_erase(const struct dhara_nand *geometry, dhara_block_t block, dhara_error_t *error)
{
    return callback_result(nand_phy_erase(&owner(geometry)->phy, block), error);
}

int dhara_nand_prog(const struct dhara_nand *geometry, dhara_page_t page, const uint8_t *data, dhara_error_t *error)
{
    return callback_result(nand_phy_program(&owner(geometry)->phy, page, data), error);
}

int dhara_nand_read(const struct dhara_nand *geometry, dhara_page_t page, size_t offset, size_t length, uint8_t *data,
                    dhara_error_t *error)
{
    size_t bytes = 1U << geometry->log2_page_size;

    if (offset > bytes || length > bytes - offset) {
        return callback_result(BFLB_NAND_ERR_INVALID_ARG, error);
    }

    return callback_result(nand_phy_read(&owner(geometry)->phy, page, offset, data, length, true), error);
}

int dhara_nand_is_free(const struct dhara_nand *geometry, dhara_page_t page)
{
    struct nand_ftl *f = owner(geometry);
    uint32_t bytes = 1U << geometry->log2_page_size;
    uint8_t marker[4];

    if (nand_phy_read(&f->phy, page, bytes, marker, sizeof(marker), false)) {
        return 0;
    }
    for (unsigned i = 0; i < sizeof(marker); ++i) {
        if (marker[i] != 0xff) {
            return 0;
        }
    }

    /* Also reject a torn page whose used marker was not programmed. */
    if (nand_phy_read(&f->phy, page, 0, f->copy_buffer, bytes, false)) {
        return 0;
    }
    for (uint32_t i = 0; i < bytes; ++i) {
        if (f->copy_buffer[i] != 0xff) {
            return 0;
        }
    }

    return 1;
}

int dhara_nand_copy(const struct dhara_nand *geometry, dhara_page_t src, dhara_page_t dst, dhara_error_t *error)
{
    struct nand_ftl *f = owner(geometry);
    int result = nand_phy_read(&f->phy, src, 0, f->copy_buffer, 1U << geometry->log2_page_size, true);

    if (!result) {
        result = nand_phy_program(&f->phy, dst, f->copy_buffer);
    }

    return callback_result(result, error);
}

static int map_result(struct nand_ftl *f, int result, dhara_error_t error)
{
    if (f->phy.io_error) {
        return f->phy.io_error;
    }
    if (!result) {
        return BFLB_NAND_OK;
    }
    if (error == DHARA_E_ECC) {
        return BFLB_NAND_ERR_ECC;
    }
    if (error == DHARA_E_MAP_FULL || error == DHARA_E_JOURNAL_FULL) {
        return BFLB_NAND_ERR_FULL;
    }
    if (error == DHARA_E_BAD_BLOCK || error == DHARA_E_TOO_BAD) {
        return BFLB_NAND_ERR_BAD_BLOCK;
    }

    return BFLB_NAND_ERR_RECOVERY;
}

void nand_ftl_close(struct nand_ftl *f)
{
    free(f->journal_buffer);
    free(f->copy_buffer);
    free(f->bad_blocks);
    f->journal_buffer = f->copy_buffer = f->bad_blocks = NULL;
}

int nand_ftl_open(struct nand_ftl *f, const bflb_spi_nand_bus_t *bus, uint8_t ratio)
{
    f->phy.bus = *bus;
    int result = nand_phy_open(&f->phy);

    if (result) {
        return result;
    }

    const struct nand_chip *chip = f->phy.chip;

    f->geometry = (struct dhara_nand){
        .log2_page_size = chip->page_shift,
        .log2_ppb = chip->block_shift,
        .num_blocks = chip->blocks,
    };
    f->journal_buffer = malloc(1U << chip->page_shift);
    f->copy_buffer = malloc(1U << chip->page_shift);
    f->bad_blocks = calloc((chip->blocks + 7U) / 8U, 1);
    if (!f->journal_buffer || !f->copy_buffer || !f->bad_blocks) {
        return BFLB_NAND_ERR_NO_MEM;
    }

    dhara_map_init(&f->map, &f->geometry, f->journal_buffer, ratio ? ratio : 45);
    dhara_error_t error = DHARA_E_NONE;

    if (!dhara_map_resume(&f->map, &error)) {
        return map_result(f, 0, error);
    }
    if (f->phy.io_error) {
        return f->phy.io_error;
    }

    /* Dhara initializes an empty map on failed recovery. Accept that state
     * only after confirming every usable page is blank. Existing damaged
     * data must never silently become a writable empty device.
     * This full scan is deliberately limited to the no-checkpoint path.
     */
    for (uint32_t block = 0; block < chip->blocks; ++block) {
        if (!dhara_nand_is_bad(&f->geometry, block)) {
            uint32_t first = block << chip->block_shift;
            for (uint32_t i = 0; i < (1U << chip->block_shift); ++i) {
                if (!dhara_nand_is_free(&f->geometry, first + i)) {
                    return f->phy.io_error ? f->phy.io_error : BFLB_NAND_ERR_RECOVERY;
                }
            }
        }
        if (f->phy.io_error) {
            return f->phy.io_error;
        }
    }

    return BFLB_NAND_OK;
}

int nand_ftl_read(struct nand_ftl *f, uint32_t page, uint8_t *data)
{
    dhara_error_t error = DHARA_E_NONE;

    f->phy.io_error = 0;
    int result = dhara_map_read(&f->map, page, data, &error);
    return map_result(f, result, error);
}

int nand_ftl_write(struct nand_ftl *f, uint32_t page, const uint8_t *data)
{
    dhara_error_t error = DHARA_E_NONE;

    f->phy.io_error = 0;
    int result = dhara_map_write(&f->map, page, data, &error);
    return map_result(f, result, error);
}

int nand_ftl_sync(struct nand_ftl *f)
{
    dhara_error_t error = DHARA_E_NONE;

    f->phy.io_error = 0;
    int result = dhara_map_sync(&f->map, &error);
    return map_result(f, result, error);
}
