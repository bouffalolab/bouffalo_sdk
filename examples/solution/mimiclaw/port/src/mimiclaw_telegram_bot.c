#include "telegram/telegram_bot.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "https_client.h"
#include "cJSON.h"
#include "esp_log.h"
#include "task.h"

#include "mimi_config.h"
#include "mimiclaw_port.h"
#include "bus/message_bus.h"

static const char *TAG = "telegram";

#define MIMICLAW_KV_TG_TOKEN   "mimiclaw.tg.token"
#define MIMICLAW_KV_TG_OFFSET  "mimiclaw.tg.offset"

#define TG_TOKEN_MAX_LEN             128
#define TG_HTTP_TIMEOUT_MS           ((MIMI_TG_POLL_TIMEOUT_S + 8) * 1000)
#define TG_HTTP_RESP_INIT_CAP        (4 * 1024)
#define TG_NO_TOKEN_BACKOFF_MS       5000
#define TG_POLL_ERROR_BACKOFF_MS     3000
#define TG_OFFSET_SAVE_STEP          10
#define TG_OFFSET_SAVE_INTERVAL_MS   (30 * 1000)
#define TG_HTTP_REQ_RETRY            2
#define TG_HTTP_REQ_RETRY_DELAY_MS   500
#define TG_POLL_FALLBACK_TIMEOUT_S   0

typedef struct {
    char *data;
    size_t len;
    size_t cap;
    int status_code;
    bool oom;
} tg_http_resp_t;

static TaskHandle_t s_poll_task;
static bool s_poll_started;
static char s_bot_token[TG_TOKEN_MAX_LEN] = MIMI_SECRET_TG_TOKEN;
static int64_t s_update_offset;
static int64_t s_last_saved_offset = -1;
static uint32_t s_last_offset_save_ms;

static esp_err_t tg_api_call(const char *method, const char *post_data, char **out_body, int *out_status);

static void safe_copy(char *dst, size_t dst_size, const char *src)
{
    size_t n;

    if (!dst || dst_size == 0) {
        return;
    }

    if (!src) {
        dst[0] = '\0';
        return;
    }

    n = strnlen(src, dst_size - 1);
    memcpy(dst, src, n);
    dst[n] = '\0';
}

static bool tg_is_integer_string(const char *s)
{
    size_t i = 0;

    if (!s || s[0] == '\0') {
        return false;
    }

    if (s[0] == '-') {
        i = 1;
        if (s[1] == '\0') {
            return false;
        }
    }

    for (; s[i] != '\0'; i++) {
        if (!isdigit((unsigned char)s[i])) {
            return false;
        }
    }
    return true;
}

static void tg_trim_spaces(char *s)
{
    char *start = s;
    char *end;

    if (!s || s[0] == '\0') {
        return;
    }

    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }

    end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    *end = '\0';
}

static bool tg_normalize_chat_id(const char *in, char *out, size_t out_size)
{
    char tmp[48];
    char *end = NULL;
    double dv;
    long long ll;

    if (!in || !out || out_size == 0) {
        return false;
    }

    safe_copy(tmp, sizeof(tmp), in);
    tg_trim_spaces(tmp);
    if (tmp[0] == '\0') {
        return false;
    }

    /* Telegram also allows channel usernames like @channel_name. */
    if (tmp[0] == '@') {
        safe_copy(out, out_size, tmp);
        return true;
    }

    if (tg_is_integer_string(tmp)) {
        safe_copy(out, out_size, tmp);
        return true;
    }

    /* Recover numeric IDs serialized in scientific notation, e.g. "9e+09". */
    dv = strtod(tmp, &end);
    if (end && *end == '\0') {
        if (dv >= 0.0) {
            ll = (long long)(dv + 0.5);
        } else {
            ll = (long long)(dv - 0.5);
        }
        snprintf(out, out_size, "%lld", ll);
        return true;
    }

    safe_copy(out, out_size, tmp);
    return true;
}

static bool tg_parse_i64(const char *s, long long *out)
{
    char *end = NULL;
    long long v;

    if (!s || !out || s[0] == '\0') {
        return false;
    }

    errno = 0;
    v = strtoll(s, &end, 10);
    if (errno != 0 || !end || *end != '\0') {
        return false;
    }

    *out = v;
    return true;
}

