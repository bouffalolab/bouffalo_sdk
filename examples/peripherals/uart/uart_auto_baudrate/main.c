#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"
#include "bl616_hbn.h"

struct bflb_device_s *uart1;

static uint32_t uart_txbuf[3];

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart1);

    if (intstatus & UART_INTSTS_RX_AD5) {
        uart_txbuf[0] = UART_AUTO_BAUD_0X55;
        bflb_uart_feature_control(uart1, UART_CMD_GET_AUTO_BAUD, (uint32_t)uart_txbuf);
        uint32_t baudrate = 40000000 / (uart_txbuf[1] + 1);
        printf("Detected baudrate by 0x55 is %d\r\n", baudrate);
    }
    bflb_uart_int_clear(uart1, UART_INTCLR_RX_AD5);

    if (intstatus & UART_INTSTS_RX_ADS) {
        uart_txbuf[0] = UART_AUTO_BAUD_START;
        bflb_uart_feature_control(uart1, UART_CMD_GET_AUTO_BAUD, (uint32_t)uart_txbuf);
        uint32_t baudrate = 40000000 / (uart_txbuf[1] + 1);
        printf("Detected baudrate by startbit is %d\r\n", baudrate);
    }
    bflb_uart_int_clear(uart1, UART_INTCLR_RX_ADS);

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
    bflb_uart_init(uart1, &cfg);

    bflb_uart_feature_control(uart1, UART_CMD_SET_AUTO_BAUD, 1);
    bflb_uart_feature_control(uart1, UART_CMD_SET_ABR_PW_VALUE, 3);

    bflb_irq_attach(uart1->irq_num, uart_isr, uart1);
    bflb_irq_enable(uart1->irq_num);

}
