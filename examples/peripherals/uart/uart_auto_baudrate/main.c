#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "bflb_clock.h"
#include "board.h"

struct bflb_device_s *uartx;

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uartx);
    int ret;
    uint32_t baudrate;

    if (intstatus & UART_INTSTS_RX_AD5) {
        bflb_uart_int_clear(uartx, UART_INTCLR_RX_AD5);
        ret = bflb_uart_feature_control(uartx, UART_CMD_GET_AUTO_BAUD, UART_AUTO_BAUD_0X55);
        baudrate = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_UART, uartx->idx) / (ret + 1);
        printf("Detected baudrate by 0x55 is %d\r\n", baudrate);
    }
  
    if (intstatus & UART_INTSTS_RX_ADS) {
        bflb_uart_int_clear(uartx, UART_INTCLR_RX_ADS);
        ret = bflb_uart_feature_control(uartx, UART_CMD_GET_AUTO_BAUD, UART_AUTO_BAUD_START);
        baudrate = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_UART, uartx->idx) / (ret + 1);
        printf("Detected baudrate by startbit is %d\r\n", baudrate);
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
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    cfg.bit_order = UART_LSB_FIRST;
    bflb_uart_init(uartx, &cfg);

    bflb_uart_feature_control(uartx, UART_CMD_SET_AUTO_BAUD, 1);
    bflb_uart_feature_control(uartx, UART_CMD_SET_ABR_ALLOWABLE_ERROR, 7);

    bflb_irq_attach(uartx->irq_num, uart_isr, NULL);
    bflb_irq_enable(uartx->irq_num);

    while (1) {
    }
}
