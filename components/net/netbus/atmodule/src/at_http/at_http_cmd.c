/**
  ******************************************************************************
  * @file    at_http_cmd.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "at_main.h"
#include "at_core.h"
#include "at_httpc_main.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <stream_buffer.h>

#define AT_HTTPC_HANDLE_MAX (3)
#define AT_HTTPC_DEFAULT_TIMEOUT (5000)
#define AT_HTTPC_RECVBUF_SIZE_DEFAULT (15*1024)

//#define AT_HTTPC_RECVBUF_CNT_MAX (2 * MAC_RXQ_DEPTH)
#define AT_HTTPC_RECVBUF_CNT_MAX (DEFAULT_TCP_RECVMBOX_SIZE)

#define AT_MSG_PEEK 0x01
#define AT_HTTPC_RECV_MODE_ACTIVE  0
#define AT_HTTPC_RECV_MODE_PASSIVE 1

#define AT_HTTP_EVT_HEAD(s)      (g_https_cfg.recv_mode == AT_HTTPC_RECV_MODE_PASSIVE)?s"\r\n":s","
#define AT_HTTP_LOCK(lock)       xSemaphoreTake(lock, portMAX_DELAY)
#define AT_HTTP_UNLOCK(lock)     xSemaphoreGive(lock)

struct at_http_ctx {
    httpc_connection_t settings;
    uint8_t *data;
    struct altcp_pcb *altcp_conn;
    QueueHandle_t recv_mbox;
    SemaphoreHandle_t mutex;
    struct pbuf *lastbuf;
    uint8_t used;
#define AT_HTTPS_NOT_AUTH        0
#define AT_HTTPS_SERVER_AUTH     1
#define AT_HTTPS_CLIENT_AUTH     2
#define AT_HTTPS_BOTH_AUTH       3
    uint8_t https_auth_type;
    uint8_t recv_mode;
    uint8_t linkid;
    char ca_file[32];
    char cert_file[32];
    char key_file[32];

    int http_tcp_wnd;
    char *url;
    uint32_t url_size;
    uint32_t recv_avail;
};

struct at_https_global_config {
    uint8_t recv_mode;
    uint32_t recvbuf_size;
};

static struct at_http_ctx g_httpc_handle[AT_HTTPC_HANDLE_MAX];

static struct at_https_global_config g_https_cfg = {0};

static const char *at_resp_string[] = {
    "",
    "+HTTPCHEAD",
    "+HTTPCGET",
    "+HTTPCPOST",
    "+HTTPCPUT",
};

static inline void free_ctx(struct at_http_ctx *ctx)
{
    ctx->used = 0;
}

static int httpc_get_recvcnt(struct at_http_ctx *ctx)
{
    if (g_https_cfg.recv_mode == AT_HTTPC_RECV_MODE_ACTIVE) {
        return 0;
    }

    return uxQueueMessagesWaiting(ctx->recv_mbox);
}

static int httpc_get_recvsize(int linkid)
{
    int size = 0;
    struct at_http_ctx *ctx = &g_httpc_handle[linkid];

    AT_HTTP_LOCK(ctx->mutex);
    size = ctx->recv_avail;
    if (ctx->lastbuf) {
    	size += ctx->lastbuf->tot_len;
    }
    AT_HTTP_UNLOCK(ctx->mutex);
    return size;
}

static inline uint8_t httpc_recvbuf_overflow(struct at_http_ctx *ctx)
{
    if (httpc_get_recvcnt(ctx) >= AT_HTTPC_RECVBUF_CNT_MAX || httpc_get_recvsize(ctx->linkid) >= g_https_cfg.recvbuf_size) {
        return 1;
    }
    return 0;
}

static int httpc_buffer_write(struct at_http_ctx *ctx, struct pbuf *p)
{
    int ret = 0;

    AT_HTTP_LOCK(ctx->mutex);
    ret = xQueueSend(ctx->recv_mbox, &p, 0);
    if (ret == pdTRUE) {
    	ctx->recv_avail += p->tot_len;
    }
    AT_HTTP_UNLOCK(ctx->mutex);

    if (ret != pdTRUE) {
        /* It will never run to here. */
        at_write("+HTTPCLOST:%d,%d\r\n", ctx->linkid, p->tot_len);
        ret = -1;
    }

    return ret;
}

