/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "bflb_nand_internal.h"

struct bflb_spi_nand {
    struct nand_ftl ftl;
    nand_mutex_t mutex;
    bflb_spi_nand_info_t info;
    uint8_t *sector_page;
};

int bflb_spi_nand_init(const bflb_spi_nand_bus_t *bus, uint8_t gc_factor, bflb_spi_nand_t **out)
{
    if (!out) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }
    *out = NULL;

    if (!bus || !bus->transfer || !bus->chip_select || (!!bus->lock != !!bus->unlock)) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    bflb_spi_nand_t *n = calloc(1, sizeof(*n));

    if (!n) {
        return BFLB_NAND_ERR_NO_MEM;
    }

    int result = nand_ftl_open(&n->ftl, bus, gc_factor);

    if (result) {
        goto cleanup;
    }

    const struct nand_chip *chip = n->ftl.phy.chip;

    n->info.manufacturer_id = chip->manufacturer;
    n->info.device_id = chip->device;
    n->info.page_size = 1U << chip->page_shift;
    n->info.block_size = n->info.page_size << chip->block_shift;
    n->info.physical_blocks = chip->blocks;

    uint32_t pages = dhara_map_capacity(&n->ftl.map);
    uint32_t sectors_per_page = n->info.page_size / BFLB_SPI_NAND_SECTOR_SIZE;

    if (!pages || pages > UINT32_MAX / sectors_per_page) {
        result = BFLB_NAND_ERR_INVALID_SIZE;
        goto cleanup;
    }
    n->info.sector_count = pages * sectors_per_page;

    n->sector_page = malloc(n->info.page_size);
    n->mutex = nand_mutex_create();
    if (!n->sector_page || !n->mutex) {
        result = BFLB_NAND_ERR_NO_MEM;
        goto cleanup;
    }

    *out = n;
    return BFLB_NAND_OK;

cleanup:
    if (n->mutex) {
        nand_mutex_destroy(n->mutex);
    }
    free(n->sector_page);
    nand_ftl_close(&n->ftl);
    free(n);
    return result;
}

int bflb_spi_nand_get_info(bflb_spi_nand_t *n, bflb_spi_nand_info_t *info)
{
    if (!n || !info) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    *info = n->info;
    return BFLB_NAND_OK;
}

static int transfer_sectors(bflb_spi_nand_t *n, uint32_t first, uint32_t count, uint8_t *destination,
                            const uint8_t *source)
{
    if (!n || (!destination && !source) || !count || first >= n->info.sector_count ||
        count > n->info.sector_count - first) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

#if SIZE_MAX < UINT64_MAX
    if (count > SIZE_MAX / BFLB_SPI_NAND_SECTOR_SIZE) {
        return BFLB_NAND_ERR_INVALID_SIZE;
    }
#endif

    uint32_t sectors_per_page = n->info.page_size / BFLB_SPI_NAND_SECTOR_SIZE;
    int result = BFLB_NAND_OK;

    nand_mutex_lock(n->mutex);
    while (count) {
        uint32_t logical_page = first / sectors_per_page;
        uint32_t offset = first % sectors_per_page;
        uint32_t portion = sectors_per_page - offset;

        if (portion > count) {
            portion = count;
        }

        size_t bytes = portion * BFLB_SPI_NAND_SECTOR_SIZE;
        size_t position = offset * BFLB_SPI_NAND_SECTOR_SIZE;

        if (destination || portion != sectors_per_page) {
            result = nand_ftl_read(&n->ftl, logical_page, n->sector_page);
            if (result) {
                break;
            }
        }

        if (source) {
            memcpy(n->sector_page + position, source, bytes);
            result = nand_ftl_write(&n->ftl, logical_page, n->sector_page);
            if (result) {
                break;
            }
            source += bytes;
        } else {
            memcpy(destination, n->sector_page + position, bytes);
            destination += bytes;
        }

        first += portion;
        count -= portion;
    }
    nand_mutex_unlock(n->mutex);

    return result;
}

int bflb_spi_nand_read(bflb_spi_nand_t *n, void *buffer, uint32_t sector, uint32_t count)
{
    return transfer_sectors(n, sector, count, buffer, NULL);
}

int bflb_spi_nand_write(bflb_spi_nand_t *n, const void *buffer, uint32_t sector, uint32_t count)
{
    return transfer_sectors(n, sector, count, NULL, buffer);
}

int bflb_spi_nand_sync(bflb_spi_nand_t *n)
{
    if (!n) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    nand_mutex_lock(n->mutex);
    int result = nand_ftl_sync(&n->ftl);
    nand_mutex_unlock(n->mutex);

    return result;
}

int bflb_spi_nand_deinit(bflb_spi_nand_t *n)
{
    if (!n) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }

    int result = bflb_spi_nand_sync(n);

    if (result) {
        return result;
    }

    nand_ftl_close(&n->ftl);
    nand_mutex_destroy(n->mutex);
    free(n->sector_page);
    free(n);
    return BFLB_NAND_OK;
}
