#include "mimiclaw_app.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "shell.h"
#include "cJSON.h"

#include "mimi_config.h"
#include "mimiclaw_port.h"
#include "mimiclaw_audio_player.h"
#include "bus/message_bus.h"
#include "agent/agent_loop.h"
#include "cron/cron_service.h"
#include "llm/llm_proxy.h"
#include "tools/tool_web_search.h"
#include "tools/tool_registry.h"
#include "telegram/telegram_bot.h"
#include "mimiclaw_tool_web_search_port.h"

#define DBG_TAG "MIMICLAW_APP"
#include "log.h"

#define MIMICLAW_CONNECT_RETRY_DELAY_MS 5000U
#ifndef MIMICLAW_TELEGRAM_AUTO_START
#define MIMICLAW_TELEGRAM_AUTO_START 1
#endif
#ifndef MIMICLAW_WS_AUTO_CONNECT
#define MIMICLAW_WS_AUTO_CONNECT 0
#endif
#define MIMICLAW_KV_HOST "mimiclaw.host"
#define MIMICLAW_KV_PATH "mimiclaw.path"
#define MIMICLAW_KV_PORT "mimiclaw.port"
#define MIMICLAW_KV_SCHEME "mimiclaw.scheme"

typedef struct {
    bool started;
    bool network_ready;
    char host[64];
    uint16_t port;
    char path[64];
    bool use_tls;
} mimiclaw_runtime_t;

static mimiclaw_runtime_t s_runtime = {
    .started = false,
    .network_ready = false,
    .host = "echo.websocket.events",
    .port = 443,
    .path = "/",
    .use_tls = true,
};

static TaskHandle_t s_connect_task;
static bool s_connect_task_running;
static TaskHandle_t s_outbound_task;
static bool s_outbound_task_running;
static bool s_agent_started;
static bool s_telegram_started;
static bool s_telegram_auto_start_logged;
static bool s_ws_auto_connect_logged;
static bool s_bus_ready;

static void mimiclaw_sanitize_text(const char *in, char *out, size_t out_size)
{
    size_t i = 0;
    size_t j = 0;

    if (!out || out_size == 0) {
        return;
    }
    out[0] = '\0';
    if (!in) {
        return;
    }

    while (in[i] != '\0' && j + 1 < out_size) {
        unsigned char c = (unsigned char)in[i++];

        if ((c >= 0x20 && c <= 0x7E) || c == '\r' || c == '\n' || c == '\t') {
            out[j++] = (char)c;
            continue;
        }

        if (j + 4 >= out_size) {
            break;
        }
        out[j++] = '\\';
        out[j++] = 'x';
        out[j++] = "0123456789ABCDEF"[c >> 4];
        out[j++] = "0123456789ABCDEF"[c & 0x0F];
    }

    out[j] = '\0';
}

static const char *mimiclaw_scheme_string(void)
{
    return s_runtime.use_tls ? "wss" : "ws";
}

static void mimiclaw_print_backend_cfg(const char *prefix)
{
    LOG_I("%s %s://%s:%u%s\r\n",
          prefix,
          mimiclaw_scheme_string(),
          s_runtime.host,
          s_runtime.port,
          s_runtime.path);
}

static int mimiclaw_save_runtime_cfg(void)
{
    uint8_t scheme = s_runtime.use_tls ? 1 : 0;
    int rc = 0;

    if (mimiclaw_kv_set_blob(MIMICLAW_KV_HOST, s_runtime.host, strlen(s_runtime.host) + 1) != 0) {
        LOG_W("save host cfg failed\r\n");
        rc = -1;
    }
    if (mimiclaw_kv_set_blob(MIMICLAW_KV_PATH, s_runtime.path, strlen(s_runtime.path) + 1) != 0) {
        LOG_W("save path cfg failed\r\n");
        rc = -1;
    }
    if (mimiclaw_kv_set_blob(MIMICLAW_KV_PORT, &s_runtime.port, sizeof(s_runtime.port)) != 0) {
        LOG_W("save port cfg failed\r\n");
        rc = -1;
    }
    if (mimiclaw_kv_set_blob(MIMICLAW_KV_SCHEME, &scheme, sizeof(scheme)) != 0) {
        LOG_W("save scheme cfg failed\r\n");
        rc = -1;
    }

    return rc;
}

