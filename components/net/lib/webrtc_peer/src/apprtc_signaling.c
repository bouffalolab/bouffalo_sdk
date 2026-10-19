#include "apprtc_signaling.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include <cJSON.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/sha1.h>
#include <mbedtls/ssl.h>

#include "base64.h"
#include "digicert_global_root_g2.h"
#include "ports.h"
#include "ssl_transport.h"

#define DBG_TAG "APPRTC"
#include "log.h"

#define APPRTC_HOST_CAPACITY          96
#define APPRTC_PATH_CAPACITY          512
#define APPRTC_URL_CAPACITY           768
#define APPRTC_ID_CAPACITY            128
#define APPRTC_ICE_URL_CAPACITY       160
#define APPRTC_ICE_USER_CAPACITY      128
#define APPRTC_ICE_CREDENTIAL_CAPACITY 256
#define APPRTC_HTTP_RESPONSE_CAPACITY 16384
#define APPRTC_WS_BUFFER_CAPACITY     16384
#define APPRTC_INITIAL_MESSAGE_COUNT  8
#define APPRTC_WS_POLL_TIMEOUT_MS     5

static int apprtc_sha1(const unsigned char *data, size_t size,
                       unsigned char digest[20])
{
#if CONFIG_MBEDTLS_2_X
    return mbedtls_sha1_ret(data, size, digest);
#else
    return mbedtls_sha1(data, size, digest);
#endif
}

typedef struct {
    char scheme[6];
    char host[APPRTC_HOST_CAPACITY];
    char path[APPRTC_PATH_CAPACITY];
    uint16_t port;
} url_parts_t;

typedef struct {
    int status;
    char *body;
    size_t body_len;
} http_response_t;

struct apprtc_signaling {
    char base_url[APPRTC_URL_CAPACITY];
    char join_url[APPRTC_URL_CAPACITY];
    char wss_url[APPRTC_URL_CAPACITY];
    char wss_post_url[APPRTC_URL_CAPACITY];
    char ice_server_url[APPRTC_URL_CAPACITY];
    char room_id[APPRTC_ID_CAPACITY];
    char client_id[APPRTC_ID_CAPACITY];

    char stun_url[APPRTC_ICE_URL_CAPACITY];
    char turn_url[APPRTC_ICE_URL_CAPACITY];
    char ice_username[APPRTC_ICE_USER_CAPACITY];
    char ice_credential[APPRTC_ICE_CREDENTIAL_CAPACITY];

    char *initial_messages[APPRTC_INITIAL_MESSAGE_COUNT];
    size_t initial_message_count;

    int is_initiator;
    int ws_connected;
    NetworkContext_t ws_net;
    uint8_t *ws_rx;
    size_t ws_rx_len;
    uint8_t *ws_fragment;
    size_t ws_fragment_len;
    int ws_fragment_active;

    apprtc_message_handler_t handler;
    void *handler_user_data;
};

static int copy_string(char *dst, size_t capacity, const char *src)
{
    size_t length;

    if (!dst || capacity == 0 || !src) {
        return -1;
    }
    length = strlen(src);
    if (length >= capacity) {
        return -1;
    }
    memcpy(dst, src, length + 1);
    return 0;
}

static int json_copy_string(cJSON *object, const char *name,
                            char *dst, size_t capacity)
{
    cJSON *item = cJSON_GetObjectItemCaseSensitive(object, name);

    if (!cJSON_IsString(item) || !item->valuestring) {
        return -1;
    }
    return copy_string(dst, capacity, item->valuestring);
}

static int parse_url(const char *url, url_parts_t *parts)
{
    const char *cursor;
    const char *authority_end;
    const char *port_start = NULL;
    size_t scheme_len;
    size_t host_len;

    if (!url || !parts) {
        return -1;
    }
    memset(parts, 0, sizeof(*parts));
    for (const unsigned char *p = (const unsigned char *)url; *p; ++p) {
        if (*p <= 0x20 || *p == 0x7f || *p == '#') {
            return -1;
        }
    }

    cursor = strstr(url, "://");
    if (!cursor) {
        return -1;
    }
    scheme_len = (size_t)(cursor - url);
    if (scheme_len == 0 || scheme_len >= sizeof(parts->scheme)) {
        return -1;
    }
    memcpy(parts->scheme, url, scheme_len);
    parts->scheme[scheme_len] = '\0';

    if (strcmp(parts->scheme, "https") == 0 ||
        strcmp(parts->scheme, "wss") == 0) {
        parts->port = 443;
    } else {
        return -1;
    }

    cursor += 3;
    authority_end = strpbrk(cursor, "/?");
    if (!authority_end) {
        authority_end = cursor + strlen(cursor);
    }
    if (authority_end == cursor) {
        return -1;
    }

    for (const char *p = cursor; p < authority_end; ++p) {
        if (*p == ':') {
            port_start = p;
        }
    }
    host_len = (size_t)((port_start ? port_start : authority_end) - cursor);
    if (host_len == 0 || host_len >= sizeof(parts->host)) {
        return -1;
    }
    memcpy(parts->host, cursor, host_len);
    parts->host[host_len] = '\0';
    for (size_t i = 0; i < host_len; i++) {
        unsigned char ch = (unsigned char)parts->host[i];
        if (!isalnum(ch) && ch != '.' && ch != '-') {
            return -1;
        }
    }

    if (port_start) {
        char *end = NULL;
        unsigned long port = strtoul(port_start + 1, &end, 10);

        if (end != authority_end || port == 0 || port > UINT16_MAX) {
            return -1;
        }
        parts->port = (uint16_t)port;
    }

    if (*authority_end == '\0') {
        copy_string(parts->path, sizeof(parts->path), "/");
    } else if (*authority_end == '?') {
        int length = snprintf(parts->path, sizeof(parts->path),
                               "/%s", authority_end);
        if (length < 0 || (size_t)length >= sizeof(parts->path)) {
            return -1;
        }
    } else if (copy_string(parts->path, sizeof(parts->path), authority_end) != 0) {
        return -1;
    }
    return 0;
}

