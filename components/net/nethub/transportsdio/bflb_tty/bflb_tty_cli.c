#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

#include "bflb_tty.h"

/*****************************************************************************
 * TTY CLI Commands
 *****************************************************************************/

/**
 * @brief Show TTY command help
 */
static void tty_show_help(void)
{
    printf("TTY CLI command help:\r\n");
    printf("  tty mtu <size>               - Test TTY MTU by sending data with specified size\r\n");
    printf("\r\n");
    printf("Examples:\r\n");
    printf("  tty mtu 64                   - Send 64 bytes to test TTY\r\n");
    printf("  tty mtu 512                  - Send 512 bytes to test TTY\r\n");
    printf("  tty mtu 1024                 - Send 1024 bytes (max MTU)\r\n");
}

/**
 * @brief TTY CLI main command handler
 * @param[in] argc Parameter count
 * @param[in] argv Parameter array
 * @retval 0 Success
 * @retval -1 Failure
 */
int cmd_tty(int argc, char **argv)
{
    if (argc < 2) {
        printf("[TTY_CLI] Error: Insufficient parameters\r\n");
        tty_show_help();
        return -1;
    }

    const char *subcmd = argv[1];

    if (strcmp(subcmd, "help") == 0) {
        tty_show_help();
        return 0;
    } else if (strcmp(subcmd, "mtu") == 0) {
        /* tty mtu <size> */
        if (argc < 3) {
            printf("[TTY_CLI] Error: MTU test requires size parameter\r\n");
            printf("Usage: tty mtu <size>\r\n");
            return -1;
        }

        uint32_t size = atoi(argv[2]);

        if (size == 0 || size > 1024) {
            printf("[TTY_CLI] Error: Invalid size (1<=size<=1024)\r\n");
            return -1;
        }

        printf("[TTY_MTU] Testing MTU: size=%u bytes\r\n", size);

        /* Allocate buffer */
        uint8_t *buffer = (uint8_t *)malloc(size);
        if (buffer == NULL) {
            printf("[TTY_MTU] Error: Failed to allocate buffer\r\n");
            return -1;
        }

        /* Fill buffer with test pattern */
        for (uint32_t i = 0; i < size; i++) {
            buffer[i] = i & 0xFF;
        }

        printf("[TTY_MTU] Sending %u bytes...\r\n", size);
        int ret = bflb_tty_upld_send(buffer, size);
        free(buffer);

        if (ret < 0) {
            printf("[TTY_MTU] Error: Failed to send data (ret=%d)\r\n", ret);
            return -1;
        }

        printf("[TTY_MTU] Successfully sent %d bytes\r\n", ret);
        printf("[TTY_MTU] Please check host if data was received correctly\r\n");
        return 0;

    } else {
        printf("[TTY_CLI] Error: Unknown subcommand '%s'\r\n", subcmd);
        tty_show_help();
        return -1;
    }
}

/* Register shell command */
SHELL_CMD_EXPORT_ALIAS(cmd_tty, tty, TTY communication commands);

