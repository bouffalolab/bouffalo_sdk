#include "tools/tool_web_search.h"

#include "mimi_config.h"
#include "mimiclaw_port.h"
#include "mimiclaw_tool_web_search_port.h"

#include "https_client.h"
#include "cJSON.h"
#include "esp_log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static const char *TAG = "web_search";

#define MIMICLAW_SEARCH_KEY_KV "mimiclaw.search_key"
#define MIMICLAW_SEARCH_PROVIDER_KV "mimiclaw.search.provider"
#define SEARCH_KEY_MAX_LEN 160
#define SEARCH_PROVIDER_MAX_LEN 16
#define SEARCH_BUF_SIZE (24 * 1024)
#define SEARCH_RESULT_COUNT_DEFAULT 5
#define SEARCH_RESULT_COUNT_MAX 8
#define SEARCH_TIMEOUT_MS (15 * 1000)
#define SEARCH_SNIPPET_MAX_LEN 220
#define SEARCH_REQUEST_RETRIES 2
#define SEARCH_RETRY_DELAY_MS 300
#define SEARCH_PROVIDER_DEFAULT "bocha"
#ifndef MIMICLAW_BOCHA_API_URL
#define MIMICLAW_BOCHA_API_URL "https://api.bochaai.com/v1/web-search"
#endif

typedef struct {
    char *data;
    size_t len;
    size_t cap;
    int status_code;
} search_buf_t;

typedef bool (*search_format_fn_t)(cJSON *root, char *output, size_t output_size);

typedef struct {
    const char *query;
    const char *freshness;
    int count;
    bool summary;
} search_options_t;

static char s_search_key[SEARCH_KEY_MAX_LEN] = { 0 };
static char s_search_provider[SEARCH_PROVIDER_MAX_LEN] = SEARCH_PROVIDER_DEFAULT;

static bool provider_is_bocha(void)
{
    return strcmp(s_search_provider, "bocha") == 0;
}

static bool provider_is_brave(void)
{
    return strcmp(s_search_provider, "brave") == 0;
}

static bool provider_is_supported(const char *provider)
{
    return provider &&
           (strcmp(provider, "bocha") == 0 || strcmp(provider, "brave") == 0);
}

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

static bool query_is_time_sensitive(const char *query)
{
    static const char *time_sensitive_terms[] = {
        "today", "tonight", "now", "current", "currently", "price", "prices",
        "weather", "forecast", "breaking", "latest", "recent", "recently",
        "news", "update", "updates", "今天", "今日", "现在", "当前", "实时",
        "最新", "近期", "最近", "新闻", "动态", "消息", "油价", "股价",
        "汇率", "天气", "行情", NULL
    };
    int i;

    if (!query || query[0] == '\0') {
        return false;
    }

    for (i = 0; time_sensitive_terms[i] != NULL; i++) {
        if (str_contains_nocase(query, time_sensitive_terms[i]) ||
            strstr(query, time_sensitive_terms[i]) != NULL) {
            return true;
        }
    }

    return false;
}

static bool query_has_recency_hint(const char *query)
{
    return str_contains_nocase(query, "latest") ||
           str_contains_nocase(query, "news") ||
           str_contains_nocase(query, "current") ||
           strstr(query, "最新") != NULL ||
           strstr(query, "新闻") != NULL ||
           strstr(query, "当前") != NULL;
}

static bool build_timely_query(const char *query, char *buf, size_t buf_size)
{
    int wr;

    if (!query || !buf || buf_size == 0) {
        return false;
    }

    if (!query_is_time_sensitive(query) || query_has_recency_hint(query)) {
        return false;
    }

    if (strstr(query, "今天") != NULL || strstr(query, "今日") != NULL ||
        strstr(query, "油价") != NULL || strstr(query, "天气") != NULL ||
        strstr(query, "股价") != NULL || strstr(query, "汇率") != NULL) {
        wr = snprintf(buf, buf_size, "%s 最新 新闻", query);
    } else {
        wr = snprintf(buf, buf_size, "%s latest news", query);
    }

    if (wr <= 0 || (size_t)wr >= buf_size) {
        return false;
    }

    return true;
}

static bool search_output_has_no_results(const char *output)
{
    return output && strcmp(output, "No web results found.") == 0;
}

static void normalize_provider(void)
{
    if (!provider_is_supported(s_search_provider)) {
        strncpy(s_search_provider, SEARCH_PROVIDER_DEFAULT, sizeof(s_search_provider) - 1);
        s_search_provider[sizeof(s_search_provider) - 1] = '\0';
    }
}

