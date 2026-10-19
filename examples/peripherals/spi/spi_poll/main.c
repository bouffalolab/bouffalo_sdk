#include "bflb_mtimer.h"
#include "bflb_spi.h"
#include "board.h"

#define SPI_MASTER_CASE 0
#define SPI_SLAVE_CASE  1

#define SPI_CASE_SELECT SPI_MASTER_CASE

#define BUFF_LEN (8 * 1024)

uint32_t tx_buff[BUFF_LEN / 4];
uint32_t rx_buff[BUFF_LEN / 4];

struct bflb_device_s *spi0;

/* poll test func */
int bflb_spi_poll_test(uint32_t data_width)
{
    uint32_t data_mask;
    uint32_t *p_tx = (uint32_t *)tx_buff;
    uint32_t *p_rx = (uint32_t *)rx_buff;

    switch (data_width) {
        case SPI_DATA_WIDTH_8BIT:
            data_mask = 0x000000FF;
            break;
        case SPI_DATA_WIDTH_16BIT:
            data_mask = 0x0000FFFF;
            break;
        case SPI_DATA_WIDTH_24BIT:
            data_mask = 0x00FFFFFF;
            break;
        case SPI_DATA_WIDTH_32BIT:
            data_mask = 0xFFFFFFFF;
            break;
        default:
            printf("data_width err\r\n");
            return -1;
            break;
    }

    /* data init */
    for (uint16_t i = 0; i < BUFF_LEN / 4; i++) {
        p_tx[i] = i;
        p_rx[i] = 0;
    }

    /* set data width */
    bflb_spi_feature_control(spi0, SPI_CMD_SET_DATA_WIDTH, data_width);

    /* send data */
    for (uint16_t i = 0; i < BUFF_LEN / 4; i++) {
        p_rx[i] = bflb_spi_poll_send(spi0, p_tx[i]);
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
        bflb_mtimer_delay_us(10); /* delay for slave device prepare ok */
#endif
    }

    /* check data */
    for (uint16_t i = 0; i < BUFF_LEN / 4; i++) {
        if (p_rx[i] != (p_tx[i] & data_mask)) {
            printf("data error, data[%d]:tx 0x%08lX, rx 0x%08lX\r\n", i, p_tx[i], p_rx[i]);
            return -1;
        }
    }
    printf("data check success\r\n");

    return 0;
}

/* poll_exchange test func */
int bflb_spi_poll_exchange_test(uint32_t data_width)
{
    void *p_tx = (uint32_t *)tx_buff;
    void *p_rx = (uint32_t *)rx_buff;

    /* data init */
    switch (data_width) {
        case SPI_DATA_WIDTH_8BIT:
            for (uint16_t i = 0; i < BUFF_LEN; i++) {
                ((uint8_t *)p_tx)[i] = i;
                ((uint8_t *)p_rx)[i] = 0;
            }
            break;
        case SPI_DATA_WIDTH_16BIT:
            for (uint16_t i = 0; i < BUFF_LEN / 2; i++) {
                ((uint16_t *)p_tx)[i] = i << 0;
                ((uint16_t *)p_rx)[i] = 0;
            }
            break;
        case SPI_DATA_WIDTH_24BIT:
            for (uint16_t i = 0; i < BUFF_LEN / 4; i++) {
                ((uint32_t *)p_tx)[i] = ((i << 0) | i) & 0x00FFFFFF;
                ((uint32_t *)p_rx)[i] = 0;
            }
            break;
        case SPI_DATA_WIDTH_32BIT:
            for (uint16_t i = 0; i < BUFF_LEN / 4; i++) {
                ((uint32_t *)p_tx)[i] = (i << 0) | i;
                ((uint32_t *)p_rx)[i] = 0;
            }
            break;
        default:
            return -1;
            break;
    }

    /* set data width */
    bflb_spi_feature_control(spi0, SPI_CMD_SET_DATA_WIDTH, data_width);

    /* send data */
    printf("spi poll exchange width %ld, len %d\r\n", data_width, BUFF_LEN);
    bflb_spi_poll_exchange(spi0, p_tx, p_rx, BUFF_LEN);

    /* check data */
    for (uint16_t i = 0; i < BUFF_LEN / 4; i++) {
        if (((uint32_t *)p_rx)[i] != ((uint32_t *)p_tx)[i]) {
            printf("data error, data[%d]:tx 0x%08lX, rx 0x%08lX\r\n", i, ((uint32_t *)p_tx)[i], ((uint32_t *)p_rx)[i]);
            return -1;
        }
    }
    printf("data check success\r\n");

    return 0;
}