static void mimiclaw_load_runtime_cfg(void)
{
    size_t out_len = 0;
    char host[sizeof(s_runtime.host)] = { 0 };
    char path[sizeof(s_runtime.path)] = { 0 };
    uint16_t port = 0;
    uint8_t scheme = 1;

    if (mimiclaw_kv_get_blob(MIMICLAW_KV_HOST, host, sizeof(host), &out_len) == 0 && out_len > 1) {
        strncpy(s_runtime.host, host, sizeof(s_runtime.host) - 1);
        s_runtime.host[sizeof(s_runtime.host) - 1] = '\0';
    }

    if (mimiclaw_kv_get_blob(MIMICLAW_KV_PATH, path, sizeof(path), &out_len) == 0 && out_len > 1) {
        strncpy(s_runtime.path, path, sizeof(s_runtime.path) - 1);
        s_runtime.path[sizeof(s_runtime.path) - 1] = '\0';
    }

    if (mimiclaw_kv_get_blob(MIMICLAW_KV_PORT, &port, sizeof(port), &out_len) == 0 && out_len == sizeof(port)) {
        if (port > 0) {
            s_runtime.port = port;
        }
    }

    if (mimiclaw_kv_get_blob(MIMICLAW_KV_SCHEME, &scheme, sizeof(scheme), &out_len) == 0 &&
        out_len == sizeof(scheme)) {
        s_runtime.use_tls = (scheme != 0);
    }

    if (s_runtime.path[0] == '\0') {
        strncpy(s_runtime.path, "/", sizeof(s_runtime.path) - 1);
        s_runtime.path[sizeof(s_runtime.path) - 1] = '\0';
    } else if (s_runtime.path[0] != '/') {
        size_t n = strlen(s_runtime.path);
        if (n < sizeof(s_runtime.path) - 1) {
            memmove(&s_runtime.path[1], &s_runtime.path[0], n + 1);
            s_runtime.path[0] = '/';
        } else {
            strncpy(s_runtime.path, "/", sizeof(s_runtime.path) - 1);
            s_runtime.path[sizeof(s_runtime.path) - 1] = '\0';
        }
    }
}

static int mimiclaw_connect_backend(void)
{
    const char *scheme = mimiclaw_scheme_string();
    LOG_I("connecting backend: %s://%s:%u%s\r\n",
          scheme,
          s_runtime.host,
          s_runtime.port,
          s_runtime.path);
    int ret = mimiclaw_ws_connect(scheme, s_runtime.host, s_runtime.port, s_runtime.path);
    if (ret != 0) {
        LOG_E("connect backend failed ret=%d\r\n", ret);
        return ret;
    }

    LOG_I("backend connected: %s://%s:%u%s\r\n",
          scheme,
          s_runtime.host,
          s_runtime.port,
          s_runtime.path);
    return 0;
}

static void mimiclaw_connect_task(void *param)
{
    int ret;
    (void)param;

    while (s_runtime.started && s_runtime.network_ready && !mimiclaw_ws_is_connected()) {
        ret = mimiclaw_connect_backend();
        if (ret == 0) {
            break;
        }

        LOG_W("backend connect retry in %u ms\r\n", MIMICLAW_CONNECT_RETRY_DELAY_MS);
        mimiclaw_port_sleep_ms(MIMICLAW_CONNECT_RETRY_DELAY_MS);
    }

    taskENTER_CRITICAL();
    s_connect_task_running = false;
    s_connect_task = NULL;
    taskEXIT_CRITICAL();
    vTaskDelete(NULL);
}