static void append_printf(char *output, size_t output_size, size_t *off, const char *fmt, ...)
{
    int wr;
    va_list ap;

    if (!output || !off || *off >= output_size - 1) {
        return;
    }

    va_start(ap, fmt);
    wr = vsnprintf(output + *off, output_size - *off, fmt, ap);
    va_end(ap);

    if (wr < 0) {
        return;
    }

    if ((size_t)wr >= output_size - *off) {
        *off = output_size - 1;
        output[*off] = '\0';
        return;
    }

    *off += (size_t)wr;
}

static size_t url_encode(const char *src, char *dst, size_t dst_size)
{
    static const char hex[] = "0123456789ABCDEF";
    size_t pos = 0;

    for (; *src && pos < dst_size - 3; src++) {
        unsigned char c = (unsigned char)*src;
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            dst[pos++] = (char)c;
        } else if (c == ' ') {
            dst[pos++] = '+';
        } else {
            dst[pos++] = '%';
            dst[pos++] = hex[c >> 4];
            dst[pos++] = hex[c & 0x0F];
        }
    }

    dst[pos] = '\0';
    return pos;
}

static void append_result(char *output,
                          size_t output_size,
                          size_t *off,
                          int index,
                          const char *title,
                          const char *url,
                          const char *text,
                          const char *date)
{
    append_printf(output, output_size, off, "%d. %s\n", index,
                  (title && title[0]) ? title : "(no title)");
    append_printf(output, output_size, off, "   %s\n", (url && url[0]) ? url : "");
    append_printf(output, output_size, off, "   %.*s\n",
                  SEARCH_SNIPPET_MAX_LEN,
                  (text && text[0]) ? text : "");
    if (date && date[0]) {
        append_printf(output, output_size, off, "   Published: %s\n", date);
    }
    append_printf(output, output_size, off, "\n");
}

static bool format_brave_results(cJSON *root, char *output, size_t output_size)
{
    cJSON *web = cJSON_GetObjectItem(root, "web");
    cJSON *results = web ? cJSON_GetObjectItem(web, "results") : NULL;
    cJSON *item;
    int idx = 0;
    size_t off = 0;

    if (!results || !cJSON_IsArray(results) || cJSON_GetArraySize(results) == 0) {
        snprintf(output, output_size, "No web results found.");
        return false;
    }

    cJSON_ArrayForEach(item, results)
    {
        cJSON *title;
        cJSON *url;
        cJSON *desc;

        if (idx >= SEARCH_RESULT_COUNT_MAX || off >= output_size - 1) {
            break;
        }

        title = cJSON_GetObjectItem(item, "title");
        url = cJSON_GetObjectItem(item, "url");
        desc = cJSON_GetObjectItem(item, "description");

        append_result(output, output_size, &off, idx + 1,
                      (title && cJSON_IsString(title)) ? title->valuestring : NULL,
                      (url && cJSON_IsString(url)) ? url->valuestring : NULL,
                      (desc && cJSON_IsString(desc)) ? desc->valuestring : NULL,
                      NULL);
        idx++;
    }

    if (idx == 0) {
        snprintf(output, output_size, "No web results found.");
        return false;
    }

    return true;
}

static cJSON *bocha_find_results_array(cJSON *root)
{
    cJSON *web_pages = cJSON_GetObjectItem(root, "webPages");
    cJSON *results = web_pages ? cJSON_GetObjectItem(web_pages, "value") : NULL;
    cJSON *data;
    cJSON *news;

    if (results && cJSON_IsArray(results)) {
        return results;
    }

    data = cJSON_GetObjectItem(root, "data");
    if (data) {
        web_pages = cJSON_GetObjectItem(data, "webPages");
        results = web_pages ? cJSON_GetObjectItem(web_pages, "value") : NULL;
        if (results && cJSON_IsArray(results)) {
            return results;
        }

        results = cJSON_GetObjectItem(data, "value");
        if (results && cJSON_IsArray(results)) {
            return results;
        }
    }

    news = cJSON_GetObjectItem(root, "news");
    results = news ? cJSON_GetObjectItem(news, "value") : NULL;
    if (results && cJSON_IsArray(results)) {
        return results;
    }

    results = cJSON_GetObjectItem(root, "items");
    if (results && cJSON_IsArray(results)) {
        return results;
    }

    return NULL;
}