static ssize_t httpc_buffer_read(struct at_http_ctx *ctx, void *mem, int len, int flags)
{
    ssize_t recvd = 0;
    ssize_t recv_left = (len <= SSIZE_MAX) ? (ssize_t)len : SSIZE_MAX;

    AT_HTTP_LOCK(ctx->mutex);
    do {
      struct pbuf *p;
      int err = 0;
      u16_t copylen;

      /* Check if there is data left from the last recv operation. */
      if (ctx->lastbuf) {
        p = ctx->lastbuf;
      } else {
        /* No data was left from the previous operation, so we try to get
           some from the network. */

    	err = xQueueReceive(ctx->recv_mbox, &p, 0);
        if (err == pdTRUE && p != NULL) {
    	    ctx->recv_avail -= p->tot_len;
        }

        if (err != pdTRUE) {
          if (recvd > 0) {
            /* already received data, return that (this trusts in getting the same error from
               netconn layer again next time netconn_recv is called) */
            goto _recv_http_done;
          }
          AT_HTTP_UNLOCK(ctx->mutex);
          return 0;
        }

        ctx->lastbuf = p;
      }

      if (recv_left > p->tot_len) {
        copylen = p->tot_len;
      } else {
        copylen = (u16_t)recv_left;
      }
      if (recvd > SSIZE_MAX - copylen) {
        /* overflow */
        copylen = (u16_t)(SSIZE_MAX - recvd);
      }

      /* copy the contents of the received buffer into
      the supplied memory pointer mem */
      if (mem) {
        pbuf_copy_partial(p, (u8_t *)mem + recvd, copylen, 0);
      }

      recvd += copylen;

      /* TCP combines multiple pbufs for one recv */
      recv_left -= copylen;

      /* Unless we peek the incoming message... */
      if ((flags & AT_MSG_PEEK) == 0) {
        /* ... check if there is data left in the pbuf */
        if (p->tot_len - copylen > 0) {
          /* If so, it should be saved in the sock structure for the next recv call.
             We store the pbuf but hide/free the consumed data: */
          ctx->lastbuf = pbuf_free_header(p, copylen);
        } else {
          ctx->lastbuf = NULL;
          pbuf_free(p);
        }
      }
    } while ((recv_left > 0) && !(flags & AT_MSG_PEEK));

_recv_http_done:
	AT_HTTP_UNLOCK(ctx->mutex);
    if (recvd > 0 && ctx->altcp_conn) {
        ctx->http_tcp_wnd += recvd;
        if (ctx->http_tcp_wnd >= MAC_RXQ_DEPTH * TCP_MSS) {
            altcp_recved(ctx->altcp_conn, recvd);
            ctx->http_tcp_wnd -= recvd;
        }
    }
    return recvd;
}

static void httpc_buffer_clear(int linkid)
{
    int len;

    len = httpc_get_recvsize(linkid);
    if (len > 0) {
        httpc_buffer_read(&g_httpc_handle[linkid], NULL, len, 0);
    }
}

static err_t cb_altcp_recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
    struct at_http_ctx *ctx = (struct at_http_ctx *)arg;

    if (g_https_cfg.recv_mode == AT_HTTPC_RECV_MODE_ACTIVE) {
        if (p->tot_len) {
            AT_CMD_DATA_SEND(p->payload, p->tot_len);
        }
        altcp_recved(conn, p->tot_len);
        pbuf_free(p);
    } else {
        ctx->altcp_conn = conn;
        httpc_buffer_write(ctx, p);
    }
    
    return 0;
}

