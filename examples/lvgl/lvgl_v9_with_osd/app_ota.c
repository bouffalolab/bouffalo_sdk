#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "FreeRTOS.h"
#include "task.h"
#include "bflb_flash.h"
#include "bl_sys.h"
#include "https_client.h"
#include "ota/ota.h"
#include "shell.h"
#include "wifi_mgmr_ext.h"
#include "app_ota.h"
#include "dpi_manager.h"

#ifdef CONFIG_IPC
#include "bflb_multi_core_sync.h"
#include "flash_ops_rpmsg.h"
#endif

#define APP_OTA_URL_SIZE       512U
#define APP_OTA_FIRST_SIZE     (sizeof(ota_header_t) + 12U)
#define APP_OTA_BUFFER_SIZE    4096U
#define APP_OTA_TIMEOUT_MS     120000U
#define APP_OTA_STACK_WORDS    2048U
#define APP_OTA_PRIORITY       12U

struct app_ota_session {
    char url[APP_OTA_URL_SIZE];
    uint8_t first[APP_OTA_FIRST_SIZE];
    uint32_t first_size;
    uint32_t received;
    uint32_t total;
    uint32_t next_log;
    ota_handle_t ota;
    int error;
    bool headers_ok;
    bool final_received;
    bool reboot;
};

volatile app_ota_status_t g_ota_status;

const char *app_ota_phase_name(app_ota_phase_t phase)
{
    static const char *const names[] = {
        "idle", "connecting", "downloading", "verifying",
        "success", "failed", "rebooting"
    };

    return (unsigned int)phase < sizeof(names) / sizeof(names[0]) ? names[phase] : "unknown";
}

static void ota_phase(app_ota_phase_t phase, int error)
{
    g_ota_status.phase = phase;
    g_ota_status.error = error;
    printf("[APP_OTA] %s, error=%d\r\n", app_ota_phase_name(phase), error);
}

static void ota_error(struct app_ota_session *session, int error, const char *reason)
{
    if (session->error == 0) {
        session->error = error;
        printf("[APP_OTA] %s (%d)\r\n", reason, error);
    }
}

static int ota_validate_first(struct app_ota_session *session)
{
    const ota_header_t *header = (const ota_header_t *)session->first;
    pt_table_stuff_config tables[2];
    pt_table_id_type active;
    pt_table_entry_config fw;
    uint32_t body_size, destination, size, running, running_size, flash_size;
    unsigned int inactive;

    if (memcmp(header->u.s.header, "BL60X_OTA_Ver1.0", 16) != 0) {
        ota_error(session, -EINVAL, "Invalid OTA header; use the generated .bin.ota file");
        return -1;
    }
    if (memcmp(header->u.s.type, "RAW ", 4) != 0) {
        ota_error(session, -ENOTSUP, "Only RAW .bin.ota is supported; XZ is disabled to preserve the old slot");
        return -1;
    }
    if (memcmp(header->u.s.ver_hardware, "BFL_Module_v1.1", 15) != 0) {
        ota_error(session, -EINVAL, "Unsupported OTA hardware header");
        return -1;
    }
    memcpy(&body_size, &header->u.s.len, sizeof(body_size));
    if (body_size < 512U ||
        body_size != session->total - sizeof(ota_header_t) ||
        memcmp(session->first + sizeof(ota_header_t), "BFNP", 4) != 0 ||
        memcmp(session->first + sizeof(ota_header_t) + 8, "FCFG", 4) != 0) {
        ota_error(session, -EINVAL, "OTA length or firmware boot header is invalid");
        return -1;
    }
    active = pt_table_get_active_partition_from_ram(tables);
    if (active == PT_TABLE_ID_INVALID ||
        pt_table_get_active_entries_by_name(&tables[active], (uint8_t *)"FW", &fw) != PT_ERROR_SUCCESS ||
        fw.active_index > 1U) {
        ota_error(session, -EINVAL, "Cannot read OTA destination");
        return -1;
    }
    inactive = !fw.active_index;
    destination = fw.start_address[inactive];
    size = fw.max_len[inactive];
    running = fw.start_address[fw.active_index];
    running_size = fw.max_len[fw.active_index];
    flash_size = bflb_flash_get_size();
    if (fw.device != 0 || destination < 0x10000U || running < 0x10000U ||
        (destination & 0xfffU) != 0 || size == 0 || running_size == 0 ||
        destination >= flash_size || size > flash_size - destination ||
        running >= flash_size || running_size > flash_size - running ||
        (destination < running + running_size && running < destination + size)) {
        ota_error(session, -EINVAL, "Unsafe or overlapping firmware slots");
        return -1;
    }
    if (body_size > size) {
        ota_error(session, -EFBIG, "Firmware exceeds the inactive slot");
        return -1;
    }
    printf("[APP_OTA] RAW body=%lu, software=%.16s, destination=0x%08lx (%lu bytes)\r\n",
           (unsigned long)body_size, header->u.s.ver_software,
           (unsigned long)destination, (unsigned long)size);
    session->ota = ota_start();
    if (session->ota == NULL) {
        ota_error(session, -EIO, "Cannot open inactive OTA slot");
        return -1;
    }
    /* Include a few payload bytes: SDK ota_update expects a nonempty first write. */
    if (ota_update(session->ota, session->first, sizeof(session->first)) != 0) {
        ota_error(session, -EIO, "Failed to write first firmware fragment");
        return -1;
    }
    return 0;
}