static void tg_debug_get_chat_once(const char *chat_id, bool numeric_chat_id)
{
    char payload[96];
    char *resp = NULL;
    int status = 0;
    int n;

    if (!chat_id || chat_id[0] == '\0') {
        return;
    }

    if (numeric_chat_id) {
        n = snprintf(payload, sizeof(payload), "{\"chat_id\":%s}", chat_id);
    } else {
        n = snprintf(payload, sizeof(payload), "{\"chat_id\":\"%s\"}", chat_id);
    }
    if (n <= 0 || n >= (int)sizeof(payload)) {
        ESP_LOGW(TAG, "skip getChat debug: payload overflow");
        return;
    }

    if (tg_api_call("getChat", payload, &resp, &status) != ESP_OK) {
        ESP_LOGW(TAG, "getChat debug request failed for chat_id=%s", chat_id);
        return;
    }

    ESP_LOGW(TAG, "getChat debug(%s) chat_id=%s status=%d body=%.180s",
             numeric_chat_id ? "num" : "str",
             chat_id,
             status,
             resp ? resp : "");
    free(resp);
}

static void tg_debug_get_chat(const char *chat_id, bool numeric_chat_id)
{
    tg_debug_get_chat_once(chat_id, false);
    if (numeric_chat_id) {
        tg_debug_get_chat_once(chat_id, true);
    }
}

static esp_err_t tg_resp_append(tg_http_resp_t *rb, const uint8_t *data, size_t len)
{
    if (!rb || !data || len == 0) {
        return ESP_OK;
    }

    while (rb->len + len + 1 > rb->cap) {
        size_t new_cap = rb->cap * 2;
        char *tmp = (char *)realloc(rb->data, new_cap);
        if (!tmp) {
            rb->oom = true;
            return ESP_ERR_NO_MEM;
        }
        rb->data = tmp;
        rb->cap = new_cap;
    }

    memcpy(rb->data + rb->len, data, len);
    rb->len += len;
    rb->data[rb->len] = '\0';
    return ESP_OK;
}

static void tg_http_response_cb(struct http_response *rsp, enum http_final_call final_data, void *user_data)
{
    tg_http_resp_t *rb = (tg_http_resp_t *)user_data;
    (void)final_data;

    if (!rb || !rsp || rb->oom) {
        return;
    }

    if (rb->status_code == 0 && rsp->http_status_code > 0) {
        rb->status_code = rsp->http_status_code;
    }

    if (rsp->body_frag_start && rsp->body_frag_len > 0) {
        (void)tg_resp_append(rb, rsp->body_frag_start, rsp->body_frag_len);
    }
}

static esp_err_t tg_api_call(const char *method, const char *post_data, char **out_body, int *out_status)
{
    char url[384];
    const char *headers_get[] = {
        "Accept: application/json\r\n",
        "Connection: close\r\n",
        NULL
    };
    const char *headers_post[] = {
        "Accept: application/json\r\n",
        "Connection: close\r\n",
        NULL
    };
    struct https_client_request req = { 0 };
    int ret = -1;
    int attempt;

    if (!method || !out_body) {
        return ESP_ERR_INVALID_ARG;
    }

    *out_body = NULL;
    if (out_status) {
        *out_status = 0;
    }

    if (s_bot_token[0] == '\0') {
        return ESP_ERR_INVALID_STATE;
    }

    if (snprintf(url, sizeof(url), "https://api.telegram.org/bot%s/%s", s_bot_token, method) >= (int)sizeof(url)) {
        return ESP_ERR_INVALID_SIZE;
    }

    req.method = post_data ? HTTP_POST : HTTP_GET;
    req.url = url;
    req.protocol = "HTTP/1.1";
    req.response = tg_http_response_cb;
    req.header_fields = post_data ? headers_post : headers_get;
    req.buffer_size = 2048;

    if (post_data) {
        req.content_type_value = "application/json";
        req.payload = post_data;
        req.payload_len = strlen(post_data);
    }

    for (attempt = 1; attempt <= TG_HTTP_REQ_RETRY; attempt++) {
        tg_http_resp_t rb = { 0 };

        rb.data = (char *)calloc(1, TG_HTTP_RESP_INIT_CAP);
        if (!rb.data) {
            return ESP_ERR_NO_MEM;
        }
        rb.cap = TG_HTTP_RESP_INIT_CAP;

        ret = https_client_request(&req, TG_HTTP_TIMEOUT_MS, &rb);
        if (ret > 0 && !rb.oom && rb.status_code > 0) {
            if (out_status) {
                *out_status = rb.status_code;
            }
            *out_body = rb.data;
            return ESP_OK;
        }

        free(rb.data);
        if (attempt < TG_HTTP_REQ_RETRY) {
            ESP_LOGW(TAG, "%s request invalid ret=%d status=%d body_len=%u, retry %d/%d",
                     method,
                     ret,
                     rb.status_code,
                     (unsigned int)rb.len,
                     attempt + 1,
                     TG_HTTP_REQ_RETRY);
            mimiclaw_port_sleep_ms(TG_HTTP_REQ_RETRY_DELAY_MS);
        }
    }

    return ESP_FAIL;
}

