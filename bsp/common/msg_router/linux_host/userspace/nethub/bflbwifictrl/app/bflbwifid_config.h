/**
 * @file bflbwifid_config.h
 * @brief Command-line configuration parsing for bflbwifid
 */

#ifndef BFLBWIFID_CONFIG_H
#define BFLBWIFID_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bflbwifid_paths.h"

#define BFLBWIFID_TTY_DEV_MAX      128
#define BFLBWIFID_SOCKET_PATH_MAX  108
#define BFLBWIFID_FILE_PATH_MAX    256

#define BFLBWIFID_CONFIG_PARSE_OK          0
#define BFLBWIFID_CONFIG_PARSE_EXIT        1
#define BFLBWIFID_CONFIG_PARSE_ERROR      -1

typedef struct {
    bool daemon_mode;
    uint8_t ctrl_backend;
    char tty_dev[BFLBWIFID_TTY_DEV_MAX];
    char socket_path[BFLBWIFID_SOCKET_PATH_MAX];
    char pid_file[BFLBWIFID_FILE_PATH_MAX];
    char log_file[BFLBWIFID_FILE_PATH_MAX];
    bflbwifid_paths_t paths;
} bflbwifid_config_t;

void bflbwifid_config_init(bflbwifid_config_t *config);
void bflbwifid_config_print_usage(FILE *stream, const char *prog_name);
int bflbwifid_config_parse_argv(bflbwifid_config_t *config, int argc, char *argv[]);

#endif /* BFLBWIFID_CONFIG_H */
