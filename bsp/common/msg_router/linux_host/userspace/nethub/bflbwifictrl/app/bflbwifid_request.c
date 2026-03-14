#include "bflbwifid_request.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "bflbwifi_command.h"
#include "bflbwifi_ipc.h"
#include "bflbwifid_log.h"
#include "bflbwifid_ota.h"
#include "bflbwifid_runtime.h"
#include "bflbwifid_service.h"

static void request_send_response(int client_fd, const char *payload)
{
    if (bflbwifi_ipc_send_response(client_fd, 0, payload ? payload : "") != 0) {
        BFLBWIFID_LOGE("Failed to send IPC response");
    }
}

static void request_send_invalid_args_response(int client_fd, const bflbwifi_command_desc_t *command)
{
    char response[MAX_RESPONSE_LEN];

    if (!command) {
        request_send_response(client_fd, "ERROR -4 Invalid command\nEND\n");
        return;
    }

    if (command->usage_args[0] != '\0') {
        snprintf(response, sizeof(response),
                 "ERROR -4 Usage: %s %s\nEND\n",
                 command->ipc_name,
                 command->usage_args);
    } else {
        snprintf(response, sizeof(response),
                 "ERROR -4 Usage: %s\nEND\n",
                 command->ipc_name);
    }

    request_send_response(client_fd, response);
}

static int ensure_ctrl_backend_ready(int client_fd, const char *command)
{
    bflbwifi_ctrl_status_t ctrl_status;
    char response[MAX_RESPONSE_LEN];
    int ret;

    ret = bflbwifid_refresh_runtime_state(false);
    if (ret != 0) {
        snprintf(response, sizeof(response), "ERROR %d %s\nEND\n", ret, bflbwifi_strerror(ret));
        request_send_response(client_fd, response);
        return -1;
    }

    bflbwifid_get_runtime_snapshot(NULL, &ctrl_status);
    if (ctrl_status.link_state == BFLBWIFI_CTRL_LINK_CONNECTED) {
        return 0;
    }

    BFLBWIFID_LOGE("Reject command %s: backend %s is %s",
                   command,
                   bflbwifi_ctrl_backend_name(ctrl_status.ctrl_backend),
                   bflbwifi_ctrl_link_state_name(ctrl_status.link_state));
    snprintf(response, sizeof(response),
             "ERROR %d Control backend unavailable (%s:%s)\nEND\n",
             E_ERR_BACKEND_DOWN,
             bflbwifi_ctrl_backend_name(ctrl_status.ctrl_backend),
             bflbwifi_ctrl_link_state_name(ctrl_status.link_state));
    request_send_response(client_fd, response);
    return -1;
}

static void process_client_command(int client_fd, const bflbwifi_ipc_request_t *request)
{
    char response[MAX_RESPONSE_LEN];
    const bflbwifi_command_desc_t *command;
    const char *token;

    if (!request || request->argc <= 0 || !request->argv || !request->argv[0] || request->argv[0][0] == '\0') {
        request_send_response(client_fd, "ERROR -4 Invalid command\nEND\n");
        return;
    }

    token = request->argv[0];
    command = bflbwifi_command_find_ipc(token);
    if (!command) {
        char response[MAX_RESPONSE_LEN];
        snprintf(response, sizeof(response), "ERROR -4 Unknown command: %s\nEND\n", token);
        request_send_response(client_fd, response);
        return;
    }

    if (bflbwifi_command_validate_argc(command, request->argc - 1) != 0) {
        request_send_invalid_args_response(client_fd, command);
        return;
    }

    {
        if (bflbwifid_ota_in_progress()) {
            time_t start_time = bflbwifid_ota_get_start_time();
            if (start_time > 0 && (time(NULL) - start_time > 1800)) {
                BFLBWIFID_LOGE("OTA timeout detected, auto-recovery triggered");
                bflbwifid_ota_reset();
            }
        }
    }

    if (bflbwifid_ota_in_progress() && command->id != BFLBWIFI_COMMAND_OTA) {
        request_send_response(client_fd, "ERROR -12 OTA in progress, try later\nEND\n");
        BFLBWIFID_LOGI("Command rejected due to OTA in progress");
        return;
    }

    switch (command->id) {
        case BFLBWIFI_COMMAND_CONNECT_AP:
            if (ensure_ctrl_backend_ready(client_fd, token) != 0) {
                return;
            }
            break;
        case BFLBWIFI_COMMAND_DISCONNECT:
            if (ensure_ctrl_backend_ready(client_fd, token) != 0) {
                return;
            }
            break;
        case BFLBWIFI_COMMAND_SCAN:
            if (ensure_ctrl_backend_ready(client_fd, token) != 0) {
                return;
            }
            break;
        case BFLBWIFI_COMMAND_STATUS:
            break;
        case BFLBWIFI_COMMAND_VERSION:
            if (ensure_ctrl_backend_ready(client_fd, token) != 0) {
                return;
            }
            break;
        case BFLBWIFI_COMMAND_REBOOT:
            if (ensure_ctrl_backend_ready(client_fd, token) != 0) {
                return;
            }
            break;
        case BFLBWIFI_COMMAND_OTA:
            if (ensure_ctrl_backend_ready(client_fd, token) != 0) {
                return;
            }
            break;
        case BFLBWIFI_COMMAND_START_AP:
            if (ensure_ctrl_backend_ready(client_fd, token) != 0) {
                return;
            }
            break;
        case BFLBWIFI_COMMAND_STOP_AP:
            if (ensure_ctrl_backend_ready(client_fd, token) != 0) {
                return;
            }
            break;
        default:
            request_send_response(client_fd, "ERROR -10 Command not supported\nEND\n");
            return;
    }

    bflbwifid_service_execute(response, sizeof(response), command, request);
    request_send_response(client_fd, response);
}

void bflbwifid_handle_client(int client_fd)
{
    bflbwifi_ipc_request_t request;

    if (bflbwifi_ipc_recv_request(client_fd, &request) != 0) {
        BFLBWIFID_LOGW("Failed to receive IPC request");
        return;
    }

    process_client_command(client_fd, &request);
    bflbwifi_ipc_request_deinit(&request);
}
