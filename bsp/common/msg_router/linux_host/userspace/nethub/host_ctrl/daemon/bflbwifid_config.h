#ifndef BFLBWIFID_CONFIG_H
#define BFLBWIFID_CONFIG_H

#include <stdbool.h>
#include <stdio.h>

#include "nethub_paths.h"

typedef struct {
    char socket_path[108];
    char pid_file[256];
    char log_file[256];
    bool daemon_mode;
    nethub_paths_t paths;
} bflbwifid_config_t;

#define BFLBWIFID_CONFIG_PARSE_OK    0
#define BFLBWIFID_CONFIG_PARSE_ERROR -1
#define BFLBWIFID_CONFIG_PARSE_EXIT  1

void bflbwifid_config_init(bflbwifid_config_t *config);
void bflbwifid_config_print_usage(FILE *stream, const char *prog_name);
int bflbwifid_config_parse_argv(bflbwifid_config_t *config, int argc, char *argv[]);

#endif /* BFLBWIFID_CONFIG_H */