static int build_base_url(const url_parts_t *parts, char *dst, size_t capacity)
{
    int length;
    int default_port;

    default_port = strcmp(parts->scheme, "https") == 0 && parts->port == 443;
    if (default_port) {
        length = snprintf(dst, capacity, "%s://%s",
                          parts->scheme, parts->host);
    } else {
        length = snprintf(dst, capacity, "%s://%s:%u",
                          parts->scheme, parts->host, parts->port);
    }
    return length > 0 && (size_t)length < capacity ? 0 : -1;
}

static int tls_send_all(NetworkContext_t *net, const uint8_t *data, size_t length)
{
    size_t sent = 0;

    while (sent < length) {
        int ret = ssl_transport_send(net, data + sent, length - sent);
        if (ret <= 0) {
            return -1;
        }
        sent += (size_t)ret;
    }
    return 0;
}

static const uint8_t *find_bytes(const uint8_t *haystack, size_t haystack_len,
                                 const char *needle)
{
    size_t needle_len = strlen(needle);

    if (needle_len == 0 || haystack_len < needle_len) {
        return NULL;
    }
    for (size_t i = 0; i <= haystack_len - needle_len; ++i) {
        if (memcmp(haystack + i, needle, needle_len) == 0) {
            return haystack + i;
        }
    }
    return NULL;
}

static int http_header_value(const uint8_t *headers, size_t headers_len,
                             const char *name, char *value,
                             size_t value_capacity)
{
    size_t name_len = strlen(name);
    size_t pos = 0;

    while (pos < headers_len) {
        size_t line_end = pos;
        size_t value_start;
        size_t value_end;

        while (line_end < headers_len && headers[line_end] != '\n') {
            line_end++;
        }
        if (line_end > pos && headers[line_end - 1] == '\r') {
            line_end--;
        }
        if (line_end > pos + name_len &&
            strncasecmp((const char *)headers + pos, name, name_len) == 0 &&
            headers[pos + name_len] == ':') {
            value_start = pos + name_len + 1;
            while (value_start < line_end &&
                   isspace((unsigned char)headers[value_start])) {
                value_start++;
            }
            value_end = line_end;
            while (value_end > value_start &&
                   isspace((unsigned char)headers[value_end - 1])) {
                value_end--;
            }
            if (value_end - value_start >= value_capacity) {
                return -1;
            }
            memcpy(value, headers + value_start, value_end - value_start);
            value[value_end - value_start] = '\0';
            return 0;
        }
        pos = line_end;
        while (pos < headers_len &&
               (headers[pos] == '\r' || headers[pos] == '\n')) {
            pos++;
        }
    }
    return -1;
}

static int parse_decimal_size(const char *text, size_t limit, size_t *value)
{
    size_t parsed = 0;

    if (!text || !text[0] || !value) {
        return -1;
    }
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        size_t digit;

        if (!isdigit(*p)) {
            return -1;
        }
        digit = (size_t)(*p - '0');
        if (digit > limit || parsed > (limit - digit) / 10) {
            return -1;
        }
        parsed = parsed * 10 + digit;
    }
    *value = parsed;
    return 0;
}

static int parse_http_status(const uint8_t *headers, size_t headers_len,
                             int *status)
{
    size_t line_len = 0;
    size_t pos = 5;
    int parsed_status;

    if (!headers || !status) {
        return -1;
    }
    while (line_len < headers_len && headers[line_len] != '\n') {
        line_len++;
    }
    if (line_len > 0 && headers[line_len - 1] == '\r') {
        line_len--;
    }
    if (line_len < 12 || memcmp(headers, "HTTP/", 5) != 0 ||
        pos >= line_len || !isdigit(headers[pos])) {
        return -1;
    }
    while (pos < line_len && isdigit(headers[pos])) {
        pos++;
    }
    if (pos >= line_len || headers[pos++] != '.' ||
        pos >= line_len || !isdigit(headers[pos])) {
        return -1;
    }
    while (pos < line_len && isdigit(headers[pos])) {
        pos++;
    }
    if (pos + 4 > line_len || headers[pos++] != ' ' ||
        !isdigit(headers[pos]) || !isdigit(headers[pos + 1]) ||
        !isdigit(headers[pos + 2]) ||
        (pos + 3 < line_len && headers[pos + 3] != ' ')) {
        return -1;
    }
    parsed_status = (headers[pos] - '0') * 100 +
                    (headers[pos + 1] - '0') * 10 +
                    headers[pos + 2] - '0';
    *status = parsed_status;
    return 0;
}

static int http_value_has_token(const char *value, const char *token)
{
    size_t token_len;

    if (!value || !token || token[0] == '\0') {
        return 0;
    }
    token_len = strlen(token);
    while (*value) {
        const char *end;
        const char *start;

        while (*value == ',' || isspace((unsigned char)*value)) {
            value++;
        }
        start = value;
        while (*value && *value != ',') {
            value++;
        }
        end = value;
        while (end > start && isspace((unsigned char)end[-1])) {
            end--;
        }
        if ((size_t)(end - start) == token_len &&
            strncasecmp(start, token, token_len) == 0) {
            return 1;
        }
    }
    return 0;
}

static int parse_chunk_size(const uint8_t *line, size_t line_len,
                            size_t limit, size_t *chunk_size)
{
    size_t parsed = 0;
    size_t pos = 0;

    if (!line || line_len == 0 || !chunk_size) {
        return -1;
    }
    while (pos < line_len && isxdigit(line[pos])) {
        size_t digit;

        if (line[pos] >= '0' && line[pos] <= '9') {
            digit = (size_t)(line[pos] - '0');
        } else {
            digit = (size_t)(tolower(line[pos]) - 'a' + 10);
        }
        if (digit > limit || parsed > (limit - digit) / 16) {
            return -1;
        }
        parsed = parsed * 16 + digit;
        pos++;
    }
    if (pos == 0) {
        return -1;
    }
    if (pos < line_len) {
        if (line[pos++] != ';' || pos == line_len) {
            return -1;
        }
        for (; pos < line_len; ++pos) {
            if (line[pos] < 0x20 || line[pos] > 0x7e) {
                return -1;
            }
        }
    }
    *chunk_size = parsed;
    return 0;
}

