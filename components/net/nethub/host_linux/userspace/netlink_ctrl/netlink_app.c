/**
 * @file netlink_app.c
 * @brief Netlink Userspace Application
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include "netlink_ctrl.h"
#include "netlink_protocol.h"

#define VERSION "1.0"

/* Global variables */
static netlink_handle_t g_handle = NULL;
static pthread_t g_recv_thread;
static volatile int g_running = 1;

/**
 * @brief Receive callback function
 */
static void recv_callback(const void *data, size_t len)
{
    const unsigned char *p = (const unsigned char *)data;
    size_t i;
    int is_printable;

    if (!data || len == 0) {
        printf("[Received] Empty data\n");
        return;
    }

    printf("[Received] %zu bytes: ", len);

    /* Check if all characters are printable */
    is_printable = 1;
    for (i = 0; i < len; i++) {
        if (p[i] < 32 || p[i] > 126) {
            is_printable = 0;
            break;
        }
    }

    if (is_printable) {
        /* Print as string with length limit to ensure null-termination */
        printf("%.*s\n", (int)len, (const char *)data);
    } else {
        /* Print hex data */
        printf("\n");
        for (i = 0; i < len; i++) {
            printf("%02X ", p[i]);
            if ((i + 1) % 16 == 0) {
                printf("\n");
            }
        }
        if (len % 16 != 0) {
            printf("\n");
        }
    }

    fflush(stdout);
}

/**
 * @brief Receive thread
 */
static void *recv_thread_func(void *arg)
{
    netlink_handle_t handle = (netlink_handle_t)arg;

    if (!handle) {
        fprintf(stderr, "Error: Invalid handle in recv thread\n");
        return NULL;
    }

    while (g_running) {
        /* Use simple recv, don't call netlink_recv_thread as it's blocking */
        fd_set readfds;
        struct timeval tv;
        int fd = netlink_get_fd(handle);
        int ret;

        if (fd < 0) {
            fprintf(stderr, "Error: Invalid file descriptor\n");
            break;
        }

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        /* 100ms timeout to check g_running flag */
        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        ret = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        if (ret > 0 && FD_ISSET(fd, &readfds)) {
            char buffer[2048];
            struct nlmsghdr *nlh;
            struct netlink_data_hdr *data_hdr;
            ssize_t len;

            len = recv(fd, buffer, sizeof(buffer), 0);
            if (len < 0) {
                if (errno == EINTR) {
                    continue;
                }
                break;
            }

            if (len == 0) {
                /* Connection closed */
                break;
            }

            if ((size_t)len >= NLMSG_HDRLEN) {
                nlh = (struct nlmsghdr *)buffer;

                /* Validate nlmsg_len to prevent integer underflow (32/64-bit safe) */
                if (nlh->nlmsg_len < NLMSG_HDRLEN || nlh->nlmsg_len > (size_t)len) {
                    fprintf(stderr, "Error: Invalid netlink message length (len=%u, hdr=%u)\n",
                            (unsigned int)len, nlh->nlmsg_len);
                    continue;
                }

                void *netlink_data = NLMSG_DATA(nlh);
                size_t netlink_data_len = nlh->nlmsg_len - NLMSG_HDRLEN;

                /* Parse data_type header */
                if (netlink_data_len >= NETLINK_DATA_HDR_LEN) {
                    data_hdr = (struct netlink_data_hdr *)netlink_data;
                    void *user_data = data_hdr->data;
                    size_t user_data_len = netlink_data_len - NETLINK_DATA_HDR_LEN;

                    if (user_data_len > 0) {
                        /* Print type information */
                        printf("[Received] %zu bytes (type=%s): ",
                               user_data_len, netlink_data_type_to_str(data_hdr->data_type));

                        /* Invoke callback */
                        recv_callback(user_data, user_data_len);
                    }
                }
            }
        }
    }

    return NULL;
}

/**
 * @brief Show help information
 */
static void show_help(void)
{
    printf("Available commands:\n");
    printf("  send [type] <string>   Send data with optional type\n");
    printf("                         Types: system, wifi, ble, user, log\n");
    printf("  test flow <cnt> <sz>   Flow control test (send count packets)\n");
    printf("  test mtu <size>        MTU test (send data with specified size)\n");
    printf("  status                 Show connection status\n");
    printf("  help                   Show this help message\n");
    printf("  quit/exit              Exit program\n");
    printf("\n");
    printf("Examples:\n");
    printf("  send user 12345678     Send data with type USER\n");
    printf("  test flow 1000 512     Send 1000 packets of 512 bytes each\n");
    printf("  test mtu 1024          Send 1024 bytes to test MTU\n");
}

