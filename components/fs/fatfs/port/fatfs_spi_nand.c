/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 * SDK diskio callbacks for a 512-byte NAND block device.
 */
#include "diskio.h"
#include "fatfs_spi_nand.h"

#if FF_MIN_SS != 512 || FF_MAX_SS != 512
#error "SPI NAND requires FatFs sectors of 512 bytes"
#endif

static bflb_spi_nand_t *flash;

static int flash_status(void)
{
    return flash ? 0 : STA_NOINIT;
}

static int flash_access(BYTE *read_data, const BYTE *write_data, LBA_t start, UINT count)
{
    if (!flash) {
        return RES_NOTRDY;
    }
    if (!count || start > UINT32_MAX) {
        return RES_PARERR;
    }

    int result = write_data ? bflb_spi_nand_write(flash, write_data, start, count) :
                              bflb_spi_nand_read(flash, read_data, start, count);

    switch (result) {
        case BFLB_NAND_OK:
            return RES_OK;

        case BFLB_NAND_ERR_INVALID_ARG:
        case BFLB_NAND_ERR_INVALID_SIZE:
            return RES_PARERR;

        default:
            return RES_ERROR;
    }
}

static int flash_read(BYTE *data, LBA_t start, UINT count)
{
    return flash_access(data, NULL, start, count);
}

static int flash_write(const BYTE *data, LBA_t start, UINT count)
{
    if (!data) {
        return RES_PARERR;
    }

    return flash_access(NULL, data, start, count);
}

static int flash_control(BYTE request, void *output)
{
    if (!flash) {
        return RES_NOTRDY;
    }
    if (request == CTRL_SYNC) {
        return bflb_spi_nand_sync(flash) ? RES_ERROR : RES_OK;
    }
    if (!output) {
        return RES_PARERR;
    }

    bflb_spi_nand_info_t info;

    if (bflb_spi_nand_get_info(flash, &info)) {
        return RES_ERROR;
    }

    switch (request) {
        case GET_BLOCK_SIZE:
            *(DWORD *)output = info.block_size / BFLB_SPI_NAND_SECTOR_SIZE;
            return RES_OK;

        case GET_SECTOR_SIZE:
            *(WORD *)output = BFLB_SPI_NAND_SECTOR_SIZE;
            return RES_OK;

        case GET_SECTOR_COUNT:
            *(LBA_t *)output = info.sector_count;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}

int fatfs_spi_nand_driver_register(bflb_spi_nand_t *nand)
{
    if (!nand) {
        return BFLB_NAND_ERR_INVALID_ARG;
    }
    if (flash) {
        return BFLB_NAND_ERR_INVALID_STATE;
    }
    if (pDiskioDriver[DEV_FLASH].disk_read && pDiskioDriver[DEV_FLASH].disk_read != flash_read) {
        return BFLB_NAND_ERR_INVALID_STATE;
    }

    bflb_spi_nand_info_t info;
    int result = bflb_spi_nand_get_info(nand, &info);

    if (result) {
        return result;
    }
    if (!info.sector_count) {
        return BFLB_NAND_ERR_INVALID_SIZE;
    }

    FATFS_DiskioDriverTypeDef callbacks = {
        .disk_status = flash_status,
        .disk_initialize = flash_status,
        .disk_read = flash_read,
        .disk_write = flash_write,
        .disk_ioctl = flash_control,
    };

    flash = nand;
    disk_driver_callback_init(DEV_FLASH, &callbacks);
    return BFLB_NAND_OK;
}

int fatfs_spi_nand_driver_unregister(void)
{
    if (!flash) {
        return BFLB_NAND_ERR_INVALID_STATE;
    }

    int result = bflb_spi_nand_sync(flash);

    if (!result) {
        flash = NULL;
    }

    return result;
}
