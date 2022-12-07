#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "shell.h"
#include "board.h"

static struct bflb_device_s *uart0;

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart0);
    if (intstatus & UART_INTSTS_RX_FIFO) {
        while (bflb_uart_rxavailable(uart0)) {
            shell_handler(bflb_uart_getchar(uart0));
        }
    }
    if (intstatus & UART_INTSTS_RTO) {
        while (bflb_uart_rxavailable(uart0)) {
            shell_handler(bflb_uart_getchar(uart0));
        }
        bflb_uart_int_clear(uart0, UART_INTCLR_RTO);
    }
}

int main(void)
{
    int ch;
    board_init();
    uart0 = bflb_device_get_by_name("uart0");
    bflb_uart_rxint_mask(uart0, false);
    bflb_irq_attach(uart0->irq_num, uart_isr, uart0);
    bflb_irq_enable(uart0->irq_num);
    shell_init();
    while (1) {
        if((ch = bflb_uart_getchar(uart0)) != -1)
        {
            shell_handler(ch);
        }
    }
}
