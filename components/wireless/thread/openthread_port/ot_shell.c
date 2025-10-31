
#include <openthread_port.h>

#include <bflb_uart.h>
#include <shell.h>
#include <utils/uart.h>

otError otPlatUartEnable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    extern struct bflb_device_s *console;

    bflb_uart_put(console, (uint8_t *)aBuf, aBufLength);

    otPlatUartSendDone();
    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void)
{
    return OT_ERROR_NONE;
}

int openthread_cli_cmd(int argc, char **argv)
{
    int i = 0;

    if (argc <= 1) {
        return 0;
    }

    for (i = 0 ; i < argc - 1; i ++) {
        argv[i][strlen(argv[i])] = ' ';
    }

    i = strlen(argv[1]);
    argv[1][i] = '\n';

    OT_THREAD_SAFE (
        otPlatUartReceived((uint8_t *)argv[1], i + 1);
    );

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(openthread_cli_cmd, otc, openthread command line.);