static int decode_chunked_body(const uint8_t *input, size_t input_len,
                               char **output, size_t *output_len)
{
    char *decoded;
    size_t in_pos = 0;
    size_t out_pos = 0;

    if (!output || !output_len) {
        return -1;
    }
    *output = NULL;
    *output_len = 0;
    if (!input || input_len > APPRTC_HTTP_RESPONSE_CAPACITY) {
        return -1;
    }
    decoded = malloc(input_len + 1);
    if (!decoded) {
        return -1;
    }

    while (in_pos < input_len) {
        size_t chunk_len;
        size_t line_len = 0;

        while (in_pos + line_len + 1 < input_len &&
               !(input[in_pos + line_len] == '\r' &&
                 input[in_pos + line_len + 1] == '\n')) {
            line_len++;
        }
        if (line_len == 0 || line_len > 64 ||
            in_pos + line_len + 1 >= input_len) {
            free(decoded);
            return -1;
        }
        if (parse_chunk_size(input + in_pos, line_len,
                             APPRTC_HTTP_RESPONSE_CAPACITY - out_pos,
                             &chunk_len) != 0) {
            free(decoded);
            return -1;
        }
        in_pos += line_len + 2;
        if (chunk_len == 0) {
            while (in_pos + 1 < input_len) {
                size_t trailer_len = 0;

                while (in_pos + trailer_len + 1 < input_len &&
                       !(input[in_pos + trailer_len] == '\r' &&
                         input[in_pos + trailer_len + 1] == '\n')) {
                    trailer_len++;
                }
                if (in_pos + trailer_len + 1 >= input_len) {
                    break;
                }
                in_pos += trailer_len + 2;
                if (trailer_len == 0) {
                    if (in_pos != input_len) {
                        break;
                    }
                    decoded[out_pos] = '\0';
                    *output = decoded;
                    *output_len = out_pos;
                    return 0;
                }
            }
            free(decoded);
            return -1;
        }
        if (chunk_len > input_len - in_pos ||
            chunk_len > APPRTC_HTTP_RESPONSE_CAPACITY - out_pos) {
            free(decoded);
            return -1;
        }
        memcpy(decoded + out_pos, input + in_pos, chunk_len);
        out_pos += chunk_len;
        in_pos += chunk_len;
        if (in_pos + 2 > input_len ||
            input[in_pos] != '\r' || input[in_pos + 1] != '\n') {
            free(decoded);
            return -1;
        }
        in_pos += 2;
    }

    free(decoded);
    return -1;
}

static void http_response_free(http_response_t *response)
{
    if (response) {
        free(response->body);
        memset(response, 0, sizeof(*response));
    }
}

