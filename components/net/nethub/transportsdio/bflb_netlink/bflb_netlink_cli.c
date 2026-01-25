#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

#include "bflb_netlink.h"
#include "bflb_netlink_protocol.h"

/* Netlink receive callback flag */
static bool g_netlink_recv_enabled = false;

#define CONFIG_NETHUB_NETLINK_TEST

/*****************************************************************************
 * Netlink CLI Commands
 *****************************************************************************/
/**
 * @brief Netlink data receive callback
 * @param[in] ctx Callback context (unused)
 * @param[in] data Received data buffer
 * @param[in] data_size Data size
 * @retval 0 Success
 */
static int netlink_recv_callback(void *ctx, uint8_t *data, uint32_t data_size)
{
    printf("[NETLINK_CLI] Received %u bytes from host: ", data_size);

    /* Print data as string if printable */
    bool is_printable = true;
    for (uint32_t i = 0; i < data_size; i++) {
        if (data[i] < 32 || data[i] > 126) {
            is_printable = false;
            break;
        }
    }

    if (is_printable) {
        printf("%.*s\r\n", (int)data_size, (char *)data);
    } else {
        /* Print hex data */
        printf("\r\n");
        for (uint32_t i = 0; i < data_size; i++) {
            printf("%02X ", data[i]);
            if ((i + 1) % 16 == 0) {
                printf("\r\n");
            }
        }
        if (data_size % 16 != 0) {
            printf("\r\n");
        }
    }

    return 0;
}

/**
 * @brief Show netlink command help
 */
static void netlink_show_help(void)
{
    printf("Netlink CLI command help:\r\n");
    printf("  netlink send [type] <string>  - Send ASCII string to host with optional type\r\n");
    printf("                                 Types: system, wifi, ble, user, log (default: user)\r\n");
    printf("  netlink recv                  - Register callback and receive data from host\r\n");
    printf("  netlink recv stop             - Unregister callback and stop receiving\r\n");
    printf("  netlink help                  - Show this help information\r\n");
#ifdef CONFIG_NETHUB_NETLINK_TEST
    printf("  netlink test flow <cnt> <sz>  - Flow control test (send count packets)\r\n");
    printf("  netlink test mtu <size>       - MTU test (send data with specified size)\r\n");
#endif
    printf("\r\n");
    printf("Examples:\r\n");
    printf("  netlink send Hello            - Send \"Hello\" to host (type=USER)\r\n");
    printf("  netlink send wifi \"scan\"      - Send \"scan\" with type=WIFI\r\n");
    printf("  netlink send ble \"connect\"    - Send \"connect\" with type=BLE\r\n");
    printf("  netlink send log \"error\"      - Send \"error\" with type=LOG\r\n");
    printf("  netlink send system \"reset\"   - Send \"reset\" with type=SYSTEM\r\n");
    printf("  netlink recv                   - Start receiving data from host\r\n");
    printf("  netlink recv stop              - Stop receiving data\r\n");
#ifdef CONFIG_NETHUB_NETLINK_TEST
    printf("  netlink test flow 1000 512    - Send 1000 packets of 512 bytes each\r\n");
    printf("  netlink test mtu %d         - Send * bytes to test MTU\r\n", NETLINK_FRAME_RAWSIZE);
#endif
}

/**
 * @brief Netlink CLI main command handler
 * @param[in] argc Parameter count
 * @param[in] argv Parameter array
 * @retval 0 Success
 * @retval -1 Failure
 */
