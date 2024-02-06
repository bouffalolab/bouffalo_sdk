#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uartx;

#define RX_BUFF_SIZE 128
static uint8_t uart_txbuf[128] = { 0 };
static uint8_t uart_rxbuf[RX_BUFF_SIZE] = { 0 };
static volatile uint8_t tx_end_flag = 0;
static uint8_t data_count = 0;

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uartx);

    if (intstatus & UART_INTSTS_RX_FIFO) {
        while (bflb_uart_rxavailable(uartx)) {
            uart_rxbuf[data_count++] = bflb_uart_getchar(uartx);
        }
    }
    if (intstatus & UART_INTSTS_RTO) {
        bflb_uart_int_clear(uartx, UART_INTCLR_RTO);
        while (bflb_uart_rxavailable(uartx)) {
            uart_rxbuf[data_count++] = bflb_uart_getchar(uartx);
        }
    }
    if (intstatus & UART_INTSTS_TX_END) {
        tx_end_flag = 1;
        bflb_uart_int_clear(uartx, UART_INTCLR_TX_END);
        printf("tx end\r\n");
    }
    if (intstatus & UART_INTSTS_RX_END) {
        bflb_uart_int_clear(uartx, UART_INTCLR_RX_END);
        printf("rx end\r\n");
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
    cfg.bit_order = UART_LSB_FIRST;
    bflb_uart_init(uartx, &cfg);

    /* use tx end must stop tx freerun */
    bflb_uart_feature_control(uartx, UART_CMD_SET_TX_FREERUN, false);

    bflb_uart_feature_control(uartx, UART_CMD_SET_RX_TRANSFER_LEN, 64);
    bflb_uart_feature_control(uartx, UART_CMD_SET_TX_END_INTERRUPT, true);
    bflb_uart_feature_control(uartx, UART_CMD_SET_RX_END_INTERRUPT, true);
    bflb_uart_rxint_mask(uartx, false);
    bflb_irq_attach(uartx->irq_num, uart_isr, NULL);
    bflb_irq_enable(uartx->irq_num);

    while (1) {
        bflb_uart_feature_control(uartx, UART_CMD_SET_TX_EN, true);
        bflb_uart_feature_control(uartx, UART_CMD_SET_TX_TRANSFER_LEN, 128);
        for (uint8_t i = 0; i < 128; i++) {
            bflb_uart_putchar(uartx, uart_txbuf[i]);
        }
        while (tx_end_flag == 0) {}
        tx_end_flag = 0;
        bflb_uart_feature_control(uartx, UART_CMD_SET_TX_EN, false);
        bflb_mtimer_delay_ms(100);
        if (data_count) {
            if (data_count > RX_BUFF_SIZE) {
                data_count = RX_BUFF_SIZE;
            }
            for (uint8_t j = 0; j < data_count; j++) {
                printf("receive data:%02x\r\n", uart_rxbuf[j]);
            }
            data_count = 0;
        }
    }
}
