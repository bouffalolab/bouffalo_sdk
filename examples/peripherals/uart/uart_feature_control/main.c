#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"
#include "shell.h"

struct bflb_device_s *uartx;
struct bflb_device_s *uart0;

int set_baudrate(int argc, char **argv)
{
    printf("Uart baudrate test\r\n");

    if ((argc != 2)) {
        printf("Usage: set_baudrate <baudrate value>\r\n"
               "example: set_baudrate 2000000\r\n");
        return 1;
    } else {
        printf("Set baudrate to %d\r\n", atoi(argv[1]));
    }

    bflb_uart_feature_control(uartx, UART_CMD_SET_BAUD_RATE, atoi(argv[1]));

    return 0;
}

int set_databit(int argc, char **argv)
{
    printf("Uart data bits test\r\n");

    if ((argc != 2) || (atoi(argv[1]) > 3)) {
        printf("Usage: set_databit <value>\r\n"
               "    0: UART_DATA_BITS_5\r\n"
               "    1: UART_DATA_BITS_6\r\n"
               "    2: UART_DATA_BITS_7\r\n"
               "    3: UART_DATA_BITS_8\r\n");
        return 1;
    } else {
        printf("Set uart data bits to %d\r\n", atoi(argv[1]));
    }

    bflb_uart_feature_control(uartx, UART_CMD_SET_DATA_BITS, atoi(argv[1]));

    return 0;
}

int set_stopbit(int argc, char **argv)
{
    printf("Uart stop bits test\r\n");

    if ((argc != 2) || (atoi(argv[1]) > 3)) {
        printf("Usage: set_stopbit <value>\r\n"
               "    0: UART_STOP_BITS_0_5\r\n"
               "    1: UART_STOP_BITS_1\r\n"
               "    2: UART_STOP_BITS_1_5\r\n"
               "    3: UART_STOP_BITS_2\r\n");
        return 1;
    } else {
        printf("Set uart stop bits to %d\r\n", atoi(argv[1]));
    }

    bflb_uart_feature_control(uartx, UART_CMD_SET_STOP_BITS, atoi(argv[1]));

    return 0;
}

int set_parity(int argc, char **argv)
{
    printf("Uart parity bits test\r\n");

    if ((argc != 2) || (atoi(argv[1]) > 2)) {
        printf("Usage: set_parity <parity value>\r\n"
               "    0: UART_PARITY_NONE\r\n"
               "    1: UART_PARITY_ODD\r\n"
               "    2: UART_PARITY_EVEN\r\n");
        return 1;
    }

    printf("Set parity bits to %d\r\n", atoi(argv[1]));
    bflb_uart_feature_control(uartx, UART_CMD_SET_PARITY_BITS, atoi(argv[1]));

    return 0;
}

int main(void)
{
    int ch;

    board_init();
    board_uartx_gpio_init();

    uart0 = bflb_device_get_by_name("uart0");
    uartx = bflb_device_get_by_name(DEFAULT_TEST_UART);

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 512000;
    cfg.data_bits = UART_DATA_BITS_7;
    cfg.stop_bits = UART_STOP_BITS_2;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    cfg.bit_order = UART_LSB_FIRST;
    bflb_uart_init(uartx, &cfg);

    bflb_uart_feature_control(uartx, UART_CMD_SET_BAUD_RATE, 2000000);
    bflb_uart_feature_control(uartx, UART_CMD_SET_DATA_BITS, UART_DATA_BITS_8);
    bflb_uart_feature_control(uartx, UART_CMD_SET_STOP_BITS, UART_STOP_BITS_1);
    bflb_uart_feature_control(uartx, UART_CMD_SET_PARITY_BITS, UART_PARITY_EVEN);

    shell_init();

    while (1) {
        while ((ch = bflb_uart_getchar(uart0)) != -1) {
            shell_handler(ch);
        }

        while ((ch = bflb_uart_getchar(uartx)) != -1) {
            bflb_uart_putchar(uartx, ch);
        }
    }
}

SHELL_CMD_EXPORT_ALIAS(set_baudrate, set_baudrate, shell set_baudrate.);
SHELL_CMD_EXPORT_ALIAS(set_databit, set_databit, shell set_databit.);
SHELL_CMD_EXPORT_ALIAS(set_stopbit, set_stopbit, shell set_stopbit.);
SHELL_CMD_EXPORT_ALIAS(set_parity, set_parity, shell set_parity.);