static int http_request(const char *url, const char *method,
                        const char *content_type,
                        const char *extra_headers,
                        const char *body, http_response_t *response)
{
    url_parts_t parts;
    NetworkContext_t net;
    uint8_t *received = NULL;
    char *request = NULL;
    const uint8_t *header_end;
    size_t body_len = body ? strlen(body) : 0;
    size_t request_capacity;
    size_t received_len = 0;
    size_t headers_len;
    size_t raw_body_len;
    int request_len;
    int ret = -1;
    char content_length_value[24];
    char transfer_encoding[32];
    char host_header[APPRTC_HOST_CAPACITY + 8];

    if (!response) {
        return -1;
    }
    memset(response, 0, sizeof(*response));
    if (!method || parse_url(url, &parts) != 0 ||
        strcmp(parts.scheme, "https") != 0) {
        return -1;
    }
    if (parts.port == 443) {
        snprintf(host_header, sizeof(host_header), "%s", parts.host);
    } else {
        snprintf(host_header, sizeof(host_header), "%s:%u",
                 parts.host, parts.port);
    }

    request_capacity = strlen(method) + strlen(parts.path) +
                       strlen(parts.host) + body_len +
                       (content_type ? strlen(content_type) : 0) +
                       (extra_headers ? strlen(extra_headers) : 0) + 320;
    request = malloc(request_capacity);
    received = malloc(APPRTC_HTTP_RESPONSE_CAPACITY + 1);
    if (!request || !received) {
        goto done;
    }

    request_len = snprintf(request, request_capacity,
                           "%s %s HTTP/1.1\r\n"
                           "Host: %s\r\n"
                           "User-Agent: webrtc-peer/1.0\r\n"
                           "Accept: application/json\r\n"
                           "%s%s%s"
                           "Content-Length: %u\r\n"
                           "Connection: close\r\n\r\n",
                           method, parts.path, host_header,
                           content_type ? "Content-Type: " : "",
                           content_type ? content_type : "",
                           content_type ? "\r\n" : "",
                           (unsigned int)body_len);
    if (request_len <= 0 || (size_t)request_len >= request_capacity) {
        goto done;
    }
    if (extra_headers && extra_headers[0] != '\0') {
        char *headers_end = strstr(request, "Content-Length:");
        size_t extra_len = strlen(extra_headers);

        if (!headers_end || (size_t)request_len + extra_len >= request_capacity) {
            goto done;
        }
        memmove(headers_end + extra_len, headers_end,
                strlen(headers_end) + 1);
        memcpy(headers_end, extra_headers, extra_len);
        request_len += (int)extra_len;
    }

    if (ssl_transport_connect(&net, parts.host, parts.port,
                              digicert_global_root_g2_pem) != 0) {
        LOG_E("TLS connect failed: %s:%u\r\n", parts.host, parts.port);
        goto done;
    }
    if (tls_send_all(&net, (const uint8_t *)request,
                     (size_t)request_len) != 0 ||
        (body_len > 0 &&
         tls_send_all(&net, (const uint8_t *)body, body_len) != 0)) {
        ssl_transport_disconnect(&net);
        goto done;
    }

    while (received_len < APPRTC_HTTP_RESPONSE_CAPACITY) {
        int bytes = ssl_transport_recv(&net, received + received_len,
                                       APPRTC_HTTP_RESPONSE_CAPACITY -
                                       received_len);
        if (bytes > 0) {
            received_len += (size_t)bytes;
            header_end = find_bytes(received, received_len, "\r\n\r\n");
            if (header_end) {
                headers_len = (size_t)(header_end - received) + 4;
                if (http_header_value(received, headers_len,
                                      "Content-Length",
                                      content_length_value,
                                      sizeof(content_length_value)) == 0) {
                    size_t expected;

                    if (parse_decimal_size(content_length_value,
                                           APPRTC_HTTP_RESPONSE_CAPACITY -
                                           headers_len, &expected) != 0) {
                        ssl_transport_disconnect(&net);
                        goto done;
                    }
                    if (expected <= received_len - headers_len) {
                        break;
                    }
                }
            }
            continue;
        }
        if (bytes == 0 || bytes == MBEDTLS_ERR_SSL_TIMEOUT) {
            break;
        }
        ssl_transport_disconnect(&net);
        goto done;
    }
    ssl_transport_disconnect(&net);
    received[received_len] = '\0';

    header_end = find_bytes(received, received_len, "\r\n\r\n");
    if (!header_end || parse_http_status(received, received_len,
                                         &response->status) != 0) {
        goto done;
    }
    headers_len = (size_t)(header_end - received) + 4;
    raw_body_len = received_len - headers_len;

    if (http_header_value(received, headers_len, "Transfer-Encoding",
                          transfer_encoding,
                          sizeof(transfer_encoding)) == 0) {
        if (!http_value_has_token(transfer_encoding, "chunked") ||
            http_header_value(received, headers_len, "Content-Length",
                              content_length_value,
                              sizeof(content_length_value)) == 0) {
            goto done;
        }
        if (decode_chunked_body(received + headers_len, raw_body_len,
                                &response->body, &response->body_len) != 0) {
            goto done;
        }
    } else {
        if (http_header_value(received, headers_len, "Content-Length",
                              content_length_value,
                              sizeof(content_length_value)) == 0) {
            size_t expected;

            if (parse_decimal_size(content_length_value,
                                   APPRTC_HTTP_RESPONSE_CAPACITY -
                                   headers_len, &expected) != 0 ||
                expected != raw_body_len) {
                goto done;
            }
        }
        response->body = malloc(raw_body_len + 1);
        if (!response->body) {
            goto done;
        }
        memcpy(response->body, received + headers_len, raw_body_len);
        response->body[raw_body_len] = '\0';
        response->body_len = raw_body_len;
    }

    ret = response->status >= 200 && response->status < 300 ? 0 : -1;

done:
    free(request);
    free(received);
    if (ret != 0 && response->status != 0) {
        LOG_E("HTTP %s failed: status=%d\r\n", method, response->status);
    }
    if (ret != 0) {
        http_response_free(response);
    }
    return ret;
}

static int ws_send_frame(apprtc_signaling_t *signaling, uint8_t opcode,
                         const uint8_t *payload, size_t payload_len)
{
    uint8_t mask[4];
    uint8_t *frame;
    size_t header_len;
    size_t frame_len;

    if (!signaling || !signaling->ws_connected ||
        payload_len > UINT16_MAX ||
        ((opcode & 0x08) != 0 && payload_len > 125) ||
        mbedtls_ctr_drbg_random(&signaling->ws_net.ctr_drbg,
                                mask, sizeof(mask)) != 0) {
        return -1;
    }

    header_len = payload_len < 126 ? 6 : 8;
    frame_len = header_len + payload_len;
    frame = malloc(frame_len);
    if (!frame) {
        return -1;
    }

    frame[0] = 0x80 | (opcode & 0x0f);
    if (payload_len < 126) {
        frame[1] = 0x80 | (uint8_t)payload_len;
        memcpy(frame + 2, mask, sizeof(mask));
    } else {
        frame[1] = 0x80 | 126;
        frame[2] = (uint8_t)(payload_len >> 8);
        frame[3] = (uint8_t)payload_len;
        memcpy(frame + 4, mask, sizeof(mask));
    }
    for (size_t i = 0; i < payload_len; ++i) {
        frame[header_len + i] = payload[i] ^ mask[i % sizeof(mask)];
    }

    int ret = tls_send_all(&signaling->ws_net, frame, frame_len);
    free(frame);
    return ret;
}

static int ws_send_text(apprtc_signaling_t *signaling, const char *text)
{
    return text ? ws_send_frame(signaling, 0x1,
                                (const uint8_t *)text,
                                strlen(text)) : -1;
}

static void dispatch_json_message(apprtc_signaling_t *signaling,
                                  const char *text, size_t length)
{
    cJSON *outer = NULL;
    cJSON *inner = NULL;
    cJSON *message;
    cJSON *wrapped;
    cJSON *type;
    cJSON *data = NULL;

    if (!signaling || !text || length == 0) {
        return;
    }

    outer = cJSON_ParseWithLength(text, length);
    if (!outer) {
        LOG_W("Ignoring invalid AppRTC JSON message\r\n");
        return;
    }
    message = outer;
    wrapped = cJSON_GetObjectItemCaseSensitive(outer, "msg");
    if (cJSON_IsString(wrapped) && wrapped->valuestring) {
        inner = cJSON_Parse(wrapped->valuestring);
        if (!inner) {
            goto done;
        }
        message = inner;
    }

    type = cJSON_GetObjectItemCaseSensitive(message, "type");
    if (!cJSON_IsString(type) || !type->valuestring) {
        goto done;
    }
    if (strcmp(type->valuestring, "offer") == 0) {
        data = cJSON_GetObjectItemCaseSensitive(message, "sdp");
        if (cJSON_IsString(data) && signaling->handler) {
            signaling->handler(APPRTC_MESSAGE_OFFER, data->valuestring,
                               signaling->handler_user_data);
        }
    } else if (strcmp(type->valuestring, "answer") == 0) {
        data = cJSON_GetObjectItemCaseSensitive(message, "sdp");
        if (cJSON_IsString(data) && signaling->handler) {
            signaling->handler(APPRTC_MESSAGE_ANSWER, data->valuestring,
                               signaling->handler_user_data);
        }
    } else if (strcmp(type->valuestring, "candidate") == 0) {
        data = cJSON_GetObjectItemCaseSensitive(message, "candidate");
        if (cJSON_IsString(data) && signaling->handler) {
            signaling->handler(APPRTC_MESSAGE_CANDIDATE, data->valuestring,
                               signaling->handler_user_data);
        }
    } else if (strcmp(type->valuestring, "bye") == 0 &&
               signaling->handler) {
        signaling->handler(APPRTC_MESSAGE_BYE, NULL,
                           signaling->handler_user_data);
    }

done:
    cJSON_Delete(inner);
    cJSON_Delete(outer);
}