static void mimiclaw_schedule_connect(void)
{
    BaseType_t ret;

    if (!MIMICLAW_WS_AUTO_CONNECT) {
        if (!s_ws_auto_connect_logged) {
            s_ws_auto_connect_logged = true;
            LOG_I("websocket backend auto connect disabled\r\n");
        }
        return;
    }

    if (!s_runtime.started || !s_runtime.network_ready || mimiclaw_ws_is_connected()) {
        return;
    }

    taskENTER_CRITICAL();
    if (s_connect_task_running) {
        taskEXIT_CRITICAL();
        return;
    }
    s_connect_task_running = true;
    taskEXIT_CRITICAL();

    ret = xTaskCreate(mimiclaw_connect_task, "mimi_conn", 2048, NULL, 12, &s_connect_task);
    if (ret != pdPASS) {
        taskENTER_CRITICAL();
        s_connect_task_running = false;
        s_connect_task = NULL;
        taskEXIT_CRITICAL();
        LOG_E("create connect task failed\r\n");
    }
}

static void mimiclaw_process_tool_exec(cJSON *root)
{
    static char tool_output[1024];
    static char safe_output[4096];
    const cJSON *tool_item = cJSON_GetObjectItem(root, "tool");
    const cJSON *id_item = cJSON_GetObjectItem(root, "id");
    const cJSON *input_item = cJSON_GetObjectItem(root, "input");
    const char *tool_name;
    const char *req_id = "";
    const char *input_json = "{}";
    char *input_heap = NULL;
    char *resp_json = NULL;
    cJSON *resp = NULL;
    esp_err_t ret;

    if (!cJSON_IsString(tool_item) || !tool_item->valuestring || tool_item->valuestring[0] == '\0') {
        LOG_W("tool_exec missing tool name\r\n");
        return;
    }

    tool_name = tool_item->valuestring;
    if (cJSON_IsString(id_item) && id_item->valuestring) {
        req_id = id_item->valuestring;
    }

    if (input_item) {
        if (cJSON_IsString(input_item) && input_item->valuestring) {
            input_json = input_item->valuestring;
        } else {
            input_heap = cJSON_PrintUnformatted((cJSON *)input_item);
            if (input_heap) {
                input_json = input_heap;
            }
        }
    }

    ret = tool_registry_execute(tool_name, input_json, tool_output, sizeof(tool_output));
    mimiclaw_sanitize_text(tool_output, safe_output, sizeof(safe_output));

    resp = cJSON_CreateObject();
    if (!resp) {
        LOG_W("alloc tool_result json failed\r\n");
        goto out;
    }
    cJSON_AddStringToObject(resp, "type", "tool_result");
    cJSON_AddStringToObject(resp, "id", req_id);
    cJSON_AddStringToObject(resp, "tool", tool_name);
    cJSON_AddNumberToObject(resp, "ret", ret);
    cJSON_AddStringToObject(resp, "ret_name", esp_err_to_name(ret));
    cJSON_AddStringToObject(resp, "output", safe_output);

    resp_json = cJSON_PrintUnformatted(resp);
    if (!resp_json) {
        LOG_W("serialize tool_result failed\r\n");
        goto out;
    }

    if (mimiclaw_ws_send_text(resp_json) != 0) {
        LOG_W("send tool_result failed\r\n");
    } else {
        LOG_I("tool_exec done: %s ret=%s(%d)\r\n", tool_name, esp_err_to_name(ret), ret);
    }

out:
    if (resp_json) {
        free(resp_json);
    }
    if (resp) {
        cJSON_Delete(resp);
    }
    if (input_heap) {
        free(input_heap);
    }
}

static int mimiclaw_send_ws_response(const char *chat_id, const char *content)
{
    cJSON *resp;
    char *resp_json;
    int ret = -1;

    resp = cJSON_CreateObject();
    if (!resp) {
        return -1;
    }

    cJSON_AddStringToObject(resp, "type", "response");
    cJSON_AddStringToObject(resp, "chat_id", chat_id ? chat_id : "");
    cJSON_AddStringToObject(resp, "content", content ? content : "");

    resp_json = cJSON_PrintUnformatted(resp);
    cJSON_Delete(resp);
    if (!resp_json) {
        return -1;
    }

    if (mimiclaw_ws_send_text(resp_json) == 0) {
        ret = 0;
    }
    free(resp_json);
    return ret;
}

