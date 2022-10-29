#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uart1;

static uint8_t uart_txbuf[128] = {0};

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart1);

    if (intstatus & UART_INTSTS_RX_FIFO) {
        while (bflb_uart_rxavailable(uart1)) {
            printf("enter rx fifo interrupt");
            printf("0x%02x\r\n", bflb_uart_getchar(uart1));
        }
    }
    if (intstatus & UART_INTSTS_RTO) {
        while (bflb_uart_rxavailable(uart1)) {
            printf("enter rto interrupt");
            printf("0x%02x\r\n", bflb_uart_getchar(uart1));
        }
        bflb_uart_int_clear(uart1, UART_INTCLR_RTO);
    }
    if (intstatus & UART_INTSTS_TX_FIFO) {
        for (uint8_t i = 0; i < 27; i++)
        {
            bflb_uart_putchar(uart1,uart_txbuf[i]);
        }
        bflb_uart_txint_mask(uart1, true);
        printf("tx interrupt end");
    }
}

int main(void)
{
    board_init();
    board_uart1_gpio_init();

    uart1 = bflb_device_get_by_name("uart1");

    for(uint8_t i=0; i < 128; i++)
    {
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
    bflb_uart_init(uart1, &cfg);

    bflb_uart_txint_mask(uart1, false);
    bflb_uart_rxint_mask(uart1, false);
    bflb_irq_attach(uart1->irq_num, uart_isr, uart1);
    bflb_irq_enable(uart1->irq_num);

    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}
