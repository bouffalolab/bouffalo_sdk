/**
 * @file app_vchan.c
 * @brief NetHub Virtual Channel demo code for user
 * @details Demonstrates usage of nethub_vchan_user_send and nethub_vchan_user_recv_register
 */

#ifdef CONFIG_MR_VIRTUALCHAN

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include "shell.h"

#define DBG_TAG "VCHAN"
#include "log.h"

#include "nethub_vchan.h"

/* Maximum data length (must match NETHUB_VCHAN_MAX_DATA_LEN) */
#define VCHAN_MAX_DATA_LEN   (1500)

/* Receive callback registration status flag (only 1 byte static memory) */
static uint8_t g_recv_callback_registered = 0;

/* Test statistics */
static struct {
    uint32_t rx_count;       /* receive count */
    uint32_t rx_bytes;       /* receive bytes */
    uint32_t tx_count;       /* transmit count */
    uint32_t tx_bytes;       /* transmit bytes */
} g_test_stats = {0};

/**
 * @brief Receive callback function
 * @param arg callback parameter (unused)
 * @param data_buff receive data buffer
 * @param data_size data length
 * @return 0 for success
 */
static int vchan_recv_callback(void *arg, uint8_t *data_buff, uint16_t data_size)
{
    (void)arg;

    g_test_stats.rx_count++;
    g_test_stats.rx_bytes += data_size;

    LOG_I("[VCHAN_RECV] %u bytes | cnt:%u\r\n", data_size, g_test_stats.rx_count);

    /* Print first 32 bytes in hex format */
    uint16_t print_len = (data_size > 32) ? 32 : data_size;
    DBG_HEXDUMP(data_buff, print_len);

    if (data_size > 32) {
        LOG_I("... (total %u bytes)\r\n", data_size);
    }

    return 0;
}

/**
 * @brief Parse hex string
 * @param hex_str hex string (e.g. "0102030405")
 * @param out_buf output buffer
 * @param out_max_len maximum length of output buffer
 * @return number of bytes parsed successfully, <0 means failure
 */
static int parse_hex_string(const char *hex_str, uint8_t *out_buf, uint16_t out_max_len)
{
    uint16_t len = 0;
    const char *p = hex_str;

    if (!hex_str || !out_buf || out_max_len == 0) {
        return -1;
    }

    while (*p != '\0') {
        /* Skip spaces */
        if (*p == ' ') {
            p++;
            continue;
        }

        /* Check if there are enough characters */
        if (*(p + 1) == '\0') {
            LOG_E("Invalid hex string (odd length)\r\n");
            return -1;
        }

        /* Parse two hex characters as one byte */
        uint8_t high = 0, low = 0;

        if (*p >= '0' && *p <= '9') {
            high = *p - '0';
        } else if (*p >= 'a' && *p <= 'f') {
            high = *p - 'a' + 10;
        } else if (*p >= 'A' && *p <= 'F') {
            high = *p - 'A' + 10;
        } else {
            LOG_E("Invalid hex char: %c\r\n", *p);
            return -1;
        }

        p++;

        if (*p >= '0' && *p <= '9') {
            low = *p - '0';
        } else if (*p >= 'a' && *p <= 'f') {
            low = *p - 'a' + 10;
        } else if (*p >= 'A' && *p <= 'F') {
            low = *p - 'A' + 10;
        } else {
            LOG_E("Invalid hex char: %c\r\n", *p);
            return -1;
        }

        p++;

        out_buf[len] = (high << 4) | low;
        len++;

        /* Check if buffer is exceeded */
        if (len >= out_max_len) {
            LOG_W("Hex data truncated (max %u bytes)\r\n", out_max_len);
            break;
        }
    }

    return len;
}

/**
 * @brief Show help information
 */
