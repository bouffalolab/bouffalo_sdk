#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "bl_sys.h"
#include "lwip/altcp_tls.h"
#include "lwip/apps/httpd.h"
#include "lwip/pbuf.h"
#include "lwip/timeouts.h"
#include "task.h"

#include "https_fota_server.h"
#include "log.h"
#include "ota/ota.h"

#define OTA_REBOOT_DELAY_MS 3000U

#define HTTPS_FOTA_SERVER_STATUS_CALLBACK(server, value)       \
    if ((server)->status != (value)) {                         \
        (server)->status = (value);                            \
        if ((server)->config.callback) {                        \
            (server)->config.callback((server)->config.user_arg, \
                                      (value));                 \
        }                                                       \
    }

typedef enum {
    OTA_POST_START,
    OTA_POST_CHUNK,
    OTA_POST_FINISH,
    OTA_POST_ABORT,
} ota_post_action_t;

struct https_fota_server {
    char firmware_url[OTA_FIRMWARE_URL_MAX];
    struct https_fota_server_config config;
    ota_handle_t ota;
    uint32_t package_size;
    uint32_t received;
    void *post_connection;
    ota_post_action_t post_action;
    uint32_t post_expected;
    uint32_t post_received;
    uint8_t status;
    bool completed;
};

static struct https_fota_server *g_httpd_server;
static bool g_httpd_started;

static void https_fota_server_release(void *arg)
{
    struct https_fota_server *server = arg;

    if (g_httpd_server == server) {
        g_httpd_server = NULL;
    }
    free(server);
}

static void https_fota_server_session_abort(struct https_fota_server *server)
{
    if (server->ota) {
        ota_abort(server->ota);
        server->ota = NULL;
    }
}

static int https_fota_server_session_begin(struct https_fota_server *server,
                                           uint32_t package_size)
{
    if (package_size <= sizeof(ota_header_t)) {
        return -1;
    }

    server->ota = ota_start();
    if (!server->ota) {
        return -1;
    }
    if ((uint64_t)package_size >
        (uint64_t)server->ota->part_size + sizeof(ota_header_t)) {
        https_fota_server_session_abort(server);
        return -1;
    }

    server->package_size = package_size;
    HTTPS_FOTA_SERVER_STATUS_CALLBACK(server, HTTPS_FOTA_SERVER_START)
    return 0;
}

https_fota_server_handle_t https_fota_server_init(
    const char *url, const struct https_fota_server_config *config)
{
    struct https_fota_server *server;

    if (!url || strlen(url) >= OTA_FIRMWARE_URL_MAX || g_httpd_server) {
        return NULL;
    }

    server = calloc(1, sizeof(*server));
    if (!server) {
        return NULL;
    }

    strcpy(server->firmware_url, url);
    if (config) {
        server->config = *config;
    }
    if (!memchr(server->config.upload_uri, '\0', sizeof(server->config.upload_uri)) ||
        !memchr(server->config.success_uri, '\0', sizeof(server->config.success_uri)) ||
        !memchr(server->config.error_uri, '\0', sizeof(server->config.error_uri))) {
        free(server);
        return NULL;
    }
    if (!server->config.upload_uri[0]) {
        strcpy(server->config.upload_uri, HTTPS_FOTA_SERVER_DEFAULT_UPLOAD_URI);
    }
    if (!server->config.success_uri[0]) {
        strcpy(server->config.success_uri, HTTPS_FOTA_SERVER_DEFAULT_SUCCESS_URI);
    }
    if (!server->config.error_uri[0]) {
        strcpy(server->config.error_uri, HTTPS_FOTA_SERVER_DEFAULT_ERROR_URI);
    }
    return server;
}