static bool tg_response_is_ok(const char *body)
{
    bool ok = false;
    cJSON *root;
    cJSON *ok_item;

    if (!body) {
        return false;
    }

    root = cJSON_Parse(body);
    if (!root) {
        return (strstr(body, "\"ok\":true") != NULL);
    }

    ok_item = cJSON_GetObjectItem(root, "ok");
    ok = cJSON_IsTrue(ok_item);
    cJSON_Delete(root);
    return ok;
}

static bool tg_parse_chat_id(const cJSON *chat_id, char *out, size_t out_size)
{
    char tmp[48];
    double dv;
    long long iv;

    if (!chat_id || !out || out_size == 0) {
        return false;
    }

    if (cJSON_IsString(chat_id) && chat_id->valuestring && chat_id->valuestring[0] != '\0') {
        return tg_normalize_chat_id(chat_id->valuestring, out, out_size);
    }

    if (cJSON_IsNumber(chat_id)) {
        /* Avoid %f formatting path: on some libc configs it can stringify
         * large numbers as scientific notation with coarse precision.
         * Convert to integer first, then print as decimal string.
         */
        dv = chat_id->valuedouble;
        if (dv >= 0.0) {
            iv = (long long)(dv + 0.5);
        } else {
            iv = (long long)(dv - 0.5);
        }
        snprintf(tmp, sizeof(tmp), "%lld", iv);
        return tg_normalize_chat_id(tmp, out, out_size);
    }

    return false;
}

static void tg_format_chat_id_debug(const cJSON *item, char *buf, size_t buf_size)
{
    if (!buf || buf_size == 0) {
        return;
    }
    buf[0] = '\0';

    if (!item) {
        snprintf(buf, buf_size, "(null)");
        return;
    }

    if (cJSON_IsString(item) && item->valuestring) {
        snprintf(buf, buf_size, "str:%s", item->valuestring);
        return;
    }

    if (cJSON_IsNumber(item)) {
        snprintf(buf, buf_size, "num:%.17g", item->valuedouble);
        return;
    }

    snprintf(buf, buf_size, "type:%d", item->type);
}

static void tg_save_offset_if_needed(bool force)
{
    bool need_save = force;
    uint32_t now;

    if (s_update_offset <= 0) {
        return;
    }

    now = mimiclaw_port_uptime_ms();
    if (!need_save) {
        if (s_last_saved_offset < 0) {
            need_save = true;
        } else if ((s_update_offset - s_last_saved_offset) >= TG_OFFSET_SAVE_STEP) {
            need_save = true;
        } else if ((uint32_t)(now - s_last_offset_save_ms) >= TG_OFFSET_SAVE_INTERVAL_MS) {
            need_save = true;
        }
    }

    if (!need_save) {
        return;
    }

    if (mimiclaw_kv_set_blob(MIMICLAW_KV_TG_OFFSET, &s_update_offset, sizeof(s_update_offset)) == 0) {
        s_last_saved_offset = s_update_offset;
        s_last_offset_save_ms = now;
    }
}

