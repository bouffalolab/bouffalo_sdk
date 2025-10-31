#include "board.h"
#include "bflb_pec_v2_instance.h"

struct bflb_device_s *pec_spi;
uint32_t rx_buffer[256];

struct bflb_pec_spi_s spi_cfg = {
    .mem = 0,                          /*!< memory address of first instruction */
    .freq = 20 * 1000 * 1000,          /*!< spi clk pin frequency */
    .cpol = 0,                         /*!< clock polarity */
    .cpha = 0,                         /*!< clock phase */
    .pin_sck = PEC_SPI_SCK_PIN % 32,   /*!< pin of spi clock */
    .pin_mosi = PEC_SPI_MOSI_PIN % 32, /*!< pin of spi master out slave in */
    .pin_miso = PEC_SPI_MISO_PIN % 32, /*!< pin of spi master in slave out */
};

int main(void)
{
    board_init();
    board_pec_spi_gpio_init();
    pec_spi = bflb_device_get_by_name("pec_sm1");
    printf("pec_spi = 0x%08lX\r\n", pec_spi);
    bflb_pec_spi_init(pec_spi, &spi_cfg);

    bflb_pec_spi_set_bits(pec_spi, 8);
    bflb_pec_fifo_write(pec_spi, 0x5A);
    bflb_pec_fifo_write(pec_spi, 0xD1);
    while (bflb_pec_fifo_get_level_rx(pec_spi) != 2);
    for (int i = 0; i < 2; i++) {
        rx_buffer[i] = bflb_pec_fifo_read(pec_spi);
    }
    if (rx_buffer[0] != 0x5A || rx_buffer[1] != 0xD1) {
        printf("loopback 8-bits test failed\r\n");
    } else {
        printf("loopback 8-bits test passed\r\n");
    }
    bflb_pec_fifo_clr_rx(pec_spi);

    bflb_pec_spi_set_bits(pec_spi, 16);
    bflb_pec_fifo_write(pec_spi, 0x1234);
    bflb_pec_fifo_write(pec_spi, 0xE5B6);
    bflb_pec_fifo_write(pec_spi, 0x9037);
    while (bflb_pec_fifo_get_level_rx(pec_spi) != 3);
    for (int i = 0; i < 3; i++) {
        rx_buffer[i] = bflb_pec_fifo_read(pec_spi);
    }
    if (rx_buffer[0] != 0x1234 || rx_buffer[1] != 0xE5B6 || rx_buffer[2] != 0x9037) {
        printf("loopback 16-bits test failed\r\n");
    } else {
        printf("loopback 16-bits test passed\r\n");
    }
    bflb_pec_fifo_clr_rx(pec_spi);

    bflb_pec_spi_set_bits(pec_spi, 24);
    bflb_pec_fifo_write(pec_spi, 0x43219A);
    bflb_pec_fifo_write(pec_spi, 0xABCDEF);
    bflb_pec_fifo_write(pec_spi, 0x987654);
    bflb_pec_fifo_write(pec_spi, 0x543210);
    while (bflb_pec_fifo_get_level_rx(pec_spi) != 4);
    for (int i = 0; i < 4; i++) {
        rx_buffer[i] = bflb_pec_fifo_read(pec_spi);
    }
    if (rx_buffer[0] != 0x43219A || rx_buffer[1] != 0xABCDEF || rx_buffer[2] != 0x987654 || rx_buffer[3] != 0x543210) {
        printf("loopback 24-bits test failed\r\n");
    } else {
        printf("loopback 24-bits test passed\r\n");
    }
    bflb_pec_fifo_clr_rx(pec_spi);

    bflb_pec_spi_set_bits(pec_spi, 32);
    bflb_pec_fifo_write(pec_spi, 0xA1B2C3D4);
    bflb_pec_fifo_write(pec_spi, 0xF9E8D7C6);
    bflb_pec_fifo_write(pec_spi, 0x4758690A);
    bflb_pec_fifo_write(pec_spi, 0x976431F5);
    bflb_pec_fifo_write(pec_spi, 0x193764C2);
    while (bflb_pec_fifo_get_level_rx(pec_spi) != 5);
    for (int i = 0; i < 5; i++) {
        rx_buffer[i] = bflb_pec_fifo_read(pec_spi);
    }
    if (rx_buffer[0] != 0xA1B2C3D4 || rx_buffer[1] != 0xF9E8D7C6 || rx_buffer[2] != 0x4758690A || rx_buffer[3] != 0x976431F5 || rx_buffer[4] != 0x193764C2) {
        printf("loopback 32-bits test failed\r\n");
    } else {
        printf("loopback 32-bits test passed\r\n");
    }
    bflb_pec_fifo_clr_rx(pec_spi);

    return 0;
}
