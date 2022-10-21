#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uart1;
static uint8_t uart_txbuf[128] = { 0 };

static uint8_t lse_int_flag = 0;

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart1);

    if (intstatus & UART_INTSTS_RX_LSE) {
        lse_int_flag++;
        printf("enter rx lse interrupt");
    }
    bflb_uart_int_clear(uart1, UART_INTCLR_RX_LSE);
}

int main(void)
{
    board_init();
    board_uart1_gpio_init();

    uart1 = bflb_device_get_by_name("uart1");

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;


    bflb_uart_errint_mask(uart1, false);
    bflb_irq_attach(uart1->irq_num, uart_isr, uart1);
    bflb_irq_enable(uart1->irq_num);

    bflb_uart_feature_control(uart1, UART_CMD_SET_BREAK_VALUE, 4);
    bflb_uart_feature_control(uart1, UART_CMD_SET_TX_LIN_VALUE, 1);
    bflb_uart_feature_control(uart1, UART_CMD_SET_RX_LIN_VALUE, 1);

    bflb_uart_init(uart1, &cfg);

    bflb_uart_putchar(uart1, 0xff);
    bflb_uart_putchar(uart1, 0x1);

    while (1) {
        if (lse_int_flag) {
            lse_int_flag = 0;
            printf("RX lin error interrupt\n");
        }
    }
}