static void cb_httpc_result(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{
    struct at_http_ctx *ctx = (struct at_http_ctx *)arg;

    printf("[DATA] Transfer finished err %d. rx_content_len is %lu httpc_result: %d\r\n", err, rx_content_len, httpc_result);
    
    if ((err == 0 && httpc_result == HTTPC_RESULT_OK) || 
        (ctx->settings.req_type == REQ_TYPE_HEAD && httpc_result == HTTPC_RESULT_ERR_CONTENT_LEN)) {
        at_response_string("\r\n+HTTPSTATUS:%d,0\r\n", ctx->linkid);
    } else {
        at_response_string("\r\n+HTTPSTATUS:%d,%d\r\n", ctx->linkid, httpc_result);
    }
    free(ctx->data);
    ctx->data = NULL;
#if LWIP_ALTCP_TLS && LWIP_ALTCP_TLS_MBEDTLS 
    if (ctx->settings.tls_config) {
        altcp_tls_free_config(ctx->settings.tls_config);
    }
#endif
    ctx->altcp_conn = NULL;
    free_ctx(ctx);
}

static err_t cb_httpc_headers_done_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    struct at_http_ctx *ctx = (struct at_http_ctx *)arg;

    printf("[HEAD] hdr->tot_len is %u, hdr_len is %u, content_len is %lu\r\n", hdr->tot_len, hdr_len, content_len);

    //printf((char *)hdr->payload);

    if (ctx->settings.req_type == REQ_TYPE_HEAD) {
        if (hdr->tot_len) {
            at_write("%s:%d,%d,", at_resp_string[ctx->settings.req_type], ctx->linkid, hdr_len);
            AT_CMD_DATA_SEND(hdr->payload, hdr->tot_len);
        }
    } else {
        at_write(AT_HTTP_EVT_HEAD("%s:%d,%d"), 
                 (g_https_cfg.recv_mode == AT_HTTPC_RECV_MODE_PASSIVE)?"+HTTPC":at_resp_string[ctx->settings.req_type], 
                 ctx->linkid, content_len);
    }
    return ERR_OK;
}

static int at_httpc_request(struct at_http_ctx *ctx, 
                            const char *url_buf,
                            httpc_result_fn result_fn, 
                            httpc_headers_done_fn headers_done_fn,
                            altcp_recv_fn recv_fn,
                            void *parg)
{
	int ret;
    char *param, *host_name, *url;
    httpc_state_t *req = NULL;
    ip_addr_t ip_addr;
    char *p_port;
    int port;

    if (httpc_recvbuf_overflow(ctx) && (ctx->settings.req_type != REQ_TYPE_HEAD)) {
        printf("HTTPC not enough RX buffer\r\n");
        free_ctx(ctx);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_RESOURCE);
    }

#if LWIP_ALTCP_TLS && LWIP_ALTCP_TLS_MBEDTLS
    ctx->settings.tls_config = NULL;