int cmd_netlink(int argc, char **argv)
{
    if (argc < 2) {
        printf("[NETLINK_CLI] Error: Insufficient parameters\r\n");
        netlink_show_help();
        return -1;
    }

    const char *subcmd = argv[1];

    if (strcmp(subcmd, "help") == 0) {
        netlink_show_help();
        return 0;
    } else if (strcmp(subcmd, "send") == 0) {
        /* netlink send [type] <string> */
        if (argc < 3) {
            printf("[NETLINK_CLI] Error: send command requires at least 1 parameter\r\n");
            printf("Usage: netlink send [type] <string>\r\n");
            printf("Types: system, wifi, ble, user, log (default: user)\r\n");
            return -1;
        }

        const char *data = NULL;
        uint8_t data_type = BFLB_NETLINK_DATA_TYPE_USER;  /* Default USER type */

        /* Parse parameters: netlink send <string> or netlink send <type> <string> */
        if (argc == 3) {
            /* netlink send <string> or netlink send <type> */
            /* Check if second parameter is type or data */
            if (strcmp(argv[2], "system") == 0 ||
                strcmp(argv[2], "wifi") == 0 ||
                strcmp(argv[2], "ble") == 0 ||
                strcmp(argv[2], "user") == 0 ||
                strcmp(argv[2], "log") == 0) {
                /* Only type, no data specified */
                printf("[NETLINK_CLI] Error: No data specified\r\n");
                printf("Usage: netlink send [type] <string>\r\n");
                return -1;
            } else {
                /* Second parameter is data */
                data = argv[2];
            }
        } else {
            /* argc >= 4: netlink send [type] <string with spaces> */
            /* Check if second parameter is a type keyword */
            const char *type_str = argv[2];
            bool is_type = false;

            if (strcmp(type_str, "system") == 0 ||
                strcmp(type_str, "wifi") == 0 ||
                strcmp(type_str, "ble") == 0 ||
                strcmp(type_str, "user") == 0 ||
                strcmp(type_str, "log") == 0) {
                /* Parse data type */
                is_type = true;
                if (strcmp(type_str, "system") == 0) {
                    data_type = BFLB_NETLINK_DATA_TYPE_SYSTEM;
                } else if (strcmp(type_str, "wifi") == 0) {
                    data_type = BFLB_NETLINK_DATA_TYPE_WIFI;
                } else if (strcmp(type_str, "ble") == 0) {
                    data_type = BFLB_NETLINK_DATA_TYPE_BLE;
                } else if (strcmp(type_str, "user") == 0) {
                    data_type = BFLB_NETLINK_DATA_TYPE_USER;
                } else if (strcmp(type_str, "log") == 0) {
                    data_type = BFLB_NETLINK_DATA_TYPE_LOG;
                }
            }

            if (is_type) {
                /* netlink send <type> <string with spaces> */
                if (argc < 4) {
                    printf("[NETLINK_CLI] Error: No data specified\r\n");
                    printf("Usage: netlink send [type] <string>\r\n");
                    return -1;
                }
                /* Reconstruct the data string from argv[3] onwards */
                size_t total_len = 0;
                for (int i = 3; i < argc; i++) {
                    total_len += strlen(argv[i]) + 1;  /* +1 for space */
                }
                if (total_len > 0) {
                    char *data_buf = (char *)malloc(total_len);
                    if (data_buf == NULL) {
                        printf("[NETLINK_CLI] Error: Failed to allocate memory\r\n");
                        return -1;
                    }
                    data_buf[0] = '\0';
                    for (int i = 3; i < argc; i++) {
                        if (i > 3) {
                            strcat(data_buf, " ");
                        }
                        strcat(data_buf, argv[i]);
                    }
                    /* Use static buffer to avoid needing free */
                    static char g_send_buffer[NETLINK_FRAME_RAWSIZE];
                    strncpy(g_send_buffer, data_buf, sizeof(g_send_buffer) - 1);
                    g_send_buffer[sizeof(g_send_buffer) - 1] = '\0';
                    free(data_buf);
                    data = g_send_buffer;
                }
            } else {
                /* netlink send <string with spaces> (no type specified) */
                /* Reconstruct the data string from argv[2] onwards */
                size_t total_len = 0;
                for (int i = 2; i < argc; i++) {
                    total_len += strlen(argv[i]) + 1;  /* +1 for space */
                }
                if (total_len > 0) {
                    char *data_buf = (char *)malloc(total_len);
                    if (data_buf == NULL) {
                        printf("[NETLINK_CLI] Error: Failed to allocate memory\r\n");
                        return -1;
                    }
                    data_buf[0] = '\0';
                    for (int i = 2; i < argc; i++) {
                        if (i > 2) {
                            strcat(data_buf, " ");
                        }
                        strcat(data_buf, argv[i]);
                    }
                    /* Use static buffer to avoid needing free */
                    static char g_send_buffer[NETLINK_FRAME_RAWSIZE];
                    strncpy(g_send_buffer, data_buf, sizeof(g_send_buffer) - 1);
                    g_send_buffer[sizeof(g_send_buffer) - 1] = '\0';
                    free(data_buf);
                    data = g_send_buffer;
                }
            }
        }

        if (data == NULL || strlen(data) == 0) {
            printf("[NETLINK_CLI] Error: No data specified\r\n");
            return -1;
        }

        size_t data_len = strlen(data);
        printf("[NETLINK_CLI] Sending to host: %s (len=%zu, type=%s)\r\n",
               data, data_len, bflb_netlink_data_type_to_str(data_type));

        int ret = bflb_netlink_upld_send(data_type, (uint8_t *)data, data_len);
        if (ret < 0) {
            printf("[NETLINK_CLI] Error: Failed to send data (ret=%d)\r\n", ret);
            return -1;
        }

        printf("[NETLINK_CLI] Sent %d bytes successfully (type=%s)\r\n",
               ret, bflb_netlink_data_type_to_str(data_type));
        return 0;
    } else if (strcmp(subcmd, "recv") == 0) {
        /* netlink recv [stop] */
        if (argc == 2) {
            /* Register callback for all data types */
            if (g_netlink_recv_enabled) {
                printf("[NETLINK_CLI] Warning: Receive callback already registered\r\n");
                return 0;
            }

            /* Data type registration table */
            const struct {
                uint8_t type;
                const char *name;
            } data_types[] = {
                {BFLB_NETLINK_DATA_TYPE_SYSTEM, "SYSTEM"},
                {BFLB_NETLINK_DATA_TYPE_WIFI, "WIFI"},
                {BFLB_NETLINK_DATA_TYPE_BLE, "BLE"},
                {BFLB_NETLINK_DATA_TYPE_USER, "USER"},
                {BFLB_NETLINK_DATA_TYPE_LOG, "LOG"},
            };

            /* Register callbacks for all data types */
            for (size_t i = 0; i < sizeof(data_types) / sizeof(data_types[0]); i++) {
                int ret = bflb_netlink_dnld_register(data_types[i].type, netlink_recv_callback, NULL);
                if (ret < 0) {
                    printf("[NETLINK_CLI] Error: Failed to register %s callback (ret=%d)\r\n",
                           data_types[i].name, ret);
                    return -1;
                }
            }

            g_netlink_recv_enabled = true;
            printf("[NETLINK_CLI] Receive callbacks registered for all types, waiting for data...\r\n");
            printf("[NETLINK_CLI] Use 'netlink recv stop' to stop receiving\r\n");
            return 0;
        } else if (argc == 3 && strcmp(argv[2], "stop") == 0) {
            /* Unregister all callbacks */
            if (!g_netlink_recv_enabled) {
                printf("[NETLINK_CLI] Warning: Receive callback not registered\r\n");
                return 0;
            }

            /* Unregister callbacks for all data types */
            bflb_netlink_dnld_register(BFLB_NETLINK_DATA_TYPE_SYSTEM, NULL, NULL);
            bflb_netlink_dnld_register(BFLB_NETLINK_DATA_TYPE_WIFI, NULL, NULL);
            bflb_netlink_dnld_register(BFLB_NETLINK_DATA_TYPE_BLE, NULL, NULL);
            bflb_netlink_dnld_register(BFLB_NETLINK_DATA_TYPE_USER, NULL, NULL);
            bflb_netlink_dnld_register(BFLB_NETLINK_DATA_TYPE_LOG, NULL, NULL);

            g_netlink_recv_enabled = false;
            printf("[NETLINK_CLI] Receive callbacks unregistered\r\n");
            return 0;
        } else {
            printf("[NETLINK_CLI] Error: Invalid recv command parameters\r\n");
            printf("Usage: netlink recv [stop]\r\n");
            return -1;
        }
    } else if (strcmp(subcmd, "test") == 0) {
#ifdef CONFIG_NETHUB_NETLINK_TEST
        /* netlink test <test_type> [args...] */
        if (argc < 3) {
            printf("[NETLINK_CLI] Error: test command requires test type\r\n");
            printf("Usage: netlink test flow <count> <size>\r\n");
            printf("       netlink test mtu <size>\r\n");
            return -1;
        }

        const char *test_type = argv[2];

        if (strcmp(test_type, "flow") == 0) {
            /* netlink test flow <count> <size> */
            if (argc < 5) {
                printf("[NETLINK_CLI] Error: flow test requires count and size\r\n");
                printf("Usage: netlink test flow <count> <size>\r\n");
                return -1;
            }

            uint32_t count = atoi(argv[3]);
            uint32_t size = atoi(argv[4]);

            if (count == 0 || size == 0 || size > NETLINK_FRAME_RAWSIZE) {
                printf("[NETLINK_CLI] Error: Invalid count or size (count>0, 1<=size<=%d)\r\n", NETLINK_FRAME_RAWSIZE);
                return -1;
            }

            printf("[FLOW_TEST] Starting flow control test: count=%u, size=%u\r\n", count, size);

            /* Send packets with sequence numbers */
            uint8_t *buffer = (uint8_t *)malloc(size);
            if (buffer == NULL) {
                printf("[FLOW_TEST] Error: Failed to allocate buffer\r\n");
                return -1;
            }

            printf("[FLOW_TEST] Sending %u packets of %u bytes each...\r\n", count, size);

            for (uint32_t i = 0; i < count; i++) {
                /* Build packet with sequence number */
                buffer[0] = (i >> 24) & 0xFF;
                buffer[1] = (i >> 16) & 0xFF;
                buffer[2] = (i >> 8) & 0xFF;
                buffer[3] = i & 0xFF;

                /* Fill remaining data with pattern */
                for (uint32_t j = 4; j < size; j++) {
                    buffer[j] = i & 0xFF;
                }

                int ret = bflb_netlink_upld_send(BFLB_NETLINK_DATA_TYPE_USER, buffer, size);
                if (ret < 0) {
                    printf("[FLOW_TEST] Warning: Failed to send packet %u (ret=%d)\r\n", i, ret);
                }

                /* Small delay to avoid overwhelming */
                if (i % 10 == 0) {
                    vTaskDelay(1);
                }
            }

            free(buffer);
            printf("[FLOW_TEST] Sent all %u packets\r\n", count);
            printf("[FLOW_TEST] Waiting for responses... (check device output)\r\n");
            return 0;

        } else if (strcmp(test_type, "mtu") == 0) {
            /* netlink test mtu <size> */
            if (argc < 4) {
                printf("[NETLINK_CLI] Error: MTU test requires size parameter\r\n");
                printf("Usage: netlink test mtu <size>\r\n");
                return -1;
            }

            uint32_t size = atoi(argv[3]);

            if (size == 0 || size > NETLINK_FRAME_RAWSIZE) {
                printf("[NETLINK_CLI] Error: Invalid size (1<=size<=%d)\r\n", NETLINK_FRAME_RAWSIZE);
                return -1;
            }

            printf("[MTU_TEST] Testing MTU: size=%u bytes\r\n", size);

            /* Allocate buffer */
            uint8_t *buffer = (uint8_t *)malloc(size);
            if (buffer == NULL) {
                printf("[MTU_TEST] Error: Failed to allocate buffer\r\n");
                return -1;
            }

            /* Fill buffer with test pattern */
            for (uint32_t i = 0; i < size; i++) {
                buffer[i] = i & 0xFF;
            }

            printf("[MTU_TEST] Sending %u bytes...\r\n", size);
            int ret = bflb_netlink_upld_send(BFLB_NETLINK_DATA_TYPE_USER, buffer, size);
            free(buffer);

            if (ret < 0) {
                printf("[MTU_TEST] Error: Failed to send data (ret=%d)\r\n", ret);
                return -1;
            }

            printf("[MTU_TEST] Successfully sent %d bytes\r\n", ret);
            printf("[MTU_TEST] Please check host if data was received correctly\r\n");
            return 0;

        } else {
            printf("[NETLINK_CLI] Error: Unknown test type '%s'\r\n", test_type);
            printf("Available tests: flow, mtu\r\n");
            return -1;
        }
#else
        printf("[NETLINK_CLI] Error: Test commands not enabled (CONFIG_NETHUB_NETLINK_TEST not set)\r\n");
        return -1;
#endif
    } else {
        printf("[NETLINK_CLI] Error: Unknown subcommand '%s'\r\n", subcmd);
        netlink_show_help();
        return -1;
    }
}

/* Register shell command */
SHELL_CMD_EXPORT_ALIAS(cmd_netlink, netlink, Netlink communication commands);