/**
 * @brief Read sysfs statistic value
 */
static int read_sysfs_stat(const char *filename)
{
    FILE *fp;
    int value = -1;
    char path[128];

    snprintf(path, sizeof(path), "/sys/class/hd_netlink/netlink0/%s", filename);
    fp = fopen(path, "r");
    if (fp) {
        if (fscanf(fp, "%d", &value) == 1) {
            fclose(fp);
            return value;
        }
        fclose(fp);
    }
    return -1;
}

/**
 * @brief Show status
 */
static void show_status(void)
{
    int tx_packets, tx_dropped, rx_packets, rx_dropped, pending_queue;
    int nl_rx_packets, nl_tx_packets, nl_tx_dropped;

    /* Read kernel ↔ device statistics from sysfs */
    tx_packets = read_sysfs_stat("tx_packets");
    tx_dropped = read_sysfs_stat("tx_dropped");
    rx_packets = read_sysfs_stat("rx_packets");
    rx_dropped = read_sysfs_stat("rx_dropped");
    pending_queue = read_sysfs_stat("pending_queue");

    /* Read kernel ↔ userspace statistics from sysfs */
    nl_rx_packets = read_sysfs_stat("nl_rx_packets");
    nl_tx_packets = read_sysfs_stat("nl_tx_packets");
    nl_tx_dropped = read_sysfs_stat("nl_tx_dropped");

    printf("Connection: Active\n");
    printf("\n");
    printf("[Kernel ↔ Device via SDIO/USB]\n");
    printf("  TX: %d packets (dropped: %d)\n",
           tx_packets >= 0 ? tx_packets : 0,
           tx_dropped >= 0 ? tx_dropped : 0);
    printf("  RX: %d packets (dropped: %d)\n",
           rx_packets >= 0 ? rx_packets : 0,
           rx_dropped >= 0 ? rx_dropped : 0);
    printf("  Pending Queue: %d packets\n",
           pending_queue >= 0 ? pending_queue : 0);
    printf("\n");
    printf("[Kernel ↔ Userspace via Netlink]\n");
    printf("  RX from userspace: %d packets\n",
           nl_rx_packets >= 0 ? nl_rx_packets : 0);
    printf("  TX to userspace: %d packets (dropped: %d)\n",
           nl_tx_packets >= 0 ? nl_tx_packets : 0,
           nl_tx_dropped >= 0 ? nl_tx_dropped : 0);
}

/**
 * @brief Handle send command
 * @note Supported syntax: send [type] <string>
 *       Type options: system, wifi, ble, user, log (default: user)
 */
static void handle_send(char *args)
{
    char *data = args;
    char *type_str = NULL;
    size_t len;
    uint8_t data_type = NETLINK_DATA_TYPE_USER;  /* Default to user type */

    if (!args) {
        printf("Usage: send [type] <string>\n");
        printf("Types: system, wifi, ble, user, log (default: user)\n");
        return;
    }

    /* Skip spaces */
    while (*data == ' ') {
        data++;
    }

    /* Check if type is specified: send [type] <string> */
    char *space = strchr(data, ' ');
    if (space != NULL) {
        *space = '\0';
        type_str = data;
        data = space + 1;

        /* Parse type */
        if (strcmp(type_str, "system") == 0) {
            data_type = NETLINK_DATA_TYPE_SYSTEM;
        } else if (strcmp(type_str, "wifi") == 0) {
            data_type = NETLINK_DATA_TYPE_WIFI;
        } else if (strcmp(type_str, "ble") == 0) {
            data_type = NETLINK_DATA_TYPE_BLE;
        } else if (strcmp(type_str, "user") == 0) {
            data_type = NETLINK_DATA_TYPE_USER;
        } else if (strcmp(type_str, "log") == 0) {
            data_type = NETLINK_DATA_TYPE_LOG;
        } else {
            /* Not a type keyword, restore as data */
            data = type_str;
            data_type = NETLINK_DATA_TYPE_USER;
        }
    }

    len = strlen(data);
    if (len == 0) {
        printf("Usage: send [type] <string>\n");
        printf("Types: system, wifi, ble, user, log (default: user)\n");
        return;
    }

    /* Call new send interface */
    int ret = netlink_send(g_handle, data_type, data, len);
    if (ret > 0) {
        printf("Sent %d bytes (type=%s)\n", ret,
               netlink_data_type_to_str(data_type));
    } else {
        printf("Send failed (ret=%d)\n", ret);
    }
}

