//#include <http_client.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <oai_config.h>
#include <bl616_sys.h>
#include <bl_https.h>
#if 1
void oai_http_request(char *offer, char *answer)
{
    const char *server = "api.openai.com";
    uint16_t port = 443;

    char request_buffer[4096];
    int request_len = snprintf(request_buffer, sizeof(request_buffer),
                               "POST /v1/realtime?model=gpt-4o-mini-realtime-preview-2024-12-17 HTTP/1.1\r\n"
                               "Host: api.openai.com\r\n"
                               "Content-Type: application/sdp\r\n"
                               "Authorization: Bearer %s\r\n"
                               "Content-Length: %zu\r\n"
                               "\r\n"
                               "%s",
                               openai_api_key, strlen(offer), offer);

    if (request_len >= sizeof(request_buffer)) {
        printf("Request buffer is too small!\r\n");
        return;
    }
    printf("\r\n====request:%s\r\n\r\n", request_buffer);
    uint8_t response_buffer[MAX_HTTP_OUTPUT_BUFFER];
    int response_len = MAX_HTTP_OUTPUT_BUFFER;

    int status_code = https_request(server, port, (const uint8_t *)request_buffer, request_len, response_buffer, &response_len);

    if (status_code != 201) {
        printf("Error perform http request, status code: %d\r\n", status_code);
        bl_sys_reset_por();
    } else {
        memcpy(answer, response_buffer, response_len);
        answer[response_len] = '\0';
        printf("\r\nanswer:%s\r\n\r\n", answer);
    }
}
#else
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

extern int bl_sys_reset_por(void);

uint8_t oai_http_event_handler(http_client_event_t *evt)
{
    static int output_len;
    switch (evt->event_id) {
        case HTTP_EVENT_REDIRECT:
            printf("HTTP_EVENT_REDIRECT\r\n");
            http_client_set_header(evt->client, "From", "user@example.com");
            http_client_set_header(evt->client, "Accept", "text/html");
            http_client_set_redirection(evt->client);
            break;
        case HTTP_EVENT_ERROR:
            printf("HTTP_EVENT_ERROR\r\n");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            printf("HTTP_EVENT_ON_CONNECTED\r\n");
            break;
        case HTTP_EVENT_HEADER_SENT:
            printf("HTTP_EVENT_HEADER_SENT\r\n");
            break;
        case HTTP_EVENT_ON_HEADER:
            printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s\r\n",
               evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA: {
            printf("HTTP_EVENT_ON_DATA, len=%d\r\n", evt->data_len);
            if (http_client_is_chunked_response(evt->client)) {
                printf("Chunked HTTP response not supported\r\n");
                //bl_sys_reset_por();
            }

            if (output_len == 0 && evt->user_data) {
                memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
            }

            // If user_data buffer is configured, copy the response into the buffer
            int copy_len = 0;
            if (evt->user_data) {
            // The last byte in evt->user_data is kept for the NULL character in
            // case of out-of-bound access.
                copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                if (copy_len) {
                    memcpy(((char *)evt->user_data) + output_len, evt->data, copy_len);
                }
            }
            output_len += copy_len;

            break;
        }
        case HTTP_EVENT_ON_FINISH:
            printf("HTTP_EVENT_ON_FINISH\r\n");
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            printf("HTTP_EVENT_DISCONNECTED\r\n");
            output_len = 0;
            break;
    }
    return 0;
}

void oai_http_request(char *offer, char *answer)
{
    http_client_config_t config;
    memset(&config, 0, sizeof(http_client_config_t));

    config.url = OPENAI_REALTIMEAPI;
    config.event_handler = oai_http_event_handler;
    config.user_data = answer;

    snprintf(answer, MAX_HTTP_OUTPUT_BUFFER, "Bearer %s", OPENAI_API_KEY);

    http_client_handle_t client = http_client_init(&config);
    http_client_set_method(client, HTTP_METHOD_POST);
    http_client_set_header(client, "Content-Type", "application/sdp");
    http_client_set_header(client, "Authorization", answer);
    http_client_set_post_field(client, offer, strlen(offer));

    http_errors_t err = http_client_perform(client);
    if (err != HTTP_CLI_OK || http_client_get_status_code(client) != 201) {
        printf("Error perform http request %d\r\n", err);
        bl_sys_reset_por();
    }

    http_client_cleanup(client);
}
#endif