static int ota_http_headers(struct http_parser *parser)
{
    struct http_request *request = (struct http_request *)((uint8_t *)parser -
                                      offsetof(struct http_request, internal.parser));
    struct app_ota_session *session = request->internal.user_data;

    if (parser->status_code != 200) {
        ota_error(session, -EIO, "HTTP response must be 200 (redirects are unsupported)");
    } else if ((parser->flags & F_CONTENTLENGTH) == 0 || (parser->flags & F_CHUNKED) != 0 ||
               parser->content_length <= APP_OTA_FIRST_SIZE || parser->content_length > UINT32_MAX) {
        ota_error(session, -EINVAL, "A fixed, valid Content-Length is required");
    } else {
        session->total = (uint32_t)parser->content_length;
        session->headers_ok = true;
        /* The parser retains the complete value when a header is split across receives. */
        request->internal.response.content_length = session->total;
        g_ota_status.total = session->total;
        ota_phase(APP_OTA_DOWNLOADING, 0);
    }
    return 0;
}

static int ota_http_cancel(void *user_data)
{
    struct app_ota_session *session = user_data;

    if (session->error == 0 && !wifi_mgmr_sta_state_get()) {
        ota_error(session, -ENETDOWN, "Wi-Fi disconnected during OTA");
    }
    return session->error != 0;
}

static void ota_http_response(struct http_response *response,
                              enum http_final_call final_data, void *user_data)
{
    struct app_ota_session *session = user_data;
    uint8_t *data;
    size_t length, copied;

    if (session->error != 0) {
        return;
    }
    if (response == NULL || !session->headers_ok) {
        if (final_data == HTTP_DATA_FINAL) {
            ota_error(session, -EIO, "Missing HTTP response headers");
        }
        return;
    }
    data = response->body_frag_start;
    length = response->body_frag_len;
    if (length > session->total - session->received || (length != 0 && data == NULL)) {
        ota_error(session, -EINVAL, "HTTP body exceeds the declared length");
        return;
    }
    session->received += length;
    if (session->first_size < sizeof(session->first)) {
        copied = sizeof(session->first) - session->first_size;
        if (copied > length) {
            copied = length;
        }
        if (copied != 0) {
            memcpy(session->first + session->first_size, data, copied);
            session->first_size += copied;
            data += copied;
            length -= copied;
        }
        if (session->first_size == sizeof(session->first) && ota_validate_first(session) != 0) {
            return;
        }
    }
    if (length != 0 && ota_update(session->ota, data, length) != 0) {
        ota_error(session, -EIO, "Firmware flash write failed");
        return;
    }
    g_ota_status.received = session->received;
    if (session->received >= session->next_log || final_data == HTTP_DATA_FINAL) {
        printf("[APP_OTA] received %lu/%lu bytes (%lu%%)\r\n",
               (unsigned long)session->received, (unsigned long)session->total,
               (unsigned long)((uint64_t)session->received * 100U / session->total));
        session->next_log = session->received + 65536U;
    }
    if (final_data == HTTP_DATA_FINAL) {
        session->final_received = session->received == session->total;
        if (!session->final_received) {
            ota_error(session, -EIO, "Firmware download is incomplete");
        }
    }
}

static void ota_reset(void)
{
    ota_phase(APP_OTA_REBOOTING, 0);
    vTaskDelay(pdMS_TO_TICKS(150));
#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
    bflb_sys_reboot_mcs();
#else
    bflb_reboot_rpmsg();
#endif
#else
    bl_sys_reset_por();
#endif
}

