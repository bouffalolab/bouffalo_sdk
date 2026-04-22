/**
  ******************************************************************************
  * @file    tcp_fota.c
  * @version V1.0
  * @date
  * @brief   TCP OTA transport layer for fota
  ******************************************************************************
  */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <FreeRTOS.h>
#include <task.h>

#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "tcp_fota.h"
#include "ota/ota.h"

#undef DBG_TAG
#define DBG_TAG "TCP_FOTA"
#include "log.h"

#define TCP_FOTA_RECV_SIZE  512

#define TCP_FOTA_STATUS_CALLBACK(handle, s)                   \
        if (handle->status != s) {                            \
            handle->status  = s;                              \
            if (handle->cb) {                                 \
                handle->cb(handle->cb_arg, handle->status);   \
            }                                                 \
        }

struct tcp_fota {
    char *ip;
    char *port;
    ota_handle_t ota;
    struct tcp_fota_config config;
    pfn_tcp_fota_t cb;
    void *cb_arg;
    uint8_t status;
    uint8_t auto_finish;
    int sock;
};

/**
 * @brief Connect to OTA server with retry logic
 */
static int tcp_fota_connect(struct tcp_fota *fota)
{
    struct sockaddr_in remote_addr;
    struct hostent *hostinfo;
    const char *port = fota->port ? fota->port : TCP_FOTA_DEFAULT_PORT;

    hostinfo = gethostbyname(fota->ip);
    if (!hostinfo) {
        LOG_E("gethostbyname failed for %s\r\n", fota->ip);
        return -1;
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(atoi(port));
    remote_addr.sin_addr = *((struct in_addr *)hostinfo->h_addr);

    LOG_I("Server: %s:%s\r\n", inet_ntoa(remote_addr.sin_addr.s_addr), port);

    fota->sock = -1;

    for (int retry = 0; retry < TCP_FOTA_MAX_RETRY; retry++) {
        LOG_I("Connection attempt %d/%d\r\n", retry + 1, TCP_FOTA_MAX_RETRY);

        fota->sock = socket(AF_INET, SOCK_STREAM, 0);
        if (fota->sock < 0) {
            LOG_E("Create socket failed\r\n");
        } else {
            if (connect(fota->sock, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == 0) {
                LOG_I("Connected to server\r\n");
                return 0;
            }
            LOG_W("Connection attempt %d failed\r\n", retry + 1);
            closesocket(fota->sock);
            fota->sock = -1;
        }

        if (retry < TCP_FOTA_MAX_RETRY - 1) {
            vTaskDelay(pdMS_TO_TICKS(TCP_FOTA_RETRY_DELAY_MS));
        }
    }

    LOG_E("Failed to connect after %d attempts\r\n", TCP_FOTA_MAX_RETRY);
    return -1;
}

/**
 * @brief Read exactly 'len' bytes from socket
 */
static int __attribute__((unused)) tcp_fota_recv(int sock, uint8_t *buf, uint32_t len)
{
    int ret;
    uint32_t received = 0;

    while (received < len) {
        ret = read(sock, buf + received, len - received);
        if (ret < 0) {
            LOG_E("read error ret=%d errno=%d\r\n", ret, errno);
            return -1;
        } else if (ret == 0) {
            break;
        }
        received += ret;
    }

    return received;
}

/**
 * @brief TCP FOTA service task - download and apply firmware
 */
static void tcp_fota_service(void *arg)
{
    struct tcp_fota *fota = (struct tcp_fota *)arg;
    uint8_t *recv_buf = NULL;
    int ret;
    uint32_t total_recv = 0;

    TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_START)

    /* Connect to server */
    ret = tcp_fota_connect(fota);
    if (ret != 0) {
        TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_SERVER_CONNECT_FAIL)
        goto _exit;
    }

    /* Allocate receive buffer */
    recv_buf = malloc(TCP_FOTA_RECV_SIZE);
    if (!recv_buf) {
        LOG_E("Failed to allocate recv buffer\r\n");
        TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_ABORT)
        goto _exit;
    }

    TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_PROCESS_TRANSFER)

    /* Receive data and feed to ota core */
    while (1) {
        ret = read(fota->sock, recv_buf, TCP_FOTA_RECV_SIZE);
        if (ret < 0) {
            LOG_E("read error ret=%d errno=%d\r\n", ret, errno);
            TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_ABORT)
            goto _exit;
        } else if (ret == 0) {
            /* Connection closed by server - transfer complete */
            break;
        }

        total_recv += ret;
        LOG_I("recv total=%u, this=%d\r\n", total_recv, ret);

        /* Feed data to OTA core (handles header parsing, flash write, SHA256) */
        if (ota_update(fota->ota, recv_buf, ret) != 0) {
            LOG_E("ota_update failed\r\n");
            TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_ABORT)
            goto _exit;
        }
    }

    TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_TRANSFER_FINISH)
    LOG_I("Transfer complete, total=%u bytes\r\n", total_recv);

    /* Auto-finish if configured (for the convenience tcp_fota() call) */
    if (fota->auto_finish && fota->status == TCP_FOTA_TRANSFER_FINISH) {
        tcp_fota_finish(fota, 1);
    }

