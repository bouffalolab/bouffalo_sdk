#include "board.h"
#include "bflb_pec_v2_instance.h"

#define UART_BAUDRATE (115200)
#define UART_DATABITS (8)
#define UART_STOPBITS (PEC_UART_STOPBITS_1P0)
#define UART_PARITY   (PEC_UART_PARITY_NONE)

#define UART_BUFFER_SIZE (1000)

struct bflb_device_s *pec_uart_tx;
struct bflb_device_s *pec_uart_rx;
uint8_t uart_tx_buff[UART_BUFFER_SIZE];
uint8_t uart_rx_buff[UART_BUFFER_SIZE];

struct bflb_pec_uart_tx_s uart_tx_cfg = {
    .mem = 0,                     /*!< memory address of first instruction */
    .baudrate = UART_BAUDRATE,    /*!< uart baudrate */
    .databits = UART_DATABITS,    /*!< uart data bits */
    .stopbits = UART_STOPBITS,    /*!< uart stop bits */
    .parity = UART_PARITY,        /*!< uart parity */
    .pin = PEC_UART_TX_PIN % 32,  /*!< pin index of uart */
    .fifo_threshold = 0,          /*!< uart fifo threshold */
    .dma_enable = 0,              /*!< enable or disable dma with uart */
    .pin_inverse = 0,             /*!< enable or disable uart_tx output level inverse */
};

struct bflb_pec_uart_rx_s uart_rx_cfg = {
#if defined(BL616L)
    .mem = 0,                        /*!< memory address of first instruction */
#else
    .mem = 128,                      /*!< memory address of first instruction */
#endif
    .baudrate = UART_BAUDRATE,       /*!< uart baudrate */
    .databits = UART_DATABITS,       /*!< uart data bits */
    .stopbits = UART_STOPBITS,       /*!< uart stop bits, note: hardware don't check stopbits length */
    .parity = UART_PARITY,           /*!< uart parity */
    .pin = PEC_UART_RX_PIN % 32,     /*!< pin index of uart */
    .fifo_threshold = 1 - 1,         /*!< uart fifo threshold */
    .dma_enable = 0,                 /*!< enable or disable dma with uart */
    .pin_inverse = 0,                /*!< enable or disable uart_rx input level inverse */
    .rto_bits = 35,                  /*!< bits of RTO(receive timeout) */
};

int main(void)
{
    board_init();
    board_pec_uart_gpio_init();
    pec_uart_tx = bflb_device_get_by_name("pec_sm0");
    printf("pec_uart_tx = 0x%08lX\r\n", pec_uart_tx);
    pec_uart_rx = bflb_device_get_by_name("pec_sm1");
    printf("pec_uart_rx = 0x%08lX\r\n", pec_uart_rx);

    bflb_pec_uart_tx_init(pec_uart_tx, &uart_tx_cfg);
    bflb_pec_uart_rx_init(pec_uart_rx, &uart_rx_cfg);
    bflb_pec_uart_rx_start(pec_uart_rx);
    for (uint32_t i = 0; i < UART_BUFFER_SIZE; i++) {
        uart_tx_buff[i] = i % 256;
        uart_rx_buff[i] = 0;
    }
    
    while (1) {
        uint8_t level;
        static uint32_t idx_tx = 0;
        static uint32_t idx_rx = 0;
        level = bflb_pec_fifo_get_level_tx(pec_uart_tx);
        level = 16 - level;
        bflb_pec_fifo_write_multi_8bits_blocking(pec_uart_tx, uart_tx_buff + idx_tx, level);
        idx_tx += level;
        level = bflb_pec_fifo_get_level_rx(pec_uart_rx);
        for (uint8_t i = 0; i < level; i++) {
            uint8_t recv = (uint8_t)bflb_pec_fifo_read(pec_uart_rx);
            uart_rx_buff[idx_rx + i] = recv;
            printf("%02X ", recv);
        }
        idx_rx += level;
        if (idx_rx >= UART_BUFFER_SIZE) {
            break;
        }
    }
    for (uint32_t i = 0; i < UART_BUFFER_SIZE; i++) {
        if (uart_rx_buff[i] != uart_tx_buff[i]) {
            printf("\r\ncheck error, [%d] expert 0x%02X, but actual is 0x%02X\r\n", i, uart_tx_buff[i], uart_rx_buff[i]);
            return -1;
        }
    }
    printf("\r\ncheck pass\r\n");

    return 0;
}