static void tg_process_updates(const char *json_str)
{
    cJSON *root;
    cJSON *ok;
    cJSON *result;
    cJSON *update;
    bool offset_moved = false;
    bool stop_polling_batch = false;

    if (!json_str || json_str[0] == '\0') {
        return;
    }

    root = cJSON_Parse(json_str);
    if (!root) {
        ESP_LOGW(TAG, "parse getUpdates response failed");
        return;
    }

    ok = cJSON_GetObjectItem(root, "ok");
    result = cJSON_GetObjectItem(root, "result");
    if (!cJSON_IsTrue(ok) || !cJSON_IsArray(result)) {
        cJSON_Delete(root);
        return;
    }

    cJSON_ArrayForEach(update, result)
    {
        cJSON *update_id = cJSON_GetObjectItem(update, "update_id");
        cJSON *message;
        cJSON *text;
        cJSON *chat;
        cJSON *chat_id;
        cJSON *from;
        cJSON *from_id;
        cJSON *chat_type_item;
        int64_t uid = -1;
        int64_t next_offset = -1;
        bool advance_offset = false;
        char chat_id_str[32];
        char from_id_str[32];
        char chat_id_dbg[64];
        char from_id_dbg[64];
        bool has_chat_id = false;
        bool has_from_id = false;
        const char *chat_type = NULL;
        const char *reply_id = NULL;
        mimi_msg_t msg = { 0 };

        if (cJSON_IsNumber(update_id)) {
            uid = (int64_t)update_id->valuedouble;
            if (uid < s_update_offset) {
                continue;
            }
            next_offset = uid + 1;
            advance_offset = true;
        }

        message = cJSON_GetObjectItem(update, "message");
        if (!message) {
            if (advance_offset && next_offset > s_update_offset) {
                s_update_offset = next_offset;
                offset_moved = true;
            }
            continue;
        }

        text = cJSON_GetObjectItem(message, "text");
        if (!cJSON_IsString(text) || !text->valuestring) {
            if (advance_offset && next_offset > s_update_offset) {
                s_update_offset = next_offset;
                offset_moved = true;
            }
            continue;
        }

        chat = cJSON_GetObjectItem(message, "chat");
        if (!chat) {
            if (advance_offset && next_offset > s_update_offset) {
                s_update_offset = next_offset;
                offset_moved = true;
            }
            continue;
        }
        from = cJSON_GetObjectItem(message, "from");

        chat_id = cJSON_GetObjectItem(chat, "id");
        tg_format_chat_id_debug(chat_id, chat_id_dbg, sizeof(chat_id_dbg));
        if (tg_parse_chat_id(chat_id, chat_id_str, sizeof(chat_id_str))) {
            has_chat_id = true;
        }

        from_id = from ? cJSON_GetObjectItem(from, "id") : NULL;
        tg_format_chat_id_debug(from_id, from_id_dbg, sizeof(from_id_dbg));
        if (tg_parse_chat_id(from_id, from_id_str, sizeof(from_id_str))) {
            has_from_id = true;
        }

        chat_type_item = cJSON_GetObjectItem(chat, "type");
        if (cJSON_IsString(chat_type_item) && chat_type_item->valuestring) {
            chat_type = chat_type_item->valuestring;
        }

        if (has_chat_id) {
            reply_id = chat_id_str;
        } else if (has_from_id) {
            reply_id = from_id_str;
        } else {
            if (advance_offset && next_offset > s_update_offset) {
                s_update_offset = next_offset;
                offset_moved = true;
            }
            continue;
        }

        strncpy(msg.channel, MIMI_CHAN_TELEGRAM, sizeof(msg.channel) - 1);
        strncpy(msg.chat_id, reply_id, sizeof(msg.chat_id) - 1);
        msg.content = strdup(text->valuestring);
        if (!msg.content) {
            continue;
        }

        if (message_bus_push_inbound(&msg) != ESP_OK) {
            ESP_LOGW(TAG, "Inbound queue full, drop telegram message");
            free(msg.content);
            stop_polling_batch = true;
            break;
        }

        ESP_LOGI(TAG,
                 "queued telegram message reply_id=%s chat.id=%s (%s) from.id=%s (%s) type=%s len=%u",
                 msg.chat_id,
                 has_chat_id ? chat_id_str : "(none)",
                 chat_id_dbg,
                 has_from_id ? from_id_str : "(none)",
                 from_id_dbg,
                 chat_type ? chat_type : "(none)",
                 (unsigned int)strlen(msg.content));

        if (advance_offset && next_offset > s_update_offset) {
            s_update_offset = next_offset;
            offset_moved = true;
        }
    }

    if (stop_polling_batch) {
        ESP_LOGW(TAG, "stop processing current updates batch, keep offset=%lld for retry",
                 (long long)s_update_offset);
    }

    if (offset_moved) {
        tg_save_offset_if_needed(false);
    }
    cJSON_Delete(root);
}

