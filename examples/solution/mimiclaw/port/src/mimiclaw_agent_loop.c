#include "agent/agent_loop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#include "bus/message_bus.h"
#include "llm/llm_proxy.h"
#include "tools/tool_registry.h"

#include "cJSON.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "agent";

#define TOOL_OUTPUT_SIZE  (8 * 1024)
#define SYSTEM_PROMPT_SIZE 1024

static TaskHandle_t s_agent_task = NULL;

static bool str_contains_nocase(const char *haystack, const char *needle)
{
    size_t needle_len;

    if (!haystack || !needle || needle[0] == '\0') {
        return false;
    }

    needle_len = strlen(needle);
    while (*haystack != '\0') {
        if (strncasecmp(haystack, needle, needle_len) == 0) {
            return true;
        }
        haystack++;
    }

    return false;
}

static bool text_has_current_time_anchor(const char *text)
{
    static const char *terms[] = {
        "today", "tonight", "now", "current", "currently", "latest",
        "recent", "recently", "breaking", "today's", "今天", "今日",
        "现在", "当前", "实时", "最新", "近期", "最近", NULL
    };
    int i;

    if (!text || text[0] == '\0') {
        return false;
    }

    for (i = 0; terms[i] != NULL; i++) {
        if (str_contains_nocase(text, terms[i]) || strstr(text, terms[i]) != NULL) {
            return true;
        }
    }

    return false;
}

static int extract_explicit_year(const char *text)
{
    const char *p;

    if (!text) {
        return 0;
    }

    for (p = text; p[0] && p[1] && p[2] && p[3]; p++) {
        int year;

        if (p[0] < '0' || p[0] > '9' ||
            p[1] < '0' || p[1] > '9' ||
            p[2] < '0' || p[2] > '9' ||
            p[3] < '0' || p[3] > '9') {
            continue;
        }

        year = (p[0] - '0') * 1000 +
               (p[1] - '0') * 100 +
               (p[2] - '0') * 10 +
               (p[3] - '0');
        if (year >= 2000 && year <= 2099) {
            return year;
        }
    }

    return 0;
}

static bool get_current_year(int *year_out)
{
    time_t now = time(NULL);
    struct tm tm_now;

    if (!year_out || now < 1735689600) {
        return false;
    }

    if (!localtime_r(&now, &tm_now)) {
        return false;
    }

    *year_out = tm_now.tm_year + 1900;
    return true;
}

static bool should_anchor_web_search_to_user(const char *user_query, const char *tool_query)
{
    int user_year;
    int tool_year;
    int current_year = 0;
    bool have_current_year;

    if (!text_has_current_time_anchor(user_query)) {
        return false;
    }

    if (!tool_query || tool_query[0] == '\0') {
        return true;
    }

    if (!text_has_current_time_anchor(tool_query)) {
        return true;
    }

    user_year = extract_explicit_year(user_query);
    tool_year = extract_explicit_year(tool_query);
    have_current_year = get_current_year(&current_year);

    if (user_year == 0 && tool_year != 0) {
        if (!have_current_year || tool_year != current_year) {
            return true;
        }
    }

    return false;
}

static char *rewrite_web_search_input(const char *tool_input, const char *replacement_query)
{
    cJSON *input;
    cJSON *query_item;
    char *rewritten;

    if (!replacement_query || replacement_query[0] == '\0') {
        return NULL;
    }

    input = cJSON_Parse(tool_input ? tool_input : "{}");
    if (!input) {
        return NULL;
    }

    query_item = cJSON_GetObjectItem(input, "query");
    if (query_item) {
        cJSON_SetValuestring(query_item, replacement_query);
    } else {
        cJSON_AddStringToObject(input, "query", replacement_query);
    }

    rewritten = cJSON_PrintUnformatted(input);
    cJSON_Delete(input);
    return rewritten;
}