static void mimiclaw_outbound_task(void *param)
{
    (void)param;
    while (1) {
        mimi_msg_t msg;
        esp_err_t bus_err = message_bus_pop_outbound(&msg, UINT32_MAX);
        if (bus_err != ESP_OK) {
            if (bus_err == ESP_ERR_INVALID_STATE) {
                mimiclaw_port_sleep_ms(500);
            }
            continue;
        }

        if (strcmp(msg.channel, MIMI_CHAN_WEBSOCKET) == 0) {
            if (mimiclaw_send_ws_response(msg.chat_id, msg.content) != 0) {
                LOG_W("send outbound ws response failed chat_id=%s\r\n", msg.chat_id);
            } else {
                LOG_I("outbound ws response sent chat_id=%s len=%u\r\n",
                      msg.chat_id,
                      (unsigned int)(msg.content ? strlen(msg.content) : 0));
            }
        } else if (strcmp(msg.channel, MIMI_CHAN_TELEGRAM) == 0) {
            esp_err_t tg_ret = telegram_send_message(msg.chat_id, msg.content ? msg.content : "");
            if (tg_ret != ESP_OK) {
                LOG_W("send outbound telegram response failed chat_id=%s ret=%s(%d)\r\n",
                      msg.chat_id, esp_err_to_name(tg_ret), tg_ret);
            } else {
                LOG_I("outbound telegram response sent chat_id=%s len=%u\r\n",
                      msg.chat_id,
                      (unsigned int)(msg.content ? strlen(msg.content) : 0));
            }
        } else if (strcmp(msg.channel, MIMI_CHAN_SYSTEM) == 0) {
            LOG_I("system outbound[%s]: %s\r\n", msg.chat_id, msg.content ? msg.content : "");
        } else {
            LOG_W("outbound channel not supported: %s\r\n", msg.channel);
        }

        free(msg.content);
    }
}

static void mimiclaw_process_user_message(cJSON *root)
{
    const cJSON *content_item = cJSON_GetObjectItem(root, "content");
    const cJSON *chat_id_item = cJSON_GetObjectItem(root, "chat_id");
    const char *chat_id = "ws";
    mimi_msg_t msg = { 0 };

    if (!cJSON_IsString(content_item) || !content_item->valuestring) {
        LOG_W("message missing content\r\n");
        return;
    }

    if (cJSON_IsString(chat_id_item) && chat_id_item->valuestring && chat_id_item->valuestring[0] != '\0') {
        chat_id = chat_id_item->valuestring;
    }

    if (!s_bus_ready) {
        LOG_W("message bus not ready, dropping user message\r\n");
        return;
    }

    strncpy(msg.channel, MIMI_CHAN_WEBSOCKET, sizeof(msg.channel) - 1);
    strncpy(msg.chat_id, chat_id, sizeof(msg.chat_id) - 1);
    msg.content = strdup(content_item->valuestring);
    if (!msg.content) {
        LOG_W("alloc inbound message failed\r\n");
        return;
    }

    if (message_bus_push_inbound(&msg) != ESP_OK) {
        LOG_W("inbound queue full, dropping user message\r\n");
        free(msg.content);
        return;
    }

    LOG_I("queued user message chat_id=%s len=%u\r\n",
          msg.chat_id,
          (unsigned int)strlen(content_item->valuestring));
}