static bool format_bocha_results(cJSON *root, char *output, size_t output_size)
{
    cJSON *results = bocha_find_results_array(root);
    cJSON *item;
    int idx = 0;
    size_t off = 0;

    if (!results || !cJSON_IsArray(results) || cJSON_GetArraySize(results) == 0) {
        snprintf(output, output_size, "No web results found.");
        return false;
    }

    cJSON_ArrayForEach(item, results)
    {
        cJSON *name;
        cJSON *url;
        cJSON *summary;
        cJSON *snippet;
        cJSON *date_published;
        const char *text = NULL;

        if (idx >= SEARCH_RESULT_COUNT_MAX || off >= output_size - 1) {
            break;
        }

        name = cJSON_GetObjectItem(item, "name");
        url = cJSON_GetObjectItem(item, "url");
        summary = cJSON_GetObjectItem(item, "summary");
        snippet = cJSON_GetObjectItem(item, "snippet");
        date_published = cJSON_GetObjectItem(item, "datePublished");

        if (summary && cJSON_IsString(summary) && summary->valuestring[0] != '\0') {
            text = summary->valuestring;
        } else if (snippet && cJSON_IsString(snippet)) {
            text = snippet->valuestring;
        }

        append_result(output, output_size, &off, idx + 1,
                      (name && cJSON_IsString(name)) ? name->valuestring : NULL,
                      (url && cJSON_IsString(url)) ? url->valuestring : NULL,
                      text,
                      (date_published && cJSON_IsString(date_published)) ?
                          date_published->valuestring :
                          NULL);
        idx++;
    }

    if (idx == 0) {
        snprintf(output, output_size, "No web results found.");
        return false;
    }

    return true;
}

static void search_response_cb(struct http_response *rsp, enum http_final_call final_data, void *user_data)
{
    search_buf_t *sb = (search_buf_t *)user_data;
    size_t copy_len;

    (void)final_data;

    if (!sb || !rsp) {
        return;
    }

    if (sb->status_code == 0 && rsp->http_status_code > 0) {
        sb->status_code = rsp->http_status_code;
    }

    if (!rsp->body_frag_start || rsp->body_frag_len == 0 || sb->len >= sb->cap - 1) {
        return;
    }

    copy_len = rsp->body_frag_len;
    if (copy_len > (sb->cap - 1 - sb->len)) {
        copy_len = sb->cap - 1 - sb->len;
    }

    memcpy(sb->data + sb->len, rsp->body_frag_start, copy_len);
    sb->len += copy_len;
    sb->data[sb->len] = '\0';
}

static esp_err_t perform_search_request(const struct https_client_request *req,
                                        char *output,
                                        size_t output_size,
                                        search_format_fn_t format_results,
                                        const char *provider_name)
{
    search_buf_t sb = { 0 };
    cJSON *root;
    bool has_results;
    int ret;
    int attempt;

    sb.data = (char *)calloc(1, SEARCH_BUF_SIZE);
    if (!sb.data) {
        snprintf(output, output_size, "Error: Out of memory");
        return ESP_ERR_NO_MEM;
    }
    sb.cap = SEARCH_BUF_SIZE;

    ret = -1;
    for (attempt = 0; attempt < SEARCH_REQUEST_RETRIES; attempt++) {
        sb.len = 0;
        sb.status_code = 0;
        sb.data[0] = '\0';

        ret = https_client_request(req, SEARCH_TIMEOUT_MS, &sb);
        if (ret >= 0) {
            break;
        }

        if (attempt + 1 < SEARCH_REQUEST_RETRIES) {
            ESP_LOGW(TAG,
                     "%s request failed ret=%d attempt=%d/%d, retrying",
                     provider_name ? provider_name : "search",
                     ret,
                     attempt + 1,
                     SEARCH_REQUEST_RETRIES);
            mimiclaw_port_sleep_ms(SEARCH_RETRY_DELAY_MS);
        }
    }

    if (ret < 0) {
        free(sb.data);
        snprintf(output, output_size, "Error: Search request failed (%d)", ret);
        return ESP_FAIL;
    }

    if (sb.status_code != 200) {
        snprintf(output, output_size, "Error: Search API returned status %d body=%.160s",
                 sb.status_code, sb.data ? sb.data : "");
        free(sb.data);
        return ESP_FAIL;
    }

    root = cJSON_Parse(sb.data);
    if (!root) {
        free(sb.data);
        snprintf(output, output_size, "Error: Failed to parse search results");
        return ESP_FAIL;
    }

    has_results = format_results(root, output, output_size);
    if (!has_results) {
        ESP_LOGW(TAG, "%s response parsed but no results: %.240s",
                 provider_name ? provider_name : "search",
                 sb.data ? sb.data : "");
    }
    cJSON_Delete(root);
    free(sb.data);
    return ESP_OK;
}