int https_fota_server_start(https_fota_server_handle_t handle)
{
    struct https_fota_server *server = handle;
    struct altcp_tls_config *tls_config;

    if (!server || g_httpd_server) {
        return -1;
    }

    if (!g_httpd_started) {
        if (!server->config.cert_pem || !server->config.cert_len ||
            !server->config.key_pem || !server->config.key_len) {
            return -1;
        }
        tls_config = altcp_tls_create_config_server_privkey_cert(
            server->config.key_pem, server->config.key_len, NULL, 0,
            server->config.cert_pem, server->config.cert_len);
        if (!tls_config) {
            LOG_E("Failed to initialize HTTPS server certificate\r\n");
            return -1;
        }
        httpd_init();
        httpd_inits(tls_config);
        g_httpd_started = true;
    }
    g_httpd_server = server;
    LOG_I("HTTP/HTTPS server OTA firmware URL: %s\r\n",
          server->firmware_url);
    return 0;
}

int https_fota_server_update(https_fota_server_handle_t handle,
                             const uint8_t *data, uint32_t size)
{
    struct https_fota_server *server = handle;

    if (!server) {
        return -1;
    }
    if (!server->ota || !data || !size ||
        size > server->package_size - server->received) {
        https_fota_server_abort(server);
        return -1;
    }

    HTTPS_FOTA_SERVER_STATUS_CALLBACK(server,
                                      HTTPS_FOTA_SERVER_PROCESS_TRANSFER)
    if (ota_update(server->ota, (uint8_t *)data, size) != 0) {
        https_fota_server_abort(server);
        return -1;
    }
    server->received += size;
    return 0;
}

int https_fota_server_callback_register(https_fota_server_handle_t handle,
                                        pfn_https_fota_server_t callback,
                                        void *user_arg)
{
    struct https_fota_server *server = handle;

    if (!server) {
        return -1;
    }
    server->config.callback = callback;
    server->config.user_arg = user_arg;
    return 0;
}

static void reboot_task(void *param)
{
    (void)param;
    vTaskDelay(pdMS_TO_TICKS(OTA_REBOOT_DELAY_MS));
    bl_sys_reset_por();
    vTaskDelete(NULL);
}

int https_fota_server_finish(https_fota_server_handle_t handle, bool reboot)
{
    struct https_fota_server *server = handle;
    int ret;

    if (!server) {
        return -1;
    }
    if (!server->ota || server->received != server->package_size ||
        (uint64_t)server->package_size !=
            (uint64_t)sizeof(ota_header_t) + server->ota->file_size) {
        https_fota_server_abort(server);
        return -1;
    }

    HTTPS_FOTA_SERVER_STATUS_CALLBACK(server,
                                      HTTPS_FOTA_SERVER_TRANSFER_FINISH)
    HTTPS_FOTA_SERVER_STATUS_CALLBACK(server, HTTPS_FOTA_SERVER_IMAGE_VERIFY)
    ret = ota_finish(server->ota, 1, 0);
    if (ret != 0) {
        https_fota_server_session_abort(server);
        HTTPS_FOTA_SERVER_STATUS_CALLBACK(
            server, HTTPS_FOTA_SERVER_IMAGE_VERIFY_FAIL)
    } else {
        server->ota = NULL;
        HTTPS_FOTA_SERVER_STATUS_CALLBACK(server, HTTPS_FOTA_SERVER_SUCCESS)
        LOG_I("FOTA server accepted\r\n");
        if (reboot && xTaskCreate(reboot_task, "ota_reboot", 512, NULL, 12, NULL) != pdPASS) {
            LOG_E("Failed to schedule OTA reboot\r\n");
        }
    }
    server->completed = true;
    if (!server->post_connection) {
        https_fota_server_release(server);
    }
    return ret;
}

int https_fota_server_abort(https_fota_server_handle_t handle)
{
    struct https_fota_server *server = handle;

    if (!server) {
        return -1;
    }
    HTTPS_FOTA_SERVER_STATUS_CALLBACK(server, HTTPS_FOTA_SERVER_ABORT)
    https_fota_server_session_abort(server);
    server->completed = true;
    if (!server->post_connection) {
        https_fota_server_release(server);
    }
    return 0;
}

static int upload_uri_matches(const struct https_fota_server *server,
                              const char *uri, const char *suffix)
{
    char path[HTTPS_FOTA_SERVER_URI_MAX + 16];
    snprintf(path, sizeof(path), "%s%s", server->config.upload_uri, suffix);
    return !strcmp(uri, path);
}

