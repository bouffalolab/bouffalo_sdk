/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "bflb_spi_nand_lhal.h"
#include "fatfs_spi_nand.h"
#include "ff.h"

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#if defined(BL618DG)
#define NAND_CS_PIN 16
#elif defined(BL616CL)
#define NAND_CS_PIN 12
#else
#error "Set the NAND CS pin and verify board_spi0_gpio_init for this board"
#endif

static FATFS filesystem;
static bflb_spi_nand_lhal_t port;
static const char filename[] = "/flash/spi_nand_test.bin";
static uint8_t expected[1537];
static uint8_t actual[sizeof(expected)];

static int open_nand(bflb_spi_nand_t **nand)
{
    bflb_spi_nand_bus_t bus;
    int error = bflb_spi_nand_lhal_init(&port, "spi0", NAND_CS_PIN, NULL, true, &bus);

    if (error != BFLB_NAND_OK) {
        return error;
    }

    return bflb_spi_nand_init(&bus, 0, nand);
}

static void run_example(void *argument)
{
    (void)argument;

    bflb_spi_nand_t *nand = NULL;
    int error = open_nand(&nand);

    if (error != BFLB_NAND_OK) {
        printf("NAND init failed: 0x%x\r\n", error);
        goto done;
    }

    bflb_spi_nand_info_t info;
    bflb_spi_nand_get_info(nand, &info);
    printf("NAND ID %02x %04x, page %lu bytes, logical sectors %lu\r\n", info.manufacturer_id, info.device_id,
           (unsigned long)info.page_size, (unsigned long)info.sector_count);

    error = fatfs_spi_nand_driver_register(nand);
    if (error != BFLB_NAND_OK) {
        goto release;
    }

    FRESULT result = f_mount(&filesystem, "/flash", 1);
#if defined(CONFIG_SPI_NAND_EXAMPLE_FORMAT) && CONFIG_SPI_NAND_EXAMPLE_FORMAT
    if (result == FR_NO_FILESYSTEM) {
        uint8_t work[4096];
        MKFS_PARM options = {
            .fmt = FM_FAT | FM_SFD,
            .au_size = 4096,
        };

        result = f_mkfs("/flash", &options, work, sizeof(work));
        if (result == FR_OK) {
            result = f_mount(&filesystem, "/flash", 1);
        }
    }
#endif
    if (result != FR_OK) {
        printf("Mount /flash failed: %u. For a blank device, explicitly enable SPI_NAND_EXAMPLE_FORMAT.\r\n", result);
        goto unregister;
    }

    for (unsigned i = 0; i < sizeof(expected); i++) {
        expected[i] = (uint8_t)(i * 17U);
    }

    FIL file;
    UINT bytes;

    result = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK) {
        goto unmount;
    }

    result = f_write(&file, expected, sizeof(expected), &bytes);
    if (result == FR_OK && bytes != sizeof(expected)) {
        result = FR_DISK_ERR;
    }
    if (result == FR_OK) {
        result = f_lseek(&file, 333);
    }
    memset(expected + 333, 0xa7, 513);
    if (result == FR_OK) {
        result = f_write(&file, expected + 333, 513, &bytes);
        if (result == FR_OK && bytes != 513) {
            result = FR_DISK_ERR;
        }
    }
    if (result == FR_OK) {
        result = f_sync(&file);
    }
    FRESULT close_result = f_close(&file);

    if (result == FR_OK) {
        result = close_result;
    }
    if (result != FR_OK) {
        goto unmount;
    }

    f_mount(NULL, "/flash", 0);
    error = fatfs_spi_nand_driver_unregister();
    if (error != BFLB_NAND_OK) {
        goto done;
    }
    error = bflb_spi_nand_deinit(nand);
    if (error != BFLB_NAND_OK) {
        goto done;
    }
    nand = NULL;

    error = open_nand(&nand);
    if (error != BFLB_NAND_OK) {
        goto done;
    }
    error = fatfs_spi_nand_driver_register(nand);
    if (error != BFLB_NAND_OK) {
        goto release;
    }
    result = f_mount(&filesystem, "/flash", 1);
    if (result == FR_OK) {
        result = f_open(&file, filename, FA_READ);
    }
    if (result == FR_OK) {
        result = f_read(&file, actual, sizeof(actual), &bytes);
        if (result == FR_OK && (bytes != sizeof(actual) || memcmp(actual, expected, sizeof(actual)))) {
            result = FR_INT_ERR;
        }
        close_result = f_close(&file);
        if (result == FR_OK) {
            result = close_result;
        }
    }
    printf("NAND FatFs rewrite + reinitialize: %s (%u)\r\n", result == FR_OK ? "PASS" : "FAIL", result);

unmount:
    if (result != FR_OK) {
        printf("FatFs operation failed: %u\r\n", result);
    }
    f_mount(NULL, "/flash", 0);

unregister:
    error = fatfs_spi_nand_driver_unregister();
    if (error != BFLB_NAND_OK) {
        goto done;
    }

release:
    if (error != BFLB_NAND_OK) {
        printf("NAND operation failed: 0x%x\r\n", error);
    }
    error = bflb_spi_nand_deinit(nand);

done:
    if (error != BFLB_NAND_OK) {
        printf("NAND error: 0x%x\r\n", error);
    }
#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    vTaskDelete(NULL);
#endif
}

int main(void)
{
    board_init();
    board_spi0_gpio_init();

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    if (xTaskCreate(run_example, "nand", 3072, NULL, 2, NULL) != pdPASS) {
        printf("Cannot create NAND task\r\n");
        return 1;
    }
    vTaskStartScheduler();
#else
    run_example(NULL);
#endif

    while (1) {
    }
}