static esp_err_t do_brave_search(const search_options_t *opts, char *output, size_t output_size)
{
    char encoded_query[256];
    char url[512];
    char token_header[SEARCH_KEY_MAX_LEN + 32];
    const char *headers[] = {
        "Accept: application/json\r\n",
        token_header,
        "Connection: close\r\n",
        NULL
    };
    struct https_client_request req = { 0 };

    url_encode(opts->query, encoded_query, sizeof(encoded_query));
    snprintf(url, sizeof(url),
             "https://api.search.brave.com/res/v1/web/search?q=%s&count=%d",
             encoded_query, opts->count);
    snprintf(token_header, sizeof(token_header), "X-Subscription-Token: %s\r\n", s_search_key);

    req.method = HTTP_GET;
    req.url = url;
    req.protocol = "HTTP/1.1";
    req.response = search_response_cb;
    req.header_fields = headers;
    req.buffer_size = 2048;

    return perform_search_request(&req, output, output_size, format_brave_results, "brave");
}

static esp_err_t do_bocha_search_once(const search_options_t *opts,
                                      const char *freshness,
                                      char *output,
                                      size_t output_size)
{
    char auth_header[SEARCH_KEY_MAX_LEN + 32];
    const char *headers[] = {
        "Accept: application/json\r\n",
        auth_header,
        "Connection: close\r\n",
        NULL
    };
    struct https_client_request req = { 0 };
    cJSON *body = cJSON_CreateObject();
    char *payload;
    esp_err_t ret;

    if (!body) {
        snprintf(output, output_size, "Error: Out of memory");
        return ESP_ERR_NO_MEM;
    }

    cJSON_AddStringToObject(body, "query", opts->query);
    cJSON_AddNumberToObject(body, "count", opts->count);
    cJSON_AddBoolToObject(body, "summary", opts->summary);
    if (freshness && freshness[0] != '\0') {
        cJSON_AddStringToObject(body, "freshness", freshness);
    }

    payload = cJSON_PrintUnformatted(body);
    cJSON_Delete(body);
    if (!payload) {
        snprintf(output, output_size, "Error: Out of memory");
        return ESP_ERR_NO_MEM;
    }

    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s\r\n", s_search_key);

    req.method = HTTP_POST;
    req.url = MIMICLAW_BOCHA_API_URL;
    req.protocol = "HTTP/1.1";
    req.response = search_response_cb;
    req.header_fields = headers;
    req.content_type_value = "application/json";
    req.payload = payload;
    req.payload_len = strlen(payload);
    req.buffer_size = 2048;

    ret = perform_search_request(&req, output, output_size, format_bocha_results, "bocha");
    free(payload);
    return ret;
}

static esp_err_t do_bocha_search(const search_options_t *opts, char *output, size_t output_size)
{
    search_options_t rewritten_opts = *opts;
    char rewritten_query[320];
    esp_err_t ret;

    if ((!opts->freshness || opts->freshness[0] == '\0') &&
        build_timely_query(opts->query, rewritten_query, sizeof(rewritten_query))) {
        rewritten_opts.query = rewritten_query;
        ESP_LOGI(TAG, "Bocha rewritten time-sensitive query=%s", rewritten_opts.query);
        ret = do_bocha_search_once(&rewritten_opts, NULL, output, output_size);
        if (ret == ESP_OK && !search_output_has_no_results(output)) {
            return ret;
        }
        ESP_LOGW(TAG, "Bocha rewritten query did not return usable results, retry original query");
    }

    return do_bocha_search_once(opts, opts->freshness, output, output_size);
}

const char *mimiclaw_web_search_get_provider(void)
{
    return s_search_provider;
}

esp_err_t mimiclaw_web_search_set_provider(const char *provider)
{
    size_t len;

    if (!provider || !provider_is_supported(provider)) {
        return ESP_ERR_INVALID_ARG;
    }

    len = strlen(provider);
    if (len >= sizeof(s_search_provider)) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (mimiclaw_kv_set_blob(MIMICLAW_SEARCH_PROVIDER_KV, provider, len + 1) != 0) {
        return ESP_FAIL;
    }

    strncpy(s_search_provider, provider, sizeof(s_search_provider) - 1);
    s_search_provider[sizeof(s_search_provider) - 1] = '\0';
    ESP_LOGI(TAG, "Search provider saved: %s", s_search_provider);
    return ESP_OK;
}