static void mimiclaw_on_ws_text(const char *text, size_t len, bool is_finished)
{
    cJSON *root = NULL;
    cJSON *type = NULL;
    char *buf = NULL;

    if (!text || len == 0 || !is_finished) {
        return;
    }

    buf = (char *)malloc(len + 1);
    if (!buf) {
        LOG_W("alloc ws text buffer failed len=%u\r\n", (unsigned int)len);
        return;
    }
    memcpy(buf, text, len);
    buf[len] = '\0';

    root = cJSON_Parse(buf);
    if (!root) {
        LOG_W("ignore non-json ws message\r\n");
        goto out;
    }

    type = cJSON_GetObjectItem(root, "type");
    if (!cJSON_IsString(type) || !type->valuestring) {
        goto out;
    }

    if (strcmp(type->valuestring, "tool_exec") == 0) {
        mimiclaw_process_tool_exec(root);
    } else if (strcmp(type->valuestring, "message") == 0) {
        mimiclaw_process_user_message(root);
    } else if (strcmp(type->valuestring, "ping") == 0) {
        mimiclaw_ws_send_text("{\"type\":\"pong\"}");
    }

out:
    if (root) {
        cJSON_Delete(root);
    }
    free(buf);
}

void mimiclaw_app_init(void)
{
    esp_err_t bus_ret;
    esp_err_t tool_ret;
    esp_err_t cron_ret;
    esp_err_t llm_ret;
    esp_err_t agent_ret;
    esp_err_t tg_ret;

    mimiclaw_load_runtime_cfg();
    mimiclaw_print_backend_cfg("backend cfg:");

    bus_ret = message_bus_init();
    s_bus_ready = (bus_ret == ESP_OK);
    if (bus_ret != ESP_OK) {
        LOG_W("message bus init failed: %s\r\n", esp_err_to_name(bus_ret));
    }

    tool_ret = tool_registry_init();
    if (tool_ret != ESP_OK) {
        LOG_W("tool registry init failed: %s\r\n", esp_err_to_name(tool_ret));
    }

    llm_ret = llm_proxy_init();
    if (llm_ret != ESP_OK) {
        LOG_W("llm proxy init failed: %s\r\n", esp_err_to_name(llm_ret));
    }

    if (s_bus_ready) {
        agent_ret = agent_loop_init();
        if (agent_ret != ESP_OK) {
            LOG_W("agent loop init failed: %s\r\n", esp_err_to_name(agent_ret));
        }
    } else {
        LOG_W("skip agent loop init: message bus not ready\r\n");
    }

    cron_ret = cron_service_init();
    if (cron_ret != ESP_OK) {
        LOG_W("cron service init failed: %s\r\n", esp_err_to_name(cron_ret));
    }

    tg_ret = telegram_bot_init();
    if (tg_ret != ESP_OK) {
        LOG_W("telegram bot init failed: %s\r\n", esp_err_to_name(tg_ret));
    }

    if (s_bus_ready && !s_outbound_task_running) {
        if (xTaskCreate(mimiclaw_outbound_task, "mimi_out", MIMI_OUTBOUND_STACK, NULL,
                        MIMI_OUTBOUND_PRIO, &s_outbound_task) == pdPASS) {
            s_outbound_task_running = true;
        } else {
            s_outbound_task = NULL;
            LOG_W("create outbound task failed\r\n");
        }
    } else if (!s_bus_ready) {
        LOG_W("skip outbound task: message bus not ready\r\n");
    }

    mimiclaw_ws_set_text_handler(mimiclaw_on_ws_text);

    LOG_I("mimiclaw init done\r\n");
}

void mimiclaw_app_start(void)
{
    if (s_runtime.started) {
        LOG_W("already started\r\n");
        return;
    }

    s_runtime.started = true;

    if (s_bus_ready && !s_agent_started) {
        if (agent_loop_start() == ESP_OK) {
            s_agent_started = true;
        } else {
            LOG_W("agent loop start failed\r\n");
        }
    } else if (!s_bus_ready) {
        LOG_W("skip agent loop start: message bus not ready\r\n");
    }

    mimiclaw_schedule_connect();

    LOG_I("mimiclaw started\r\n");
}

void mimiclaw_app_stop(void)
{
    if (!s_runtime.started) {
        return;
    }

    mimiclaw_ws_disconnect();
    s_runtime.started = false;
    LOG_I("mimiclaw stopped\r\n");
}

