/**
 * @file nethub_vchan_app.c
 * @brief NetHub Virtual Channel Test Application with Readline Support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#ifdef HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "nethub_vchan.h"

#define VERSION "1.0"

/* Command list for auto-completion */
static const char *commands[] = { "send", "test", "status", "help", "quit", "exit", NULL };

/* Data type list for send command completion */
static const char *data_types[] = { "system", "user", "at", NULL };

/* Test subcommand list for test command completion */
static const char *test_subcmds[] = { "flow", "mtu", NULL };

#ifdef HAVE_READLINE
/**
 * @brief Command name generator for readline completion
 */
static char *command_generator(const char *text, int state)
{
    static int list_index = 0;
    const char *name;
    size_t len;

    /* If state is 0, initialize */
    if (!state) {
        list_index = 0;
    }

    len = strlen(text);

    /* Return matching names */
    while ((name = commands[list_index])) {
        list_index++;
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

/**
 * @brief Data type generator for send command completion
 */
static char *data_type_generator(const char *text, int state)
{
    static int list_index = 0;
    const char *name;
    size_t len;

    if (!state) {
        list_index = 0;
    }

    len = strlen(text);

    while ((name = data_types[list_index])) {
        list_index++;
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

/**
 * @brief Test subcommand generator
 */
static char *test_subcmd_generator(const char *text, int state)
{
    static int list_index = 0;
    const char *name;
    size_t len;

    if (!state) {
        list_index = 0;
    }

    len = strlen(text);

    while ((name = test_subcmds[list_index])) {
        list_index++;
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

/**
 * @brief Readline completion function (compatible with readline 4.2+)
 */
static char **nethub_vchan_completion(const char *text, int start, int end)
{
    char **matches = NULL;

    (void)end; /* Unused */

    /* Complete command name at start of line */
    if (start == 0) {
        matches = rl_completion_matches(text, command_generator);
    }
    /* Complete data type after 'send' command */
    else {
        /* Get previous line to check command context */
        char *prev_line = strndup(rl_line_buffer, start);
        if (prev_line) {
            char *cmd = strtok(prev_line, " ");
            if (cmd && strcmp(cmd, "send") == 0) {
                /* Check if we already have a data type */
                char *token = strtok(NULL, " ");
                if (!token) {
                    /* Still on data type parameter */
                    matches = rl_completion_matches(text, data_type_generator);
                }
            } else if (cmd && strcmp(cmd, "test") == 0) {
                char *subcmd = strtok(NULL, " ");
                if (!subcmd) {
                    matches = rl_completion_matches(text, test_subcmd_generator);
                }
            }
            free(prev_line);
        }
    }

    return matches;
}
#endif

/**
 * @brief Convert data type to string
 */
static const char *data_type_to_str(uint8_t data_type)
{
    switch (data_type) {
        case NETHUB_VCHAN_DATA_TYPE_SYSTEM:
            return "SYSTEM";
        case NETHUB_VCHAN_DATA_TYPE_USER:
            return "USER";
        case NETHUB_VCHAN_DATA_TYPE_AT:
            return "AT";
        default:
            return "UNKNOWN";
    }
}

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
 * @brief Show help information
 */
static void show_help(void)
{
    printf("Available commands:\n");
    printf("  send [type] <string>   Send data with optional type\n");
    printf("                         Types: system, user, at\n");
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
    nethub_vchan_state_snapshot_t snapshot = {
        .link_state = NETHUB_VCHAN_LINK_DOWN,
        .host_state = NETHUB_VCHAN_HOST_STATE_UNKNOWN,
    };

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

    if (nethub_vchan_get_state_snapshot(&snapshot) != 0) {
        snapshot.link_state = NETHUB_VCHAN_LINK_DOWN;
        snapshot.host_state = NETHUB_VCHAN_HOST_STATE_UNKNOWN;
    }

    printf("Connection: %s\n", nethub_vchan_link_state_name(snapshot.link_state));
    printf("Host State: %s\n", nethub_vchan_host_state_name(snapshot.host_state));
    printf("\n");
    printf("[Kernel ↔ Device via SDIO/USB]\n");
    printf("  TX: %d packets (dropped: %d)\n", tx_packets >= 0 ? tx_packets : 0, tx_dropped >= 0 ? tx_dropped : 0);
    printf("  RX: %d packets (dropped: %d)\n", rx_packets >= 0 ? rx_packets : 0, rx_dropped >= 0 ? rx_dropped : 0);
    printf("  Pending Queue: %d packets\n", pending_queue >= 0 ? pending_queue : 0);
    printf("\n");
    printf("[Kernel ↔ Userspace via Netlink]\n");
    printf("  RX from userspace: %d packets\n", nl_rx_packets >= 0 ? nl_rx_packets : 0);
    printf("  TX to userspace: %d packets (dropped: %d)\n", nl_tx_packets >= 0 ? nl_tx_packets : 0,
           nl_tx_dropped >= 0 ? nl_tx_dropped : 0);
}

/**
 * @brief Handle send command
 * @note Supported syntax: send [type] <string>
 *       Type options: system, user, at (default: user)
 */
static void handle_send(char *args)
{
    char *data = args;
    char *type_str = NULL;
    size_t len;
    uint8_t data_type = NETHUB_VCHAN_DATA_TYPE_USER; /* Default to user type */
    char *send_buffer = NULL;
    size_t send_len = 0;
    if (!args) {
        printf("Usage: send [type] <string>\n");
        printf("Types: system, user, at (default: user)\n");
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
            data_type = NETHUB_VCHAN_DATA_TYPE_SYSTEM;
        } else if (strcmp(type_str, "user") == 0) {
            data_type = NETHUB_VCHAN_DATA_TYPE_USER;
        } else if (strcmp(type_str, "at") == 0) {
            data_type = NETHUB_VCHAN_DATA_TYPE_AT;
        } else {
            /* Not a type keyword, restore as data */
            data = type_str;
            data_type = NETHUB_VCHAN_DATA_TYPE_USER;
        }
    }

    len = strlen(data);
    if (len == 0) {
        printf("Usage: send [type] <string>\n");
        printf("Types: system, user, at (default: user)\n");
        return;
    }
    /* For AT type, append \r\n if not already present */
    if (data_type == NETHUB_VCHAN_DATA_TYPE_AT) {
        size_t data_len = strlen(data);
        int has_crlf = 0;

        /* Check if data already ends with \r\n */
        if (data_len >= 2) {
            if (data[data_len - 2] == '\r' && data[data_len - 1] == '\n') {
                has_crlf = 1;
            }
        }

        if (has_crlf) {
            /* Already has \r\n, use original data */
            send_buffer = data;
            send_len = data_len;
        } else {
            /* Append \r\n */
            send_buffer = malloc(data_len + 3); /* +2 for \r\n, +1 for null terminator */
            if (!send_buffer) {
                printf("Error: Memory allocation failed\n");
                return;
            }
            memcpy(send_buffer, data, data_len);
            send_buffer[data_len] = '\r';
            send_buffer[data_len + 1] = '\n';
            send_buffer[data_len + 2] = '\0';
            send_len = data_len + 2;
        }
    } else {
        /* For non-AT types, use original data */
        send_buffer = data;
        send_len = len;
    }

    /* Call send interface */
    int ret = nethub_vchan_send(data_type, send_buffer, send_len);
    if (ret > 0) {
        printf("Sent %d bytes (type=%s)\n", ret, data_type_to_str(data_type));
    } else {
        printf("Send failed (ret=%d)\n", ret);
    }
    if (send_buffer != data) {
        free(send_buffer);
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

        if (count == 0 || size == 0 || size > NETHUB_VCHAN_MAX_DATA_LEN) {
            printf("Error: Invalid count or size (count>0, 1<=size<=%d)\n", NETHUB_VCHAN_MAX_DATA_LEN);
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

            int ret = nethub_vchan_send(NETHUB_VCHAN_DATA_TYPE_USER, buffer, (int)size);
            if (ret < 0) {
                failed_count++;
                if (failed_count <= 10) { /* Only print first 10 errors */
                    printf("[FLOW_TEST] Warning: Failed to send packet %u (ret=%d)\n", i, ret);
                }
            } else {
                success_count++;
            }

            /* Print progress every 100 packets */
            if ((i + 1) % 100 == 0) {
                printf("[FLOW_TEST] Progress: %u/%u packets sent (%.1f%%)\n", i + 1, count,
                       ((i + 1) * 100.0) / (double)count);
                fflush(stdout);
            }

            /* Small delay to avoid overwhelming */
            if ((i + 1) % 10 == 0) {
                usleep(1000); /* 1ms */
            }
        }

        free(buffer);

        /* Print final statistics */
        printf("\n");
        printf("========================================\n");
        printf("[FLOW_TEST] Send test completed\n");
        printf("[FLOW_TEST] Total packets:   %u\n", count);
        printf("[FLOW_TEST] Sent successfully: %u packets (%.2f%%)\n", success_count,
               (success_count * 100.0) / (double)count);
        printf("[FLOW_TEST] Failed:          %u packets (%.2f%%)\n", failed_count,
               (failed_count * 100.0) / (double)count);
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

        if (size == 0 || size > NETHUB_VCHAN_MAX_DATA_LEN) {
            printf("Error: Invalid size (1<=size<=%d)\n", NETHUB_VCHAN_MAX_DATA_LEN);
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
        int ret = nethub_vchan_send(NETHUB_VCHAN_DATA_TYPE_USER, buffer, (int)size);
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
 * @brief Command loop with readline support
 */
static void command_loop(void)
{
    char *line;
    char *cmd, *args;

    printf("NetHub Virtual Channel Tool v%s\n", VERSION);
    printf("Type 'help' for available commands\n\n");

#ifdef HAVE_READLINE
    /* Initialize readline (compatible with old versions) */
    rl_readline_name = "nethub_vchan";
    rl_attempted_completion_function = nethub_vchan_completion;

    /* Initialize history */
    using_history();
    stifle_history(100); /* Limit history to 100 entries */

    /* Try to load history from file (optional, skip if fails) */
    /* read_history("/tmp/.nethub_vchan_history"); */
#endif

    while (1) {
#ifdef HAVE_READLINE
        /* Use readline with auto-completion and history */
        line = readline("nethub_vchan> ");
        if (!line) {
            /* EOF (Ctrl+D) */
            break;
        }

        /* Skip empty lines */
        if (line[0] == '\0') {
            free(line);
            continue;
        }

        /* Add to history */
        add_history(line);
#else
        /* Fallback to standard input without readline */
        char buffer[2048];
        printf("nethub_vchan> ");
        fflush(stdout);

        if (!fgets(buffer, sizeof(buffer), stdin)) {
            break;
        }

        /* Remove newline */
        buffer[strcspn(buffer, "\n")] = '\0';

        /* Skip empty lines */
        if (buffer[0] == '\0') {
            continue;
        }

        line = strdup(buffer);
        if (!line) {
            fprintf(stderr, "Memory allocation failed\n");
            continue;
        }
#endif

        /* Parse command */
        cmd = strtok(line, " ");
        args = strtok(NULL, "");

        if (!cmd) {
            free(line);
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
            free(line);
            break;
        } else {
            printf("Unknown command: %s (type 'help' for available commands)\n", cmd);
        }

        free(line);
    }

#ifdef HAVE_READLINE
    /* Save history to file (optional) */
    /* write_history("/tmp/.nethub_vchan_history"); */
#endif

    printf("\nBye!\n");
}

/**
 * @brief Main function
 */
int main(int argc, char *argv[])
{
    (void)argc; /* Unused parameters */
    (void)argv;
    int ret;

    /* Initialize NetHub Virtual Channel (singleton, receive thread auto-started) */
    ret = nethub_vchan_init();
    if (ret < 0) {
        fprintf(stderr, "Failed to initialize nethub_vchan: %d\n", ret);
        fprintf(stderr, "Make sure kernel module is loaded (sudo modprobe hd_netlink)\n");
        return 1;
    }

    /* Register receive callbacks for all data types */
    nethub_vchan_register_callback(NETHUB_VCHAN_DATA_TYPE_SYSTEM, recv_callback);
    nethub_vchan_register_callback(NETHUB_VCHAN_DATA_TYPE_USER, recv_callback);
    nethub_vchan_register_callback(NETHUB_VCHAN_DATA_TYPE_AT, recv_callback);

    /* Enter command loop */
    command_loop();

    /* Cleanup - nethub_vchan_deinit() will auto-stop recv thread */
    nethub_vchan_deinit();

    return 0;
}