/**
 * @brief Handle test command
 */
static void handle_test(char *args)
{
    char *test_type;
    uint32_t count, size;
    uint8_t *buffer;
    uint32_t i;

    if (!args) {
        printf("Usage: test flow <count> <size>\n");
        printf("       test mtu <size>\n");
        return;
    }

    /* Parse test type */
    test_type = strtok(args, " ");

    if (!test_type) {
        printf("Usage: test flow <count> <size>\n");
        printf("       test mtu <size>\n");
        return;
    }

    if (strcmp(test_type, "flow") == 0) {
        /* test flow <count> <size> */
        char *count_str = strtok(NULL, " ");
        char *size_str = strtok(NULL, " ");

        if (!count_str || !size_str) {
            printf("Usage: test flow <count> <size>\n");
            return;
        }

        count = (uint32_t)atoi(count_str);
        size = (uint32_t)atoi(size_str);

        if (count == 0 || size == 0 || size > NETLINK_USER_DATA_MTU) {
            printf("Error: Invalid count or size (count>0, 1<=size<=%d)\n", NETLINK_USER_DATA_MTU);
            return;
        }

        printf("[FLOW_TEST] Starting flow control test: count=%u, size=%u\n", count, size);

        /* Allocate buffer */
        buffer = (uint8_t *)malloc(size);
        if (!buffer) {
            printf("[FLOW_TEST] Error: Failed to allocate buffer\n");
            return;
        }

        printf("[FLOW_TEST] Sending %u packets of %u bytes each...\n", count, size);

        /* Send packets with sequence numbers */
        uint32_t success_count = 0;
        uint32_t failed_count = 0;

        for (i = 0; i < count; i++) {
            /* Build packet with sequence number */
            buffer[0] = (i >> 24) & 0xFF;
            buffer[1] = (i >> 16) & 0xFF;
            buffer[2] = (i >> 8) & 0xFF;
            buffer[3] = i & 0xFF;

            /* Fill remaining data with pattern */
            for (uint32_t j = 4; j < size; j++) {
                buffer[j] = i & 0xFF;
            }

            int ret = netlink_send(g_handle, NETLINK_DATA_TYPE_USER, buffer, (int)size);
            if (ret < 0) {
                failed_count++;
                if (failed_count <= 10) {  /* Only print first 10 errors */
                    printf("[FLOW_TEST] Warning: Failed to send packet %u (ret=%d)\n", i, ret);
                }
            } else {
                success_count++;
            }

            /* Print progress every 100 packets */
            if ((i + 1) % 100 == 0) {
                printf("[FLOW_TEST] Progress: %u/%u packets sent (%.1f%%)\n",
                       i + 1, count, ((i + 1) * 100.0) / (double)count);
                fflush(stdout);
            }

            /* Small delay to avoid overwhelming */
            if ((i + 1) % 10 == 0) {
                usleep(1000);  /* 1ms */
            }
        }

        free(buffer);

        /* Print final statistics */
        printf("\n");
        printf("========================================\n");
        printf("[FLOW_TEST] Send test completed\n");
        printf("[FLOW_TEST] Total packets:   %u\n", count);
        printf("[FLOW_TEST] Sent successfully: %u packets (%.2f%%)\n",
               success_count, (success_count * 100.0) / (double)count);
        printf("[FLOW_TEST] Failed:          %u packets (%.2f%%)\n",
               failed_count, (failed_count * 100.0) / (double)count);
        printf("[FLOW_TEST] Total bytes:     %u bytes\n", success_count * size);
        printf("========================================\n");
        printf("[FLOW_TEST] Please check device output to verify reception\n");
        fflush(stdout);

    } else if (strcmp(test_type, "mtu") == 0) {
        /* test mtu <size> */
        char *size_str = strtok(NULL, " ");

        if (!size_str) {
            printf("Usage: test mtu <size>\n");
            return;
        }

        size = (uint32_t)atoi(size_str);

        if (size == 0 || size > NETLINK_USER_DATA_MTU) {
            printf("Error: Invalid size (1<=size<=%d)\n", NETLINK_USER_DATA_MTU);
            return;
        }

        printf("[MTU_TEST] Testing MTU: size=%u bytes\n", size);

        /* Allocate buffer */
        buffer = (uint8_t *)malloc(size);
        if (!buffer) {
            printf("[MTU_TEST] Error: Failed to allocate buffer\n");
            return;
        }

        /* Fill buffer with test pattern */
        for (i = 0; i < size; i++) {
            buffer[i] = (uint8_t)i;
        }

        printf("[MTU_TEST] Sending %u bytes...\n", size);
        int ret = netlink_send(g_handle, NETLINK_DATA_TYPE_USER, buffer, (int)size);
        free(buffer);

        if (ret < 0) {
            printf("[MTU_TEST] Error: Failed to send data (ret=%d)\n", ret);
            return;
        }

        printf("[MTU_TEST] Successfully sent %d bytes\n", ret);
        printf("[MTU_TEST] Please check device if data was received correctly\n");
    } else {
        printf("Error: Unknown test type '%s'\n", test_type);
        printf("Available tests: flow, mtu\n");
    }
}

