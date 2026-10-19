#ifndef NETHUB_IPC_H
#define NETHUB_IPC_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "nethub_cmd.h"
#include "nethub_wifi_types.h"

#define NETHUB_IPC_MAGIC       0x4e485749u /* "NHWI" */
#define NETHUB_IPC_VERSION     1u
#define NETHUB_IPC_MAX_PAYLOAD 16384u

#ifndef PATH_MAX
#define NETHUB_IPC_OTA_PATH_MAX 4096
#else
#define NETHUB_IPC_OTA_PATH_MAX PATH_MAX
#endif

typedef uint16_t nethub_ipc_msg_type_t;

#define NETHUB_IPC_MSG_REQUEST  ((nethub_ipc_msg_type_t)1u)
#define NETHUB_IPC_MSG_RESPONSE ((nethub_ipc_msg_type_t)2u)

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t msg_type;
    uint16_t command;
    uint16_t flags;
    uint32_t payload_len;
    int32_t status;
} nethub_ipc_header_t;

typedef struct {
    nethub_cmd_id_t command;
    void *payload;
    size_t payload_len;
} nethub_ipc_request_t;

typedef struct {
    int status;
    void *payload;
    size_t payload_len;
} nethub_ipc_response_t;

typedef struct {
    nethub_wifi_sta_config_t cfg;
    int32_t timeout_ms;
} nethub_ipc_sta_connect_req_t;

typedef struct {
    int32_t max_count;
    int32_t timeout_ms;
} nethub_ipc_scan_req_t;

typedef struct {
    int32_t actual_count;
    nethub_wifi_ap_info_t aps[NETHUB_WIFI_SCAN_MAX_APS];
} nethub_ipc_scan_resp_t;

typedef struct {
    nethub_wifi_ap_config_t cfg;
    int32_t timeout_ms;
} nethub_ipc_ap_start_req_t;

typedef struct {
    int32_t timeout_ms;
    char path[NETHUB_IPC_OTA_PATH_MAX];
} nethub_ipc_ota_req_t;

int nethub_ipc_send_request(int fd, nethub_cmd_id_t command, const void *payload, size_t payload_len);
int nethub_ipc_recv_request(int fd, nethub_ipc_request_t *request);
void nethub_ipc_request_deinit(nethub_ipc_request_t *request);

int nethub_ipc_send_response(int fd, nethub_cmd_id_t command, int status, const void *payload, size_t payload_len);
int nethub_ipc_recv_response(int fd, nethub_ipc_response_t *response);
void nethub_ipc_response_deinit(nethub_ipc_response_t *response);

#endif /* NETHUB_IPC_H */