static int ws_process_frames(apprtc_signaling_t *signaling)
{
    size_t consumed = 0;

    if (!signaling || !signaling->ws_rx ||
        signaling->ws_rx_len > APPRTC_WS_BUFFER_CAPACITY) {
        return -1;
    }

    while (signaling->ws_rx_len - consumed >= 2) {
        const uint8_t *frame = signaling->ws_rx + consumed;
        uint8_t opcode = frame[0] & 0x0f;
        int fin = (frame[0] & 0x80) != 0;
        int masked = (frame[1] & 0x80) != 0;
        uint8_t payload_len_code = frame[1] & 0x7f;
        uint64_t payload_len = frame[1] & 0x7f;
        size_t header_len = 2;
        const uint8_t *payload;
        int control = (opcode & 0x08) != 0;

        if ((frame[0] & 0x70) != 0 || masked ||
            (control && (!fin || payload_len_code > 125))) {
            LOG_E("Server sent an invalid WebSocket frame\r\n");
            return -1;
        }
        if (payload_len == 126) {
            if (signaling->ws_rx_len - consumed < 4) {
                break;
            }
            payload_len = ((uint16_t)frame[2] << 8) | frame[3];
            header_len = 4;
            if (payload_len < 126) {
                return -1;
            }
        } else if (payload_len == 127) {
            if (signaling->ws_rx_len - consumed < 10) {
                break;
            }
            payload_len = 0;
            for (int i = 0; i < 8; ++i) {
                payload_len = (payload_len << 8) | frame[2 + i];
            }
            header_len = 10;
            if ((frame[2] & 0x80) != 0 || payload_len <= UINT16_MAX) {
                return -1;
            }
        }
        if (payload_len > APPRTC_WS_BUFFER_CAPACITY ||
            payload_len > signaling->ws_rx_len - consumed - header_len) {
            if (payload_len > APPRTC_WS_BUFFER_CAPACITY) {
                return -1;
            }
            break;
        }
        payload = frame + header_len;

        if (opcode == 0x8) {
            if (payload_len == 1) {
                return -1;
            }
            return -1;
        } else if (opcode == 0x9) {
            if (payload_len > 125 ||
                ws_send_frame(signaling, 0xA, payload,
                              (size_t)payload_len) != 0) {
                return -1;
            }
        } else if (opcode == 0xA) {
            /* Pong. */
        } else if (opcode == 0x1) {
            if (signaling->ws_fragment_active) {
                return -1;
            }
            if (fin) {
                dispatch_json_message(signaling, (const char *)payload,
                                      (size_t)payload_len);
            } else {
                memcpy(signaling->ws_fragment, payload,
                       (size_t)payload_len);
                signaling->ws_fragment_len = (size_t)payload_len;
                signaling->ws_fragment_active = 1;
            }
        } else if (opcode == 0x0) {
            if (!signaling->ws_fragment_active ||
                payload_len > APPRTC_WS_BUFFER_CAPACITY -
                              signaling->ws_fragment_len) {
                return -1;
            }
            memcpy(signaling->ws_fragment + signaling->ws_fragment_len,
                   payload, (size_t)payload_len);
            signaling->ws_fragment_len += (size_t)payload_len;
            if (fin) {
                dispatch_json_message(signaling,
                                      (const char *)signaling->ws_fragment,
                                      signaling->ws_fragment_len);
                signaling->ws_fragment_len = 0;
                signaling->ws_fragment_active = 0;
            }
        } else {
            return -1;
        }

        consumed += header_len + (size_t)payload_len;
    }

    if (consumed > 0) {
        memmove(signaling->ws_rx, signaling->ws_rx + consumed,
                signaling->ws_rx_len - consumed);
        signaling->ws_rx_len -= consumed;
    }
    if (signaling->ws_rx_len == APPRTC_WS_BUFFER_CAPACITY) {
        return -1;
    }
    return 0;
}

