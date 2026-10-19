#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uartx;

static uint8_t uart_txbuf[128] = { 0 };
static uint8_t uart_rxbuf[128] = { 0 };

int main(void)
{
    board_init();
    board_uartx_gpio_init();

    uartx = bflb_device_get_by_name(DEFAULT_TEST_UART);

    for (uint8_t i = 0; i < 128; i++) {
        uart_txbuf[i] = i;
        uart_rxbuf[i] = 0;
    }

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 115200;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 1;
    cfg.bit_order = UART_LSB_FIRST;
    bflb_uart_init(uartx, &cfg);

    struct bflb_uart_ir_config_s ircfg;

    ircfg.tx_en = 1;
    ircfg.rx_en = 1;
    ircfg.tx_inverse = 1;
    ircfg.rx_inverse = 0;
    ircfg.tx_pluse_start = 112;
    ircfg.tx_pluse_stop = 159;
    ircfg.rx_pluse_start = 111;
    bflb_uart_feature_control(uartx, UART_CMD_IR_CONFIG, (size_t)&ircfg);

    for (uint8_t i = 0; i < 128; i++) {
        bflb_uart_putchar(uartx, uart_txbuf[i]);
        while (bflb_uart_feature_control(uartx, UART_CMD_GET_RX_FIFO_CNT, 1) == 0) {
        }
        uart_rxbuf[i] = bflb_uart_getchar(uartx);
    }
    printf("All data arrived\r\n");

    for (uint8_t j = 0; j < 128; j++) {
        if (uart_txbuf[j] != uart_rxbuf[j]) {
            printf("check fail, %d tx: %02x, rx: %02x\r\n", j, uart_txbuf[j], uart_rxbuf[j]);
        }
    }
}