static void prepare_tool_inputs(const llm_response_t *resp, const char *user_query, char **tool_inputs)
{
    int i;

    for (i = 0; i < resp->call_count; i++) {
        const llm_tool_call_t *call = &resp->calls[i];
        cJSON *input = NULL;
        cJSON *query = NULL;
        const char *tool_query = NULL;

        tool_inputs[i] = NULL;
        if (strcmp(call->name, "web_search") != 0 || !user_query || user_query[0] == '\0') {
            continue;
        }

        input = cJSON_Parse(call->input ? call->input : "{}");
        if (input) {
            query = cJSON_GetObjectItem(input, "query");
            if (query && cJSON_IsString(query)) {
                tool_query = query->valuestring;
            }
        }

        if (should_anchor_web_search_to_user(user_query, tool_query)) {
            tool_inputs[i] = rewrite_web_search_input(call->input, user_query);
            if (tool_inputs[i]) {
                ESP_LOGI(TAG,
                         "anchored web_search query to user text: original=%s replacement=%s",
                         tool_query ? tool_query : "(empty)",
                         user_query);
            }
        }

        cJSON_Delete(input);
    }
}

static void free_tool_inputs(const llm_response_t *resp, char **tool_inputs)
{
    int i;

    for (i = 0; i < resp->call_count; i++) {
        free(tool_inputs[i]);
        tool_inputs[i] = NULL;
    }
}

static void build_system_prompt(char *buf, size_t size)
{
    time_t now = time(NULL);
    struct tm tm_now;
    char date_line[64] = { 0 };

    if (now >= 1735689600 && localtime_r(&now, &tm_now)) {
        snprintf(date_line,
                 sizeof(date_line),
                 "Current local date context: %04d-%02d-%02d.\n",
                 tm_now.tm_year + 1900,
                 tm_now.tm_mon + 1,
                 tm_now.tm_mday);
    }

    snprintf(buf, size,
             "You are MimiClaw running on Bouffalo SDK.\n"
             "Use available tools when needed.\n"
             "If user asks for current time/date, call get_current_time.\n"
             "For time-sensitive questions about today/latest/current/recent news, prices, weather, or markets:\n"
             "- Keep the first web_search query very close to the user's original wording.\n"
             "- Do not invent or change years unless the user explicitly specifies a year.\n"
             "- If the user asks about today/latest and gives no year, prefer current-date information over historical years.\n"
             "When finished, answer clearly and concisely.\n"
             "%s",
             date_line);
}

static cJSON *build_assistant_content(const llm_response_t *resp, char **tool_inputs)
{
    cJSON *content = cJSON_CreateArray();
    int i;

    if (resp->text && resp->text_len > 0) {
        cJSON *text_block = cJSON_CreateObject();
        cJSON_AddStringToObject(text_block, "type", "text");
        cJSON_AddStringToObject(text_block, "text", resp->text);
        cJSON_AddItemToArray(content, text_block);
    }

    for (i = 0; i < resp->call_count; i++) {
        const llm_tool_call_t *call = &resp->calls[i];
        cJSON *tool_block = cJSON_CreateObject();
        const char *tool_input = tool_inputs && tool_inputs[i] ? tool_inputs[i] : (call->input ? call->input : "{}");
        cJSON *input = cJSON_Parse(tool_input);

        cJSON_AddStringToObject(tool_block, "type", "tool_use");
        cJSON_AddStringToObject(tool_block, "id", call->id);
        cJSON_AddStringToObject(tool_block, "name", call->name);
        cJSON_AddItemToObject(tool_block, "input", input ? input : cJSON_CreateObject());
        cJSON_AddItemToArray(content, tool_block);
    }

    return content;
}

static cJSON *build_tool_results(const llm_response_t *resp,
                                 char **tool_inputs,
                                 char *tool_output,
                                 size_t tool_output_size)
{
    cJSON *content = cJSON_CreateArray();
    int i;

    for (i = 0; i < resp->call_count; i++) {
        const llm_tool_call_t *call = &resp->calls[i];
        const char *tool_input = tool_inputs && tool_inputs[i] ? tool_inputs[i] : (call->input ? call->input : "{}");
        cJSON *result_block;
        esp_err_t tool_ret;

        tool_output[0] = '\0';
        tool_ret = tool_registry_execute(call->name, tool_input, tool_output, tool_output_size);
        ESP_LOGI(TAG, "Tool %s ret=%s(%d)", call->name, esp_err_to_name(tool_ret), tool_ret);

        result_block = cJSON_CreateObject();
        cJSON_AddStringToObject(result_block, "type", "tool_result");
        cJSON_AddStringToObject(result_block, "tool_use_id", call->id);
        cJSON_AddStringToObject(result_block, "content", tool_output);
        cJSON_AddItemToArray(content, result_block);
    }

    return content;
}