static int ws_handshake(apprtc_signaling_t *signaling,
                        const url_parts_t *parts)
{
    uint8_t random_key[16];
    uint8_t accept_digest[20];
    char client_key[32];
    char expected_accept[40];
    char server_accept[64];
    char upgrade[32];
    char connection[128];
    char request[1024];
    uint8_t response[2048];
    size_t response_len = 0;
    const uint8_t *header_end;
    size_t headers_len;
    int request_len;
    int client_key_len;
    int expected_len;
    char accept_source[96];

    if (mbedtls_ctr_drbg_random(&signaling->ws_net.ctr_drbg,
                                random_key, sizeof(random_key)) != 0) {
        return -1;
    }
    client_key_len = libpeer_base64_encode(random_key, sizeof(random_key),
                                           client_key,
                                           sizeof(client_key));
    if (client_key_len <= 0) {
        return -1;
    }
    request_len = snprintf(request, sizeof(request),
                           "GET %s HTTP/1.1\r\n"
                           "Host: %s:%u\r\n"
                           "Upgrade: websocket\r\n"
                           "Connection: Upgrade\r\n"
                           "Sec-WebSocket-Key: %s\r\n"
                           "Sec-WebSocket-Version: 13\r\n"
                           "Origin: %s\r\n"
                           "User-Agent: webrtc-peer/1.0\r\n\r\n",
                           parts->path, parts->host, parts->port,
                           client_key, signaling->base_url);
    if (request_len <= 0 || (size_t)request_len >= sizeof(request) ||
        tls_send_all(&signaling->ws_net, (const uint8_t *)request,
                     (size_t)request_len) != 0) {
        return -1;
    }

    while (response_len < sizeof(response)) {
        int bytes = ssl_transport_recv(&signaling->ws_net,
                                       response + response_len,
                                       sizeof(response) - response_len);
        if (bytes <= 0) {
            return -1;
        }
        response_len += (size_t)bytes;
        header_end = find_bytes(response, response_len, "\r\n\r\n");
        if (header_end) {
            break;
        }
    }
    header_end = find_bytes(response, response_len, "\r\n\r\n");
    if (!header_end) {
        return -1;
    }
    headers_len = (size_t)(header_end - response) + 4;
    if (parse_http_status(response, headers_len, &request_len) != 0 ||
        request_len != 101 ||
        http_header_value(response, headers_len, "Upgrade",
                          upgrade, sizeof(upgrade)) != 0 ||
        !http_value_has_token(upgrade, "websocket") ||
        http_header_value(response, headers_len, "Connection",
                          connection, sizeof(connection)) != 0 ||
        !http_value_has_token(connection, "upgrade") ||
        http_header_value(response, headers_len, "Sec-WebSocket-Accept",
                          server_accept, sizeof(server_accept)) != 0) {
        return -1;
    }

    if (snprintf(accept_source, sizeof(accept_source),
                 "%s258EAFA5-E914-47DA-95CA-C5AB0DC85B11",
                 client_key) >= (int)sizeof(accept_source) ||
        apprtc_sha1((const unsigned char *)accept_source,
                     strlen(accept_source), accept_digest) != 0) {
        return -1;
    }
    expected_len = libpeer_base64_encode(accept_digest,
                                         sizeof(accept_digest),
                                         expected_accept,
                                         sizeof(expected_accept));
    if (expected_len <= 0 || strcmp(server_accept, expected_accept) != 0) {
        LOG_E("WebSocket server accept key mismatch\r\n");
        return -1;
    }

    if (response_len > headers_len) {
        size_t extra = response_len - headers_len;
        if (extra > APPRTC_WS_BUFFER_CAPACITY) {
            return -1;
        }
        memcpy(signaling->ws_rx, response + headers_len, extra);
        signaling->ws_rx_len = extra;
    }
    return 0;
}

static int parse_join_response(apprtc_signaling_t *signaling,
                               const char *body)
{
    cJSON *root = NULL;
    cJSON *params;
    cJSON *result;
    cJSON *initiator;
    cJSON *messages;
    int ret = -1;

    root = cJSON_Parse(body);
    if (!root) {
        return -1;
    }
    result = cJSON_GetObjectItemCaseSensitive(root, "result");
    params = cJSON_GetObjectItemCaseSensitive(root, "params");
    if (!cJSON_IsString(result) ||
        strcmp(result->valuestring, "SUCCESS") != 0 ||
        !cJSON_IsObject(params) ||
        json_copy_string(params, "client_id",
                         signaling->client_id,
                         sizeof(signaling->client_id)) != 0 ||
        json_copy_string(params, "room_id",
                         signaling->room_id,
                         sizeof(signaling->room_id)) != 0 ||
        json_copy_string(params, "wss_url",
                         signaling->wss_url,
                         sizeof(signaling->wss_url)) != 0 ||
        json_copy_string(params, "wss_post_url",
                         signaling->wss_post_url,
                         sizeof(signaling->wss_post_url)) != 0 ||
        json_copy_string(params, "ice_server_url",
                         signaling->ice_server_url,
                         sizeof(signaling->ice_server_url)) != 0) {
        goto done;
    }

    initiator = cJSON_GetObjectItemCaseSensitive(params, "is_initiator");
    if (!cJSON_IsString(initiator) && !cJSON_IsBool(initiator)) {
        goto done;
    }
    signaling->is_initiator = cJSON_IsTrue(initiator) ||
        (cJSON_IsString(initiator) &&
         strcmp(initiator->valuestring, "true") == 0);

    messages = cJSON_GetObjectItemCaseSensitive(params, "messages");
    if (cJSON_IsArray(messages)) {
        cJSON *item = NULL;
        cJSON_ArrayForEach(item, messages) {
            if (signaling->initial_message_count >=
                APPRTC_INITIAL_MESSAGE_COUNT) {
                LOG_W("Ignoring excess initial AppRTC messages\r\n");
                break;
            }
            if (cJSON_IsString(item) && item->valuestring) {
                char *copy = strdup(item->valuestring);
                if (!copy) {
                    goto done;
                }
                signaling->initial_messages[
                    signaling->initial_message_count++] = copy;
            }
        }
    }
    ret = 0;

done:
    cJSON_Delete(root);
    return ret;
}