static void telegram_poll_task(void *arg)
{
    (void)arg;
    ESP_LOGI(TAG, "Telegram polling task started");

    while (1) {
        char payload[128];
        char fallback_payload[128];
        char *body = NULL;
        int status = 0;

        if (s_bot_token[0] == '\0') {
            ESP_LOGW(TAG, "No Telegram bot token configured");
            mimiclaw_port_sleep_ms(TG_NO_TOKEN_BACKOFF_MS);
            continue;
        }

        snprintf(payload, sizeof(payload),
                 "{\"offset\":%lld,\"timeout\":%d}",
                 (long long)s_update_offset, MIMI_TG_POLL_TIMEOUT_S);

        if (tg_api_call("getUpdates", payload, &body, &status) != ESP_OK) {
            mimiclaw_port_sleep_ms(TG_POLL_ERROR_BACKOFF_MS);
            continue;
        }

        if (status == 200 && body && body[0] != '\0') {
            tg_process_updates(body);
            free(body);
            continue;
        }

        free(body);
        body = NULL;
        status = 0;

        snprintf(fallback_payload, sizeof(fallback_payload),
                 "{\"offset\":%lld,\"timeout\":%d}",
                 (long long)s_update_offset, TG_POLL_FALLBACK_TIMEOUT_S);

        if (tg_api_call("getUpdates", fallback_payload, &body, &status) == ESP_OK &&
            status == 200 && body && body[0] != '\0') {
            ESP_LOGW(TAG, "getUpdates long-poll fallback -> short-poll");
            tg_process_updates(body);
            free(body);
            continue;
        }

        ESP_LOGW(TAG, "getUpdates failed long/short poll, short_status=%d body=%.120s",
                 status, body ? body : "");
        free(body);
        mimiclaw_port_sleep_ms(TG_POLL_ERROR_BACKOFF_MS);
    }
}

esp_err_t telegram_bot_init(void)
{
    char token_buf[TG_TOKEN_MAX_LEN] = { 0 };
    size_t out_len = 0;
    int64_t offset = 0;

    if (mimiclaw_kv_get_blob(MIMICLAW_KV_TG_TOKEN, token_buf, sizeof(token_buf), &out_len) == 0 &&
        out_len > 1 && token_buf[0] != '\0') {
        safe_copy(s_bot_token, sizeof(s_bot_token), token_buf);
    }

    if (mimiclaw_kv_get_blob(MIMICLAW_KV_TG_OFFSET, &offset, sizeof(offset), &out_len) == 0 &&
        out_len == sizeof(offset) && offset > 0) {
        s_update_offset = offset;
        s_last_saved_offset = offset;
        s_last_offset_save_ms = mimiclaw_port_uptime_ms();
        ESP_LOGI(TAG, "loaded telegram offset=%lld", (long long)offset);
    }

    if (s_bot_token[0] != '\0') {
        ESP_LOGI(TAG, "Telegram bot token loaded (len=%u)", (unsigned int)strlen(s_bot_token));
    } else {
        ESP_LOGW(TAG, "No Telegram bot token, use mimiclaw_set_tg_token <token>");
    }

    return ESP_OK;
}

esp_err_t telegram_bot_start(void)
{
    if (s_poll_started) {
        return ESP_OK;
    }

    if (xTaskCreate(telegram_poll_task, "tg_poll", MIMI_TG_POLL_STACK, NULL,
                    MIMI_TG_POLL_PRIO, &s_poll_task) != pdPASS) {
        s_poll_task = NULL;
        return ESP_FAIL;
    }

    s_poll_started = true;
    return ESP_OK;
}