static int parse_upload_size(const struct https_fota_server *server,
                             const char *uri, uint32_t *size)
{
    char prefix[HTTPS_FOTA_SERVER_URI_MAX + 16];
    char *end;
    unsigned long value;
    int len = snprintf(prefix, sizeof(prefix), "%s/start?size=",
                       server->config.upload_uri);

    if (strncmp(uri, prefix, (size_t)len) != 0) {
        return -1;
    }
    value = strtoul(uri + len, &end, 10);
    if (*end || end == uri + len || value > UINT32_MAX) {
        return -1;
    }
    *size = (uint32_t)value;
    return 0;
}

err_t httpd_post_begin(void *connection, const char *uri,
                       const char *http_request, u16_t http_request_len,
                       int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)
{
    struct https_fota_server *server = g_httpd_server;
    uint32_t size = 0;

    (void)http_request;
    (void)http_request_len;
    if (!server || server->post_connection || server->completed) {
        return ERR_ARG;
    }
    snprintf(response_uri, response_uri_len, "%s", server->config.error_uri);

    if (content_len == 0 && !server->ota &&
        parse_upload_size(server, uri, &size) == 0) {
        server->post_action = OTA_POST_START;
    } else if (upload_uri_matches(server, uri, "/chunk") && content_len > 0 &&
               server->ota) {
        server->post_action = OTA_POST_CHUNK;
    } else if (upload_uri_matches(server, uri, "/finish") && content_len == 0 &&
               server->ota) {
        server->post_action = OTA_POST_FINISH;
    } else if (upload_uri_matches(server, uri, "/abort") && content_len == 0) {
        server->post_action = OTA_POST_ABORT;
    } else {
        return ERR_ARG;
    }

    server->post_connection = connection;
    server->post_expected = content_len;
    server->post_received = 0;
    *post_auto_wnd = 1;
    if (server->post_action == OTA_POST_START) {
        if (https_fota_server_session_begin(server, size) != 0) {
            https_fota_server_abort(server);
            server->post_connection = NULL;
            sys_timeout(0, https_fota_server_release, server);
            return ERR_ARG;
        }
        LOG_I("OTA upload started: %lu bytes\r\n", (unsigned long)size);
    }
    return ERR_OK;
}

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
    struct https_fota_server *server = g_httpd_server;
    struct pbuf *chunk;

    if (!server || server->post_connection != connection) {
        pbuf_free(p);
        return ERR_ARG;
    }

    for (chunk = p; chunk; chunk = chunk->next) {
        if (!chunk->len) {
            continue;
        }
        if (chunk->len > server->post_expected - server->post_received) {
            https_fota_server_abort(server);
            pbuf_free(p);
            return ERR_ARG;
        }
        if (https_fota_server_update(server, chunk->payload, chunk->len) != 0) {
            pbuf_free(p);
            return ERR_ARG;
        }
        server->post_received += chunk->len;
    }

    pbuf_free(p);
    return ERR_OK;
}

void httpd_post_finished(void *connection, char *response_uri,
                         u16_t response_uri_len)
{
    struct https_fota_server *server = g_httpd_server;
    bool success = false;

    if (!server || server->post_connection != connection) {
        if (server) {
            snprintf(response_uri, response_uri_len, "%s", server->config.error_uri);
        }
        return;
    }

    if (!server->completed) {
        success = server->post_received == server->post_expected;
        if (success && server->post_action == OTA_POST_FINISH) {
            success = https_fota_server_finish(server, server->config.reboot) == 0;
        } else if (!success || server->post_action == OTA_POST_ABORT) {
            https_fota_server_abort(server);
        }
    }

    snprintf(response_uri, response_uri_len, "%s",
             success ? server->config.success_uri : server->config.error_uri);
    server->post_connection = NULL;
    if (server->completed) {
        /* HTTPD opens the response file after this callback returns. */
        sys_timeout(0, https_fota_server_release, server);
    }
}

const char *https_fota_server_url(void)
{
    return g_httpd_server ? g_httpd_server->firmware_url : "";
}

const struct https_fota_server_config *https_fota_server_get_config(void)
{
    return g_httpd_server ? &g_httpd_server->config : NULL;
}