/**
 * @brief Command loop
 */
static void command_loop(void)
{
    char line[2048];
    char *cmd, *args;

    printf("Netlink Tool v%s\n", VERSION);
    printf("Type 'help' for available commands\n\n");

    while (g_running) {
        printf("netlink> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }

        /* Remove newline */
        line[strcspn(line, "\n")] = '\0';

        /* Skip empty lines */
        if (line[0] == '\0') {
            continue;
        }

        /* Parse command */
        cmd = strtok(line, " ");
        args = strtok(NULL, "");

        if (!cmd) {
            continue;
        }

        if (strcmp(cmd, "send") == 0) {
            handle_send(args);
        } else if (strcmp(cmd, "test") == 0) {
            handle_test(args);
        } else if (strcmp(cmd, "status") == 0) {
            show_status();
        } else if (strcmp(cmd, "help") == 0) {
            show_help();
        } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
            break;
        } else {
            printf("Unknown command: %s (type 'help' for available commands)\n", cmd);
        }
    }

    printf("\nBye!\n");
}

/**
 * @brief SIGINT signal handler (async-signal-safe)
 * @note Only uses async-signal-safe functions
 */
static void signal_handler(int sig)
{
    (void)sig;  /* Unused parameter */
    g_running = 0;
    /* Don't call printf here - it's not async-signal-safe */
}

/**
 * @brief Main function
 */
int main(int argc, char *argv[])
{
    (void)argc;  /* Unused parameters */
    (void)argv;
    int ret;

    /* Setup signal handlers */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Initialize Netlink */
    ret = netlink_init(&g_handle);
    if (ret < 0) {
        fprintf(stderr, "Failed to initialize netlink: %d\n", ret);
        fprintf(stderr, "Make sure kernel module is loaded (sudo modprobe hd_netlink)\n");
        return 1;
    }

    /* Register receive callbacks for all data types */
    netlink_register_callback(g_handle, NETLINK_DATA_TYPE_SYSTEM, recv_callback);
    netlink_register_callback(g_handle, NETLINK_DATA_TYPE_WIFI, recv_callback);
    netlink_register_callback(g_handle, NETLINK_DATA_TYPE_BLE, recv_callback);
    netlink_register_callback(g_handle, NETLINK_DATA_TYPE_USER, recv_callback);
    netlink_register_callback(g_handle, NETLINK_DATA_TYPE_LOG, recv_callback);

    /* Start receive thread */
    ret = pthread_create(&g_recv_thread, NULL, recv_thread_func, g_handle);
    if (ret != 0) {
        fprintf(stderr, "Failed to create recv thread: %d\n", ret);
        netlink_deinit(g_handle);
        return 1;
    }

    /* Enter command loop */
    command_loop();

    /* Cleanup */
    g_running = 0;
    pthread_join(g_recv_thread, NULL);
    netlink_deinit(g_handle);

    return 0;
}
