#include "nethub_wifi.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "nethub_ipc.h"

#define CLIENT_CONNECT_RETRY_COUNT    20
#define CLIENT_CONNECT_RETRY_DELAY_US 100000

struct nethub_wifi_client {
    char socket_path[sizeof(((struct sockaddr_un *)0)->sun_path)];
};

const char *nethub_wifi_strerror(int err)
{
    switch (err) {
        case NETHUB_WIFI_OK: return "Success";
        case NETHUB_WIFI_ERR: return "General error";
        case NETHUB_WIFI_ERR_TIMEOUT: return "Operation timeout";
        case NETHUB_WIFI_ERR_INVALID_PARAM: return "Invalid parameter";
        case NETHUB_WIFI_ERR_NO_MEMORY: return "Out of memory";
        case NETHUB_WIFI_ERR_CONNECT_FAILED: return "Connection failed";
        case NETHUB_WIFI_ERR_WRONG_PASSWORD: return "Wrong password";
        case NETHUB_WIFI_ERR_NO_AP_FOUND: return "AP not found";
        case NETHUB_WIFI_ERR_NOT_INITIALIZED: return "Not initialized";
        case NETHUB_WIFI_ERR_NOT_SUPPORTED: return "Operation not supported";
        case NETHUB_WIFI_ERR_BUFFER_TOO_SMALL: return "Buffer too small";
        case NETHUB_WIFI_ERR_BUSY: return "Device busy";
        case NETHUB_WIFI_ERR_LINK_DOWN: return "Control link down";
        case NETHUB_WIFI_ERR_PROTOCOL: return "Protocol error";
        default: return "Unknown error";
    }
}

const char *nethub_wifi_sta_state_name(nethub_wifi_sta_state_t state)
{
    switch (state) {
        case NETHUB_WIFI_STA_IDLE: return "IDLE";
        case NETHUB_WIFI_STA_CONNECTING: return "CONNECTING";
        case NETHUB_WIFI_STA_CONNECTED: return "CONNECTED";
        case NETHUB_WIFI_STA_GOT_IP: return "GOTIP";
        case NETHUB_WIFI_STA_DISCONNECTED: return "DISCONNECTED";
        case NETHUB_WIFI_STA_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

const char *nethub_wifi_link_state_name(nethub_wifi_link_state_t state)
{
    switch (state) {
        case NETHUB_WIFI_LINK_UP: return "UP";
        case NETHUB_WIFI_LINK_DOWN: return "DOWN";
        default: return "UNKNOWN";
    }
}

static int client_connect(const nethub_wifi_client_t *client)
{
    struct sockaddr_un addr;
    int last_errno = 0;

    if (!client || client->socket_path[0] == '\0') {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, client->socket_path, strlen(client->socket_path) + 1u);

    for (int i = 0; i < CLIENT_CONNECT_RETRY_COUNT; i++) {
        int fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0) {
            return -1;
        }

        if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
            return fd;
        }

        last_errno = errno;
        close(fd);
        if (last_errno != ENOENT && last_errno != ECONNREFUSED) {
            break;
        }
        usleep(CLIENT_CONNECT_RETRY_DELAY_US);
    }

    return -1;
}

static int client_request(nethub_wifi_client_t *client,
                          nethub_cmd_id_t command,
                          const void *req_payload,
                          size_t req_len,
                          nethub_ipc_response_t *response)
{
    int fd;
    int ret;

    if (!client || !response) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }
    memset(response, 0, sizeof(*response));

    fd = client_connect(client);
    if (fd < 0) {
        return NETHUB_WIFI_ERR_CONNECT_FAILED;
    }

    ret = nethub_ipc_send_request(fd, command, req_payload, req_len);
    if (ret == 0) {
        ret = nethub_ipc_recv_response(fd, response);
    }
    close(fd);

    if (ret != 0) {
        return NETHUB_WIFI_ERR_PROTOCOL;
    }

    return response->status;
}

int nethub_wifi_client_open(nethub_wifi_client_t **client, const char *socket_path)
{
    nethub_wifi_client_t *ctx;
    const char *path = socket_path ? socket_path : NETHUB_WIFI_DEFAULT_SOCKET_PATH;

    if (!client || !path || path[0] == '\0') {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }
    *client = NULL;
    if (strlen(path) >= sizeof(((nethub_wifi_client_t *)0)->socket_path)) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    ctx = (nethub_wifi_client_t *)calloc(1, sizeof(*ctx));
    if (!ctx) {
        return NETHUB_WIFI_ERR_NO_MEMORY;
    }

    memcpy(ctx->socket_path, path, strlen(path) + 1u);
    *client = ctx;
    return NETHUB_WIFI_OK;
}

void nethub_wifi_client_close(nethub_wifi_client_t *client)
{
    free(client);
}

int nethub_wifi_get_status(nethub_wifi_client_t *client, nethub_wifi_status_t *status)
{
    nethub_ipc_response_t resp;
    int ret;

    if (!status) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    ret = client_request(client, NETHUB_CMD_STATUS, NULL, 0, &resp);
    if (ret == NETHUB_WIFI_OK) {
        if (resp.payload_len != sizeof(*status) || !resp.payload) {
            ret = NETHUB_WIFI_ERR_PROTOCOL;
        } else {
            memcpy(status, resp.payload, sizeof(*status));
        }
    }
    nethub_ipc_response_deinit(&resp);
    return ret;
}

