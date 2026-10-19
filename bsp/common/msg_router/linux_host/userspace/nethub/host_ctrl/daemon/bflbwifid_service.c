#include "bflbwifid_service.h"

#include <string.h>

#include "nethub_ipc.h"
#include "nethub_log.h"
#include "nethub_wifi_service.h"

static int send_simple_response(int fd, nethub_cmd_id_t cmd, int status)
{
    if (nethub_ipc_send_response(fd, cmd, status, NULL, 0) != 0) {
        NETHUB_LOGE("Failed to send IPC response");
        return -1;
    }
    return 0;
}

static int handle_status(int fd, nethub_cmd_id_t cmd)
{
    nethub_wifi_status_t status;
    int ret;

    memset(&status, 0, sizeof(status));
    ret = nethub_wifi_service_get_status(&status);
    return nethub_ipc_send_response(fd, cmd, ret, ret == 0 ? &status : NULL, ret == 0 ? sizeof(status) : 0);
}

static int handle_version(int fd, nethub_cmd_id_t cmd)
{
    char version[NETHUB_WIFI_VERSION_MAX_LEN];
    int ret;

    memset(version, 0, sizeof(version));
    ret = nethub_wifi_service_get_version(version, sizeof(version));
    return nethub_ipc_send_response(fd, cmd, ret, ret == 0 ? version : NULL, ret == 0 ? strlen(version) : 0);
}

static int handle_scan(int fd, nethub_cmd_id_t cmd, const nethub_ipc_request_t *request)
{
    const nethub_ipc_scan_req_t *req;
    nethub_ipc_scan_resp_t resp;
    int max_count;
    int ret;

    if (request->payload_len != sizeof(*req) || !request->payload) {
        return send_simple_response(fd, cmd, NETHUB_WIFI_ERR_INVALID_PARAM);
    }

    req = (const nethub_ipc_scan_req_t *)request->payload;
    max_count = req->max_count;
    if (max_count > NETHUB_WIFI_SCAN_MAX_APS) {
        max_count = NETHUB_WIFI_SCAN_MAX_APS;
    }

    memset(&resp, 0, sizeof(resp));
    ret = nethub_wifi_service_scan(resp.aps, max_count, &resp.actual_count, req->timeout_ms);
    if (ret == NETHUB_WIFI_ERR_BUFFER_TOO_SMALL) {
        ret = 0;
    }

    return nethub_ipc_send_response(fd, cmd, ret, ret == 0 ? &resp : NULL, ret == 0 ? sizeof(resp) : 0);
}

static int handle_connect(int fd, nethub_cmd_id_t cmd, const nethub_ipc_request_t *request)
{
    const nethub_ipc_sta_connect_req_t *req;
    int ret;

    if (request->payload_len != sizeof(*req) || !request->payload) {
        return send_simple_response(fd, cmd, NETHUB_WIFI_ERR_INVALID_PARAM);
    }

    req = (const nethub_ipc_sta_connect_req_t *)request->payload;
    ret = nethub_wifi_service_sta_connect(&req->cfg, req->timeout_ms);
    return send_simple_response(fd, cmd, ret);
}

static int handle_ap_start(int fd, nethub_cmd_id_t cmd, const nethub_ipc_request_t *request)
{
    const nethub_ipc_ap_start_req_t *req;
    int ret;

    if (request->payload_len != sizeof(*req) || !request->payload) {
        return send_simple_response(fd, cmd, NETHUB_WIFI_ERR_INVALID_PARAM);
    }

    req = (const nethub_ipc_ap_start_req_t *)request->payload;
    ret = nethub_wifi_service_ap_start(&req->cfg, req->timeout_ms);
    return send_simple_response(fd, cmd, ret);
}

static int handle_ota(int fd, nethub_cmd_id_t cmd, const nethub_ipc_request_t *request)
{
    const nethub_ipc_ota_req_t *req;
    int ret;

    if (request->payload_len != sizeof(*req) || !request->payload) {
        return send_simple_response(fd, cmd, NETHUB_WIFI_ERR_INVALID_PARAM);
    }

    req = (const nethub_ipc_ota_req_t *)request->payload;
    if (memchr(req->path, '\0', sizeof(req->path)) == NULL || req->path[0] == '\0') {
        return send_simple_response(fd, cmd, NETHUB_WIFI_ERR_INVALID_PARAM);
    }

    ret = nethub_wifi_service_ota_update(req->path, req->timeout_ms);
    return send_simple_response(fd, cmd, ret);
}

int bflbwifid_handle_client(int client_fd)
{
    nethub_ipc_request_t request;
    int ret;

    if (nethub_ipc_recv_request(client_fd, &request) != 0) {
        NETHUB_LOGW("Failed to receive IPC request");
        return -1;
    }

    switch (request.command) {
        case NETHUB_CMD_CONNECT_AP:
            ret = handle_connect(client_fd, request.command, &request);
            break;
        case NETHUB_CMD_DISCONNECT:
            ret = send_simple_response(client_fd, request.command, nethub_wifi_service_sta_disconnect());
            break;
        case NETHUB_CMD_SCAN:
            ret = handle_scan(client_fd, request.command, &request);
            break;
        case NETHUB_CMD_STATUS:
            ret = handle_status(client_fd, request.command);
            break;
        case NETHUB_CMD_VERSION:
            ret = handle_version(client_fd, request.command);
            break;
        case NETHUB_CMD_REBOOT:
            ret = send_simple_response(client_fd, request.command, nethub_wifi_service_reboot());
            break;
        case NETHUB_CMD_START_AP:
            ret = handle_ap_start(client_fd, request.command, &request);
            break;
        case NETHUB_CMD_STOP_AP:
            ret = send_simple_response(client_fd, request.command, nethub_wifi_service_ap_stop());
            break;
        case NETHUB_CMD_OTA:
            ret = handle_ota(client_fd, request.command, &request);
            break;
        default:
            ret = send_simple_response(client_fd, request.command, NETHUB_WIFI_ERR_NOT_SUPPORTED);
            break;
    }

    nethub_ipc_request_deinit(&request);
    return ret;
}
