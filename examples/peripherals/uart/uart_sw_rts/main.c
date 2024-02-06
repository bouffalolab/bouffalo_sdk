#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uartx;

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uartx);

    if (intstatus & UART_INTSTS_RX_FIFO) {
        printf("rx fifo\r\n");
        while (bflb_uart_rxavailable(uartx)) {
            printf("0x%02x\r\n", bflb_uart_getchar(uartx));
        }
        bflb_uart_feature_control(uartx, UART_CMD_SET_RTS_VALUE, 1);
    }
    if (intstatus & UART_INTSTS_RTO) {
        bflb_uart_int_clear(uartx, UART_INTCLR_RTO);
        printf("rto\r\n");
        while (bflb_uart_rxavailable(uartx)) {
            printf("0x%02x\r\n", bflb_uart_getchar(uartx));
        }
    }
}

int main(void)
{
    board_init();
    board_uartx_gpio_init();

    uartx = bflb_device_get_by_name(DEFAULT_TEST_UART);

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 256000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    cfg.bit_order = UART_LSB_FIRST;
    bflb_uart_init(uartx, &cfg);

    bflb_uart_rxint_mask(uartx, false);
    bflb_irq_attach(uartx->irq_num, uart_isr, NULL);
    bflb_irq_enable(uartx->irq_num);

    bflb_uart_feature_control(uartx, UART_CMD_SET_SW_RTS_CONTROL, true);
    bflb_uart_feature_control(uartx, UART_CMD_SET_RTS_VALUE, 0);
    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}
