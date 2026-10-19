#ifndef FLASHLOADER_COMMANDS_H
#define FLASHLOADER_COMMANDS_H

#include <stdint.h>

struct flashloader_command_result {
    uint16_t status;
    uint16_t data_length;
    uint8_t has_data;
    uint8_t reset;
};

void flashloader_commands_init(void);
struct flashloader_command_result flashloader_command_execute(
    uint8_t command, const uint8_t *payload, uint16_t payload_length,
    uint8_t *response_data, uint16_t response_capacity);

#endif