static int parse_ice_response(apprtc_signaling_t *signaling,
                              const char *body)
{
    cJSON *root = NULL;
    cJSON *servers;
    cJSON *server;
    cJSON *urls;
    cJSON *username;
    cJSON *credential;
    int ret = -1;

    root = cJSON_Parse(body);
    if (!root) {
        return -1;
    }
    servers = cJSON_GetObjectItemCaseSensitive(root, "iceServers");
    server = cJSON_IsArray(servers) ? cJSON_GetArrayItem(servers, 0) : NULL;
    urls = server ? cJSON_GetObjectItemCaseSensitive(server, "urls") : NULL;
    username = server ? cJSON_GetObjectItemCaseSensitive(server,
                                                          "username") : NULL;
    credential = server ? cJSON_GetObjectItemCaseSensitive(server,
                                                            "credential") : NULL;
    if (!cJSON_IsArray(urls) || !cJSON_IsString(username) ||
        !cJSON_IsString(credential) ||
        copy_string(signaling->ice_username,
                    sizeof(signaling->ice_username),
                    username->valuestring) != 0 ||
        copy_string(signaling->ice_credential,
                    sizeof(signaling->ice_credential),
                    credential->valuestring) != 0) {
        goto done;
    }

    cJSON *url = NULL;
    cJSON_ArrayForEach(url, urls) {
        if (!cJSON_IsString(url) || !url->valuestring) {
            continue;
        }
        if (strncmp(url->valuestring, "stun:", 5) == 0 &&
            signaling->stun_url[0] == '\0') {
            if (copy_string(signaling->stun_url,
                            sizeof(signaling->stun_url),
                            url->valuestring) != 0) {
                goto done;
            }
        } else if (strncmp(url->valuestring, "turn:", 5) == 0 &&
                   signaling->turn_url[0] == '\0') {
            if (copy_string(signaling->turn_url,
                            sizeof(signaling->turn_url),
                            url->valuestring) != 0) {
                goto done;
            }
        }
    }
    if (signaling->stun_url[0] == '\0') {
        goto done;
    }
    ret = 0;

done:
    cJSON_Delete(root);
    return ret;
}

apprtc_signaling_t *apprtc_signaling_join(const char *join_url)
{
    apprtc_signaling_t *signaling = NULL;
    http_response_t response;
    url_parts_t parts;

    if (!join_url || parse_url(join_url, &parts) != 0 ||
        strcmp(parts.scheme, "https") != 0) {
        return NULL;
    }

    signaling = calloc(1, sizeof(*signaling));
    if (!signaling ||
        copy_string(signaling->join_url, sizeof(signaling->join_url),
                    join_url) != 0 ||
        build_base_url(&parts, signaling->base_url,
                       sizeof(signaling->base_url)) != 0) {
        apprtc_signaling_destroy(signaling);
        return NULL;
    }
    signaling->ws_net.tcp_socket.fd = -1;

    if (http_request(join_url, "POST",
                     "application/json; charset=utf-8", NULL, "",
                     &response) != 0) {
        apprtc_signaling_destroy(signaling);
        return NULL;
    }
    if (parse_join_response(signaling, response.body) != 0) {
        http_response_free(&response);
        apprtc_signaling_destroy(signaling);
        return NULL;
    }
    http_response_free(&response);

    if (http_request(signaling->ice_server_url, "POST",
                     "application/json; charset=utf-8", NULL, "",
                     &response) != 0) {
        apprtc_signaling_leave(signaling);
        apprtc_signaling_destroy(signaling);
        return NULL;
    }
    if (parse_ice_response(signaling, response.body) != 0) {
        http_response_free(&response);
        apprtc_signaling_leave(signaling);
        apprtc_signaling_destroy(signaling);
        return NULL;
    }
    http_response_free(&response);

    LOG_I("Joined AppRTC room as %s\r\n",
          signaling->is_initiator ? "initiator" : "answerer");
    return signaling;
}

int apprtc_signaling_connect(apprtc_signaling_t *signaling,
                             apprtc_message_handler_t handler,
                             void *user_data)
{
    url_parts_t parts;
    cJSON *registration = NULL;
    char *payload = NULL;
    int ret = -1;

    if (!signaling || !handler || signaling->ws_connected ||
        signaling->ws_rx || signaling->ws_fragment ||
        parse_url(signaling->wss_url, &parts) != 0 ||
        strcmp(parts.scheme, "wss") != 0) {
        return -1;
    }

    signaling->handler = handler;
    signaling->handler_user_data = user_data;
    signaling->ws_rx = malloc(APPRTC_WS_BUFFER_CAPACITY);
    signaling->ws_fragment = malloc(APPRTC_WS_BUFFER_CAPACITY);
    if (!signaling->ws_rx || !signaling->ws_fragment) {
        goto done;
    }

    if (ssl_transport_connect(&signaling->ws_net, parts.host, parts.port,
                              digicert_global_root_g2_pem) != 0) {
        goto done;
    }
    if (ws_handshake(signaling, &parts) != 0) {
        ssl_transport_disconnect(&signaling->ws_net);
        goto done;
    }
    signaling->ws_connected = 1;
    mbedtls_ssl_conf_read_timeout(&signaling->ws_net.conf,
                                  APPRTC_WS_POLL_TIMEOUT_MS);

    registration = cJSON_CreateObject();
    if (!registration ||
        !cJSON_AddStringToObject(registration, "cmd", "register") ||
        !cJSON_AddStringToObject(registration, "roomid",
                                 signaling->room_id) ||
        !cJSON_AddStringToObject(registration, "clientid",
                                 signaling->client_id)) {
        goto done;
    }
    payload = cJSON_PrintUnformatted(registration);
    if (!payload || ws_send_text(signaling, payload) != 0) {
        goto done;
    }

    for (size_t i = 0; i < signaling->initial_message_count; ++i) {
        dispatch_json_message(signaling, signaling->initial_messages[i],
                              strlen(signaling->initial_messages[i]));
        free(signaling->initial_messages[i]);
        signaling->initial_messages[i] = NULL;
    }
    signaling->initial_message_count = 0;
    ret = ws_process_frames(signaling);

done:
    free(payload);
    cJSON_Delete(registration);
    if (ret != 0) {
        if (signaling && signaling->ws_connected) {
            signaling->ws_connected = 0;
            ssl_transport_disconnect(&signaling->ws_net);
        }
        free(signaling ? signaling->ws_rx : NULL);
        free(signaling ? signaling->ws_fragment : NULL);
        if (signaling) {
            signaling->ws_rx = NULL;
            signaling->ws_fragment = NULL;
        }
    }
    return ret;
}

