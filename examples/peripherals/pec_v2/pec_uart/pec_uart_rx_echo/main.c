#include "board.h"
#include "bflb_pec_v2_instance.h"

#define UART_BAUDRATE (115200)
#define UART_DATABITS (8)
#define UART_STOPBITS (PEC_UART_STOPBITS_1P0)
#define UART_PARITY   (PEC_UART_PARITY_EVEN)

struct bflb_device_s *pec_uart;
uint8_t uart_buff[300];

struct bflb_pec_uart_rx_s uart_cfg = {
    .mem = 0,                        /*!< memory address of first instruction */
    .baudrate = UART_BAUDRATE,       /*!< uart baudrate */
    .databits = UART_DATABITS,       /*!< uart data bits */
    .stopbits = UART_STOPBITS,       /*!< uart stop bits, note: hardware don't check stopbits length */
    .parity = UART_PARITY,           /*!< uart parity */
    .pin = PEC_UART_RX_PIN % 32,     /*!< pin index of uart */
    .fifo_threshold = 0,             /*!< uart fifo threshold */
    .dma_enable = 0,                 /*!< enable or disable dma with uart */
    .pin_inverse = 0,                /*!< enable or disable uart_rx input level inverse */
    .rto_bits = 35,                  /*!< bits of RTO(receive timeout) */
};

int main(void)
{
    uint32_t len;

    board_init();
    board_pec_uart_gpio_init();
    pec_uart = bflb_device_get_by_name("pec_sm1");
    printf("pec_uart = 0x%08lX\r\n", pec_uart);

    bflb_pec_uart_rx_init(pec_uart, &uart_cfg);
    for (uint32_t i = 0; i < sizeof(uart_buff); i++) {
        uart_buff[i] = 0;
    }
    bflb_pec_uart_rx_start(pec_uart);
    while (1) {
        len = bflb_pec_fifo_read_multi_8bits(pec_uart, uart_buff, sizeof(uart_buff));
        for (uint32_t i = 0; i < len; i++) {
            printf("%02X ", uart_buff[i]);
        }
    }
}