void mimiclaw_app_on_network_ready(void)
{
    s_runtime.network_ready = true;

    if (MIMICLAW_TELEGRAM_AUTO_START) {
        if (!s_telegram_started) {
            if (telegram_bot_start() == ESP_OK) {
                s_telegram_started = true;
            } else {
                LOG_W("telegram bot start failed\r\n");
            }
        }
    } else if (!s_telegram_auto_start_logged) {
        s_telegram_auto_start_logged = true;
        LOG_I("telegram auto start disabled\r\n");
    }

    if (s_bus_ready) {
        if (cron_service_start() != ESP_OK) {
            LOG_W("cron service start failed\r\n");
        }
    } else {
        LOG_W("skip cron start: message bus not ready\r\n");
    }

    mimiclaw_schedule_connect();
}

void mimiclaw_app_on_network_lost(void)
{
    s_runtime.network_ready = false;
    cron_service_stop();
    mimiclaw_ws_disconnect();
}

static void cmd_mimiclaw_start(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    mimiclaw_app_start();
}

static void cmd_mimiclaw_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    mimiclaw_app_stop();
}

static void cmd_mimiclaw_cfg(int argc, char **argv)
{
    const char *path_arg = "/";
    int port;

    if (argc < 4 || argc > 5) {
        printf("Usage: mimiclaw_cfg <ws|wss> <host> <port> [path]\\r\\n");
        return;
    }

    if (strcmp(argv[1], "wss") == 0) {
        s_runtime.use_tls = true;
    } else if (strcmp(argv[1], "ws") == 0) {
        s_runtime.use_tls = false;
    } else {
        printf("Invalid scheme, use ws or wss\\r\\n");
        return;
    }

    if (argv[2][0] == '\0') {
        printf("Invalid host\\r\\n");
        return;
    }

    port = atoi(argv[3]);
    if (port <= 0 || port > 65535) {
        printf("Invalid port\\r\\n");
        return;
    }

    strncpy(s_runtime.host, argv[2], sizeof(s_runtime.host) - 1);
    s_runtime.host[sizeof(s_runtime.host) - 1] = '\0';
    s_runtime.port = (uint16_t)port;

    if (argc == 5 && argv[4][0] != '\0') {
        path_arg = argv[4];
    }
    if (path_arg[0] == '/') {
        strncpy(s_runtime.path, path_arg, sizeof(s_runtime.path) - 1);
    } else {
        snprintf(s_runtime.path, sizeof(s_runtime.path), "/%s", path_arg);
    }
    s_runtime.path[sizeof(s_runtime.path) - 1] = '\0';

    (void)mimiclaw_save_runtime_cfg();

    printf("mimiclaw backend => %s://%s:%u%s\\r\\n",
           mimiclaw_scheme_string(),
           s_runtime.host,
           s_runtime.port,
           s_runtime.path);

    if (s_runtime.started && s_runtime.network_ready) {
        mimiclaw_ws_disconnect();
        mimiclaw_schedule_connect();
    }
}

static void cmd_mimiclaw_cfg_show(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("mimiclaw backend => %s://%s:%u%s\\r\\n",
           mimiclaw_scheme_string(),
           s_runtime.host,
           s_runtime.port,
           s_runtime.path);
}

static void cmd_mimiclaw_tools(int argc, char **argv)
{
    const char *json;
    (void)argc;
    (void)argv;

    json = tool_registry_get_tools_json();
    if (!json) {
        printf("(tool registry empty)\r\n");
        return;
    }

    printf("%s\r\n", json);
}