int apprtc_signaling_poll(apprtc_signaling_t *signaling)
{
    int bytes;

    if (!signaling || !signaling->ws_connected) {
        return -1;
    }

    if (signaling->ws_rx_len >= APPRTC_WS_BUFFER_CAPACITY) {
        return -1;
    }

    bytes = ssl_transport_recv(&signaling->ws_net,
                               signaling->ws_rx + signaling->ws_rx_len,
                               APPRTC_WS_BUFFER_CAPACITY -
                               signaling->ws_rx_len);
    if (bytes > 0) {
        signaling->ws_rx_len += (size_t)bytes;
    } else if (bytes == 0) {
        return -1;
    } else if (bytes != MBEDTLS_ERR_SSL_TIMEOUT &&
               bytes != MBEDTLS_ERR_SSL_WANT_READ &&
               bytes != MBEDTLS_ERR_SSL_WANT_WRITE) {
        return -1;
    }
    return ws_process_frames(signaling);
}

int apprtc_signaling_send_sdp(apprtc_signaling_t *signaling,
                              int offer, const char *sdp)
{
    char url[APPRTC_URL_CAPACITY];
    cJSON *message = NULL;
    char *payload = NULL;
    http_response_t response;
    int length;
    int ret = -1;

    if (!signaling || !sdp) {
        return -1;
    }
    length = snprintf(url, sizeof(url), "%s/message/%s/%s",
                      signaling->base_url, signaling->room_id,
                      signaling->client_id);
    if (length <= 0 || (size_t)length >= sizeof(url)) {
        return -1;
    }

    message = cJSON_CreateObject();
    if (!message ||
        !cJSON_AddStringToObject(message, "type",
                                 offer ? "offer" : "answer") ||
        !cJSON_AddStringToObject(message, "sdp", sdp)) {
        goto done;
    }
    payload = cJSON_PrintUnformatted(message);
    if (!payload) {
        goto done;
    }
    ret = http_request(url, "POST", "application/json; charset=utf-8",
                       NULL, payload, &response);
    if (ret == 0) {
        http_response_free(&response);
        LOG_I("Sent %s SDP (%u bytes)\r\n",
              offer ? "offer" : "answer", (unsigned int)strlen(sdp));
    }

done:
    free(payload);
    cJSON_Delete(message);
    return ret;
}

void apprtc_signaling_leave(apprtc_signaling_t *signaling)
{
    char url[APPRTC_URL_CAPACITY];
    char referer[APPRTC_URL_CAPACITY];
    char *inner = NULL;
    char *outer = NULL;
    cJSON *inner_json = NULL;
    cJSON *outer_json = NULL;
    http_response_t response;

    if (!signaling) {
        return;
    }

    if (snprintf(url, sizeof(url), "%s/leave/%s/%s",
                 signaling->base_url, signaling->room_id,
                 signaling->client_id) < (int)sizeof(url) &&
        snprintf(referer, sizeof(referer), "Referer: %s/r/%s\r\n",
                 signaling->base_url, signaling->room_id) <
                 (int)sizeof(referer) &&
        http_request(url, "POST", "application/json; charset=utf-8",
                     referer, "", &response) == 0) {
        http_response_free(&response);
    }

    if (signaling->ws_connected) {
        inner_json = cJSON_CreateObject();
        outer_json = cJSON_CreateObject();
        if (inner_json && outer_json &&
            cJSON_AddStringToObject(inner_json, "type", "bye")) {
            inner = cJSON_PrintUnformatted(inner_json);
            if (inner &&
                cJSON_AddStringToObject(outer_json, "cmd", "send") &&
                cJSON_AddStringToObject(outer_json, "msg", inner)) {
                outer = cJSON_PrintUnformatted(outer_json);
                if (outer) {
                    ws_send_text(signaling, outer);
                    ports_sleep_ms(50);
                }
            }
        }
    }
    free(inner);
    free(outer);
    cJSON_Delete(inner_json);
    cJSON_Delete(outer_json);

    if (snprintf(url, sizeof(url), "%s/%s/%s",
                 signaling->wss_post_url, signaling->room_id,
                 signaling->client_id) < (int)sizeof(url) &&
        http_request(url, "DELETE", NULL, NULL, NULL, &response) == 0) {
        http_response_free(&response);
    }

    if (signaling->ws_connected) {
        uint8_t close_code[2] = { 0x03, 0xE8 };
        ws_send_frame(signaling, 0x8, close_code, sizeof(close_code));
        signaling->ws_connected = 0;
        ssl_transport_disconnect(&signaling->ws_net);
    }
}

void apprtc_signaling_destroy(apprtc_signaling_t *signaling)
{
    if (!signaling) {
        return;
    }
    if (signaling->ws_connected) {
        signaling->ws_connected = 0;
        ssl_transport_disconnect(&signaling->ws_net);
    }
    for (size_t i = 0; i < APPRTC_INITIAL_MESSAGE_COUNT; ++i) {
        free(signaling->initial_messages[i]);
    }
    free(signaling->ws_rx);
    free(signaling->ws_fragment);
    free(signaling);
}

int apprtc_signaling_is_initiator(const apprtc_signaling_t *signaling)
{
    return signaling ? signaling->is_initiator : 0;
}

const char *apprtc_signaling_room_id(const apprtc_signaling_t *signaling)
{
    return signaling ? signaling->room_id : NULL;
}

const char *apprtc_signaling_stun_url(const apprtc_signaling_t *signaling)
{
    return signaling && signaling->stun_url[0] ?
           signaling->stun_url : NULL;
}

const char *apprtc_signaling_turn_url(const apprtc_signaling_t *signaling)
{
    return signaling && signaling->turn_url[0] ?
           signaling->turn_url : NULL;
}

const char *apprtc_signaling_ice_username(const apprtc_signaling_t *signaling)
{
    return signaling && signaling->ice_username[0] ?
           signaling->ice_username : NULL;
}

const char *apprtc_signaling_ice_credential(const apprtc_signaling_t *signaling)
{
    return signaling && signaling->ice_credential[0] ?
           signaling->ice_credential : NULL;
}
