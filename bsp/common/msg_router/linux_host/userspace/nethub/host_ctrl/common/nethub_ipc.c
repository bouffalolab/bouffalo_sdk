#include "nethub_ipc.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int ipc_write_all(int fd, const void *buf, size_t len)
{
    const uint8_t *cursor = (const uint8_t *)buf;

    while (len > 0) {
        ssize_t written = send(fd, cursor, len, MSG_NOSIGNAL);
        if (written < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (written == 0) {
            return -1;
        }
        cursor += (size_t)written;
        len -= (size_t)written;
    }

    return 0;
}

static int ipc_read_all(int fd, void *buf, size_t len)
{
    uint8_t *cursor = (uint8_t *)buf;

    while (len > 0) {
        ssize_t nread = recv(fd, cursor, len, 0);
        if (nread < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (nread == 0) {
            return -1;
        }
        cursor += (size_t)nread;
        len -= (size_t)nread;
    }

    return 0;
}

static int ipc_validate_header(const nethub_ipc_header_t *hdr, nethub_ipc_msg_type_t expected)
{
    if (!hdr) {
        return -1;
    }

    if (hdr->magic != NETHUB_IPC_MAGIC ||
        hdr->version != NETHUB_IPC_VERSION ||
        hdr->msg_type != expected ||
        hdr->payload_len > NETHUB_IPC_MAX_PAYLOAD) {
        return -1;
    }

    if (expected == NETHUB_IPC_MSG_REQUEST &&
        (hdr->command == 0u || hdr->command >= NETHUB_CMD_COUNT)) {
        return -1;
    }

    return 0;
}

static int ipc_send_msg(int fd,
                        nethub_ipc_msg_type_t type,
                        nethub_cmd_id_t command,
                        int status,
                        const void *payload,
                        size_t payload_len)
{
    nethub_ipc_header_t hdr;

    if (fd < 0 || payload_len > NETHUB_IPC_MAX_PAYLOAD || (payload_len > 0 && !payload)) {
        return -1;
    }

    memset(&hdr, 0, sizeof(hdr));
    hdr.magic = NETHUB_IPC_MAGIC;
    hdr.version = NETHUB_IPC_VERSION;
    hdr.msg_type = type;
    hdr.command = (uint16_t)command;
    hdr.payload_len = (uint32_t)payload_len;
    hdr.status = status;

    if (ipc_write_all(fd, &hdr, sizeof(hdr)) != 0) {
        return -1;
    }

    if (payload_len > 0 && ipc_write_all(fd, payload, payload_len) != 0) {
        return -1;
    }

    return 0;
}

int nethub_ipc_send_request(int fd, nethub_cmd_id_t command, const void *payload, size_t payload_len)
{
    return ipc_send_msg(fd, NETHUB_IPC_MSG_REQUEST, command, 0, payload, payload_len);
}

int nethub_ipc_recv_request(int fd, nethub_ipc_request_t *request)
{
    nethub_ipc_header_t hdr;
    void *payload = NULL;

    if (fd < 0 || !request) {
        return -1;
    }

    memset(request, 0, sizeof(*request));
    if (ipc_read_all(fd, &hdr, sizeof(hdr)) != 0 ||
        ipc_validate_header(&hdr, NETHUB_IPC_MSG_REQUEST) != 0) {
        return -1;
    }

    if (hdr.payload_len > 0) {
        payload = malloc(hdr.payload_len);
        if (!payload) {
            return -1;
        }
        if (ipc_read_all(fd, payload, hdr.payload_len) != 0) {
            free(payload);
            return -1;
        }
    }

    request->command = (nethub_cmd_id_t)hdr.command;
    request->payload = payload;
    request->payload_len = hdr.payload_len;
    return 0;
}

void nethub_ipc_request_deinit(nethub_ipc_request_t *request)
{
    if (!request) {
        return;
    }
    free(request->payload);
    memset(request, 0, sizeof(*request));
}

int nethub_ipc_send_response(int fd, nethub_cmd_id_t command, int status, const void *payload, size_t payload_len)
{
    return ipc_send_msg(fd, NETHUB_IPC_MSG_RESPONSE, command, status, payload, payload_len);
}

int nethub_ipc_recv_response(int fd, nethub_ipc_response_t *response)
{
    nethub_ipc_header_t hdr;
    void *payload = NULL;

    if (fd < 0 || !response) {
        return -1;
    }

    memset(response, 0, sizeof(*response));
    if (ipc_read_all(fd, &hdr, sizeof(hdr)) != 0 ||
        ipc_validate_header(&hdr, NETHUB_IPC_MSG_RESPONSE) != 0) {
        return -1;
    }

    if (hdr.payload_len > 0) {
        payload = calloc((size_t)hdr.payload_len + 1u, 1u);
        if (!payload) {
            return -1;
        }
        if (ipc_read_all(fd, payload, hdr.payload_len) != 0) {
            free(payload);
            return -1;
        }
    }

    response->status = hdr.status;
    response->payload = payload;
    response->payload_len = hdr.payload_len;
    return 0;
}

void nethub_ipc_response_deinit(nethub_ipc_response_t *response)
{
    if (!response) {
        return;
    }
    free(response->payload);
    memset(response, 0, sizeof(*response));
}