int nethub_wifi_get_version(nethub_wifi_client_t *client, char *buf, size_t len)
{
    nethub_ipc_response_t resp;
    int ret;

    if (!buf || len == 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    ret = client_request(client, NETHUB_CMD_VERSION, NULL, 0, &resp);
    if (ret == NETHUB_WIFI_OK) {
        if (!resp.payload || resp.payload_len == 0) {
            ret = NETHUB_WIFI_ERR_PROTOCOL;
        } else if (resp.payload_len >= len) {
            ret = NETHUB_WIFI_ERR_BUFFER_TOO_SMALL;
        } else {
            memcpy(buf, resp.payload, resp.payload_len);
            buf[resp.payload_len] = '\0';
        }
    }
    nethub_ipc_response_deinit(&resp);
    return ret;
}

int nethub_wifi_reboot(nethub_wifi_client_t *client)
{
    nethub_ipc_response_t resp;
    int ret = client_request(client, NETHUB_CMD_REBOOT, NULL, 0, &resp);

    nethub_ipc_response_deinit(&resp);
    return ret;
}

int nethub_wifi_sta_connect(nethub_wifi_client_t *client,
                            const nethub_wifi_sta_config_t *cfg,
                            int timeout_ms)
{
    nethub_ipc_sta_connect_req_t req;
    nethub_ipc_response_t resp;
    int ret;

    if (!cfg || timeout_ms < 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    memset(&req, 0, sizeof(req));
    req.cfg = *cfg;
    req.timeout_ms = timeout_ms;
    ret = client_request(client, NETHUB_CMD_CONNECT_AP, &req, sizeof(req), &resp);
    nethub_ipc_response_deinit(&resp);
    return ret;
}

int nethub_wifi_sta_disconnect(nethub_wifi_client_t *client)
{
    nethub_ipc_response_t resp;
    int ret = client_request(client, NETHUB_CMD_DISCONNECT, NULL, 0, &resp);

    nethub_ipc_response_deinit(&resp);
    return ret;
}

int nethub_wifi_scan(nethub_wifi_client_t *client,
                     nethub_wifi_ap_info_t *aps,
                     int max_count,
                     int *actual_count,
                     int timeout_ms)
{
    nethub_ipc_scan_req_t req;
    nethub_ipc_response_t resp;
    int ret;

    if (!aps || max_count <= 0 || !actual_count || timeout_ms < 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    memset(&req, 0, sizeof(req));
    req.max_count = max_count;
    req.timeout_ms = timeout_ms;
    ret = client_request(client, NETHUB_CMD_SCAN, &req, sizeof(req), &resp);
    if (ret == NETHUB_WIFI_OK || ret == NETHUB_WIFI_ERR_BUFFER_TOO_SMALL) {
        if (resp.payload_len != sizeof(nethub_ipc_scan_resp_t) || !resp.payload) {
            ret = NETHUB_WIFI_ERR_PROTOCOL;
        } else {
            const nethub_ipc_scan_resp_t *scan = (const nethub_ipc_scan_resp_t *)resp.payload;
            int copy_count = scan->actual_count;
            if (copy_count > max_count) {
                copy_count = max_count;
                ret = NETHUB_WIFI_ERR_BUFFER_TOO_SMALL;
            }
            if (copy_count > NETHUB_WIFI_SCAN_MAX_APS) {
                copy_count = NETHUB_WIFI_SCAN_MAX_APS;
            }
            memcpy(aps, scan->aps, (size_t)copy_count * sizeof(aps[0]));
            *actual_count = scan->actual_count;
        }
    }
    nethub_ipc_response_deinit(&resp);
    return ret;
}

int nethub_wifi_ap_start(nethub_wifi_client_t *client,
                         const nethub_wifi_ap_config_t *cfg,
                         int timeout_ms)
{
    nethub_ipc_ap_start_req_t req;
    nethub_ipc_response_t resp;
    int ret;

    if (!cfg || timeout_ms < 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    memset(&req, 0, sizeof(req));
    req.cfg = *cfg;
    req.timeout_ms = timeout_ms;
    ret = client_request(client, NETHUB_CMD_START_AP, &req, sizeof(req), &resp);
    nethub_ipc_response_deinit(&resp);
    return ret;
}

int nethub_wifi_ap_stop(nethub_wifi_client_t *client)
{
    nethub_ipc_response_t resp;
    int ret = client_request(client, NETHUB_CMD_STOP_AP, NULL, 0, &resp);

    nethub_ipc_response_deinit(&resp);
    return ret;
}

int nethub_wifi_ota_update(nethub_wifi_client_t *client, const char *fw_path, int timeout_ms)
{
    nethub_ipc_ota_req_t req;
    nethub_ipc_response_t resp;
    char resolved[NETHUB_IPC_OTA_PATH_MAX];
    int ret;

    if (!fw_path || fw_path[0] == '\0' || timeout_ms < 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    if (!realpath(fw_path, resolved)) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }
    if (strlen(resolved) >= sizeof(req.path)) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    memset(&req, 0, sizeof(req));
    req.timeout_ms = timeout_ms;
    memcpy(req.path, resolved, strlen(resolved) + 1u);

    ret = client_request(client, NETHUB_CMD_OTA, &req, sizeof(req), &resp);
    nethub_ipc_response_deinit(&resp);
    return ret;
}
