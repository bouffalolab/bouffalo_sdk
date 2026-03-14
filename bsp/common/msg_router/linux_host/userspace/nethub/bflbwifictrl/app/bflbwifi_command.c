#include "bflbwifi_command.h"

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

static const bflbwifi_command_desc_t g_commands[] = {
    {
        .id = BFLBWIFI_COMMAND_CONNECT_AP,
        .cli_name = "connect_ap",
        .ipc_name = "CONNECT_AP",
        .usage_args = "<ssid> [password]",
        .summary = "Connect to AP (open network if no password)",
        .min_args = 1,
        .max_args = 2,
    },
    {
        .id = BFLBWIFI_COMMAND_DISCONNECT,
        .cli_name = "disconnect",
        .ipc_name = "DISCONNECT",
        .usage_args = "",
        .summary = "Disconnect from AP",
        .min_args = 0,
        .max_args = 0,
    },
    {
        .id = BFLBWIFI_COMMAND_SCAN,
        .cli_name = "scan",
        .ipc_name = "SCAN",
        .usage_args = "",
        .summary = "Scan for APs",
        .min_args = 0,
        .max_args = 0,
    },
    {
        .id = BFLBWIFI_COMMAND_STATUS,
        .cli_name = "status",
        .ipc_name = "STATUS",
        .usage_args = "",
        .summary = "Show WiFi status",
        .min_args = 0,
        .max_args = 0,
    },
    {
        .id = BFLBWIFI_COMMAND_VERSION,
        .cli_name = "version",
        .ipc_name = "VERSION",
        .usage_args = "",
        .summary = "Show version",
        .min_args = 0,
        .max_args = 0,
    },
    {
        .id = BFLBWIFI_COMMAND_REBOOT,
        .cli_name = "reboot",
        .ipc_name = "REBOOT",
        .usage_args = "",
        .summary = "Reboot WiFi module",
        .min_args = 0,
        .max_args = 0,
    },
    {
        .id = BFLBWIFI_COMMAND_OTA,
        .cli_name = "ota",
        .ipc_name = "OTA",
        .usage_args = "<file.bin>",
        .summary = "OTA firmware upgrade",
        .min_args = 1,
        .max_args = 1,
    },
    {
        .id = BFLBWIFI_COMMAND_START_AP,
        .cli_name = "start_ap",
        .ipc_name = "START_AP",
        .usage_args = "<ssid> [password]",
        .summary = "Start SoftAP",
        .min_args = 1,
        .max_args = 2,
    },
    {
        .id = BFLBWIFI_COMMAND_STOP_AP,
        .cli_name = "stop_ap",
        .ipc_name = "STOP_AP",
        .usage_args = "",
        .summary = "Stop SoftAP",
        .min_args = 0,
        .max_args = 0,
    },
};

static const bflbwifi_command_desc_t *command_find(const char *name, bool by_ipc_name)
{
    if (!name || name[0] == '\0') {
        return NULL;
    }

    for (size_t i = 0; i < (sizeof(g_commands) / sizeof(g_commands[0])); i++) {
        const char *candidate = by_ipc_name ? g_commands[i].ipc_name : g_commands[i].cli_name;
        if (strcmp(candidate, name) == 0) {
            return &g_commands[i];
        }
    }

    return NULL;
}

const bflbwifi_command_desc_t *bflbwifi_command_find_cli(const char *name)
{
    return command_find(name, false);
}

const bflbwifi_command_desc_t *bflbwifi_command_find_ipc(const char *name)
{
    return command_find(name, true);
}

const bflbwifi_command_desc_t *bflbwifi_command_at(size_t index)
{
    if (index >= (sizeof(g_commands) / sizeof(g_commands[0]))) {
        return NULL;
    }

    return &g_commands[index];
}

size_t bflbwifi_command_count(void)
{
    return sizeof(g_commands) / sizeof(g_commands[0]);
}

int bflbwifi_command_validate_argc(const bflbwifi_command_desc_t *desc, int arg_count)
{
    if (!desc || arg_count < 0) {
        return -1;
    }

    if ((uint8_t)arg_count < desc->min_args || (uint8_t)arg_count > desc->max_args) {
        return -1;
    }

    return 0;
}
