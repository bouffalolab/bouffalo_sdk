#include <stdlib.h>
#include "bflb_uart.h"
#include "board.h"
#include "log.h"
#include "pikaScript.h"
#include <stdio.h>

struct bflb_device_s* uartx = NULL;

char pika_platform_getchar(void) {
    while (1) {
        int c = bflb_uart_getchar(uartx);
        if (c != -1) {
            return c;
        }
    }
}

int pika_platform_putchar(char ch) {
    bflb_uart_putchar(uartx, ch);
    return 0;
}

int main(void) {
    board_init();

    uartx = bflb_device_get_by_name("uart0");
    bflb_uart_feature_control(uartx, UART_CMD_SET_BAUD_RATE, 115200);

    PikaObj* root = pikaScriptInit();
    pikaScriptShell(root);
}
