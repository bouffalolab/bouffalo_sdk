#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uartx;

static uint8_t uart_txbuf[128] = { 0 };

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uartx);

    if (intstatus & UART_INTSTS_RX_FIFO) {
        printf("rx fifo\r\n");
        while (bflb_uart_rxavailable(uartx)) {
            printf("0x%02x\r\n", bflb_uart_getchar(uartx));
        }
    }
    if (intstatus & UART_INTSTS_RTO) {
        printf("rto\r\n");
        while (bflb_uart_rxavailable(uartx)) {
            printf("0x%02x\r\n", bflb_uart_getchar(uartx));
        }
        bflb_uart_int_clear(uartx, UART_INTCLR_RTO);
    }
    if (intstatus & UART_INTSTS_TX_FIFO) {
        printf("tx fifo\r\n");
        for (uint8_t i = 0; i < 27; i++) {
            bflb_uart_putchar(uartx, uart_txbuf[i]);
        }
        bflb_uart_txint_mask(uartx, true);
    }
}

int main(void)
{
    board_init();
    board_uartx_gpio_init();

    uartx = bflb_device_get_by_name(DEFAULT_TEST_UART);

    for (uint8_t i = 0; i < 128; i++) {
        uart_txbuf[i] = i;
    }

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    bflb_uart_init(uartx, &cfg);

    bflb_uart_txint_mask(uartx, false);
    bflb_uart_rxint_mask(uartx, false);
    bflb_irq_attach(uartx->irq_num, uart_isr, NULL);
    bflb_irq_enable(uartx->irq_num);

    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}
