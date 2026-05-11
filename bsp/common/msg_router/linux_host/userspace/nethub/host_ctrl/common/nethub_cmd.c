#include "nethub_cmd.h"

#include <string.h>

static const nethub_cmd_desc_t g_cmds[] = {
    { NETHUB_CMD_CONNECT_AP, "connect_ap", "Connect to AP", "<ssid> [password]", 1, 2 },
    { NETHUB_CMD_DISCONNECT, "disconnect", "Disconnect from AP", "", 0, 0 },
    { NETHUB_CMD_SCAN, "scan", "Scan APs", "", 0, 0 },
    { NETHUB_CMD_STATUS, "status", "Show WiFi status", "", 0, 0 },
    { NETHUB_CMD_VERSION, "version", "Show module version", "", 0, 0 },
    { NETHUB_CMD_REBOOT, "reboot", "Reboot WiFi module", "", 0, 0 },
    { NETHUB_CMD_START_AP, "start_ap", "Start SoftAP", "<ssid> [password]", 1, 2 },
    { NETHUB_CMD_STOP_AP, "stop_ap", "Stop SoftAP", "", 0, 0 },
    { NETHUB_CMD_OTA, "ota", "Update firmware", "<firmware_path>", 1, 1 },
};

const nethub_cmd_desc_t *nethub_cmd_find_cli(const char *name)
{
    size_t i;

    if (!name || name[0] == '\0') {
        return NULL;
    }

    for (i = 0; i < sizeof(g_cmds) / sizeof(g_cmds[0]); i++) {
        if (strcmp(g_cmds[i].cli_name, name) == 0) {
            return &g_cmds[i];
        }
    }

    return NULL;
}

const nethub_cmd_desc_t *nethub_cmd_at(size_t index)
{
    if (index >= sizeof(g_cmds) / sizeof(g_cmds[0])) {
        return NULL;
    }

    return &g_cmds[index];
}

size_t nethub_cmd_count(void)
{
    return sizeof(g_cmds) / sizeof(g_cmds[0]);
}

int nethub_cmd_validate_argc(const nethub_cmd_desc_t *desc, int arg_count)
{
    if (!desc || arg_count < 0) {
        return -1;
    }

    if ((uint8_t)arg_count < desc->min_args || (uint8_t)arg_count > desc->max_args) {
        return -1;
    }

    return 0;
}
