#include "bflbwifi_ipc.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int ipc_write_all(int fd, const void *buf, size_t len)
{
    const uint8_t *cursor = (const uint8_t *)buf;

    while (len > 0) {
        ssize_t written = send(fd, cursor, len, 0);
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

static int ipc_validate_header(const bflbwifi_ipc_header_t *header, bflbwifi_ipc_msg_type_t expected_type)
{
    if (!header) {
        return -1;
    }

    if (header->magic != BFLBWIFI_IPC_MAGIC ||
        header->version != BFLBWIFI_IPC_VERSION ||
        header->msg_type != expected_type) {
        return -1;
    }

    if (header->payload_len > MAX_BUFFER_SIZE) {
        return -1;
    }

    if (expected_type == BFLBWIFI_IPC_MSG_REQUEST &&
        (header->argc <= 0 || header->argc > BFLBWIFI_IPC_MAX_ARGS)) {
        return -1;
    }

    if (expected_type == BFLBWIFI_IPC_MSG_RESPONSE && header->argc != 0u) {
        return -1;
    }

    return 0;
}

static int ipc_derive_status_from_payload(const char *payload)
{
    int status = -1;

    if (!payload || payload[0] == '\0') {
        return -1;
    }

    if (strncmp(payload, "OK", 2) == 0) {
        return 0;
    }

    if (sscanf(payload, "ERROR %d", &status) == 1) {
        return status;
    }

    return -1;
}

int bflbwifi_ipc_send_request(int fd, int argc, const char *const argv[])
{
    bflbwifi_ipc_header_t header;
    size_t payload_len = 0;
    char *payload;
    char *cursor;

    if (fd < 0 || argc <= 0 || argc > BFLBWIFI_IPC_MAX_ARGS || !argv) {
        return -1;
    }

    for (int i = 0; i < argc; i++) {
        if (!argv[i]) {
            return -1;
        }
        payload_len += strlen(argv[i]) + 1u;
    }

    if (payload_len == 0 || payload_len > MAX_BUFFER_SIZE) {
        return -1;
    }

    payload = (char *)malloc(payload_len);
    if (!payload) {
        return -1;
    }

    cursor = payload;
    for (int i = 0; i < argc; i++) {
        size_t arg_len = strlen(argv[i]) + 1u;
        memcpy(cursor, argv[i], arg_len);
        cursor += arg_len;
    }

    memset(&header, 0, sizeof(header));
    header.magic = BFLBWIFI_IPC_MAGIC;
    header.version = BFLBWIFI_IPC_VERSION;
    header.msg_type = BFLBWIFI_IPC_MSG_REQUEST;
    header.payload_len = (uint32_t)payload_len;
    header.argc = (uint16_t)argc;

    if (ipc_write_all(fd, &header, sizeof(header)) != 0 ||
        ipc_write_all(fd, payload, payload_len) != 0) {
        free(payload);
        return -1;
    }

    free(payload);
    return 0;
}

int bflbwifi_ipc_recv_request(int fd, bflbwifi_ipc_request_t *request)
{
    bflbwifi_ipc_header_t header;
    char *payload;
    char **argv;
    size_t offset = 0;

    if (fd < 0 || !request) {
        return -1;
    }

    memset(request, 0, sizeof(*request));

    if (ipc_read_all(fd, &header, sizeof(header)) != 0 ||
        ipc_validate_header(&header, BFLBWIFI_IPC_MSG_REQUEST) != 0) {
        return -1;
    }

    payload = (char *)malloc((size_t)header.payload_len);
    argv = (char **)calloc(header.argc, sizeof(char *));
    if (!payload || !argv) {
        free(payload);
        free(argv);
        return -1;
    }

    if (ipc_read_all(fd, payload, header.payload_len) != 0) {
        free(payload);
        free(argv);
        return -1;
    }

    for (uint16_t i = 0; i < header.argc; i++) {
        size_t arg_len;

        if (offset >= header.payload_len) {
            free(payload);
            free(argv);
            return -1;
        }

        arg_len = strnlen(payload + offset, header.payload_len - offset);
        if (offset + arg_len >= header.payload_len) {
            free(payload);
            free(argv);
            return -1;
        }

        argv[i] = payload + offset;
        offset += arg_len + 1u;
    }

    if (offset != header.payload_len) {
        free(payload);
        free(argv);
        return -1;
    }

    request->argc = (int)header.argc;
    request->argv = argv;
    request->payload = payload;
    request->payload_len = header.payload_len;
    return 0;
}

void bflbwifi_ipc_request_deinit(bflbwifi_ipc_request_t *request)
{
    if (!request) {
        return;
    }

    free(request->argv);
    free(request->payload);
    memset(request, 0, sizeof(*request));
}

int bflbwifi_ipc_send_response(int fd, int status, const char *payload)
{
    bflbwifi_ipc_header_t header;
    size_t payload_len = payload ? strlen(payload) : 0u;

    if (fd < 0 || payload_len > MAX_RESPONSE_LEN) {
        return -1;
    }

    if (payload && status == 0) {
        status = ipc_derive_status_from_payload(payload);
    }

    memset(&header, 0, sizeof(header));
    header.magic = BFLBWIFI_IPC_MAGIC;
    header.version = BFLBWIFI_IPC_VERSION;
    header.msg_type = BFLBWIFI_IPC_MSG_RESPONSE;
    header.payload_len = (uint32_t)payload_len;
    header.status = status;

    if (ipc_write_all(fd, &header, sizeof(header)) != 0) {
        return -1;
    }

    if (payload_len > 0 && ipc_write_all(fd, payload, payload_len) != 0) {
        return -1;
    }

    return 0;
}

int bflbwifi_ipc_recv_response(int fd, bflbwifi_ipc_response_t *response)
{
    bflbwifi_ipc_header_t header;
    char *payload;

    if (fd < 0 || !response) {
        return -1;
    }

    memset(response, 0, sizeof(*response));

    if (ipc_read_all(fd, &header, sizeof(header)) != 0 ||
        ipc_validate_header(&header, BFLBWIFI_IPC_MSG_RESPONSE) != 0) {
        return -1;
    }

    payload = (char *)calloc((size_t)header.payload_len + 1u, 1u);
    if (!payload) {
        return -1;
    }

    if (header.payload_len > 0 &&
        ipc_read_all(fd, payload, header.payload_len) != 0) {
        free(payload);
        return -1;
    }

    response->status = header.status;
    response->payload = payload;
    response->payload_len = header.payload_len;
    return 0;
}

void bflbwifi_ipc_response_deinit(bflbwifi_ipc_response_t *response)
{
    if (!response) {
        return;
    }

    free(response->payload);
    memset(response, 0, sizeof(*response));
}
