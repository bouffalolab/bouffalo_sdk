/**
 * @file bflbwifid_config.c
 * @brief Command-line configuration parsing for bflbwifid
 */

#include "bflbwifid_config.h"

#include <getopt.h>
#include <string.h>

#include "../include/bflbwifi.h"

static int config_set_string(char *dst, size_t dst_size, const char *value, const char *name)
{
    size_t value_len;

    if (!dst || dst_size == 0 || !value || value[0] == '\0') {
        fprintf(stderr, "Error: %s cannot be empty\n", name);
        return -1;
    }

    value_len = strlen(value);
    if (value_len >= dst_size) {
        fprintf(stderr, "Error: %s too long: %s\n", name, value);
        return -1;
    }

    memcpy(dst, value, value_len + 1u);
    return 0;
}

static int config_set_backend(bflbwifid_config_t *config, const char *backend)
{
    if (!config || !backend) {
        return -1;
    }

    if (strcmp(backend, "vchan") == 0 || strcmp(backend, "virtualchannel") == 0) {
        config->ctrl_backend = BFLBWIFI_CTRL_BACKEND_VCHAN;
        return 0;
    }

    if (strcmp(backend, "tty") == 0) {
        config->ctrl_backend = BFLBWIFI_CTRL_BACKEND_TTY;
        return 0;
    }

    fprintf(stderr, "Error: Invalid channel type '%s'\n", backend);
    fprintf(stderr, "Valid options: tty, vchan\n");
    return -1;
}

void bflbwifid_config_init(bflbwifid_config_t *config)
{
    if (!config) {
        return;
    }

    memset(config, 0, sizeof(*config));
    config->ctrl_backend = BFLBWIFI_CTRL_BACKEND_TTY;

    memcpy(config->tty_dev, "/dev/ttyAT0", sizeof("/dev/ttyAT0"));
    memcpy(config->socket_path, BFLBWIFID_DEFAULT_SOCKET_PATH, sizeof(BFLBWIFID_DEFAULT_SOCKET_PATH));
    memcpy(config->pid_file, BFLBWIFID_DEFAULT_PID_FILE, sizeof(BFLBWIFID_DEFAULT_PID_FILE));
    memcpy(config->log_file, BFLBWIFID_DEFAULT_LOG_FILE, sizeof(BFLBWIFID_DEFAULT_LOG_FILE));

    config->paths.socket_path = config->socket_path;
    config->paths.pid_file = config->pid_file;
    config->paths.log_file = config->log_file;
}

void bflbwifid_config_print_usage(FILE *stream, const char *prog_name)
{
    const char *name = prog_name ? prog_name : "bflbwifid";

    fprintf(stream, "Usage: %s [options]\n", name);
    fprintf(stream, "Options:\n");
    fprintf(stream, "  -p <dev>   TTY device (default: /dev/ttyAT0)\n");
    fprintf(stream, "  -c <type>  Channel type: tty or vchan (default: tty)\n");
    fprintf(stream, "  -s <path>  Unix socket path (default: %s)\n", BFLBWIFID_DEFAULT_SOCKET_PATH);
    fprintf(stream, "  -P <path>  PID file path (default: %s)\n", BFLBWIFID_DEFAULT_PID_FILE);
    fprintf(stream, "  -l <path>  Log file path (default: %s)\n", BFLBWIFID_DEFAULT_LOG_FILE);
    fprintf(stream, "  -d         Run as daemon\n");
    fprintf(stream, "  -h         Show this help\n");
}

int bflbwifid_config_parse_argv(bflbwifid_config_t *config, int argc, char *argv[])
{
    int opt;

    if (!config) {
        return BFLBWIFID_CONFIG_PARSE_ERROR;
    }

    opterr = 0;
    optind = 1;

    while ((opt = getopt(argc, argv, "p:c:s:P:l:dh")) != -1) {
        switch (opt) {
            case 'p':
                if (config_set_string(config->tty_dev, sizeof(config->tty_dev), optarg, "TTY device path") != 0) {
                    return BFLBWIFID_CONFIG_PARSE_ERROR;
                }
                break;
            case 'c':
                if (config_set_backend(config, optarg) != 0) {
                    return BFLBWIFID_CONFIG_PARSE_ERROR;
                }
                break;
            case 's':
                if (config_set_string(config->socket_path, sizeof(config->socket_path), optarg, "socket path") != 0) {
                    return BFLBWIFID_CONFIG_PARSE_ERROR;
                }
                break;
            case 'P':
                if (config_set_string(config->pid_file, sizeof(config->pid_file), optarg, "PID file path") != 0) {
                    return BFLBWIFID_CONFIG_PARSE_ERROR;
                }
                break;
            case 'l':
                if (config_set_string(config->log_file, sizeof(config->log_file), optarg, "log file path") != 0) {
                    return BFLBWIFID_CONFIG_PARSE_ERROR;
                }
                break;
            case 'd':
                config->daemon_mode = true;
                break;
            case 'h':
                bflbwifid_config_print_usage(stdout, argv ? argv[0] : NULL);
                return BFLBWIFID_CONFIG_PARSE_EXIT;
            case '?':
                if (optopt != 0) {
                    fprintf(stderr, "Error: Missing value for option -%c\n", optopt);
                } else if (optind > 0 && argv && argv[optind - 1]) {
                    fprintf(stderr, "Error: Unknown option: %s\n", argv[optind - 1]);
                } else {
                    fprintf(stderr, "Error: Invalid command line option\n");
                }
                return BFLBWIFID_CONFIG_PARSE_ERROR;
            default:
                return BFLBWIFID_CONFIG_PARSE_ERROR;
        }
    }

    if (optind < argc) {
        fprintf(stderr, "Error: Unexpected argument: %s\n", argv[optind]);
        return BFLBWIFID_CONFIG_PARSE_ERROR;
    }

    return BFLBWIFID_CONFIG_PARSE_OK;
}
