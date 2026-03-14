/**
 * @file bflbwifid_service.h
 * @brief WiFi command execution and response formatting for bflbwifid
 */

#ifndef BFLBWIFID_SERVICE_H
#define BFLBWIFID_SERVICE_H

#include <stddef.h>

#include "bflbwifi_command.h"
#include "bflbwifi_ipc.h"

int bflbwifid_service_execute(char *response,
                              size_t response_len,
                              const bflbwifi_command_desc_t *command,
                              const bflbwifi_ipc_request_t *request);

#endif /* BFLBWIFID_SERVICE_H */