esp_err_t tool_web_search_init(void)
{
    char key_buf[SEARCH_KEY_MAX_LEN] = { 0 };
    char provider_buf[SEARCH_PROVIDER_MAX_LEN] = { 0 };
    size_t out_len = 0;

    strncpy(s_search_provider, SEARCH_PROVIDER_DEFAULT, sizeof(s_search_provider) - 1);
    s_search_provider[sizeof(s_search_provider) - 1] = '\0';

    if (MIMI_SECRET_SEARCH_KEY[0] != '\0') {
        strncpy(s_search_key, MIMI_SECRET_SEARCH_KEY, sizeof(s_search_key) - 1);
        s_search_key[sizeof(s_search_key) - 1] = '\0';
    }

    if (mimiclaw_kv_get_blob(MIMICLAW_SEARCH_KEY_KV, key_buf, sizeof(key_buf), &out_len) == 0 &&
        out_len > 0 && key_buf[0] != '\0') {
        strncpy(s_search_key, key_buf, sizeof(s_search_key) - 1);
        s_search_key[sizeof(s_search_key) - 1] = '\0';
    }

    if (mimiclaw_kv_get_blob(MIMICLAW_SEARCH_PROVIDER_KV,
                             provider_buf,
                             sizeof(provider_buf),
                             &out_len) == 0 &&
        out_len > 0 && provider_buf[0] != '\0') {
        strncpy(s_search_provider, provider_buf, sizeof(s_search_provider) - 1);
        s_search_provider[sizeof(s_search_provider) - 1] = '\0';
    }

    normalize_provider();

    if (s_search_key[0] != '\0') {
        ESP_LOGI(TAG, "Web search initialized provider=%s (key configured)", s_search_provider);
    } else {
        ESP_LOGW(TAG, "Web search initialized provider=%s (no API key configured)", s_search_provider);
    }

    return ESP_OK;
}

esp_err_t tool_web_search_set_key(const char *api_key)
{
    size_t len;

    if (!api_key || api_key[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    len = strlen(api_key);
    if (len >= sizeof(s_search_key)) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (mimiclaw_kv_set_blob(MIMICLAW_SEARCH_KEY_KV, api_key, len + 1) != 0) {
        return ESP_FAIL;
    }

    strncpy(s_search_key, api_key, sizeof(s_search_key) - 1);
    s_search_key[sizeof(s_search_key) - 1] = '\0';
    ESP_LOGI(TAG, "Search API key saved for provider=%s", s_search_provider);
    return ESP_OK;
}

esp_err_t tool_web_search_execute(const char *input_json, char *output, size_t output_size)
{
    cJSON *input;
    cJSON *query;
    cJSON *count;
    cJSON *summary;
    cJSON *freshness;
    search_options_t opts = {
        .query = NULL,
        .freshness = NULL,
        .count = SEARCH_RESULT_COUNT_DEFAULT,
        .summary = true,
    };
    esp_err_t ret;

    if (!output || output_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_search_key[0] == '\0') {
        snprintf(output, output_size, "Error: No search API key configured");
        return ESP_ERR_INVALID_STATE;
    }

    input = cJSON_Parse(input_json);
    if (!input) {
        snprintf(output, output_size, "Error: Invalid input JSON");
        return ESP_ERR_INVALID_ARG;
    }

    query = cJSON_GetObjectItem(input, "query");
    if (!query || !cJSON_IsString(query) || query->valuestring[0] == '\0') {
        cJSON_Delete(input);
        snprintf(output, output_size, "Error: Missing 'query' field");
        return ESP_ERR_INVALID_ARG;
    }
    opts.query = query->valuestring;

    count = cJSON_GetObjectItem(input, "count");
    if (count && cJSON_IsNumber(count)) {
        int value = count->valueint;
        if (value < 1) {
            value = 1;
        }
        if (value > SEARCH_RESULT_COUNT_MAX) {
            value = SEARCH_RESULT_COUNT_MAX;
        }
        opts.count = value;
    }

    summary = cJSON_GetObjectItem(input, "summary");
    if (summary && cJSON_IsBool(summary)) {
        opts.summary = cJSON_IsTrue(summary);
    }

    freshness = cJSON_GetObjectItem(input, "freshness");
    if (freshness && cJSON_IsString(freshness) && freshness->valuestring[0] != '\0') {
        opts.freshness = freshness->valuestring;
    }

    ESP_LOGI(TAG, "Searching provider=%s query=%s", s_search_provider, opts.query);

    if (provider_is_bocha()) {
        ret = do_bocha_search(&opts, output, output_size);
    } else if (provider_is_brave()) {
        ret = do_brave_search(&opts, output, output_size);
    } else {
        snprintf(output, output_size, "Error: Unsupported search provider '%s'", s_search_provider);
        ret = ESP_ERR_INVALID_STATE;
    }

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Search output preview: %.200s", output);
    }

    cJSON_Delete(input);
    return ret;
}
