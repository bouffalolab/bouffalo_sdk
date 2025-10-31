#include "bflb_mtimer.h"
#include "bflb_spi.h"
#include "board.h"

#define SPI_FLASH_ADDR      (0)
#define SPI_FLASH_PAGE_SIZE (256)

struct bflb_device_s *spi0;

static uint8_t tx[512];
static uint8_t rx[512];

static struct bflb_spi_config_s spi_cfg = {
    .freq = 40 * 1000 * 1000,
    .role = SPI_ROLE_MASTER,
    .mode = SPI_MODE0,
    .data_width = SPI_DATA_WIDTH_8BIT,
    .bit_order = SPI_BIT_MSB,
    .byte_order = SPI_BYTE_MSB,
    .tx_fifo_threshold = 0,
    .rx_fifo_threshold = 0,
};

static void spi_flash_wait_ready(void)
{
    tx[0] = 0x05;
    tx[1] = 0;
    while (1) {
        bflb_spi_poll_exchange(spi0, tx, rx, 2);
        if ((rx[1] & 0x01) == 0) {
            break;
        }
    }
}

static void spi_flash_read_jedec(void)
{
    tx[0] = 0x9F;
    tx[1] = 0;
    tx[2] = 0;
    tx[3] = 0;
    bflb_spi_poll_exchange(spi0, tx, rx, 4);
    if (rx[1] != 0xEF) {
        printf("SPI flash error\r\n");
        return;
    }
    printf("SPI Flash JEDEC ID: %02X %02X %02X\r\n", rx[1], rx[2], rx[3]);
}

static void spi_flash_enable_write(void)
{
    tx[0] = 0x06;
    bflb_spi_poll_exchange(spi0, tx, rx, 1);
}

static void spi_flash_erase_sector(void)
{
    spi_flash_enable_write();
    tx[0] = 0x20;
    tx[1] = (uint8_t)(SPI_FLASH_ADDR >> 16);
    tx[2] = (uint8_t)(SPI_FLASH_ADDR >> 8);
    tx[3] = (uint8_t)(SPI_FLASH_ADDR >> 0);
    bflb_spi_poll_exchange(spi0, tx, rx, 4);
    spi_flash_wait_ready();
    printf("SPI flash erase complete\r\n");
}

static void spi_flash_write_page(void)
{
    spi_flash_enable_write();
    tx[0] = 0x02;
    tx[1] = 0;
    tx[2] = 0;
    tx[3] = 0;
    for (size_t i = 0; i < SPI_FLASH_PAGE_SIZE; i++) {
        tx[4 + i] = i;
    }
    bflb_spi_poll_exchange(spi0, tx, rx, 4 + SPI_FLASH_PAGE_SIZE);
    spi_flash_wait_ready();
    printf("SPI flash write complete\r\n");
}

static void spi_flash_read(void)
{
    tx[0] = 0x03;
    tx[1] = (uint8_t)(SPI_FLASH_ADDR >> 16);
    tx[2] = (uint8_t)(SPI_FLASH_ADDR >> 8);
    tx[3] = (uint8_t)(SPI_FLASH_ADDR >> 0);
    for (size_t i = 0; i < SPI_FLASH_PAGE_SIZE; i++) {
        rx[4 + i] = 0;
    }
    bflb_spi_poll_exchange(spi0, tx, rx, 4 + SPI_FLASH_PAGE_SIZE);
    for (size_t i = 0; i < SPI_FLASH_PAGE_SIZE; i++) {
        if (rx[4 + i] != i) {
            printf("SPI flash read error\r\n");
            return;
        }
    }
    printf("SPI flash read ok\r\n");
}

int main(void)
{
    board_init();
    board_spi0_gpio_init();

    spi0 = bflb_device_get_by_name("spi0");
    bflb_spi_init(spi0, &spi_cfg);
    bflb_spi_feature_control(spi0, SPI_CMD_SET_CS_INTERVAL, true);

    printf("\n\rspi flash test\n\r");

    spi_flash_read_jedec();
    spi_flash_erase_sector();
    spi_flash_write_page();
    spi_flash_read();

    printf("\n\rtest end\n\r");

    return 0;
}