static void ota_worker(void *arg)
{
    static const struct http_parser_settings parser_callbacks = {
        .on_headers_complete = ota_http_headers,
    };
    struct app_ota_session *session = arg;
    struct https_client_request request = {0};
    bool success = false;
    bool reboot = session->reboot;
    int ret;

    ret = video_ota_pause();
    if (ret != 0) {
        ota_error(session, ret, "Cannot pause application for OTA");
        goto done;
    }
    request.method = HTTP_GET;
    request.url = session->url;
    request.protocol = "HTTP/1.1";
    request.response = ota_http_response;
    request.http_cb = &parser_callbacks;
    request.buffer_size = APP_OTA_BUFFER_SIZE;
    request.cancel = ota_http_cancel;
    ret = https_client_request(&request, APP_OTA_TIMEOUT_MS, session);
    if (session->error == 0 && (ret < 0 || !session->final_received || session->ota == NULL)) {
        ota_error(session, ret < 0 ? ret : -EIO, "HTTP download did not complete");
    }
    if (session->error != 0) {
        goto done;
    }
    ota_phase(APP_OTA_VERIFYING, 0);
    if (ota_finish(session->ota, 1, 0) != 0) {
        ota_error(session, -EIO, "SHA256 readback verification or partition commit failed");
        goto done;
    }
    session->ota = NULL; /* ota_finish frees the handle only on success. */
    success = true;
done:
    if (session->ota != NULL) {
        ota_abort(session->ota);
    }
    video_ota_resume();
    ota_phase(success ? APP_OTA_SUCCESS : APP_OTA_FAILED, session->error);
    free(session);
    /* Shell commands are serialized; release busy only after cleanup. */
    g_ota_status.reboot_pending = success;
    g_ota_status.busy = success && reboot;
    if (success) {
        printf("[APP_OTA] SHA256 verified, new firmware committed; %s\r\n",
               reboot ? "reboot in one second" : "run ota_reboot to boot the new firmware");
        if (reboot) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            ota_reset();
        }
    }
    vTaskDelete(NULL);
}

static int app_ota_start(const char *url, bool reboot)
{
    struct app_ota_session *session;
    struct http_parser_url parsed;
    uint32_t ip;
    size_t length;

    if (url == NULL || strncmp(url, "http://", 7) != 0) {
        printf("[APP_OTA] Use http:// for local OTA; HTTPS trust is not configured\r\n");
        return -EINVAL;
    }
    length = strlen(url);
    if (length >= APP_OTA_URL_SIZE || strchr(url, '\r') || strchr(url, '\n') ||
        http_parser_parse_url(url, length, false, &parsed) != 0 ||
        !(parsed.field_set & (1U << UF_HOST)) || (parsed.field_set & (1U << UF_USERINFO))) {
        printf("[APP_OTA] Invalid or oversized HTTP URL\r\n");
        return -EINVAL;
    }
    if (!wifi_mgmr_sta_state_get() ||
        wifi_sta_ip4_addr_get(&ip, NULL, NULL, NULL) != 0 || ip == 0) {
        printf("[APP_OTA] Connect Wi-Fi and wait for GOT_IP first\r\n");
        return -ENETDOWN;
    }
    session = calloc(1, sizeof(*session));
    if (session == NULL) {
        return -ENOMEM;
    }
    memcpy(session->url, url, length + 1U);
    session->reboot = reboot;
    if (g_ota_status.busy || g_ota_status.reboot_pending) {
        free(session);
        printf("[APP_OTA] Busy or committed upgrade awaits reboot\r\n");
        return -EBUSY;
    }
    g_ota_status = (app_ota_status_t){ .phase = APP_OTA_CONNECTING, .busy = true };
    if (xTaskCreate(ota_worker, "app_ota", APP_OTA_STACK_WORDS, session,
                    APP_OTA_PRIORITY, NULL) != pdPASS) {
        free(session);
        ota_phase(APP_OTA_FAILED, -ENOMEM);
        g_ota_status.busy = false;
        return -ENOMEM;
    }
    return 0;
}

static int cmd_https_ota_start(int argc, char **argv)
{
    if (argc < 2 || argc > 3 ||
        (argc == 3 && strcmp(argv[2], "0") != 0 && strcmp(argv[2], "1") != 0)) {
        printf("Usage: https_ota_start <http://host/file.bin.ota> [reboot: 0|1, default: 1]\r\n");
        return -EINVAL;
    }
    return app_ota_start(argv[1], argc < 3 || strcmp(argv[2], "1") == 0);
}

static int cmd_app_ota_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("[APP_OTA] phase=%s received=%lu total=%lu error=%d busy=%u reboot_pending=%u\r\n",
           app_ota_phase_name(g_ota_status.phase), (unsigned long)g_ota_status.received,
           (unsigned long)g_ota_status.total, g_ota_status.error, g_ota_status.busy, g_ota_status.reboot_pending);
    return 0;
}

static int cmd_app_ota_reboot(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (g_ota_status.busy || !g_ota_status.reboot_pending) {
        printf("[APP_OTA] No completed staged upgrade to reboot\r\n");
        return -EINVAL;
    }
    g_ota_status.busy = true;
    ota_reset();
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_https_ota_start, https_ota_start, Download and verify RAW HTTP OTA);
SHELL_CMD_EXPORT_ALIAS(cmd_app_ota_status, ota_status, Show OTA progress);
SHELL_CMD_EXPORT_ALIAS(cmd_app_ota_reboot, ota_reboot, Boot the verified staged upgrade);
