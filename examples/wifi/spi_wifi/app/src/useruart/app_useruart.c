#include <stdio.h>
#include <stdint.h>

#include <bflb_uart.h>
#include <FreeRTOS.h>
#include <task.h>
#include <shell.h>

#include <spisync.h>

#define USERUART_STACK_SIZE         (2048)
#define USERUUART_TASK_PRIORITY     (16)

struct bflb_device_s *s_useruart = NULL;

extern spisync_t *g_pspisync;

static void _uart_isr(int irq, void *arg)
{
    char ch;
    uint32_t intstatus = bflb_uart_get_intstatus(s_useruart);

    if (intstatus & UART_INTSTS_RX_FIFO) {
        while (bflb_uart_rxavailable(s_useruart)) {
            //uart_rxbuf[uart_rx_count++] = bflb_uart_getchar(s_useruart);
            ch = (char)bflb_uart_getchar(s_useruart);
            //printf("recv fofo uart1:%02X\r\n", ch);
            if (g_pspisync) {
                if (1 != spisync_fakewrite_forread(g_pspisync, &ch, 1, 0)) {
                    printf("fifo uart to stream error.\r\n");
                }
            }
        }
    }
    if (intstatus & UART_INTSTS_RTO) {
        bflb_uart_int_clear(s_useruart, UART_INTCLR_RTO);
        while (bflb_uart_rxavailable(s_useruart)) {
            //uart_rxbuf[uart_rx_count++] = bflb_uart_getchar(s_useruart);
            ch = (char)bflb_uart_getchar(s_useruart);
            //printf("recv rto uart1:%02X\r\n", ch);
            if (g_pspisync) {
                if (1 != spisync_fakewrite_forread(g_pspisync, &ch, 1, 0)) {
                    printf("rto uart to stream error.\r\n");
                }
            }
        }
    }
#if 0
    if (intstatus & UART_INTSTS_TX_FIFO) {
        printf("tx fifo\r\n");
        for (uint8_t i = 0; i < 27; i++) {
            //bflb_uart_putchar(s_useruart, uart_txbuf[i]);
        }
        bflb_uart_txint_mask(s_useruart, true);
    }
#endif
}
static void _uart_init(void)
{
    struct bflb_uart_config_s cfg;

    s_useruart = bflb_device_get_by_name("uart1");

    cfg.baudrate = 115200;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    cfg.bit_order = UART_LSB_FIRST;
    bflb_uart_init(s_useruart, &cfg);

    //bflb_uart_txint_mask(s_useruart, false);
    bflb_uart_rxint_mask(s_useruart, false);
    bflb_irq_attach(s_useruart->irq_num, _uart_isr, NULL);
    bflb_irq_enable(s_useruart->irq_num);
}
#if 1
void useruart_demo_task(void *arg)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    _uart_init();

    vTaskDelete(NULL);
}

int cmd_useruart_test(int argc, char **argv)
{
    xTaskCreate(useruart_demo_task, (char *)"useruart", USERUART_STACK_SIZE/4, NULL, USERUUART_TASK_PRIORITY, NULL);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_useruart_test, useruart_test, useruart test.);

void app_useruart_init(void)
{
}
#else
void app_useruart_init(void)
{
    _uart_init();
}
#endif

