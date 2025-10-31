#ifndef LWIP_HTTPD_DEFINE_H
#define LWIP_HTTPD_DEFINE_H

#define VerifyOrExit(aCondition, ...) \
    do                                \
    {                                 \
        if (!(aCondition))            \
        {                             \
            __VA_ARGS__;              \
            goto exit;                \
        }                             \
    } while (false)

#define HTTP_CONTENT_LEN                                  "Content-Length: "
#define HTTP_CONTENT_TYPE(contenttype)                    "Content-Type: "contenttype""
#define HTTP_ACCEPT_TYPE(contenttype)                     "Accept: "contenttype""

#define HTTP_HDR_HTML           HTTP_CONTENT_TYPE("text/html")
#define HTTP_HDR_GIF            HTTP_CONTENT_TYPE("image/gif")
#define HTTP_HDR_PNG            HTTP_CONTENT_TYPE("image/png")
#define HTTP_HDR_JPG            HTTP_CONTENT_TYPE("image/jpeg")
#define HTTP_HDR_BMP            HTTP_CONTENT_TYPE("image/bmp")
#define HTTP_HDR_ICO            HTTP_CONTENT_TYPE("image/x-icon")
#define HTTP_HDR_APP            HTTP_CONTENT_TYPE("application/octet-stream")
#define HTTP_HDR_JS             HTTP_CONTENT_TYPE("application/javascript")
#define HTTP_HDR_RA             HTTP_CONTENT_TYPE("application/javascript")
#define HTTP_HDR_CSS            HTTP_CONTENT_TYPE("text/css")
#define HTTP_HDR_SWF            HTTP_CONTENT_TYPE("application/x-shockwave-flash")
#define HTTP_HDR_XML            HTTP_CONTENT_TYPE("text/xml")
#define HTTP_HDR_PDF            HTTP_CONTENT_TYPE("application/pdf")
#define HTTP_HDR_JSON           HTTP_CONTENT_TYPE("application/json")
#define HTTP_HDR_CSV            HTTP_CONTENT_TYPE("text/csv")
#define HTTP_HDR_TSV            HTTP_CONTENT_TYPE("text/tsv")
#define HTTP_HDR_SVG            HTTP_CONTENT_TYPE("image/svg+xml")
#define HTTP_HDR_SVGZ           HTTP_CONTENT_TYPE_ENCODING("image/svg+xml", "gzip")

#define HTTP_HDR_DEFAULT_TYPE   HTTP_CONTENT_TYPE("text/plain")

#define HTTP_HDR_ACCEPT_JSON    HTTP_ACCEPT_TYPE("application/json")
#define HTTP_HDR_ACCEPT_TXT     HTTP_ACCEPT_TYPE("text/plain")
#define HTTP_HDR_ACCEPT_ANY     HTTP_ACCEPT_TYPE("*/*")

#define OT_REST_RESOURCE_PATH_DIAGNOSTICS "/diagnostics"
#define OT_REST_RESOURCE_PATH_NODE "/node"
#define OT_REST_RESOURCE_PATH_NODE_BAID "/node/ba-id"
#define OT_REST_RESOURCE_PATH_NODE_RLOC "/node/rloc"
#define OT_REST_RESOURCE_PATH_NODE_RLOC16 "/node/rloc16"
#define OT_REST_RESOURCE_PATH_NODE_EXTADDRESS "/node/ext-address"
#define OT_REST_RESOURCE_PATH_NODE_STATE "/node/state"
#define OT_REST_RESOURCE_PATH_NODE_NETWORKNAME "/node/network-name"
#define OT_REST_RESOURCE_PATH_NODE_LEADERDATA "/node/leader-data"
#define OT_REST_RESOURCE_PATH_NODE_NUMOFROUTER "/node/num-of-router"
#define OT_REST_RESOURCE_PATH_NODE_EXTPANID "/node/ext-panid"
#define OT_REST_RESOURCE_PATH_NODE_DATASET_ACTIVE "/node/dataset/active"
#define OT_REST_RESOURCE_PATH_NODE_DATASET_PENDING "/node/dataset/pending"
#define OT_REST_RESOURCE_PATH_NETWORK "/networks"
#define OT_REST_RESOURCE_PATH_NETWORK_CURRENT "/networks/current"
#define OT_REST_RESOURCE_PATH_NETWORK_CURRENT_COMMISSION "/networks/commission"
#define OT_REST_RESOURCE_PATH_NETWORK_CURRENT_PREFIX "/networks/current/prefix"

typedef enum {
    http_method_type_get = 0,
    http_method_type_put,
    http_method_type_delete,
    http_method_type_unkown = -1,
} http_method_type_t;

typedef struct _http_method {
    const http_method_type_t id;
    const char * method;
    const size_t method_len;
} http_method_t;

typedef enum {
    http_accept_type_txt = 0,
    http_accept_type_json,
    http_accept_type_not_specified,
    http_accept_type_unsupport,
} http_accept_type_t;

typedef enum {
    http_resp_state_ok = 0, /** 200 */
    http_resp_state_created, /** 201*/
    http_resp_state_no_content, /** 204 */
    http_resp_state_bad_request, /** 400 */
    http_resp_state_forbidden, /** 403 */
    http_resp_state_not_found, /** 404 */
    http_resp_state_not_allowed, /** 405 */
    http_resp_state_request_timeout, /** 408 */
    http_resp_state_conflict, /** 409 */
    http_resp_state_internal_server_error, /** 500 */
    http_resp_state_not_implemented, /** 501 */

    http_resp_state_none = -1,
} http_resp_state_t;

typedef void (* http_continue_func_t)(void *connection);

bool openthread_rest_request(void *connection, http_method_type_t method, http_accept_type_t accept_type, char *uri, char *body, http_continue_func_t continue_func);
char * openthread_rest_construct_resp(http_resp_state_t resp_state, char * body);
err_t http_setup_file(void * connection, http_resp_state_t resp_state, char * body);
err_t http_init_error_file(void *hs, http_resp_state_t resp_state);

#endif /* LWIP_HTTPD_DEFINE_H */
