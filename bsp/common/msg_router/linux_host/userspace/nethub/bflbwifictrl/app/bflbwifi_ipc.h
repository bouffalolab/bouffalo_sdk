/**
 * @file bflbwifi_ipc.h
 * @brief Local IPC protocol shared by bflbwifictrl and bflbwifid
 */

#ifndef BFLBWIFI_IPC_H
#define BFLBWIFI_IPC_H

#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE       16384
#define MAX_RESPONSE_LEN      16384
#define BFLBWIFI_IPC_MAX_ARGS 16

#define BFLBWIFI_IPC_MAGIC    0x42465749u /* "BFWI" */
#define BFLBWIFI_IPC_VERSION  1u

typedef uint16_t bflbwifi_ipc_msg_type_t;

#define BFLBWIFI_IPC_MSG_REQUEST  ((bflbwifi_ipc_msg_type_t)1u)
#define BFLBWIFI_IPC_MSG_RESPONSE ((bflbwifi_ipc_msg_type_t)2u)

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t msg_type;
    uint32_t payload_len;
    uint16_t argc;
    uint16_t reserved;
    int32_t status;
} bflbwifi_ipc_header_t;

typedef struct {
    int argc;
    char **argv;
    char *payload;
    size_t payload_len;
} bflbwifi_ipc_request_t;

typedef struct {
    int status;
    char *payload;
    size_t payload_len;
} bflbwifi_ipc_response_t;

int bflbwifi_ipc_send_request(int fd, int argc, const char *const argv[]);
int bflbwifi_ipc_recv_request(int fd, bflbwifi_ipc_request_t *request);
void bflbwifi_ipc_request_deinit(bflbwifi_ipc_request_t *request);

int bflbwifi_ipc_send_response(int fd, int status, const char *payload);
int bflbwifi_ipc_recv_response(int fd, bflbwifi_ipc_response_t *response);
void bflbwifi_ipc_response_deinit(bflbwifi_ipc_response_t *response);

#endif /* BFLBWIFI_IPC_H */