#endif
    ctx->settings.result_fn = result_fn;
    ctx->settings.headers_done_fn = headers_done_fn;

    ctx->http_tcp_wnd = 0;
    if (0 == strncmp(url_buf, "http://", 7)) {
        port = 80;
        url = url_buf + 7;
    } else if (0 == strncmp(url_buf, "https://", 8)) {
        port = 443;
        url = url_buf + 8;
#if LWIP_ALTCP_TLS && LWIP_ALTCP_TLS_MBEDTLS 
        uint8_t *ca_buf = NULL, *cert_buf = NULL, *privkey_buf = NULL;
        uint32_t ca_len, cert_len, privkey_len;

        if (ctx->https_auth_type == AT_HTTPS_NOT_AUTH) {

            ctx->settings.tls_config = altcp_tls_create_config_client(NULL, 0);

        } else if (ctx->https_auth_type == AT_HTTPS_CLIENT_AUTH) {

            at_load_file(ctx->cert_file, &cert_buf, &cert_len);
            at_load_file(ctx->key_file, &privkey_buf, &privkey_len);

            ctx->settings.tls_config = altcp_tls_create_config_client_2wayauth(NULL, 0,
                                                                               privkey_buf, privkey_len,
                                                                               NULL, 0,
                                                                               cert_buf, cert_len);

            free(cert_buf);
            free(privkey_buf);
        } else if (ctx->https_auth_type == AT_HTTPS_SERVER_AUTH) {

            at_load_file(ctx->ca_file, &ca_buf, &ca_len);
            ctx->settings.tls_config = altcp_tls_create_config_client(ca_buf, ca_len);
            free(ca_buf);

        } else if (ctx->https_auth_type == AT_HTTPS_BOTH_AUTH) {

            at_load_file(ctx->cert_file, &cert_buf, &cert_len);
            at_load_file(ctx->key_file, &privkey_buf, &privkey_len);
            at_load_file(ctx->ca_file, &ca_buf, &ca_len);

            ctx->settings.tls_config = altcp_tls_create_config_client_2wayauth(ca_buf, ca_len,
                                                                               privkey_buf, privkey_len,
                                                                               NULL, 0,
                                                                               cert_buf, cert_len);
            free(cert_buf);
            free(privkey_buf);
            free(ca_buf);
        }

#endif
    } else {
        free_ctx(ctx);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
    }

    if ((param = strstr(url, "/")) == NULL) {
        param = url + strlen(url);
    }

    host_name = strdup(url);
    host_name[param - url] = '\0';
    
    if ((p_port = strstr(host_name, ":")) != NULL) {
        *p_port = '\0';
        p_port++;
        port = atoi(p_port);
    }

    if (ipaddr_aton(host_name, &ip_addr)) {
        printf("host_name:%s port:%d uri:%s\r\n", ipaddr_ntoa(&ip_addr), port, param);
        ret = at_httpc_get_file((const ip_addr_t* )&ip_addr,
                        port,
                        param,
                        &ctx->settings,
                        recv_fn,
                        (void *)ctx,
                        &req);
    } else {
        printf("host_name:%s port:%d uri:%s\r\n", host_name, port, param);
        ret = at_httpc_get_file_dns(
                host_name,
                port,
                param,
                &ctx->settings,
                recv_fn,
                (void *)ctx,
                &req);
    }

    free(host_name);
    if (ret != ERR_OK) {
    	free_ctx(ctx);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_httpsslcfg(int argc, const char **argv)
{
    int linkid, scheme;
    char cert_file[32] = {0};
    char key_file[32] = {0};
    char ca_file[32] = {0};
    struct at_http_ctx *ctx = NULL;
    int cert_file_valid = 0, key_file_valid = 0, ca_file_valid = 0;

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_NUMBER(1, &scheme);
    AT_CMD_PARSE_OPT_STRING(2, cert_file, sizeof(cert_file), cert_file_valid);
    AT_CMD_PARSE_OPT_STRING(3, key_file, sizeof(key_file), key_file_valid);
    AT_CMD_PARSE_OPT_STRING(4, ca_file, sizeof(ca_file), ca_file_valid);
 
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    
    ctx = &g_httpc_handle[linkid];
    if (ctx->used) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
    }

    if (scheme == AT_HTTPS_NOT_AUTH) {
        cert_file[0] = '\0';
        key_file[0] = '\0';
        ca_file[0] = '\0';
    } else if (scheme == AT_HTTPS_SERVER_AUTH) {
        if ((!ca_file_valid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        cert_file[0] = '\0';
        key_file[0] = '\0';
    } else if (scheme == AT_HTTPS_CLIENT_AUTH) {
        if ((!key_file_valid) || (!cert_file_valid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        ca_file[0] = '\0';
    } else if (scheme == AT_HTTPS_BOTH_AUTH) {
        if ((!key_file_valid) || (!cert_file_valid) || (!ca_file_valid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    ctx->https_auth_type = scheme;

    strlcpy(ctx->cert_file, cert_file, sizeof(ctx->cert_file));
    strlcpy(ctx->key_file, key_file, sizeof(ctx->key_file));
    strlcpy(ctx->ca_file, ca_file, sizeof(ctx->ca_file));

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_httpsslcfg(int argc, const char **argv)
{
    int linkid;
    struct at_http_ctx *ctx = NULL;

    AT_CMD_PARSE_NUMBER(0, &linkid);
 
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    
    ctx = &g_httpc_handle[linkid];

    at_response_string("+HTTPSSLCFG:%d,%d,\"%s\",\"%s\",\"%s\"\r\n",linkid, 
            ctx->https_auth_type, ctx->cert_file, ctx->key_file, ctx->ca_file);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_httpclient(int argc, const char **argv)
{
    int opt, content_type, linkid;
    struct at_http_ctx *ctx = NULL;
    uint8_t data_valid = 0;
    char url_buf[256];
    char *data = malloc(256);
   
    if (!data) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_NUMBER(1, &opt);
    AT_CMD_PARSE_NUMBER(2, &content_type);
    AT_CMD_PARSE_STRING(3, url_buf, sizeof(url_buf));
    AT_CMD_PARSE_OPT_STRING(4, data, 256, data_valid);
 
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (opt < 0 || opt > REQ_TYPE_PUT) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (content_type < CONTENT_TYPE_WWW || content_type > CONTENT_TYPE_TEXT) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    ctx = &g_httpc_handle[linkid];
    //memset(ctx, 0, sizeof(struct at_http_ctx));

    if (ctx->used) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
    }
    ctx->used = 1;

    ctx->data = data;
    
    if (strlen(url_buf) == 0) {
        if ((ctx->url == NULL) && (ctx->url_size == 0)) {
            free(ctx->data);
            free_ctx(ctx);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        strlcpy(url_buf, ctx->url, sizeof(url_buf));
    }   
    ctx->settings.timeout = AT_HTTPC_DEFAULT_TIMEOUT;
    ctx->settings.use_proxy = 0;
    ctx->settings.req_type  = opt;
    ctx->settings.content_type = content_type;
    ctx->settings.data = ctx->data;
    ctx->linkid = linkid;

    int ret = at_httpc_request(ctx, url_buf, cb_httpc_result, cb_httpc_headers_done_fn, cb_altcp_recv_fn, ctx);
    if (ret != 0) {
        free(data);
        ctx->data = NULL;
        return ret;
    }

    return AT_RESULT_CODE_OK;
}

static err_t cb_httpgetsize_headers_done_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    struct at_http_ctx *ctx = (struct at_http_ctx *)arg;

    at_response_string("+HTTPGETSIZE:%d,%d\r\n", ctx->linkid, content_len);

    return ERR_OK;
}

static err_t cb_httpgetsize_recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
    altcp_recved(conn, p->tot_len);
    pbuf_free(p);
    return 0;
}

static int at_setup_cmd_httpgetsize(int argc, const char **argv)
{
    int linkid, timeout, timeout_valild = 0;
    struct at_http_ctx *ctx = NULL;
    char url_buf[256];
 
    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_STRING(1, url_buf, sizeof(url_buf));
    AT_CMD_PARSE_OPT_NUMBER(2, &timeout, timeout_valild);
 
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
 
    ctx = &g_httpc_handle[linkid];
    //memset(ctx, 0, sizeof(struct at_http_ctx));

    if (ctx->used) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
    }
    ctx->used = 1;

    if (timeout_valild) {
        if (timeout < 0 || timeout > 180000) {
            free_ctx(ctx);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
    } else {
        timeout = AT_HTTPC_DEFAULT_TIMEOUT;
    }

    if (strlen(url_buf) == 0) {
        if ((ctx->url == NULL) && (ctx->url_size == 0)) {
            free_ctx(ctx);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        strlcpy(url_buf, ctx->url, sizeof(url_buf));
    }

    ctx->data = NULL;
    ctx->settings.timeout = timeout;
    ctx->settings.use_proxy = 0;
    ctx->settings.req_type  = REQ_TYPE_HEAD;
    ctx->settings.content_type = 0;
    ctx->settings.data = NULL;

    int ret = at_httpc_request(ctx, url_buf, cb_httpc_result, cb_httpgetsize_headers_done_fn, cb_httpgetsize_recv_fn, ctx);
    if (ret != 0) {
        return ret;
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_httpcget(int argc, const char **argv)
{
    int linkid, timeout, timeout_valild = 0;
    struct at_http_ctx *ctx = NULL;
    char url_buf[256];
    
    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_STRING(1, url_buf, sizeof(url_buf));
    AT_CMD_PARSE_OPT_NUMBER(2, &timeout, timeout_valild);
 
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
 
    ctx = &g_httpc_handle[linkid];
    //memset(ctx, 0, sizeof(struct at_http_ctx));

    if (ctx->used) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
    }
    ctx->used = 1;

    if (timeout_valild) {
        if (timeout < 0 || timeout > 180000) {
            free_ctx(ctx);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
    } else {
        timeout = AT_HTTPC_DEFAULT_TIMEOUT;
    }

    if (strlen(url_buf) == 0) {
        if ((ctx->url == NULL) && (ctx->url_size == 0)) {
            free_ctx(ctx);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        strlcpy(url_buf, ctx->url, sizeof(url_buf));
    }
    
    ctx->data = NULL;
    ctx->settings.timeout = timeout;
    ctx->settings.use_proxy = 0;
    ctx->settings.req_type  = REQ_TYPE_GET;
    ctx->settings.content_type = 0;
    ctx->settings.data = NULL;

    int ret = at_httpc_request(ctx, url_buf, cb_httpc_result, cb_httpc_headers_done_fn, cb_altcp_recv_fn, ctx);
    if (ret != 0) {
        return ret;
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_httpcpost(int argc, const char **argv)
{
    int ret, linkid;
    int len, recv_num = 0;
    struct at_http_ctx *ctx = NULL;
    char url_buf[256];
 
    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_STRING(1, url_buf, sizeof(url_buf));
    AT_CMD_PARSE_NUMBER(2, &len);
    
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
 
    ctx = &g_httpc_handle[linkid];
    //memset(ctx, 0, sizeof(struct at_http_ctx));

    if (ctx->used) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
    }
    ctx->used = 1;

    if (strlen(url_buf) == 0) {
        if ((ctx->url == NULL) && (ctx->url_size == 0)) {
            free_ctx(ctx);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        strlcpy(url_buf, ctx->url, sizeof(url_buf));
    }   

    ctx->data = malloc(len + 1);
    if (!ctx->data) {
        free_ctx(ctx);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    memset(ctx->data, 0, len + 1);

    ctx->settings.timeout = AT_HTTPC_DEFAULT_TIMEOUT;
    ctx->settings.use_proxy = 0;
    ctx->settings.req_type  = REQ_TYPE_POST;
    ctx->settings.content_type = 0;
    ctx->settings.data = ctx->data;

    at_response_result(AT_RESULT_CODE_OK);
    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);
    while(recv_num < len) {
        recv_num += AT_CMD_DATA_RECV(ctx->data + recv_num, len - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    if (len == recv_num) {
        ret = AT_RESULT_CODE_SEND_OK;
    } else {
        ret = AT_RESULT_CODE_SEND_FAIL;
    }
    
    ret = at_httpc_request(ctx, url_buf, cb_httpc_result, cb_httpc_headers_done_fn, cb_altcp_recv_fn, ctx);
    if (ret != 0) {
        free(ctx->data);
        ctx->data = NULL;
        return ret;
    }

    return AT_RESULT_CODE_IGNORE;
}

static int at_setup_cmd_httpcput(int argc, const char **argv)
{
    int ret, linkid;
    int len, content_type, recv_num = 0;
    struct at_http_ctx *ctx = NULL;
    char url_buf[256];
   
    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_STRING(1, url_buf, sizeof(url_buf));
    AT_CMD_PARSE_NUMBER(2, &content_type);
    AT_CMD_PARSE_NUMBER(3, &len);
  
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (content_type < CONTENT_TYPE_WWW || content_type > CONTENT_TYPE_TEXT) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    ctx = &g_httpc_handle[linkid];
    if (ctx->used) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
    }

    ctx->used = 1;
    //memset(ctx, 0, sizeof(struct at_http_ctx));

    if (strlen(url_buf) == 0) {
        if ((ctx->url == NULL) && (ctx->url_size == 0)) {
            free_ctx(ctx);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        strlcpy(url_buf, ctx->url, sizeof(url_buf));
    }   

    ctx->data = malloc(len + 1);
    if (!ctx->data) {
        free_ctx(ctx);
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    memset(ctx->data, 0, len + 1);

    ctx->settings.timeout = AT_HTTPC_DEFAULT_TIMEOUT;
    ctx->settings.use_proxy = 0;
    ctx->settings.req_type  = REQ_TYPE_PUT;
    ctx->settings.content_type = content_type;
    ctx->settings.data = ctx->data;

    at_response_result(AT_RESULT_CODE_OK);
    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);
    while(recv_num < len) {
        recv_num += AT_CMD_DATA_RECV(ctx->data + recv_num, len - recv_num);
    }

    at_response_string("Recv %d bytes\r\n", recv_num);

    if (len == recv_num) {
        ret = AT_RESULT_CODE_SEND_OK;
    } else {
        ret = AT_RESULT_CODE_SEND_FAIL;
    }
    
    ret = at_httpc_request(ctx, url_buf, cb_httpc_result, cb_httpc_headers_done_fn, cb_altcp_recv_fn, ctx);
    if (ret != 0) {
        free(ctx->data);
        ctx->data = NULL;
        return ret;
    }

    return AT_RESULT_CODE_IGNORE;
}

static int at_setup_cmd_httpcurlcfg(int argc, const char **argv)
{
    int linkid;
    int len, recv_num = 0;
    struct at_http_ctx *ctx = NULL;

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_NUMBER(1, &len);
    if (len < 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
 
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    
    ctx = &g_httpc_handle[linkid];
    if (ctx->used) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
    }

    if (len == 0) {
        free(ctx->url);
        ctx->url_size = 0;
        ctx->url = NULL;
        return AT_RESULT_CODE_OK;
    }

    if (ctx->url != NULL) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    ctx->url = malloc(len + 1);
    if (!ctx->url) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }
    memset(ctx->url, 0, len + 1);

    at_response_result(AT_RESULT_CODE_OK);
    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);
    while(recv_num < len) {
        recv_num += AT_CMD_DATA_RECV(ctx->url + recv_num, len - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);
    ctx->url_size = len;

    ctx->url[len] = '\0';
    if (len == recv_num) {
        return AT_RESULT_CODE_SEND_OK;
    }
    return AT_RESULT_CODE_SEND_FAIL;
}

static int at_query_cmd_httpcurlcfg(int argc, const char **argv)
{
    int linkid;
    struct at_http_ctx *ctx = NULL;
    AT_CMD_PARSE_NUMBER(0, &linkid);
 
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    
    ctx = &g_httpc_handle[linkid];

    at_response_string("+HTTPURLCFG:%d,%d,%s\r\n", linkid, ctx->url_size, ctx->url);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_httprecvmode(int argc, const char **argv)
{
    at_response_string("+HTTPRECVMODE:%d\r\n", g_https_cfg.recv_mode);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_httprecvmode(int argc, const char **argv)
{
    int mode, id;

    AT_CMD_PARSE_NUMBER(0, &mode);
    if(mode != AT_HTTPC_RECV_MODE_ACTIVE && mode != AT_HTTPC_RECV_MODE_PASSIVE) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }
   
    if (at_get_work_mode() != AT_WORK_MODE_CMD) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }
 
    if (g_https_cfg.recv_mode == mode) {
        return AT_RESULT_CODE_OK;
    }

    for (id = 0; id < AT_HTTPC_HANDLE_MAX; id++) {
        if (g_httpc_handle[id].used) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_PROCESSING);
        }
    }

    for (id = 0; id < AT_HTTPC_HANDLE_MAX; id++) {

        httpc_buffer_clear(id);
        AT_HTTP_LOCK(g_httpc_handle[id].mutex);

        if (g_httpc_handle[id].recv_mbox) {
            vQueueDelete(g_httpc_handle[id].recv_mbox);
            g_httpc_handle[id].recv_mbox = NULL;
        }
        g_httpc_handle[id].recv_avail = 0;
        if (mode == AT_HTTPC_RECV_MODE_PASSIVE) {
            g_httpc_handle[id].recv_mbox = xQueueCreate(AT_HTTPC_RECVBUF_CNT_MAX, sizeof(void *));
        }
    
        AT_HTTP_UNLOCK(g_httpc_handle[id].mutex);
    }

    g_https_cfg.recv_mode = mode;

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_httprecvdata(int argc, const char **argv)
{
    int linkid;
    int read_len, size, ret = 0, n, offset = 0;
    uint8_t *buffer;
    struct at_http_ctx *ctx = NULL;

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_NUMBER(1, &size);
 
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    ctx = &g_httpc_handle[linkid];

    if (size <= 0 || size > g_https_cfg.recvbuf_size) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    buffer = (char *)pvPortMalloc(size + 48);

    read_len = httpc_get_recvsize(linkid);
    read_len = read_len > size ? size : read_len;

    n = snprintf(buffer + offset, 48, "+HTTPRECVDATA:%d,", read_len);
    if (n > 0) {
        offset += n;
    }

    if (read_len) {
        ret = httpc_buffer_read(ctx, buffer + offset, read_len, 0);
        if (ret != read_len) {
            printf("at_net_recvbuf_read error %d\r\n", ret);
        }
    }
    
    offset += ret;
   
    memcpy(buffer + offset, "\r\n", 2);
    offset += 2;
    AT_CMD_DATA_SEND((uint8_t *)buffer, offset);
    
    vPortFree(buffer);

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_httprecvbuf(int argc, const char **argv)
{
    int size;
        
    AT_CMD_PARSE_NUMBER(0, &size);
 
    if (size <= 0 || size > at_lwip_heap_free_size()) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    g_https_cfg.recvbuf_size = size;

    return AT_RESULT_CODE_OK;
}


static int at_query_cmd_httprecvbuf(int argc, const char **argv)
{
    at_response_string("+HTTPRECVBUF:%d\r\n", g_https_cfg.recvbuf_size);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_httprecvlen(int argc, const char **argv)
{
    int linkid = 0;
    AT_CMD_PARSE_NUMBER(0, &linkid);
 
    if (linkid < 0 || linkid >= AT_HTTPC_HANDLE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    at_response_string("+HTTPRECVLEN:%d,%d\r\n",linkid, httpc_get_recvsize(linkid));
    return AT_RESULT_CODE_OK;
}

static const at_cmd_struct at_http_cmd[] = {
    {"+HTTPRECVDATA", NULL, NULL, at_setup_cmd_httprecvdata, NULL, 2, 2},
    {"+HTTPRECVMODE", NULL, at_query_cmd_httprecvmode, at_setup_cmd_httprecvmode, NULL, 1, 1},
    {"+HTTPRECVBUF", NULL, at_query_cmd_httprecvbuf, at_setup_cmd_httprecvbuf, NULL, 1, 1},
    {"+HTTPRECVLEN", NULL, at_query_cmd_httprecvlen, NULL, NULL, 1, 1},
    {"+HTTPCLIENT", NULL, NULL, at_setup_cmd_httpclient, NULL, 4, 5},
    {"+HTTPGETSIZE", NULL, NULL, at_setup_cmd_httpgetsize, NULL, 2, 3},
    {"+HTTPCGET", NULL, NULL, at_setup_cmd_httpcget, NULL, 2, 3},
    {"+HTTPCPOST", NULL, NULL, at_setup_cmd_httpcpost, NULL, 3, 3},
    {"+HTTPCPUT", NULL, NULL, at_setup_cmd_httpcput, NULL, 4, 4},
    {"+HTTPURLCFG", NULL, at_query_cmd_httpcurlcfg, at_setup_cmd_httpcurlcfg, NULL, 1, 2},
    {"+HTTPSSLCFG", NULL, at_query_cmd_httpsslcfg, at_setup_cmd_httpsslcfg, NULL, 2, 5},
};

bool at_http_cmd_regist(void)
{
    memset(&g_httpc_handle, 0, sizeof(g_httpc_handle));

    g_https_cfg.recvbuf_size = AT_HTTPC_RECVBUF_SIZE_DEFAULT;
    for (int i = 0; i < AT_HTTPC_HANDLE_MAX; i++) {
        g_httpc_handle[i].mutex = xSemaphoreCreateMutex();
    }

    if (at_cmd_register(at_http_cmd, sizeof(at_http_cmd) / sizeof(at_http_cmd[0])) == 0)
        return true;
    else
        return false;
}