/* main */
int main(void)
{
    board_init();
    board_spi0_gpio_init();

    struct bflb_spi_config_s spi_cfg = {
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
        .freq = 1 * 1000 * 1000,
        .role = SPI_ROLE_MASTER,
#else
        .freq = 32 * 1000 * 1000,
        .role = SPI_ROLE_SLAVE,
#endif
        .mode = SPI_MODE3,
        .data_width = SPI_DATA_WIDTH_8BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    spi0 = bflb_device_get_by_name("spi0");
    bflb_spi_init(spi0, &spi_cfg);

    bflb_spi_feature_control(spi0, SPI_CMD_SET_CS_INTERVAL, 0);

    printf("\r\n************** spi poll send 8-bit test **************\r\n");
    if (bflb_spi_poll_test(SPI_DATA_WIDTH_8BIT) < 0) {
        printf("poll send 8-bit test error!!!\r\n");
    } else {
        printf("poll send 8-bit test success!\r\n");
    }
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif
    printf("\r\n************** spi poll send 16-bit test **************\r\n");
    if (bflb_spi_poll_test(SPI_DATA_WIDTH_16BIT) < 0) {
        printf("poll send 16-bit test error!!!\r\n");
    } else {
        printf("poll send 16-bit test success!\r\n");
    }
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif
    printf("\r\n************** spi poll send 24-bit test **************\r\n");
    if (bflb_spi_poll_test(SPI_DATA_WIDTH_24BIT) < 0) {
        printf("poll send 24-bit test error!!!\r\n");
    } else {
        printf("poll send 24-bit test success!\r\n");
    }
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif
    printf("\r\n************** spi poll send 32-bit test **************\r\n");
    if (bflb_spi_poll_test(SPI_DATA_WIDTH_32BIT) < 0) {
        printf("poll send 32-bit test error!!!\r\n");
    } else {
        printf("poll send 32-bit test success!\r\n");
    }

#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif

    printf("\r\n************** spi poll exchange 8-bit test **************\r\n");

    if (bflb_spi_poll_exchange_test(SPI_DATA_WIDTH_8BIT) < 0) {
        printf("poll exchange 8-bit test error!!!\r\n");
    } else {
        printf("poll exchange 8-bit test success!\r\n");
    }
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif
    printf("\r\n************** spi poll exchange 16-bit test **************\r\n");
    if (bflb_spi_poll_exchange_test(SPI_DATA_WIDTH_16BIT) < 0) {
        printf("poll exchange 16-bit test error!!!\r\n");
    } else {
        printf("poll exchange 16-bit test success!\r\n");
    }
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif
    printf("\r\n************** spi poll exchange 24-bit test **************\r\n");
    if (bflb_spi_poll_exchange_test(SPI_DATA_WIDTH_24BIT) < 0) {
        printf("poll exchange 24-bit test error!!!\r\n");
    } else {
        printf("poll exchange 24-bit test success!\r\n");
    }
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif
    printf("\r\n************** spi poll exchange 32-bit test **************\r\n");
    if (bflb_spi_poll_exchange_test(SPI_DATA_WIDTH_32BIT) < 0) {
        printf("poll exchange 32-bit test error!!!\r\n");
    } else {
        printf("poll exchange 32-bit test success!\r\n");
    }

#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif

    printf("\r\n************** spi poll exchange only send 32-bit test **************\r\n");
    bflb_spi_poll_exchange(spi0, tx_buff, NULL, BUFF_LEN);
    printf("poll exchange 32-bit only send test end!\r\n");
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif
    printf("\r\n************** spi poll exchange only receive 32-bit test **************\r\n");
    bflb_spi_poll_exchange(spi0, NULL, rx_buff, BUFF_LEN);
    printf("poll exchange 32-bit only receive test end!\r\n");
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
    bflb_mtimer_delay_ms(1000); /* delay for slave device prepare ok */
#endif
    printf("\r\n************** spi poll exchange spare time clock 32-bit test **************\r\n");
    bflb_spi_poll_exchange(spi0, NULL, NULL, BUFF_LEN);
    printf("poll exchange 32-bit spare time clock test end!\r\n");

    printf("\r\nspi test end\r\n");

    while (1) {
    }
}
