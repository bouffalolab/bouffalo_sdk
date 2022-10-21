#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uart1;

static uint8_t pce_int_flag = 0;

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart1);

    if (intstatus & UART_INTSTS_PCE) {
        pce_int_flag++;
        printf("Enter Parity int\r\n");
    }
    bflb_uart_int_clear(uart1, UART_INTCLR_PCE);
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
    cfg.parity = UART_PARITY_ODD;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    bflb_uart_init(uart1, &cfg);

    bflb_uart_errint_mask(uart1, false);
    bflb_irq_attach(uart1->irq_num, uart_isr, uart1);
    bflb_irq_enable(uart1->irq_num);

    int ch;
    while (1) {
        ch = bflb_uart_getchar(uart1);
        if (ch != -1) {
            bflb_uart_putchar(uart1, ch);
        }
        if(pce_int_flag){
            pce_int_flag = 0;
            printf("Parity type mismatch at odd parity\n");
    }
    }
}