_exit:
    if (recv_buf) {
        free(recv_buf);
    }
    if (fota->sock >= 0) {
        closesocket(fota->sock);
        fota->sock = -1;
    }

    /* If not auto_finish and failed, abort */
    if (!fota->auto_finish && fota->status != TCP_FOTA_TRANSFER_FINISH &&
        fota->status != TCP_FOTA_SUCCESS) {
        tcp_fota_abort(fota);
    }

    vTaskDelete(NULL);
}

tcp_fota_handle_t tcp_fota_init(const char *ip, const char *port, const struct tcp_fota_config *config)
{
    struct tcp_fota *fota;

    if (!ip) {
        LOG_E("Server IP is required\r\n");
        return NULL;
    }

    fota = calloc(sizeof(struct tcp_fota), 1);
    if (!fota) {
        LOG_E("TCP FOTA alloc fail\r\n");
        return NULL;
    }

    fota->ota = ota_start();
    if (!fota->ota) {
        free(fota);
        return NULL;
    }

    fota->ip = strdup(ip);
    fota->port = port ? strdup(port) : NULL;
    fota->sock = -1;

    if (config) {
        fota->config = *config;
        fota->cb = config->callback;
        fota->cb_arg = config->user_arg;
    }

    return fota;
}

int tcp_fota_start(tcp_fota_handle_t handle)
{
    int ret;

    ret = xTaskCreate(tcp_fota_service, "tcp_fota", 2048, handle, 10, NULL);
    if (ret != pdPASS) {
        LOG_E("TCP FOTA task create fail ret:%d\r\n", ret);
        return -1;
    }

    return 0;
}

int tcp_fota_finish(tcp_fota_handle_t handle, bool reboot)
{
    int ret = 0;
    struct tcp_fota *fota = (struct tcp_fota *)handle;

    if (fota->status != TCP_FOTA_TRANSFER_FINISH) {
        LOG_E("Transfer not completed\r\n");
        return -TCP_FOTA_PROCESS_TRANSFER;
    }

    TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_IMAGE_VERIFY)

    ret = ota_finish(fota->ota, 1, reboot);
    if (ret != 0) {
        TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_IMAGE_VERIFY_FAIL)
        ota_abort(fota->ota);
        ret = -TCP_FOTA_IMAGE_VERIFY_FAIL;
        goto __end;
    }

    TCP_FOTA_STATUS_CALLBACK(fota, TCP_FOTA_SUCCESS)
    ret = TCP_FOTA_SUCCESS;

__end:
    free(fota->ip);
    free(fota->port);
    free(fota);
    return ret;
}

int tcp_fota_abort(tcp_fota_handle_t handle)
{
    struct tcp_fota *fota = (struct tcp_fota *)handle;

    if (fota->sock >= 0) {
        closesocket(fota->sock);
        fota->sock = -1;
    }

    ota_abort(fota->ota);
    free(fota->ip);
    free(fota->port);
    free(fota);
    return 0;
}

int tcp_fota(const char *ip, const char *port, const struct tcp_fota_config *config)
{
    int ret;
    struct tcp_fota *handle = (struct tcp_fota *)tcp_fota_init(ip, port, config);

    if (!handle) {
        LOG_E("TCP FOTA init fail\r\n");
        return -TCP_FOTA_ABORT;
    }

    handle->auto_finish = 1;
    ret = tcp_fota_start(handle);
    if (ret != 0) {
        tcp_fota_abort(handle);
        return -TCP_FOTA_ABORT;
    }

    return 0;
}

int tcp_ota_rollback(void)
{
    return ota_rollback();
}
