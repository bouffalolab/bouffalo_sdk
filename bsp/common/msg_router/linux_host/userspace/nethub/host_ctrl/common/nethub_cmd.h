#ifndef NETHUB_CMD_H
#define NETHUB_CMD_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    NETHUB_CMD_CONNECT_AP = 1,
    NETHUB_CMD_DISCONNECT,
    NETHUB_CMD_SCAN,
    NETHUB_CMD_STATUS,
    NETHUB_CMD_VERSION,
    NETHUB_CMD_REBOOT,
    NETHUB_CMD_START_AP,
    NETHUB_CMD_STOP_AP,
    NETHUB_CMD_OTA,
    NETHUB_CMD_COUNT,
} nethub_cmd_id_t;

typedef struct {
    nethub_cmd_id_t id;
    const char *cli_name;
    const char *summary;
    const char *usage_args;
    uint8_t min_args;
    uint8_t max_args;
} nethub_cmd_desc_t;

const nethub_cmd_desc_t *nethub_cmd_find_cli(const char *name);
const nethub_cmd_desc_t *nethub_cmd_at(size_t index);
size_t nethub_cmd_count(void);
int nethub_cmd_validate_argc(const nethub_cmd_desc_t *desc, int arg_count);

#endif /* NETHUB_CMD_H */