static void cmd_mimiclaw_tool_exec(int argc, char **argv)
{
    static char output[1024];
    char input_storage[512];
    const char *tool_name;
    const char *input_json = "{}";
    esp_err_t ret;
    int i;
    int n = 0;

    if (argc < 2) {
        printf("Usage: mimiclaw_tool_exec <tool_name> [input_json]\r\n");
        return;
    }

    tool_name = argv[1];
    if (argc >= 3) {
        input_storage[0] = '\0';
        for (i = 2; i < argc; i++) {
            int wr = snprintf(input_storage + n,
                              sizeof(input_storage) - (size_t)n,
                              "%s%s",
                              (i == 2) ? "" : " ",
                              argv[i]);
            if (wr <= 0 || wr >= (int)(sizeof(input_storage) - (size_t)n)) {
                printf("input_json too long\r\n");
                return;
            }
            n += wr;
        }
        input_json = input_storage;
    }

    ret = tool_registry_execute(tool_name, input_json, output, sizeof(output));
    printf("ret=%s(%d)\r\n%s\r\n", esp_err_to_name(ret), ret, output);
}

static void cmd_mimiclaw_set_search_key(int argc, char **argv)
{
    esp_err_t ret;

    if (argc < 2) {
        printf("Usage: mimiclaw_set_search_key <api_key>\r\n");
        return;
    }

    ret = tool_web_search_set_key(argv[1]);
    printf("ret=%s(%d)\r\n", esp_err_to_name(ret), ret);
}

static void cmd_mimiclaw_set_search_provider(int argc, char **argv)
{
    esp_err_t ret;

    if (argc < 2) {
        printf("Usage: mimiclaw_set_search_provider <bocha|brave>\r\n");
        printf("current=%s\r\n", mimiclaw_web_search_get_provider());
        return;
    }

    ret = mimiclaw_web_search_set_provider(argv[1]);
    printf("ret=%s(%d)\r\n", esp_err_to_name(ret), ret);
}

static void cmd_mimiclaw_set_api_key(int argc, char **argv)
{
    esp_err_t ret;
    if (argc < 2) {
        printf("Usage: mimiclaw_set_api_key <api_key>\r\n");
        return;
    }
    ret = llm_set_api_key(argv[1]);
    printf("ret=%s(%d)\r\n", esp_err_to_name(ret), ret);
}

static void cmd_mimiclaw_set_model(int argc, char **argv)
{
    esp_err_t ret;
    if (argc < 2) {
        printf("Usage: mimiclaw_set_model <model>\r\n");
        return;
    }
    ret = llm_set_model(argv[1]);
    printf("ret=%s(%d)\r\n", esp_err_to_name(ret), ret);
}

static void cmd_mimiclaw_set_provider(int argc, char **argv)
{
    esp_err_t ret;
    if (argc < 2) {
        printf("Usage: mimiclaw_set_provider <anthropic|openai|deepseek|minimax>\r\n");
        return;
    }
    ret = llm_set_provider(argv[1]);
    printf("ret=%s(%d)\r\n", esp_err_to_name(ret), ret);
}

static void cmd_mimiclaw_set_tg_token(int argc, char **argv)
{
    esp_err_t ret;

    if (argc < 2) {
        printf("Usage: mimiclaw_set_tg_token <telegram_bot_token>\r\n");
        return;
    }

    ret = telegram_set_token(argv[1]);
    printf("ret=%s(%d)\r\n", esp_err_to_name(ret), ret);
}

static void cmd_mimiclaw_tg_start(int argc, char **argv)
{
    esp_err_t ret;
    (void)argc;
    (void)argv;

    ret = telegram_bot_start();
    if (ret == ESP_OK) {
        s_telegram_started = true;
    }
    printf("ret=%s(%d)\r\n", esp_err_to_name(ret), ret);
}

static void cmd_mimiclaw_tg_getme(int argc, char **argv)
{
    esp_err_t ret;
    (void)argc;
    (void)argv;

    ret = telegram_bot_test_get_me();
    printf("ret=%s(%d)\r\n", esp_err_to_name(ret), ret);
}