esp_err_t telegram_send_message(const char *chat_id, const char *text)
{
    char normalized_chat_id[32];
    long long numeric_chat_id = 0;
    bool chat_id_is_numeric = false;
    size_t text_len;
    size_t offset = 0;
    bool all_ok = true;

    if (!chat_id || chat_id[0] == '\0' || !text) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_bot_token[0] == '\0') {
        return ESP_ERR_INVALID_STATE;
    }
    if (!tg_normalize_chat_id(chat_id, normalized_chat_id, sizeof(normalized_chat_id)) ||
        normalized_chat_id[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }
    chat_id_is_numeric = tg_parse_i64(normalized_chat_id, &numeric_chat_id);

    text_len = strlen(text);
    if (text_len == 0) {
        return ESP_OK;
    }

    while (offset < text_len) {
        size_t chunk = text_len - offset;
        char *segment;
        cJSON *body_json;
        cJSON *fallback_body_json;
        char *payload;
        char *fallback_payload;
        char *resp = NULL;
        char *fallback_resp = NULL;
        int status = 0;
        int fallback_status = 0;
        bool sent_ok = false;
        bool send_numeric_first = false;

        if (chunk > MIMI_TG_MAX_MSG_LEN) {
            chunk = MIMI_TG_MAX_MSG_LEN;
        }

        segment = (char *)malloc(chunk + 1);
        if (!segment) {
            return ESP_ERR_NO_MEM;
        }
        memcpy(segment, text + offset, chunk);
        segment[chunk] = '\0';

        body_json = cJSON_CreateObject();
        if (!body_json) {
            free(segment);
            return ESP_ERR_NO_MEM;
        }
        send_numeric_first = chat_id_is_numeric && normalized_chat_id[0] != '@';
        if (send_numeric_first) {
            cJSON_AddRawToObject(body_json, "chat_id", normalized_chat_id);
        } else {
            cJSON_AddStringToObject(body_json, "chat_id", normalized_chat_id);
        }
        cJSON_AddStringToObject(body_json, "text", segment);
        payload = cJSON_PrintUnformatted(body_json);
        cJSON_Delete(body_json);

        if (!payload) {
            free(segment);
            all_ok = false;
            offset += chunk;
            continue;
        }

        if (tg_api_call("sendMessage", payload, &resp, &status) != ESP_OK ||
            status != 200 || !tg_response_is_ok(resp)) {
            ESP_LOGE(TAG, "sendMessage failed chat_id=%s status=%d body=%.120s",
                     normalized_chat_id, status, resp ? resp : "");
            if (chat_id_is_numeric) {
                fallback_body_json = cJSON_CreateObject();
                fallback_payload = NULL;
                fallback_resp = NULL;
                fallback_status = 0;

                if (fallback_body_json) {
                    if (send_numeric_first) {
                        cJSON_AddStringToObject(fallback_body_json, "chat_id", normalized_chat_id);
                    } else {
                        cJSON_AddRawToObject(fallback_body_json, "chat_id", normalized_chat_id);
                    }
                    cJSON_AddStringToObject(fallback_body_json, "text", segment);
                    fallback_payload = cJSON_PrintUnformatted(fallback_body_json);
                    cJSON_Delete(fallback_body_json);
                }

                if (fallback_payload &&
                    tg_api_call("sendMessage", fallback_payload, &fallback_resp, &fallback_status) == ESP_OK &&
                    fallback_status == 200 && tg_response_is_ok(fallback_resp)) {
                    ESP_LOGW(TAG, "sendMessage fallback(%s) success chat_id=%s",
                             send_numeric_first ? "str" : "num", normalized_chat_id);
                    sent_ok = true;
                } else {
                    ESP_LOGW(TAG, "sendMessage fallback(%s) failed chat_id=%s status=%d body=%.120s",
                             send_numeric_first ? "str" : "num",
                             normalized_chat_id,
                             fallback_status,
                             fallback_resp ? fallback_resp : "");
                }

                free(fallback_payload);
                free(fallback_resp);
            }

            if (!sent_ok && status == 400 && resp && strstr(resp, "chat not found")) {
                tg_debug_get_chat(normalized_chat_id, chat_id_is_numeric);
            }
            if (!sent_ok) {
                all_ok = false;
            }
        } else {
            sent_ok = true;
        }

        free(resp);
        free(payload);
        free(segment);
        offset += chunk;
    }

    return all_ok ? ESP_OK : ESP_FAIL;
}

esp_err_t telegram_set_token(const char *token)
{
    size_t len;

    if (!token || token[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    len = strlen(token);
    if (len >= sizeof(s_bot_token)) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (mimiclaw_kv_set_blob(MIMICLAW_KV_TG_TOKEN, token, len + 1) != 0) {
        return ESP_FAIL;
    }

    safe_copy(s_bot_token, sizeof(s_bot_token), token);
    ESP_LOGI(TAG, "Telegram bot token saved");
    return ESP_OK;
}

esp_err_t telegram_bot_test_get_me(void)
{
    char *body = NULL;
    int status = 0;

    if (s_bot_token[0] == '\0') {
        return ESP_ERR_INVALID_STATE;
    }

    if (tg_api_call("getMe", NULL, &body, &status) != ESP_OK) {
        ESP_LOGE(TAG, "getMe request failed");
        return ESP_FAIL;
    }

    if (status != 200 || !tg_response_is_ok(body)) {
        ESP_LOGE(TAG, "getMe failed status=%d body=%.200s", status, body ? body : "");
        free(body);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "getMe ok: %.200s", body ? body : "");
    free(body);
    return ESP_OK;
}