static void agent_loop_task(void *arg)
{
    const char *tools_json = tool_registry_get_tools_json();
    char system_prompt[SYSTEM_PROMPT_SIZE];
    static char tool_output[TOOL_OUTPUT_SIZE];
    (void)arg;

    ESP_LOGI(TAG, "agent loop started");

    while (1) {
        mimi_msg_t msg;
        cJSON *messages = NULL;
        cJSON *user_msg = NULL;
        char *final_text = NULL;
        int iteration = 0;
        esp_err_t err = message_bus_pop_inbound(&msg, UINT32_MAX);
        if (err != ESP_OK) {
            if (err == ESP_ERR_INVALID_STATE) {
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            continue;
        }

        ESP_LOGI(TAG, "inbound message from %s:%s", msg.channel, msg.chat_id);

        messages = cJSON_CreateArray();
        user_msg = cJSON_CreateObject();
        cJSON_AddStringToObject(user_msg, "role", "user");
        cJSON_AddStringToObject(user_msg, "content", msg.content ? msg.content : "");
        cJSON_AddItemToArray(messages, user_msg);

        while (iteration < MIMI_AGENT_MAX_TOOL_ITER) {
            llm_response_t resp;
            char *tool_inputs[MIMI_MAX_TOOL_CALLS] = { 0 };

            build_system_prompt(system_prompt, sizeof(system_prompt));
            err = llm_chat_tools(system_prompt, messages, tools_json, &resp);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "llm call failed: %s", esp_err_to_name(err));
                break;
            }

            if (!resp.tool_use) {
                if (resp.text && resp.text_len > 0) {
                    final_text = strdup(resp.text);
                }
                llm_response_free(&resp);
                break;
            }

            ESP_LOGI(TAG, "tool_use iteration=%d calls=%d", iteration + 1, resp.call_count);
            prepare_tool_inputs(&resp, msg.content, tool_inputs);

            cJSON *asst_msg = cJSON_CreateObject();
            cJSON_AddStringToObject(asst_msg, "role", "assistant");
            cJSON_AddItemToObject(asst_msg, "content", build_assistant_content(&resp, tool_inputs));
            cJSON_AddItemToArray(messages, asst_msg);

            cJSON *result_msg = cJSON_CreateObject();
            cJSON_AddStringToObject(result_msg, "role", "user");
            cJSON_AddItemToObject(result_msg,
                                  "content",
                                  build_tool_results(&resp, tool_inputs, tool_output, sizeof(tool_output)));
            cJSON_AddItemToArray(messages, result_msg);

            free_tool_inputs(&resp, tool_inputs);
            llm_response_free(&resp);
            iteration++;
        }

        cJSON_Delete(messages);

        {
            mimi_msg_t out = { 0 };
            strncpy(out.channel, msg.channel, sizeof(out.channel) - 1);
            strncpy(out.chat_id, msg.chat_id, sizeof(out.chat_id) - 1);

            if (!final_text || final_text[0] == '\0') {
                free(final_text);
                final_text = strdup("Sorry, I encountered an error.");
            }

            out.content = final_text;
            if (!out.content) {
                ESP_LOGW(TAG, "drop outbound: no memory");
            } else if (message_bus_push_outbound(&out) != ESP_OK) {
                ESP_LOGW(TAG, "drop outbound: queue full");
                free(out.content);
            } else {
                final_text = NULL;
            }
        }

        free(final_text);
        free(msg.content);
    }
}

esp_err_t agent_loop_init(void)
{
    ESP_LOGI(TAG, "agent loop initialized");
    return ESP_OK;
}

esp_err_t agent_loop_start(void)
{
    if (s_agent_task) {
        return ESP_OK;
    }

    if (xTaskCreate(agent_loop_task, "agent_loop", MIMI_AGENT_STACK, NULL, MIMI_AGENT_PRIO, &s_agent_task) != pdPASS) {
        s_agent_task = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}