static void vchan_show_help(void)
{
    LOG_I("=== NetHub Virtual Channel User Commands ===\r\n");
    LOG_I("Usage:\r\n");
    LOG_I("  vchanuser send <data>             - Send ASCII data\r\n");
    LOG_I("  vchanuser send hex <hex_data>     - Send hex data\r\n");
    LOG_I("  vchanuser recv on                 - Register receive callback\r\n");
    LOG_I("  vchanuser recv off                - Unregister receive callback\r\n");
    LOG_I("  vchanuser test flow <cnt> <sz>    - Flow test\r\n");
    LOG_I("  vchanuser test mtu <size>         - MTU test\r\n");
    LOG_I("  vchanuser stats                   - Show statistics\r\n");
    LOG_I("  vchanuser help                    - Show this help\r\n");
    LOG_I("\r\n");
    LOG_I("Examples:\r\n");
    LOG_I("  vchanuser send hello              - Send ASCII string\r\n");
    LOG_I("  vchanuser send hex 0102030405     - Send hex data\r\n");
    LOG_I("  vchanuser recv on                 - Enable receive\r\n");
    LOG_I("  vchanuser test flow 100 512       - Send 100 packets of 512 bytes each\r\n");
    LOG_I("  vchanuser test mtu 1024           - Test 1024 bytes MTU\r\n");
    LOG_I("========================================\r\n");
}

/**
 * @brief vchan send subcommand
 * @param argc argument count
 * @param argv argument array (argv[0]="send", argv[1...] is data)
 * @return 0 for success
 */
static int vchan_do_send(int argc, char **argv)
{
    uint8_t data_buf[VCHAN_MAX_DATA_LEN];
    uint16_t data_len = 0;
    int ret;
    int is_hex = 0;

    /* Check arguments */
    if (argc < 1) {
        LOG_E("Usage: vchanuser send <data> OR vchanuser send hex <hex_data>\r\n");
        return -1;
    }

    /* Check if it is hex mode */
    if (argc >= 2 && strcmp(argv[0], "hex") == 0) {
        is_hex = 1;
        ret = parse_hex_string(argv[1], data_buf, sizeof(data_buf));
        if (ret < 0) {
            LOG_E("Failed to parse hex string\r\n");
            return -1;
        }
        data_len = ret;
    } else {
        /* ASCII mode: concatenate all arguments into one string */
        for (int i = 0; i < argc; i++) {
            uint16_t arg_len = strlen(argv[i]);
            if (data_len + arg_len >= sizeof(data_buf)) {
                LOG_W("Data too long, truncated\r\n");
                break;
            }
            memcpy(data_buf + data_len, argv[i], arg_len);
            data_len += arg_len;
            /* Add space between arguments (except the last one) */
            if (i < argc - 1) {
                data_buf[data_len++] = ' ';
            }
        }
    }

    /* Send data */
    ret = nethub_vchan_user_send(data_buf, data_len);
    if (ret < 0) {
        LOG_E("Send failed: %d\r\n", ret);
        return -1;
    }

    g_test_stats.tx_count++;
    g_test_stats.tx_bytes += data_len;

    LOG_I("[VCHAN_TX] Sent %u bytes (mode:%s)\r\n", data_len, is_hex ? "HEX" : "ASCII");

    return 0;
}

/**
 * @brief vchan recv subcommand
 * @param argc argument count
 * @param argv argument array (argv[0]="on"|"off")
 * @return 0 for success
 */
static int vchan_do_recv(int argc, char **argv)
{
    /* Check arguments */
    if (argc < 1) {
        LOG_E("Usage: vchanuser recv <on|off>\r\n");
        return -1;
    }

    if (strcmp(argv[0], "on") == 0) {
        /* Register receive callback */
        if (g_recv_callback_registered) {
            LOG_W("Recv callback already registered\r\n");
            return 0;
        }

        int ret = nethub_vchan_user_recv_register(vchan_recv_callback, NULL);
        if (ret < 0) {
            LOG_E("Failed to register recv callback: %d\r\n", ret);
            return -1;
        }

        g_recv_callback_registered = 1;
        LOG_I("Recv callback registered successfully\r\n");

    } else if (strcmp(argv[0], "off") == 0) {
        /* Unregister receive callback */
        if (!g_recv_callback_registered) {
            LOG_W("Recv callback not registered\r\n");
            return 0;
        }

        int ret = nethub_vchan_user_recv_register(NULL, NULL);
        if (ret < 0) {
            LOG_E("Failed to unregister recv callback: %d\r\n", ret);
            return -1;
        }

        g_recv_callback_registered = 0;
        LOG_I("Recv callback unregistered\r\n");

    } else {
        LOG_E("Invalid argument: %s (use 'on' or 'off')\r\n", argv[0]);
        return -1;
    }

    return 0;
}

