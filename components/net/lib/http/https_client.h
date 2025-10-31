/*
 * Copyright (c) 2025 Bouffalolab
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <lwip/api.h>
#include <lwip/arch.h>
#include <lwip/opt.h>
#include <lwip/inet.h>
#include <lwip/errno.h>
#include <netdb.h>
#include "http/client.h"
#include "https_wrapper.h"

#define HTTP_CLIENT_DEFAULT_RX_BUFFER_SIZE            (1024)

struct https_client_request {
    /* User should fill in following parameters */

    /** The HTTP method: GET, HEAD, OPTIONS, POST, ... */
    enum http_method method;

    /** User supplied callback function to call when response is
     * received.
     */
    http_response_cb_t response;

    /** User supplied list of HTTP callback functions if the
     * calling application wants to know the parsing status or the HTTP
     * fields. This is optional and normally not needed.
     */
    const struct http_parser_settings *http_cb;

    /** The URL for this request, for example: /index.html */
    const char *url;

    /** The HTTP protocol, for example "HTTP/1.1" */
    const char *protocol;

    /** The HTTP header fields (application specific)
     * The Content-Type may be specified here or in the next field.
     * Depending on your application, the Content-Type may vary, however
     * some header fields may remain constant through the application's
     * life cycle. This is a NULL terminated list of header fields.
     */
    const char **header_fields;

    /** The value of the Content-Type header field, may be NULL */
    const char *content_type_value;

    /** User supplied callback function to call when payload
     * needs to be sent. This can be NULL in which case the payload field
     * in http_request is used. The idea of this payload callback is to
     * allow user to send more data that is practical to store in allocated
     * memory.
     */
    http_payload_cb_t payload_cb;

    /** Payload, may be NULL */
    const char *payload;

    /** Payload length is used to calculate Content-Length. Set to 0
     * for chunked transfers.
     */
    size_t payload_len;

    /** User supplied callback function to call when optional headers need
     * to be sent. This can be NULL, in which case the optional_headers
     * field in http_request is used. The idea of this optional_headers
     * callback is to allow user to send more HTTP header data that is
     * practical to store in allocated memory.
     */
    http_header_cb_t optional_headers_cb;

    /** A NULL terminated list of any optional headers that
     * should be added to the HTTP request. May be NULL.
     * If the optional_headers_cb is specified, then this field is ignored.
     * Note that there are two similar fields that contain headers,
     * the header_fields above and this optional_headers. This is done
     * like this to support Websocket use case where Websocket will use
     * header_fields variable and any optional application specific
     * headers will be placed into this field.
     */
    const char **optional_headers;

    const char *ca_pem;             /*!< SSL server CA certification, PEM format as string, if the client requires to verify server */
    size_t      ca_len;
    const char *client_cert_pem;    /*!< SSL client certification, PEM format as string, if the server requires to verify client */
    size_t      client_cert_len;
    const char *client_key_pem;     /*!< SSL client key, PEM format as string, if the server requires to verify client */
    size_t      client_key_len;
    size_t      buffer_size;         /*!< HTTP buffer size (both send and receive) */
};

int https_client_request(const struct https_client_request *request, uint32_t timeout, void *user_data);