static void cmd_mimiclaw_tg_send(int argc, char **argv)
{
    esp_err_t ret;
    char text[384];
    int i;
    int n = 0;

    if (argc < 3) {
        printf("Usage: mimiclaw_tg_send <chat_id> <text>\r\n");
        return;
    }

    text[0] = '\0';
    for (i = 2; i < argc; i++) {
        int wr = snprintf(text + n, sizeof(text) - (size_t)n, "%s%s", (i == 2) ? "" : " ", argv[i]);
        if (wr <= 0 || wr >= (int)(sizeof(text) - (size_t)n)) {
            printf("text too long\r\n");
            return;
        }
        n += wr;
    }

    ret = telegram_send_message(argv[1], text);
    printf("ret=%s(%d)\r\n", esp_err_to_name(ret), ret);
}

static void cmd_mimiclaw_ws_send(int argc, char **argv)
{
    char payload[512];
    int n = 0;
    int i;

    if (argc < 2) {
        printf("Usage: mimiclaw_ws_send <text>\r\n");
        return;
    }

    payload[0] = '\0';
    for (i = 1; i < argc; i++) {
        int wr = snprintf(payload + n, sizeof(payload) - (size_t)n, "%s%s", (i == 1) ? "" : " ", argv[i]);
        if (wr <= 0 || wr >= (int)(sizeof(payload) - (size_t)n)) {
            printf("payload too long\r\n");
            return;
        }
        n += wr;
    }

    if (mimiclaw_ws_send_text(payload) != 0) {
        printf("send failed (ws not connected?)\r\n");
        return;
    }
    printf("sent\r\n");
}

static void cmd_mimiclaw_audio_play(int argc, char **argv)
{
    esp_err_t ret;
    char output[160];

    if (argc < 2) {
        printf("Usage: mimiclaw_audio_play </sd/file.wav|/sd/file.mp3>\r\n");
        return;
    }

    ret = mimiclaw_audio_play_file(argv[1], output, sizeof(output));
    printf("ret=%s(%d)\r\n%s\r\n", esp_err_to_name(ret), ret, output);
}

static void cmd_mimiclaw_audio_stop(int argc, char **argv)
{
    esp_err_t ret;
    char output[160];
    (void)argc;
    (void)argv;

    ret = mimiclaw_audio_stop(output, sizeof(output));
    printf("ret=%s(%d)\r\n%s\r\n", esp_err_to_name(ret), ret, output);
}

static void cmd_mimiclaw_audio_status(int argc, char **argv)
{
    esp_err_t ret;
    char output[160];
    (void)argc;
    (void)argv;

    ret = mimiclaw_audio_status(output, sizeof(output));
    printf("ret=%s(%d)\r\n%s\r\n", esp_err_to_name(ret), ret, output);
}

SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_start, mimiclaw_start, start mimiclaw runtime);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_stop, mimiclaw_stop, stop mimiclaw runtime);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_cfg, mimiclaw_cfg, config mimiclaw backend);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_cfg_show, mimiclaw_cfg_show, show mimiclaw backend config);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_tools, mimiclaw_tools, list registered mimiclaw tools);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_tool_exec, mimiclaw_tool_exec, execute mimiclaw tool by name);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_set_search_key, mimiclaw_set_search_key, set web search API key);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_set_search_provider, mimiclaw_set_search_provider, set web search provider);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_set_api_key, mimiclaw_set_api_key, set LLM API key);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_set_model, mimiclaw_set_model, set LLM model);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_set_provider, mimiclaw_set_provider, set LLM provider);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_set_tg_token, mimiclaw_set_tg_token, set Telegram bot token);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_tg_start, mimiclaw_tg_start, start Telegram polling task manually);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_tg_getme, mimiclaw_tg_getme, Telegram getMe connectivity test);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_tg_send, mimiclaw_tg_send, send text message via Telegram bot API);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_ws_send, mimiclaw_ws_send, send raw text to backend websocket);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_audio_play, mimiclaw_audio_play, play audio file from SD card);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_audio_stop, mimiclaw_audio_stop, stop audio playback);
SHELL_CMD_EXPORT_ALIAS(cmd_mimiclaw_audio_status, mimiclaw_audio_status, show audio playback status);