/**
 * @brief vchan test subcommand
 * @param argc argument count
 * @param argv argument array
 * @return 0 for success
 */
static int vchan_do_test(int argc, char **argv)
{
    uint8_t *data_buf;
    uint32_t count, size, i;
    int ret;

    /* Check arguments */
    if (argc < 1) {
        LOG_E("Usage:\r\n");
        LOG_E("  vchanuser test flow <count> <size>  - Flow test\r\n");
        LOG_E("  vchanuser test mtu <size>           - MTU test\r\n");
        return -1;
    }

    /* Flow test */
    if (strcmp(argv[0], "flow") == 0) {
        if (argc < 3) {
            LOG_E("Usage: vchanuser test flow <count> <size>\r\n");
            return -1;
        }

        count = atoi(argv[1]);
        size = atoi(argv[2]);

        if (count == 0 || size == 0) {
            LOG_E("Invalid count or size\r\n");
            return -1;
        }

        if (size > VCHAN_MAX_DATA_LEN) {
            LOG_E("Size too large (max %d)\r\n", VCHAN_MAX_DATA_LEN);
            return -1;
        }

        /* Auto register receive callback */
        if (!g_recv_callback_registered) {
            ret = nethub_vchan_user_recv_register(vchan_recv_callback, NULL);
            if (ret < 0) {
                LOG_E("Failed to register recv callback: %d\r\n", ret);
                return -1;
            }
            g_recv_callback_registered = 1;
            LOG_I("Auto registered recv callback for flow test\r\n");
        }

        /* Clear statistics */
        memset(&g_test_stats, 0, sizeof(g_test_stats));

        LOG_I("=== Flow Test: count=%u, size=%u ===\r\n", count, size);

        /* Allocate temporary buffer */
        data_buf = (uint8_t *)pvPortMalloc(size);
        if (data_buf == NULL) {
            LOG_E("Failed to allocate buffer\r\n");
            return -1;
        }

        /* Send data packets */
        uint32_t success = 0;
        uint32_t failed = 0;

        for (i = 0; i < count; i++) {
            /* Fill test data: first 4 bytes are sequence number, rest is pattern */
            data_buf[0] = (i >> 24) & 0xFF;
            data_buf[1] = (i >> 16) & 0xFF;
            data_buf[2] = (i >> 8) & 0xFF;
            data_buf[3] = i & 0xFF;

            for (uint32_t j = 4; j < size; j++) {
                data_buf[j] = (uint8_t)(i & 0xFF);
            }

            ret = nethub_vchan_user_send(data_buf, (uint16_t)size);
            if (ret < 0) {
                failed++;
                if (failed <= 10) {
                    LOG_E("Send failed at packet %u: %d\r\n", i, ret);
                }
            } else {
                success++;
                g_test_stats.tx_count++;
                g_test_stats.tx_bytes += size;
            }

            /* Show progress every 100 packets */
            if ((i + 1) % 100 == 0) {
                LOG_I("Progress: %u/%u (%.1f%%)\r\n", i + 1, count, (i + 1) * 100.0 / count);
            }

            /* Small delay to avoid congestion */
            if ((i + 1) % 10 == 0) {
                vTaskDelay(pdMS_TO_TICKS(1));
            }
        }

        vPortFree(data_buf);

        /* Wait for RX to complete */
        LOG_I("Waiting for RX to complete...\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));

        /* Show statistics */
        LOG_I("=== Flow Test Result ===\r\n");
        LOG_I("TX: %u packets, %u bytes\r\n", success, success * size);
        LOG_I("RX: %u packets, %u bytes\r\n", g_test_stats.rx_count, g_test_stats.rx_bytes);
        LOG_I("Success rate: %.2f%%\r\n", success * 100.0 / count);
        LOG_I("========================\r\n");

    } else if (strcmp(argv[0], "mtu") == 0) {
        /* MTU test */
        if (argc < 2) {
            LOG_E("Usage: vchanuser test mtu <size>\r\n");
            return -1;
        }

        size = atoi(argv[1]);

        if (size == 0 || size > VCHAN_MAX_DATA_LEN) {
            LOG_E("Invalid size (1 ~ %d)\r\n", VCHAN_MAX_DATA_LEN);
            return -1;
        }

        LOG_I("=== MTU Test: size=%u ===\r\n", size);

        /* Auto register receive callback */
        if (!g_recv_callback_registered) {
            ret = nethub_vchan_user_recv_register(vchan_recv_callback, NULL);
            if (ret < 0) {
                LOG_E("Failed to register recv callback: %d\r\n", ret);
                return -1;
            }
            g_recv_callback_registered = 1;
            LOG_I("Auto registered recv callback for MTU test\r\n");
        }

        /* Allocate and fill test data */
        data_buf = (uint8_t *)pvPortMalloc(size);
        if (data_buf == NULL) {
            LOG_E("Failed to allocate buffer\r\n");
            return -1;
        }

        /* Fill with incrementing pattern */
        for (i = 0; i < size; i++) {
            data_buf[i] = (uint8_t)(i & 0xFF);
        }

        /* Send */
        ret = nethub_vchan_user_send(data_buf, (uint16_t)size);
        vPortFree(data_buf);

        if (ret < 0) {
            LOG_E("MTU test failed: %d\r\n", ret);
            return -1;
        }

        LOG_I("MTU test: Sent %u bytes successfully\r\n", size);
        LOG_I("Please check if data was received correctly\r\n");

    } else {
        LOG_E("Invalid test type: %s (use 'flow' or 'mtu')\r\n", argv[0]);
        return -1;
    }

    return 0;
}

/**
 * @brief vchan stats subcommand
 * @param argc argument count
 * @param argv argument array
 * @return 0 for success
 */
static int vchan_do_stats(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    LOG_I("=== Virtual Channel Statistics ===\r\n");
    LOG_I("Recv callback: %s\r\n", g_recv_callback_registered ? "Registered" : "Unregistered");
    LOG_I("TX: %u packets, %u bytes\r\n", g_test_stats.tx_count, g_test_stats.tx_bytes);
    LOG_I("RX: %u packets, %u bytes\r\n", g_test_stats.rx_count, g_test_stats.rx_bytes);
    LOG_I("==================================\r\n");

    return 0;
}

/**
 * @brief vchan help subcommand
 * @param argc argument count
 * @param argv argument array
 * @return 0 for success
 */
static int vchan_do_help(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    vchan_show_help();
    return 0;
}

/**
 * @brief vchanuser main command entry
 * @param argc argument count (including command name)
 * @param argv argument array (argv[0]="vchanuser", argv[1]=subcommand, argv[2...]=subcommand args)
 * @return 0 for success
 */
int cmd_vchanuser(int argc, char **argv)
{
    /* Check arguments */
    if (argc < 2) {
        vchan_show_help();
        return 0;
    }

    /* Parse subcommand */
    char *subcmd = argv[1];

    /* Adjust arguments: remove command name and subcommand */
    int sub_argc = argc - 2;
    char **sub_argv = &argv[2];

    if (strcmp(subcmd, "send") == 0) {
        return vchan_do_send(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "recv") == 0) {
        return vchan_do_recv(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "test") == 0) {
        return vchan_do_test(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "stats") == 0) {
        return vchan_do_stats(sub_argc, sub_argv);
    } else if (strcmp(subcmd, "help") == 0) {
        return vchan_do_help(sub_argc, sub_argv);
    } else {
        LOG_E("Unknown subcommand: %s\r\n", subcmd);
        LOG_I("Type 'vchanuser help' for usage\r\n");
        return -1;
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_vchanuser, vchanuser, nethub virtual channel user command);

/**
 * @brief Initialize Virtual Channel application
 * @note This function does not register any callback, only prepares for operation
 *       Actual callback registration is triggered by vchan recv on command
 */
void app_vchan_init(void)
{
    LOG_I("NetHub Virtual Channel Demo Initialized\r\n");
    LOG_I("Type 'vchanuser help' for available commands\r\n");

    /* Do not register any callback by default, triggered by user command */
    g_recv_callback_registered = 0;
    memset(&g_test_stats, 0, sizeof(g_test_stats));
}

#endif /* CONFIG_MR_VIRTUALCHAN */
