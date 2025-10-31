#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uartx;

int main(void)
{
    board_init();
    board_uartx_gpio_init();

    uartx = bflb_device_get_by_name(DEFAULT_TEST_UART);

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    cfg.bit_order = UART_LSB_FIRST;
    bflb_uart_init(uartx, &cfg);

    int ch;
    while (1) {
        ch = bflb_uart_getchar(uartx);
        if (ch != -1) {
            bflb_uart_putchar(uartx, ch);
        }
    }
}
