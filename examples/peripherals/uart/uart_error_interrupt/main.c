#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uartx;

static uint8_t pce_int_flag = 0;

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uartx);

    if (intstatus & UART_INTSTS_PCE) {
        bflb_uart_int_clear(uartx, UART_INTCLR_PCE);
        pce_int_flag++;
        printf("Enter Parity int\r\n");
    }
}

int main(void)
{
    board_init();
    board_uartx_gpio_init();

    uartx = bflb_device_get_by_name(DEFAULT_TEST_UART);

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_ODD;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    cfg.bit_order = UART_LSB_FIRST;
    bflb_uart_init(uartx, &cfg);

    bflb_uart_errint_mask(uartx, false);
    bflb_irq_attach(uartx->irq_num, uart_isr, NULL);
    bflb_irq_enable(uartx->irq_num);

    int ch;
    while (1) {
        ch = bflb_uart_getchar(uartx);
        if (ch != -1) {
            bflb_uart_putchar(uartx, ch);
        }
        if(pce_int_flag){
            pce_int_flag = 0;
            printf("Parity type mismatch at odd parity\n");
    }
    }
}
