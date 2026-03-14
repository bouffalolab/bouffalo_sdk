/**
 * @file bflbwifi_command.h
 * @brief Shared public command specification for CLI and daemon
 */

#ifndef BFLBWIFI_COMMAND_H
#define BFLBWIFI_COMMAND_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    BFLBWIFI_COMMAND_CONNECT_AP = 0,
    BFLBWIFI_COMMAND_DISCONNECT,
    BFLBWIFI_COMMAND_SCAN,
    BFLBWIFI_COMMAND_STATUS,
    BFLBWIFI_COMMAND_VERSION,
    BFLBWIFI_COMMAND_REBOOT,
    BFLBWIFI_COMMAND_OTA,
    BFLBWIFI_COMMAND_START_AP,
    BFLBWIFI_COMMAND_STOP_AP,
    BFLBWIFI_COMMAND_COUNT
} bflbwifi_command_id_t;

typedef struct {
    bflbwifi_command_id_t id;
    const char *cli_name;
    const char *ipc_name;
    const char *usage_args;
    const char *summary;
    uint8_t min_args;
    uint8_t max_args;
} bflbwifi_command_desc_t;

const bflbwifi_command_desc_t *bflbwifi_command_find_cli(const char *name);
const bflbwifi_command_desc_t *bflbwifi_command_find_ipc(const char *name);
const bflbwifi_command_desc_t *bflbwifi_command_at(size_t index);
size_t bflbwifi_command_count(void);
int bflbwifi_command_validate_argc(const bflbwifi_command_desc_t *desc, int arg_count);

#endif /* BFLBWIFI_COMMAND_H */
